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
#include "agui/event.h"
#include "agui/action.h"
#include "agui/button.h"
#include "agui/panel.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

typedef GUIEvent        NativeGUIEvent;
typedef GUIKeyEvent     NativeGUIKeyEvent;
typedef GUIClickEvent   NativeGUIClickEvent;
typedef GUIMouseEvent   NativeGUIMouseEvent;
typedef GUITouchEvent   NativeGUITouchEvent;
typedef GUIActionEvent  NativeGUIActionEvent;
typedef GUISwitchEvent  NativeGUISwitchEvent;
typedef GUIHighlightedEvent NativeGUIHighlightedEvent;

/**
 * @class WrapNativeGUIEvent
 */
class WrapNativeGUIEvent: public WrapBase {
public:
  typedef GUIEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void origin(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIEvent);
    if ( self->origin() ) {
      Wrap<View>* wrap = Wrap<View>::wrap(self->origin());
      av8_return( worker->local(wrap->handle()) );
    } else {
      av8_return_null();
    }
  }
  
  static void timestamp(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIEvent);
    av8_return( self->timestamp() );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUIEvent, constructor, {
      av8_set_cls_property(origin, origin);
      av8_set_cls_property(timestamp, timestamp);
    }, Event<>);
  }
};

/**
 * @class WrapNativeGUIActionEvent
 */
class WrapNativeGUIActionEvent: public WrapBase {
public:
  typedef GUIActionEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void action(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIActionEvent);
    if ( self->action() ) {
      Wrap<Action>* wrap = Wrap<Action>::wrap(self->action());
      av8_return( wrap->local() );
    } else {
      av8_return_null();
    }
  }
  static void delay(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIActionEvent);
    av8_return( self->delay() / 1000 );
  }
  static void frame(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIActionEvent);
    av8_return( self->frame() );
  }
  static void loop(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIActionEvent);
    av8_return( self->loop() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUIActionEvent, constructor, {
      av8_set_cls_property(action, action);
      av8_set_cls_property(delay, delay);
      av8_set_cls_property(frame, frame);
      av8_set_cls_property(loop, loop);
    }, GUIEvent);
  }
};

/**
 * @class WrapNativeGUIKeyEvent
 */
class WrapNativeGUIKeyEvent: public WrapBase {
public:
  typedef GUIKeyEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void keycode(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->keycode() );
  }
  
  static void repeat(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->repeat() );
  }
  
  static void shift(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->shift() );
  }
  
  static void ctrl(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->ctrl() );
  }
  
  static void alt(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->alt() );
  }
  
  static void command(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->command() );
  }
  
  static void caps_lock(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->caps_lock() );
  }
    
  static void device(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->device() );
  }
  
  static void source(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIKeyEvent);
    av8_return( self->source() );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUIKeyEvent, constructor, {
      av8_set_cls_property(keycode, keycode);
      av8_set_cls_property(repeat, repeat);
      av8_set_cls_property(shift, shift);
      av8_set_cls_property(ctrl, ctrl);
      av8_set_cls_property(alt, alt);
      av8_set_cls_property(command, command);
      av8_set_cls_property(caps_lock, caps_lock);
      av8_set_cls_property(device, device);
      av8_set_cls_property(source, source);
    }, GUIEvent);
  }
};

/**
 * @class WrapNativeGUIKeyEvent
 */
class WrapNativeGUIClickEvent: public WrapBase {
public:
  typedef GUIClickEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIClickEvent);
    av8_return( self->x() );
  }
  
  static void y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIClickEvent);
    av8_return( self->y() );
  }
  
  static void count(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIClickEvent);
    av8_return( self->count() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUIClickEvent, constructor, {
      av8_set_cls_property(x, x);
      av8_set_cls_property(y, y);
      av8_set_cls_property(count, count);
    }, GUIEvent);
  }
};

/**
 * @class WrapNativeGUIHighlightedEvent
 */
class WrapNativeGUIHighlightedEvent: public WrapBase {
public:

  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }

  static void status(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIHighlightedEvent);
    av8_return( self->status() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    
    av8_set_field(HIGHLIGHTED_NORMAL, HIGHLIGHTED_NORMAL);
    av8_set_field(HIGHLIGHTED_HOVER, HIGHLIGHTED_HOVER);
    av8_set_field(HIGHLIGHTED_DOWN, HIGHLIGHTED_DOWN);
    
    av8_binding_class(NativeGUIHighlightedEvent, constructor, {
      av8_set_cls_property(status, status);
    }, GUIEvent);
  }
};

/**
 * @class NativeGUIMouseEvent
 */
class WrapNativeGUIMouseEvent: public WrapBase {
public:
  typedef GUIMouseEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIMouseEvent);
    av8_return( self->x() );
  }
  
  static void y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIMouseEvent);
    av8_return( self->y() );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUIMouseEvent, constructor, {
      av8_set_cls_property(x, x);
      av8_set_cls_property(y, y);
    }, GUIKeyEvent);
  }
};

/**
 * @class WrapNativeGUITouchEvent
 */
class WrapNativeGUITouchEvent: public WrapBase {
public:
  typedef GUITouchEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void change_touches(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_unwrap(GUITouchEvent);
    v8::HandleScope scope(worker->isolate());
    
    Local<v8::Value> r = wrap->Get(worker->strs()->m_change_touches());
    
    if ( r.IsEmpty() ) {
      Local<v8::Array> arr = worker->NewArray().As<v8::Array>();
      int j = 0;
      
      for ( auto& i : wrap->self()->change_touches() ) {
        Local<v8::Object> item = worker->NewObject().As<v8::Object>();
        //
        Wrap<View>* view = Wrap<View>::wrap(i.value().view);
        item->Set(worker->strs()->id(), worker->New(i.value().id));
        item->Set(worker->strs()->start_x(), worker->New(i.value().start_x));
        item->Set(worker->strs()->start_y(), worker->New(i.value().start_y));
        item->Set(worker->strs()->x(), worker->New(i.value().x));
        item->Set(worker->strs()->y(), worker->New(i.value().y));
        item->Set(worker->strs()->force(), worker->New(i.value().force));
        item->Set(worker->strs()->view(), view->local());
        //
        arr->Set(j, item);
        j++;
      }
      
      r = arr.As<v8::Value>();
      
      wrap->Set(worker->strs()->m_change_touches(), r);
    }
    av8_return( r );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUITouchEvent, constructor, {
      av8_set_cls_property(change_touches, change_touches);
    }, GUIEvent);
  }
};

/**
 * @class WrapNativeGUISwitchEvent
 */
class WrapNativeGUISwitchEvent: public WrapBase {
public:
  typedef GUISwitchEvent Type;
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Access forbidden.");
  }
  
  static void direction(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUISwitchEvent);
    av8_return( worker->gui_value_program()->New(self->direction()) );
  }
  
  static void button(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUISwitchEvent);
    
    if ( self->button() ) {
      av8_return( Wrap<Button>::wrap(self->button())->local() );
    } else {
      av8_return_null();
    }
  }
  
  static void target_button(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUISwitchEvent);
    
    if ( self->target_button() ) {
      av8_return( Wrap<Button>::wrap(self->target_button(), View::BUTTON)->local() );
    } else {
      av8_return_null();
    }
  }
  
  static void set_target_button(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUISwitchEvent);
    
    Button* button = nullptr;
    
    if ( worker->has(value, View::BUTTON) ) {
      button = Wrap<Button>::Self(value);
    } else if ( ! value->IsNull() ) {
      av8_throw_err("Bad argument.");
    }
    self->target_button(button);
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUISwitchEvent, constructor, {
      av8_set_cls_property(direction, direction);
      av8_set_cls_property(button, button);
      av8_set_cls_property(target_button, target_button, set_target_button);
    }, GUIEvent);
  }
};

void binding_gui_event(Local<v8::Object> exports, Worker* worker) {
  worker->binding("_event");
  WrapNativeGUIEvent::binding(exports, worker);
  WrapNativeGUIActionEvent::binding(exports, worker);
  WrapNativeGUIKeyEvent::binding(exports, worker);
  WrapNativeGUIClickEvent::binding(exports, worker);
  WrapNativeGUIMouseEvent::binding(exports, worker);
  WrapNativeGUITouchEvent::binding(exports, worker);
  WrapNativeGUISwitchEvent::binding(exports, worker);
  WrapNativeGUIHighlightedEvent::binding(exports, worker);
}

av8_nsd
