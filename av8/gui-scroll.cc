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
#include "agui/scroll.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

#define av8_scroll_self() BasicScroll* self = dynamic_cast<BasicScroll*>(Wrap<View>::Self(args))

class WrapBasicScroll {

  /**
   * @func scroll_to(scroll[,duration[,curve]])
   * @arg scroll {Vec2}
   * @arg [duration] {uint} ms
   * @arg [curve] {Curve}
   */
  static void scroll_to(FunctionCall args) {
    av8_worker(args);
    int64 duration = 0;
    
    if ( args.Length() == 0 ) {
      av8_throw_err(
        "* @func scroll_to(scroll[,duration[,curve]])\n"
        "* @arg scroll {Vec2}\n"
        "* @arg [duration] {uint} ms\n"
        "* @arg [curve] {Curve}\n"
      );
    }
    
    av8_gui_parse_value(Vec2, vec2, args[0], "BasicScroll.scroll(%s)");
    
    Vec2 scroll = out;
    
    av8_scroll_self();
    
    if ( args.Length() > 1 && args[1]->IsNumber() ) {
      duration = args[1]->ToNumber()->Value() / 1000;
      duration = av_max(duration, 0);
    }
    
    if ( args.Length() > 2 ) {
      av8_gui_parse_value(Curve, curve, args[2], "BasicScroll.scroll(vec2, %s)");
      self->scroll_to(scroll, duration, out);
      return;
    }
    
    self->scroll_to(scroll, duration);
  }
  
  static void scroll(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( worker->gui_value_program()->New(self->scroll()) );
  }
  
  static void set_scroll(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "BasicScroll.scroll = %s");
    av8_scroll_self();
    self->set_scroll(out);
  }
  
  static void scroll_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scroll_x() );
  }
  
  static void scroll_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scroll_y() );
  }
  
  /**
   * @set scroll_x {float} 
   */
  static void set_scroll_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set scroll_x {float} ");
    av8_scroll_self();
    self->set_scroll_x(value->ToNumber()->Value());
  }
  
  /**
   * @set scroll_y {float} 
   */
  static void set_scroll_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set scroll_y {float} ");
    av8_scroll_self();
    self->set_scroll_y(value->ToNumber()->Value());
  }
  
  static void scroll_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scroll_width() );
  }
  
  static void scroll_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scroll_height() );
  }
  
  static void scrollbar(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scrollbar() );
  }
  
  static void set_scrollbar(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_scroll_self();
    self->set_scrollbar(value->ToBoolean()->Value());
  }
  
  static void resistance(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->resistance() );
  }
  
  /**
   * @set resistance {float} 
   */
  static void set_resistance(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set resistance {float} ");
    av8_scroll_self();
    self->set_resistance(value->ToNumber()->Value());
  }
  
  static void bounce(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->bounce() );
  }
  
  static void set_bounce(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_scroll_self();
    self->set_bounce(value->ToBoolean()->Value());
  }
  
  static void bounce_lock(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->bounce_lock() );
  }
  
  static void set_bounce_lock(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_scroll_self();
    self->set_bounce_lock(value->ToBoolean()->Value());
  }
  
  static void momentum(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->momentum() );
  }
  
  static void set_momentum(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_scroll_self();
    self->set_momentum(value->ToBoolean()->Value());
  }
  
  static void lock_direction(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->lock_direction() );
  }
  
  static void set_lock_direction(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_scroll_self();
    self->set_lock_direction(value->ToBoolean()->Value());
  }
  
  static void catch_position_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->catch_position_x() );
  }
  
  static void catch_position_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->catch_position_y() );
  }
  
  /**
   * @set catch_position_x {float} 
   */
  static void set_catch_position_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set catch_position_x {float} ");
    av8_scroll_self();
    self->set_catch_position_x(value->ToNumber()->Value());
  }
  
  /**
   * @set catch_position_y {float} 
   */
  static void set_catch_position_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set catch_position_y {float} ");
    av8_scroll_self();
    self->set_catch_position_y(value->ToNumber()->Value());
  }
  
  static void scrollbar_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( worker->gui_value_program()->New(self->scrollbar_color()) );
  }
  
  static void set_scrollbar_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "BasicScroll.scrollbar_color = %s");
    av8_scroll_self();
    self->set_scrollbar_color(out);
  }
  
  static void h_scrollbar(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->h_scrollbar() );
  }
  
  static void v_scrollbar(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->v_scrollbar() );
  }
  
  static void scrollbar_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scrollbar_width() );
  }
  
  /**
   * @set scrollbar_width {float} 
   */
  static void set_scrollbar_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set scrollbar_width {float} ");
    av8_scroll_self();
    self->set_scrollbar_width( value->ToNumber()->Value() );
  }
  
  static void scrollbar_margin(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->scrollbar_margin() );
  }
  
  /**
   * @set scrollbar_margin {float} 
   */
  static void set_scrollbar_margin(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set scrollbar_margin {float} ");
    av8_scroll_self();
    self->set_scrollbar_margin( value->ToNumber()->Value() );
  }
  
  static void default_scroll_duration(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( self->default_scroll_duration() / 1000 );
  }
  
  /**
   * @set default_scroll_duration {uint} ms
   */
  static void set_default_scroll_duration(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) av8_throw_err("* @set default_scroll_duration {uint} ms");
    av8_scroll_self();
    self->set_default_scroll_duration( av_max(value->ToNumber()->Value(), 0) );
  }
  
  static void default_scroll_curve(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_scroll_self();
    av8_return( worker->gui_value_program()->New(self->default_scroll_curve()) );
  }
  
  static void set_default_scroll_curve(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Curve, curve, value, "BasicScroll.default_scroll_curve = %s");
    av8_scroll_self();
    self->set_default_scroll_curve(out);
  }
  
  static void termination_motion(FunctionCall args) {
    av8_worker(args);
    av8_scroll_self();
    self->termination_motion();
  }

public:
  static void inherit(Local<FunctionTemplate> cls, Worker* worker) {
    av8_set_cls_method(scroll_to, scroll_to);
    av8_set_cls_method(termination_motion, termination_motion);
    av8_set_cls_property(scroll, scroll, set_scroll);
    av8_set_cls_property(scroll_x, scroll_x, set_scroll_x);
    av8_set_cls_property(scroll_y, scroll_y, set_scroll_y);
    av8_set_cls_property(scroll_width, scroll_width);
    av8_set_cls_property(scroll_height, scroll_height);
    av8_set_cls_property(scrollbar, scrollbar, set_scrollbar);
    av8_set_cls_property(resistance, resistance, set_resistance);
    av8_set_cls_property(bounce, bounce, set_bounce);
    av8_set_cls_property(bounce_lock, bounce_lock, set_bounce_lock);
    av8_set_cls_property(momentum, momentum, set_momentum);
    av8_set_cls_property(lock_direction, lock_direction, set_lock_direction);
    av8_set_cls_property(catch_position_x, catch_position_x, set_catch_position_x);
    av8_set_cls_property(catch_position_y, catch_position_y, set_catch_position_y);
    av8_set_cls_property(scrollbar_color, scrollbar_color, set_scrollbar_color);
    av8_set_cls_property(h_scrollbar, h_scrollbar);
    av8_set_cls_property(v_scrollbar, v_scrollbar);
    av8_set_cls_property(scrollbar_width, scrollbar_width, set_scrollbar_width);
    av8_set_cls_property(scrollbar_margin, scrollbar_margin, set_scrollbar_margin);
    av8_set_cls_property(default_scroll_duration, default_scroll_duration, set_default_scroll_duration);
    av8_set_cls_property(default_scroll_curve, default_scroll_curve, set_default_scroll_curve);
  }
};

class WrapScroll: public BasicWrapView {
public:

  virtual bool add_event_listener(cString& name, cString& func, int id) {
    if ( name == GUI_EVENT_SCROLL.to_string() ) {
      return ViewUtil::add_event_listener(reinterpret_cast<Wrap<View>*>(this), GUI_EVENT_SCROLL, func, id);
    } else {
      return ViewUtil::panel_add_event_listener(reinterpret_cast<Wrap<View>*>(this), name, func, id);
    }
  }
  
  virtual bool remove_event_listener(cString& name, int id) {
    if ( name == GUI_EVENT_SCROLL.to_string() ) {
      return ViewUtil::remove_event_listener(reinterpret_cast<Wrap<View>*>(this), GUI_EVENT_SCROLL, id);
    } else {
      return ViewUtil::panel_remove_event_listener(reinterpret_cast<Wrap<View>*>(this), name, id);
    }
  }
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapScroll>(args, new Scroll());
  }
  
  static void switch_motion_margin_left(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( self->switch_motion_margin_left() );
  }
  
  static void switch_motion_margin_right(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( self->switch_motion_margin_right() );
  }
  
  static void switch_motion_margin_top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( self->switch_motion_margin_top() );
  }
  
  static void switch_motion_margin_bottom(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( self->switch_motion_margin_bottom() );
  }
  
  static void switch_motion_align_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( worker->gui_value_program()->New( self->switch_motion_align_x() ) );
  }
  
  static void switch_motion_align_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( worker->gui_value_program()->New( self->switch_motion_align_y() ) );
  }
  
  /**
   * @set switch_motion_margin_left {float}
   */
  static void set_switch_motion_margin_left(Local<v8::String> name,
                                            Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(Scroll);
    self->switch_motion_margin_left( value->ToNumber()->Value() );
  }
  
  /**
   * @set switch_motion_margin_right {float}
   */
  static void set_switch_motion_margin_right(Local<v8::String> name,
                                             Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(Scroll);
    self->switch_motion_margin_right( value->ToNumber()->Value() );
  }
  
  /**
   * @set switch_motion_margin_top {float}
   */
  static void set_switch_motion_margin_top(Local<v8::String> name,
                                           Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(Scroll);
    self->switch_motion_margin_top( value->ToNumber()->Value() );
  }
  
  /**
   * @set switch_motion_margin_bottom {float}
   */
  static void set_switch_motion_margin_bottom(Local<v8::String> name,
                                              Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(Scroll);
    self->switch_motion_margin_bottom( value->ToNumber()->Value() );
  }
  
  /**
   * @set switch_motion_align_x {float}
   */
  static void set_switch_motion_align_x(Local<v8::String> name,
                                        Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Align, align, value, "BasicScroll.switch_motion_align_x = %s");
    av8_self(Scroll);
    self->set_switch_motion_align_x(out);
  }
  
  static void set_switch_motion_align_y(Local<v8::String> name,
                                        Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Align, align, value, "BasicScroll.switch_motion_align_y = %s");
    av8_self(Scroll);
    self->set_switch_motion_align_y(out);
  }
  
  static void enable_switch_motion(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( self->enable_switch_motion() );
  }
  
  static void set_enable_switch_motion(Local<v8::String> name,
                                       Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Scroll);
    self->set_enable_switch_motion( value->ToBoolean()->Value() );
  }
  
  static void enable_fixed_scroll_size(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Scroll);
    av8_return( self->enable_fixed_scroll_size() );
  }
  
  static void set_enable_fixed_scroll_size(Local<v8::String> name,
                                           Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "BasicScroll.enable_fixed_scroll_size = %s");
    av8_self(Scroll);
    self->set_enable_fixed_scroll_size( out );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Scroll, constructor, {
      av8_set_cls_property(switch_motion_margin_left,
                          switch_motion_margin_left, set_switch_motion_margin_left);
      av8_set_cls_property(switch_motion_margin_right,
                          switch_motion_margin_right, set_switch_motion_margin_right);
      av8_set_cls_property(switch_motion_margin_top,
                          switch_motion_margin_top, set_switch_motion_margin_top);
      av8_set_cls_property(switch_motion_margin_bottom,
                          switch_motion_margin_bottom, set_switch_motion_margin_bottom);
      av8_set_cls_property(switch_motion_align_x, switch_motion_align_x, set_switch_motion_align_x);
      av8_set_cls_property(switch_motion_align_y, switch_motion_align_y, set_switch_motion_align_y);
      av8_set_cls_property(enable_switch_motion, enable_switch_motion, set_enable_switch_motion);
      av8_set_cls_property(enable_fixed_scroll_size,
                          enable_fixed_scroll_size, set_enable_fixed_scroll_size);
      WrapBasicScroll::inherit(cls, worker);
    }, Panel);
    worker->native_constructors()->set_alias(av8_typeid(Scroll), View::SCROLL);
  }
};

void ViewUtil::inherit_scroll(Local<FunctionTemplate> cls, Worker* worker) {
  WrapBasicScroll::inherit(cls, worker);
}

void binding_scroll(Local<v8::Object> exports, Worker* worker) {
  WrapScroll::binding(exports, worker);
}

av8_nsd
