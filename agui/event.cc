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

#include "event.h"
#include "app-1.h"
#include "root.h"
#include "button.h"
#include "css.h"

av_gui_begin

typedef value::CGRect CGRect;

const GUIEventName GUI_EVENT_KEYDOWN          ("keydown", GUI_EVENT_CATEGORY_KEYBOARD);
const GUIEventName GUI_EVENT_KEYPRESS         ("keypress", GUI_EVENT_CATEGORY_KEYBOARD);
const GUIEventName GUI_EVENT_KEYUP            ("keyup", GUI_EVENT_CATEGORY_KEYBOARD);
const GUIEventName GUI_EVENT_KEYESC           ("keyesc", GUI_EVENT_CATEGORY_KEYBOARD);
const GUIEventName GUI_EVENT_KEYENTER         ("keyenter", GUI_EVENT_CATEGORY_KEYBOARD);
const GUIEventName GUI_EVENT_BACK             ("back", GUI_EVENT_CATEGORY_CLICK);
const GUIEventName GUI_EVENT_CLICK            ("click", GUI_EVENT_CATEGORY_CLICK);
const GUIEventName GUI_EVENT_HIGHLIGHTED      ("highlighted", GUI_EVENT_CATEGORY_HIGHLIGHTED);
const GUIEventName GUI_EVENT_MOUSEDOWN        ("mousedown", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSEMOVE        ("mousemove", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSEUP          ("mouseup", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSEENTER       ("mouseenter", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSELEAVE       ("mouseleave", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSEOVER        ("mouseover", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSEOUT         ("mouseout", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_MOUSEWHELL       ("mousewhell", GUI_EVENT_CATEGORY_MOUSE);
const GUIEventName GUI_EVENT_TOUCHSTART       ("touchstart", GUI_EVENT_CATEGORY_TOUCH);
const GUIEventName GUI_EVENT_TOUCHMOVE        ("touchmove", GUI_EVENT_CATEGORY_TOUCH);
const GUIEventName GUI_EVENT_TOUCHEND         ("touchend", GUI_EVENT_CATEGORY_TOUCH);
const GUIEventName GUI_EVENT_TOUCHCANCEL      ("touchcancel", GUI_EVENT_CATEGORY_TOUCH);
const GUIEventName GUI_EVENT_FOCUS            ("focus", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_BLUR             ("blur", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_REMOVE_VIEW      ("remove_view", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_ACTION_KEYFRAME  ("action_keyframe", GUI_EVENT_CATEGORY_ACTION);
const GUIEventName GUI_EVENT_ACTION_LOOP      ("action_loop", GUI_EVENT_CATEGORY_ACTION);
const GUIEventName GUI_EVENT_SWITCH           ("switch", GUI_EVENT_CATEGORY_SWITCH);
const GUIEventName GUI_EVENT_ENTER            ("enter", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_LEAVE            ("leave", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_SCROLL           ("scroll", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_CHANGE           ("change", GUI_EVENT_CATEGORY_DEFAULT);

const Map<String, GUIEventName> GUI_EVENT_TABLE([]() -> Map<String, GUIEventName> {
  Map<String, GUIEventName> r;
  r.set(GUI_EVENT_KEYDOWN.to_string(), GUI_EVENT_KEYDOWN);
  r.set(GUI_EVENT_KEYPRESS.to_string(),  GUI_EVENT_KEYPRESS);
  r.set(GUI_EVENT_KEYUP.to_string(),  GUI_EVENT_KEYUP);
  r.set(GUI_EVENT_KEYESC.to_string(),  GUI_EVENT_KEYESC);
  r.set(GUI_EVENT_KEYENTER.to_string(),  GUI_EVENT_KEYENTER);
  r.set(GUI_EVENT_BACK.to_string(),  GUI_EVENT_BACK);
  r.set(GUI_EVENT_CLICK.to_string(),  GUI_EVENT_CLICK);
  r.set(GUI_EVENT_HIGHLIGHTED.to_string(),  GUI_EVENT_HIGHLIGHTED);
  r.set(GUI_EVENT_MOUSEDOWN.to_string(),  GUI_EVENT_MOUSEDOWN);
  r.set(GUI_EVENT_MOUSEMOVE.to_string(),  GUI_EVENT_MOUSEMOVE);
  r.set(GUI_EVENT_MOUSEUP.to_string(),  GUI_EVENT_MOUSEUP);
  r.set(GUI_EVENT_MOUSEENTER.to_string(),  GUI_EVENT_MOUSEENTER);
  r.set(GUI_EVENT_MOUSELEAVE.to_string(),  GUI_EVENT_MOUSELEAVE);
  r.set(GUI_EVENT_MOUSEOVER.to_string(),  GUI_EVENT_MOUSEOVER);
  r.set(GUI_EVENT_MOUSEOUT.to_string(),  GUI_EVENT_MOUSEOUT);
  r.set(GUI_EVENT_MOUSEWHELL.to_string(),  GUI_EVENT_MOUSEWHELL);
  r.set(GUI_EVENT_TOUCHSTART.to_string(),  GUI_EVENT_TOUCHSTART);
  r.set(GUI_EVENT_TOUCHMOVE.to_string(),  GUI_EVENT_TOUCHMOVE);
  r.set(GUI_EVENT_TOUCHEND.to_string(),  GUI_EVENT_TOUCHEND);
  r.set(GUI_EVENT_TOUCHCANCEL.to_string(),  GUI_EVENT_TOUCHCANCEL);
  r.set(GUI_EVENT_FOCUS.to_string(),  GUI_EVENT_FOCUS);
  r.set(GUI_EVENT_BLUR.to_string(),  GUI_EVENT_BLUR);
  r.set(GUI_EVENT_REMOVE_VIEW.to_string(), GUI_EVENT_REMOVE_VIEW);
  r.set(GUI_EVENT_ACTION_KEYFRAME.to_string(), GUI_EVENT_ACTION_KEYFRAME);
  r.set(GUI_EVENT_ACTION_LOOP.to_string(),  GUI_EVENT_ACTION_LOOP);
  return r;
}());

static inline HighlightedStatus HOVER_or_NORMAL(View* view) {
  return view->is_focus() ? HIGHLIGHTED_HOVER : HIGHLIGHTED_NORMAL;
}

template<class T, typename... Args>
inline static Handle<T> NewEvent(Args... args) { return new T(args...); }

class GUIResponder::Inl: public GUIResponder {
public:
#define _inl_res(self) static_cast<GUIResponder::Inl*>(static_cast<GUIResponder*>(self))
  
  /**
   * @func trigger_highlightted
   */
  ReturnValue& trigger_highlightted(GUIHighlightedEvent& evt) {
    evt.return_value = 1;
    
    View* view = sender();
    while( view ) {
      if ( view->m_has_receive ) {
        if ( view->Responder::trigger(GUI_EVENT_HIGHLIGHTED, evt) ) {
          CSSViewClasss* classs = sender()->classs();
          if ( classs ) { // 切换样式表状态
            classs->set_style_pseudo_status( CSSPseudoClass(evt.status()) );
          }
        } else {
          break; // Stop bubble
        }
      }
      view = view->parent();
    }
    return evt.return_value;
  }

  int& trigger(const Name& name, GUIEvent& evt) {
    return GUIResponder::trigger(name, evt);
  }
  
  /**
   * @func bubble_trigger
   */
  int& bubble_trigger(const Name& name, GUIEvent& evt) {
    evt.return_value = 1;
    
    View* view = sender();
    while( view ) {
      if ( view->m_has_receive ) {
        if ( ! view->Responder::trigger(name, evt) ) {
          break; // Stop bubble
        }
      }
      view = view->parent();
    }
    return evt.return_value;
  }
  
};

int& GUIResponder::trigger(const Name& name, GUIEvent& evt, bool force) {
  if ( m_has_receive || force ) {
    return Responder::trigger(name, evt);
  }
  return evt.return_value;
}

/**
 * @func trigger
 */
int GUIResponder::trigger(const Name& name, bool force) {
  if ( m_has_receive || force ) {
    auto del = noticer(name);
    if ( del ) {
      return del->trigger(**NewEvent<GUIEvent>(sender()));
    }
  }
  return 0;
}

void GUIResponder::trigger_listener_change(const Name& name, int count, int change) {
  if ( change > 0 ) {
    m_has_receive = true; // bind event auto open option
  }
}

/**
 * @func focus
 */
bool GUIResponder::focus() {
  if ( is_focus() ) {
    return true;
  }
  
  View* v = static_cast<View*>(this);
  
  if ( ! v->final_visible() ) {
    return false;
  }

  View* v1 = static_cast<View*>(this);
  View* v2 = static_cast<View*>(_inl_app(app())->first_responder());
  Panel* act1 = v1->as_button() ? v1->as_button()->panel() : nullptr;
  Panel* act2 = v2->as_button() ? v2->as_button()->panel() : nullptr;
  
  if ( v2 ) {
    _inl_res(v2)->bubble_trigger(GUI_EVENT_BLUR, **NewEvent<GUIEvent>(v2));
    
    if ( act2 && act1 != act2 ) { // leave
      _inl_res(act2)->trigger(GUI_EVENT_LEAVE, **NewEvent<GUIEvent>(v2));
    }
    
    _inl_res(v2)->trigger_highlightted(**NewEvent<GUIHighlightedEvent>(v2, HIGHLIGHTED_NORMAL));
  }
  
  _inl_app(app())->set_first_responder(this);
  
  _inl_res(this)->bubble_trigger(GUI_EVENT_FOCUS, **NewEvent<GUIEvent>(v1));
  
  if ( act1 && act1 != act2 ) { // enter
    Panel* panel = v1->as_button()->panel();
    _inl_res(act1)->trigger(GUI_EVENT_ENTER, **NewEvent<GUIEvent>(v1));
  }
  
  _inl_res(this)->trigger_highlightted(
     **NewEvent<GUIHighlightedEvent>(v1, HIGHLIGHTED_HOVER)
  );

  return true;
}

/**
 * @func blur
 */
bool GUIResponder::blur() {
  if ( is_focus() ) {
    View* v = root();
    if ( v ) {
      if ( v != this ) {
        return v->focus();
      }
    }
    return false;
  }
  return true;
}

/**
 * @func is_focus
 */
bool GUIResponder::is_focus() const {
  return this == _inl_app(app())->first_responder();
}

/**
 * @class GUIEventEmitter::OriginTouche
 */
class GUIEventEmitter::OriginTouche {
public:
  
  OriginTouche() { av_unreachable(); }
  OriginTouche(View* view)
  : _view(view)
  , _start_position(view_position(view))
  , _is_click_invalid(false), _is_click_down(false) {
    _view->retain();
  }
  ~OriginTouche() {
    _view->release();
  }
  static Vec2 view_position(View* view) {
    return Vec2(view->m_final_matrix[2], view->m_final_matrix[5]);
  }
  inline View* view() { return _view; }
  inline Vec2 view_start_position() { return _start_position; }
  inline bool is_click_invalid() { return _is_click_invalid; }
  inline bool is_click_down() { return _is_click_down; }
  inline void set_click_invalid() {
    _is_click_invalid = true;
    _is_click_down = false;
  }
  inline void set_click_down(bool value) {
    if ( !_is_click_invalid )
      _is_click_down = value;
  }
  inline Map<uint, GUITouch>& values() { return _touches; }
  inline GUITouch& operator[](uint id) { return _touches[id]; }
  inline uint count() { return _touches.length(); }
  inline bool has(uint id) { return _touches.has(id); }
  inline void del(uint id) { _touches.del(id); }
private:
  View* _view;
  Map<uint, GUITouch> _touches;
  Vec2  _start_position;
  bool  _is_click_invalid;
  bool  _is_click_down;
};

/**
 * @class GUIEventEmitter::Inl
 */
av_def_inl_members(GUIEventEmitter, Inl) {
public:
  #define _inl_emit(self) static_cast<GUIEventEmitter::Inl*>(self)
  
  // -------------------------- touch --------------------------
  
  void touch_start3(View* view, List<GUITouch>& in) {
    if ( view->receive() && view->m_visible_draw && in.length() ) {
      Array<GUITouch> change_touches;
      
      for ( auto i = in.begin(), e = in.end(); i != e; ) {
        auto j = i++;
        
        if ( view->overlap_test(Vec2(j.value().x, j.value().y)) ) {
          GUITouch& touch = j.value();
          touch.start_x = touch.x;
          touch.start_y = touch.y;
          touch.click_in = true;
          touch.view = view;
          
          if ( !m_origin_touches.has(view) ) {
            m_origin_touches.set(view, new OriginTouche(view));
          }
          (*m_origin_touches[view])[touch.id] = touch;
          
          change_touches.push( touch );
          in.del(j);
        }
      }
      
      if ( change_touches.length() ) { // notice
        auto evt = NewEvent<GUITouchEvent>(view, change_touches);
        _inl_res(view)->bubble_trigger(GUI_EVENT_TOUCHSTART, **evt); // emit event
        
        if ( !m_origin_touches[view]->is_click_down() ) { // trigger click down
          m_origin_touches[view]->set_click_down(true);
          auto evt = NewEvent<GUIHighlightedEvent>(view, HIGHLIGHTED_DOWN);
          _inl_res(view)->trigger_highlightted(**evt); // emit event
        }
      }
    }
  }
  
  /**
   * @func touch_start2
   */
  void touch_start2(View* view, List<GUITouch>& in) {
    
    if ( view->m_visible && in.length() ) {
      if ( view->m_visible_draw || view->m_need_draw ) {
        
        if ( view->m_last && view->is_clip() ) {
          List<GUITouch> in2;
          
          for ( auto i = in.begin(), e = in.end(); i != e; ) {
            auto j = i++;
            if ( view->overlap_test(Vec2(j.value().x, j.value().y)) ) {
              in2.push(j.value());
              in.del(j);
            }
          }
          
          View* v = view->m_last;
          while( v && in2.length() ) {
            touch_start2(v, in2);
            v = v->m_prev;
          }
          
          touch_start3(view, in2);
          
          if ( in2.length() ) {
            in.push(move(in2));
          }
        } else {
          View* v = view->m_last;
          while( v && in.length() ) {
            touch_start2(v, in);
            v = v->m_prev;
          }
          
          touch_start3(view, in);
        }
      }
      
    }
  }
  
  /**
   * @func touch_start
   */
  void touch_start(List<GUITouch>& in) {
    Root* r = app_->root();
    if (r) {
      touch_start2(r, in);
    }
  }
  
  /**
   * @func touch_move
   */
  void touch_move(List<GUITouch>& in) {
    Map<PrtKey<View>, Array<GUITouch>> change_touches;
    
    for ( auto& i : in ) {
      GUITouch& in_touch = i.value();
      for ( auto& touches : m_origin_touches ) {
        if ( touches.value()->has(in_touch.id) ) {
          GUITouch& touch = (*touches.value())[in_touch.id];
          touch.x = in_touch.x;
          touch.y = in_touch.y;
          touch.force = in_touch.force;
          if ( !touches.value()->is_click_invalid() ) {
            touch.click_in = touch.view->overlap_test(Vec2(touch.x, touch.y));
            //if ( !touch.is_click_range ) {
            //  av_debug("Out click range");
            //}
          }
          change_touches[touch.view].push(touch);
          break;
        }
      }
    }
    
    for ( auto& i : change_touches ) {
      
      Array<GUITouch>& touchs = i.value();
      View* view = touchs[0].view;
      // emit event
      _inl_res(view)->bubble_trigger(
         GUI_EVENT_TOUCHMOVE,
         **NewEvent<GUITouchEvent>(view, i.value())
       );
      
      OriginTouche* origin_touche = m_origin_touches[view];
      
      if ( !origin_touche->is_click_invalid() ) { // no invalid
        Vec2 position = OriginTouche::view_position(view);
        Vec2 start_position = origin_touche->view_start_position();
        
        float d = sqrtf(powf((position.x() - start_position.x()), 2) +
                        powf((position.y() - start_position.y()), 2));
        // 视图位置移动超过2取消点击状态
        if ( d > 2 ) { // trigger invalid status
          if ( origin_touche->is_click_down() ) { // trigger style up
            // emit style status event
            auto evt = NewEvent<GUIHighlightedEvent>(view, HOVER_or_NORMAL(view));
            _inl_res(view)->trigger_highlightted(**evt);
          }
          origin_touche->set_click_invalid();
        }
        else { // no invalid
          
          if ( origin_touche->is_click_down() ) { // May trigger click up
            bool trigger_event = true;
            for ( auto& t : origin_touche->values() ) {
              if (t.value().click_in) {
                trigger_event = false; break;
              }
            }
            if ( trigger_event ) {
              origin_touche->set_click_down(false); // set up status
              // emit style status event
              auto evt = NewEvent<GUIHighlightedEvent>(view, HOVER_or_NORMAL(view));
              _inl_res(view)->trigger_highlightted(**evt);
            }
          } else { // May trigger click down
            for ( auto& item : touchs ) {
              if ( item.value().click_in ) { // find range == true
                origin_touche->set_click_down(true); // set down status
                // emit style down event
                auto evt = NewEvent<GUIHighlightedEvent>(view, HIGHLIGHTED_DOWN);
                _inl_res(view)->trigger_highlightted(**evt);
                break;
              }
            }
          }
        } // no invalid end
      } // if end
    } // each end
  }
  
  /**
   * @func touch_end2
   */
  void touch_end2(List<GUITouch>& in, const GUIEventName& type) {
    
    Map<PrtKey<View>, Array<GUITouch>> change_touches;
    
    for ( auto& i : in ) {
      GUITouch& in_touch = i.value();
      for ( auto& item : m_origin_touches ) {
        if ( item.value()->has(in_touch.id) ) {
          GUITouch& touch = (*item.value())[in_touch.id];
          touch.x = in_touch.x;
          touch.y = in_touch.y;
          touch.force = in_touch.force;
          change_touches[touch.view].push(touch);
          item.value()->del(touch.id); // del touch point
          break;
        }
      }
    }
    
    for ( auto& i : change_touches ) { // views
      
      Array<GUITouch>& touchs = i.value();
      View* view = touchs[0].view;
      _inl_res(view)->bubble_trigger(type, **NewEvent<GUITouchEvent>(view, touchs)); // emit touch end event
      
      OriginTouche* origin_touche = m_origin_touches[view];
      
      if ( origin_touche->count() == 0 ) {
        if ( origin_touche->is_click_down() ) { // trigger click
          for ( auto& item : touchs ) {
            // find range == true
            if ( item.value().click_in ) {
              // emit style up event
              auto evt = NewEvent<GUIHighlightedEvent>(view, HOVER_or_NORMAL(view));
              _inl_res(view)->trigger_highlightted(**evt);
              
              if ( type == GUI_EVENT_TOUCHEND ) {
                auto evt = NewEvent<GUIClickEvent>(view, item.value().x, item.value().y);
                _inl_res(view)->bubble_trigger(GUI_EVENT_CLICK, **evt); // emit click event
              }
              break;
            }
          }
        }
        delete origin_touche;
        m_origin_touches.del(view); // del
      }
      //
    }
  }
  
  /**
   * @func touch_end
   */
  void touch_end(List<GUITouch>& in) {
    touch_end2(in, GUI_EVENT_TOUCHEND);
  }
  
  /**
   * @func touch_cancel
   */
  void touch_cancel(List<GUITouch>& in) {
    touch_end2(in, GUI_EVENT_TOUCHCANCEL);
  }
  
  void receive_touch_start(SimpleEvent& evt) {
    touch_start(*static_cast<List<GUITouch>*>(evt.data));
  }
  
  void receive_touch_move(SimpleEvent& evt) {
    touch_move(*static_cast<List<GUITouch>*>(evt.data));
  }
  
  void receive_touch_end(SimpleEvent& evt) {
    touch_end(*static_cast<List<GUITouch>*>(evt.data));
  }
  
  void receive_touch_cancel(SimpleEvent& evt) {
    touch_cancel(*static_cast<List<GUITouch>*>(evt.data));
  }
  
  // -------------------------- mouse --------------------------

  void mouse_down(Vec2 point, int key_code) {
    
  }

  void mouse_move(Vec2 point, int key_code) {
    
  }

  void mouse_up(Vec2 point, int key_code) {
    
  }

  void mouse_wheel(int len) { 
    
  }
  
  // -------------------------- keyboard --------------------------

  void keyboard_down() {
    
    KeyboardKeyName name = m_keyboard->keyname();

    View* view = static_cast<View*>(_inl_app(app_)->first_responder());
    if ( view ) {

      auto evt = NewEvent<GUIKeyEvent>(view, name,
        m_keyboard->shift(),
        m_keyboard->ctrl(), m_keyboard->alt(),
        m_keyboard->command(), m_keyboard->caps_lock(),
        m_keyboard->repeat(), m_keyboard->device(), m_keyboard->source()
      );
      
      int r = _inl_res(view)->bubble_trigger(GUI_EVENT_KEYDOWN, **evt);
      
      if ( r ) {
        
        if ( name == KEYCODE_ESC ) {
          _inl_res(view)->bubble_trigger(GUI_EVENT_KEYESC, **evt);
        } else if ( name == KEYCODE_ENTER ) {
          _inl_res(view)->bubble_trigger(GUI_EVENT_KEYENTER, **evt);
        }
        
        int keypress_code = m_keyboard->keypress();
        if ( keypress_code ) { // keypress
          evt->set_keycode( keypress_code );
          _inl_res(view)->bubble_trigger(GUI_EVENT_KEYPRESS, **evt);
        }
      }
      
      if ( name == KEYCODE_CENTER && m_keyboard->repeat() == 0 ) {
        CGRect rect = view->screen_rect();
        auto evt = NewEvent<GUIHighlightedEvent>(view, HIGHLIGHTED_DOWN);
        _inl_res(view)->trigger_highlightted(**evt); // emit click status event
      }
      
      if ( !r ) { // 取消
        return;
      }
      
      Direction direction = Direction::LEFT;
      
      switch ( name ) {
        case KEYCODE_LEFT: direction = Direction::LEFT; break; // left
        case KEYCODE_UP: direction = Direction::TOP; break; // top
        case KEYCODE_RIGHT: direction = Direction::RIGHT; break; // right
        case KEYCODE_DOWN: direction = Direction::BOTTOM; break; // bottom
        default: return;
      }

      Handle<GUISwitchEvent> evt2;
      Button* button = view->as_button();
      
      if ( button ) {
        Panel* panel = button->panel();
        if ( panel->enable_switch() ) {
          Button* target = button->find_next_button(direction, panel);
          evt2 = NewEvent<GUISwitchEvent>(panel, direction, button, target);
        } // end if ( activity->enable_switch() ) {
      } else { // else
        Root* r = root();
        if ( r->enable_switch() ) {
          evt2 = NewEvent<GUISwitchEvent>(r, direction, nullptr, r->first_button());
        }
      } // end if ( button )

      if ( ! evt2.is_null() ) {
        evt2->return_value = 1;
        if ( _inl_res(evt2->origin())->trigger(GUI_EVENT_SWITCH, **evt2 ) ) { // target event
          if ( evt2->target_button() ) {
            evt2->target_button()->focus();
          }
        }
      } // if ( ! evt.is_null() ) {
    }
  }
  
  void keyboard_up() {
    
    KeyboardKeyName name = m_keyboard->keyname();

    View* view = static_cast<View*>(_inl_app(app_)->first_responder());
    if ( view ) {
      auto evt = NewEvent<GUIKeyEvent>(view, name,
        m_keyboard->shift(),
        m_keyboard->ctrl(), m_keyboard->alt(),
        m_keyboard->command(), m_keyboard->caps_lock(),
        m_keyboard->repeat(), m_keyboard->device(), m_keyboard->source()
      );
      
      if ( _inl_res(view)->bubble_trigger(GUI_EVENT_KEYUP, **evt) ) {
        if ( name == KEYCODE_BACK ) {
          CGRect rect = view->screen_rect();
          auto evt = NewEvent<GUIClickEvent>(view, rect.origin.x() + rect.size.x() / 2,
                                              rect.origin.y() + rect.size.y() / 2);
          _inl_res(view)->bubble_trigger(GUI_EVENT_BACK, **evt); // emit back
        } 
        else if ( name == KEYCODE_CENTER ) {
          auto evt = NewEvent<GUIHighlightedEvent>(view, HIGHLIGHTED_HOVER);
          _inl_res(view)->trigger_highlightted(**evt); // emit style status event
          
          CGRect rect = view->screen_rect();
          auto evt2 = NewEvent<GUIClickEvent>(view, rect.origin.x() + rect.size.x() / 2,
                                             rect.origin.y() + rect.size.y() / 2);
          _inl_res(view)->bubble_trigger(GUI_EVENT_CLICK, **evt2);
        } // 
      }
    }
  }
  
};

GUIEventEmitter::GUIEventEmitter(GUIApplication* app): app_(app), m_text_input(nullptr) {
  m_keyboard = KeyboardAdapter::create();
}

GUIEventEmitter::~GUIEventEmitter() {
  for (auto& i : m_origin_touches) {
    delete i.value();
  }
  Release(m_keyboard);
}

#define _loop static_cast<PostMessage*>(app_->loop())

void KeyboardAdapter::receive(int keycode, bool soft,
                              bool is_down, int repeat, int device, int source) {
  
  async_callback(Callback([=](SimpleEvent& evt) {
    repeat_ = repeat; device_ = device;
    source_ = source;
    
    bool clear = transformation(keycode, soft, is_down);
    
    if ( is_down ) {
      _inl_emit(_inl_app(app_)->emitter())->keyboard_down();
    } else {
      _inl_emit(_inl_app(app_)->emitter())->keyboard_up();
    }
    
    if ( clear ) {
      shift_ = alt_ = false;
      ctrl_ = command_ = false;
    }
  }), _loop);
}

void GUIEventEmitter::receive_touch_start(List<GUITouch>&& list) {
  async_callback(Callback(&Inl::receive_touch_start, _inl_emit(this)), move(list), _loop);
}

void GUIEventEmitter::receive_touch_move(List<GUITouch>&& list) {
  async_callback(Callback(&Inl::receive_touch_move, _inl_emit(this)), move(list), _loop);
}

void GUIEventEmitter::receive_touch_end(List<GUITouch>&& list) {
  async_callback(Callback(&Inl::receive_touch_end, _inl_emit(this)), move(list), _loop);
}

void GUIEventEmitter::receive_touch_cancel(List<GUITouch>&& list) {
  async_callback(Callback(&Inl::receive_touch_cancel, _inl_emit(this)), move(list), _loop);
}

void GUIEventEmitter::receive_mouse_down(Vec2 point, int key_code) {
  async_callback(Callback([=](SimpleEvent& d) {
    _inl_emit(this)->mouse_down(point, key_code);
  }), _loop);
}

void GUIEventEmitter::receive_mouse_move(Vec2 point, int key_code) {
  async_callback(Callback([=](SimpleEvent& d) {
    _inl_emit(this)->mouse_move(point, key_code);
  }), _loop);
}

void GUIEventEmitter::receive_mouse_up(Vec2 point, int key_code) {
  async_callback(Callback([=](SimpleEvent& d) {
    _inl_emit(this)->mouse_up(point, key_code);
  }), _loop);
}

void GUIEventEmitter::receive_mouse_wheel(int len) {
  async_callback(Callback([=](SimpleEvent& d) {
    _inl_emit(this)->mouse_wheel(len);
  }), _loop);
}

void GUIEventEmitter::receive_input_delete_text(int count) {
  async_callback(Callback([=](SimpleEvent& d) {
    if ( m_text_input ) {
      m_text_input->input_delete_text(count);
      bool can_back_space = m_text_input->input_can_back_space();
      bool can_delete = m_text_input->input_can_delete();
      _inl_app(app_)->input_keyboard_can_back_space(can_back_space, can_delete);
    }
  }), _loop);
}

void GUIEventEmitter::receive_input_insert_text(cString& text) {
  async_callback(Callback([=](SimpleEvent& d) {
    if ( m_text_input ) {
      m_text_input->input_insert_text(text);
    }
  }), _loop);
}

void GUIEventEmitter::receive_input_marked_text(cString& text) {
  async_callback(Callback([=](SimpleEvent& d) {
    if ( m_text_input ) {
      m_text_input->input_marked_text(text);
    }
  }), _loop);
}

void GUIEventEmitter::receive_input_unmark_text(cString& text) {
  async_callback(Callback([=](SimpleEvent& d) {
    if ( m_text_input ) {
      m_text_input->input_unmark_text(text);
    }
  }), _loop);
}

/**
 * @func make_text_input
 */
void GUIEventEmitter::make_text_input(TextInput* input) {
  if ( input != m_text_input ) {
    TextInput* old = m_text_input;
    m_text_input = input;
    
    if ( input ) {
      _inl_app(app_)->input_keyboard_open({
        true, input->keyboard_type(), input->keyboard_return_type()
      });
    } else {
      _inl_app(app_)->input_keyboard_close();
    }
  } else {
    if ( input ) {
      _inl_app(app_)->input_keyboard_open({
        false, input->keyboard_type(), input->keyboard_return_type()
      });
    }
  }
}

av_gui_end
