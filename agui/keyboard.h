/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, xuewen.chu
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of xuewen.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL xuewen.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

#ifndef __avocado__gui__keyboard__
#define __avocado__gui__keyboard__

#include "autil/util.h"
#include "autil/map.h"

av_gui_begin

class GUIApplication;

enum KeyboardKeyName {
  KEYCODE_UNKNOWN         = 0,
  KEYCODE_MOUSE_LEFT      = 1,  //
  KEYCODE_MOUSE_CENTER    = 2,  //
  KEYCODE_MOUSE_RIGHT     = 3,  // 
  KEYCODE_BACK_SPACE      = 8,  // back space
  KEYCODE_TAB             = 9,  // tab
  KEYCODE_CLEAR           = 12, // clear
  KEYCODE_ENTER           = 13, // enter
  KEYCODE_SHIFT           = 16, // shift
  KEYCODE_CTRL            = 17, // ctrl
  KEYCODE_ALT             = 18, // alt
  KEYCODE_CAPS_LOCK       = 20, // caps lock
  KEYCODE_ESC             = 27, // esc
  KEYCODE_SPACE           = 32, // space
  KEYCODE_COMMAND         = 91, // command/win
  KEYCODE_LEFT            = 37, // left
  KEYCODE_UP              = 38, // up
  KEYCODE_RIGHT           = 39, // right
  KEYCODE_DOWN            = 40, // down
  KEYCODE_INSERT          = 45, // insert
  KEYCODE_DELETE          = 46, // delete
  KEYCODE_PAGE_UP         = 33, // page up
  KEYCODE_PAGE_DOWN       = 34, // page down
  KEYCODE_MOVE_END        = 35, // end
  KEYCODE_MOVE_HOME       = 36, // home
  KEYCODE_SCROLL_LOCK     = 145,
  KEYCODE_BREAK           = 19,
  KEYCODE_SYSRQ           = 124,
  KEYCODE_0               = 48, // 0 )
  KEYCODE_1               = 49, // 1 !
  KEYCODE_2               = 50, // 2 @
  KEYCODE_3               = 51, // 3 #
  KEYCODE_4               = 52, // 4 $
  KEYCODE_5               = 53, // 5 %
  KEYCODE_6               = 54, // 6 ^
  KEYCODE_7               = 55, // 7 &
  KEYCODE_8               = 56, // 8 *
  KEYCODE_9               = 57, // 9 (
  KEYCODE_A               = 65, // a-z
  KEYCODE_B               = 66,
  KEYCODE_C               = 67,
  KEYCODE_D               = 68,
  KEYCODE_E               = 69,
  KEYCODE_F               = 70,
  KEYCODE_G               = 71,
  KEYCODE_H               = 72,
  KEYCODE_I               = 73,
  KEYCODE_J               = 74,
  KEYCODE_K               = 75,
  KEYCODE_L               = 76,
  KEYCODE_M               = 77,
  KEYCODE_N               = 78,
  KEYCODE_O               = 79,
  KEYCODE_P               = 80,
  KEYCODE_Q               = 81,
  KEYCODE_R               = 82,
  KEYCODE_S               = 83,
  KEYCODE_T               = 84,
  KEYCODE_U               = 85,
  KEYCODE_V               = 86,
  KEYCODE_W               = 87,
  KEYCODE_X               = 88,
  KEYCODE_Y               = 89,
  KEYCODE_Z               = 90,
  KEYCODE_NUM_LOCK        = 144, // numpad
  KEYCODE_NUMPAD_0        = 96,
  KEYCODE_NUMPAD_1        = 97,
  KEYCODE_NUMPAD_2        = 98,
  KEYCODE_NUMPAD_3        = 99,
  KEYCODE_NUMPAD_4        = 100,
  KEYCODE_NUMPAD_5        = 101,
  KEYCODE_NUMPAD_6        = 102,
  KEYCODE_NUMPAD_7        = 103,
  KEYCODE_NUMPAD_8        = 104,
  KEYCODE_NUMPAD_9        = 105,
  KEYCODE_NUMPAD_DIVIDE   = 111, // /
  KEYCODE_NUMPAD_MULTIPLY = 106, // *
  KEYCODE_NUMPAD_SUBTRACT = 109, // -
  KEYCODE_NUMPAD_ADD      = 107, // +
  KEYCODE_NUMPAD_DOT      = 110, // .
  KEYCODE_NUMPAD_ENTER    = 108, // enter
  KEYCODE_F1              = 112, // f1 - f12
  KEYCODE_F2              = 113, 
  KEYCODE_F3              = 114,
  KEYCODE_F4              = 115,
  KEYCODE_F5              = 116,
  KEYCODE_F6              = 117,
  KEYCODE_F7              = 118,
  KEYCODE_F8              = 119,
  KEYCODE_F9              = 120,
  KEYCODE_F10             = 121,
  KEYCODE_F11             = 122,
  KEYCODE_F12             = 123,
  KEYCODE_SEMICOLON       = 186,  // ; :
  KEYCODE_EQUALS          = 187,  // = +
  KEYCODE_MINUS           = 189,  // - _
  KEYCODE_COMMA           = 188,  // , <
  KEYCODE_PERIOD          = 190,  // . >
  KEYCODE_SLASH           = 191,  // / ?
  KEYCODE_GRAVE           = 192,  // ` ~
  KEYCODE_LEFT_BRACKET    = 219,  // [ {
  KEYCODE_BACK_SLASH      = 220,  // \ |
  KEYCODE_RIGHT_BRACKET   = 221,  // ] }
  KEYCODE_APOSTROPHE      = 222,  // ' "
  // ---------------------------------------------------
  KEYCODE_HOME            = 300,     // 按键Home
  KEYCODE_BACK            = 301,     // 返回键
  KEYCODE_CALL            = 302,     // 拨号键
  KEYCODE_ENDCALL         = 303,     // 挂机键
  KEYCODE_STAR            = 304,     // *
  KEYCODE_POUND           = 305,     // #
  KEYCODE_CENTER          = 306,     // 导航键 确定键
  KEYCODE_VOLUME_UP       = 307,     // 音量增加键
  KEYCODE_VOLUME_DOWN     = 308,     // 音量减小键
  KEYCODE_POWER           = 309,     // 电源键
  KEYCODE_CAMERA          = 310,     // 拍照键
  KEYCODE_FOCUS           = 311,     // 拍照对焦键
  KEYCODE_MENU            = 312,     // 菜单键
  KEYCODE_SEARCH          = 313,     // 搜索键
  KEYCODE_MEDIA_PLAY_PAUSE= 314,     // 多媒体键 播放/暂停
  KEYCODE_MEDIA_STOP      = 315,     // 多媒体键 停止
  KEYCODE_MEDIA_NEXT      = 316,     // 多媒体键 下一首
  KEYCODE_MEDIA_PREVIOUS  = 317,     // 多媒体键 上一首
  KEYCODE_MEDIA_REWIND    = 318,     // 多媒体键 快退
  KEYCODE_MEDIA_FAST_FORWARD = 319,  // 多媒体键 快进
  KEYCODE_MUTE            = 320,     // 话筒静音键
  KEYCODE_CHANNEL_UP      = 321,     // 按键Channel up
  KEYCODE_CHANNEL_DOWN    = 322,     // 按键Channel down
  KEYCODE_MEDIA_PLAY      = 323,     // 多媒体键 播放
  KEYCODE_MEDIA_PAUSE     = 324,     // 多媒体键 暂停
  KEYCODE_MEDIA_CLOSE     = 325,     // 多媒体键 关闭
  KEYCODE_MEDIA_EJECT     = 326,     // 多媒体键 弹出
  KEYCODE_MEDIA_RECORD    = 327,     // 多媒体键 录音
  KEYCODE_VOLUME_MUTE     = 328,     // 扬声器静音键
  KEYCODE_MUSIC           = 329,     // music
  KEYCODE_EXPLORER        = 330,     // 按键Explorer special function
  KEYCODE_ENVELOPE        = 331,     // 按键Envelope special function
  KEYCODE_BOOKMARK        = 332,     // 按键Bookmark
  KEYCODE_ZOOM_IN         = 333,     // 放大键
  KEYCODE_ZOOM_OUT        = 334,     // 缩小键
  KEYCODE_HELP            = 335,     // Help
};

/**
 * @class KeyboardAdapter
 */
class KeyboardAdapter: public Object {
public:

  KeyboardAdapter();
  
  /**
   * @func create
   */
  static KeyboardAdapter* create();
  
  /**
   * @func keyname
   */
  inline KeyboardKeyName keyname() const { return keyname_; }

  /**
   * @func keypress
   */
  inline int keypress() const { return keypress_; }
  
  inline bool shift() const { return shift_; }
  inline bool alt() const { return alt_; }
  inline bool ctrl() const { return ctrl_; }
  inline bool command() const { return command_; }
  inline bool caps_lock() const { return caps_lock_; }
  inline int  repeat() const { return repeat_; }
  inline int  device() const { return device_; }
  inline int  source() const { return source_; }
  
  /**
   * @func receive
   */
  void receive(int keycode, bool soft, bool is_down, int repeat, int device, int source);
  
  /**
   * @func transformation_keypress
   */
  virtual int transformation_keypress(KeyboardKeyName name);
  
  /**
   * @func transformation
   */
  virtual bool transformation(int keycode, bool soft, bool is_down);
  
protected:
  
  struct SoftKeyboardKeycode {
    KeyboardKeyName name;
    bool is_shift;
  };
  
  struct SymbolKeypressValue {
    int normal, shift;
  };
  
  Map<int, KeyboardKeyName>     m_keycodes;
  Map<int, SoftKeyboardKeycode> m_soft_keycodes;
  Map<int, SymbolKeypressValue> m_symbol_keypress;
  
  GUIApplication* app_;
  KeyboardKeyName keyname_;
  int   keypress_;
  bool  shift_;
  bool  alt_, ctrl_;
  bool  command_, caps_lock_;
  int   repeat_, device_, source_;
};

av_gui_end
#endif
