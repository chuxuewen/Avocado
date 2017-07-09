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
#include "agui/sprite.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapSprite
 */
class WrapSprite: public BasicWrapView {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapSprite>(args, new Sprite());
  }
  // get
  static void src(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->src() );
  }
  static void texture(Local<v8::String> name, PropertyCall args) {
    // TODO ?
    av8_worker(args);
    av8_self(Sprite);
    av8_return( worker->Null() );
  }
  static void start_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->start_x() );
  }
  static void start_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->start_y() );
  }
  static void width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->width() );
  }
  static void height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->height() );
  }
  static void start(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( worker->gui_value_program()->New(self->start()) );
  }
  // set
  static void set_src(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Sprite);
    self->set_src(worker->to_string_utf8(value));
  }
  static void set_texture(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    // TODO ?
  }

  /**
   * @set start_x {float}
   */
  static void set_start_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set start_x {float}");
    }
    av8_self(Sprite);
    self->set_start_x( value->ToNumber()->Value() );
  }

  /**
   * @set start_y {float}
   */
  static void set_start_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set start_y {float}");
    }
    av8_self(Sprite);
    self->set_start_y( value->ToNumber()->Value() );
  }

  static void set_start(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "Sprite.start = %s");
    av8_self(Sprite);
    self->start( out );
  }

  /**
   * @set width {float}
   */
  static void set_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set width {float}");
    }
    av8_self(Sprite);
    self->set_width( value->ToNumber()->Value() );
  }

  /**
   * @set height {float}
   */
  static void set_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber()) {
      av8_throw_err("* @set height {float}");
    }
    av8_self(Sprite);
    self->set_height( value->ToNumber()->Value() );
  }

  static void ratio_x(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->ratio_x() );
  }

  static void ratio_y(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( self->ratio_y() );
  }

  /**
   * @set ratio_x {float}
   */
  static void set_ratio_x(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("* @set ratio_x {float}");
    }
    av8_self(Sprite);
    self->set_ratio_x( value->ToNumber()->Value() );
  }

  /**
   * @set ratio_y {float}
   */
  static void set_ratio_y(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("* @set ratio_y {float}");
    }
    av8_self(Sprite);
    self->set_ratio_y( value->ToNumber()->Value() );
  }
  static void ratio(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( worker->gui_value_program()->New(self->ratio()) );
  }
  static void set_ratio(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Vec2, vec2, value, "Sprite.ratio = %s");
    av8_self(Sprite);
    self->set_ratio( out );
  }
  static void repeat(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Sprite);
    av8_return( worker->gui_value_program()->New(self->repeat()) );
  }
  static void set_repeat(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Repeat, repeat, value, "Sprite.repeat = %s");
    av8_self(Sprite);
    self->set_repeat( out );
  }
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Sprite, constructor, {
      av8_set_cls_property(src, src, set_src);
      av8_set_cls_property(texture, texture, set_texture);
      av8_set_cls_property(start_x, start_x, set_start_x);
      av8_set_cls_property(start_y, start_y, set_start_y);
      av8_set_cls_property(width, width, set_width);
      av8_set_cls_property(height, height, set_height);
      av8_set_cls_property(start, start, set_start);
      av8_set_cls_property(ratio_x, ratio_x, set_ratio_x);
      av8_set_cls_property(ratio_y, ratio_y, set_ratio_y);
      av8_set_cls_property(ratio, ratio, set_ratio);
      av8_set_cls_property(repeat, repeat, set_repeat);
    }, View);
    worker->native_constructors()->set_alias(av8_typeid(Sprite), View::SPRITE);
  }
};

void binding_sprite(Local<v8::Object> exports, Worker* worker) {
  WrapSprite::binding(exports, worker);
}

av8_nsd
