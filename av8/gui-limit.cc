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
#include "agui/limit.h"
#include "agui/limit-indep.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

template<class T> class WrapLimit: public BasicWrapView {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapLimit<T>>(args, new T());
  }
  static void min_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( worker->gui_value_program()->New(self->min_width()) );
  }
  static void min_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( worker->gui_value_program()->New(self->min_height()) );
  }
  static void max_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( worker->gui_value_program()->New(self->max_width()) );
  }
  static void max_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( worker->gui_value_program()->New(self->max_height()) );
  }
  static void set_min_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Limit.min_width = %s");
    av8_self(T);
    self->set_min_width(out);
  }
  static void set_min_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Limit.min_height = %s");
    av8_self(T);
    self->set_min_height(out);
  }
  static void set_max_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Limit.max_width = %s");
    av8_self(T);
    self->set_max_width(out);
  }
  static void set_max_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_gui_parse_value(Value, value, value, "Limit.max_height = %s");
    av8_self(T);
    self->set_max_height(out);
  }
};

/**
 * @class WrapLimitDiv
 */
class WrapLimitDiv: public WrapLimit<Limit> {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Limit, constructor, {
      av8_set_cls_property(min_width, min_width, set_min_width);
      av8_set_cls_property(min_height, min_height, set_min_height);
      av8_set_cls_property(max_width, max_width, set_max_width);
      av8_set_cls_property(max_height, max_height, set_max_height);
    }, Div);
    worker->native_constructors()->set_alias(av8_typeid(Limit), View::LIMIT);
  }
};

/**
 * @class WrapLimitIndep
 */
class WrapLimitIndep: public WrapLimit<LimitIndep> {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(LimitIndep, constructor, {
      av8_set_cls_property(min_width, min_width, set_min_width);
      av8_set_cls_property(min_height, min_height, set_min_height);
      av8_set_cls_property(max_width, max_width, set_max_width);
      av8_set_cls_property(max_height, max_height, set_max_height);
    }, Indep);
    worker->native_constructors()->set_alias(av8_typeid(LimitIndep), View::LIMIT_INDEP);
  }
};

void binding_limit(Local<v8::Object> exports, Worker* worker) {
  WrapLimitDiv::binding(exports, worker);
  WrapLimitIndep::binding(exports, worker);
}

av8_nsd
