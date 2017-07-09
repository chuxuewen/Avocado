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
#include "agui/panel.h"
#include "agui/button.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

bool ViewUtil::panel_add_event_listener(Wrap<View>* wrap, cString& name, cString& func, int id) {
  
  if ( name == GUI_EVENT_SWITCH.to_string() ) {
    return ViewUtil::add_event_listener(wrap, GUI_EVENT_SWITCH, func, id);
  }
  else if ( name == GUI_EVENT_ENTER.to_string() ) {
    return ViewUtil::add_event_listener(wrap, GUI_EVENT_ENTER, func, id);
  }
  else if ( name == GUI_EVENT_LEAVE.to_string() ) {
    return ViewUtil::add_event_listener(wrap, GUI_EVENT_LEAVE, func, id);
  }
  else {
    return ViewUtil::add_event_listener(wrap, name, func, id);
  }
}

bool ViewUtil::panel_remove_event_listener(Wrap<View>* wrap, cString& name, int id) {
  
  if ( name == GUI_EVENT_SWITCH.to_string() ) {
    return ViewUtil::remove_event_listener(wrap, GUI_EVENT_SWITCH, id);
  }
  else if ( name == GUI_EVENT_ENTER.to_string() ) {
    return ViewUtil::remove_event_listener(wrap, GUI_EVENT_ENTER, id);
  }
  else if ( name == GUI_EVENT_LEAVE.to_string() ) {
    return ViewUtil::remove_event_listener(wrap, GUI_EVENT_LEAVE, id);
  }
  else {
    return ViewUtil::remove_event_listener(wrap, name, id);
  }
}

/**
 * @class WrapPanel
 */
class WrapPanel: public BasicWrapView {
public:

  /**
   * @func overwrite
   */
  virtual bool add_event_listener(cString& name, cString& func, int id) {
    return ViewUtil::panel_add_event_listener(reinterpret_cast<Wrap<View>*>(this), name, func, id);
  }
  
  /**
   * @func overwrite
   */
  virtual bool remove_event_listener(cString& name, int id) {
    return ViewUtil::panel_remove_event_listener(reinterpret_cast<Wrap<View>*>(this), name, id);
  }
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapPanel>(args, new Panel());
  }
  
  /**
   * @func first_button()
   * @ret {Button}
   */
  static void first_button(FunctionCall args) {
    av8_worker(args);
    av8_self(Panel);
    Button* button = self->first_button();
    if ( button ) {
      av8_return( Wrap<Button>::wrap(button, View::BUTTON)->local() );
    } else {
      av8_return_null();
    }
  }
    
  /**
   * @get allow_leave {bool}
   */
  static void allow_leave(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Panel);
    av8_return( self->allow_leave() );
  }

  /**
   * @set allow_leave {bool}
   */
  static void set_allow_leave(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Panel);
    self->set_allow_leave( value->ToBoolean()->Value() );
  }
  
  /**
   * @get switch_time {uint} ms
   */
  static void switch_time(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Panel);
    av8_return( self->switch_time() );
  }
  
  /**
   * @set switch_time {uint} ms
   */
  static void set_switch_time(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("* @set switch_time {uint} ms");
    }
    av8_self(Panel);
    int64 num = value->ToNumber()->Value();
    self->set_switch_time( uint64(1000) * av_min(0, num) );
  }
  
  /**
   * @get enable_switch {bool}
   */
  static void enable_switch(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Panel);
    av8_return( self->enable_switch() );
  }
  
  /**
   * @set enable_switch {bool}
   */
  static void set_enable_switch(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsBoolean() ) {
      av8_throw_err("* @set enable_switch {bool}");
    }
    av8_self(Panel);
    self->set_enable_switch( value->ToBoolean()->Value() );
  }
  
  /**
   * @get is_activity {bool}
   */
  static void is_activity(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Panel);
    av8_return( self->is_activity() );
  }
  
  /**
   * @get parent_panel {Pabel}
   */
  static void parent_panel(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Panel);
    Panel* panel = self->parent_panel();
    if ( panel ) {
      av8_return( Wrap<Panel>::wrap(panel, View::PANEL)->local() );
    } else {
      av8_return_null();
    }
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Panel, constructor, {
      av8_set_cls_method(first_button, first_button);
      av8_set_cls_property(allow_leave, allow_leave, set_allow_leave);
      av8_set_cls_property(switch_time, switch_time, set_switch_time);
      av8_set_cls_property(enable_switch, enable_switch, set_enable_switch);
      av8_set_cls_property(is_activity, is_activity);
      av8_set_cls_property(parent_panel, parent_panel);
    }, Div);
    worker->native_constructors()->set_alias(av8_typeid(Panel), View::PANEL);
  }
};

void binding_panel(Local<v8::Object> exports, Worker* worker) {
  WrapPanel::binding(exports, worker);
}

av8_nsd
