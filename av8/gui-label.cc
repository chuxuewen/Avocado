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
#include "agui/label.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapLabel
 */
class WrapLabel: public BasicWrapView {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapLabel>(args, new Label());
  }
  
  static void length(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Label);
    av8_return( self->length() );
  }

  static void value(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Label);
    av8_return( self->value() );
  }
  
  static void text_hori_bearing(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Label);
    av8_return( self->text_hori_bearing() );
  }
  
  static void text_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Label);
    av8_return( self->text_height() );
  }
  
  static void set_value(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Label);
    Ucs2String str = worker->to_string_ucs2(value);
    self->set_value(str);
  }
  
  static void text_align(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Label);
    av8_return( worker->gui_value_program()->New(self->text_align()) );
  }
  
  static void set_text_align(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(TextAlign, text_align, value, "Label.text_align = %s");
    av8_self(Label);
    self->set_text_align(out);
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Label, constructor, {
      av8_set_cls_property(length, length);
      av8_set_cls_property(value, value, set_value);
      av8_set_cls_property(text_hori_bearing, text_hori_bearing);
      av8_set_cls_property(text_height, text_height);
      av8_set_cls_property(text_align, text_align, set_text_align);
      ViewUtil::inherit_text_font(cls, worker);
    }, View);
    worker->native_constructors()->set_alias(av8_typeid(Label), View::LABEL);
  }
};

void binding_label(Local<v8::Object> exports, Worker* worker) {
  WrapLabel::binding(exports, worker);
}

av8_nsd
