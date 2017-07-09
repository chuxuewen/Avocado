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
#include "agui/css.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

#define def_get_property(Name) \
static void Name(Local<v8::String> name, PropertyCall args) { }

#define def_set_property(Name)\
static void set_##Name(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) { \
  av8_worker(args); \
  av8_self(StyleSheets); \
  if ( ! value->IsNumber() ) { av8_throw_err("Bad argument."); } \
  self->set_##Name( value->ToNumber()->Value() ); \
}

#define def_get_property_from_type(Name)\
static void Name(Local<v8::String> name, PropertyCall args) { }

#define def_set_property_from_type(Name, Type, parse_func)\
static void set_##Name(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {\
  av8_worker(args);\
  av8_self(StyleSheets);\
  av8_gui_parse_value(Type, parse_func, value, "CSS."#Name" = %s");\
  self->set_##Name(out);\
}

#define def_property(Name) def_get_property(Name) def_set_property(Name)

#define def_property_from_type(Name, Type, parse_func) \
def_get_property_from_type(Name)\
def_set_property_from_type(Name, Type, parse_func)

/**
 * @class WrapFrame
 */
class WrapStyleSheets: public WrapBase {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Forbidden access abstract");
  }
  
  static void time(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(StyleSheets);
    av8_return( self->time() / 1000 );
  }
  
  static void set_time(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( ! value->IsNumber() ) {
      av8_throw_err("Bad argument.");
    }
    av8_self(StyleSheets);
    self->set_time(uint64(1000) * value->ToNumber()->Value());
  }
  
  // -------------------- get/set property --------------------
  
  def_property_from_type(translate, Vec2, vec2);
  def_property_from_type(scale, Vec2, vec2);
  def_property_from_type(skew, Vec2, vec2);
  def_property_from_type(origin, Vec2, vec2);
  def_property_from_type(margin, Value, value);
  def_property_from_type(border, Border, border);
  def_property(border_width);
  def_property_from_type(border_color, Color, color);
  def_property(border_radius);
  def_property_from_type(min_width, Value, value);
  def_property_from_type(min_height, Value, value);
  def_property_from_type(start, Vec2, vec2);
  def_property_from_type(ratio, Vec2, vec2);
  def_property_from_type(align, Align, align);
  
  // --------------------
  
  static void width(Local<v8::String> name, PropertyCall args) {
    
  }
  static void height(Local<v8::String> name, PropertyCall args) {
    
  }
  
  static void set_width(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(StyleSheets);
    if ( value->IsNumber() ) {
      self->set_width( Value( value->ToNumber()->Value() ) );
      self->set_width2( value->ToNumber()->Value() );
    } else {
      av8_gui_parse_value(Value, value, value, "CSS.width = %s");
      self->set_width(out);
    }
  }
  static void set_height(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(StyleSheets);
    if ( value->IsNumber() ) {
      self->set_height( Value( value->ToNumber()->Value() ) );
      self->set_height2( value->ToNumber()->Value() );
    } else {
      av8_gui_parse_value(Value, value, value, "CSS.height = %s");
      self->set_height(out);
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
  def_property_from_type(newline, bool, bool);
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
  
  static void NewCSS(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsObject() || args[0]->IsNull() ) {
      av8_throw_err("Bad argument.");
    }
    
    av8_handle_scope();
    
    Local<v8::Object> arg = args[0].As<v8::Object>();
    Local<v8::Array> names = arg->GetPropertyNames();
    
    for ( uint i = 0, len = names->Length(); i < len; i++ ) {
      Local<v8::Value> key = names->Get(i);
      Local<v8::Value> val;
      if ( ! arg->Get(worker->context(), key).ToLocal(&val) ) {
        return; // js error
      }
      if ( val.IsEmpty() || ! val->IsObject() ) {
        av8_throw_err("Bad argument.");
      }
      
      Array<StyleSheets*> arr = 
        root_styles()->instances( worker->to_string_utf8(key) ); // new instances
      
      if ( arr.length() ) {
        Local<v8::Object> propertys = val.As<v8::Object>();
        Local<v8::Array> names = propertys->GetPropertyNames();
      
        for ( uint j = 0, len = names->Length(); j < len; j++ ) {
          Local<v8::Value> key = names->Get(j);
          if ( ! propertys->Get(worker->context(), key).ToLocal(&val) ) {
            return; // js error
          }
          for ( auto& i : arr ) {
            StyleSheets* ss = i.value();
            Local<v8::Object> local = Wrap<StyleSheets>::wrap(ss)->local();
            
            if ( ! local->Set(worker->context(), key, val).FromMaybe(false) ) {
              return; // js error
            }
          }
        }
        // if (arr.length)
      }
    }
  }

public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    
    // PROPERTY
#define av_def_property(ENUM, TYPE, NAME) \
av8_set_field(ENUM, ENUM);
    av_each_property_table(av_def_property)
#undef av_def_property
    
    exports->Delete( worker->NewStringAscii("PROPERTY_WIDTH2") );
    exports->Delete( worker->NewStringAscii("PROPERTY_HEIGHT2") );
    
    av8_binding_class_no_exports(StyleSheets, constructor, {
      
      av8_set_cls_property(time, time, set_time);
      //
      av8_set_cls_property(translate, translate, set_translate);
      av8_set_cls_property(scale, scale, set_scale);
      av8_set_cls_property(skew, skew, set_skew);
      av8_set_cls_property(origin, origin, set_origin);
      av8_set_cls_property(margin, margin, set_margin);
      av8_set_cls_property(border, border, set_border);
      av8_set_cls_property(border_width, nullptr, set_border_width);
      av8_set_cls_property(border_color, nullptr, set_border_color);
      av8_set_cls_property(border_radius, border_radius, set_border_radius);
      av8_set_cls_property(min_width, min_width, set_min_width);
      av8_set_cls_property(min_height, min_height, set_min_height);
      av8_set_cls_property(start, start, set_start);
      av8_set_cls_property(ratio, ratio, set_ratio);
      av8_set_cls_property(align, align, set_align);
      //
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
    }, nullptr);
    
    av8_set_method(CSS, NewCSS);
  }
};

av8_reg_module(_css, WrapStyleSheets);
av8_nsd
