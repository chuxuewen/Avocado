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
#include "agui/app.h"
#include "agui/action.h"
#include "agui/css.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

class NativeViewController: public ViewController {
public:
  virtual void trigger_remove_view(View* view) {
    Wrap<NativeViewController>* wrap = av8_wrap(NativeViewController, this);
    Wrap<View>* wrap_view = Wrap<View>::wrap(view, view->view_type());
    Local<v8::Value> arg = wrap_view->local();
    wrap->call(wrap->worker()->strs()->trigger_remove_view(), 1, &arg); // trigger event
  }
};

/**
 * @class WrapController
 */
class WrapNativeViewController: public WrapBase {
public:
  
  // ====================
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    New<WrapNativeViewController>(args, new NativeViewController());
  }
  
  static void load_view(FunctionCall args) {
    // noop
  }
  
  static void trigger_remove_view(FunctionCall args) {
    // noop
  }
  
  static void parent(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ViewController);
    ViewController* ctr = self->parent();
    if ( ctr) {
      Wrap<ViewController>* wrap = av8_wrap(ViewController, ctr);
      av8_return( wrap->local() );
    } else {
      av8_return( worker->Null() );
    }
  }
  
  static void view(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ViewController);
    View* view = self->view();
    if ( view ) {
      Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
      av8_return( wrap->local() );
    } else {
      av8_return( worker->Null() );
    }
  }
  
  /**
   * @set view {View}
   */
  static void set_view(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! worker->has(value, View::VIEW)) {
      av8_throw_err("* @set view {View}");
    }
    av8_unwrap(ViewController);
    
    av8_try_catch({
      wrap->self()->view( Wrap<View>::Self(value) );
    }, Error);
    
    // 让视图与控制器相互建立引用,确保不被CLR错误的回收,这样只要持有一个对像另一个对像会在弱引用下继续保持
    wrap->Set(worker->strs()->__view(), value);
    value.As<v8::Object>()->Set(worker->strs()->__controller(), wrap->local());
  }
  
  /**
   * @func find(id)
   * @arg id {uint}
   * @ret {View|ViewController}
   */
  static void find(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func find(id)\n"
        "* @arg id {uint}\n"
        "* @ret {View|ViewController}\n"
      );
    }
    av8_self(ViewController);
    
    Member* member = self->find( worker->to_string_utf8(args[0]) );
    if ( member ) {
      View* view = member->as_view();
      WrapBase* wrap = nullptr;
      if ( view ) {
        wrap = Wrap<View>::wrap(view, view->view_type());
      } else {
        wrap = av8_wrap(ViewController, member->as_ctr());
      }
      av8_return( wrap->local() );
    } else {
      av8_return( worker->Null() );
    }
  }
  
  /**
   * @get id {String}
   */
  static void id(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ViewController);
    av8_return( self->id() );
  }
  
  /**
   * @set id {String}
   */
  static void set_id(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(ViewController);
    av8_try_catch({
      self->set_id(worker->to_string_utf8(value));
    }, Error);
  }
  
  /**
   * @func remove() 
   */
  static void remove(FunctionCall args) {
    av8_worker(args);
    av8_self(ViewController);
    self->remove();
  }
  
public:
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeViewController, constructor, {
      av8_set_cls_method(load_view, load_view);
      av8_set_cls_method(trigger_remove_view, trigger_remove_view);
      av8_set_cls_property(parent, parent);
      av8_set_cls_property(view, view, set_view);
      av8_set_cls_property(id, id, set_id);
      av8_set_cls_method(find, find);
      av8_set_cls_method(remove, remove);
    }, nullptr);
  }
};

// ================= View ================

template<class T>
static void add_event_listener_(Wrap<View>* wrap, const GUIEventName& type, cString& func, int id) {
  auto f = [wrap, func]( GUIEvent& evt ) {
    v8::HandleScope scope(wrap->worker()->isolate());
    // arg event
    Wrap<T>* ev = Wrap<T>::wrap(static_cast<T*>(&evt), av8_typeid(T));
    
    Local<v8::Value> args[2] = { ev->local(), wrap->worker()->New(true) };
    // call js trigger func
    wrap->call( wrap->worker()->NewStringAscii(func), 2, args );
  };
  
  View* view = wrap->self();
  
  view->on(type, f, id);
}

/**
 * @func add_event_listener
 */
bool ViewUtil::add_event_listener(Wrap<View>* wrap, cString& name, cString& func, int id) {
  auto i = GUI_EVENT_TABLE.find(name);
  if ( i == GUI_EVENT_TABLE.end() ) {
    return false;
  }
  
  return add_event_listener(wrap, i.value(), func, id);
}

bool ViewUtil::add_event_listener(Wrap<View>* wrap, const GUIEventName& name, cString& func, int id) {
  switch ( name.category() ) {
    case GUI_EVENT_CATEGORY_KEYBOARD:
      add_event_listener_<GUIKeyEvent>(wrap, name, func, id); break;
    case GUI_EVENT_CATEGORY_CLICK:
      add_event_listener_<GUIClickEvent>(wrap, name, func, id); break;
    case GUI_EVENT_CATEGORY_HIGHLIGHTED:
      add_event_listener_<GUIHighlightedEvent>(wrap, name, func, id); break;
    case GUI_EVENT_CATEGORY_MOUSE:
      add_event_listener_<GUIMouseEvent>(wrap, name, func, id); break;
    case GUI_EVENT_CATEGORY_TOUCH:
      add_event_listener_<GUITouchEvent>(wrap, name, func, id); break;
    case GUI_EVENT_CATEGORY_ACTION:
      add_event_listener_<GUIActionEvent>(wrap, name, func, id); break;
    case GUI_EVENT_CATEGORY_SWITCH:
      add_event_listener_<GUISwitchEvent>(wrap, name, func, id); break;
    default:
      add_event_listener_<GUIEvent>(wrap, name, func, id); break;
  }
  
  return true;
}

/**
 * @func remove_event_listener
 */
bool ViewUtil::remove_event_listener(Wrap<View>* wrap, cString& name, int id) {
  
  auto i = GUI_EVENT_TABLE.find(name);
  if ( i == GUI_EVENT_TABLE.end() ) {
    return false;
  }
  
  // off event listener
  wrap->self()->off(i.value(), id);
  
  return true;
}

bool ViewUtil::remove_event_listener(Wrap<View>* wrap, const GUIEventName& name, int id) {
  wrap->self()->off(name, id);
  return true;
}

/**
 * @class WrapView
 */
class WrapView: public BasicWrapView {
public:

  // v8 bind
  
  /**
   * @constructor() 
   */
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapView>(args, new View());
  }
  
  /**
   * @func prepend(child) 
   * @arg child {View}
   */
  static void prepend(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! worker->has(args[0], View::VIEW)) {
      av8_throw_err(
        "* @func prepend(child)\n"
        "* @arg child {View}\n"
      );
    }
    av8_self(View);
    View* child = Wrap<View>::Self(args[0]);
    try { self->prepend(child); }
    catch (cError& err) { av8_throw_err(err); }
  }

  /**
   * @func append(child)
   * @arg child {View}
   */
  static void append(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! worker->has(args[0], View::VIEW)) {
      av8_throw_err(
        "* @func append(child)\n"
        "* @arg child {View}\n"
      );
    }
    av8_self(View);
    View* child = Wrap<View>::Self(args[0]);
    try { self->append(child); }
    catch (cError& err) { av8_throw_err(err); }
  }

  /**
   * @func append_text(text)
   * @arg text {String}
   * @ret {View}
   */
  static void append_text(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 ) {
      av8_throw_err(
        "* @func append_text(text)\n"
        "* @arg text {String}\n"
        "* @ret {View}\n"
      );
    }
    av8_self(View);
    View* view = nullptr;
    
    av8_try_catch({
      view = self->append_text( worker->to_string_ucs2(args[0]) );
    }, Error);
    
    if (view) {
      Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
      av8_return( wrap->local() );
    } else {
      av8_return( worker->Null() );
    }
  }

  /**
   * @func append_to(parent)
   * @arg parent {View}
   */
  static void append_to(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! worker->has(args[0], View::VIEW)) {
      av8_throw_err(
        "* @func append_to(parent)\n"
        "* @arg parent {View}\n"
      );
    }
    av8_self(View);
    View* parent = Wrap<View>::Self(args[0]);
    
    try { self->append_to(parent); }
    catch (cError& err) { av8_throw_err(err); }
  }

  /**
   * @func before(prev)
   * @arg prev {View}
   */
  static void before(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! worker->has(args[0], View::VIEW)) {
      av8_throw_err(
        "* @func before(prev)\n"
        "* @arg prev {View}\n"
      );
    }
    av8_self(View);
    View* brother = Wrap<View>::Self(args[0]);
    try { self->before(brother); }
    catch (cError& err) { av8_throw_err(err); }
  }

  /**
   * @func after(next)
   * @arg next {View}
   */
  static void after(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !worker->has(args[0], View::VIEW)) {
      av8_throw_err(
        "* @func after(next)\n"
        "* @arg next {View}\n"
      );
    }
    av8_self(View);
    View* brother = Wrap<View>::Self(args[0]);
    try { self->after(brother); }
    catch (cError& err) { av8_throw_err(err); }
  }

  /**
   * @func move_to_before();
   */
  static void move_to_before(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    self->move_to_before();
  }

  /**
   * @func move_to_after();
   */
  static void move_to_after(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    self->move_to_after();
  }

  /**
   * @func move_to_first();
   */
  static void move_to_first(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    self->move_to_first();
  }

  /**
   * @func move_to_last();
   */
  static void move_to_last(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    self->move_to_last();
  }

  /**
   * @func remove()
   */
  static void remove(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    self->remove();
  }

  /**
   * @func remove_all_child()
   */
  static void remove_all_child(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    self->remove_all_child();
  }

  /**
   * @func focus()
   * @ret {bool}
   */
  static void focus(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->focus() );
  }

  /**
   * @func blur()
   * @ret {bool}
   */
  static void blur(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->blur() );
  }

  /**
   * @func layout_offset()
   * @ret {Vec2}
   */
  static void layout_offset(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    Vec2 rev = self->layout_offset();
    av8_return( worker->gui_value_program()->New(rev) );
  }
  
  /**
   * @func layout_offset_from([parents])
   * @arg [parents=parent] {View}
   * @ret {Vec2}
   */
  static void layout_offset_from(FunctionCall args) {
    av8_worker(args);
    View* target = nullptr;
    if ( args.Length() > 0 && worker->has_view(args[0]) ) {
      target = Wrap<View>::Self(args[0]);
    }
    av8_self(View);
    Vec2 rev = self->layout_offset_from(target);
    av8_return( worker->gui_value_program()->New(rev) );
  }

  /**
   * @func children(index)
   * @arg index {uint}
   * @ret {View}
   */
  static void children(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsUint32()) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    View* view = self->children( args[0]->ToUint32()->Value() );
    if ( view ) {
      Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
      av8_return( wrap->local() );
    } else {
      av8_return( worker->Null() );
    }
  }

  /**
   * @func get_action()
   * @ret {Action}
   */
  static void get_action(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    Action* action = self->action();
    if ( action ) {
      av8_return( Wrap<Action>::wrap(action)->local() );
    } else {
      av8_return_null();
    }
  }

  /**
   * @func set_action(action)
   * @arg action {Action}
   */
  static void set_action(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !worker->has<Action>(args[0]) ) {
      av8_throw_err(
        "* @func set_action(action)\n"
        "* @arg action {Action}\n"
      );
    }
    av8_self(View);
    Action* action = Wrap<Action>::Self(args[0]);
    av8_try_catch({
      self->action(action);
    }, Error)
  }

  /**
   * @func screen_rect()
   * @ret {Rect}
   */
  static void screen_rect(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->screen_rect()) );
  }

  /**
   * @func final_matrix()
   * @ret {Mat}
   */
  static void final_matrix(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->final_matrix()) );
  }

  /**
   * @func final_opacity()
   * @ret {float}
   */
  static void final_opacity(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->final_opacity() );
  }

  /**
   * @func position()
   * @ret {Vec2}
   */
  static void position(FunctionCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->position()) );
  }

  /**
   * @func overlap_test(point)
   * @arg point {Vec2}
   * @ret {bool}
   */
  static void overlap_test(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 ) {
      av8_throw_err(
        "* @func overlap_test(point)\n"
        "* @arg point {Vec2}\n"
        "* @ret {bool}\n"
      );
    }
    av8_gui_parse_value(Vec2, vec2, args[0], "View.overlap_test( %s )");
    av8_self(View);
    av8_return( self->overlap_test(out) );
  }

  /**
   * @func add_class(name)
   * @arg name {String}
   */
  static void add_class(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsString() ) {
      av8_throw_err(
        "* @func add_class(name)\n"
        "* @arg name {String}\n"
      );
    }
    av8_self(View);
    self->add_class( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func remove_class(name)
   * @arg name {String}
   */
  static void remove_class(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || ! args[0]->IsString() ) {
      av8_throw_err(
        "* @func remove_class(name)\n"
        "* @arg name {String}\n"
      );
    }
    av8_self(View);
    self->remove_class( worker->to_string_utf8(args[0]) );
  }

  /**
   * @func toggle_class(name)
   * @arg name {String}
   */
  static void toggle_class(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || ! args[0]->IsString() ) {
      av8_throw_err(
        "* @func toggle_class(name)\n"
        "* @arg name {String}\n"
      );
    }
    av8_self(View);
    self->toggle_class( worker->to_string_utf8(args[0]) );
  }
  
  // ----------------------------- get --------------------------------
  
  /**
   * @get children_count {uint}
   */
  static void children_count(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->children_count() );
  }

  /**
   * @get inner_text {String}
   */
  static void inner_text(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->inner_text() );
  }

  /**
   * @get id {String}
   */
  static void id(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->id() );
  }

  /**
   * @get controller {ViewController}
   */
  static void controller(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    ViewController* controller = self->controller();
    if ( ! controller) av8_return( worker->Null() );
    Wrap<ViewController>* wrap = av8_wrap(ViewController, controller);
    av8_return( worker->local(wrap->handle()) );
  }


  /**
   * @get top {View}
   */
  static void top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    View* view = self->top();
    if ( ! view) av8_return( worker->Null() );
    Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get top_ctr {ViewController}
   */
  static void top_ctr(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    ViewController* controller = self->top_ctr();
    if ( ! controller) av8_return( worker->Null() );
    Wrap<ViewController>* wrap = av8_wrap(ViewController, controller);
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get parent {View}
   */
  static void parent(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    View* view = self->parent();
    if ( ! view) av8_return( worker->Null() );
    Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get prev {View}
   */
  static void prev(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    View* view = self->prev();
    if ( ! view) av8_return( worker->Null() );
    Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get next {View}
   */
  static void next(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    View* view = self->next();
    if ( ! view) av8_return( worker->Null() );
    Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get first {View}
   */
  static void first(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    View* view = self->first();
    if ( ! view) av8_return( worker->Null() );
    Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get last {View}
   */
  static void last(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    View* view = self->last();
    if ( ! view) av8_return( worker->Null() );
    Wrap<View>* wrap = Wrap<View>::wrap(view, view->view_type());
    av8_return( worker->local(wrap->handle()) );
  }

  /**
   * @get x {float}
   */
  static void x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->x() );
  }

  /**
   * @get y {float}
   */
  static void y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->y() );
  }

  /**
   * @get scale_x {float}
   */
  static void scale_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->scale_y() );
  }

  /**
   * @get scale_y {float}
   */
  static void scale_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->scale_y() );
  }

  /**
   * @get rotate_z {float}
   */
  static void rotate_z(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->rotate_z() );
  }

  /**
   * @get skew_x {float}
   */
  static void skew_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->skew_x() );
  }

  /**
   * @get skew_y {float}
   */
  static void skew_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->skew_y() );
  }

  /**
   * @get opacity {float}
   */
  static void opacity(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->opacity() );
  }

  /**
   * @get visible {bool}
   */
  static void visible(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->visible() );
  }

  /**
   * @get final_visible {bool}
   */
  static void final_visible(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->final_visible() );
  }

  /**
   * @get translate {Vec2}
   */
  static void translate(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->translate()) );
  }

  /**
   * @get scale {Vec2}
   */
  static void scale(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->scale()) );
  }

  /**
   * @get skew {Vec2}
   */
  static void skew(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->skew()) );
  }

  /**
   * @get origin_x {float}
   */
  static void origin_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->origin_x() );
  }

  /**
   * @get origin_y {float}
   */
  static void origin_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->origin_y() );
  }

  /**
   * @get origin_y {Vec2}
   */
  static void origin(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->origin()) );
  }

  /**
   * @get matrix {Mat}
   */
  static void matrix(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( worker->gui_value_program()->New(self->matrix()) );
  }

  /**
   * @get level {uint}
   */
  static void level(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->level() );
  }

  /**
   * @get need_draw {bool}
   */
  static void need_draw(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->need_draw() );
  }

  /**
   * @get receive {bool}
   */
  static void receive(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->receive() );
  }

  /**
   * @get is_focus {bool}
   */
  static void is_focus(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->is_focus() );
  }

  /**
   * @get view_type {uint}
   */
  static void view_type(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    av8_return( self->view_type() );
  }

  /**
   * @get style {Object}
   */
  static void style(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_return( args.This() );
  }

  /**
   * @get classs {Object}
   */
  static void classs(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    CSSViewClasss* classs = self->classs();
    if ( classs ) {
      Local<v8::Value> rv = worker->NewObject();
      rv.As<v8::Object>()->Set( worker->strs()->name(), worker->New(classs->name()) );
      av8_return( rv );
    } else {
      av8_return_null();
    }
  }
  
  // ------------------------------------ set ----------------------------------
  
  /**
   * @set inner_text {String}
   */
  static void set_inner_text(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(View);
    Ucs2String str = worker->to_string_ucs2(value);
    av8_try_catch({
      self->inner_text(str);
    }, Error);
  }

  /**
   * @set id {String}
   */
  static void set_id(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(View);
    av8_try_catch({
      self->set_id(worker->to_string_utf8(value));
    }, Error);
  }

  /**
   * @set x {float}
   */
  static void set_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_x( value->ToNumber()->Value() );
  }

  /**
   * @set y {float}
   */
  static void set_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_y( value->ToNumber()->Value() );
  }

  /**
   * @set scale_x {float}
   */
  static void set_scale_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_scale_x( value->ToNumber()->Value() );
  }

  /**
   * @set scale_y {float}
   */
  static void set_scale_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_scale_y( value->ToNumber()->Value() );
  }

  /**
   * @set rotate_z {float}
   */
  static void set_rotate_z(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_rotate_z( value->ToNumber()->Value() );
  }

  /**
   * @set skew_x {float}
   */
  static void set_skew_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_skew_x( value->ToNumber()->Value() );
  }

  /**
   * @set skew_y {float}
   */
  static void set_skew_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_skew_y( value->ToNumber()->Value() );
  }

  /**
   * @set opacity {float}
   */
  static void set_opacity(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_opacity( value->ToNumber()->Value() );
  }

  /**
   * @set translate {Vec2}
   */
  static void set_translate(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "View.xy = %s");
    av8_self(View);
    self->set_translate( out );
  }

  /**
   * @set scale {Vec2}
   */
  static void set_scale(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "View.scale = %s");
    av8_self(View);
    self->set_scale( out );
  }

  /**
   * @set skew {Vec2}
   */
  static void set_skew(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "View.skew = %s");
    av8_self(View);
    self->set_skew( out );
  }

  /**
   * @set origin_x {float}
   */
  static void set_origin_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_origin_x( value->ToNumber()->Value() );
  }

  /**
   * @set origin_y {float}
   */
  static void set_origin_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->set_origin_y( value->ToNumber()->Value() );
  }

  /**
   * @set origin {Vec2}
   */
  static void set_origin(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "View.origin = %s");
    av8_self(View);
    self->set_origin( out );
  }

  /**
   * @set visible {bool}
   */
  static void set_visible(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(View);
    self->set_visible( value->ToBoolean()->Value() );
  }

  /**
   * @set need_draw {bool}
   */
  static void set_need_draw(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(View);
    self->set_need_draw( value->ToBoolean()->Value() );
  }

  /**
   * @set receive {bool}
   */
  static void set_receive(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(View);
    self->set_receive( value->ToBoolean()->Value() );
  }

  /**
   * @set style {Object}
   */
  static void set_style(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_handle_scope();
    
    if ( value->IsObject() && ! value->IsNull() ) {
      Local<v8::Object> arg = value.As<v8::Object>();
      Local<v8::Array> names = arg->GetPropertyNames();
      Local<v8::Object> handle = args.This();
      
      for ( uint i = 0, len = names->Length(); i < len; i++ ) {
        Local<v8::Value> key = names->Get(i);
        Local<v8::Value> val;
        if ( ! arg->Get(worker->context(), key).ToLocal(&val) ||
            ! handle->Set(worker->context(), key, val).FromMaybe(false) ) { // js error
          return;
        }
      }
    }
  }

  /**
   * @set classs {String}
   */
  static void set_classs(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsString() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(View);
    self->classs( worker->to_string_utf8(value) );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
#define set_field(enum, class, name) av8_set_field(enum, View::enum);
    av_each_all_view(set_field)
#undef set_field
    av8_binding_class(View, constructor, {
      // method
      av8_set_cls_method(prepend, prepend);
      av8_set_cls_method(append, append);
      av8_set_cls_method(append_text, append_text);
      av8_set_cls_method(append_to, append_to);
      av8_set_cls_method(before, before);
      av8_set_cls_method(after, after);
      av8_set_cls_method(move_to_before, move_to_before);
      av8_set_cls_method(move_to_after, move_to_after);
      av8_set_cls_method(move_to_first, move_to_first);
      av8_set_cls_method(move_to_last, move_to_last);
      av8_set_cls_method(remove, remove);
      av8_set_cls_method(remove_all_child, remove_all_child);
      av8_set_cls_method(focus, focus);
      av8_set_cls_method(blur, blur);
      av8_set_cls_method(layout_offset, layout_offset);
      av8_set_cls_method(layout_offset_from, layout_offset_from);
      av8_set_cls_method(children, children);
      av8_set_cls_method(get_action, get_action);
      av8_set_cls_method(set_action, set_action);
      av8_set_cls_method(screen_rect, screen_rect);
      av8_set_cls_method(final_matrix, final_matrix);
      av8_set_cls_method(final_opacity, final_opacity);
      av8_set_cls_method(position, position);
      av8_set_cls_method(overlap_test, overlap_test);
      av8_set_cls_method(add_class, add_class);
      av8_set_cls_method(remove_class, remove_class);
      av8_set_cls_method(toggle_class, toggle_class);
      // property
      av8_set_cls_property(inner_text, inner_text, set_inner_text);
      av8_set_cls_property(children_count, children_count);
      av8_set_cls_property(id, id, set_id);
      av8_set_cls_property(controller, controller);
      av8_set_cls_property(ctr, controller);
      av8_set_cls_property(top, top);
      av8_set_cls_property(top_ctr, top_ctr);
      av8_set_cls_property(parent, parent);
      av8_set_cls_property(prev, prev);
      av8_set_cls_property(next, next);
      av8_set_cls_property(first, first);
      av8_set_cls_property(last, last);
      av8_set_cls_property(x, x, set_x);
      av8_set_cls_property(y, y, set_y);
      av8_set_cls_property(scale_x, scale_x, set_scale_x);
      av8_set_cls_property(scale_y, scale_y, set_scale_y);
      av8_set_cls_property(rotate_z, rotate_z, set_rotate_z);
      av8_set_cls_property(skew_x, skew_x, set_skew_x);
      av8_set_cls_property(skew_y, skew_y, set_skew_y);
      av8_set_cls_property(opacity, opacity, set_opacity);
      av8_set_cls_property(visible, visible, set_visible);
      av8_set_cls_property(final_visible, final_visible);
      av8_set_cls_property(translate, translate, set_translate);
      av8_set_cls_property(scale, scale, set_scale);
      av8_set_cls_property(skew, skew, set_skew);
      av8_set_cls_property(origin_x, origin_x, set_origin_x);
      av8_set_cls_property(origin_y, origin_y, set_origin_y);
      av8_set_cls_property(origin, origin, set_origin);
      av8_set_cls_property(matrix, matrix);
      av8_set_cls_property(level, level);
      av8_set_cls_property(need_draw, need_draw, set_need_draw);
      av8_set_cls_property(receive, receive, set_receive);
      av8_set_cls_property(is_focus, is_focus);
      av8_set_cls_property(view_type, view_type);
      av8_set_cls_property(style, style, set_style);
      av8_set_cls_property(class, classs, set_classs);
      av8_set_cls_property(classs, classs, set_classs);
    }, nullptr);
    worker->native_constructors()->set_alias(av8_typeid(View), View::VIEW);
  }
};

void binding_view(Local<v8::Object> exports, Worker* worker) {
  WrapNativeViewController::binding(exports, worker);
  WrapView::binding(exports, worker);
}

av8_nsd
