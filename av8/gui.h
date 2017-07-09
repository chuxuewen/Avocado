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

#ifndef __avocado__av8__gui__
#define __avocado__av8__gui__

#include "./av8.h"
#include "agui/app.h"
#include "agui/value.h"
#include "agui/view.h"
#include "agui/bezier.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

using namespace avocado::gui;
using namespace avocado::gui::value;

#define av8_check_gui_app() if ( ! app() ) { \
  av8_worker(args); av8_throw_err("Need to create a `new GUIApplication()`"); }

#define av8_gui_parse_value(cls, name, value, desc) \
  cls out; \
  if ( ! worker->gui_value_program()->parse_##name(value, out, desc)) \
  { return;/*av8_throw_err("Bad argument.");*/ }

// ------------- values -------------

#define av8_gui_values(F)                                                                         \
F(text_align, TextAlign)    F(align, Align)             F(content_align, ContentAlign)            \
F(border, Border)           F(shadow, ShadowValue)      F(color, Color)                           \
F(vec2, Vec2)               F(vec3, Vec3)               F(vec4, Vec4)                             \
F(rect, CGRect)             F(mat, Mat)                 F(mat4, Mat4)                             \
F(value, Value)             F(color_value, ColorValue)  F(text_size_value, TextSizeValue)         \
F(text_family_value, TextFamilyValue)         F(text_style_value, TextStyleValue)                 \
F(text_shadow_value, TextShadowValue)         F(text_line_height_value, TextLineHeightValue)      \
F(text_decoration_value, TextDecorationValue) F(repeat, Repeat)                                   \
F(curve, Curve)  F(direction, Direction)      F(string, String)   F(bool, bool)                   \
F(text_overflow_value, TextOverflowValue)     F(text_white_space_value, TextWhiteSpaceValue)      \
F(keyboard_type, KeyboardType)                F(keyboard_return_type, KeyboardReturnType)

/**
 * @class ValueProgram
 */
class ValueProgram: public WorkerObject {
public:
#define def_attr_fn(name, Class)              \
  Local<v8::Value> New(const Class & value);  \
  bool parse_##name(Local<v8::Value> in, Class& out, cchar* desc); \
  bool is_##name(Local<v8::Value> value);
  
#define def_attr(name, Class) \
  v8::Persistent<v8::Function> _constructor_##name; \
  v8::Persistent<v8::Function> _parse_##name; \
  v8::Persistent<v8::Function> _parse_##name##_description; \
  v8::Persistent<v8::Function> _##name;

  ValueProgram(Worker* worker, Local<v8::Object> exports, Local<v8::Object> _native);
  
  virtual ~ValueProgram();
  
  av8_gui_values(def_attr_fn);
  bool parse_values(Local<v8::Value> in, Array<Value>& out, cchar* desc);
  bool parse_float_values(Local<v8::Value> in, Array<float>& out, cchar* desc);
  bool is_base(Local<v8::Value> value);
  
private:
  av8_gui_values(def_attr)
  v8::Persistent<v8::Function> _border_rgba;
  v8::Persistent<v8::Function> _shadow_rgba;
  v8::Persistent<v8::Function> _color_value_rgba;
  v8::Persistent<v8::Function> _text_shadow_value_rgba;
  v8::Persistent<v8::Function> _parse_values;
  v8::Persistent<v8::Function> _parse_float_values;
  v8::Persistent<v8::Function> _is_base;
  #undef def_attr_fn
  #undef def_attr
};

/**
 * @class ViewUtil
 */
class ViewUtil {
public:
  
  /**
   * @func inherit_text_font
   */
  static void inherit_text_font(Local<FunctionTemplate> cls, Worker* worker);
  
  /**
   * @func inherit_text_layout
   */
  static void inherit_text_layout(Local<FunctionTemplate> cls, Worker* worker);
  
  /**
   * @func inherit_scroll
   */
  static void inherit_scroll(Local<FunctionTemplate> cls, Worker* worker);
  
  /**
   * @func add_event_listener
   */
  static bool add_event_listener(Wrap<View>* wrap, cString& name, cString& func, int id);
  
  /**
   * @func remove_event_listener
   */
  static bool remove_event_listener(Wrap<View>* wrap, cString& name, int id);
  
  /**
   * @func add_event_listener
   */
  static bool add_event_listener(Wrap<View>* wrap, const GUIEventName& name, cString& func, int id);

  /**
   * @func remove_event_listener
   */
  static bool remove_event_listener(Wrap<View>* wrap, const GUIEventName& name, int id);
  
  /**
   * @func panel_add_event_listener
   */
  static bool panel_add_event_listener(Wrap<View>* wrap, cString& name, cString& func, int id);
  
  /**
   * @func panel_remove_event_listener
   */
  static bool panel_remove_event_listener(Wrap<View>* wrap, cString& name, int id);
  
};

/**
 * @class BasicWrapView
 */
class BasicWrapView: public WrapBase {
public:
  
  /**
   * @func overwrite
   */
  virtual bool add_event_listener(cString& name, cString& func, int id) {
    return ViewUtil::add_event_listener(reinterpret_cast<Wrap<View>*>(this), name, func, id);
  }
  
  /**
   * @func overwrite
   */
  virtual bool remove_event_listener(cString& name, int id) {
    return ViewUtil::remove_event_listener(reinterpret_cast<Wrap<View>*>(this), name, id);
  }
  
};

av8_nsd
#endif
