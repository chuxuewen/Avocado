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

#include "./gui.h"
#include "agui/box.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapBox
 */
class WrapBox: public WrapBase {
  
  static void constructor(FunctionCall args) {
    av8_worker(args);
    av8_throw_err("Forbidden access abstract");
  }
  
  /**
   * @get width {Value}
   */
  static void width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->width()) );
  }

  /**
   * @get width {Value}
   */
  static void height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->height()) );
  }

  /**
   * @get margin_left {Value}
   */
  static void margin_left(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->margin_left()) );
  }

  /**
   * @get margin_top {Value}
   */
  static void margin_top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->margin_top()) );
  }

  /**
   * @get margin_right {Value}
   */
  static void margin_right(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->margin_right()) );
  }

  /**
   * @get margin_bottom {Value}
   */
  static void margin_bottom(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->margin_bottom()) );
  }

  /**
   * @get border_left {Border}
   */
  static void border_left(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_left()) );
  }

  /**
   * @get border_top {Border}
   */
  static void border_top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_top()) );
  }

  /**
   * @get border_right {Border}
   */
  static void border_right(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_right()) );
  }

  /**
   * @get border_bottom {Border}
   */
  static void border_bottom(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_bottom()) );
  }

  /**
   * @get border_left_width {float}
   */
  static void border_left_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_left().width );
  }

  /**
   * @get border_top_width {float}
   */
  static void border_top_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_top().width );
  }

  /**
   * @get border_right_width {float}
   */
  static void border_right_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_right().width );
  }

  /**
   * @get border_bottom_width {float}
   */
  static void border_bottom_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_bottom().width );
  }

  /**
   * @get border_left_color {Color}
   */
  static void border_left_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_left().color) );
  }

  /**
   * @get border_top_color {Color}
   */
  static void border_top_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_top().color) );
  }

  /**
   * @get border_right_color {Color}
   */
  static void border_right_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_right().color) );
  }

  /**
   * @get border_bottom_color {Color}
   */
  static void border_bottom_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->border_bottom().color) );
  }

  /**
   * @get border_radius_left_top {float}
   */
  static void border_radius_left_top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_radius_left_top() );
  }

  /**
   * @get border_radius_right_top {float}
   */
  static void border_radius_right_top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_radius_left_top() );
  }

  /**
   * @get border_radius_right_bottom {float}
   */
  static void border_radius_right_bottom(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_radius_right_bottom() );
  }

  /**
   * @get border_radius_left_bottom {float}
   */
  static void border_radius_left_bottom(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->border_radius_left_bottom() );
  }

  /**
   * @get background_color {Color}
   */
  static void background_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( worker->gui_value_program()->New(self->background_color()) );
  }

  /**
   * @get newline {bool}
   */
  static void newline(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->newline() );
  }

  /**
   * @get final_width {float}
   */
  static void final_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->final_width() );
  }

  /**
   * @get final_height {float}
   */
  static void final_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->final_height() );
  }

  /**
   * @get final_margin_left {float}
   */
  static void final_margin_left(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->final_margin_left() );
  }

  /**
   * @get final_margin_top {float}
   */
  static void final_margin_top(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->final_margin_top() );
  }

  /**
   * @get final_margin_right {float}
   */
  static void final_margin_right(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->final_margin_right() );
  }

  /**
   * @get final_margin_bottom {float}
   */
  static void final_margin_bottom(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Box);
    av8_return( self->final_margin_bottom() );
  }

  /**
   * @set width {Value}
   */
  static void set_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.width = %s");
    av8_self(Box);
    self->set_width(out);
  }

  /**
   * @set height {Value}
   */
  static void set_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.height = %s");
    av8_self(Box);
    self->set_height(out);
  }

  /**
   * @set margin {Value}
   */
  static void set_margin(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.margin = %s");
    av8_self(Box);
    self->set_margin(out);
  }

  /**
   * @set margin_left {Value}
   */
  static void set_margin_left(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.margin_left = %s");
    av8_self(Box);
    self->set_margin_left(out);
  }

  /**
   * @set margin_top {Value}
   */
  static void set_margin_top(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.margin_top = %s");
    av8_self(Box);
    self->set_margin_top(out);
  }

  /**
   * @set margin_right {Value}
   */
  static void set_margin_right(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.margin_right = %s");
    av8_self(Box);
    self->set_margin_right(out);
  }

  /**
   * @set margin_bottom {Value}
   */
  static void set_margin_bottom(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Box.margin_bottom = %s");
    av8_self(Box);
    self->set_margin_bottom(out);
  }

  /**
   * @set border {Border}
   */
  static void set_border(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Border, border, value, "Box.border = %s");
    av8_self(Box);
    self->set_border(out);
  }

  /**
   * @set border_left {Border}
   */
  static void set_border_left(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Border, border, value, "Box.border_left = %s");
    av8_self(Box);
    self->set_border_left(out);
  }

  /**
   * @set border_top {Border}
   */
  static void set_border_top(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Border, border, value, "Box.border_top = %s");
    av8_self(Box);
    self->set_border_top(out);
  }

  /**
   * @set border_right {Border}
   */
  static void set_border_right(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Border, border, value, "Box.border_right = %s");
    av8_self(Box);
    self->set_border_right(out);
  }

  /**
   * @set border_bottom {Border}
   */
  static void set_border_bottom(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Border, border, value, "Box.border_bottom = %s");
    av8_self(Box);
    self->set_border_bottom(out);
  }

  /**
   * @set border_width {float}
   */
  static void set_border_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set border_width {float}");
    }
    av8_self(Box);
    self->set_border_width( value->ToNumber()->Value() );
  }

  /**
   * @set border_left_width {float}
   */
  static void set_border_left_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set border_left_width {float}");
    }
    av8_self(Box);
    self->set_border_left_width( value->ToNumber()->Value() );
  }

  /**
   * @set border_top_width {float}
   */
  static void set_border_top_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set border_top_width {float}");
    }
    av8_self(Box);
    self->set_border_top_width( value->ToNumber()->Value() );
  }

  /**
   * @set border_right_width {float}
   */
  static void set_border_right_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set border_right_width {float}");
    }
    av8_self(Box);
    self->set_border_right_width( value->ToNumber()->Value() );
  }

  /**
   * @set border_bottom_width {float}
   */
  static void set_border_bottom_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set border_bottom_width {float}");
    }
    av8_self(Box);
    self->set_border_bottom_width( value->ToNumber()->Value() );
  }

  /**
   * @set border_color {Color}
   */
  static void set_border_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Box.border_color = %s");
    av8_self(Box);
    self->set_border_color(out);
  }

  /**
   * @set border_left_color {Color}
   */
  static void set_border_left_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Box.border_left_color = %s");
    av8_self(Box);
    self->set_border_left_color(out);
  }

  /**
   * @set border_top_color {Color}
   */
  static void set_border_top_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Box.border_top_color = %s");
    av8_self(Box);
    self->set_border_top_color(out);
  }

  /**
   * @set border_right_color {Color}
   */
  static void set_border_right_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Box.border_right_color = %s");
    av8_self(Box);
    self->set_border_right_color(out);
  }

  /**
   * @set border_bottom_color {Color}
   */
  static void set_border_bottom_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Box.border_bottom_color = %s");
    av8_self(Box);
    self->set_border_bottom_color(out);
  }

  /**
   * @set border_radius {float}
   */
  static void set_border_radius(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) av8_throw_err("* @set border_radius {float}");
    av8_self(Box);
    self->set_border_radius( value->ToNumber()->Value() );
  }

  /**
   * @set border_radius_left_top {float}
   */
  static void set_border_radius_left_top(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) av8_throw_err("* @set border_color {float}");
    av8_self(Box);
    self->set_border_radius_left_top( value->ToNumber()->Value() );
  }

  /**
   * @set border_radius_right_top {float}
   */
  static void set_border_radius_right_top(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) av8_throw_err("* @set border_radius_right_top {float}");
    av8_self(Box);
    self->set_border_radius_right_top( value->ToNumber()->Value() );
  }

  /**
   * @set border_radius_right_bottom {float}
   */
  static void set_border_radius_right_bottom(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) av8_throw_err("* @set border_radius_right_bottom {float}");
    av8_self(Box);
    self->set_border_radius_right_bottom( value->ToNumber()->Value() );
  }

  /**
   * @set border_radius_left_bottom {float}
   */
  static void set_border_radius_left_bottom(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) av8_throw_err("* @set border_radius_left_bottom {float}");
    av8_self(Box);
    self->set_border_radius_left_bottom( value->ToNumber()->Value() );
  }

  /**
   * @set background_color {Color}
   */
  static void set_background_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Color, color, value, "Box.background_color = %s");
    av8_self(Box);
    self->set_background_color(out);
  }

  /**
   * @set newline {bool}
   */
  static void set_newline(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Box);
    self->set_newline( value->ToBoolean()->Value() );
  }
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class_no_exports(Box, constructor, {
      av8_set_cls_property(width, width, set_width);
      av8_set_cls_property(height, height, set_height);
      av8_set_cls_property(margin, nullptr, set_margin);
      av8_set_cls_property(margin_left, margin_left, set_margin_left);
      av8_set_cls_property(margin_top, margin_top, set_margin_top);
      av8_set_cls_property(margin_right, margin_right, set_margin_right);
      av8_set_cls_property(margin_bottom, margin_bottom, set_margin_bottom);
      av8_set_cls_property(border, nullptr, set_border);
      av8_set_cls_property(border_left, border_left, set_border_left);
      av8_set_cls_property(border_top, border_top, set_border_top);
      av8_set_cls_property(border_right, border_right, set_border_right);
      av8_set_cls_property(border_bottom, border_bottom, set_border_bottom);
      av8_set_cls_property(border_width, nullptr, set_border_width);
      av8_set_cls_property(border_left_width, border_left_width, set_border_left_width);
      av8_set_cls_property(border_top_width, border_top_width, set_border_top_width);
      av8_set_cls_property(border_right_width, border_right_width, set_border_right_width);
      av8_set_cls_property(border_bottom_width, border_bottom_width, set_border_bottom_width);
      av8_set_cls_property(border_color, nullptr, set_border_color);
      av8_set_cls_property(border_left_color, border_left_color, set_border_left_color);
      av8_set_cls_property(border_top_color, border_top_color, set_border_top_color);
      av8_set_cls_property(border_right_color, border_right_color, set_border_right_color);
      av8_set_cls_property(border_bottom_color, border_bottom_color, set_border_bottom_color);
      av8_set_cls_property(border_radius, nullptr, set_border_radius);
      av8_set_cls_property(border_radius_left_top, border_radius_left_top, set_border_radius_left_top);
      av8_set_cls_property(border_radius_right_top, border_radius_right_top, set_border_radius_right_top);
      av8_set_cls_property(border_radius_right_bottom, border_radius_right_bottom, set_border_radius_right_bottom);
      av8_set_cls_property(border_radius_left_bottom, border_radius_left_bottom, set_border_radius_left_bottom);
      av8_set_cls_property(background_color, background_color, set_background_color);
      av8_set_cls_property(newline, newline, set_newline);
      av8_set_cls_property(final_width, final_width);
      av8_set_cls_property(final_height, final_height);
      av8_set_cls_property(final_margin_left, final_margin_left);
      av8_set_cls_property(final_margin_top, final_margin_top);
      av8_set_cls_property(final_margin_right, final_margin_right);
      av8_set_cls_property(final_margin_bottom, final_margin_bottom);
    }, Layout);
    worker->native_constructors()->set_alias(av8_typeid(Box), View::BOX);
  }
};

void binding_box(Local<v8::Object> exports, Worker* worker) {
  WrapBox::binding(exports, worker);
}

av8_nsd
