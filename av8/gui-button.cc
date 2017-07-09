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
#include "agui/button.h"
#include "agui/panel.h"
#include "agui/hybrid.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapButton
 */
class WrapButton: public BasicWrapView {
public:
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapButton>(args, new Button());
  }
  
  /**
   * @func find_next_button(direction[,panel])
   * @arg direction {Direction}
   * @arg [panel] {Panel}
   * @ret {Button}
   */
  static void find_next_button(FunctionCall args) {
    
    cchar* argument =
    "* @func find_next_button(direction[,panel])\n"
    "* @arg direction {Direction}\n"
    "* @arg [panel] {Panel}\n"
    "* @ret {Button}\n"
    ;
    
    av8_worker(args);
    if ( args.Length() < 1 ) {
      av8_throw_err(argument);
    }
    av8_gui_parse_value(Direction, direction, args[0], "Button.find_next_button(%s)");
    
    Panel* panel = nullptr;
    if ( args.Length() > 1 ) {
      if ( ! worker->has(args[1], View::PANEL) ) {
        av8_throw_err(argument);
      }
      panel = Wrap<Panel>::Self(args[1]);
    }
    av8_self(Button);
    Button* button = self->find_next_button(out, panel);
    
    if ( button ) {
      av8_return( Wrap<Button>::wrap(button, View::BUTTON)->local() );
    } else {
      av8_return_null();
    }
  }
  
  static void panel(FunctionCall args) {
    av8_worker(args);
    av8_self(Button);
    Panel* panel = self->panel();
    
    if ( panel ) {
      av8_return( Wrap<Panel>::wrap(panel, View::PANEL)->local() );
    } else {
      av8_return_null();
    }
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Button, constructor, {
      av8_set_cls_method(find_next_button, find_next_button);
      av8_set_cls_method(panel, panel);
    }, Hybrid);
    worker->native_constructors()->set_alias(av8_typeid(Button), View::BUTTON);
  }
};

void binding_button(Local<v8::Object> exports, Worker* worker) {
  WrapButton::binding(exports, worker);
}

av8_nsd
