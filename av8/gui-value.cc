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
#include "agui/font.h"
#include "agui/draw.h"
#include "native-core-js.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)


ValueProgram::ValueProgram(Worker* worker,
                         Local<v8::Object> exports,
                         Local<v8::Object> _native): WorkerObject(worker) {
  Isolate* ioslate = worker->isolate();
  
#define NewString(s) worker->NewStringAscii(s)
  
#define av8_init_func(name, Class) \
  _constructor_##name .Reset(ioslate, exports->Get(NewString(#Class)).As<Function>()); \
  _parse_##name       .Reset(ioslate, exports->Get(NewString("parse_"#name)).As<Function>()); \
  _parse_##name##_description \
  .Reset(ioslate, _native->Get(NewString("_parse_"#name"_description")).As<Function>()); \
  _##name             .Reset(ioslate, _native->Get(NewString("_"#name)).As<Function>());
  
  av8_gui_values(av8_init_func)
  _border_rgba            .Reset(ioslate, _native->Get(NewString("_border_rgba")).As<Function>());
  _shadow_rgba            .Reset(ioslate, _native->Get(NewString("_shadow_rgba")).As<Function>());
  _color_value_rgba       .Reset(ioslate, _native->Get(NewString("_color_value_rgba")).As<Function>());
  _text_shadow_value_rgba .Reset(ioslate, _native->Get(NewString("_text_shadow_value_rgba")).As<Function>());
  _is_base                .Reset(ioslate, _native->Get(NewString("_is_base")).As<Function>());
  _parse_values           .Reset(ioslate, _native->Get(NewString("_parse_values")).As<Function>());
  _parse_float_values     .Reset(ioslate, _native->Get(NewString("_parse_float_values")).As<Function>());
  
#undef NewString
#undef av8_init_func
}

ValueProgram::~ValueProgram() {
  
}

Local<v8::Value> ValueProgram::New(const TextAlign& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_text_align)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const Align& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_align)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const ContentAlign& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_content_align)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const Repeat& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_repeat)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const Direction& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_direction)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const KeyboardType& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_keyboard_type)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const KeyboardReturnType& value) {
  Local<v8::Value> arg = worker->New((uint)value);
  return worker->local(_keyboard_return_type)->Call(worker->Null(), 1, &arg);
}
Local<v8::Value> ValueProgram::New(const Border& value) {
  Array<Local<v8::Value>> args(5);
  args[0] = worker->New(value.width);
  args[1] = worker->New(value.color.r());
  args[2] = worker->New(value.color.g());
  args[3] = worker->New(value.color.b());
  args[4] = worker->New(value.color.a());
  return worker->local(_border_rgba)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const ShadowValue& value) {
  Array<Local<v8::Value>> args(7);
  args[0] = worker->New(value.offset_x);
  args[1] = worker->New(value.offset_y);
  args[2] = worker->New(value.size);
  args[3] = worker->New(value.color.b());
  args[4] = worker->New(value.color.g());
  args[5] = worker->New(value.color.b());
  args[6] = worker->New(value.color.a());
  return worker->local(_shadow_rgba)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const Color& value) {
  Array<Local<v8::Value>> args(4);
  args[0] = worker->New(value.r());
  args[1] = worker->New(value.g());
  args[2] = worker->New(value.b());
  args[3] = worker->New(value.a());
  return worker->local(_color)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const Vec2& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New(value.x());
  args[1] = worker->New(value.y());
  return worker->local(_vec2)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const Vec3& value) {
  Array<Local<v8::Value>> args(3);
  args[0] = worker->New(value.x());
  args[1] = worker->New(value.y());
  args[2] = worker->New(value.z());
  return worker->local(_vec3)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const Vec4& value) {
  Array<Local<v8::Value>> args(4);
  args[0] = worker->New(value.x());
  args[1] = worker->New(value.y());
  args[2] = worker->New(value.z());
  args[3] = worker->New(value.w());
  return worker->local(_vec4)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(cCurve& value) {
  Array<Local<v8::Value>> args(4);
  args[0] = worker->New(value.p1().x());
  args[1] = worker->New(value.p1().y());
  args[2] = worker->New(value.p2().x());
  args[3] = worker->New(value.p2().y());
  return worker->local(_curve)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const CGRect& value) {
  Array<Local<v8::Value>> args(4);
  args[0] = worker->New(value.origin.x());
  args[1] = worker->New(value.origin.y());
  args[2] = worker->New(value.size.width());
  args[3] = worker->New(value.size.height());
  return worker->local(_rect)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const Mat& value) {
  Local<v8::Array> arr = v8::Array::New(worker->isolate(), 6);
  arr->Set(0, worker->New( value[0] ));
  arr->Set(1, worker->New( value[1] ));
  arr->Set(2, worker->New( value[2] ));
  arr->Set(3, worker->New( value[3] ));
  arr->Set(4, worker->New( value[4] ));
  arr->Set(5, worker->New( value[5] ));
  return worker->local(_mat)->Call(worker->Null(), 1,
                                    reinterpret_cast<Local<v8::Value>*>(&arr));
}
Local<v8::Value> ValueProgram::New(const Mat4& value) {
  Local<v8::Array> arr = v8::Array::New(worker->isolate(), 16);
  arr->Set(0, worker->New( value[0] ));
  arr->Set(1, worker->New( value[1] ));
  arr->Set(2, worker->New( value[2] ));
  arr->Set(3, worker->New( value[3] ));
  arr->Set(4, worker->New( value[4] ));
  arr->Set(5, worker->New( value[5] ));
  arr->Set(6, worker->New( value[6] ));
  arr->Set(7, worker->New( value[7] ));
  arr->Set(8, worker->New( value[8] ));
  arr->Set(9, worker->New( value[9] ));
  arr->Set(10, worker->New( value[10] ));
  arr->Set(11, worker->New( value[11] ));
  arr->Set(12, worker->New( value[12] ));
  arr->Set(13, worker->New( value[13] ));
  arr->Set(14, worker->New( value[14] ));
  arr->Set(15, worker->New( value[15] ));
  return worker->local(_mat4)->Call(worker->Null(), 1,
                                    reinterpret_cast<Local<v8::Value>*>(&arr));
}
Local<v8::Value> ValueProgram::New(const Value& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New(value.value);
  return worker->local(_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const ColorValue& value) {
  Array<Local<v8::Value>> args(5);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New(value.value.r());
  args[2] = worker->New(value.value.g());
  args[3] = worker->New(value.value.b());
  args[4] = worker->New(value.value.a());
  return worker->local(_color_value_rgba)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextSizeValue& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New(value.value);
  return worker->local(_text_size_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextFamilyValue& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New(value.value->name());
  return worker->local(_text_family_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextStyleValue& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New((uint)value.value);
  return worker->local(_text_style_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextShadowValue& value) {
  Array<Local<v8::Value>> args(8);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New(value.value.offset_x);
  args[2] = worker->New(value.value.offset_y);
  args[3] = worker->New(value.value.size);
  args[4] = worker->New(value.value.color.r());
  args[5] = worker->New(value.value.color.g());
  args[6] = worker->New(value.value.color.b());
  args[7] = worker->New(value.value.color.a());
  return worker->local(_text_shadow_value_rgba)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextLineHeightValue& value) {
  Array<Local<v8::Value>> args(3);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New(value.value.is_auto);
  args[2] = worker->New(value.value.height);
  return worker->local(_text_line_height_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextDecorationValue& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New((uint)value.value);
  return worker->local(_text_decoration_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const String& value) {
  return worker->New(value);
}
Local<v8::Value> ValueProgram::New(const bool& value) {
  return worker->New(value);
}
Local<v8::Value> ValueProgram::New(const TextOverflowValue& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New((uint)value.value);
  return worker->local(_text_overflow_value)->Call(worker->Null(), args.length(), &args[0]);
}
Local<v8::Value> ValueProgram::New(const TextWhiteSpaceValue& value) {
  Array<Local<v8::Value>> args(2);
  args[0] = worker->New((uint)value.type);
  args[1] = worker->New((uint)value.value);
  return worker->local(_text_white_space_value)->Call(worker->Null(), args.length(), &args[0]);
}

static void parse_error_throw(Worker* worker, cchar* desc, v8::Persistent<v8::Function>& func) {
  String msg;
  v8::Local<v8::Value> o;
  
  if ( worker->local(func)->Call(worker->context(), worker->Null(), 0, nullptr).ToLocal(&o) ) {
    msg = worker->to_string_utf8(o);
  }
  
  // Bad argument. Input.type = [Bad]
  // reference value, "
  
  v8::Local<v8::Value> err =
    worker->NewTypeError("Bad argument. %s, "
                         "reference value %s", *String::format(desc, "[Bad]"), *msg);
  worker->throw_err(err);
}

#define av8_parse(name, ok) { \
v8::Local<v8::Object> object;\
if ( in->IsString() ) {\
  v8::Local<v8::Value> o = worker->local(_parse_##name)->Call(worker->Null(), 1, &in);\
  if ( o.IsEmpty() || o->IsNull() ) {\
    goto err;\
  } else {\
    object = o.As<v8::Object>();\
  }\
} else if ( is_##name(in) ) {\
  object = in.As<v8::Object>();\
} else {\
err:\
  parse_error_throw(worker, desc, _parse_##name##_description);\
  return false;\
}\
ok \
return true;\
}

// parse
bool ValueProgram::parse_text_align(Local<v8::Value> in, TextAlign& out, cchar* desc) {
  v8::Local<v8::Object> object;
  if ( in->IsString() ) {
    v8::Local<v8::Value> o = worker->local(_parse_text_align)->Call(worker->Null(), 1, &in);
    if ( o.IsEmpty() || o->IsNull() ) {
      goto err;
    } else {
      object = o.As<v8::Object>();
    }
  } else if ( is_text_align(in) ) {
    object = in.As<v8::Object>();
  } else {
  err:
    parse_error_throw(worker, desc, _parse_text_align_description);
    return false;
  }
  out = (TextAlign)object->Get(worker->strs()->value())->ToUint32()->Value();
  return true;
}
bool ValueProgram::parse_align(Local<v8::Value> in, Align& out, cchar* desc) {
  av8_parse(align, {
    out = (Align)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_content_align(Local<v8::Value> in, ContentAlign& out, cchar* desc) {
  av8_parse(content_align, {
    out = (ContentAlign)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_repeat(Local<v8::Value> in, Repeat& out, cchar* desc) {
  av8_parse(repeat, {
    out = (Repeat)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_direction(Local<v8::Value> in, Direction& out, cchar* desc) {
  av8_parse(direction, {
    out = (Direction)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_keyboard_type(Local<v8::Value> in, KeyboardType& out, cchar* desc) {
  av8_parse(keyboard_type, {
    out = (KeyboardType)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_keyboard_return_type(Local<v8::Value> in, KeyboardReturnType& out, cchar* desc) {
  av8_parse(keyboard_return_type, {
    out = (KeyboardReturnType)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_border(Local<v8::Value> in, Border& out, cchar* desc) {
  av8_parse(border, {
    out.width = object->Get(worker->strs()->width())->ToNumber()->Value();
    out.color.r(object->Get(worker->strs()->r())->ToUint32()->Value());
    out.color.g(object->Get(worker->strs()->g())->ToUint32()->Value());
    out.color.b(object->Get(worker->strs()->b())->ToUint32()->Value());
    out.color.a(object->Get(worker->strs()->a())->ToUint32()->Value());
  });
}
bool ValueProgram::parse_shadow(Local<v8::Value> in, ShadowValue& out, cchar* desc) {
  av8_parse(shadow, {
    out.offset_x = object->Get(worker->strs()->offset_x())->ToNumber()->Value();
    out.offset_y = object->Get(worker->strs()->offset_y())->ToNumber()->Value();
    out.size = object->Get(worker->strs()->size())->ToNumber()->Value();
    out.color.r(object->Get(worker->strs()->r())->ToUint32()->Value());
    out.color.g(object->Get(worker->strs()->g())->ToUint32()->Value());
    out.color.b(object->Get(worker->strs()->b())->ToUint32()->Value());
    out.color.a(object->Get(worker->strs()->a())->ToUint32()->Value());
  });
}
bool ValueProgram::parse_color(Local<v8::Value> in, Color& out, cchar* desc) {
  av8_parse(color, {
    out.r(object->Get(worker->strs()->r())->ToUint32()->Value());
    out.g(object->Get(worker->strs()->g())->ToUint32()->Value());
    out.b(object->Get(worker->strs()->b())->ToUint32()->Value());
    out.a(object->Get(worker->strs()->a())->ToUint32()->Value());
  });
}
bool ValueProgram::parse_vec2(Local<v8::Value> in, Vec2& out, cchar* desc) {
  av8_parse(vec2, {
    out.x(object->Get(worker->strs()->x())->ToNumber()->Value());
    out.y(object->Get(worker->strs()->y())->ToNumber()->Value());
  });
}
bool ValueProgram::parse_vec3(Local<v8::Value> in, Vec3& out, cchar* desc) {
  av8_parse(vec3, {
    out.x(object->Get(worker->strs()->x())->ToNumber()->Value());
    out.y(object->Get(worker->strs()->y())->ToNumber()->Value());
    out.z(object->Get(worker->strs()->z())->ToNumber()->Value());
  });
}
bool ValueProgram::parse_vec4(Local<v8::Value> in, Vec4& out, cchar* desc) {
  av8_parse(vec4, {
    out.x(object->Get(worker->strs()->x())->ToNumber()->Value());
    out.y(object->Get(worker->strs()->y())->ToNumber()->Value());
    out.z(object->Get(worker->strs()->z())->ToNumber()->Value());
    out.w(object->Get(worker->strs()->w())->ToNumber()->Value());
  });
}
bool ValueProgram::parse_curve(Local<v8::Value> in, Curve& out, cchar* desc) {
  v8::Local<v8::Object> object;
  
  if ( in->IsString() ) {
    static Map<String, cCurve*> const_curve([] {
      Map<String, cCurve*> rv;
      rv.set("linear", &LINEAR);
      rv.set("ease", &EASE);
      rv.set("ease_in", &EASE_IN);
      rv.set("ease_out", &EASE_OUT);
      rv.set("ease_in_out", &EASE_IN_OUT);
      rv.set("LINEAR", &LINEAR);
      rv.set("EASE", &EASE);
      rv.set("EASE_IN", &EASE_IN);
      rv.set("EASE_OUT", &EASE_OUT);
      rv.set("EASE_IN_OUT", &EASE_IN_OUT);
      return rv;
    }());
    
    av8_cur_worker();
    auto it = const_curve.find(worker->to_string_ascii(in));
    if ( !it.is_null() ) {
      out = *it.value(); return true;
    }
    
    v8::Local<v8::Value> o = worker->local(_parse_curve)->Call(worker->Null(), 1, &in);
    if ( o.IsEmpty() || o->IsNull() ) {
      return false;
    } else {
      object = o.As<v8::Object>();
    }
  } else if ( in->IsUint32() ) {
    static Map<uint, cCurve*> const_curve([] {
      Map<uint, cCurve*> rv;
      rv.set(0, &LINEAR);
      rv.set(1, &EASE);
      rv.set(2, &EASE_IN);
      rv.set(3, &EASE_OUT);
      rv.set(4, &EASE_IN_OUT);
      return rv;
    }());
    
    auto it = const_curve.find(in->ToUint32()->Value());
    if ( !it.is_null() ) {
      out = *it.value(); return true;
    }
    return false;
  } else if ( is_curve(in) ) {
    object = in.As<v8::Object>();
  } else {
    return false;
  }
  
  out = Curve(object->Get(worker->strs()->p1_x())->ToNumber()->Value(),
              object->Get(worker->strs()->p1_y())->ToNumber()->Value(),
              object->Get(worker->strs()->p2_x())->ToNumber()->Value(),
              object->Get(worker->strs()->p2_y())->ToNumber()->Value());
  return true;
}
bool ValueProgram::parse_rect(Local<v8::Value> in, CGRect& out, cchar* desc) {
  av8_parse(rect, {
    out.origin.x(object->Get(worker->strs()->x())->ToNumber()->Value());
    out.origin.y(object->Get(worker->strs()->y())->ToNumber()->Value());
    out.size.width(object->Get(worker->strs()->width())->ToNumber()->Value());
    out.size.height(object->Get(worker->strs()->height())->ToNumber()->Value());
  });
}
bool ValueProgram::parse_mat(Local<v8::Value> in, Mat& out, cchar* desc) {
  av8_parse(mat, {
    Local<v8::Array> mat = object->Get(worker->strs()->_value()).As<v8::Array>();
    out.m0(mat->Get(0)->ToNumber()->Value());
    out.m1(mat->Get(1)->ToNumber()->Value());
    out.m2(mat->Get(2)->ToNumber()->Value());
    out.m3(mat->Get(3)->ToNumber()->Value());
  });
}
bool ValueProgram::parse_mat4(Local<v8::Value> in, Mat4& out, cchar* desc) {
  av8_parse(mat4, {
    Local<v8::Array> mat = object->Get(worker->strs()->_value()).As<v8::Array>();
    out.m0(mat->Get(0)->ToNumber()->Value());
    out.m1(mat->Get(1)->ToNumber()->Value());
    out.m2(mat->Get(2)->ToNumber()->Value());
    out.m3(mat->Get(3)->ToNumber()->Value());
    out.m4(mat->Get(4)->ToNumber()->Value());
    out.m5(mat->Get(5)->ToNumber()->Value());
    out.m6(mat->Get(6)->ToNumber()->Value());
    out.m7(mat->Get(7)->ToNumber()->Value());
    out.m8(mat->Get(8)->ToNumber()->Value());
    out.m9(mat->Get(9)->ToNumber()->Value());
    out.m10(mat->Get(10)->ToNumber()->Value());
    out.m11(mat->Get(11)->ToNumber()->Value());
    out.m12(mat->Get(12)->ToNumber()->Value());
    out.m13(mat->Get(13)->ToNumber()->Value());
    out.m14(mat->Get(14)->ToNumber()->Value());
    out.m15(mat->Get(15)->ToNumber()->Value());
  });
}
bool ValueProgram::parse_value(Local<v8::Value> in, Value& out, cchar* desc) {
  if (in->IsNumber()) {
    out.type = ValueType::PIXEL;
    out.value = in->ToNumber()->Value();
    return true;
  }
  av8_parse(value, {
    out.type = (ValueType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value = object->Get(worker->strs()->value())->ToNumber()->Value();
  });
}
bool ValueProgram::parse_values(Local<v8::Value> in, Array<Value>& out, cchar* desc) {
  if (in->IsNumber()) {
    out.push({ ValueType::PIXEL, (float)in->ToNumber()->Value() }); return true;
  }
  v8::Local<v8::Array> arr;
  if (in->IsString()) {
    v8::Local<v8::Value> o = worker->local(_parse_values)->Call(worker->Null(), 1, &in);
    if ( o.IsEmpty() || o->IsNull()) {
      return false;
    } else {
      arr = o.As<v8::Array>();
    }
  } else if ( is_value(in) ) {
    out.push({
      (ValueType)in.As<v8::Object>()->Get(worker->strs()->type())->ToUint32()->Value(),
      (float)in.As<v8::Object>()->Get(worker->strs()->value())->ToNumber()->Value()
    });
    return true;
  } else {
    return false;
  }
  
  for(int i = 0, len = arr->Length(); i < len; i++) {
    Local<v8::Object> obj = arr->Get(i).As<v8::Object>();
    out.push({
      (ValueType)obj->Get(worker->strs()->type())->ToUint32()->Value(),
      (float)obj->Get(worker->strs()->value())->ToNumber()->Value()
    });
  }
  return true;
}
bool ValueProgram::parse_float_values(Local<v8::Value> in, Array<float>& out, cchar* desc) {
  if (in->IsNumber()) {
    out.push(in->ToNumber()->Value()); return true;
  }
  v8::Local<v8::Array> arr;
  if (in->IsString()) {
    v8::Local<v8::Value> o = worker->local(_parse_float_values)->Call(worker->Null(), 1, &in);
    if ( o.IsEmpty() || o->IsNull() ) {
      return false;
    } else {
      arr = o.As<v8::Array>();
    }
  } else {
    return false;
  }
  for(int i = 0, len = arr->Length(); i < len; i++) {
    out.push( arr->Get(i)->ToNumber()->Value() );
  }
  return true;
}
bool ValueProgram::parse_color_value(Local<v8::Value> in, ColorValue& out, cchar* desc) {
  av8_parse(color_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value.r(object->Get(worker->strs()->r())->ToUint32()->Value());
    out.value.g(object->Get(worker->strs()->g())->ToUint32()->Value());
    out.value.b(object->Get(worker->strs()->b())->ToUint32()->Value());
    out.value.a(object->Get(worker->strs()->a())->ToUint32()->Value());
  });
}
bool ValueProgram::parse_text_size_value(Local<v8::Value> in, TextSizeValue& out, cchar* desc) {
  if (in->IsNumber()) {
    out.type = TextArrtsType::VALUE;
    out.value = in->ToNumber()->Value();
    return true;
  }
  av8_parse(text_size_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value = object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_text_family_value(Local<v8::Value> in, TextFamilyValue& out, cchar* desc) {
  av8_parse(text_family_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    String fonts = worker->to_string_utf8(object->Get(worker->strs()->value()));
    out.value = FontPool::get_font_familys_id(fonts);
  });
}
bool ValueProgram::parse_text_style_value(Local<v8::Value> in, TextStyleValue& out, cchar* desc) {
  av8_parse(text_style_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value = (TextStyle)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_text_shadow_value(Local<v8::Value> in, TextShadowValue& out, cchar* desc) {
  av8_parse(text_shadow_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value.offset_x = object->Get(worker->strs()->offset_x())->ToNumber()->Value();
    out.value.offset_y = object->Get(worker->strs()->offset_y())->ToNumber()->Value();
    out.value.size = object->Get(worker->strs()->size())->ToNumber()->Value();
    out.value.color.r(object->Get(worker->strs()->r())->ToUint32()->Value());
    out.value.color.g(object->Get(worker->strs()->g())->ToUint32()->Value());
    out.value.color.b(object->Get(worker->strs()->b())->ToUint32()->Value());
    out.value.color.a(object->Get(worker->strs()->a())->ToUint32()->Value());
  });
}
bool ValueProgram::parse_text_line_height_value(Local<v8::Value> in, TextLineHeightValue& out, cchar* desc) {
  if (in->IsNumber()) {
    out.type = TextArrtsType::VALUE;
    out.value.is_auto = false;
    out.value.height = in->ToNumber()->Value();
    return true;
  }
  av8_parse(text_line_height_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value.is_auto = object->Get(worker->strs()->is_auto())->ToBoolean()->Value();
    out.value.height = object->Get(worker->strs()->height())->ToNumber()->Value();
  });
}
bool ValueProgram::parse_text_decoration_value(Local<v8::Value> in, TextDecorationValue& out, cchar* desc) {
  av8_parse(text_decoration_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value = (TextDecoration)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_string(Local<v8::Value> in, String& out, cchar* desc) {
  out = worker->to_string_utf8(in);
  return 1;
}
bool ValueProgram::parse_bool(Local<v8::Value> in, bool& out, cchar* desc) {
  out = in->ToBoolean()->IsTrue();
  return 1;
}
bool ValueProgram::parse_text_overflow_value(Local<v8::Value> in, TextOverflowValue& out, cchar* desc) {
  av8_parse(text_overflow_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value = (TextOverflow)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
bool ValueProgram::parse_text_white_space_value(Local<v8::Value> in, TextWhiteSpaceValue& out, cchar* desc) {
  av8_parse(text_white_space_value, {
    out.type = (TextArrtsType)object->Get(worker->strs()->type())->ToUint32()->Value();
    out.value = (TextWhiteSpace)object->Get(worker->strs()->value())->ToUint32()->Value();
  });
}
// is
bool ValueProgram::is_text_align(Local<v8::Value> value) {
  //return false;
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_align)->Equals(constructor);
}
bool ValueProgram::is_align(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_align)->Equals(constructor);
}
bool ValueProgram::is_content_align(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_content_align)->Equals(constructor);
}
bool ValueProgram::is_repeat(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_repeat)->Equals(constructor);
}
bool ValueProgram::is_direction(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_direction)->Equals(constructor);
}
bool ValueProgram::is_keyboard_type(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_keyboard_type)->Equals(constructor);
}
bool ValueProgram::is_keyboard_return_type(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_keyboard_return_type)->Equals(constructor);
}
bool ValueProgram::is_border(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_border)->Equals(constructor);
}
bool ValueProgram::is_shadow(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_shadow)->Equals(constructor);
}
bool ValueProgram::is_color(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_color)->Equals(constructor);
}
bool ValueProgram::is_vec2(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_vec2)->Equals(constructor);
}
bool ValueProgram::is_vec3(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_vec3)->Equals(constructor);
}
bool ValueProgram::is_vec4(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_vec4)->Equals(constructor);
}
bool ValueProgram::is_curve(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_curve)->Equals(constructor);
}
bool ValueProgram::is_rect(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_rect)->Equals(constructor);
}
bool ValueProgram::is_mat(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_mat)->Equals(constructor);
}
bool ValueProgram::is_mat4(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_mat4)->Equals(constructor);
}
bool ValueProgram::is_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_value)->Equals(constructor);
}
bool ValueProgram::is_color_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_color_value)->Equals(constructor);
}
bool ValueProgram::is_text_size_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_size_value)->Equals(constructor);
}
bool ValueProgram::is_text_family_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_family_value)->Equals(constructor);
}
bool ValueProgram::is_text_style_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_style_value)->Equals(constructor);
}
bool ValueProgram::is_text_shadow_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_shadow_value)->Equals(constructor);
}
bool ValueProgram::is_text_line_height_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_line_height_value)->Equals(constructor);
}
bool ValueProgram::is_text_decoration_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_decoration_value)->Equals(constructor);
}
bool ValueProgram::is_base(Local<v8::Value> value) {
  return worker->strong(_is_base)->Call(worker->Undefined(), 1, &value)->IsTrue();
}
bool ValueProgram::is_string(Local<v8::Value> value) {
  return 1;
}
bool ValueProgram::is_bool(Local<v8::Value> value) {
  return 1;
}
bool ValueProgram::is_text_overflow_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_overflow_value)->Equals(constructor);
}
bool ValueProgram::is_text_white_space_value(Local<v8::Value> value) {
  if ( ! value->IsObject()) return false;
  Local<v8::Value> constructor = value->ToObject()->Get(worker->strs()->constructor());
  return worker->strong(_constructor_text_white_space_value)->Equals(constructor);
}

// is_base

/**
 * @class NativeValue
 */
class NativeValue {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    Local<v8::Object> _native = v8::Object::New(worker->isolate());
    exports->Set(worker->strs()->_native(), _native);
    
    {
      v8::TryCatch try_catch;
      
      if (!worker->run_native_script(exports, (cchar*)
                                     CORE_native_js_code_gui_value_,
                                     CORE_native_js_code_gui_value_count_, "value")) {
        if ( try_catch.HasCaught() ) {
          worker->report_exception(&try_catch);
        }
        av_fatal("Could not initialize gui-value.js");
      }
    }
    ValueProgram* program = new ValueProgram(worker, exports, _native);
    worker->set_gui_value_program(program);
  }
};

av8_reg_module(_value, NativeValue);
av8_nsd
