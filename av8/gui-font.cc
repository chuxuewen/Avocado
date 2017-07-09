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

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

using namespace avocado::gui;

/**
 * @class WrapFontStatic
 */
class WrapFontStatic {
  
  /**
   * @func set_default_fonts(fonts)
   * @arg fonts {String|Array}
   */
  static void set_default_fonts(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    if ( args.Length() < 1 ) {
      av8_throw_err("Bad argument.");
    }
    if ( args[0]->IsString() ) {
      font_pool()->set_default_fonts( worker->to_string_utf8(args[0]).split(',') );
    } else if ( args[0]->IsArray() ) {
      font_pool()->set_default_fonts( worker->to_arr_string(args[0]) );
    } else {
      av8_throw_err("Bad argument.");
    }
  }
  
  /**
   * @func default_font_names()
   * @ret {Array}
   */
  static void default_font_names(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    Array<String> arr = font_pool()->default_font_names();
    av8_return(arr);
  }
  
  /**
   * @func family_names()
   * @ret {Array}
   */
  static void family_names(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    Array<String> arr = font_pool()->family_names();
    av8_return(arr);
  }
    
  /**
   * @func font_names(family)
   * @arg family {String}
   * @ret {Array}
   */
  static void font_names(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    if ( args.Length() < 1 ) {
      av8_throw_err(
        "* @func font_names(family)\n"
        "* @arg family {String}\n"
        "* @ret {Array}\n"
      );
    }
    Array<String> arr = font_pool()->font_names( worker->to_string_utf8(args[0]) );
    av8_return(arr);
  }
  
  /**
   * @func test(name) test font or family
   * @arg name {String} font name or family name
   * @ret {bool}
   */
  static void test(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    if ( args.Length() < 1 ) {
      av8_throw_err(
        "* @func test(name) test font or family\n"
        "* @arg name {String} font name or family name\n"
        "* @ret {bool}\n"
      );
    }
    bool is = font_pool()->test( worker->to_string_utf8(args[0]) );
    av8_return(is);
  }
  
  /**
   * @func register_font(font_data[,alias])
   * @arg font_data {Buffer} 
   * @arg alias {String}
   */
  static void register_font(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    if ( args.Length() < 1 && ! worker->has_buffer(args[0]) ) {
      av8_throw_err(
        "* @func register_font(font_data)\n"
        "* @arg font_data {Buffer}\n"
        "* @arg alias {String}\n"
      );
    }
    Wrap<Buffer>* wrap = Wrap<Buffer>::unwrap(args[0]);
    String alias;
    
    if ( args.Length() > 1 ) {
      alias = worker->to_string_utf8(args[1]);
    }
    font_pool()->register_font( *wrap->self(), alias );
  }
  
  /**
   * @func register_font_file(path[,alias])
   * @arg path {String}
   * @arg alias {String}
   */
  static void register_font_file(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    if ( args.Length() < 1 && ! args[0]->IsString() ) {
      av8_throw_err(
        "* @func register_font_file(path[,alias])\n"
        "* @arg path {String}\n"
        "* @arg alias {String}\n"
      );
    }
    String path = worker->to_string_utf8(args[0]);
    String alias;
    
    if ( args.Length() > 1 ) {
      alias = worker->to_string_utf8(args[1]);
    }
    font_pool()->register_font_file( path, alias );
  }
  
  /**
   * @func set_family_alias(family, alias)
   * @arg family {String}
   * @arg alias {String}
   */
  static void set_family_alias(FunctionCall args) {
    av8_worker(args);
    av8_check_gui_app();
    if ( args.Length() < 2 ) {
      av8_throw_err(
        "* @func set_family_alias(family, alias)\n"
        "* @arg family {String}\n"
        "* @arg alias {String}\n"
      );
    }
    font_pool()->set_family_alias( worker->to_string_utf8(args[0]), worker->to_string_utf8(args[1]) );
  }
  
public:
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_set_method(set_default_fonts, set_default_fonts);
    av8_set_method(default_font_names, default_font_names);
    av8_set_method(family_names, family_names);
    av8_set_method(font_names, font_names);
    av8_set_method(test, test);
    av8_set_method(register_font, register_font);
    av8_set_method(register_font_file, register_font_file);
    av8_set_method(set_family_alias, set_family_alias);
  }
};

av8_reg_module(_font, WrapFontStatic);

av8_nsd
