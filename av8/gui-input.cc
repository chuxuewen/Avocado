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

#include "gui.h"
#include "agui/input.h"
#include "agui/textarea.h"
#include "agui/scroll.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

class WrapInput: public BasicWrapView {
public:
  
  virtual bool add_event_listener(cString& name, cString& func, int id) {
    Wrap<View>* wrap = reinterpret_cast<Wrap<View>*>(this);
    
    if ( name == GUI_EVENT_CHANGE.to_string() ) {
      return ViewUtil::add_event_listener(wrap, GUI_EVENT_CHANGE, func, id);
    } else {
      return ViewUtil::add_event_listener(wrap, name, func, id);
    }
  }
  
  virtual bool remove_event_listener(cString& name, int id) {
    Wrap<View>* wrap = reinterpret_cast<Wrap<View>*>(this);
    
    if ( name == GUI_EVENT_CHANGE.to_string() ) {
      return ViewUtil::remove_event_listener(wrap, GUI_EVENT_CHANGE, id);
    } else {
      return ViewUtil::remove_event_listener(wrap, name, id);
    }
  }
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapInput>(args, new Input());
  }
  
  static void type(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Input);
    av8_return( worker->gui_value_program()->New(self->type()) );
  }
  
  static void return_type(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Input);
    av8_return( worker->gui_value_program()->New(self->return_type()) );
  }
  
  static void placeholder(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Input);
    av8_return( self->placeholder() );
  }
  
  static void placeholder_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Input);
    av8_return( worker->gui_value_program()->New(self->placeholder_color()) );
  }
  
  static void security(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Input);
    av8_return( self->security() );
  }
  
  static void text_margin(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Input);
    av8_return( self->text_margin() );
  }
  
  static void set_type(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(KeyboardType, keyboard_type, value, "Input.type = %s");
    av8_self(Input);
    self->set_type(out);
  }
  
  static void set_return_type(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(KeyboardReturnType, keyboard_return_type, value, "Input.return_type = %s");
    av8_self(Input);
    self->set_return_type(out);
  }
  
  static void set_placeholder(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Input);
    self->set_placeholder( worker->to_string_ucs2(value) );
  }
  
  static void set_placeholder_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Input.placeholder_color = %s");
    av8_self(Input);
    self->set_placeholder_color( out );
  }
  
  static void set_security(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Input);
    self->set_security(value->ToBoolean()->Value());
  }
  
  /**
   * @set text_margin {float}
   */
  static void set_text_margin(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("* @set text_margin {float}");
    }
    av8_self(Input);
    self->set_text_margin(value.As<v8::Number>()->Value());
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Input, constructor, {
      av8_set_cls_property(type, type, set_type);
      av8_set_cls_property(return_type, return_type, set_return_type);
      av8_set_cls_property(placeholder, placeholder, set_placeholder);
      av8_set_cls_property(placeholder_color, placeholder_color, set_placeholder_color);
      av8_set_cls_property(security, security, set_security);
      av8_set_cls_property(text_margin, text_margin, set_text_margin);
    }, Text);
    worker->native_constructors()->set_alias(av8_typeid(Input), View::INPUT);
  }
};

class WrapTextarea: public BasicWrapView {
public:
  
  virtual bool add_event_listener(cString& name, cString& func, int id) {
    Wrap<View>* wrap = reinterpret_cast<Wrap<View>*>(this);
    
    if ( name == GUI_EVENT_CHANGE.to_string() ) {
      return ViewUtil::add_event_listener(wrap, GUI_EVENT_CHANGE, func, id);
    } else if ( name == GUI_EVENT_SCROLL.to_string() ) {
      return ViewUtil::add_event_listener(wrap, GUI_EVENT_SCROLL, func, id);
    } else {
      return ViewUtil::add_event_listener(wrap, name, func, id);
    }
  }
  
  virtual bool remove_event_listener(cString& name, int id) {
    Wrap<View>* wrap = reinterpret_cast<Wrap<View>*>(this);
    
    if ( name == GUI_EVENT_CHANGE.to_string() ) {
      return ViewUtil::remove_event_listener(wrap, GUI_EVENT_CHANGE, id);
    } if ( name == GUI_EVENT_SCROLL.to_string() ) {
      return ViewUtil::remove_event_listener(wrap, GUI_EVENT_SCROLL, id);
    } else {
      return ViewUtil::remove_event_listener(wrap, name, id);
    }
  }
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapTextarea>(args, new Textarea());
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Textarea, constructor, {
      ViewUtil::inherit_scroll(cls, worker);
    }, Input);
    worker->native_constructors()->set_alias(av8_typeid(Textarea), View::TEXTAREA);
  }
};

void binding_input(Local<v8::Object> exports, Worker* worker) {
  WrapInput::binding(exports, worker);
  WrapTextarea::binding(exports, worker);
}

av8_nsd
