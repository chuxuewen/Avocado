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

#include "./av8.h"
#include "event-1.h"
#include "native-core-js.h"

/**
 * @ns avocado::av8
 */

av8_begin

EventDataCast::EventDataCast(Cast cast): m_cast(cast) {

}

EventDataCast::~EventDataCast() {
  av_unreachable();
}

void EventDataCast::release() {
  // Do nothing
}

Local<v8::Value> EventDataCast::cast(const Object& object, Worker* worker) {
  return m_cast ? m_cast(object, worker): worker->Null();
}

// ------------------------------------------------------------------------

typedef Event<> NativeEvent;

class WrapNativeEvent: public WrapBase {
public: typedef Event<> Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void noticer(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    Wrap<NativeEvent>* wrap = Wrap<NativeEvent>::unwrap(args);
    av8_return( wrap->Get( worker->strs()->m_noticer() ) );
  }
  
  static void sender(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    Wrap<NativeEvent>* wrap = Wrap<NativeEvent>::unwrap(args);
    Local<v8::Value> noticer = wrap->Get(worker->strs()->m_noticer());
    
    if ( !noticer.IsEmpty() && noticer->IsObject() ) {
      Local<v8::Value> sender;
      if (noticer.As<v8::Object>()->Get(worker->context(),
                                        worker->strs()->sender()).ToLocal(&sender)) {
        av8_return(sender);
      }
    } else {
      if ( wrap->self()->noticer() ) {
        Wrap<Object>* sender = Wrap<Object>::wrap( wrap->self()->sender() );
        av8_return( sender->local() );
      } else {
        av8_return_null();
      }
    }
  }
  
  static void name(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    Wrap<NativeEvent>* wrap = Wrap<NativeEvent>::unwrap(args);
    Local<v8::Value> noticer = wrap->Get(worker->strs()->m_noticer());
    
    if ( !noticer.IsEmpty() && noticer->IsObject() ) {
      Local<v8::Value> name;
      if (noticer.As<v8::Object>()->Get(worker->context(),
                                        worker->strs()->name()).ToLocal(&name)) {
        av8_return(name);
      }
    } else {
      if ( wrap->self()->noticer() ) {
        av8_return( wrap->self()->name() );
      } else {
        av8_return_null();
      }
    }
  }
  
  static void data(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_unwrap(NativeEvent);
    EventDataCast* cast = static_cast<EventDataCast*>(wrap->get_data());
    if ( cast ) {
      av8_return( cast->cast(*wrap->self()->data(), worker) );
    }
    av8_return_null();
  }
  
  static void return_value(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(NativeEvent);
    av8_return( self->return_value );
  }
  
  static void set_return_value(Local<v8::String> name,
                               Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsBoolean() && !value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(NativeEvent);
    self->return_value = value->ToInt32()->Value();
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeEvent, constructor, {
      av8_set_cls_property(noticer, noticer);
      av8_set_cls_property(sender, sender);
      av8_set_cls_property(data, data);
      av8_set_cls_property(name, name);
      av8_set_cls_property(return_value, return_value, set_return_value);
    }, nullptr);
  }
};

/**
 * @class BindingNativeEvent
 */
class BindingNativeEvent {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    WrapNativeEvent::binding(exports, worker);
    worker->run_native_script(exports, (cchar*)
                              CORE_native_js_code_event_,
                              CORE_native_js_code_event_count_, "event");
  }
};

av8_reg_module(_event, BindingNativeEvent)
av8_end
