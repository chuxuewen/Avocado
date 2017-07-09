/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, Louis.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

#include "autil/string.h"
#include "autil/fs.h"
#include "autil/sys.h"
#include "native-core-js.h"
#include "autil/thread.h"
#include "autil/string-builder.h"
#include "./gui.h"
#include "./tools-1.h"
#include "./json-1.h"

/**
 * @ns avocado::av8
 */

av8_begin

/**
 * @class WrapSimpleHash
 */
class WrapSimpleHash: public WrapBase {
public:
  
  static void constructor(FunctionCall args) {
    New<WrapSimpleHash>(args, new SimpleHash());
  }
  
  static void hash_code(FunctionCall args) {
    av8_worker(args);
    av8_self(SimpleHash);
    av8_return( self->hash_code() );
  }
  
  static void update(FunctionCall args) {
    av8_worker(args);
    if (  args.Length() < 1 ||
        !(args[0]->IsString() ||
          args[0]->IsArrayBuffer() || worker->has_buffer(args[0]))
    ) {
      av8_throw_err("Bad argument");
    }
    av8_self(SimpleHash);
    
    if (args[0]->IsString()) { // 字符串
      v8::Local<v8::String> str = args[0]->ToString();
      uint16 buffer[128];
      int index = 0, count;
      do {
        count = str->Write(buffer, index, 128);
        self->update(buffer, count);
        index += count;
      } while(count);
    } else if (args[0]->IsArrayBuffer()) { // ArrayBuffer
      auto con = args[0].As<v8::ArrayBuffer>()->GetContents();
      self->update((cchar*)con.Data(), (uint)con.ByteLength());
    } else { // Buffer
      Buffer* buff = Wrap<Buffer>::unwrap(args[0]->ToObject())->self();
      self->update(**buff, buff->length());
    }
  }
  
  static void digest(FunctionCall args) {
    av8_worker(args);
    av8_self(SimpleHash);
    av8_return( self->digest() );
  }
  
  static void clear(FunctionCall args) {
    av8_self(SimpleHash);
    self->clear();
  }

  /**
   * @func binding
   */
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(SimpleHash, constructor, {
      av8_set_cls_method(hash_code, hash_code);
      av8_set_cls_method(update, update);
      av8_set_cls_method(digest, digest);
      av8_set_cls_method(clear, clear);
    }, NULL);
  }
};

class Timer: public Reference {
public:
  
  uint      _timer_id;  // id
  uint64    _timeout;   // 超时时间
  int       _loop;      // -1 为无限循环
  RunLoop*  _run_loop;  // 消息队列
  Callback  _cb;
  Callback  _cb2;
  
  void _run_cb(SimpleEvent& d) {
    SimpleEvent evt = { 0, this }; _cb->call(evt);
    
    _timer_id = 0;
    
    if (_loop > 0) {
      _loop--;
    }
    if (_loop) {
      _run();
    } else {
      _cb2 = Callback(); // destroy callback
    }
  }
  
  void _run() {
    if ( !_timer_id ) {
      _timer_id = _run_loop->post(_cb2, _timeout * 1000);
    }
  }
  
  Timer(RunLoop* loop, Callback cb)
  : _timer_id(0)
  , _timeout(0)
  , _loop(1)
  , _run_loop(loop)
  , _cb(cb) {
    
  }
  
  virtual ~Timer() {
    stop();
  }
  
  int loop() const {
    return _loop;
  }
  
  void loop(int loop) {
    _loop = loop;
  }
  
  void run(uint64 timeout, int loop = 1) {
    _timeout = timeout;
    _loop = loop;
    if ( !_timer_id ) {
      _cb2 = Callback(&Timer::_run_cb, this);
      _run();
    }
  }
  
  void stop() {
    if ( _loop && _timer_id ) {
      _run_loop->abort( _timer_id );
      _loop = 0;
      _timer_id = 0;
      _cb2 = Callback(); // destroy callback
    }
  }

};

class WrapTimer: public WrapBase {
public:
  
  static void constructor(FunctionCall args) {
    // av_assert( !WrapBase::is_external(args) );
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction()) {
      av8_throw_err("Bad argument");
    }
    RunLoop* loop = RunLoop::current();
    if (!loop) { // 没有消息队列无法执行这个操作
      av8_throw_err("Unable to obtain thread run queue");
    }
    
    Wrap<Timer>* wrap = nullptr;
    
    wrap = WrapBase::New<WrapTimer>(args, new Timer(loop, Callback([](SimpleEvent& evt) {
      Wrap<Timer>* wrap = Wrap<Timer>::wrap(static_cast<Timer*>(evt.data));
      wrap->call(wrap->worker()->NewStringAscii("__native_handle_cb__"));
    })));
    wrap->Set(worker->NewStringAscii(String("__native_handle_cb__")), args[0]);
  }
  
  static Wrap<Timer>* New(Worker* worker, Local<v8::Value> cb, uint64 timeout, int loop) {
    Local<v8::Object> o = worker->NewNative(av8_typeid(Timer), 1, &cb);
    if ( !o.IsEmpty() ) {
      Wrap<Timer>* wrap = Wrap<Timer>::unwrap(o);
      wrap->self()->run(timeout, loop);
      return wrap;
    }
    return nullptr;
  }
  
  static void Loop(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Timer);
    av8_return( self->loop() );
  }
  
  static void SetLoop(Local<v8::String> name, Local<Value> value, PropertySetCall args) {
    av8_worker(args);
    if (!value->IsInt32()) {
      av8_throw_err("Bad argument");
    }
    av8_self(Timer);
    self->loop(value->ToInt32()->Value());
  }
  
  static void Run(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsNumber()) {
      av8_throw_err("Bad argument");
    }
    uint64 timeout = av_max(0, args[0]->ToNumber()->Value());
    int loop = 1;
    if (args.Length() > 1 && args[1]->IsInt32()) {
      loop = args[1]->ToInt32()->Value();
    }
    av8_self(Timer);
    self->run(timeout, loop);
  }
  
  static void Stop(FunctionCall args) {
    av8_worker(args);
    av8_self(Timer);
    self->stop();
  }

  // global function
  
  static void run_timer(FunctionCall args, int loop) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction()) {
      av8_throw_err("Bad argument");
    }
    uint64 timeout = 0;
    if (args.Length() > 1 && args[1]->IsNumber()) {
      timeout = av_max(0, args[1]->ToNumber()->Value());
    }
    Wrap<Timer>* wrap = New(worker, args[0], timeout, loop);
    if ( wrap ) {
      av8_return( wrap->local() );
    }
  }
  
  static void setTimeout(FunctionCall args) {
    run_timer(args, 1);
  }
  
  static void setInterval(FunctionCall args) {
    run_timer(args, -1);
  }
  
  static void clearTimeout(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || ! worker->has<Timer>(args[0]) ) {
      av8_throw_err("Bad argument");
    }
    Wrap<Timer>::unwrap(args[0])->self()->stop();
  }
  
  static void clearInterval(FunctionCall args) {
    clearTimeout(args);
  }

  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Timer, constructor, {
      av8_set_cls_property(loop, Loop, SetLoop);
      av8_set_cls_method(run, Run);
      av8_set_cls_method(stop, Stop);
    }, nullptr);
    exports = worker->global();
    av8_set_method(setTimeout, setTimeout);
    av8_set_method(setInterval, setInterval);
    av8_set_method(clearTimeout, clearTimeout);
    av8_set_method(clearInterval, clearInterval);
  }
};

static cString Space(' ');

/**
 * @class NativeConsole
 */
class NativeConsole {
public:

  static void print_to(FunctionCall args, void(*func)(cString&)) {
    av8_worker(args);
    StringBuilder rv;
    bool is_space = false;
    
    for (int i = 0; i < args.Length(); i++) {
      if (is_space) {
        rv.push(Space);
      }
      if (args[i]->IsObject()) {
        if (!JSON::stringify_console_styled(worker, args[i], &rv)) {
          return;
        }
      } else {
        rv.push( worker->to_string_utf8(args[i]) );
      }
      is_space = true;
    }
    func(rv.to_string());
  }
  
  static void log(FunctionCall args) {
    print_to(args, console::log);
  }
  
  static void warn(FunctionCall args) {
    print_to(args, console::warn);
  }
  
  static void error(FunctionCall args) {
    print_to(args, console::error);
  }
  
  static void clear(FunctionCall args) {
    console::clear();
  }
  
  static void debug(FunctionCall args) {
    print_to(args, console::log);
  }
  
  static void info(FunctionCall args) {
    print_to(args, console::log);
  }
  
  static void dir(FunctionCall args) { }
  
  static void dirxml(FunctionCall args) { }
  
  static void table(FunctionCall args) { }
  
  static void trace(FunctionCall args) { }
  
  static void group(FunctionCall args) { }
  
  static void groupCollapsed(FunctionCall args) { }
  
  static void groupEnd(FunctionCall args) { }
  
  static void count(FunctionCall args) { }
  
  static void assert(FunctionCall args) { }
  
  static void markTimeline(FunctionCall args) { }
  
  static void profile(FunctionCall args) { }
  
  static void profileEnd(FunctionCall args) { }
  
  static void timeline(FunctionCall args) { }
  
  static void timelineEnd(FunctionCall args) { }
  
  static void time(FunctionCall args) { }
  
  static void timeEnd(FunctionCall args) { }
  
  static void timeStamp(FunctionCall args) { }
  
  static void memory(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_return( worker->Null() );
  }

  static void binding(Local<v8::Object> util, Worker* worker) {
    Local<v8::Object> exports = v8::Object::New(worker->isolate());
    av8_set_method(log, log);
    av8_set_method(warn, warn);
    av8_set_method(error, error);
    av8_set_method(clear, clear);
    av8_set_method(debug, debug);
    av8_set_method(info, info);
    av8_set_method(dir, dir);
    av8_set_method(dirxml, dirxml);
    av8_set_method(table, table);
    av8_set_method(trace, trace);
    av8_set_method(group, group);
    av8_set_method(groupCollapsed, groupCollapsed);
    av8_set_method(groupEnd, groupEnd);
    av8_set_method(count, count);
    av8_set_method(assert, assert);
    av8_set_method(markTimeline, markTimeline);
    av8_set_method(profile, profile);
    av8_set_method(profileEnd, profileEnd);
    av8_set_method(timeline, timeline);
    av8_set_method(timelineEnd, timelineEnd);
    av8_set_method(time, time);
    av8_set_method(timeEnd, timeEnd);
    av8_set_method(timeStamp, timeStamp);
    av8_set_property(memory, memory);
    worker->global()->Set(worker->strs()->console(), exports);
  }
};

/**
 * @class NativeUtil
 */
class NativeUtil {
public:

  static SimpleHash get_hash_code(FunctionCall args) {
    v8::Local<v8::String> str = args[0]->ToString();
    SimpleHash hash;
    uint16 buffer[128];
    int index = 0, count;
    do {
      count = str->Write(buffer, index, 128);
      hash.update(buffer, count);
      index += count;
    } while(count);
    
    return hash;
  }
  
  // api
  static void fatal(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() ) {
      String msg = worker->to_string_utf8(args[0]);
      worker->fatal(msg);
    } else {
      worker->fatal("fatal");
    }
  }
  
  static void abort(FunctionCall args) {
    avocado::abort();
  }
  
  static void exit(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsInt32()) {
      av8_throw_err("Bad argument");
    }
    avocado::exit( args[0]->ToInt32()->Value() );
  }
  
  static void hash_code(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    av8_return( get_hash_code(args).hash_code() );
  }
  
  static void hash(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    av8_return( get_hash_code(args).digest() );
  }
  
  static void iid(FunctionCall args) {
    av8_worker(args);
    av8_return( avocado::iid() );
  }
  
  static void version(FunctionCall args) {
    av8_worker(args);
    av8_return( avocado::version() );
  }
  
  static void add_native_event_listener(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 3 || !args[0]->IsObject() ||
        !args[1]->IsString() || !args[2]->IsFunction()) {
      av8_throw_err("Bad argument");
    }
    if ( ! WrapBase::is_wrap(args[0]->ToObject()) ) {
      av8_throw_err("Bad argument");
    }
    int id = 0;
    if ( args.Length() > 3 && args[3]->IsNumber() ) {
      id = args[3]->ToNumber()->Value();
    }
    { v8::HandleScope scope(worker->isolate());
      WrapBase* wrap = WrapBase::unwrap(args[0]);
      String name = worker->to_string_ascii(args[1]->ToString());
      String func = String("__on").push(name).push("_native_handle");
      wrap->Set(worker->NewStringAscii(func), args[2]);
      bool ok = wrap->add_event_listener(name, func, id);
      av8_return(ok);
    }
  }
  
  static void remove_native_event_listener(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 2 || !args[0]->IsObject() || !args[1]->IsString()) {
      av8_throw_err("Bad argument");
    }
    if ( ! WrapBase::is_wrap(args[0]->ToObject()) ) {
      av8_throw_err("Bad argument");
    }
    int id = 0;
    if ( args.Length() > 2 && args[2]->IsNumber() ) {
      id = args[2]->ToNumber()->Value();
    }
    { v8::HandleScope scope(worker->isolate());
      String name = worker->to_string_ascii(args[1]->ToString());
      WrapBase* wrap = WrapBase::unwrap(args[0]);
      bool ok = wrap->remove_event_listener(name, id);
      if ( ok ) {
        String func = String("__on").push(name).push("_native_handle");
        wrap->local()->Delete( worker->context(), worker->New(func) );
      }
      av8_return(ok);
    }
  }
  
  static void install_native_lib(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    av8_return( Tools::install_native_lib(worker->to_string_utf8(args[0])) );
  }
  
  static void uninstall_native_lib(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    av8_return( Tools::uninstall_native_lib(worker->to_string_utf8(args[0])) );
  }
  
  static void run_script(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    Local<v8::String> name;
    Local<v8::Object> sandbox;
    if (args.Length() > 1) {
      name = args[1]->ToString();
    } else {
      name = worker->NewStringAscii("[eval]").As<v8::String>();
    }
    if (args.Length() > 2 && args[2]->IsObject()) {
      sandbox = args[2]->ToObject();
    }
    Local<Value> rv;
    if (worker->run_script(args[0]->ToString(), name, sandbox).ToLocal(&rv)) { // 没有值可能有异常
      av8_return( rv );
    }
  }
  
  static void next_tick(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction()) {
      av8_throw_err("Bad argument");
    }
    PersistentFunc func(worker->isolate(), args[0].As<Function>());
    RunLoop::next_tick(Callback([worker, func](SimpleEvent& d) { worker->call(func); } ));
  }
  
  static void garbage_collection(FunctionCall args) {
    av8_worker(args);
    worker->isolate()->LowMemoryNotification();
#if av_memory_trace_mark
    std::vector<Object*> objs = Object::mark_objects();
    LOG("All unrelease heap objects count: %d", objs.size());
#endif
  }

  /**
   * @func binding
   */
  static void binding(Local<v8::Object> exports, Worker* worker) {
    worker->binding("_buffer");
    av8_set_field(args, worker->New(worker->args()));
    av8_set_field(debug, worker->debug());
    av8_set_method(fatal, fatal);
    av8_set_method(abort, abort);
    av8_set_method(exit, exit);
    av8_set_method(hash_code, hash_code);
    av8_set_method(hash, hash);
    av8_set_method(iid, iid);
    av8_set_method(version, version);
    av8_set_method(add_native_event_listener, add_native_event_listener);
    av8_set_method(remove_native_event_listener, remove_native_event_listener);
    av8_set_method(install_native_library, install_native_lib);
    av8_set_method(uninstall_native_library, uninstall_native_lib);
    av8_set_method(run_script, run_script);
    av8_set_method(next_tick, next_tick);
    av8_set_method(garbage_collection, garbage_collection);
    WrapSimpleHash::binding(exports, worker);
    WrapTimer::binding(exports, worker);
    NativeConsole::binding(exports, worker);
  }
};

av8_reg_module(_util, NativeUtil)
av8_end
