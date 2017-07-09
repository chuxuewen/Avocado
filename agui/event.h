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

#ifndef __avocado__gui__event__
#define __avocado__gui__event__

#include "autil/event.h"
#include "autil/array.h"
#include "autil/sys.h"
#include "mathe.h"
#include "value.h"
#include "keyboard.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

class GUIApplication;
class View;
class Action;
class Activity;
class Button;

// ---------------------------------------- EVENT TYPE ----------------------------------------

enum {
  GUI_EVENT_CATEGORY_DEFAULT,
  GUI_EVENT_CATEGORY_KEYBOARD,
  GUI_EVENT_CATEGORY_CLICK,
  GUI_EVENT_CATEGORY_HIGHLIGHTED,
  GUI_EVENT_CATEGORY_MOUSE,
  GUI_EVENT_CATEGORY_TOUCH,
  GUI_EVENT_CATEGORY_ACTION,
  GUI_EVENT_CATEGORY_SWITCH,
};

class GUIEventName {
public:
  inline GUIEventName() { av_unreachable(); }
  inline GUIEventName(cString& n, uint category)
  : name_(n), code_(n.hash_code()), category_(category) { }
  inline uint hash_code() const { return code_; }
  inline bool equals(const GUIEventName& o) const { return o.hash_code() == code_; }
  inline String to_string() const { return name_; }
  inline uint category() const { return category_; }
  inline bool operator==(const GUIEventName& type) const { return type.code_ == code_; }
  inline bool operator!=(const GUIEventName& type) const { return type.code_ != code_; }
private:
  String  name_;
  uint    code_, category_;
};

extern const GUIEventName GUI_EVENT_KEYDOWN;     // can bubble event
extern const GUIEventName GUI_EVENT_KEYPRESS;
extern const GUIEventName GUI_EVENT_KEYUP;
extern const GUIEventName GUI_EVENT_KEYESC;
extern const GUIEventName GUI_EVENT_KEYENTER;
extern const GUIEventName GUI_EVENT_BACK;
extern const GUIEventName GUI_EVENT_CLICK;
extern const GUIEventName GUI_EVENT_HIGHLIGHTED; // normal / hover / down
extern const GUIEventName GUI_EVENT_MOUSEDOWN;
extern const GUIEventName GUI_EVENT_MOUSEMOVE;
extern const GUIEventName GUI_EVENT_MOUSEUP;
extern const GUIEventName GUI_EVENT_MOUSEENTER;
extern const GUIEventName GUI_EVENT_MOUSELEAVE;
extern const GUIEventName GUI_EVENT_MOUSEOVER;
extern const GUIEventName GUI_EVENT_MOUSEOUT;
extern const GUIEventName GUI_EVENT_MOUSEWHELL;
extern const GUIEventName GUI_EVENT_TOUCHSTART;
extern const GUIEventName GUI_EVENT_TOUCHMOVE;
extern const GUIEventName GUI_EVENT_TOUCHEND;
extern const GUIEventName GUI_EVENT_TOUCHCANCEL;
extern const GUIEventName GUI_EVENT_FOCUS;
extern const GUIEventName GUI_EVENT_BLUR;
extern const GUIEventName GUI_EVENT_REMOVE_VIEW;    // no bubble event
extern const GUIEventName GUI_EVENT_ACTION_KEYFRAME;
extern const GUIEventName GUI_EVENT_ACTION_LOOP;
extern const GUIEventName GUI_EVENT_SWITCH;
extern const GUIEventName GUI_EVENT_ENTER;
extern const GUIEventName GUI_EVENT_LEAVE;
extern const GUIEventName GUI_EVENT_SCROLL;
extern const GUIEventName GUI_EVENT_CHANGE;
//
extern const Map<String, GUIEventName> GUI_EVENT_TABLE;

// --------------------------------------------------------------------------------

enum HighlightedStatus {
  HIGHLIGHTED_NORMAL = 1,
  HIGHLIGHTED_HOVER,
  HIGHLIGHTED_DOWN,
};

struct GUITouch { // touch event point
  uint    id;
  float   start_x, start_y;
  float   x, y, force;
  bool    click_in;
  View*   view;
};

/**
 * @func GUIEvent gui event
 */
class GUIEvent: public Event<Object, View> {
public:
  inline GUIEvent(cSendData data): Event<Object, View>() { av_unreachable(); }
  inline GUIEvent(View* origin, cSendData data = SendData())
  : Event(data), origin_(origin), time_(sys::time()) { }
  inline View* origin() const { return origin_; }
  inline uint64 timestamp() const { return time_; }
  virtual void release() { origin_ = nullptr; Event<Object, View>::release(); }
protected:
  View*   origin_;
  uint64  time_;
};

/**
 * @class GUIActionEvent
 */
class GUIActionEvent: public GUIEvent {
public:
  inline GUIActionEvent(Action* action, uint64 delay, uint frame, uint loop)
  : GUIEvent(nullptr), action_(action), delay_(delay), frame_(frame), loop_(loop) { }
  inline Action* action() const { return action_; }
  inline uint64 delay() const { return delay_; }
  inline uint frame() const { return frame_; }
  inline uint loop() const { return loop_; }
  inline void set_origin(View* value) { origin_ = value; }
  virtual void release() { action_ = nullptr; GUIEvent::release(); }
private:
  Action* action_;
  uint64  delay_;
  uint    frame_, loop_;
};

/**
 * @func GUIKeyEvent keyboard event
 */
class GUIKeyEvent: public GUIEvent {
public:
  inline GUIKeyEvent(View* origin, uint keycode,
                     bool shift, bool ctrl, bool alt, bool command, bool caps_lock,
                     uint repeat, int device, int source)
  : GUIEvent(origin), keycode_(keycode)
  , repeat_(repeat), device_(device), source_(source), shift_(shift)
  , ctrl_(ctrl), alt_(alt), command_(command), caps_lock_(caps_lock) { }
  inline int  keycode() const { return keycode_; }
  inline int  repeat() const { return repeat_; }
  inline int  device() const { return device_; }
  inline int  source() const { return source_; }
  inline bool shift() const { return shift_; }
  inline bool ctrl() const { return ctrl_; }
  inline bool alt() const { return alt_; }
  inline bool command() const { return command_; }
  inline bool caps_lock() const { return caps_lock_; }
  inline void set_keycode(int value) { keycode_ = value; }
private:
  int  keycode_;
  int  device_, source_, repeat_;
  bool shift_, ctrl_, alt_;
  bool command_, caps_lock_;
};

/**
 * @class GUIClickEvent click event
 */
class GUIClickEvent: public GUIEvent {
public:
  inline GUIClickEvent(View* origin, float x, float y, uint count = 1)
  : GUIEvent(origin), x_(x), y_(y), count_(count) { }
  inline float x() const { return x_; }
  inline float y() const { return y_; }
  inline uint count() const { return count_; }
private:
  float x_, y_;
  uint count_;
};

/**
 * @class GUIMouseEvent mouse event
 */
class GUIMouseEvent: public GUIKeyEvent {
public:
  inline GUIMouseEvent(View* origin, float x, float y, uint keycode,
                       bool shift, bool ctrl, bool alt, bool command, bool caps_lock,
                       uint repeat = 0, int device = 0, int source = 0)
  : GUIKeyEvent(origin, keycode,shift, ctrl, alt,
                command, caps_lock, repeat, device, source), x_(x), y_(y) { }
  inline float x() const { return x_; }
  inline float y() const { return y_; }
private:
  float x_, y_;
};

/**
 * @class GUIHighlightedEvent status event
 */
class GUIHighlightedEvent: public GUIEvent {
public:
  inline GUIHighlightedEvent(View* origin, HighlightedStatus status)
  : GUIEvent(origin), _status(status) { }
  inline HighlightedStatus status() const { return _status; }
private:
  HighlightedStatus _status;
};

/**
 * @class GUITouchEvent touch event
 */
class GUITouchEvent: public GUIEvent {
public:
  inline GUITouchEvent(View* origin, Array<GUITouch>& touches)
    : GUIEvent(origin), m_change_touches(touches) { }
  inline Array<GUITouch>& change_touches() { return m_change_touches; }
private:
  Array<GUITouch> m_change_touches;
};

/**
 * @class GUISwitchEvent
 */
class GUISwitchEvent: public GUIEvent {
public:
  inline GUISwitchEvent(View* origin, Direction direction, Button* button, Button* target)
  : GUIEvent(origin), m_direction(direction), m_button(button), m_target_button(target) { }
  inline Direction direction() const { return m_direction; }
  inline Button* button() { return m_button; }
  inline Button* target_button() { return m_target_button; }
  inline void target_button(Button* value) { m_target_button = value; }
  virtual void release() {
    m_button = nullptr; m_target_button = nullptr; GUIEvent::release();
  }
private:
  Direction m_direction;
  Button*   m_button;
  Button*   m_target_button;
};

/**
 * @class GUIResponder
 */
class GUIResponder: public Responder<GUIEvent, GUIEventName, Reference> {
public:
  inline bool receive() const { return m_has_receive; }
  inline void set_receive(bool value) { m_has_receive = value; }
  bool focus();
  bool blur();
  bool is_focus() const;
  
protected:
  inline GUIResponder(View* sender): Responder(sender), m_has_receive(false) { }
  virtual ~GUIResponder() { blur(); }
  
  virtual void trigger_listener_change(const Name& name, int count, int change);
  ReturnValue& trigger(const Name& name, GUIEvent& evt, bool force = false);
  ReturnValue trigger(const Name& name, bool force = false);
  
  bool        m_has_receive;
  av_def_inl_cls(Inl);
  av_def_inl_cls(ActionInl);
};

/**
 * @class TextInput
 */
class TextInput {
public:
  typedef ProtocolCharacteristic Characteristic;
  
  virtual void input_delete_text(int count) = 0;
  virtual void input_insert_text(cString& text) = 0;
  virtual void input_marked_text(cString& text) = 0;
  virtual void input_unmark_text(cString& text) = 0;
  virtual bool input_can_delete() = 0;
  virtual bool input_can_back_space() = 0;
  virtual KeyboardType keyboard_type() = 0;
  virtual KeyboardReturnType keyboard_return_type() = 0;
};

/**
 * @class GUIEventEmitter
 */
class GUIEventEmitter: public Object {
public:

  GUIEventEmitter(GUIApplication* app);
  
  virtual ~GUIEventEmitter();
  
  void receive_touch_start(List<GUITouch>&& touches);   // touch
  void receive_touch_move(List<GUITouch>&& touches);
  void receive_touch_end(List<GUITouch>&& touches);
  void receive_touch_cancel(List<GUITouch>&& touches);
  void receive_mouse_down(Vec2 point, int key_code);    // mouse
  void receive_mouse_move(Vec2 point, int key_code);
  void receive_mouse_up(Vec2 point, int key_code);
  void receive_mouse_wheel(int len);
  void receive_input_delete_text(int count);     // input text
  void receive_input_insert_text(cString& text);
  void receive_input_marked_text(cString& text);
  void receive_input_unmark_text(cString& text);
  
  /**
   * @func make_text_input
   */
  void make_text_input(TextInput* input);
  
  /**
   * @func keyboard_adapter
   */
  inline KeyboardAdapter* keyboard_adapter() {
    return m_keyboard;
  }
  
private:
    
  class OriginTouche; typedef Map<PrtKey<View>, OriginTouche*> OriginTouches;
  
  GUIApplication*  app_;
  OriginTouches    m_origin_touches;
  KeyboardAdapter* m_keyboard;
  TextInput*       m_text_input;
  
  av_def_inl_cls(Inl);
};

av_gui_end
#endif
