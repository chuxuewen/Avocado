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

#include "autil/http.h"
#include "./av8.h"
#include "./fs-1.h"

/**
 * @ns avocado::av8
 */

av8_begin

static cString const_url("url");
static cString const_method("method");
static cString const_headers("headers");
static cString const_post_data("post_data");
static cString const_save("save");
static cString const_upload("upload");
static cString const_disable_ssl_verify("disable_ssl_verify");
static cString const_disable_cache("disable_cache");
static cString const_disable_cookie("disable_cookie");

typedef HttpClientRequest NativeHttpClientRequest;

/**
 * @class WrapNativeHttpClientRequest
 */
class WrapNativeHttpClientRequest: public WrapBase {
  
public: typedef HttpClientRequest Type;
  
  class Delegate: public Data, public HttpClientRequest::Delegate {
  public:
  
    String trigger_error;
    String trigger_write;
    String trigger_header;
    String trigger_data;
    String trigger_end;
    String trigger_readystate_change;
    String trigger_timeout;
    String trigger_abort;
    
    WrapNativeHttpClientRequest* host() {
      return _host;
    }
    
    Worker* worker() {
      return _host->worker();
    }
    
    virtual void trigger_http_error(HttpClientRequest* req, cError& error) {
      if ( !trigger_error.is_empty() ) {
        v8::HandleScope scope(worker()->isolate());
        Local<v8::Value> arg = worker()->New( error );
        host()->call( worker()->NewStringAscii(trigger_error), 1, &arg );
      }
    }
    virtual void trigger_http_write(HttpClientRequest* req) {
      if ( !trigger_write.is_empty() ) {
        host()->call( worker()->NewStringAscii(trigger_write) );
      }
    }
    virtual void trigger_http_header(HttpClientRequest* req) {
      if ( !trigger_header.is_empty() ) {
        host()->call( worker()->NewStringAscii(trigger_header) );
      }
    }
    virtual void trigger_http_data(HttpClientRequest* req, Buffer buffer) {
      if ( !trigger_data.is_empty() ) {
        v8::HandleScope scope(host()->worker()->isolate());
        Local<Value> arg = worker()->New( move(buffer) );
        host()->call( worker()->NewStringAscii(trigger_data), 1, &arg );
      }
    }
    virtual void trigger_http_end(HttpClientRequest* req) {
      if ( !trigger_end.is_empty() ) {
        host()->call( worker()->NewStringAscii(trigger_end) );
      }
    }
    virtual void trigger_http_readystate_change(HttpClientRequest* req) {
      if ( !trigger_readystate_change.is_empty() ) {
        host()->call( worker()->NewStringAscii(trigger_readystate_change) );
      }
    }
    virtual void trigger_http_timeout(HttpClientRequest* req) {
      if ( !trigger_timeout.is_empty() ) {
        host()->call( worker()->NewStringAscii(trigger_timeout) );
      }
    }
    virtual void trigger_http_abort(HttpClientRequest* req) {
      if ( !trigger_abort.is_empty() ) {
        host()->call( worker()->NewStringAscii(trigger_abort) );
      }
    }
    
    WrapNativeHttpClientRequest* _host;
    
  };
  
  Delegate* del() {
    return static_cast<Delegate*>(get_data());
  }
  
  virtual bool add_event_listener(cString& name, cString& func, int id) {
    
    Delegate* _del = del();
    
    if ( !_del ) {
      _del = new Delegate();
      _del->_host = this;
      self<Type>()->set_delegate(_del);
      set_data(_del);
    }
    
    if ( id != -1 ) return 0;
    
    if ( name == "error" ) {
      _del->trigger_error = func;
    } else if ( name == "write" ) {
      _del->trigger_write = func;
    } else if ( name == "header" ) {
      _del->trigger_header = func;
    } else if ( name == "data" ) {
      _del->trigger_data = func;
    } else if ( name == "end" ) {
      _del->trigger_end = func;
    } else if ( name == "readystate_change" ) {
      _del->trigger_readystate_change = func;
    } else if ( name == "timeout" ) {
      _del->trigger_timeout = func;
    } else if ( name == "abort" ) {
      _del->trigger_abort = func;
    } else {
      return false;
    }
    return true;
  }
  
  virtual bool remove_event_listener(cString& name, int id) {
    
    Delegate* _del = del();
    
    if ( id != -1 || !_del ) return 0;
    
    if ( name == "error" ) {
      _del->trigger_error = Ucs2String::empty;
    } else if ( name == "write" ) {
      _del->trigger_write = Ucs2String::empty;
    } else if ( name == "header" ) {
      _del->trigger_header = Ucs2String::empty;
    } else if ( name == "data" ) {
      _del->trigger_data = Ucs2String::empty;
    } else if ( name == "end" ) {
      _del->trigger_end = Ucs2String::empty;
    } else if ( name == "readystate_change" ) {
      _del->trigger_readystate_change = Ucs2String::empty;
    } else if ( name == "timeout" ) {
      _del->trigger_timeout = Ucs2String::empty;
    } else if ( name == "abort" ) {
      _del->trigger_abort = Ucs2String::empty;
    } else {
      return false;
    }
    return true;
  }
  
  static void constructor(FunctionCall args) {
    New<WrapNativeHttpClientRequest>(args, new HttpClientRequest());
  }
  
  /**
   * @func set_method(method)
   * @arg method {HttpMethod}
   */
  static void set_method(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsUint32()) {
      av8_throw_err(
        "* @func set_method(method)\n"
        "* @arg method {HttpMethod}\n"
      );
    }
    uint32 arg = args[0]->ToUint32()->Value();
    HttpMethod method = arg > 4 ? HTTP_METHOD_GET: (HttpMethod)arg;
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_method(method); }, Error);
  }
  
  /**
   * @func set_url(url)
   * @arg url {String}
   */
  static void set_url(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func set_url(url)\n"
        "* @arg url {String}\n"
      );
    }
    String arg = worker->to_string_utf8(args[0]);
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_url(arg); }, Error);
  }
  
  /**
   * @func set_save_path(path)
   * @arg path {String}
   */
  static void set_save_path(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func set_save_path(path)\n"
        "* @arg path {String}\n"
      );
    }
    String arg = worker->to_string_utf8(args[0]);
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_save_path(arg); }, Error);
  }
  
  /**
   * @func set_username(username)
   * @arg username {String}
   */
  static void set_username(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func set_username(username)\n"
        "* @arg username {String}\n"
      );
    }
    String arg = worker->to_string_utf8(args[0]);
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_username(arg); }, Error);
  }
  
  /**
   * @func set_password(password)
   * @arg password {String}
   */
  static void set_password(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      av8_throw_err(
        "* @func set_password(password)\n"
        "* @arg password {String}\n"
      );
    }
    String arg = worker->to_string_utf8(args[0]);
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_password(arg); }, Error);
  }
  
  /**
   * @func disable_cache(disable)
   * @arg disable {bool}
   */
  static void disable_cache(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func disable_cache(disable)\n"
        "* @arg disable {bool}\n"
      );
    }
    uint32 arg = args[0]->ToBoolean()->Value();
    av8_self(HttpClientRequest);
    av8_try_catch({ self->disable_cache(arg); }, Error);
  }
  
  /**
   * @func disable_cookie(disable)
   * @arg disable {bool}
   */
  static void disable_cookie(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func disable_cookie(disable)\n"
        "* @arg disable {bool}\n"
      );
    }
    uint32 arg = args[0]->ToBoolean()->Value();
    av8_self(HttpClientRequest);
    av8_try_catch({ self->disable_cookie(arg); }, Error);
  }
  
  /**
   * @func disable_send_cookie(disable)
   * @arg disable {bool}
   */
  static void disable_send_cookie(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func disable_send_cookie(disable)\n"
        "* @arg disable {bool}\n"
      );
    }
    uint32 arg = args[0]->ToBoolean()->Value();
    av8_self(HttpClientRequest);
    av8_try_catch({ self->disable_send_cookie(arg); }, Error);
  }
  
  /**
   * @func disable_ssl_verify(disable)
   * @arg disable {bool}
   */
  static void disable_ssl_verify(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func disable_ssl_verify(disable)\n"
        "* @arg disable {bool}\n"
      );
    }
    av8_self(HttpClientRequest);
    av8_try_catch({ self->disable_ssl_verify(args[0]->ToBoolean()->Value()); }, Error);
  }
  
  /**
   * @func set_request_header(header_name, value)
   * @arg header_name {String} ascii string
   * @arg value {String}
   */
  static void set_request_header(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 && !args[0]->IsString() && !args[1]->IsString()) {
      av8_throw_err(
        "* @func set_request_header(header_name, value)\n"
        "* @arg header_name {String} ascii string\n"
        "* @arg value {String}\n"
      );
    }
    av8_self(HttpClientRequest);
    av8_try_catch({
      self->set_request_header(worker->to_string_ascii(args[0]), worker->to_string_utf8(args[1]));
    }, Error);
    av8_return( args.Holder() );
  }
  
  /**
   * @func set_form(form_name, value)
   * @arg form_name {String}
   * @arg value {String}
   */
  static void set_form(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString() ) {
      av8_throw_err(
        "* @func set_form(form_name, value)\n"
        "* @arg form_name {String}\n"
        "* @arg value {String}\n"
      );
    }
    String form_name = worker->to_string_utf8(args[0]);
    String value = worker->to_string_utf8(args[1]);
    av8_self(HttpClientRequest);
    av8_try_catch({
      self->set_form(form_name, value);
    }, Error);
  }
  
  /**
   * @func set_upload_file(form_name, local_path)
   * @arg form_name {String}
   * @arg local_path {String}
   */
  static void set_upload_file(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString() ) {
      av8_throw_err(
        "* @func set_upload_file(form_name, local_path)\n"
        "* @arg form_name {String}\n"
        "* @arg local_path {String}\n"
      );
    }
    String form_name = worker->to_string_utf8(args[0]);
    String local_path = worker->to_string_utf8(args[1]);
    av8_self(HttpClientRequest);
    av8_try_catch({
      self->set_upload_file(form_name, local_path);
    }, Error);
  }
  
  /**
   * @func clear_request_header()
   */
  static void clear_request_header(FunctionCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_try_catch({ self->clear_request_header(); }, Error);
  }
  
  /**
   * @func clear_form_data()
   */
  static void clear_form_data(FunctionCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_try_catch({ self->clear_form_data(); }, Error);
  }
  
  /**
   * @func get_response_header(header_name)
   * @arg header_name {String}
   * @ret {String}
   */
  static void get_response_header(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 && !args[0]->IsString()) {
      av8_throw_err(
        "* @func get_response_header(header_name)\n"
        "* @arg header_name {String}\n"
        "* @ret {String}\n"
      );
    }
    av8_self(HttpClientRequest);
    String rv;
    av8_try_catch({ rv = self->get_response_header( worker->to_string_ascii(args[0]) ); }, Error);
    av8_return( rv );
  }
  
  /**
   * @func get_all_response_headers()
   * @ret {Object}
   */
  static void get_all_response_headers(FunctionCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    const Map<String, String>* rv;
    av8_try_catch({ rv = &self->get_all_response_headers(); }, Error);
    av8_return( *rv );
  }
  
  /**
   * @func set_keep_alive(keep_alive)
   * @arg keep_alive {bool}
   */
  static void set_keep_alive(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0) {
      av8_throw_err(
        "* @func set_keep_alive(keep_alive)\n"
        "* @arg keep_alive {bool}\n"
      );
    }
    bool enable = args[0]->ToBoolean()->Value();
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_keep_alive(enable); }, Error);
  }

  /**
   * @func set_timeout(time)
   * @arg time {uint} ms
   */
  static void set_timeout(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsNumber()) {
      av8_throw_err(
        "* @func set_timeout(time)\n"
        "* @arg time {uint} ms\n"
      );
    }
    av8_self(HttpClientRequest);
    av8_try_catch({ self->set_timeout(args[0]->ToUint32()->Value()); }, Error);
  }

  /**
   * @get upload_total {uint}
   */
  static void upload_total(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->upload_total() );
  }

  /**
   * @get upload_size {uint}
   */
  static void upload_size(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->upload_size() );
  }

  /**
   * @get download_total {uint}
   */
  static void download_total(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->download_total() );
  }

  /**
   * @get download_size {uint}
   */
  static void download_size(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->download_size() );
  }

  /**
   * @get download_size {HttpReadyState}
   */
  static void ready_state(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->ready_state() );
  }

  /**
   * @get status_code {int}
   */
  static void status_code(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->status_code() );
  }

  /**
   * @get url {String}
   */
  static void url(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    av8_return( self->url() );
  }

  /**
   * @func send([data])
   * @arg [data] {String|ArrayBuffer|Buffer}
   */
  static void send(FunctionCall args) {
    av8_worker(args);
    av8_self(HttpClientRequest);
    if (args.Length() == 0) {
      av8_try_catch({ self->send(); }, Error);
    } else {
      if (args[0]->IsString()) {
        av8_try_catch({
          self->send( worker->to_string_utf8(args[0]).collapse_buffer() );
        }, Error);
      } else if (args[0]->IsArrayBuffer()) {
        v8::ArrayBuffer::Contents con = args[0].As<v8::ArrayBuffer>()->GetContents();
        WeakBuffer buff((cchar*)con.Data(), (uint)con.ByteLength());
        av8_try_catch({ self->send( buff.copy() ); }, Error);
      } else if (worker->has_buffer(args[0])) {
        Buffer* buff = Wrap<Buffer>::Self(args[0]);
        av8_try_catch({ self->send( *buff ); }, Error);
      } else {
        av8_try_catch({ self->send(); }, Error );
      }
    }
  }

  /**
   * @func pause()
   */
  static void pause(FunctionCall args) {
    av8_self(HttpClientRequest);
    self->pause();
  }

  /**
   * @func resume()
   */
  static void resume(FunctionCall args) {
    av8_self(HttpClientRequest);
    self->resume();
  }

  /**
   * @func abort()
   */
  static void abort(FunctionCall args) {
    av8_self(HttpClientRequest);
    self->abort();
  }
  
  /**
   * @func binding
   */
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeHttpClientRequest, constructor, {
      av8_set_cls_method(set_method, set_method);
      av8_set_cls_method(set_url, set_url);
      av8_set_cls_method(set_save_path, set_save_path);
      av8_set_cls_method(set_username, set_username);
      av8_set_cls_method(set_password, set_password);
      av8_set_cls_method(disable_cache, disable_cache);
      av8_set_cls_method(disable_cookie, disable_cookie);
      av8_set_cls_method(disable_send_cookie, disable_send_cookie);
      av8_set_cls_method(disable_ssl_verify, disable_ssl_verify);
      av8_set_cls_method(set_keep_alive, set_keep_alive);
      av8_set_cls_method(set_timeout, set_timeout);
      av8_set_cls_method(set_request_header, set_request_header);
      av8_set_cls_method(set_form, set_form);
      av8_set_cls_method(set_upload_file, set_upload_file);
      av8_set_cls_method(clear_request_header, clear_request_header);
      av8_set_cls_method(clear_form_data, clear_form_data);
      av8_set_cls_method(get_response_header, get_response_header);
      av8_set_cls_method(get_all_response_headers, get_all_response_headers);
      av8_set_cls_property(upload_total, upload_total);
      av8_set_cls_property(upload_size, upload_size);
      av8_set_cls_property(download_total, download_total);
      av8_set_cls_property(download_size, download_size);
      av8_set_cls_property(ready_state, ready_state);
      av8_set_cls_property(status_code, status_code);
      av8_set_cls_property(url, url);
      av8_set_cls_method(send, send);
      av8_set_cls_method(pause, pause);
      av8_set_cls_method(resume, resume);
      av8_set_cls_method(abort, abort);
    }, nullptr);
  }
};

/**
 * @class NativeHttp
 */
class NativeHttp {
public:

  static HttpHelper::RequestOptions get_options(Worker* worker, Local<Value> arg) {
    Local<v8::Object> obj = arg.As<v8::Object>();
    
    HttpHelper::RequestOptions opt = {
      String::empty,
      HTTP_METHOD_GET,
      Map<String, String>(),
      Buffer(),
      String::empty,
      String::empty,
      false,
      false,
      false,
    };
    Local<Value> value;

    if ( obj->Get(worker->context(), worker->NewStringAscii(const_url)).ToLocal(&value) ) {
      if ( value->IsString() ) opt.url = worker->to_string_utf8(value);
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_method)).ToLocal(&value) ) {
      if ( value->IsUint32() ) {
        uint32 arg = value->ToUint32()->Value();
        opt.method = arg > 4 ? HTTP_METHOD_GET: (HttpMethod)arg;
      }
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_headers)).ToLocal(&value) ) {
      opt.headers = worker->to_map_string(value);
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_post_data)).ToLocal(&value) ) {
      if ( value->IsString() ) {
        opt.post_data = worker->to_string_utf8(value).collapse_buffer();
      } else if (value->IsArrayBuffer()) {
        v8::ArrayBuffer::Contents con = value.As<v8::ArrayBuffer>()->GetContents();
        opt.post_data = WeakBuffer((cchar*)con.Data(), (uint)con.ByteLength()).copy();
      } else if (worker->has_buffer(value)) {
        opt.post_data = *Wrap<Buffer>::Self(value);
      }
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_save)).ToLocal(&value) ) {
      if ( value->IsString() ) {
        opt.save = worker->to_string_utf8(value);
      }
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_upload)).ToLocal(&value) ) {
      if ( value->IsString() ) {
        opt.upload = worker->to_string_utf8(value);
      }
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_disable_ssl_verify)).ToLocal(&value) ) {
      opt.disable_ssl_verify = value->ToBoolean()->Value();
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_disable_cache)).ToLocal(&value) ) {
      opt.disable_cache = value->ToBoolean()->Value();
    }
    if ( obj->Get(worker->context(), worker->NewStringAscii(const_disable_cookie)).ToLocal(&value) ) {
      opt.disable_cookie = value->ToBoolean()->Value();
    }
    
    return opt;
  }

  template<bool stream> static void request(FunctionCall args, cchar* argument) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsObject()) {
      av8_throw_err(argument);
    }
    av8_handle_scope();
    uint rev = 0;
    auto opt = get_options(worker, args[0]);
    Callback cb;
    
    if ( args.Length() > 1 ) {
      cb = stream ? get_callback_for_io_stream_http_error(worker, args[1]) :
                    get_callback_for_buffer_http_error(worker, args[1]);
    }

    av8_try_catch({
      if ( stream ) {
        rev = HttpHelper::request_stream(opt, cb);
      } else {
        rev = HttpHelper::request(opt, cb);
      }
    }, HttpError);
    
    av8_return( rev );
  }
  
  /**
   * @func request(options[,cb])
   * @arg options {RequestOptions}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void request(FunctionCall args) {
    request<false>(args,
      "* @func request(options[,cb])\n"
      "* @arg options {RequestOptions}\n"
      "* @arg [cb] {Function}\n"
      "* @ret {uint} return req id\n"
    );
  }
  
  /**
   * @func request_stream(options[,cb])
   * @arg options {RequestOptions}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void request_stream(FunctionCall args) {
    request<true>(args, 
      "* @func request_stream(options[,cb])\n"
      "* @arg options {RequestOptions}\n"
      "* @arg [cb] {Function}\n"
      "* @ret {uint} return req id\n"
    );
  }
  
  /**
   * @func request_sync(url)
   * @arg url {String}
   * @ret {Buffer}
   */
  static void request_sync(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsObject()) {
      av8_throw_err(
        "* @func request_sync(url)\n"
        "* @arg url {String}\n"
        "* @ret {Buffer}\n"
      );
    }
    auto opt = get_options(worker, args[0]);
    av8_try_catch({
      av8_return( HttpHelper::request_sync(opt) );
    }, HttpError);
  }
  
  /**
   * @func download(url,save[,cb])
   * @arg url {String}
   * @arg save {String}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void download(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      av8_throw_err(
        "* @func download(url,save[,cb])\n"
        "* @arg url {String}\n"
        "* @arg save {String}\n"
        "* @arg [cb] {Function}\n"
        "* @ret {uint} return req id\n"
      );
    }
    uint rev = 0;
    String url = worker->to_string_utf8(args[0]);
    String save = worker->to_string_utf8(args[1]);
    Callback cb;
    
    if ( args.Length() > 2 ) {
      cb = get_callback_for_buffer_http_error(worker, args[2]);
    }
    av8_try_catch({
      rev = HttpHelper::download(url, save, cb);
    }, HttpError);
    av8_return( rev );
  }
  
  /**
   * @func download_sync(url,save)
   * @arg url {String}
   * @arg save {String}
   */
  static void download_sync(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      av8_throw_err(
        "* @func download_sync(url,save)\n"
        "* @arg url {String}\n"
        "* @arg save {String}\n"
      );
    }
    String url = worker->to_string_utf8(args[0]);
    String save = worker->to_string_utf8(args[1]);
    av8_try_catch({
      HttpHelper::download_sync(url, save);
    }, HttpError);
  }
    
  /**
   * @func upload(url,local_path[,cb])
   * @arg url {String}
   * @arg local_path {String}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void upload(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      av8_throw_err(
        "* @func upload(url,local_path[,cb])\n"
        "* @arg url {String}\n"
        "* @arg local_path {String}\n"
        "* @arg [cb] {Function}\n"
        "* @ret {uint} return req id\n"
      );
    }
    uint rev = 0;
    String url = worker->to_string_utf8(args[0]);
    String file = worker->to_string_utf8(args[1]);
    Callback cb;
    
    if ( args.Length() > 2 ) {
      cb = get_callback_for_buffer_http_error(worker, args[2]);
    }
    av8_try_catch({
      rev = HttpHelper::upload(url, file, cb);
    }, HttpError);
    av8_return( rev );
  }
  
  /**
   * @func upload_sync(url,local_path)
   * @arg url {String}
   * @arg local_path {String}
   * @ret {Buffer}
   */
  static void upload_sync(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      av8_throw_err(
        "* @func upload_sync(url,local_path)\n"
        "* @arg url {String}\n"
        "* @arg local_path {String}\n"
        "* @ret {Buffer}\n"
      );
    }
    String url = worker->to_string_utf8(args[0]);
    String file = worker->to_string_utf8(args[1]);
    av8_try_catch({
      av8_return( HttpHelper::upload_sync(url, file) );
    }, HttpError);
  }
  
  template<bool stream> static void get(FunctionCall args, cchar* argument) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(argument);
    }
    uint rev = 0;
    String url = worker->to_string_utf8(args[0]);
    Callback cb;
    
    if ( args.Length() > 1 ) {
      cb = stream ? get_callback_for_io_stream_http_error(worker, args[1]) :
                    get_callback_for_buffer_http_error(worker, args[1]);
    }
    
    if ( stream ) {
      av8_try_catch({ rev = HttpHelper::get_stream(url, cb); }, HttpError);
    } else {
      av8_try_catch({ rev = HttpHelper::get(url, cb); }, HttpError);
    }
    av8_return( rev );
  }

  /**
   * @func get(url[,cb])
   * @arg url {String}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void get(FunctionCall args) {
    get<false>(args, 
      "* @func get(url[,cb])\n"
      "* @arg url {String}\n"
      "* @arg [cb] {Function}\n"
      "* @ret {uint} return req id\n"
               );
  }

  /**
   * @func get_stream(url[,cb])
   * @arg url {String}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void get_stream(FunctionCall args) {
    get<true>(args, 
      "* @func get_stream(url[,cb])\n"
      "* @arg url {String}\n"
      "* @arg [cb] {Function}\n"
      "* @ret {uint} return req id\n"
              );
  }

  /**
   * @func post(url,data[,cb])
   * @arg url {String}
   * @arg data {String|ArrayBuffer|Buffer}
   * @arg [cb] {Function}
   * @ret {uint} return req id
   */
  static void post(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 2 || ! args[0]->IsString() ||
        !(args[1]->IsString() ||
          args[1]->IsArrayBuffer() || worker->has_buffer(args[1]))
    ) {
      av8_throw_err(
        "* @func post(url,data[,cb])\n"
        "* @arg url {String}\n"
        "* @arg data {String|ArrayBuffer|Buffer}\n"
        "* @arg [cb] {Function}\n"
        "* @ret {uint} return req id\n"
      );
    }
    uint rev = 0;
    String url = worker->to_string_utf8(args[0]);
    Callback cb;
    
    if ( args.Length() > 2 ) {
      cb = get_callback_for_buffer_http_error(worker, args[2]);
    }
    
    av8_try_catch({
      if (args[1]->IsString()) {
        rev = HttpHelper::post(url, worker->to_string_utf8(args[1]).collapse_buffer(), cb);
      } else if (args[1]->IsArrayBuffer()) {
        v8::ArrayBuffer::Contents con = args[1].As<v8::ArrayBuffer>()->GetContents();
        WeakBuffer buffer((cchar*)con.Data(), (uint)con.ByteLength());
        rev = HttpHelper::post(url, buffer.copy(), cb);
      } else {
        Buffer* buff = Wrap<Buffer>::Self(args[1]->ToObject());
        rev = HttpHelper::post(url, *buff, cb);
      }
    }, HttpError);
    av8_return( rev );
  }

  /**
   * @func get_sync(url)
   * @arg url {String}
   * @ret {Buffer}
   */
  static void get_sync(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func get_sync(url)\n"
        "* @arg url {String}\n"
        "* @ret {Buffer}\n"
      );
    }
    String url = worker->to_string_utf8(args[0]);
    av8_try_catch({ av8_return( HttpHelper::get_sync(url) ); }, HttpError);
  }
  
  /**
   * @func post_sync(url,data)
   * @arg url {String}
   * @arg data {String|ArrayBuffer|Buffer}
   * @ret {Buffer}
   */
  static void post_sync(FunctionCall args) {
    av8_worker(args);
    if (  args.Length() < 2 || !args[0]->IsString() ||
        !(args[1]->IsString() || 
          args[1]->IsArrayBuffer() || 
          worker->has_buffer(args[1])
        )
    ) {
      av8_throw_err(
        "* @func post_sync(url,data)\n"
        "* @arg url {String}\n"
        "* @arg data {String|ArrayBuffer|Buffer}\n"
        "* @ret {Buffer}\n"
      );
    }
    
    String url = worker->to_string_utf8(args[0]);
    Buffer rev;
    
    av8_try_catch({
      if (args[1]->IsString()) {
        rev = HttpHelper::post_sync(url, worker->to_string_utf8(args[1]).collapse_buffer());
      } else if (args[1]->IsArrayBuffer()) {
        v8::ArrayBuffer::Contents con = args[1].As<v8::ArrayBuffer>()->GetContents();
        WeakBuffer buff((cchar*)con.Data(), (uint)con.ByteLength());
        rev = HttpHelper::post_sync(url, buff.copy());
      } else {
        Buffer* buff = Wrap<Buffer>::Self(args[1]->ToObject());
        rev = HttpHelper::post_sync(url, *buff);
      }
    }, HttpError);
    av8_return( move(rev) );
  }

  /**
   * @func abort(id)
   * @arg id {uint} abort id
   */
  static void abort(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || !args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func abort(id)\n"
        "* @arg id {uint} abort id\n"
      );
    }
    HttpHelper::abort( args[0]->ToUint32()->Value() );
  }

  /**
   * @func user_agent()
   * @ret {String}
   */
  static void user_agent(FunctionCall args) {
    av8_worker(args);
    av8_return( HttpHelper::user_agent() );
  }

  /**
   * @func set_user_agent(user_agent)
   * @arg user_agent {String}
   */
  static void set_user_agent(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsString()) {
      av8_throw_err("Bad argument");
    }
    HttpHelper::set_user_agent( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func ssl_cacert_file()
   * @ret {String} return cacert file path
   */
  static void ssl_cacert_file(FunctionCall args) {
    av8_worker(args);
    av8_return( HttpHelper::ssl_cacert_file() );
  }

  /**
   * @func ssl_cacert_file(path)
   * @arg path {String}
   */
  static void set_ssl_cacert_file(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func ssl_cacert_file(path)\n"
        "* @arg path {String}\n"
      );
    }
    HttpHelper::set_ssl_cacert_file( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func set_ssl_client_key_file(path)
   * @arg path {String}
   */
  static void set_ssl_client_key_file(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func set_ssl_client_key_file(path)\n"
        "* @arg path {String}\n"
      );
    }
    HttpHelper::set_ssl_client_key_file( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func set_ssl_client_keypasswd(password)
   * @arg password {String}
   */
  static void set_ssl_client_keypasswd(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func set_ssl_client_keypasswd(password)\n"
        "* @arg password {String}\n"
      );
    }
    HttpHelper::set_ssl_client_keypasswd( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func cache_path()
   * @ret {String}
   */
  static void cache_path(FunctionCall args) {
    av8_worker(args);
    av8_return( HttpHelper::cache_path() );
  }

  /**
   * @func set_cache_path(path)
   * @arg path {String}
   */
  static void set_cache_path(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func set_cache_path(path)\n"
        "* @arg path {String}\n"
      );
    }
    HttpHelper::set_cache_path( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func clear_cache()
   */
  static void clear_cache(FunctionCall args) {
    HttpHelper::clear_cache();
  }

  /**
   * @func clear_cookie()
   */
  static void clear_cookie(FunctionCall args) {
    HttpHelper::clear_cookie();
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    WrapNativeHttpClientRequest::binding(exports, worker);
    //
    av8_set_field(HTTP_METHOD_GET, HTTP_METHOD_GET);
    av8_set_field(HTTP_METHOD_POST, HTTP_METHOD_POST);
    av8_set_field(HTTP_METHOD_HEAD, HTTP_METHOD_HEAD);
    av8_set_field(HTTP_METHOD_DELETE, HTTP_METHOD_DELETE);
    av8_set_field(HTTP_METHOD_PUT, HTTP_METHOD_PUT);
    //
    av8_set_field(HTTP_READY_STATE_INITIAL, HTTP_READY_STATE_INITIAL);
    av8_set_field(HTTP_READY_STATE_READY, HTTP_READY_STATE_READY);
    av8_set_field(HTTP_READY_STATE_SENDING, HTTP_READY_STATE_SENDING);
    av8_set_field(HTTP_READY_STATE_RESPONSE, HTTP_READY_STATE_RESPONSE);
    av8_set_field(HTTP_READY_STATE_COMPLETED, HTTP_READY_STATE_COMPLETED);
    //
    av8_set_method(request, request);
    av8_set_method(request_stream, request_stream);
    av8_set_method(request_sync, request_sync);
    av8_set_method(download, download);
    av8_set_method(download_sync, download_sync);
    av8_set_method(upload, upload);
    av8_set_method(upload_sync, upload_sync);
    av8_set_method(get, get);
    av8_set_method(get_stream, get_stream);
    av8_set_method(post, post);
    av8_set_method(get_sync, get_sync);
    av8_set_method(post_sync, post_sync);
    av8_set_method(abort, abort);
    av8_set_method(user_agent, user_agent);
    av8_set_method(set_user_agent, set_user_agent);
    av8_set_method(ssl_cacert_file, ssl_cacert_file);
    av8_set_method(set_ssl_cacert_file, set_ssl_cacert_file);
    av8_set_method(set_ssl_client_key_file, set_ssl_client_key_file);
    av8_set_method(set_ssl_client_keypasswd, set_ssl_client_keypasswd);
    av8_set_method(cache_path, cache_path);
    av8_set_method(set_cache_path, set_cache_path);
    av8_set_method(clear_cache, clear_cache);
    av8_set_method(clear_cookie, clear_cookie);
  }
};

av8_reg_module(_http, NativeHttp);
av8_end
