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
#include "agui/app.h"
#include "agui/root.h"
#include "agui/display-port.h"
#include "agui/draw.h"
#include "agui/limit.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

using namespace avocado::gui;

static cString load("load");
static cString unload("unload");
static cString background("background");
static cString foreground("foreground");
static cString pause("pause");
static cString resume("resume");
static cString memorywarning("memorywarning");

typedef GUIApplication NativeGUIApplication;

/**
 * @class WrapNativeGUIApplication
 */
class WrapNativeGUIApplication: public WrapBase {
public: typedef GUIApplication Type;

  /**
   * @func add_event_listener
   */
  virtual bool add_event_listener(cString& name, cString& func, int id)
  {
    if ( name == load ) {
      self<Type>()->av8_bind_common_native_event(load);
    } else if ( name == unload ) {
      self<Type>()->av8_bind_common_native_event(unload);
    } else if ( name == background ) {
      self<Type>()->av8_bind_common_native_event(background);
    } else if ( name == foreground ) {
      self<Type>()->av8_bind_common_native_event(foreground);
    } else if ( name == pause ) {
      self<Type>()->av8_bind_common_native_event(pause);
    } else if ( name == resume ) {
      self<Type>()->av8_bind_common_native_event(resume);
    } else if ( name == memorywarning ) {
      self<Type>()->av8_bind_common_native_event(memorywarning);
    } else {
      return false;
    }
    return true;
  }
  
  /**
   * @func remove_event_listener
   */
  virtual bool remove_event_listener(cString& name, int id)
  {
    if ( name == load ) {
      self<Type>()->av8_unbind_native_event(load);
    } else if ( name == unload ) {
      self<Type>()->av8_unbind_native_event(unload);
    } else if ( name == background ) {
      self<Type>()->av8_unbind_native_event(background);
    } else if ( name == foreground ) {
      self<Type>()->av8_unbind_native_event(foreground);
    } else if ( name == pause ) {
      self<Type>()->av8_unbind_native_event(pause);
    } else if ( name == resume ) {
      self<Type>()->av8_unbind_native_event(resume);
    } else if ( name == memorywarning ) {
      self<Type>()->av8_unbind_native_event(memorywarning);
    } else {
      return false;
    }
    return true;
  }
  
  void memorywarning_handle(Event<>& evt) {
    _worker->isolate()->LowMemoryNotification(); // 清理内存
#if av_memory_trace_mark
    uint count = Object::mark_objects_count();
    LOG("All unrelease heap objects count: %d", count);
#endif
  }

  /**
   * @constructor([options])
   * @arg [options] {Object} { anisotropic {bool}, mipmap {bool}, multisample {0-4} }
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);
    
    Map<String, int> option;
    
    if ( args.Length() > 0 ) {
      option = worker->to_map_int(args[0]);
    }
    
    Wrap<NativeGUIApplication>* wrap = nullptr;
    try {
      wrap = New<WrapNativeGUIApplication>(args, new GUIApplication(option));
      wrap->self()->run();
      wrap->self()->av_on(memorywarning,
                          &WrapNativeGUIApplication::memorywarning_handle,
                          reinterpret_cast<WrapNativeGUIApplication*>(wrap));
    } catch(cError& err) {
      if ( wrap )
        delete wrap;
      av8_throw_err(err);
    }
  }
  
  /**
   * @func clear() clear gui application resources
   */
  static void clear(FunctionCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    if ( args.Length() > 1 ) {
      self->clear( args[0]->ToBoolean()->Value() );
    } else {
      self->clear();
    }
  }
  
  /**
   * @get is_load {bool}
   */
  static void is_load(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( self->is_load() );
  }
  
  /**
   * @get display_port {DisplayPort}
   */
  static void display_port(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    Wrap<DisplayPort>* wrap = av8_wrap(DisplayPort, self->display_port());
    av8_return( wrap->local() );
  }
  
  /**
   * @get root {Root}
   */
  static void root(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    Root* root = self->root();
    if (! root) { // null
      av8_return( v8::Null(worker->isolate()) );
    }
    Wrap<View>* wrap = Wrap<View>::wrap(root);
    av8_return( wrap->local() );
  }
  
  /**
   * @get first_responder {View}
   */
  static void first_responder(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    GUIResponder* responder = self->first_responder();
    if (! responder) { // null
      av8_return_null();
    }
    Wrap<GUIResponder>* wrap = Wrap<GUIResponder>::wrap(responder);
    av8_return( wrap->local() );
  }
  
  /**
   * @get default_text_background_color {ColorValue}
   */
  static void default_text_background_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_background_color()) );
  }
  
  /**
   * @get default_text_color {ColorValue}
   */
  static void default_text_color(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_color()) );
  }
  
  /**
   * @get default_text_size {TextSizeValue}
   */
  static void default_text_size(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_size()) );
  }
  
  /**
   * @get default_text_style {TextStyleValue}
   */
  static void default_text_style(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_style()) );
  }
  
  /**
   * @get default_text_family {TextFamilyValue}
   */
  static void default_text_family(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_family()) );
  }
  
  /**
   * @get default_text_shadow {TextShadowValue}
   */
  static void default_text_shadow(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_shadow()) );
  }
  
  /**
   * @get default_text_line_height {TextLineHeightValue}
   */
  static void default_text_line_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_line_height()) );
  }
  
  /**
   * @get default_text_decoration {TextDecorationValue}
   */
  static void default_text_decoration(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_decoration()) );
  }
  
  /**
   * @get default_text_overflow {TextOverflowValue}
   */
  static void default_text_overflow(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_overflow()) );
  }
  
  /**
   * @get default_text_white_space {TextWhiteSpaceValue}
   */
  static void default_text_white_space(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_return( worker->gui_value_program()->New(self->default_text_white_space()) );
  }
  
  /**
   * @set default_text_background_color {ColorValue}
   */
  static void set_default_text_background_color(Local<v8::String> name,
                                                Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(ColorValue, color_value, value,
                       "GUIApplication.default_text_background_color = %s");
    self->set_default_text_background_color(out);
  }
  
  /**
   * @set default_text_color {ColorValue}
   */
  static void set_default_text_color(Local<v8::String> name,
                                     Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(ColorValue, color_value, value,
                       "GUIApplication.default_text_color = %s");
    self->set_default_text_color(out);
  }
  
  /**
   * @set default_text_size {TextSizeValue}
   */
  static void set_default_text_size(Local<v8::String> name,
                                    Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextSizeValue, text_size_value, value,
                       "GUIApplication.default_text_size = %s");
    self->set_default_text_size(out);
  }
  
  /**
   * @set default_text_style {TextStyleValue}
   */
  static void set_default_text_style(Local<v8::String> name,
                                     Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextStyleValue, text_style_value, value,
                       "GUIApplication.default_text_style = %s");
    self->set_default_text_style(out);
  }
  
  /**
   * @set default_text_family {TextFamilyValue}
   */
  static void set_default_text_family(Local<v8::String> name,
                                      Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextFamilyValue, text_family_value, value,
                       "GUIApplication.default_text_family = %s");
    self->set_default_text_family(out);
  }
  
  /**
   * @set default_text_shadow {TextShadowValue}
   */
  static void set_default_text_shadow(Local<v8::String> name,
                                      Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextShadowValue, text_shadow_value, value,
                       "GUIApplication.default_text_shadow = %s");
    self->set_default_text_shadow(out);
  }
  
  /**
   * @set default_text_line_height {TextLineHeightValue}
   */
  static void set_default_text_line_height(Local<v8::String> name,
                                           Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextLineHeightValue, text_line_height_value, value,
                       "GUIApplication.default_text_line_height = %s");
    self->set_default_text_line_height(out);
  }
  
  /**
   * @set default_text_decoration {TextDecorationValue}
   */
  static void set_default_text_decoration(Local<v8::String> name,
                                          Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextDecorationValue, text_decoration_value, value,
                       "GUIApplication.default_text_decoration = %s");
    self->set_default_text_decoration(out);
  }
  
  /**
   * @set default_text_overflow {TextOverflowValue}
   */
  static void set_default_text_overflow(Local<v8::String> name,
                                        Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextOverflowValue, text_overflow_value, value,
                       "GUIApplication.default_text_overflow = %s");
    self->set_default_text_overflow(out);
  }
  
  /**
   * @set default_text_white_space {TextWhiteSpaceValue}
   */
  static void set_default_text_white_space(Local<v8::String> name,
                                              Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(GUIApplication);
    av8_gui_parse_value(TextWhiteSpaceValue, text_white_space_value, value,
                       "GUIApplication.default_text_white_space = %s");
    self->set_default_text_white_space(out);
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(NativeGUIApplication, constructor, {
      av8_set_cls_method(clear, clear);
      av8_set_cls_property(is_load, is_load);
      av8_set_cls_property(display_port, display_port);
      av8_set_cls_property(root, root);
      av8_set_cls_property(first_responder, first_responder);
      av8_set_cls_property(default_text_background_color,
                          default_text_background_color, set_default_text_background_color);
      av8_set_cls_property(default_text_color, default_text_color, set_default_text_color);
      av8_set_cls_property(default_text_size, default_text_size, set_default_text_size);
      av8_set_cls_property(default_text_style, default_text_style, set_default_text_style);
      av8_set_cls_property(default_text_family, default_text_family, set_default_text_family);
      av8_set_cls_property(default_text_shadow, default_text_shadow, set_default_text_shadow);
      av8_set_cls_property(default_text_line_height, default_text_line_height, set_default_text_line_height);
      av8_set_cls_property(default_text_decoration, default_text_decoration, set_default_text_decoration);
      av8_set_cls_property(default_text_overflow, default_text_overflow, set_default_text_overflow);
      av8_set_cls_property(default_text_white_space, default_text_white_space, set_default_text_overflow);
      
    }, NULL);
  }
};

void binding_app(Local<v8::Object> exports, Worker* worker) {
  WrapNativeGUIApplication::binding(exports, worker);
}

av8_nsd
