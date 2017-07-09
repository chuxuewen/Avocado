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
#include "autil/buffer.h"
#include "autil/thread.h"
#include "autil/string-builder.h"
#include "autil/http.h"
#include "libplatform/libplatform.h"
#include "native-core-js.h"
#include "av8.h"
#include "gui.h"
#include "debug-1.h"
#include "agui/css.h"

/**
 * @ns avocado::av8
 */

av8_begin

static Map<ThreadID, Worker*>        all_workers;
static Mutex                         all_workers_lock;
static Condition                     all_workers_cond;
static v8::Platform*                 platform = nullptr;
static uint                          is_initializ = 0;
static String                        unknown("[Unknown]");
static Ucs2String unknown_ucs2(String("[Unknown]"));

// global native module binding function
static Map<String, Worker::Binding>& global_native_module_bindings() {
  static Map<String, Worker::Binding> bindings;
  return bindings;
}

NativeConstructors::NativeConstructors(Worker* worker)
: WorkerObject(worker)
, _attach_object(nullptr) {
}

Local<FunctionTemplate> NativeConstructors::get_template(uint64 id) {
  auto i = _constructors.find(id);
  if ( i != _constructors.end() ) {
    return Local<FunctionTemplate>::New(worker->isolate(), i.value().function_template);
  }
  return Local<FunctionTemplate>();
}

uint64 NativeConstructors::set_constructor(uint64 id,
                                           Local<FunctionTemplate> func,
                                           NativeAllocator allocator) av_def_err {
  av_assert_err( ! _constructors.has(id), "Set native Constructors ID repeat");
  Constructor& con = _constructors.get(id);
  con.function_template.Reset(worker->isolate(), func);
  Local<Value> p = func->GetFunction()->Get(worker->strs()->prototype());
  con.prototype.Reset(worker->isolate(), p);
  con.allocator = allocator;
  return id;
}

uint64 NativeConstructors::set_alias(uint64 id, uint64 alias) av_def_err {
  av_assert_err( _constructors.has(id), "No Constructors ID");
  Constructor& con = _constructors.get(id);
  set_constructor(alias, worker->strong(con.function_template), con.allocator);
  return alias;
}

Local<Value> NativeConstructors::prototype(uint64 id) {
  auto i = _constructors.find(id);
  if ( i != _constructors.end() ) {
    return worker->local(i.value().prototype);
  }
  return Local<v8::Value>();
}

WrapBase* NativeConstructors::attach_external(uint64 id, WrapBase* wrap) {
  av_assert( !wrap->worker() );
  
  auto it = _constructors.find(id);
  if ( it != _constructors.end() ) {
    it.value().allocator.attach_external(wrap);
    
    av_assert( !_attach_object );
    
    Local<FunctionTemplate> func = worker->strong(it.value().function_template);
    _attach_object = wrap;
    func->GetFunction()->NewInstance();
    _attach_object = nullptr;
    
    if ( !wrap->handle().IsEmpty() ) {
      return wrap;
    }
  }
  return nullptr;
}

Local<Function> NativeConstructors::get(uint64 id) {
  if (_constructors.has(id)) {
    return worker->strong(_constructors.get(id).function_template)->GetFunction();
  } else {
    return Local<Function>();
  }
}

bool NativeConstructors::instanceof(Local<Value> val, uint64 id) {
  if (_constructors.has(id)) {
    Local<FunctionTemplate> func = worker->local(_constructors.get(id).function_template);
    return func->HasInstance(val);
  }
  return false;
}

CommonString::CommonString(Worker* worker): WorkerObject(worker) {
#define av8_init_persistent_string(name) \
  __##name##_$_.Reset(worker->isolate(), worker->NewStringAscii(#name));
  
  __Throw_$_.Reset(worker->isolate(), worker->NewStringAscii("throw"));
  av8_common_string(av8_init_persistent_string);
}

class Worker::_Inl: public Worker {
public:
#define _inl_worker(self) static_cast<Worker::_Inl*>(self)
  
  MaybeLocal<Value> run_script(Local<v8::String> source_string,
                               Local<v8::String> name,
                               Local<v8::Object> sandbox) {
    ScriptOrigin origin(name);
    v8::ScriptCompiler::Source source(source_string, ScriptOrigin(name));
    MaybeLocal<v8::Value> result;
    
    if ( sandbox.IsEmpty() ) {
      Local<v8::Script> script;
      if ( v8::ScriptCompiler::Compile(_context, &source).ToLocal(&script) ) {
        result = script->Run(_context);
      }
    } else {
      Local<v8::Function> func;
      if ( v8::ScriptCompiler::CompileFunctionInContext(_context, &source, 0, NULL, 1, &sandbox)
          .ToLocal(&func) ) {
        result = func->Call(_context, Undefined(), 0, NULL);
      }
    }
    return result;
  }
  
  bool run_native_script(Local<v8::Object> exports,
                         cchar* source,
                         uint source_len, cString& name) {
    WeakBuffer buff(source, source_len);
    Local<v8::String> _name = NewStringAscii(String::format("[native %s.js]", *name));
    Local<v8::String> _souece = NewString(buff);
    MaybeLocal<Value> rv;
    rv = run_script(_souece, _name, Local<v8::Object>());
    if ( !rv.IsEmpty() ) {
      Local<Function> func = rv.ToLocalChecked().As<Function>();
      Array<Local<v8::Value>> args(2);
      args[0] = exports;
      args[1] = _global;
      rv = func->Call(_context, Undefined(), 2, &args[0]);
    }
    //  av_assert_err(rv.IsEmpty(), "Unable to execute Native script \"\"", *name);
    return !rv.IsEmpty();
  }
  
  // Extracts a C string from a V8 Utf8Value.
  static cchar* to_cstring(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
  }
  
  String parse_exception_message(Local<v8::Message> message, Local<v8::Value> error) {
    v8::HandleScope handle_scope(_isolate);
    v8::String::Utf8Value exception(error);
    const char* exception_string = to_cstring(exception);
    if (message.IsEmpty()) {
      // V8 didn't provide any extra information about this error; just
      // print the exception.
      // av_err("%s", exception_string);
      return exception_string;
    } else {
      StringBuilder out;
      // Print (filename):(line number): (message).
      v8::String::Utf8Value filename(message->GetScriptOrigin().ResourceName());
      v8::Local<v8::Context> context(_isolate->GetCurrentContext());
      const char* filename_string = to_cstring(filename);
      int linenum = message->GetLineNumber(context).FromJust();
      //av_err("%s:%d: %s", filename_string, linenum, exception_string);
      out.push(String::format("%s:%d: %s\n", filename_string, linenum, exception_string));
      // Print line of source code.
      v8::String::Utf8Value sourceline(message->GetSourceLine(context).ToLocalChecked());
      const char* sourceline_string = to_cstring(sourceline);
      //av_err("%s", sourceline_string);
      out.push(sourceline_string); out.push('\n');
      // Print wavy underline (GetUnderline is deprecated).
      int start = message->GetStartColumn(context).FromJust();
      for (int i = 0; i < start; i++) {
        //av_print_err(" ");
        out.push( ' ' );
      }
      int end = message->GetEndColumn(context).FromJust();
      for (int i = start; i < end; i++) {
        //av_print_err("^");
        out.push( '^' );
      }
      //av_err("");
      out.push( '\n' );
      
      if (error->IsObject()) {
        v8::Local<v8::Value> stack_trace_string;
        if (error->ToObject()->Get(_context, strs()->stack())
            .ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
          v8::String::Utf8Value stack_trace(stack_trace_string);
          const char* stack_trace_string = to_cstring(stack_trace);
          // av_err("%s", stack_trace_string);
          out.push( stack_trace_string ); out.push('\n');
        }
      }
      // av_err( out.to_string() );
      return out.to_string();
    }
  }
  
  void report_exception(Local<v8::Message> message, Local<v8::Value> error) {
    Local<v8::Value> _uncaught_exception = _util->Get(strs()->_uncaught_exception());
    if ( _uncaught_exception->IsFunction() ) {
      v8::TryCatch try_catch;
      Local<v8::Value> rv;
      if ( call(_uncaught_exception.As<Function>(), Undefined(), 1, &error).ToLocal(&rv) ) {
        if ( rv->IsTrue() ) {
          return;
        }
      } else {
        if ( try_catch.HasCaught() ) {
          av_err( parse_exception_message(Local<v8::Message>(), try_catch.Exception()) );
        }
      }
    }
    av_err( parse_exception_message(message, error) );
  }
  
  void fatal_exception(v8::TryCatch* try_catch) {
    if ( try_catch->HasCaught() ) {
      String msg = parse_exception_message(try_catch->Message(), try_catch->Exception());
      fatal( msg );
    }
  }
  
  void exit() {
    Local<v8::Value> func = _util->Get(strs()->_exit());
    if (func->IsFunction()) {
      call(func.As<Function>());
    }
    all_workers_cond.notify_one();
  }
  
  static void binding(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    Local<v8::Object> rev = worker->binding(args[0]);
    if (rev.IsEmpty()) {
      av8_throw_err("Bad argument");
    }
    av8_return( rev );
  }
  
  static void AtExit() {
    Lock ul(all_workers_lock);
    ThreadID cur = Thread::current_id();
    for (auto i = all_workers.begin(); i != all_workers.end(); i++) {
      auto id = i.value()->_thread_id;
      if (id == cur) {
        _inl_worker(i.value())->exit();
      } else {
        auto loop = RunLoop::get_run_loop(id);
        if (loop) { // 没有队列无法调用
          auto worker = _inl_worker(i.value());
          loop->post(Callback([worker](SimpleEvent& d) { worker->exit(); }));
          all_workers_cond.wait(ul); // 等待调用完成
        }
      }
    }
  }
  
  static void MessageCallback(Local<v8::Message> message, Local<v8::Value> error) {
    _inl_worker(Worker::current())->report_exception(message, error);
  }
  
  static void OnFatalError(const char* location, const char* message) {
    if (location) {
      av_fatal("FATAL ERROR: %s %s\n", location, message);
    } else {
      av_fatal("FATAL ERROR: %s\n", message);
    }
  }
};

struct _HandleScope {
  HandleScope value;
  inline _HandleScope(Isolate* isolate): value(isolate) { }
};

#ifndef CONTEXT_INL_CONTEXT_DATA_INDEX
#define CONTEXT_INL_CONTEXT_DATA_INDEX 0
#endif

void set_avocado_js_framework_file_reader(Worker* worker);

void recovery_default_file_reader();

Worker::Worker(const Array<String>& args)
: _args(args)
, _debug(false)
, _isolate( nullptr )
, _locker( nullptr )
, _handle_scope( nullptr )
, _constructors( nullptr )
, _gui_value_program( nullptr )
, _common_string( nullptr )
, _thread_id(Thread::current_id())
{
  { ScopeLock lock(all_workers_lock);
    initializ();
    // set flags
    String FlagsFromString = args.join(' ');
    int Flags_index = FlagsFromString.index_of("-v8 ");
    if ( Flags_index != -1 ) {
      v8::V8::SetFlagsFromString(*FlagsFromString + Flags_index + 3,
                                 FlagsFromString.length() - Flags_index - 3);
    }
    //av_assert_err( Isolate::GetCurrent(), "The current thread already runs the worker !" );
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    _isolate = v8::Isolate::New(create_params);
    _locker = new Locker(_isolate);
    _isolate->Enter();
    
    all_workers.set(_thread_id, this);
  }
  
  _handle_scope = new _HandleScope(_isolate);
  _context = v8::Context::New(_isolate);
  _context->Enter();
  _isolate->SetData(CONTEXT_INL_CONTEXT_DATA_INDEX, this);
  _constructors = new NativeConstructors(this);
  _common_string = new CommonString(this);
  _global = _context->Global();
  _binding.Reset(_isolate, v8::Object::New(_isolate));
  
  //
  
  set_field(_global, "global", _context->Global().As<Value>());
  set_method(_global, "binding", _Inl::binding);
  
  _isolate->SetFatalErrorHandler(_Inl::OnFatalError);
  _isolate->AddMessageListener(_Inl::MessageCallback);
  
  // set debug mark
  for ( int i = 0; i < _args.length(); i++ ) {
    cString& arg = _args[i];
    if (arg.index_of("--debug") == 0) {
      _debug = true; break;
    }
  }

  _util = binding("_util");
  
  if ( _util.IsEmpty() ) {
    av_fatal("Not initialize");
  }
  
  if ( !run_native_script(_util, (cchar*)
                          CORE_native_js_code_ext_,
                          CORE_native_js_code_ext_count_, "ext") ) {
    av_fatal("Not initialize");
  }
  
  set_avocado_js_framework_file_reader(this);
  
  v8::TryCatch try_catch;

  if ( !run_native_script(_util, (cchar*)
                          CORE_native_js_code_avocado_,
                          CORE_native_js_code_avocado_count_, "avocado") ) {
    _inl_worker(this)->fatal_exception(&try_catch);
  }
}

static void set_av8_allocator();

Worker::~Worker() {
  for (auto& i : _data) {
    Release(i.value());
  }
  recovery_default_file_reader();
  _data.clear();
  _binding.Reset();
  _global.Clear();
  Release(_gui_value_program);
  _gui_value_program = nullptr;
  Release(_constructors);
  Release(_common_string);
  _constructors = nullptr;
  _common_string = nullptr;
  _context->Exit();
  _context.Clear();
  delete static_cast<_HandleScope*>( _handle_scope ); // 删除变量范围
  _isolate->Exit();
  delete _locker;
  _isolate->Dispose();
  _isolate = nullptr;
  _locker = nullptr;
  _handle_scope = nullptr;
  
  { std::lock_guard<Mutex> lock(all_workers_lock);
    all_workers.del(_thread_id);
  }
}

void Worker::initializ() {
  if ( is_initializ++ == 0 ) {
    set_av8_allocator();
    platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    v8::V8::SetFlagsFromString(       "--use_strict --harmony",
                               sizeof("--use_strict --harmony"));
    atexit(_Inl::AtExit);
  }
}

void Worker::dispose() {
  if (platform) {
    v8::V8::ShutdownPlatform();
    v8::V8::Dispose();
    delete platform;
    // set_default_allocator();
  }
}

Worker* Worker::create(const Array<String>& args) av_def_err {
  return new Worker(args);
}

Worker* Worker::current() {
  Isolate* isolate = Isolate::GetCurrent();
  return isolate ? worker(isolate) : nullptr;
}

Worker* Worker::worker(Isolate* isolate) {
  return static_cast<Worker*>( isolate->GetData(CONTEXT_INL_CONTEXT_DATA_INDEX) );
}

Worker* Worker::worker(FunctionCall args) {
  return static_cast<Worker*>( args.GetIsolate()->GetData(CONTEXT_INL_CONTEXT_DATA_INDEX) );
}

Worker* Worker::worker(PropertyCall args) {
  return static_cast<Worker*>( args.GetIsolate()->GetData(CONTEXT_INL_CONTEXT_DATA_INDEX) );
}

Worker* Worker::worker(PropertySetCall args) {
  return static_cast<Worker*>( args.GetIsolate()->GetData(CONTEXT_INL_CONTEXT_DATA_INDEX) );
}

void Worker::reg_module(cString& name, Binding binding) {
  if ( global_native_module_bindings().has(name) ) {
    av_fatal("Module Repeated definition \"%s\".", *name);
  }
  global_native_module_bindings().set(name, binding);
}

void Worker::del_module(cString& name) {
  global_native_module_bindings().del(name);
}

Local<Value> Worker::New(float data) {
  return v8::Number::New(_isolate, data);
}
Local<Value> Worker::New(double data) {
  return v8::Number::New(_isolate, data);
}
Local<Value> Worker::New(bool data) {
  return Boolean::New(_isolate, data);
}
Local<Value> Worker::New(char data) {
  return Integer::New(_isolate, data);
}
Local<Value> Worker::New(byte data) {
  return Integer::New(_isolate, data);
}
Local<Value> Worker::New(int16 data) {
  return Integer::New(_isolate, data);
}
Local<Value> Worker::New(uint16 data) {
  return Integer::New(_isolate, data);
}
Local<Value> Worker::New(int data) {
  return Integer::New(_isolate, data);
}
Local<Value> Worker::New(uint data) {
  return v8::Number::New(_isolate, data);
}
Local<Value> Worker::New(int64 data) {
  return v8::Number::New(_isolate, data);
}
Local<Value> Worker::New(uint64 data) {
  return v8::Number::New(_isolate, data);
}
Local<v8::Value> Worker::New(cchar* data) {
  return v8::String::NewFromUtf8(_isolate, data, v8::String::kNormalString);
}
Local<Value> Worker::New(cString& data) {
  return v8::String::NewFromUtf8(_isolate, *data, v8::String::kNormalString, data.length());
}
Local<Value> Worker::New(cUcs2String& data) {
  return v8::String::NewExternal(_isolate, new V8ExternalStringResource(data));
}
Local<Value> Worker::New(cError& data) {
  return NewError(*data.message());
}
Local<v8::Value> Worker::New(const HttpError& err) {
  return NewError(err);
}
Local<Value> Worker::New(const Array<String>& data) {
  Local<v8::Array> rev = v8::Array::New(_isolate);
  { v8::HandleScope scope(_isolate);
    for (int i = 0, e = data.length(); i < e; i++) {
      Local<v8::Value> value = New(data[i]);
      rev->Set(i, value);
    }
  }
  return rev.As<Value>();
}
Local<v8::Value> Worker::New(Array<FileStat>& ls) {
  return New(move(ls));
}
Local<v8::Value> Worker::New(Array<FileStat>&& ls) {
  Local<v8::Array> rev = v8::Array::New(_isolate);
  { v8::HandleScope scope(_isolate);
    for (int i = 0, e = ls.length(); i < e; i++) {
      Local<v8::Value> value = New( move(ls[i]) );
      rev->Set(context(), i, value).IsJust();
    }
  }
  return rev;
}
Local<Value> Worker::New(const Map<String, String>& data) {
  Local<v8::Object> rev = v8::Object::New(_isolate);
  { v8::HandleScope scope(_isolate);
    for (auto i = data.begin(), e = data.end(); i != e; i++) {
      Local<v8::String> key = NewStringAscii(i.data().key());
      Local<v8::Value> value = New(i.value());
      rev->Set(key.As<Value>(), value);
    }
  }
  return rev.As<Value>();
}
Local<v8::Value> Worker::New(Buffer& buff) {
  return New(move(buff));
}
Local<v8::Value> Worker::New(Buffer&& buff) {
  Local<v8::Value> rev = native_constructors()->get(av8_typeid(Buffer))->NewInstance();
  *Wrap<Buffer>::unwrap(rev->ToObject())->self() = move(buff);
  return rev;
}
Local<v8::Value> Worker::New(FileStat& stat) {
  return New(move(stat));
}
Local<v8::Value> Worker::New(FileStat&& stat) {
  Local<v8::Value> rev = native_constructors()->get(av8_typeid(FileStat))->NewInstance();
  *Wrap<FileStat>::unwrap(rev->ToObject())->self() = move(stat);
  return rev;
}
Local<v8::Value> NewDirent(Worker* worker, const Dirent& dir) {
  Local<v8::Object> rev = worker->NewObject().As<v8::Object>();
  rev->Set(worker->context(), worker->strs()->name(), worker->New(dir.name)).IsJust();
  rev->Set(worker->context(), worker->strs()->pathname(), worker->New(dir.pathname)).IsJust();
  rev->Set(worker->context(), worker->strs()->type(), worker->New(dir.type)).IsJust();
  return rev.As<v8::Value>();
}
Local<v8::Value> Worker::New(const Dirent& dir) {
  return NewDirent(this, dir);
}
Local<v8::Value> Worker::New(Array<Dirent>& ls) {
  return New(move(ls));
}
Local<v8::Value> Worker::New(Array<Dirent>&& ls) {
  Local<v8::Array> rev = v8::Array::New(_isolate);
  for (int i = 0, e = ls.length(); i < e; i++) {
    Local<v8::Value> value = NewDirent(this, ls[i]);
    rev->Set(context(), i, value).IsJust();
  }
  return rev.As<v8::Value>();
}
Local<v8::Value> Worker::New(Local<v8::Value> value) {
  return value;
}
Local<v8::Object> Worker::NewNative(uint64 id, uint argc, Local<v8::Value>* argv) {
  Local<Function> constructor = native_constructors()->get(id);
  if (!constructor.IsEmpty()) {
    return constructor->NewInstance(argc, argv);
  }
  return Local<v8::Object>();
}
Local<v8::Value> Worker::NewObject() {
  return v8::Object::New(_isolate);
}
Local<v8::Value> Worker::NewArray() {
  return v8::Array::New(_isolate);
}
Local<v8::Value> Worker::Null() {
  return v8::Null(_isolate);
}
Local<v8::Value> Worker::Undefined() {
  return v8::Undefined(_isolate);
}
Local<v8::String> Worker::NewStringAscii(cString& ascii) {
  return v8::String::NewExternal(_isolate, new V8ExternalOneByteStringResource(ascii));
}
Local<v8::String> Worker::NewString(const Buffer& data) {
  return v8::String::NewFromUtf8(_isolate, *data, v8::String::kNormalString, data.length());
}
Local<v8::Value> Worker::NewBuffer(Local<v8::String> str, Encoding en) {
  Array<Local<Value>> args(2);
  args[0] = str;
  args[1] = NewStringAscii(Coder::encoding_string(en));
  return native_constructors()->get(av8_typeid(Buffer))->NewInstance(2, &args[0]);
}
Local<Value> Worker::NewRangeError(cchar* errmsg, ...) {
  av_string_format(errmsg, str);
  return v8::Exception::RangeError( New(str)->ToString() );
}
Local<Value> Worker::NewReferenceError(cchar* errmsg, ...) {
  av_string_format(errmsg, str);
  return v8::Exception::ReferenceError( New(str)->ToString() );
}
Local<Value> Worker::NewSyntaxError(cchar* errmsg, ...) {
  av_string_format(errmsg, str);
  return v8::Exception::SyntaxError( New(str)->ToString() );
}
Local<Value> Worker::NewTypeError(cchar* errmsg, ...) {
  av_string_format(errmsg, str);
  return v8::Exception::TypeError( New(str)->ToString() );
}
Local<Value> Worker::NewError(cchar* errmsg, ...) {
  av_string_format(errmsg, str);
  Local<v8::Object> e = v8::Exception::Error(New(str)->ToString()).As<v8::Object>();
  e->Set( strs()->code(), New( ERR_UNKNOWN_ERROR ) );
  return e.As<Value>();
}
Local<Value> Worker::NewError(cError& err) {
  Local<v8::Object> e = v8::Exception::Error(New(err.message())->ToString()).As<v8::Object>();
  e->Set( strs()->code(), New(err.code()) );
  return e.As<Value>();
}
Local<v8::Value> Worker::NewError(const HttpError& err) {
  Local<v8::Object> e = NewError(err.message()).As<v8::Object>();
  e->Set( strs()->status(), New(err.status()) );
  e->Set( strs()->url(), New(err.url()) );
  e->Set( strs()->code(), New(err.code()) );
  return e.As<Value>();
}
Local<Value> Worker::NewError(Local<v8::Object> value) {
  Local<v8::Value> msg = value->Get(strs()->message());
  Local<v8::Object> e = v8::Exception::Error(msg->ToString()).As<v8::Object>();
  Local<v8::Array> names = value->GetPropertyNames();
  for (int i = 0, j = 0; i < names->Length(); i++) {
    Local<v8::Value> key = names->Get(i);
    e->Set(key, value->Get(key));
  }
  return e.As<Value>();
}
void Worker::throw_err(Local<v8::Value> exception) {
  _isolate->ThrowException(exception);
}
void Worker::throw_err(cchar* errmsg, ...) {
  av_string_format(errmsg, str);
  throw_err(NewError(*str));
}

String Worker::to_string_ascii(v8::Local<v8::Value> ascii) {
  Local<v8::String> str = ascii->ToString();
  if ( str.IsEmpty() ) {
    return unknown;
  }
  String rev;
  Buffer buffer(128);
  int index = 0, count;
  do {
    count = str->WriteOneByte((byte*)*buffer, index, 128);
    rev.push(*buffer, count);
    index += count;
  } while(count);
  return rev;
}

String Worker::to_string_utf8(v8::Local<v8::Value> uft8) {
  if ( uft8.IsEmpty() ) {
    return unknown;
  }
  Local<v8::String> str = uft8->ToString();
  if ( str.IsEmpty() ) {
    return unknown;
  }
  StringBuilder rev;
  ArrayBuffer<uint16> buffer(128);
  int index = 0;
  int count;
  while( (count = str->Write(*buffer, index, 128)) ) {
    Buffer buff = Coder::encoding(Encoding::utf8, *buffer, count);
    rev.push(move(buff));
    index += count;
  }
  return rev.to_string();
}

Ucs2String Worker::to_string_ucs2(v8::Local<v8::Value> value) {
  Local<v8::String> str = value->ToString();
  if ( str.IsEmpty() ) {
    return unknown_ucs2;
  }
  Ucs2String rev;
  uint16 buffer[128];
  int index = 0, count;
  do {
    count = str->Write(buffer, index, 128);
    rev.push(buffer, count);
    index += count;
  } while(count);
  
  return rev;
}

Map<String, int> Worker::to_map_int(v8::Local<v8::Value> value) {
  Map<String, int> r;
  
  if ( !value.IsEmpty() && value->IsObject() ) {
    v8::Local<v8::Object> object = value.As<v8::Object>();
    Local<v8::Array> names = object->GetPropertyNames();
    
    for ( uint i = 0, len = names->Length(); i < len; i++ ) {
      Local<v8::Value> key = names->Get(i);
      Local<v8::Value> val;
      if ( object->Get(this->context(), key).ToLocal(&val) ) {
        if ( val->IsNumber() ) {
          r.set( to_string_utf8(key), val->ToNumber()->Value() );
        } else {
          r.set( to_string_utf8(key), val->ToBoolean()->Value() );
        }
      }
    }
  }
  return r;
}

Map<String, String> Worker::to_map_string(v8::Local<v8::Value> value) {
  Map<String, String> r;
  
  if ( !value.IsEmpty() && value->IsObject() ) {
    v8::Local<v8::Object> object = value.As<v8::Object>();
    Local<v8::Array> names = object->GetPropertyNames();
    
    for ( uint i = 0, len = names->Length(); i < len; i++ ) {
      Local<v8::Value> key = names->Get(i);
      Local<v8::Value> val;
      if ( object->Get(this->context(), key).ToLocal(&val) ) {
        if ( val->IsNumber() ) {
          r.set( to_string_utf8(key), val->ToNumber()->Value() );
        } else {
          r.set( to_string_utf8(key), val->ToBoolean()->Value() );
        }
      }
    }
  }
  return r;
}

Array<String> Worker::to_arr_string(v8::Local<v8::Value> value) {
  Array<String> rv;
  if ( !value.IsEmpty() && value->IsArray() ) {
    v8::Local<v8::Array> arr = value.As<v8::Array>();
    for ( uint i = 0, len = arr->Length(); i < len; i++ ) {
      Local<v8::Value> val;
      if ( arr->Get(this->context(), i).ToLocal(&val) ) {
        rv.push( to_string_utf8(val) );
      }
    }
  }
  return rv;
}

bool Worker::has_buffer(Local<Value> val) {
  return _constructors->instanceof(val, av8_typeid(Buffer));
}

bool Worker::has_view(Local<v8::Value> val) {
  return _constructors->instanceof(val, gui::View::VIEW);
}

bool Worker::equals_ascii(Local<v8::Value> val1, cString& ascii) {
  return val1->Equals( NewStringAscii( ascii ));
}

Local<FunctionTemplate> Worker::def_class(cString& name,
                                          FunctionCallback constructor,
                                          Local<FunctionTemplate> base) {
  Local<FunctionTemplate> func = FunctionTemplate::New(_isolate, constructor);
  Local<v8::String> class_name = NewStringAscii(name);
  if ( ! base.IsEmpty() )
    func->Inherit(base);
  func->InstanceTemplate()->SetInternalFieldCount(1);
  func->SetClassName(class_name);
  return func;
}

Local<FunctionTemplate> Worker::def_class(cString& name, FunctionCallback constructor, uint64 base) {
  return def_class(name, constructor, native_constructors()->get_template(base));
}

void Worker::def_class_end(cString& name, uint64 id,
                           Local<FunctionTemplate> func,
                           NativeAllocator allocator, Local<v8::Object> exports) {
  native_constructors()->set_constructor(id, func, allocator);
  if ( ! exports.IsEmpty()) {
    exports->Set(NewStringAscii( name ), func->GetFunction());
  }
}

void Worker::set_cls_method(Local<FunctionTemplate> cls, cString& name, FunctionCallback func) {
  Local<Signature> s = Signature::New(_isolate, cls);
  //  Local<FunctionTemplate> t = FunctionTemplate::New(_isolate, func, Local<Value>(), s);
  Local<FunctionTemplate> t =
    FunctionTemplate::NewWithFastHandler(_isolate, func, NULL, Local<Value>(), s);
  Local<v8::String> fn_name = NewStringAscii(name);
  t->SetClassName(fn_name);
  cls->PrototypeTemplate()->Set(fn_name, t);
}

void Worker::set_cls_property(Local<FunctionTemplate> cls,
                              cString& name,
                              AccessorGetterCallback get, AccessorSetterCallback set) {
  Local<AccessorSignature> s = AccessorSignature::New(_isolate, cls);
  Local<v8::String> fn_name = NewStringAscii(name);
  cls->PrototypeTemplate()->SetAccessor(fn_name, get, set,
                                        Local<Value>(), v8::DEFAULT, v8::None, s);
}

void Worker::set_cls_indexed(Local<FunctionTemplate> cls,
                             IndexedPropertyGetterCallback get, IndexedPropertySetterCallback set) {
  v8::IndexedPropertyHandlerConfiguration cfg(get, set);
  cls->PrototypeTemplate()->SetHandler(cfg);
}

void Worker::set_method(Local<Template> exports, cString& name, FunctionCallback func) {
  //  Local<FunctionTemplate> t = FunctionTemplate::New(_isolate, func);
  Local<FunctionTemplate> t = FunctionTemplate::NewWithFastHandler(_isolate, func);
  Local<v8::String> fn_name = NewStringAscii(name);
  t->SetClassName(fn_name);
  exports->Set(fn_name, t);
}

void Worker::set_method(Local<v8::Object> exports, cString& name, FunctionCallback func) {
  //  Local<FunctionTemplate> t = v8::FunctionTemplate::New(_isolate, func);
  Local<FunctionTemplate> t = FunctionTemplate::NewWithFastHandler(_isolate, func);
  Local<Function> fn = t->GetFunction();
  Local<v8::String> fn_name = NewStringAscii(name);
  fn->SetName(fn_name);
  exports->Set(fn_name, fn);
}

void Worker::set_property(Local<v8::Object> exports, cString& name,
                          AccessorGetterCallback get, AccessorSetterCallback set) {
  Local<v8::String> fn_name = NewStringAscii(name);
  exports->SetAccessor(fn_name, get, set);
}

Local<v8::Object> Worker::binding(cString& name) {
  return binding( NewStringAscii(name) );
}

Local<v8::Object> Worker::binding(Local<Value> name) {
  Local<v8::Value> rev = local(_binding)->Get(name);
  if ( rev->IsObject() ) {
    return rev->ToObject();
  } else {
    String str = to_string_ascii(name.As<Value>());
    auto i = global_native_module_bindings().find(str);
    if (i != global_native_module_bindings().end()) {
      Local<v8::Object> exports = v8::Object::New(_isolate);
      { HandleScope handle_scope(_isolate);
        local(_binding)->Set(name, exports);
        i.value()(exports, this);
      }
      return exports;
    }
  }
  return Local<v8::Object>();
}

void Worker::report_exception(v8::TryCatch* try_catch) {
  _inl_worker(this)->report_exception(try_catch->Message(), try_catch->Exception());
}

MaybeLocal<v8::Value> Worker::call(Local<v8::Function> func,
                                   Local<v8::Value> recv, int argc, v8::Local<v8::Value> argv[]) {
  if (recv.IsEmpty()) {
    recv = Undefined();
  }
  return func->Call(_context, recv, argc, argv);
}
MaybeLocal<v8::Value> Worker::call(PersistentFunc func,
                                   Local<v8::Value> recv, int argc, v8::Local<v8::Value> argv[]) {
  return call(local(func), recv, argc, argv);
}

MaybeLocal<v8::Value> Worker::call_constructor(Local<v8::Function> func,
                                               int argc, v8::Local<v8::Value> argv[]) {
  return func->CallAsConstructor(_context, argc, argv);
}
MaybeLocal<v8::Value> Worker::call_constructor(PersistentFunc func,
                                               int argc, v8::Local<v8::Value> argv[]) {
  return call_constructor(local(func), argc, argv);
}

MaybeLocal<Value> Worker::run_script(cString& source,
                                     cString& name, Local<v8::Object> sandbox) {
  return run_script(New(name)->ToString(), New(source)->ToString(), sandbox);
}

MaybeLocal<Value> Worker::run_script(Local<v8::String> source_string,
                                     Local<v8::String> name, Local<v8::Object> sandbox) {
  return _inl_worker(this)->run_script(source_string, name, sandbox);
}

bool Worker::run_native_script(Local<v8::Object>
                               exports, cchar* source, uint source_len, cString& name) {
  HandleScope scope(_isolate);
  return _inl_worker(this)->run_native_script(exports, source, source_len, name);
}

Object* Worker::get_data(int id) {
  auto i = _data.find(id);
  if (i != _data.end()) {
    return i.value();
  }
  return nullptr;
}

void Worker::set_data(int id, Object* obj) {
  auto i = _data.find(id);
  if (i != _data.end()) {
    return Release( i.value() );
  }
  _data.set(id, obj);
}

void Worker::set_gui_value_program(ValueProgram* value) av_def_err {
  av_assert_err(!_gui_value_program, "No need to repeat settings");
  _gui_value_program = value;
}

/**
 * @func fatal exit worker
 */
void Worker::fatal(cString& msg) {
  RunLoop* loop = RunLoop::get_run_loop(_thread_id);
  if ( loop ) {
    loop->post(Callback([msg](SimpleEvent& evt) {
      av8_cur_worker();
      
      if ( worker ) {
        av_err( msg );
        // av_fatal("fatal");
        // TODO exit worker ...
      } else { // No worker
        av_err( msg );
      }
    }));
  } else { // No loop
    av_err( msg );
  }
}

//#undef av_memory_trace_mark
//#define av_memory_trace_mark 0

#if av_memory_trace_mark
static int record_wrap_count = 0;
static int record_strong_count = 0;

# define print_wrap(s) \
  av_debug("record_wrap_count: %d, strong: %d, %s", record_wrap_count, record_strong_count, s)

#else
# define print_wrap(s)
#endif

/**
 * @class WrapBase::_Inl
 */
class WrapBase::Inl: public WrapBase {
public:
#define _inl_wrap(self) static_cast<WrapBase::Inl*>(self)
  
  static void weak_callback(const v8::WeakCallbackInfo<WrapBase>& data) {
    WrapBase* obj = data.GetParameter();
    obj->_handle.Reset();
    delete obj;
  }
  
  inline void mark_strong() {
#if av_memory_trace_mark
    if (_handle.IsWeak()) record_strong_count++;
    print_wrap("mark_strong");
#endif
    _handle.ClearWeak<WrapBase>();
  }
  
};

void WrapBase::initializ2(FunctionCall args) {
  av_assert(args.This()->InternalFieldCount() > 0);
  av_assert(args.IsConstructCall());
  _worker = Worker::worker(args);
  av_assert( !_worker->native_constructors()->_attach_object );
  _handle.Reset(_worker->isolate(), args.This());
  args.This()->SetAlignedPointerInInternalField(0, this);
#if av_memory_trace_mark
  record_wrap_count++; record_strong_count++;
#endif
  make_weak();
  initializ();
}

WrapBase* WrapBase::External(FunctionCall args) {
  av8_worker(args);
  auto cons = worker->native_constructors();
  if ( cons->_attach_object ) {
    WrapBase* wrap = cons->_attach_object;
    av_assert(!wrap->_worker);
    av_assert(args.This()->InternalFieldCount() > 0);
    av_assert(args.IsConstructCall());
    wrap->_worker = worker;
    wrap->_handle.Reset(worker->isolate(), args.This());
    args.This()->SetAlignedPointerInInternalField(0, wrap);
    cons->_attach_object = nullptr;
    wrap->initializ();
#if av_memory_trace_mark
    record_wrap_count++; record_strong_count++;
    print_wrap("External");
#endif
    return wrap;
  }
  return nullptr;
}

WrapBase::~WrapBase() {
#if av_memory_trace_mark
  record_wrap_count--;
  print_wrap("~WrapBase");
#endif
  av_assert( _handle.IsEmpty() );
  
  if ( _data ) {
    _data->release();
    _data = nullptr;
  }
  Object* s = self();
  s->~Object();
}

void WrapBase::make_weak() {
#if av_memory_trace_mark
  if (!_handle.IsWeak()) record_strong_count--;
  print_wrap("make_weak");
#endif
  _handle.MarkIndependent();
  _handle.SetWeak(this, Inl::weak_callback, v8::WeakCallbackType::kParameter);
}

/**
 * @func set_data
 */
void WrapBase::set_data(Data* data) {
  if ( _data )
    _data->release();
  _data = data;
}

MaybeLocal<v8::Value> WrapBase::call(Local<Value> name, int argc, v8::Local<v8::Value> argv[]) {
  Local<v8::Object> o = _worker->strong(_handle);
  Local<v8::Value> func = o->Get(name);
  if ( func->IsFunction() ) {
    return _worker->call(func.As<Function>(), o, argc, argv);
  } else {
    _worker->throw_err("Function not found, \"%s\"", *_worker->to_string_utf8(name));
    return MaybeLocal<v8::Value>();
  }
}

MaybeLocal<v8::Value> WrapBase::call(cString& name, int argc, v8::Local<v8::Value> argv[]) {
  return call(_worker->New(name), argc, argv);
}

WrapBase* WrapBase::wrap(Object* object, uint64 type_id) {
  WrapBase* wrap = reinterpret_cast<WrapBase*>(object) - 1;
  if ( !wrap->worker() ) { // uninitialized
    av8_cur_worker();
    return worker->native_constructors()->attach_external(type_id, wrap);
  }
  return wrap;
}

class Allocator {
public:
  
  static void* av8_alloc(ulong size) {
    WrapBase* o = (WrapBase*)::malloc(size + sizeof(WrapBase));
    av_assert(o);
    o->_worker = nullptr;
    return o + 1;
  }
  
  static void av8_release(Object* obj) {
    WrapBase* wrap = reinterpret_cast<WrapBase*>(obj) - 1;
    if ( wrap->worker() ) {
      wrap->make_weak();
    }  else { // uninitialized
      obj->~Object();
      ::free(wrap);
    }
  }
  
  static void av8_retain(Object* obj) {
    WrapBase* wrap = reinterpret_cast<WrapBase*>(obj) - 1;
    if ( wrap->worker() ) {
      _inl_wrap(wrap)->mark_strong();
    } // else // uninitialized
  }

};

static void set_av8_allocator() {
  set_allocator(&Allocator::av8_alloc, &Allocator::av8_release, &Allocator::av8_retain);
}

// ----------------- start worker -----------------

/**
 * @func start
 */
static int start(const Array<String>& args) {
  static Mutex mutex; // 锁住这个函数同时只能一个线程调用
  ScopeLock lock(mutex);
  RunLoop loop;
  
  loop.post(Callback([&](SimpleEvent& d) {
    //start_debug(String("http://192.168.1.100:1026/TouchCodePro --debug").split(" "));
    start_debug(args);
    Worker::create(args); // start
  }));
  loop.run_loop(-1); // 运行消息循环
  
  av8_cur_worker();
  Release(worker);
  print_wrap("exit");
  
  stop_debug();
  
  return 0;
}

/**
 * @func start
 */
int start(int argc, char* argv[]) {
  Array<String> args(argc);
  for (int i = 0; i < argc; i++) {
    args[i] = argv[i];
  }
  return start(args);
}

/**
 * @func start
 */
int start(cString& args) {
  return start(args.split(' '));
}

av8_end
