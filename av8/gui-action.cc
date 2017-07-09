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
#include "agui/action.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapAction
 */
class WrapAction: public WrapBase {
public:

  static void constructor(FunctionCall args) {
    av8_worker(args);
    av8_throw_err("Forbidden access abstract");
  }
  
  /**
   * @func play()
   */
  static void play(FunctionCall args) {
    av8_worker(args);
    av8_self(Action);
    self->play();
  }
  
  /**
   * @func stop()
   */
  static void stop(FunctionCall args) {
    av8_worker(args);
    av8_self(Action);
    self->stop();
  }
  
  /**
   * @func seek(ms)
   * @arg ms {uint}
   */
  static void seek(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func seek(ms)\n"
        "* @arg ms {uint}\n"
      );
    }
    av8_self(Action);
    self->seek( uint64(1000) * args[0]->ToUint32()->Value() );
  }
  
  /**
   * @func seek_play(ms)
   * @arg ms {uint}
   */
  static void seek_play(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func seek_play(ms)\n"
        "* @arg ms {uint}\n"
      );
    }
    av8_self(Action);
    self->seek_play( uint64(1000) * args[0]->ToUint32()->Value() );
  }
  
  /**
   * @func seek_stop(ms)
   * @arg ms {uint}
   */
  static void seek_stop(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func seek_stop(ms)\n"
        "* @arg ms {uint}\n"
      );
    }
    av8_self(Action);
    self->seek_play( uint64(1000) * args[0]->ToUint32()->Value() );
  }
  
  /**
   * @func clear()
   */
  static void clear(FunctionCall args) {
    av8_worker(args);
    av8_self(Action);
    self->clear();
  }
  
  /** 
   * @get loop {uint}
   */
  static void loop(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    av8_return( self->loop() );
  }

  /** 
   * @get delay {uint} ms
   */
  static void delay(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    av8_return( self->delay() / 1000 );
  }

  /** 
   * @get delayed {int} ms
   */
  static void delayed(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    av8_return( self->delayed() / 1000 );
  }

  /** 
   * @get speed {float} 0.1-10
   */
  static void speed(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    av8_return( self->speed() );
  }

  /** 
   * @get speed {bool}
   */
  static void playing(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    av8_return( self->playing() );
  }

  /** 
   * @get duration {uint} ms
   */
  static void duration(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    av8_return( self->duration() / 1000 );
  }

  /** 
   * @get parent {Action}
   */
  static void parent(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Action);
    Action* action = self->parent();
    if ( action ) {
      Wrap<Action>* wrap = Wrap<Action>::wrap(action);
      av8_return( wrap->local() );
    } else {
      av8_return_null();
    }
  }

  /**
   * @set playing {bool}
   */
  static void set_playing(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Action);
    self->playing( value->ToBoolean()->Value() );
  }

  /**
   * @set loop {uint}
   */
  static void set_loop(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsUint32() ) {
      av8_throw_err(
        
        "* @set loop {uint}\n"
        
      );
    }
    av8_self(Action);
    self->loop( value->ToUint32()->Value() );
  }

  /**
   * @set delay {uint} ms
   */
  static void set_delay(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsUint32() ) {
      av8_throw_err(
        "* @set delay {uint} ms\n"
      );
    }
    av8_self(Action);
    self->delay( uint64(1000) * value->ToUint32()->Value() );
  }

  /**
   * @set speed {float} 0.1-10
   */
  static void set_speed(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err(
        "* @set speed {float} 0.1-10\n"
      );
    }
    av8_self(Action);
    self->speed( value->ToNumber()->Value() );
  }

  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Action, constructor, {
      av8_set_cls_method(play, play);
      av8_set_cls_method(stop, stop);
      av8_set_cls_method(seek, seek);
      av8_set_cls_method(seek_play, seek_play);
      av8_set_cls_method(seek_stop, seek_stop);
      av8_set_cls_method(clear, clear);
      av8_set_cls_property(delayed, delayed);
      av8_set_cls_property(duration, duration);
      av8_set_cls_property(parent, parent);
      av8_set_cls_property(playing, playing, set_playing);
      av8_set_cls_property(loop, loop, set_loop);
      av8_set_cls_property(delay, delay, set_delay);
      av8_set_cls_property(speed, speed, set_speed);
    }, nullptr);
  }
};

/**
 * @class WrapGroupAction
 */
class WrapGroupAction: public WrapBase {
public:

  static void constructor(FunctionCall args) {
    av8_worker(args);
    av8_throw_err("Forbidden access abstract");
  }
  
  /**
   * @get length {uint}
   */
  static void length(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GroupAction);
    av8_return( self->length() );
  }
  
  /**
   * @func append(child)
   * @arg child {Action}
   */
  static void append(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !worker->has<Action>(args[0]) ) {
      av8_throw_err(
        "* @func append(child)\n"
        "* @arg child {Action}\n"
      );
    }
    av8_self(GroupAction);
    Action* child = Wrap<Action>::Self(args[0]);
    self->append( child );
  }
  
  /**
   * @func insert(index, child)
   * @arg index {uint}
   * @arg child {Action}
   */
  static void insert(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 2 || !args[0]->IsUint32() || !worker->has<Action>(args[1]) ) {
      av8_throw_err(
        "* @func insert(index, child)\n"
        "* @arg index {uint}\n"
        "* @arg child {Action}\n"
      );
    }
    av8_self(GroupAction);
    Action* child = Wrap<Action>::Self(args[1]);
    self->insert( args[0]->ToUint32()->Value(), child );
  }
  
  /**
   * @func remove_child(index)
   * @arg index {uint}
   */
  static void remove_child(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func remove_child(index)\n"
        "* @arg index {uint}\n"
      );
    }
    av8_self(GroupAction);
    self->remove_child( args[0]->ToUint32()->Value() );
  }
  
  static void children(FunctionCall args, cchar* argument) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_throw_err(argument);
    }
    av8_self(GroupAction);
    
    uint index = args[0]->ToUint32()->Value();
    if ( index < self->length() ) {
      Action* action = (*self)[ args[0]->ToUint32()->Value() ];
      Wrap<Action>* wrap = Wrap<Action>::wrap(action);
      av8_return( wrap->local() );
    } else {
      av8_return_null();
    }
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class_no_exports(GroupAction, constructor, {
      av8_set_cls_property(length, length);
      av8_set_cls_method(append, append);
      av8_set_cls_method(insert, insert);
      av8_set_cls_method(remove_child, remove_child);
    }, Action);
  }
};

/**
 * @class WrapSpawnAction
 */
class WrapSpawnAction: public WrapBase {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapSpawnAction>(args, new SpawnAction());
  }
  
  /**
   * @func spawn(index)
   * @arg index {uint}
   * @ret {Action} return child action
   */
  static void spawn(FunctionCall args) {
    WrapGroupAction::children(args,
                              "* @func spawn(index)\n"
                              "* @arg index {uint}\n"
                              "* @ret {Action} return child action\n"
                              );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(SpawnAction, constructor, {
      av8_set_cls_method(spawn, spawn);
    }, GroupAction);
  }
};

/**
 * @class WrapSequenceAction
 */
class WrapSequenceAction: public WrapBase {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapSequenceAction>(args, new SequenceAction());
  }
  
  /**
   * @func seq(index)
   * @arg index {uint}
   * @ret {Action} return child action
   */
  static void seq(FunctionCall args) {
    WrapGroupAction::children(args,
                              "* @func seq(index)\n"
                              "* @arg index {uint}\n"
                              "* @ret {Action} return child action\n"
                              );
  }

  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(SequenceAction, constructor, {
      av8_set_cls_method(seq, seq);
    }, GroupAction);
  }
};

typedef KeyframeAction::Frame Frame;

#define def_get_property(Name) \
static void Name(Local<v8::String> name, PropertyCall args) { \
  av8_worker(args); \
  av8_self(Frame); \
  if (self->host()) av8_return( self->Name() ); \
}

#define def_set_property(Name)\
static void set_##Name(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) { \
  av8_worker(args); \
  av8_self(Frame); \
  if (self->host()) { \
    if ( ! value->IsNumber() ) { av8_throw_err("Bad argument."); } \
    self->set_##Name( value->ToNumber()->Value() ); \
  }\
}

#define def_get_property_from_type(Name)\
static void Name(Local<v8::String> name, PropertyCall args) {\
  av8_worker(args);\
  av8_self(Frame);\
  if (self->host()) av8_return( worker->gui_value_program()->New(self->Name()) );\
}

#define def_set_property_from_type(Name, Type, parse_func)\
static void set_##Name(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {\
  av8_worker(args);\
  av8_self(Frame);\
  if (self->host()) {\
    av8_gui_parse_value(Type, parse_func, value, "Action."#Name" = %s");\
    self->set_##Name(out);\
  }\
}

#define def_property(Name) def_get_property(Name) def_set_property(Name)

#define def_property_from_type(Name, Type, parse_func) \
def_get_property_from_type(Name)\
def_set_property_from_type(Name, Type, parse_func)

/**
 * @class WrapFrame
 */
class WrapFrame: public WrapBase {
public: typedef Frame Type;

  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Forbidden access abstract");
  }
  
  /**
   * @func fetch([view]) fetch style attribute by view
   * @arg [view] {View}
   */
  static void fetch(FunctionCall args) {
    av8_worker(args);
    View* view = nullptr;
    if ( args.Length() > 0 && worker->has(args[0], View::VIEW) ) {
      view = Wrap<View>::Self(args[0]);
    }
    av8_self(Frame);
    self->fetch(view);
  }

  /**
   * @func flush() flush frame restore default values
   */
  static void flush(FunctionCall args) {
    av8_worker(args);
    av8_self(Frame);
    self->flush();
  }

  /**
   * @get index {uint} frame index in action
   */
  static void index(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Frame);
    av8_return( self->index() );
  }

  /**
   * @get time {uint} ms
   */
  static void time(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Frame);
    av8_return( self->time() / 1000 );
  }

  /**
   * @set time {uint} ms
   */
  static void set_time(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err(
        "* @set time {uint} ms\n"
      );
    }
    av8_self(Frame);
    self->set_time(uint64(1000) * value->ToNumber()->Value());
  }

  /**
   * @get host {KeyframeAction}
   */
  static void host(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Frame);
    KeyframeAction* host = self->host();
    if ( host ) {
      av8_return( Wrap<KeyframeAction>::wrap(host)->local() );
    } else {
      av8_return_null();
    }
  }
  
  def_property_from_type(curve, FixedCubicBezier, curve)
  
  // -------------------- get/set property --------------------
  
  def_property_from_type(translate, Vec2, vec2);
  def_property_from_type(scale, Vec2, vec2);
  def_property_from_type(skew, Vec2, vec2);
  def_property_from_type(origin, Vec2, vec2);
  def_set_property_from_type(margin, Value, value);
  def_set_property_from_type(border, Border, border);
  def_set_property(border_width);
  def_set_property_from_type(border_color, Color, color);
  def_set_property(border_radius);
  def_property_from_type(min_width, Value, value);
  def_property_from_type(min_height, Value, value);
  def_property_from_type(start, Vec2, vec2);
  def_property_from_type(ratio, Vec2, vec2);
  def_set_property_from_type(align, Align, align);

  // --------------------
  
  static void width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Frame);
    KeyframeAction* host = self->host();
    if ( host ) {
      if ( host->is_bind_view() ) {
        if ( host->match_property(PROPERTY_WIDTH) ) {
          av8_return( worker->gui_value_program()->New(self->width()) );
        } else {
          av8_return( self->width2() );
        }
      } else {
        if ( host->has_property(PROPERTY_WIDTH) ) {
          av8_return( worker->gui_value_program()->New(self->width()) );
        } else {
          av8_return( self->width2() );
        }
      }
    }
  }
  static void height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Frame);
    KeyframeAction* host = self->host();
    if ( host ) {
      if ( host->is_bind_view() ) {
        if ( host->match_property(PROPERTY_HEIGHT) ) {
          av8_return( worker->gui_value_program()->New(self->height()) );
        } else {
          av8_return( self->height2() );
        }
      } else {
        if ( host->has_property(PROPERTY_HEIGHT) ) {
          av8_return( worker->gui_value_program()->New(self->height()) );
        } else {
          av8_return( self->height2() );
        }
      }
    }
  }
  static void set_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Frame);
    KeyframeAction* host = self->host();
    if ( host ) {
      if ( host->is_bind_view() ) {
        if ( host->match_property(PROPERTY_WIDTH) ) {
          av8_gui_parse_value(Value, value, value, "Action.width");
          self->set_width(out);
        } else {
          if ( ! value->IsNumber() ) {
            av8_throw_err("* @set width {float}");
          }
          self->set_width2( value->ToNumber()->Value() );
        }
      } else {
        av8_gui_parse_value(Value, value, value, "Action.width");
        self->set_width(out);
        if ( out.type == ValueType::PIXEL ) {
          self->set_width2(out.value);
        }
      }
    }
  }
  static void set_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Frame);
    KeyframeAction* host = self->host();
    if ( host ) {
      if ( host->is_bind_view() ) {
        if ( host->match_property(PROPERTY_HEIGHT) ) {
          av8_gui_parse_value(Value, value, value, "Action.height = %s");
          self->set_height(out);
        } else {
          if ( ! value->IsNumber() ) {
            av8_throw_err("* @set height {float}");
          }
          self->set_height2( value->ToNumber()->Value() );
        }
      } else {
        av8_gui_parse_value(Value, value, value, "Action.height = %s");
        self->set_height(out);
        if ( out.type == ValueType::PIXEL ) {
          self->set_height2(out.value);
        }
      }
    }
  }
  
  def_property(x);
  def_property(y);
  def_property(scale_x);
  def_property(scale_y);
  def_property(skew_x);
  def_property(skew_y);
  def_property(origin_x);
  def_property(origin_y);
  def_property(rotate_z);
  def_property(opacity);
  def_property_from_type(visible, bool, bool);
  def_property_from_type(margin_left, Value, value);
  def_property_from_type(margin_top, Value, value);
  def_property_from_type(margin_right, Value, value);
  def_property_from_type(margin_bottom, Value, value);
  def_property_from_type(border_left, Border, border);
  def_property_from_type(border_top, Border, border);
  def_property_from_type(border_right, Border, border);
  def_property_from_type(border_bottom, Border, border);
  def_property(border_left_width);
  def_property(border_top_width);
  def_property(border_right_width);
  def_property(border_bottom_width);
  def_property_from_type(border_left_color, Color, color);
  def_property_from_type(border_top_color, Color, color);
  def_property_from_type(border_right_color, Color, color);
  def_property_from_type(border_bottom_color, Color, color);
  def_property(border_radius_left_top);
  def_property(border_radius_right_top);
  def_property(border_radius_right_bottom);
  def_property(border_radius_left_bottom);
  def_property_from_type(background_color, Color, color);
  def_property_from_type(newline, bool, bool);
  def_property_from_type(content_align, ContentAlign, content_align);
  def_property_from_type(text_align, TextAlign, text_align);
  def_property_from_type(max_width, Value, value);
  def_property_from_type(max_height, Value, value);
  def_property(start_x);
  def_property(start_y);
  def_property(ratio_x);
  def_property(ratio_y);
  def_property_from_type(repeat, Repeat, repeat);
  def_property_from_type(text_background_color, ColorValue, color_value);
  def_property_from_type(text_color, ColorValue, color_value);
  def_property_from_type(text_size, TextSizeValue, text_size_value);
  def_property_from_type(text_style, TextStyleValue, text_style_value);
  def_property_from_type(text_family, TextFamilyValue, text_family_value);
  def_property_from_type(text_line_height, TextLineHeightValue, text_line_height_value);
  def_property_from_type(text_shadow, TextShadowValue, text_shadow_value);
  def_property_from_type(text_decoration, TextDecorationValue, text_decoration_value);
  def_property_from_type(text_overflow, TextOverflowValue, text_overflow_value);
  def_property_from_type(text_white_space, TextWhiteSpaceValue, text_white_space_value);
  def_property_from_type(align_x, Align, align);
  def_property_from_type(align_y, Align, align);
  def_property_from_type(shadow, ShadowValue, shadow);
  def_property_from_type(src, String, string);
  def_property_from_type(background_image, String, string);

public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Frame, constructor, {
      av8_set_cls_method(fetch, fetch);
      av8_set_cls_method(flush, flush);
      av8_set_cls_property(index, index);
      av8_set_cls_property(time, time, set_time);
      av8_set_cls_property(host, host);
      av8_set_cls_property(curve, curve, set_curve);
      //
      av8_set_cls_property(translate, translate, set_translate);
      av8_set_cls_property(scale, scale, set_scale);
      av8_set_cls_property(skew, skew, set_skew);
      av8_set_cls_property(origin, origin, set_origin);
      av8_set_cls_property(margin, nullptr, set_margin);
      av8_set_cls_property(border, nullptr, set_border);
      av8_set_cls_property(border_width, nullptr, set_border_width);
      av8_set_cls_property(border_color, nullptr, set_border_color);
      av8_set_cls_property(border_radius, nullptr, set_border_radius);
      av8_set_cls_property(min_width, min_width, set_min_width);
      av8_set_cls_property(min_height, min_height, set_min_height);
      av8_set_cls_property(start, start, set_start);
      av8_set_cls_property(ratio, ratio, set_ratio);
      av8_set_cls_property(align, nullptr, set_align);
      // style property
      av8_set_cls_property(x, x, set_x);
      av8_set_cls_property(y, y, set_y);
      av8_set_cls_property(scale_x, scale_x, set_scale_x);
      av8_set_cls_property(scale_y, scale_y, set_scale_y);
      av8_set_cls_property(skew_x, skew_x, set_skew_x);
      av8_set_cls_property(skew_y, skew_y, set_skew_y);
      av8_set_cls_property(origin_x, origin_x, set_origin_x);
      av8_set_cls_property(origin_y, origin_y, set_origin_y);
      av8_set_cls_property(rotate_z, rotate_z, set_rotate_z);
      av8_set_cls_property(opacity, opacity, set_opacity);
      av8_set_cls_property(visible, visible, set_visible);
      av8_set_cls_property(width, width, set_width);
      av8_set_cls_property(height, height, set_height);
      av8_set_cls_property(margin_left, margin_left, set_margin_left);
      av8_set_cls_property(margin_top, margin_top, set_margin_top);
      av8_set_cls_property(margin_right, margin_right, set_margin_right);
      av8_set_cls_property(margin_bottom, margin_bottom, set_margin_bottom);
      av8_set_cls_property(border_left, border_left, set_border_left);
      av8_set_cls_property(border_top, border_top, set_border_top);
      av8_set_cls_property(border_right, border_right, set_border_right);
      av8_set_cls_property(border_bottom, border_bottom, set_border_bottom);
      av8_set_cls_property(border_left_width, border_left_width, set_border_left_width);
      av8_set_cls_property(border_top_width, border_top_width, set_border_top_width);
      av8_set_cls_property(border_right_width, border_right_width, set_border_right_width);
      av8_set_cls_property(border_bottom_width, border_bottom_width, set_border_bottom_width);
      av8_set_cls_property(border_left_color, border_left_color, set_border_left_color);
      av8_set_cls_property(border_top_color, border_top_color, set_border_top_color);
      av8_set_cls_property(border_right_color, border_right_color, set_border_right_color);
      av8_set_cls_property(border_bottom_color, border_bottom_color, set_border_bottom_color);
      av8_set_cls_property(border_radius_left_top, border_radius_left_top, set_border_radius_left_top);
      av8_set_cls_property(border_radius_right_top, border_radius_right_top, set_border_radius_right_top);
      av8_set_cls_property(border_radius_right_bottom, border_radius_right_bottom, set_border_radius_right_bottom);
      av8_set_cls_property(border_radius_left_bottom, border_radius_left_bottom, set_border_radius_left_bottom);
      av8_set_cls_property(background_color, background_color, set_background_color);
      av8_set_cls_property(newline, newline, set_newline);
      av8_set_cls_property(content_align, content_align, set_content_align);
      av8_set_cls_property(text_align, text_align, set_text_align);
      av8_set_cls_property(max_width, max_width, set_max_width);
      av8_set_cls_property(max_height, max_height, set_max_height);
      av8_set_cls_property(start_x, start_x, set_start_x);
      av8_set_cls_property(start_y, start_y, set_start_y);
      av8_set_cls_property(ratio_x, ratio_x, set_ratio_x);
      av8_set_cls_property(ratio_y, ratio_y, set_ratio_y);
      av8_set_cls_property(repeat, repeat, set_repeat);
      av8_set_cls_property(text_background_color, text_background_color, set_text_background_color);
      av8_set_cls_property(text_color, text_color, set_text_color);
      av8_set_cls_property(text_size, text_size, set_text_size);
      av8_set_cls_property(text_style, text_style, set_text_style);
      av8_set_cls_property(text_family, text_family, set_text_family);
      av8_set_cls_property(text_line_height, text_line_height, set_text_line_height);
      av8_set_cls_property(text_shadow, text_shadow, set_text_shadow);
      av8_set_cls_property(text_decoration, text_decoration, set_text_decoration);
      av8_set_cls_property(text_overflow, text_overflow, set_text_overflow);
      av8_set_cls_property(text_white_space, text_white_space, set_text_white_space);
      av8_set_cls_property(align_x, align_x, set_align_x);
      av8_set_cls_property(align_y, align_y, set_align_y);
      av8_set_cls_property(shadow, shadow, set_shadow);
      av8_set_cls_property(src, src, set_src);
      av8_set_cls_property(background_image, background_image, set_background_image);
      // style property end
    }, nullptr);
  }
};

/**
 * @class WrapKeyframeAction
 */
class WrapKeyframeAction: public WrapBase {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapKeyframeAction>(args, new KeyframeAction());
  }
  
  /**
   * @func has_property(name)
   * @arg name {emun PropertyName} 
   * @ret {bool}
   */
  static void has_property(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func has_property(name)\n"
        "* @arg name {emun PropertyName}\n"
        "* @ret {bool}\n"
      );
    }
    av8_self(KeyframeAction);
    av8_return( self->has_property( static_cast<PropertyName>(args[0]->ToUint32()->Value()) ));
  }
  
  /**
   * @func match_property(name)
   * @arg name {emun PropertyName} 
   * @ret {bool}
   */
  static void match_property(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || ! args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func match_property(name)\n"
        "* @arg name {emun PropertyName}\n"
        "* @ret {bool}\n"
      );
    }
    av8_self(KeyframeAction);
    av8_return( self->match_property( static_cast<PropertyName>(args[0]->ToUint32()->Value()) ));
  }
  
  /**
   * @func frame(index)
   * @arg index {uint}
   * @ret {Frame}
   */
  static void frame(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || ! args[0]->IsUint32() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(KeyframeAction);
    uint index = args[0]->ToUint32()->Value();
    if ( index < self->length() ) {
      Frame* frame = self->frame(index);
      av8_return( Wrap<Frame>::wrap(frame)->local() );
    } else {
      av8_return_null();
    }
  }
  
  /**
   * @func add([time[,curve]][style])
   * arg [time=0] {uint}
   * arg [curve] {Curve}
   * arg [style] {Object}
   * @ret {Frame}
   */
  static void add(FunctionCall args) {
    av8_worker(args);
    uint64 time = 0;
    
    if ( args.Length() > 0 ) {
      if ( args[0]->IsObject() && ! args[0]->IsNull() ) {
        av8_handle_scope();
        
        Local<v8::Object> arg = args[0].As<v8::Object>();
        Local<v8::Array> names = arg->GetPropertyNames();
        Local<v8::Value> t;
        if ( arg->Get(worker->context(), worker->strs()->time()).ToLocal(&t) ) {
          if ( t->IsNumber() ) {
            time = uint64(1000) * t.As<v8::Number>()->Value();
          }
        } else { // js error
          return;
        }
        av8_self(KeyframeAction);
        Frame* frame = self->add(time);
        Local<v8::Object> handle = Wrap<Frame>::wrap(frame, av8_typeid(Frame))->local();
        
        for ( uint i = 0, len = names->Length(); i < len; i++ ) {
          Local<v8::Value> key = names->Get(i);
          Local<v8::Value> val = arg->Get( key );
          if ( ! handle->Set(worker->context(), key, val).FromMaybe(false) ) { // js error
            return;
          }
        }
        
        av8_return( handle );
      } else if ( args[0]->IsNumber() ) {
        time = uint64(1000) * args[0].As<v8::Number>()->Value();
      }
    }
    
    av8_self(KeyframeAction);
    
    Frame* frame = nullptr;
    if ( args.Length() > 1 && ! args[1]->IsUndefined() ) {
      av8_gui_parse_value(Curve, curve, args[1], "Action.add(time, curve");
      frame = self->add(time, out);
    } else {
      frame = self->add(time);
    }
    
    Wrap<Frame>* wrap = Wrap<Frame>::wrap(frame, av8_typeid(Frame));
    av8_return( wrap->local() );
  }
  
  /**
   * @get first {Frame}
   */
  static void first(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(KeyframeAction);
    if ( self->length() ) {
      Frame* frame = self->first();
      av8_return( Wrap<Frame>::wrap(frame)->local() );
    } else {
      av8_return_null();
    }
  }
  
  /**
   * @get last {Frame}
   */
  static void last(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(KeyframeAction);
    if ( self->length() ) {
      Frame* frame = self->last();
      av8_return( Wrap<Frame>::wrap(frame)->local() );
    } else {
      av8_return_null();
    }
  }
  
  /**
   * @get length {uint}
   */
  static void length(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(KeyframeAction);
    av8_return( self->length() );
  }
  
  /**
   * @get position {uint} get play frame position
   */
  static void position(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(KeyframeAction);
    av8_return( self->position() );
  }
  
  /**
   * @get time {uint} ms get play time position
   */
  static void time(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(KeyframeAction);
    av8_return( self->time() / 1000 );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(KeyframeAction, constructor, {
      av8_set_cls_method(has_property, has_property);
      av8_set_cls_method(match_property, match_property);
      av8_set_cls_method(frame, frame);
      av8_set_cls_method(add, add);
      av8_set_cls_property(first, first);
      av8_set_cls_property(last, last);
      av8_set_cls_property(length, length);
      av8_set_cls_property(position, position);
      av8_set_cls_property(time, time);
    }, Action);
  }
};

/**
 * @class NativeAction
 */
class BindingAction {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    
    // CubicBezier const
    av8_set_field(LINEAR, 0);
    av8_set_field(EASE, 1);
    av8_set_field(EASE_IN, 2);
    av8_set_field(EASE_OUT, 3);
    av8_set_field(EASE_IN_OUT, 4);
    
    WrapAction::binding(exports, worker);
    WrapGroupAction::binding(exports, worker);
    WrapSpawnAction::binding(exports, worker);
    WrapSequenceAction::binding(exports, worker);
    WrapKeyframeAction::binding(exports, worker);
    WrapFrame::binding(exports, worker);
  }
};

av8_reg_module(_action, BindingAction);
av8_nsd
