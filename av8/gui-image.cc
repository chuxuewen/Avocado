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
#include "agui/image.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapImage
 */
class WrapImage: public BasicWrapView {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapImage>(args, new Image());
  }
  
  static void src(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Image);
    av8_return( self->src() );
  }
  
  static void set_src(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Image);
    String src = worker->to_string_utf8(value);
    self->set_src(src);
  }
  
  static void source_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Image);
    av8_return( self->source_width() );
  }
  
  static void source_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Image);
    av8_return( self->source_height() );
  }

  static void background_image(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Image);
    av8_return( self->background_image() );
  }

  static void set_background_image(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Image);
    String src = worker->to_string_utf8(value);
    self->set_background_image(src);
  }

public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Image, constructor, {
      av8_set_cls_property(src, src, set_src);
      av8_set_cls_property(source_width, source_width);
      av8_set_cls_property(source_height, source_height);
      av8_set_cls_property(background_image, background_image, set_background_image);
    }, Div);
    worker->native_constructors()->set_alias(av8_typeid(Image), View::IMAGE);
  }
};

void binding_image(Local<v8::Object> exports, Worker* worker) {
  WrapImage::binding(exports, worker);
}

av8_nsd
