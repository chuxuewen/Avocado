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

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

void binding_gui_event(Local<v8::Object> exports, Worker* worker);
void binding_app(Local<v8::Object> exports, Worker* worker);
void binding_display(Local<v8::Object> exports, Worker* worker);
void binding_view(Local<v8::Object> exports, Worker* worker);
void binding_sprite(Local<v8::Object> exports, Worker* worker);
void binding_layout(Local<v8::Object> exports, Worker* worker);
void binding_box(Local<v8::Object> exports, Worker* worker);
void binding_div(Local<v8::Object> exports, Worker* worker);
void binding_hybrid(Local<v8::Object> exports, Worker* worker);
void binding_span(Local<v8::Object> exports, Worker* worker);
void binding_text_node(Local<v8::Object> exports, Worker* worker);
void binding_image(Local<v8::Object> exports, Worker* worker);
void binding_indep_div(Local<v8::Object> exports, Worker* worker);
void binding_root(Local<v8::Object> exports, Worker* worker);
void binding_video(Local<v8::Object> exports, Worker* worker);
void binding_label(Local<v8::Object> exports, Worker* worker);
void binding_limit(Local<v8::Object> exports, Worker* worker);
void binding_action(Local<v8::Object> exports, Worker* worker);
void binding_button(Local<v8::Object> exports, Worker* worker);
void binding_panel(Local<v8::Object> exports, Worker* worker);
void binding_scroll(Local<v8::Object> exports, Worker* worker);
void binding_text(Local<v8::Object> exports, Worker* worker);
void binding_clip(Local<v8::Object> exports, Worker* worker);
void binding_input(Local<v8::Object> exports, Worker* worker);

/**
 * @class NativeGUI
 */
class NativeGUI {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    worker->binding("_value");
    binding_gui_event(exports, worker);
    binding_app(exports, worker);
    binding_display(exports, worker);
    binding_view(exports, worker);
    binding_sprite(exports, worker);
    binding_layout(exports, worker);
    binding_box(exports, worker);
    binding_div(exports, worker);
    binding_panel(exports, worker);
    binding_hybrid(exports, worker);
    binding_span(exports, worker);
    binding_text_node(exports, worker);
    binding_image(exports, worker);
    binding_indep_div(exports, worker);
    binding_root(exports, worker);
    binding_video(exports, worker);
    binding_label(exports, worker);
    binding_limit(exports, worker);
    binding_scroll(exports, worker);
    binding_text(exports, worker);
    binding_button(exports, worker);
    binding_clip(exports, worker);
    binding_input(exports, worker);
  }
};

av8_reg_module(_gui, NativeGUI);
av8_nsd
