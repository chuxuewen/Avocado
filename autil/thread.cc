
/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, louis.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

#include "thread.h"
#include "sys.h"
#if av_android
#include "autil/os/android-jni.h"
#endif
#include <uv.h>

av_ns(avocado)

static Map<ThreadID, RunLoop*>        all_loop;
static Mutex                          all_loop_mutex;
static Map<ThreadID, Thread*>         all_threads;
static Mutex                          all_threads_mutex;

template<> uint Compare<ThreadID>::hash(const ThreadID& key) {
  ThreadID* p = const_cast<ThreadID*>(&key);
  size_t* t = reinterpret_cast<size_t*>(p);
  return (*t) % Uint::max;
}
template<> bool Compare<ThreadID>::equals(const ThreadID& a,
                     const ThreadID& b, uint ha, uint hb) {
  return a == b;
}

// --------------------- Thread ---------------------

av_def_inl_members(Thread, Inl) {
public:
  static ThreadID fork(ThreadExec exec, uint gid, cString& name) {
    Thread* thread = new Thread();
    
    std::thread std_thread([exec](Thread* t) {
    #if av_android
      JNI::ScopeENV scope;
    #endif
      Handle<Thread> h(t);
      if ( !t->m_abort ) {
        exec(*t);
      }
      { ScopeLock scope(all_threads_mutex);
        ScopeLock lock(t->m_mutex);
        all_threads.del(t->m_id);
        for ( auto& i : t->m_wait_end ) { // notice external wait
          ScopeLock scope(i.value()->mutex);
          i.value()->cond.notify_one();
        }
      }
    }, thread);
    
    thread->m_gid = gid;
    thread->m_id = std_thread.get_id();
    thread->m_name = name;
    
    { ScopeLock lock(all_threads_mutex);
      all_threads[thread->m_id] = thread;
    }
    std_thread.detach();
    
    return thread->m_id;
  }
  
  static void abort_group(uint gid) {
    ScopeLock scope(all_threads_mutex);
    for ( auto& i : all_threads ) {
      if ( i.value()->m_gid == gid ) {
        ScopeLock lock(i.value()->m_mutex);
        i.value()->m_abort = true;
        i.value()->m_cond.notify_one(); // awaken sleep status
      }
    }
  }
  
  static void awaken_group(uint gid) {
    ScopeLock scope(all_threads_mutex);
    for ( auto& i : all_threads ) {
      if (i.value()->m_gid == gid) {
        ScopeLock lock(i.value()->m_mutex);
        i.value()->m_cond.notify_one(); // awaken sleep status
      }
    }
  }
};

ThreadID Thread::fork(ThreadExec exec, cString& name) {
  return Thread::Inl::fork(exec, 0, name);
}

void Thread::abort(ThreadID id, bool wait_end) {
  Lock lock(all_threads_mutex);
  auto i = all_threads.find(id);
  if ( !i.is_null() ) {
    Thread* t = i.value();
    t->m_mutex.lock();
    t->m_abort = true;
    t->m_cond.notify_one(); // awaken sleep status
    
    if ( wait_end ) {
      lock.unlock();
      // Add wait to Thread obj
      WaitEnd wait; t->m_wait_end.push(&wait);
      Lock lock(wait.mutex);
      t->m_mutex.unlock();
      wait.cond.wait(lock); // wait
    } else {
      t->m_mutex.unlock();
    }
  }
}

void Thread::wait_end(ThreadID id) {
  Lock lock(all_threads_mutex);
  auto i = all_threads.find(id);
  if ( !i.is_null() ) {
    Thread* t = i.value();
    t->m_mutex.lock(); //
    // Add wait to Thread obj
    lock.unlock();
    WaitEnd wait; t->m_wait_end.push(&wait);
    Lock lock(wait.mutex);
    t->m_mutex.unlock();
    wait.cond.wait(lock); // wait
  }
}

/**
 * @func sleep_cur current thread
 */
void Thread::sleep_cur(uint64 timeUs) {
  std::this_thread::sleep_for(std::chrono::microseconds(timeUs));
}

/**
 * @func current_thread_id
 */
ThreadID Thread::current_id() {
  return std::this_thread::get_id();
}

static ThreadID _main_thread_id(Thread::current_id());

/**
 * @func current_thread_id
 */
ThreadID Thread::main_thread_id() {
  return _main_thread_id;
}

/**
 * @func awaken
 */
void Thread::awaken(ThreadID id) {
  ScopeLock lock(all_threads_mutex);
  auto i = all_threads.find(id);
  if ( !i.is_null() ) {
    ScopeLock scope(i.value()->m_mutex);
    i.value()->m_cond.notify_one(); // awaken sleep status
  }
}

/**
 * @func sleep
 */
void Thread::sleep(uint64 timeUs) {
  Lock lock(m_mutex);
  if ( current_id() == m_id && !m_abort ) {
    if (timeUs == 0) {
      m_cond.wait(lock); // wait
    } else {
      m_cond.wait_for(lock, std::chrono::microseconds(timeUs));
    }
  }
}

// --------------------- ThreadRunLoop ---------------------

class RunLoop::Inl: public RunLoop {
public:
#define _inl(self) static_cast<RunLoop::Inl*>(self)
  
  void loop_exec() {

    while(true) {

      int64 r = exec_queue();

      if (r < 0) { // 小于0结束
        break; // exit
      } else {
        if (r) { // 等待,直到收到新消息或超时
          std::unique_lock<Mutex> ul(_action_lock);
          _cond.wait_for(ul, std::chrono::microseconds(r));
        }
      }
    }
  }
  
  bool run() {

    { ScopeLock lock(_action_lock);

      ThreadID id = std::this_thread::get_id();
      RunLoop* l = get_run_loop(id);

      if (l) { // 当前正在运行,避免无限递归
        av_err("Current thread message queue already running!\n");
        return false;
      }

      { ScopeLock lock(all_loop_mutex);
        all_loop.set(id, this);
      }

      _stop_signal = false;
      _id = id;
      _is_alive = true;
    }

    av_trigger(start);

    exec: loop_exec();
    
    { Lock ul(_action_lock);
      
      exec2:
      
      if ( _exec.length() ) goto exec;
      
      if ( _stop_signal ) {
        // 不再执行任何消息回调, 直到代理被安全释放
        while ( _ref_count ) {
          _cond.wait(ul); // 永远等待,直到收到新消息
        }
      } else {

        // 最后往队列中发送消息
        
        if ( _timeout_us >= 0 || _ref_count ) {
          if ( _timeout_us == 0 || _ref_count ) {
            _cond.wait(ul); // 永远等待,直到收到新消息
          } else {
            _cond.wait_for(ul, std::chrono::microseconds(_timeout_us)); // 等待,直到收到新消息或超时
          }
          goto exec2;
        }
      }
    }

    av_trigger(end);
    
    List<ExecWrap> _tmp_clear;
    
    // 真的结束了
    { //
      ScopeLock lock2(all_loop_mutex);
      ScopeLock lock(_action_lock);
      _tmp_clear = move(_exec);
      _stop_signal = false;
      _ref_count = 0;
      _is_alive = false;
      all_loop.del(_id);
    }
    _tmp_clear.clear();
    
    return true;
  }
  
  int64 exec_queue() {

    av_trigger(loop);

    List<ExecWrap> exec;
    
    { ScopeLock lock(_action_lock);
      if ( ! _exec.length() ) {
        return -1;
      }
      exec = avocado::move(_exec);
    }

    auto now = std::chrono::steady_clock::now().time_since_epoch();

    for (auto i = exec.begin(), e = exec.end(); i != e; ) {
      auto t = i; i++;
      if (now >= t.value().du) { //
        SimpleEvent data = { nullptr, nullptr, 0 };
        // not try exception
        t.value().exec->call(data);
        exec.del(t);
      }
    }

    { ScopeLock lock(_action_lock);
      _exec.unshift( move(exec) );

      if (_exec.length()) {

        auto now = std::chrono::steady_clock::now().time_since_epoch();
        auto min = std::chrono::steady_clock::duration::max();

        for ( auto i = _exec.begin(), e = _exec.end(); i != e; i++ ) {

          auto du = i.value().du - now;

          if ( du.count() <= 0 ) {
            return 0;
          } else {
            if (du < min) {
              min = du;
            }
          }
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(min).count();
      } else {
        return -1;
      }
    }
  }
  
  /**
   * @func sync # 同步线程
   */
  uint post(Callback exec, uint group) {
    if ( !_stop_signal ) { // 如果已收到停止信号不接收任何
      ScopeLock lock(_action_lock);
      uint id = iid32();
      _exec.push({ id, group, std::chrono::steady_clock::duration(0), avocado::move(exec) });
      _cond.notify_one(); // 通知继续
      return id;
    }
    return 0;
  }
  
  /**
   * @func sync # 延时
   */
  uint post(Callback exec, uint group, uint64 delay_us) {
    if ( !_stop_signal ) { // 如果已收到停止信号不接收任何
      ScopeLock lock(_action_lock);
      std::chrono::steady_clock::duration du =
      (std::chrono::steady_clock::now() + std::chrono::microseconds(delay_us)).time_since_epoch();
      uint id = iid32();
      _exec.push({ id, group, du, avocado::move(exec) });
      _cond.notify_one(); // 通知继续
      return id;
    }
    return 0;
  }
  
  void abort_group(uint group) {
    ScopeLock lock(_action_lock);
    for (auto i = _exec.begin(), e = _exec.end(); i != e; ) {
      auto j = i++;
      if (j.value().group == group) {
        _exec.del(j);
      }
    }
    _cond.notify_one(); // 通知继续
  }
  
  void activate() {
    _cond.notify_one(); // 通知继续
  }
  
};

uint LoopProxy::post(Callback exec) {
  return _inl(m_loop)->post(exec, m_group);
}

uint LoopProxy::post(Callback exec, uint64 delay_us) {
  return _inl(m_loop)->post(exec, m_group, delay_us);
}

uint LoopProxy::post_message(Callback cb, uint64 delay_us) {
  if ( delay_us == 0 ) {
    return _inl(m_loop)->post(cb, m_group);
  } else {
    return _inl(m_loop)->post(cb, m_group, delay_us);
  }
}

void LoopProxy::abort_all() {
  _inl(m_loop)->abort_group(m_group); // abort all
}

LoopProxy::~LoopProxy() {
  _inl(m_loop)->abort_group(m_group); // abort all
  // 减少一个引用计数
  ScopeLock lock(m_loop->_action_lock);
  if (m_loop->_ref_count > 0) {
    m_loop->_ref_count--;
  }
  if (m_loop->_ref_count == 0) { // 可以结束了
    _inl(m_loop)->activate(); // 激活循环状态,不再等待
  }
}

/**
 * @constructor
 */
RunLoop::RunLoop()
: av_init_event(start)
, av_init_event(loop)
, av_init_event(end)
, _timeout_us(0)
, _stop_signal(false)
, _ref_count(0)
, _is_alive(false)
{

}

/**
 * @destructor
 */
RunLoop::~RunLoop() {
  {
    std::lock_guard<Mutex> lock(_action_lock);
    _stop_signal = true;   // stop signal
    for (auto i = _attach_obj.begin(),
                 e = _attach_obj.end(); i != e; i++) {
      Release(i.value());
    }
    _attach_obj.clear();
  }
  {
    // 安全的删除,必须确保已经退出运行循环。如果在循环内部调用删除,会锁死线程
    std::lock_guard<Mutex> lock2(_run_lock);
  }
}

/**
 * @func is_alive
 */
bool RunLoop::is_alive() {
  std::lock_guard<Mutex> lock(_action_lock);
  return _is_alive;
}

/**
 * @func attach
 */
Object* RunLoop::attach(int id) {
  std::lock_guard<Mutex> lock(_action_lock);
  auto i = _attach_obj.find(id);
  if (i != _attach_obj.end()) {
    return i.value();
  }
  return NULL;
}

/**
 * @func set_attach
 */
void RunLoop::set_attach(int id, Object* obj) {
  std::lock_guard<Mutex> lock(_action_lock);
  auto i = _attach_obj.find(id);
  if (i != _attach_obj.end()) {
    return Release(i.value());
  }
  _attach_obj.set(id, obj);
}

/**
 * @func sync # 同步线程
 */
uint RunLoop::post(Callback cb) {
  return _inl(this)->post(cb, 0);
}

/**
 * @func sync # 延时
 */
uint RunLoop::post(Callback cb, uint64 delay_us) {
  return _inl(this)->post(cb, 0, delay_us);
}

/**
 * @overwrite
 */
uint RunLoop::post_message(Callback cb, uint64 delay_us) {
  if ( delay_us == 0 ) {
    return _inl(this)->post(cb, 0);
  } else {
    return _inl(this)->post(cb, 0, delay_us);
  }
}

/**
 * @func abort # 中止同步
 */
void RunLoop::abort(uint id) {
  std::lock_guard<Mutex> lock(_action_lock);
  for (auto i = _exec.begin(), e = _exec.end(); i != e; i++) {
    if (i.value().id == id) {
      _exec.del(i); break;
    }
  }
  _cond.notify_one(); // 通知继续
}

/**
 * @func run_loop # 运行消息循环
 */
bool RunLoop::run_loop(int64 timeout_us) {
  std::lock_guard<Mutex> lock(_run_lock);
  _timeout_us = timeout_us;
  return _inl(this)->run();
}

/**
 * @func stop_signal
 */
void RunLoop::stop_signal() {
  {
    std::lock_guard<Mutex> lock(_action_lock);
    if ( _stop_signal ) {
      return;
    }
    _stop_signal = true;
  }
  _inl(this)->activate(); // 激活循环状态,不再等待
}

/**
 * 保持活动状态,并返回一个代理,只要不删除返回的代理对像,消息队列会一直保持活跃状态
 * 当前必须为活跃状态否则返回NULL
 * @func keep_alive
 */
LoopProxy* RunLoop::keep_alive() {
  ScopeLock lock(_action_lock);
  if (_is_alive) { // 还活着
    _ref_count++;  // 增加一个引用计数
    LoopProxy* rv = new LoopProxy();
    rv->m_loop = this;
    return rv;
  }
  return nullptr;
}

/**
 * @func current # 获取当前线程消息队列,如果没有返回NULL
 */
RunLoop* RunLoop::current() {
  return get_run_loop(std::this_thread::get_id());
}

/**
 * @func keep_alive_current 保持当前循环活跃并返回代理
 */
LoopProxy* RunLoop::keep_alive_current() {
  RunLoop* loop = current();
  if ( loop ) {
    return loop->keep_alive();
  }
  return nullptr;
}

/**
 * @func get_run_loop # 通过线程获取
 * @ret {ThreadRunLoop*}
 */
RunLoop* RunLoop::get_run_loop(ThreadID id) {
  ScopeLock lock(all_loop_mutex);
  auto it = all_loop.find(id);
  if (it != all_loop.end()) {
    return it.value();
  }
  return nullptr;
}

struct LoopWrap {
  
  LoopWrap(cString& name): _name(name), _loop(nullptr) { }
  
  RunLoop* loop() {
    
    ScopeLock lock(_lock);
    if ( ! _loop ) {
      _loop = new RunLoop();
      
      Thread::fork([this](Thread& t) {
      loop:
        _loop->run_loop(20000 * 1000); // 使用20超时,20秒后没有新消息结束线程
        
        { // end run, delete queue
          ScopeLock lock(_lock);
          /* 趁着循环运行结束到上面这句lock片刻时间拿到队列对像的线程,这里是最后的200毫秒,
           * 200毫秒后没有向队列发送新消息结束线程
           * * *
           * 这里休眠200毫秒给外部线程足够时间往队列发送消息
           */
          t.sleep_cur(200 * 1000);
          
          if ( _loop->is_alive() ) {
            goto loop; // 继续运行
          }
          
          Release(_loop);
          _loop = nullptr;
        }
      }, _name);
    }
    return _loop;
  }

  String         _name;
  RunLoop*       _loop;
  Mutex          _lock;
};

struct IOLoop {
private:
  
  uv_loop_t*      _uv_loop_ptr;
  bool            _uv_init;
  ThreadID        _id;
  uv_loop_t       _uv_loop;
  List<Callback>  _frame_loop_cb;
  Mutex           _mutex;
  
  static int uv_frame_loop_cb(uv_loop_t* loop) {
    IOLoop* self = (IOLoop*)loop->data;
    
    if ( self->_frame_loop_cb.length() ) {
      
      List<Callback> tmp;
      { //
        ScopeLock scope(self->_mutex);
        tmp = move(self->_frame_loop_cb);
      }
      for ( auto& i : tmp ) {
        SimpleEvent evt = { 0,0,0 };
        i.value()->call(evt);
      }
    }
    
    return 16;
  }
  
  void run_uv_loop() {
    if ( !_uv_init ) {
      if ( uv_loop_init(&_uv_loop) == 0 ) {
        _uv_init = true;
      } else {
        printf("no init uv loop\n"); abort();
      }
    }
    _uv_loop.data = this;
    _uv_loop_ptr = &_uv_loop;
    _id = Thread::fork([this](Thread& t) { // run loop
    loop:
      int keep_idle = 1250;
      uv_run2(_uv_loop_ptr, UV_RUN_DEFAULT, uv_frame_loop_cb);
      
      while ( keep_idle-- ) {
        t.sleep_cur(16 * 1000); // 16ms
        if ( uv_loop_alive(_uv_loop_ptr) ) {
          goto loop;
        } else if ( _frame_loop_cb.length() ) {
          keep_idle = 1250;
          uv_frame_loop_cb(_uv_loop_ptr);
        }
      }
      
      { // end
        ScopeLock lock(_mutex);
        t.sleep_cur(200 * 1000); // 200 ms
#if DEBUG
        printf("io loop end\n");
#endif
        if ( uv_loop_alive(_uv_loop_ptr) ) {
          goto loop;
        }
        _uv_loop_ptr = nullptr;
      }
    }, "default_uv_loop");
  }
  
public:
  
  IOLoop(): _uv_loop_ptr(nullptr), _uv_init(false) { }
  
  uv_loop_t* get_uv_loop() {
    ScopeLock lock(_mutex);
    if ( !_uv_loop_ptr ) {
      run_uv_loop();
    }
    return _uv_loop_ptr;
  }
  
  bool is_current_thread() {
    return Thread::current_id() == _id;
  }
  
  void post(Callback& cb, bool fast) {
    if ( fast && is_current_thread() ) {
      SimpleEvent evt = { 0,0,0 }; cb->call(evt);
    } else {
      ScopeLock scope(_mutex);
      if ( !_uv_loop_ptr ) {
        run_uv_loop();
      }
      _frame_loop_cb.push(cb);
    }
  }
  
};

static LoopWrap* _utils_loop(new LoopWrap("default_utils"));
static IOLoop*   _io_loop(new IOLoop);

/**
 * @func utils
 */
RunLoop* RunLoop::utils() {
  return _utils_loop->loop();
}

/**
 * @func next_tick
 */
void RunLoop::next_tick(Callback cb) {
  RunLoop* loop = RunLoop::current();
  if ( loop ) {
    loop->post(cb);
  } else { // 没有消息队列 post to io loop
    post_io(cb);
    // av_throw(ERR_NOT_RUN_LOOP, "Unable to obtain thread io run loop");
  }
}

/**
 * @func is_io_thread
 */
bool Thread::is_io_thread() {
  return _io_loop->is_current_thread();
}

uv_loop_t* _uv_loop() {
  return _io_loop->get_uv_loop();
}

/**
 * @func post_io
 */
void RunLoop::post_io(Callback cb, bool fast) {
  _io_loop->post(cb, fast);
}

/**
 * @constructor
 */
ParallelWorking::ParallelWorking() av_def_err : m_proxy(nullptr), m_gid(iid32()) {
  RunLoop* loop = RunLoop::current();
  av_assert_err(loop, "Can not find current thread run loop.");
  m_proxy = loop->keep_alive();
}

/**
 * @destructor
 */
ParallelWorking::~ParallelWorking() {
  abort_fork();
  Release(m_proxy); m_proxy = nullptr;
}

/**
 * @func fork
 */
ThreadID ParallelWorking::fork(ThreadExec exec, cString& name) {
  return Thread::Inl::fork(exec, m_gid, name);
}

/**
 * @func post message to main thread
 */
uint ParallelWorking::post(Callback exec) {
  return m_proxy->post(exec);
}

/**
 * @func post
 */
uint ParallelWorking::post(Callback exec, uint64 delayUs) {
  return m_proxy->post(exec, delayUs);
}

/**
 * @func abort_fork
 */
void ParallelWorking::abort_fork(ThreadID id) {
  if ( id == ThreadID() ) {
    Thread::Inl::abort_group(m_gid);
  } else {
    Thread::abort(id);
  }
}

/**
 * @func abort
 */
void ParallelWorking::abort_post(uint id) {
  if ( id ) {
    m_proxy->abort(id);
  } else {
    m_proxy->abort_all();
  }
}

/**
 * @func awaken
 */
void ParallelWorking::awaken(ThreadID id) {
  if ( id == ThreadID() ) {
    Thread::Inl::awaken_group(m_gid);
  } else {
    Thread::awaken(id);
  }
}

av_end
