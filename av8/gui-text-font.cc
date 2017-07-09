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
#include "agui/text-font.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapTextFont
 */
class WrapTextFont {
  
  static void text_background_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_background_color()) );
  }
  static void text_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_color()) );
  }
  static void text_size(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_size()) );
  }
  static void text_style(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_style()) );
  }
  static void text_family(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_family()) );
  }
  static void text_shadow(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_shadow()) );
  }
  static void text_line_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_line_height()) );
  }
  static void text_decoration(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextFont* text = self->as_text_font();
    av8_return( worker->gui_value_program()->New(text->text_decoration()) );
  }
  static void set_text_background_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(ColorValue, color_value, value, "TextFont.color_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_background_color(out);
  }
  // set
  static void set_text_color(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(ColorValue, color_value, value, "TextFont.color_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_color(out);
  }
  static void set_text_size(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextSizeValue,
                       text_size_value, value, "TextFont.text_size_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_size(out);
  }
  static void set_text_style(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextStyleValue,
                       text_style_value, value, "TextFont.text_style_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_style(out);
  }
  static void set_text_family(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextFamilyValue,
                       text_family_value, value, "TextFont.text_family_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_family(out);
  }
  static void set_text_shadow(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextShadowValue,
                       text_shadow_value, value, "TextFont.text_shadow_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_shadow(out);
  }
  static void set_text_line_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextLineHeightValue,
                       text_line_height_value, value, "TextFont.text_line_height_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_line_height(out);
  }
  static void set_text_decoration(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextDecorationValue,
                       text_decoration_value, value, "TextFont.text_decoration_value = %s");
    av8_self(View);
    TextFont* text = self->as_text_font();
    text->set_text_decoration(out);
  }
  
  static void simple_layout_width(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 ) av8_throw_err("Bad argument.");
    av8_self(View);
    TextFont* text = self->as_text_font();
    float width = text->simple_layout_width( worker->to_string_ucs2(args[0]) );
    av8_return ( width );
  }

public:
  static void inherit(Local<FunctionTemplate> cls, Worker* worker) {
    av8_set_cls_method(simple_layout_width, simple_layout_width);
    av8_set_cls_property(text_background_color, text_background_color, set_text_background_color);
    av8_set_cls_property(text_color, text_color, set_text_color);
    av8_set_cls_property(text_size, text_size, set_text_size);
    av8_set_cls_property(text_style, text_style, set_text_style);
    av8_set_cls_property(text_family, text_family, set_text_family);
    av8_set_cls_property(text_shadow, text_shadow, set_text_shadow);
    av8_set_cls_property(text_line_height, text_line_height, set_text_line_height);
    av8_set_cls_property(text_decoration, text_decoration, set_text_decoration);
  }
};

class WrapTextLayout {
  static void text_overflow(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextLayout* text = self->as_text_layout();
    av8_return( worker->gui_value_program()->New(text->text_overflow()) );
  }
  static void text_white_space(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(View);
    TextLayout* text = self->as_text_layout();
    av8_return( worker->gui_value_program()->New(text->text_white_space()) );
  }
  static void set_text_overflow(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextOverflowValue,
                       text_overflow_value, value, "TextLayout.text_overflow = %s");
    av8_self(View);
    TextLayout* text = self->as_text_layout();
    text->set_text_overflow(out);
  }
  static void set_text_white_space(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextWhiteSpaceValue,
                       text_white_space_value, value, "TextLayout.text_white_space = %s");
    av8_self(View);
    TextLayout* text = self->as_text_layout();
    text->set_text_white_space(out);
  }
  
public:
  static void inherit(Local<FunctionTemplate> cls, Worker* worker) {
    WrapTextFont::inherit(cls, worker);
    av8_set_cls_property(text_overflow, text_overflow, set_text_overflow);
    av8_set_cls_property(text_white_space, text_white_space, set_text_white_space);
  }
};

void ViewUtil::inherit_text_font(Local<FunctionTemplate> cls, Worker* worker) {
  WrapTextFont::inherit(cls, worker);
}

void ViewUtil::inherit_text_layout(Local<FunctionTemplate> cls, Worker* worker) {
  WrapTextLayout::inherit(cls, worker);
}

av8_nsd
