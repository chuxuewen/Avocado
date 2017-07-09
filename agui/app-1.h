/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, louis.chu
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

#ifndef __trurh__gui_app_1__
#define __trurh__gui_app_1__

#include "app.h"

/**
 * @ns trurh::gui
 */

av_gui_begin

av_def_inl_members(GUIApplication, Inl) {
public:
  
  struct KeyboardOptions {
    bool         is_clear;
    KeyboardType type;
    KeyboardReturnType return_type;
  };
  
  void initializ(const Map<String, int>& option);
  void post(Callback exec);
  void terminate();
  //
  void onLoad();
  void onRender();
  void onPause();
  void onResume();
  void onBackground();
  void onForeground();
  void onMemorywarning();
  void onUnload();
  void onRelease();
  
  /**
   * @func set_root
   */
  void set_root(Root* value) av_def_err;
  
  /**
   * @func set_first_responder
   */
  void set_first_responder(GUIResponder* responder);
  
  /**
   * @func emitter
   * */
  inline GUIEventEmitter* emitter() { return m_emitter; }
  
  /**
   * @func start_up_gui_thread
   */
  static void start_up_gui_thread(int argc, char* argv[]);
  
  /**
   * @func input_keyboard_open
   */
  void input_keyboard_open(KeyboardOptions options);
  
  /**
   * @func input_keyboard_can_back_space
   */
  void input_keyboard_can_back_space(bool can_back_space, bool can_delete);
  
  /**
   * @func input_keyboard_close
   */
  void input_keyboard_close();
  
};

#define _inl_app(self) static_cast<GUIApplication::Inl*>(self)

av_gui_end

#endif
