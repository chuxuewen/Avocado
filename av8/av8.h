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

#ifndef __avocado__av8__av8__
#define __avocado__av8__av8__

#include "autil/util.h"
#include "autil/string.h"
#include "autil/buffer.h"
#include "autil/map.h"
#include "autil/error.h"
#include "autil/event.h"
#include "autil/fs.h"
#include "autil/thread.h"
#include <v8.h>

// ------------- v8 common macro -------------

#define av8_begin            av_ns(avocado) av_ns(av8) using namespace v8;
#define av8_end              av_end av_end
#define av8_ns(name)         av8_begin av_ns(name)
#define av8_nsd              av8_end av_end
#define av8_worker(iso)      avocado::av8::Worker* worker = avocado::av8::Worker::worker((iso))
#define av8_cur_worker()     avocado::av8::Worker* worker = avocado::av8::Worker::current()
#define av8_self(type)       type* self = Wrap<type>::Self(args)
#define av8_unwrap(type)     avocado::av8::Wrap<type>* wrap = avocado::av8::Wrap<type>::unwrap(args)
#define av8_return(rev)      return worker->result(args, (rev))
#define av8_return_null()    return worker->result(args, worker->Null())
#define av8_wrap(T, o)       avocado::av8::Wrap<T>::wrap(o, av8_typeid(T))

#define av8_throw_range_err(err, ...) \
  return worker->throw_err(worker->NewRangeError((err), ##__VA_ARGS__))

#define av8_throw_reference_err(err, ...) \
  return worker->throw_err(worker->NewReferenceError((err), ##__VA_ARGS__))

#define av8_throw_syntax_err(err, ...) \
  return worker->throw_err(worker->NewSyntaxError((err), ##__VA_ARGS__))

#define av8_throw_type_err(err, ...) \
  return worker->throw_err(worker->NewTypeError((err), ##__VA_ARGS__))

#define av8_throw_err(err, ...)  return worker->throw_err(worker->NewError((err), ##__VA_ARGS__))
#define av8_try_catch(block, Error) try block catch(const Error& e) { av8_throw_err(e); }
#define av8_handle_scope()  v8::HandleScope scope(worker->isolate())

#define av8_typeid(t) (typeid(t).hash_code())

#define av8_reg_module(name, cls) \
  av_init_block(av8_reg_module_##name) { avocado::av8::Worker::reg_module(#name, cls::binding); }

#define av8_binding_class(name, constructor, block, base) \
  auto cls = worker->def_class(#name, constructor, av8_typeid(base)); \
  struct Attach { static void attach(WrapBase* o) { \
    static_assert(sizeof(WrapBase)==sizeof(Wrap##name), \
    "Derived wrap class pairs cannot declare data members"); new(o) Wrap##name(); } }; \
  block worker->def_class_end(#name, av8_typeid(name), cls, { 0, &Attach::attach }, exports)

#define av8_binding_class_no_exports(name, constructor, block, base) \
  auto cls = worker->def_class(#name, constructor, av8_typeid(base)); \
  struct Attach { static void attach(WrapBase* o) { \
    static_assert(sizeof(WrapBase)==sizeof(Wrap##name), \
    "Derived wrap class pairs cannot declare data members"); new(o) Wrap##name(); } }; \
  block worker->def_class_end(#name, av8_typeid(name), cls, { 0, &Attach::attach })

#define av8_set_cls_method(name, func)       worker->set_cls_method(cls, #name, func)
#define av8_set_cls_property(name, get, ...) worker->set_cls_property(cls, #name, get, ##__VA_ARGS__)
#define av8_set_cls_indexed(get, ...)        worker->set_cls_indexed(cls, get, ##__VA_ARGS__)
#define av8_set_method(name, func)           worker->set_method(exports, #name, func)
#define av8_set_property(name, get, ...)     worker->set_property(exports, #name, get, ##__VA_ARGS__)
#define av8_set_field(name, value)           worker->set_field(exports, #name, value)
#define av8_set_static_field(name, value)    worker->set_field(cls, #name, value)
#define av8_set_cls_field(name, value)       worker->set_cls_field(cls, #name, value)

#define av8_bind_native_event( name, type, block) \
  av_on(name, [this, func]( type & evt) { v8::HandleScope scope(worker()->isolate()); block }, id)

#define av8_bind_common_native_event( name ) \
  av8_bind_native_event(name, Event<>, { call(_worker->NewStringAscii(func)); })

#define av8_unbind_native_event( name ) av_off(name, id);

#define av8_external(args) if (WrapBase::External(args)) return

// ------------- v8 common string -------------

#define av8_common_string(F)  \
F(global)     F(exports)  F(constructor)  F(console)  F(_native)  F(__proto__)          \
F(prototype)  F(type)     F(value)        F(is_auto)  F(width)    F(height)             \
F(offset)     F(offset_x) F(offset_y)     F(_value)   F(r)        F(g)                  \
F(b)          F(a)        F(x)            F(y)        F(z)        F(start)              \
F(point)      F(end)      F(w)            F(size)     F(color)    F(toJSON)             \
F(___mark_json_stringify__)               F(stack)    F(get_path) F(_uncaught_exception)\
F(_exit)      F(__view)   F(trigger_remove_view)                                        \
F(code)       F(message)  F(status)       F(url)      F(id)       F(start_x)            \
F(start_y)    F(force)    F(view)         F(m_noticer)            F(p1_x)     F(p1_y)   \
F(p2_x)       F(p2_y)     F(time)         F(m_change_touches)     F(name)     F(pathname)\
F(styles)     F(sender)   F(__controller)

namespace avocado {
  class HttpError;
}

/**
 * @ns avocado::av8
 */

av8_begin

namespace gui {
  class ValueProgram;
}

using gui::ValueProgram;

struct NativeJSCode {
  uint count;
  const byte* code;
  String name;
};

template<class T> struct CopyablePersistent {
  typedef Persistent<T, CopyablePersistentTraits<T>> Handle;
};
typedef const FunctionCallbackInfo<Value>& FunctionCall;
typedef const PropertyCallbackInfo<Value>& PropertyCall;
typedef const PropertyCallbackInfo<void>& PropertySetCall;
typedef CopyablePersistent<Function>::Handle PersistentFunc;

class Worker;
class V8Factory;
class WrapBase;
class Allocator;

/**
 * @class WorkerObject
 */
class WorkerObject: public Object {
  av_hidden_all_copy(WorkerObject); protected:
  Worker* worker;
  WorkerObject(Worker* worker): worker(worker) { }
};

/**
 * @class CommonString
 */
class CommonString: public WorkerObject {
#define av8_def_persistent_string(name) \
private: v8::Persistent<v8::Value> __##name##_$_; \
public: v8::Local<v8::Value> name() { \
  return *reinterpret_cast<Local<Value>*>(const_cast<Persistent<Value>*>(&__##name##_$_)); \
}
private:
  av8_def_persistent_string(Throw)
  av8_common_string(av8_def_persistent_string); public:
  CommonString(Worker* worker);
};

/**
 * @class NativeConstructors
 */
class NativeConstructors: public WorkerObject {
public:
  NativeConstructors(Worker* worker);
  
  typedef void* (*Alloc)();
  typedef void  (*Attach)(WrapBase* wrap);
  
  struct NativeAllocator {
    Alloc   alloc;
    Attach  attach_external;
  };
  
  /**
   * @func set_constructor
   */
  uint64 set_constructor(uint64 id, Local<FunctionTemplate> func, NativeAllocator allocator) av_def_err;
  
  /**
   * @func set_alias
   */
  uint64 set_alias(uint64 id, uint64 alias) av_def_err;
  
  /**
   * @func get
   */
  Local<Function> get(uint64 id);
  
  /**
   * @func constructor
   */
  Local<FunctionTemplate> get_template(uint64 id);
  
  /**
   * @func prototype
   */
  Local<Value> prototype(uint64 id);
  
  /**
   * @func attach_external
   */
  WrapBase* attach_external(uint64 id, WrapBase* wrap);
  
  /**
   * @func instanceof
   */
  bool instanceof(Local<Value> val, uint64 id);
  
  /**
   * @func has
   */
  inline bool has(uint64 id) {
    return _constructors.has(id);
  }
  
private:
  
  struct Constructor {
    Persistent<FunctionTemplate>  function_template;
    Persistent<Value>             prototype;
    NativeAllocator               allocator;
  };
  Map<uint64, Constructor> _constructors;
  WrapBase*                _attach_object;
  
  friend class WrapBase;
};

/**
 * @class Worker
 */
class Worker: public Object {
  av_hidden_all_copy(Worker);
  
  /**
   * @constructor
   */
  Worker(const Array<String>& args);
  
public:
  
  typedef NativeConstructors::NativeAllocator NativeAllocator;
  typedef void (*Binding)(Local<v8::Object> exports, Worker* worker);
  
  /**
   * @destructor
   */
  virtual ~Worker();
  
  /**
   * @func initializ
   */
  static void initializ();
  
  /**
   * @func dispose
   */
  static void dispose();
  
  /**
   * @func create
   */
  static Worker* create(const Array<String>& args = Array<String>()) av_def_err;
  
  /**
   * @func current
   */
  static Worker* current();
  
  /**
   * @func worker
   */
  static Worker* worker(Isolate* isolate);
  
  /**
   * @func worker
   */
  static Worker* worker(FunctionCall args);
  
  /**
   * @func worker
   */
  static Worker* worker(PropertyCall args);
  
  /**
   * @func worker
   */
  static Worker* worker(PropertySetCall args);
  
  /**
   * @func reg_module
   */
  static void reg_module(cString& name, Binding binding);
  
  /**
   * @func del_module
   */
  static void del_module(cString& name);
  
  /**
   * @func New
   */
  Local<v8::Value> New(float data);
  Local<v8::Value> New(double data);
  Local<v8::Value> New(bool data);
  Local<v8::Value> New(char data);
  Local<v8::Value> New(byte data);
  Local<v8::Value> New(int16 data);
  Local<v8::Value> New(uint16 data);
  Local<v8::Value> New(int data);
  Local<v8::Value> New(uint data);
  Local<v8::Value> New(int64 data);
  Local<v8::Value> New(uint64 data);
  Local<v8::Value> New(cchar* data);
  Local<v8::Value> New(cString& data);
  Local<v8::Value> New(cUcs2String& data);
  Local<v8::Value> New(cError& data);
  Local<v8::Value> New(const HttpError& err);
  Local<v8::Value> New(const Array<String>& data);
  Local<v8::Value> New(Array<FileStat>& data);
  Local<v8::Value> New(Array<FileStat>&& data);
  Local<v8::Value> New(const Map<String, String>& data);
  Local<v8::Value> New(Buffer& buff);
  Local<v8::Value> New(Buffer&& buff);
  Local<v8::Value> New(FileStat& stat);
  Local<v8::Value> New(FileStat&& stat);
  Local<v8::Value> New(const Dirent& dir);
  Local<v8::Value> New(Array<Dirent>& data);
  Local<v8::Value> New(Array<Dirent>&& data);
  Local<v8::Value> New(Local<v8::Value> value);
  Local<v8::Object>NewNative(uint64 id, uint argc = 0, Local<v8::Value>* argv = NULL);
  Local<v8::String>NewStringAscii(cString& ascii);
  Local<v8::String>NewString(cBuffer& data);
  Local<v8::Value> NewBuffer(Local<v8::String> str, Encoding enc = Encoding::utf8);
  Local<v8::Value> NewRangeError(cchar* errmsg, ...);
  Local<v8::Value> NewReferenceError(cchar* errmsg, ...);
  Local<v8::Value> NewSyntaxError(cchar* errmsg, ...);
  Local<v8::Value> NewTypeError(cchar* errmsg, ...);
  Local<v8::Value> NewError(cchar* errmsg, ...);
  Local<v8::Value> NewError(cError& err);
  Local<v8::Value> NewError(const HttpError& err);
  Local<v8::Value> NewError(Local<v8::Object> value);
  Local<v8::Value> NewObject();
  Local<v8::Value> NewArray();
  Local<v8::Value> Null();
  Local<v8::Value> Undefined();
  // Number
  inline Local<v8::Value> New(const Bool& v) { return New(v.value); }
  inline Local<v8::Value> New(const Float& v) { return New(v.value); }
  inline Local<v8::Value> New(const Double& v) { return New(v.value); }
  inline Local<v8::Value> New(const Char& v) { return New(v.value); }
  inline Local<v8::Value> New(const Byte& v) { return New(v.value); }
  inline Local<v8::Value> New(const Int16& v) { return New(v.value); }
  inline Local<v8::Value> New(const Uint16& v) { return New(v.value); }
  inline Local<v8::Value> New(const Int& v) { return New(v.value); }
  inline Local<v8::Value> New(const Uint& v) { return New(v.value); }
  inline Local<v8::Value> New(const Int64& v) { return New(v.value); }
  inline Local<v8::Value> New(const Uint64& v) { return New(v.value); }
  // static_cast
  template<class T> inline static Local<v8::Value> cast(const Object& v, Worker* w) {
    return w->New( static_cast<const T*>(&v) );
  }
  
  /**
   * @func throw_err
   */
  void throw_err(Local<v8::Value> exception);
  void throw_err(cchar* errmsg, ...);
  
  /**
   * @func to_string_ascii
   */
  String to_string_ascii(v8::Local<v8::Value> ascii);
  String to_string_utf8(v8::Local<v8::Value> uft8);
  Ucs2String to_string_ucs2(v8::Local<v8::Value> value);
  Map<String, int> to_map_int(v8::Local<v8::Value> value);
  Map<String, String> to_map_string(v8::Local<v8::Value> value);
  Array<String> to_arr_string(v8::Local<v8::Value> value);
  
  /**
   * @func has_buffer
   */
  bool has_buffer(Local<v8::Value> val);
  
  /**
   * @func has_view
   */
  bool has_view(Local<v8::Value> val);
  
  /**
   * @func has
   */
  inline bool has(Local<Value> val, uint64 id) {
    return _constructors->instanceof(val, id);
  }
  
  /**
   * @func has_native
   */
  template<class T> inline bool has(Local<v8::Value> val) {
    return _constructors->instanceof(val, av8_typeid(T));
  }
  
  /**
   * @func equals
   */
  template <class T>
  bool equals(Local<v8::Value> val1, const T& data) {
    return val1->Equals( New(data) );
  }
  
  /**
   * @func equals_ascii
   */
  bool equals_ascii(Local<v8::Value> val1, cString& ascii);
  
  /**
   * @func local
   */
  template <class T, class M = NonCopyablePersistentTraits<T>>
  inline Local<T> local(const Persistent<T, M>& persistent) {
    if (persistent.IsWeak()) {
      return Local<T>::New(_isolate, persistent);
    } else {
      return strong(persistent);
    }
  }
  
  /**
   * @func strong
   */
  template <class T, class M = NonCopyablePersistentTraits<T>>
  inline Local<T> strong(const Persistent<T, M>& persistent) {
    return *reinterpret_cast<Local<T>*>(const_cast<Persistent<T, M>*>(&persistent));
  }
  
  /**
   * @func result
   */
  template <class Args, class T>
  inline void result(const Args& args, Local<T> data) {
    args.GetReturnValue().Set( data.template As<Value>() );
  }
  
  /**
   * @func result
   */
  template <class Args, class T>
  inline void result(const Args& args, const T& data) {
    args.GetReturnValue().Set( New(data) );
  }
  
  /**
   * @func result
   */
  template <class Args, class T>
  inline void result(const Args& args, T&& data) {
    args.GetReturnValue().Set( New(move(data)) );
  }
  
  /**
   * @get isolate
   */
  inline Isolate* isolate() { return _isolate; }
  
  /**
   * @func context
   */
  inline Local<v8::Context> context() { return _context; }
  
  /**
   * @func def_class
   */
  Local<FunctionTemplate> def_class(cString& name, FunctionCallback constructor,
                                    Local<FunctionTemplate> base = Local<FunctionTemplate>());
  /**
   * @func def_class
   */
  Local<FunctionTemplate> def_class(cString& name, FunctionCallback constructor, uint64 base);
  
  /**
   * @func def_class_end
   */
  void def_class_end(cString& name, uint64 id,
                     Local<FunctionTemplate> func, NativeAllocator allocator,
                     Local<v8::Object> exports = v8::Local<v8::Object>());
  /**
   * @func set_cls_method
   */
  void set_cls_method(Local<FunctionTemplate> cls, cString& name, FunctionCallback func);
  
  /**
   * @func set_cls_property
   */
  void set_cls_property(Local<FunctionTemplate> cls,
                        cString& name,
                        AccessorGetterCallback get, AccessorSetterCallback set = NULL);
  /**
   * @func set_cls_indexed
   */
  void set_cls_indexed(Local<FunctionTemplate> cls,
                       IndexedPropertyGetterCallback get, IndexedPropertySetterCallback set = NULL);
  /**
   * @func set_method
   */
  void set_method(Local<Template> exports, cString& name, FunctionCallback func);
  
  /**
   * @func set_method
   */
  void set_method(Local<v8::Object> exports, cString& name, FunctionCallback func);
  
  /**
   * @func set_property
   */
  void set_property(Local<v8::Object> exports, cString& name,
                    AccessorGetterCallback get, AccessorSetterCallback set = NULL);
  
  /**
   * @func set_field
   */
  template<class T, class T2>
  void set_field(Local<T> exports, cString& name, T2 value) {
    exports->Set(NewStringAscii(name), New(value));
  }
  
  /**
   * @func set_cls_field
   */
  template<class T>
  void set_cls_field(Local<FunctionTemplate> cls, cString& name, T value) {
    cls->PrototypeTemplate()->Set(NewStringAscii(name), New(value));
  }
  
  /**
   * @func binding
   */
  Local<v8::Object> binding(cString& name);
  
  /**
   * @func binding
   */
  Local<v8::Object> binding(Local<Value> name);
  
  /**
   * @func run_script
   */
  MaybeLocal<Value> run_script(cString& source,
                               cString& name,
                               Local<v8::Object> sandbox = Local<v8::Object>());
  /**
   * @func run_script
   */
  MaybeLocal<Value> run_script(Local<v8::String> source,
                               Local<v8::String> name,
                               Local<v8::Object> sandbox = Local<v8::Object>());
  /**
   * @func run_native_script
   */
  bool run_native_script(Local<v8::Object> exports,
                         cchar* source, uint source_len, cString& name);
  /**
   * @func get_data
   */
  Object* get_data(int id);
  
  /**
   * @func add_extend_object
   */
  void set_data(int id, Object* obj);
  
  /**
   * @func gui_value_program
   */
  inline ValueProgram* gui_value_program() { return _gui_value_program; }
  
  /**
   * @func set_gui_value_program
   */
  void set_gui_value_program(ValueProgram* value) av_def_err;
  
  /**
   * @func strs
   */
  inline CommonString* strs() { return _common_string; }
  
  /**
   * @func global
   */
  inline Local<v8::Object> global() { return _global; }
  
  /**
   * @func native_constructors
   */
  inline NativeConstructors* native_constructors() { return _constructors; }
  
  /**
   * @func args
   */
  inline const Array<String>& args() const { return _args; }
  
  /**
   * @func debug
   */
  inline bool debug() const { return _debug; }
  
  /**
   * @func report_exception
   */
  void report_exception(v8::TryCatch* try_catch);
  
  /**
   * @func call
   */
  MaybeLocal<v8::Value> call(Local<v8::Function> func,
                             Local<v8::Value> recv = Local<v8::Value>(),
                             int argc = 0, v8::Local<v8::Value> argv[] = NULL);
  MaybeLocal<v8::Value> call(PersistentFunc func,
                             Local<v8::Value> recv = Local<v8::Value>(),
                             int argc = 0, v8::Local<v8::Value> argv[] = NULL);
  /**
   * @func call_constructor
   */
  MaybeLocal<v8::Value> call_constructor(Local<v8::Function> func,
                             int argc = 0, v8::Local<v8::Value> argv[] = NULL);
  MaybeLocal<v8::Value> call_constructor(PersistentFunc func,
                                         int argc = 0, v8::Local<v8::Value> argv[] = NULL);
  
  /**
   * @func fatal exit worker
   */
  void fatal(cString& msg);
  
private:
  
  Array<String>             _args;
  bool                      _debug;
  Isolate*                  _isolate;
  Locker*                   _locker;
  void*                     _handle_scope;
  Local<v8::Context>        _context;
  Local<v8::Object>         _global;
  Local<v8::Object>         _util;
  NativeConstructors*       _constructors;
  Persistent<v8::Object>    _binding;
  Map<int, Object*>         _data;
  ValueProgram*             _gui_value_program;
  CommonString*             _common_string;
  ThreadID                  _thread_id;
  
  av_def_inl_cls(_Inl);
};

template<class T> class Wrap;

/**
 * @class WrapBase
 */
class WrapBase {
  av_hidden_all_copy(WrapBase);
  
  /**
   * @func initializ2
   */
  void initializ2(FunctionCall args);

  protected:
  
  /**
   * @constructor
   */
  inline WrapBase(): _worker(nullptr), _data(nullptr) { }
  
  /**
   * @func New
   */
  template<class T, class T2>
  static Wrap<T2>* New(FunctionCall av8_args, T2* ptr) {
    static_assert(sizeof(T) == sizeof(WrapBase),
                  "Derived wrap class pairs cannot declare data members");
    static_assert(T2::Characteristic::is_object, "Must be object");
    auto wrap = new(reinterpret_cast<WrapBase*>(ptr) - 1) T();
    wrap->initializ2(av8_args);
    return static_cast<Wrap<T2>*>(static_cast<WrapBase*>(wrap));
  }
  
  /**
   * @func External
   */
  static WrapBase* External(FunctionCall av8_args);
  
  /**
   * @destructor
   */
  virtual ~WrapBase();
  
  public:
  
  /**
   * @func initializ
   */
  virtual void initializ() { }
  
  class Data {
  public:
    virtual ~Data() { }
    virtual void release() { delete this; }
  };
  
  inline Worker* worker() {
    return _worker;
  }
  
  /**
   * @func add_event_listener
   */
  virtual bool add_event_listener(cString& name, cString& func, int id) {
    // subclass implementation
    return false;
  }
  
  /**
   * @func remove_event_listener
   */
  virtual bool remove_event_listener(cString& name, int id) {
    // subclass implementation
    return false;
  }
  
  /**
   * @func get_data
   */
  inline Data* get_data() { return _data; }
  
  /**
   * @func set_data
   */
  void  set_data(Data* data);
  
  /**
   * @func handle
   */
  inline Persistent<v8::Object>& handle() {
    return _handle;
  }
  
  /**
   * @func local
   */
  inline Local<v8::Object> local() {
    return _worker->local(_handle);
  }

  /**
   * @func Get
   */
  inline Local<v8::Value> Get(Local<v8::Value> key) {
    return local()->Get(_worker->context(), key).FromMaybe(Local<v8::Value>());
  }
  
  /**
   * @func Set
   */
  inline bool Set(Local<v8::Value> key, Local<v8::Value> value) {
    return local()->Set(_worker->context(), key, value).IsJust();
  }
  
  /**
   * @func make_weak
   */
  void make_weak();
  
  /**
   * @func self
   */
  Object* self() {
    return reinterpret_cast<Object*>(this + 1);
  }
  
  template<class T> T* self() {
    return static_cast<T*>(reinterpret_cast<Object*>(this + 1));
  }
  
  /**
   * @func call
   */
  MaybeLocal<v8::Value> call(Local<Value> name, int argc = 0, v8::Local<v8::Value> argv[] = nullptr);
  
  /**
   * @func call
   */
  MaybeLocal<v8::Value> call(cString& name, int argc = 0, v8::Local<v8::Value> argv[] = nullptr);
  
  /**
   * @func is_wrap
   */
  static inline bool is_wrap(Local<v8::Object> av8_object) {
    av_assert(!av8_object.IsEmpty());
    return av8_object->InternalFieldCount() > 0 && av8_object->GetAlignedPointerFromInternalField(0);
  }
  
  /**
   * @func unwrap
   */
  static inline WrapBase* unwrap(Local<v8::Object> av8_object) {
    av_assert(!av8_object.IsEmpty());
    av_assert(av8_object->InternalFieldCount() > 0);
    return static_cast<WrapBase*>(av8_object->GetAlignedPointerFromInternalField(0));
  }
  static inline WrapBase* unwrap(FunctionCall args) { return unwrap(args.Holder()); }
  static inline WrapBase* unwrap(PropertyCall args) { return unwrap(args.This()); }
  static inline WrapBase* unwrap(PropertySetCall args) { return unwrap(args.This()); }
  static inline WrapBase* unwrap(Local<v8::Value> value) { return unwrap(value->ToObject()); }
  
  /**
   * @func wrap
   */
  static WrapBase* wrap(Object* object, uint64 type_id);
  
protected:
  
  Worker*                 _worker;
  Data*                   _data;
  Persistent<v8::Object>  _handle;
  
  av_def_inl_cls(Inl);
  
  friend class Allocator;
};

/**
 * @class Wrap utils
 */
template<class T> class Wrap: public WrapBase {
  Wrap() = delete;
public:
  typedef T Type;
  
  template<class T2> static Wrap* unwrap(T2 av8_object) {
    return static_cast<Wrap*>(WrapBase::unwrap(av8_object));
  }
  template<class T2> static T* Self(T2 av8_object) {
    return static_cast<Wrap*>(WrapBase::unwrap(av8_object))->self();
  }
  inline static Wrap* wrap(T* object, uint64 id) {
    return static_cast<Wrap*>(WrapBase::wrap(object, id));
  }
  inline static Wrap* wrap(T* object) {
    return static_cast<Wrap*>(WrapBase::wrap(object, av8_typeid(*object)));
  }
  inline T* self() {
    return reinterpret_cast<T*>(this + 1);
  }
  inline T* operator*() { return self(); }
  inline T* operator->() { return self(); }
  
};

/**
 * @class V8ExternalOneByteStringResource
 */
class V8ExternalOneByteStringResource: public v8::String::ExternalOneByteStringResource {
  String _str;
public:
  V8ExternalOneByteStringResource(cString& value): _str(value) { }
  virtual cchar* data() const { return *_str; }
  virtual size_t length() const { return _str.length(); }
};

/**
 * @class V8ExternalStringResource
 */
class V8ExternalStringResource: public v8::String::ExternalStringResource {
  Ucs2String _str;
public:
  V8ExternalStringResource(const Ucs2String& value): _str(value) { }
  virtual const uint16* data() const { return *_str; }
  virtual size_t length() const { return _str.length(); }
};

/**
 * @func start
 */
int start(int argc, char* argv[]);

/**
 * @func start
 */
int start(cString& args);

av8_end
#endif
