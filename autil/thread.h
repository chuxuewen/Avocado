/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, xuewen.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of xuewen.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL xuewen.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __avocado__thread__
#define __avocado__thread__

#include "util.h"
#include "list.h"
#include "map.h"
#include "event.h"
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "cb.h"

/**
 * @ns avocado
 */

av_ns(avocado)
av_ns(gui)

class GUIApplication;

av_end

typedef std::atomic_bool  AtomicBool;
typedef std::atomic_int   AtomicInt;
typedef std::thread::id   ThreadID;
typedef std::mutex Mutex;
typedef std::recursive_mutex RecursiveMutex;
typedef std::lock_guard<Mutex> ScopeLock;
typedef std::unique_lock<Mutex> Lock;
typedef std::condition_variable Condition;

template<> uint Compare<ThreadID>::hash(const ThreadID &key);
template<> bool Compare<ThreadID>::equals(const ThreadID& a, const ThreadID& b, uint ha, uint hb);

class LoopProxy;

/**
 * @class Thread
 */
class Thread: public Object {
public:

#define av_thread_lock(thread) \
  ScopeLock thread##_lock(thread.mutex()); if (!t.is_abort())

  typedef std::function<void(Thread& thread)> ThreadExec;
  
  /**
   * @func detach
   */
  static ThreadID fork(ThreadExec exec, cString& name);
  
  /**
   * @func abort
   */
  static void abort(ThreadID id, bool wait_end = false);
  
  /**
   * @func wait_end
   */
  static void wait_end(ThreadID id);
  
  /**
   * @func awaken
   */
  static void awaken(ThreadID id);

  /**
   * @func sleep_cur sleep current thread
   */
  static void sleep_cur(uint64 timeUs);
  
  /**
   * @func current_id
   */
  static ThreadID current_id();
  
  /**
   * @func is_io_thread
   */
  static bool is_io_thread();

  /**
   * @func main_thread_id
   */
  static ThreadID main_thread_id();
  
  /**
   * @func is_abort
   */
  inline bool is_abort() const { return m_abort; }
  
  /**
   * @func mutex
   */
  inline Mutex& mutex() { return m_mutex; }
  
  /**
   * @func sleep
   */
  void sleep(uint64 timeUs = 0);
  
  /**
   * @func id
   */
  inline ThreadID id() const { return m_id; }
  
  /**
   * @func name
   */
  inline String name() const { return m_name; }
  
private:
  
  struct WaitEnd {
    Mutex mutex;
    Condition cond;
  };
  
  typedef Array<WaitEnd*> WaitEnds;
  
  Mutex     m_mutex;
  Condition m_cond;
  WaitEnds  m_wait_end;
  bool      m_abort;
  ThreadID  m_id;
  uint      m_gid;
  String    m_name;
  
  av_def_inl_cls(Inl);
};

class PostMessage {
public:
  virtual uint post_message(Callback cb, uint64 delay_us = 0) = 0;
};

/**
 * @class RunLoop
 */
class RunLoop: public Object, public PostMessage {
  av_hidden_all_copy(RunLoop);
public:
  
  RunLoop();
  
  /**
   * @destructor
   */
  virtual ~RunLoop();
  
  /**
   * @event onstart
   */
  av_event(onstart);
  
  /**
   * @event onloop
   */
  av_event(onloop);
  
  /**
   * @event onend
   */
  av_event(onend);
  
  /**
   * @get thread {ThreadID}
   */
  inline ThreadID thread_id() const { return _id; }
  
  /**
   * @get timeout {int64}
   */
  inline int64 timeout() const { return _timeout_us; }
  
  /**
   * @func runing
   */
  inline bool runing() const { return get_run_loop(_id); }
  
  /**
   * @func is_alive
   */
  bool is_alive();
  
  /**
   * @func attach
   */
  Object* attach(int id);
  
  /**
   * @func set_attach
   */
  void set_attach(int id, Object* obj);

  /**
   * @func post
   */
  uint post(Callback cb);
  
  /**
   * @func post message
   */
  uint post(Callback cb, uint64 delay_us);
  
  /**
   * @overwrite
   */
  virtual uint post_message(Callback cb, uint64 delay_us = 0);
  
  /**
   * @func abort # 中止消息
   */
  void abort(uint id);
  
  /**
   * 超时设置
   * 没有新消息时的等待时间,
   * 小于0没有新消息立即结束,0永远等待,大于0为超时时间(微妙us)
   * @func run_loop # 运行消息循环
   * @arg timeout_us {int64}
   */
  bool run_loop(int64 timeout_us = 0);
  
  /**
   * @func stop_signal 发出停止信号,并不会立即停止,但不会接收任何新消息
   */
  void stop_signal();
  
  /**
   * 保持活动状态,并返回一个代理,只要不删除返回的代理对像,消息队列会一直保持活跃状态
   * 当前必须为活跃状态否则返回NULL
   * @func keep_alive
   */
  LoopProxy* keep_alive();
  
  /**
   * @func current # 获取当前线程消息队列,如果没有返回NULL
   */
  static RunLoop* current();
  
  /**
   * @func keep_alive_current 保持当前循环活跃并返回代理
   */
  static LoopProxy* keep_alive_current();
  
  /**
   * @func loop # 通过线程获取
   * @ret {RunLoop*}
   */
  static RunLoop* get_run_loop(ThreadID id);
  
  /**
   * @func utils
   */
  static RunLoop* utils();
  
  /**
   * @func next_tick
   */
  static void next_tick(Callback cb);
  
  /**
   * @func post_io
   */
  static void post_io(Callback cb, bool fast = false);

private:
  
  friend class LoopProxy;
  
  av_def_inl_cls(Inl);
  
  struct ExecWrap {
    uint id;
    uint group;
    std::chrono::steady_clock::duration du;
    Callback exec;
  };
  
  ThreadID            _id;
  Condition           _cond;
  List<ExecWrap>      _exec;
  bool                _stop_signal;
  Mutex               _action_lock;
  Mutex               _run_lock;
  int64               _timeout_us;   /* 超时设置
                                      * 没有新消息时的等待时间,
                                      * 小于0立即结束,0永远等待,大于0为超时时间(微妙us) */
  Map<int, Object*>   _attach_obj;
  uint                _ref_count;
  bool                _is_alive;
};

/**
 * @class LoopProxy
 */
class LoopProxy: public Object, public PostMessage {
  av_hidden_all_copy(LoopProxy);
public:
  use_default_allocator();
  virtual ~LoopProxy();
  uint post(Callback cb);
  uint post(Callback cb, uint64 delay_us);
  virtual uint post_message(Callback cb, uint64 delay_us = 0);
  void  abort_all();
  inline void abort(uint id) { m_loop->abort(id); }
  inline RunLoop* host() { return m_loop; }
private:
  inline LoopProxy(): m_group(iid32()) { }
  RunLoop* m_loop;
  uint     m_group;
  friend class RunLoop;
};

/**
 * @class ParallelWorking
 */
class ParallelWorking: public Object {
  av_hidden_all_copy(ParallelWorking);
public:
  typedef Thread::ThreadExec ThreadExec;
  
  ParallelWorking() av_def_err;
  
  /**
   * @destructor
   */
  virtual ~ParallelWorking();
  
  /**
   * @func fork
   */
  ThreadID fork(ThreadExec exec, cString& name);
  
  /**
   * @func abort_fork
   */
  void abort_fork(ThreadID id = ThreadID());
  
  /**
   * @func awaken
   */
  void awaken(ThreadID id = ThreadID());
  
  /**
   * @func post message to main thread
   */
  uint post(Callback cb);

  /**
   * @func post
   */
  uint post(Callback cb, uint64 delay_us);

  /**
   * @func abort_post
   */
  void abort_post(uint id = 0);
  
  /**
   * @func mutex
   */
  inline Mutex& mutex() { return m_mutex; }
  
private:
  
  LoopProxy*              m_proxy;
  Map<ThreadID, ThreadID> m_threads;
  Mutex                   m_mutex;
  uint                    m_gid;
};

av_end
#endif
