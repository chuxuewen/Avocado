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

#include "action.h"
#include "view.h"

av_gui_begin

/**
 * @class GUIResponder::ActionInl
 */
class GUIResponder::ActionInl: public GUIResponder {
public:
#define _inl_gui_responder(self) \
  static_cast<GUIResponder::ActionInl*>(static_cast<GUIResponder*>(self))
  
  /**
   * @func trigger
   */
  inline ReturnValue& trigger(const Name& name, GUIEvent& evt) {
    return GUIResponder::trigger(name, evt);
  }
  
};

/**
 * @class ActionCenter::Inl
 */
class ActionCenter::Inl: public ActionCenter {
public:
#define _inl_action_center(self) static_cast<ActionCenter::Inl*>(self)
  
  /**
   * @func add
   */
  void add(Action* action) {
    if ( action->m_action_center_id.is_null() ) {
      action->m_action_center_id = m_actions.push({ action, 0 });
      action->retain();
    }
  }
  
  /**
   * @func del
   */
  void del(Action* action) {
    if ( action && !action->m_action_center_id.is_null() ) {
      action->m_action_center_id.value().value = nullptr; // del
      // m_actions.del(action->m_action_center_id);
      action->m_action_center_id = List<Action::Wrap>::Iterator();
      action->release();
    }
  }
  
};

/**
 * @class Action::Inl
 */
class Action::Inl: public Action {
public:
#define _inl_action(self) static_cast<Action::Inl*>(static_cast<Action*>(self))
  
  void set_parent(Action* parent) av_def_err {
    
    if ( m_parent || m_views.length() || !m_action_center_id.is_null() ) {
      av_throw(ERR_ACTION_ILLEGAL_CHILD, "illegal child action!");
    }
    
    retain(); // retain
    
    // bind view
    m_parent = parent;
    while ( parent->m_parent ) {
      parent = parent->m_parent;
    }
    
    View* first = first_view();
    
    if ( first ) {
      bind_view( first );
    }
  }
  
  View* first_view() {
    for ( auto& i : m_views ) {
      if (i.value()) {
        return i.value();
      }
    }
    return nullptr;
  }
  
  void clear_parent() {
    m_parent = nullptr;
    release();
  }
  
  /**
   * @func view
   */
  inline View* view() {
    Action* action = this;
    while ( action->m_parent ) {
      action = action->m_parent;
    }
    return first_view();
  }
  
  /**
   * @func views
   */
  inline List<View*>& views() {
    return m_views;
  }
  
  /**
   * @func is_playing with root
   */
  inline bool is_playing() {
    return ! m_action_center_id.is_null();
  }
  
  /**
   * @func trigger_action_loop
   */
  void trigger_action_loop(uint64 delay, Action* root) {
    Handle<GUIActionEvent> evt = New<GUIActionEvent>(this, delay, 0, m_loop);
    for ( auto i = m_views.begin(); !i.is_null(); ) { // trigger event action_loop
      if (i.value()) {
        evt->set_origin(i.value());
        _inl_gui_responder(i.value())->trigger(GUI_EVENT_ACTION_LOOP, **evt);
        i++;
      } else {
        m_views.del(i++);
      }
    }
  }
  
  /**
   * @func trigger_action_key_frame
   */
  void trigger_action_key_frame(uint64 delay, uint frame_index, Action* root) {
    Handle<GUIActionEvent> evt = New<GUIActionEvent>(this, delay, frame_index, m_loop);
    for ( auto i = m_views.begin(); !i.is_null(); ) { // trigger event action_keyframe
      if (i.value()) {
        evt->set_origin(i.value());
        _inl_gui_responder(i.value())->trigger(GUI_EVENT_ACTION_KEYFRAME, **evt);
        i++;
      } else {
        m_views.del(i++);
      }
    }
  }
  
  /**
   * @func update_duration
   */
  void update_duration(int64 duration) {
    // TODO ***************************** SpawnAction BUG *****************************
    Action* action = this;
    do {
      action->m_duration += duration;
      action = m_parent;
    } while(action);
  }
  
  /**
   * @func add_view
   */
  void add_view(View* view) av_def_err {
    
    if ( m_parent ) {
      av_throw(ERR_ACTION_ILLEGAL_ROOT, "Cannot set non root action !");
    }
    View* first = first_view();
    if ( first ) {
      if ( first->view_type() != view->view_type() ) {
        av_throw(ERR_ACTION_ILLEGAL_VIEW_TYPE, "Action can only be bound to the same type of view !");
      }
    } else {
      bind_view(view);
    }
    m_views.push({view});
  }
  
  /**
   * @func del_view
   */
  void del_view(View* view) {
    uint len = m_views.length();
    for ( auto& i : m_views ) {
      if ( i.value() == view ) {
        i.value() = nullptr;
        len--;
        break;
      }
    }
    if ( len == 0 ) {
      stop(); // stop action
    }
  }
  
};

#include "action.cc.inl"
#include "action.cc.frame.inl"

/**
 * @func GroupAction::Inl
 */
class GroupAction::Inl: public GroupAction {
public:
#define _inl_group_action(self) static_cast<GroupAction::Inl*>(static_cast<GroupAction*>(self))
  
  /**
   * @func clear_all
   */
  void clear_all() {
    
    for ( auto& i : m_actions ) {
      GroupAction* group = i.value()->as_group();
      if (group) {
        _inl_group_action(group)->clear_all();
        if ( group->as_sequence() ) {
          group->as_sequence()->m_action = Iterator();
        }
      }
      _inl_action(i.value())->clear_parent();
    }
    m_actions.clear();
    m_actions_index.clear();
    m_duration = 0;
    m_delay = 0;
  }
  
  /**
   * @func m_remove
   */
  uint64 m_remove(uint index) {
    Iterator it =
      m_actions_index.length() == m_actions.length() ?
      m_actions_index[index] : m_actions.find(index);
    uint64 duration = 0;
    if ( it != m_actions.end() ) {
      duration = it.value()->duration();
      _inl_action(it.value())->clear_parent();
      m_actions.del( it );
      m_actions_index.clear();
    }
    return 0;
  }
  
};

/**
 * @func time set
 */
void Frame::set_time(uint64 value) {
  if ( m_host && m_index && value != m_time ) {
    uint next = m_index + 1;
    if ( next < m_host->length() ) {
      uint64 max_time = m_host->frame(next)->time();
      m_time = av_min(value, max_time);
    } else { // no next
      m_time = value;
    }
  }
}

/**
 * @func fetch property
 */
void Frame::fetch(View* view) {
  if ( view && view->view_type() == m_host->m_bind_view_type ) {
    for ( auto& i : m_host->m_property ) {
      i.value()->fetch(m_index, view);
    }
  } else {
    view = _inl_action(m_host)->view();
    if ( view ) {
      for ( auto& i : m_host->m_property ) {
        i.value()->fetch(m_index, view);
      }
    }
  }
}

/**
 * @func flush recovery default property value
 */
void Frame::flush() {
  for ( auto& i : m_host->m_property ) {
    i.value()->default_value(m_index);
  }
}

/**
 * @class KeyframeAction::Inl
 */
class KeyframeAction::Inl: public KeyframeAction {
public:
#define _inl_key_action(self) static_cast<KeyframeAction::Inl*>(self)
  
  /**
   * @func transition
   */
  inline void transition(uint f1, uint f2, float x, float y, Action* root) {
    for ( auto& i : m_property ) {
      i.value()->transition(f1, f2, x, y, root);
    }
  }
  
  /**
   * @func transition
   */
  inline void transition(uint f1, Action* root) {
    for ( auto& i : m_property ) {
      i.value()->transition(f1, root);
    }
  }
  
  /**
   * @func advance
   */
  uint64 advance(uint64 time_span, Action* root) {
    
  start:
    
    uint f1 = m_frame;
    uint f2 = f1 + 1;
    
    if ( f2 < length() ) {
    advance:
      
      if ( ! _inl_action(root)->is_playing() ) { // is playing
        return 0;
      }
      
      int64 time = m_time + time_span;
      uint64 time1 = m_frames[f1]->time();
      uint64 time2 = m_frames[f2]->time();
      int64 t = time - time2;
      
      if ( t < 0 ) {
        
        time_span = 0;
        m_time = time;
        float x = (time - time1) / float(time2 - time1);
        float y = m_frames[f1]->curve().solve(x, 0.001);
        transition(f1, f2, x, y, root);
        
      } else if ( t > 0 ) {
        time_span = t;
        m_frame = f2;
        m_time = time2;
        _inl_action(this)->trigger_action_key_frame(t, f2, root); // trigger event action_key_frame
        
        f1 = f2; f2++;
        
        if ( f2 < length() ) {
          goto advance;
        } else {
          if ( m_loop && m_duration ) {
            goto loop;
          } else {
            transition(f1, root);
          }
        }
      } else { // t == 0
        time_span = 0;
        m_time = time;
        m_frame = f2;
        transition(f2, root);
        _inl_action(this)->trigger_action_key_frame(0, f2, root); // trigger event action_key_frame
      }
      
    } else { // last frame
      
      if ( m_loop && m_duration - m_delay ) {
      loop:
        m_loop--;
        m_frame = 0;
        m_time = 0;
        _inl_action(this)->trigger_action_loop(time_span, root);
        _inl_action(this)->trigger_action_key_frame(time_span, 0, root);
        goto start;
      }
    }
    
    return time_span;
  }
  
};

/**
 * @func action
 */
void View::action(Action* action) av_def_err {
  if ( action ) {
    if ( m_action ) {
      _inl_action(m_action)->del_view(this);
      m_action->release();
    }
    _inl_action(action)->add_view(this);
    m_action = action;
    action->retain();
  } else {
    if ( m_action ) {
      _inl_action(m_action)->del_view(this);
      m_action->release();
      m_action = nullptr;
    }
  }
}

Action::Action()
: m_parent(nullptr)
, m_loop(0)
, m_duration(0)
, m_delay(0)
, m_delayed(-1), m_speed(1) { }

/**
 * @destructor
 */
Action::~Action() {
  av_assert( m_action_center_id.is_null() );
}

/**
 * @func delay
 */
void Action::delay(uint64 value) {
  int64 du = value - m_delay;
  if ( du ) {
    m_delay = value;
    _inl_action(this)->update_duration(du);
  }
}

/**
 * @func playing
 */
bool Action::playing() const {
  return m_parent ? m_parent->playing() : !m_action_center_id.is_null();
}

/**
 * @func play
 */
void Action::play() {
  if ( m_parent ) {
    m_parent->play();
  } else {
    if ( m_views.length() ) {
      _inl_action_center(ActionCenter::shared())->add(this);
    }
  }
}

/**
 * @func stop
 */
void Action::stop() {
  if ( m_parent ) {
    m_parent->stop();
  } else {
    _inl_action_center(ActionCenter::shared())->del(this);
  }
}

/**
 * @func playing
 */
void Action::playing(bool value) {
  if ( value ) {
    play();
  } else {
    stop();
  }
}

/**
 * @destructor
 */
GroupAction::~GroupAction() {
  _inl_group_action(this)->clear_all();
}

/**
 * @func operator[]
 */
Action* GroupAction::operator[](uint index) {
  if ( m_actions_index.length() != m_actions.length() ) {
    m_actions_index = Array<Iterator>(m_actions.length());
    uint j = 0;
    for ( auto& i : m_actions ) {
      m_actions_index[j] = i;
      j++;
    }
  }
  return m_actions_index[index].value();
}

/**
 * @func append
 */
void GroupAction::append(Action* action) av_def_err {
  av_assert(action);
  _inl_action(action)->set_parent(this);
  m_actions.push(action);
  m_actions_index.clear();

}

/**
 * @func insert
 */
void GroupAction::insert(uint index, Action* action) av_def_err {
  av_assert(action);
  if ( index == 0 ) {
    _inl_action(action)->set_parent(this);
    m_actions.unshift(action);
    m_actions_index.clear();
  } else if ( index < m_actions.length() ) {
    _inl_action(action)->set_parent(this);
    if ( m_actions_index.length() == m_actions.length() ) {
      m_actions.after(m_actions_index[index - 1], action);
    } else {
      m_actions.after(m_actions.find(index - 1), action);
    }
    m_actions_index.clear();
  } else {
    append(action);
  }
}

void SpawnAction::append(Action* action) av_def_err {
  GroupAction::append(action);
  uint64 du = action->duration() + m_delay;
  if ( du > m_duration ) {
    _inl_action(this)->update_duration( du - m_duration );
  }
}

void SpawnAction::insert(uint index, Action* action) av_def_err {
  GroupAction::insert(index, action);
  uint64 du = action->duration() + m_delay;
  if ( du > m_duration ) {
    _inl_action(this)->update_duration( du - m_duration );
  }
}

void SequenceAction::append(Action* action) av_def_err {
  GroupAction::append(action);
  if ( action->duration() ) {
    _inl_action(this)->update_duration( action->duration() );
  }
}

void SequenceAction::insert(uint index, Action* action) av_def_err {
  GroupAction::insert(index, action);
  if ( action->duration() ) {
    _inl_action(this)->update_duration( action->duration() );
  }
}

/**
 * @func remove
 */
void GroupAction::remove_child(uint index) {
  _inl_group_action(this)->m_remove(index);
}

void SpawnAction::remove_child(uint index) {
  uint64 duration = _inl_group_action(this)->m_remove(index) + m_delay;
  if ( duration == m_duration ) {
    
    uint64 new_duration = 0;
    for ( auto& i : m_actions ) {
      new_duration = av_max(i.value()->duration(), new_duration);
    }
    new_duration += m_delay;
    _inl_action(this)->update_duration( new_duration - duration );
  }
}

void SequenceAction::remove_child(uint index) {
  Iterator it =
    m_actions_index.length() == m_actions.length() ?
    m_actions_index[index] : m_actions.find(index);
  if ( it != m_actions.end() ) {
    if ( it == m_action ) {
      m_action = Iterator();
    }
    uint64 duration = it.value()->duration();
    _inl_action(it.value())->clear_parent();
    m_actions.del( it );
    m_actions_index.clear();
    if ( duration ) {
      _inl_action(this)->update_duration(-duration);
    }
  }
}

void GroupAction::clear() {
  for ( auto& i : m_actions ) {
    _inl_action(i.value())->clear_parent();
  }
  m_actions.clear();
  m_actions_index.clear();
  if ( m_duration ) {
    _inl_action(this)->update_duration( m_delay - m_duration );
  }
}

void SequenceAction::clear() {
  GroupAction::clear();
  m_action = Iterator();
}

/**
 * @func seek
 */
void Action::seek(uint64 time) {
  time = av_min(m_duration, time);
  if (m_parent) {
    m_parent->start_seek(time, this);
  } else {
    seek_time(time, this);
  }
}

/**
 * @func seek_play
 */
void Action::seek_play(uint64 time) {
  seek(time);
  play();
}

/**
 * @func seek_stop
 */
void Action::seek_stop(uint64 time) {
  seek(time);
  stop();
}

void SpawnAction::start_seek(uint64 time, Action* child) {
  time += m_delay;
  if (m_parent) {
    m_parent->start_seek(time, this);
  } else {
    seek_time(time, this);
  }
}

void SequenceAction::start_seek(uint64 time, Action* child) {
  time += m_delay;
  for ( auto& i : m_actions ) {
    if ( child == i.value() ) {
      break;
    } else {
      time += i.value()->duration();
    }
  }
  if (m_parent) {
    m_parent->start_seek(time, this);
  } else {
    seek_time(time, this);
  }
}

void KeyframeAction::start_seek(uint64 time, Action* child) {
  // noop
}

void SpawnAction::seek_time(uint64 time, Action* root) {
  
  int64 t = time - m_delay;
  if ( t < 0 ) {
    m_delayed = -t; return;
  } else {
    m_delayed = 0;
    time = t;
  }
  
  for ( auto& i : m_actions ) {
    i.value()->seek_time(time, root);
  }
}

void SequenceAction::seek_time(uint64 time, Action* root) {
  
  int64 t = time - m_delay;
  if ( t < 0 ) {
    m_delayed = -t;
    m_action = Iterator();
    return;
  } else {
    m_delayed = 0;
    time = t;
  }
  
  uint64 duration = 0;
  
  for ( auto& i : m_actions ) {
    uint64 du = duration + i.value()->duration();
    if ( du > time ) {
      m_action = i;
      i.value()->seek_time(time - duration, root);
      return;
    }
    duration = du;
  }
  
  if ( length() ) {
    m_action = (m_actions.end()--);
    m_action.value()->seek_time(time - duration, root);
  }
}

void KeyframeAction::seek_time(uint64 time, Action* root) {
  
  int64 t = time - m_delay;
  if ( t < 0 ) {
    m_delayed = -t;
    m_frame = -1;
    m_time = t; return;
  } else {
    m_delayed = 0;
    time = t;
  }
  
  if ( length() ) {
    Frame* frame = nullptr;
    
    for ( auto& i: m_frames ) {
      if ( time < i.value()->time() ) {
        break;
      }
      frame = i.value();
    }
    
    m_frame = frame->index();
    m_time = av_min(time, frame->time());
    
    uint f1 = m_frame;
    uint f2 = f1 + 1;
    
    if ( f2 < length() ) {
      uint64 time1 = frame->time();
      uint64 time2 = m_frames[f2]->time();
      float x = (m_time - time1) / float(time2 - time1);
      float t = frame->curve().solve(x, 0.001);
      _inl_key_action(this)->transition(f1, f2, x, t, root);
    } else { // last frame
      _inl_key_action(this)->transition(f1, root);
    }
    
    if ( m_time == frame->time() ) {
      _inl_action(this)->trigger_action_key_frame(0, m_frame, root);
    }
  }
}

uint64 SpawnAction::advance(uint64 time_span, bool restart, Action* root) {
  
  time_span *= m_speed; // Amplification time
  
  if ( m_delayed == -1 || restart ) { // restart
    restart = true;
    m_delayed = m_delay;
  }
  
  if ( m_delayed ) {
    int64 time = m_delayed - time_span;
    if ( time >= 0 ) {
      m_delayed = time;
      return 0;
    } else {
      m_delayed = 0;
      time_span = -time;
    }
  }
  
  uint64 surplus_time = time_span;
  
advance:
  
  for ( auto& i : m_actions ) {
    uint64 time = i.value()->advance(time_span, restart, root);
    surplus_time = av_min(surplus_time, time);
  }
  
  if ( surplus_time ) {
    if ( m_loop && m_duration - m_delay ) {
      restart = true;
      m_loop--;
      time_span = surplus_time;
      _inl_action(this)->trigger_action_loop(time_span, root);
      
      if ( _inl_action(root)->is_playing() ) {
        goto advance;
      }
      
      return 0; // end
    }
  }
  
  return surplus_time / m_speed;
}

uint64 SequenceAction::advance(uint64 time_span, bool restart, Action* root) {
  
  time_span *= m_speed; // Amplification time
  
  if ( m_action.is_null() || restart ) { // no start play
    
    if ( m_delayed == -1 || restart ) { // restart
      m_delayed = m_delay;
      m_action = Iterator();
    }
    
    if ( m_delayed ) {
      int64 time = m_delayed - time_span;
      if ( time >= 0 ) {
        m_delayed = time;
        return 0;
      } else {
        m_delayed = 0;
        time_span = -time;
      }
    }
    
    if ( length() ) {
      restart = true;
      m_action = m_actions.begin();
    } else {
      return time_span / m_speed;
    }
  }
  
advance:
  
  time_span = m_action.value()->advance(time_span, restart, root);
  
  if ( time_span ) {
    
    if ( m_action.is_null() ) { // May have been deleted
      if ( length() ) { // Restart
        restart = true;
        m_action = m_actions.begin();
        goto advance;
      }
    } else {
      if ( m_action.value() == m_actions.last() ) { // last action
        if ( m_loop && m_duration - m_delay ) {
          restart = true;
          m_loop--;
          m_action = m_actions.begin();
          _inl_action(this)->trigger_action_loop(time_span, root); // trigger event
          
          if ( _inl_action(root)->is_playing() ) {
            goto advance;
          }
          return 0; // end
        }
      } else {
        m_action++;
        goto advance;
      }
    }
  }
  
  return time_span / m_speed;
}

KeyframeAction::~KeyframeAction() {
  clear();
}

uint64 KeyframeAction::advance(uint64 time_span, bool restart, Action* root) {
  
  time_span *= m_speed;
  
  if ( m_frame == -1 || restart ) { // no start play
    
    if ( m_delayed == -1 || restart ) { // restart
      m_delayed = m_delay;
      m_frame = -1;
      m_time = -m_delayed;
    }
    
    if ( m_delayed ) { // delay
      int64 time = m_delayed - time_span;
      if ( time >= 0 ) {
        m_delayed = time;
        m_time = -m_delayed;
        return 0;
      } else {
        m_delayed = 0;
        time_span = -time;
      }
    }
    
    if ( length() ) {
      m_frame = 0;
      m_time = 0;
      _inl_key_action(this)->transition(0, root);
      _inl_action(this)->trigger_action_key_frame(time_span, 0, root);
      
      if ( time_span == 0 ) {
        return 0;
      }
      
      if ( length() == 1 ) {
        return time_span / m_speed;
      }
    } else {
      return time_span / m_speed;
    }
  }
  
  return _inl_key_action(this)->advance(time_span, root) / m_speed;
}

void GroupAction::bind_view(View* view) {
  for ( auto& i : m_actions ) {
    i.value()->bind_view(view);
  }
}

void KeyframeAction::bind_view(View* view) {
  int view_type = view->view_type();
  if ( view_type != m_bind_view_type ) {
    m_bind_view_type = view_type;
    for ( auto& i : m_property ) {
      i.value()->bind_view(view_type);
    }
  }
}

/**
 * @func add new frame
 */
Frame* KeyframeAction::add(uint64 time, const FixedCubicBezier& curve) {
  
  if ( length() ) {
    Frame* frame = last();
    int64 duration = time - frame->time();
    if ( duration <= 0 ) {
      time = frame->time();
    } else {
      _inl_action(this)->update_duration(duration);
    }
  } else {
    time = 0;
  }
  
  Frame* frame = new Frame(this, m_frames.length(), curve);
  m_frames.push(frame);
  frame->m_time = time;
  
  for ( auto& i : m_property ) {
    i.value()->add_frame();
  }
  
  return frame;
}

/**
 * @func clear all frame and property
 */
void KeyframeAction::clear() {
  
  for (auto& i : m_frames) {
    i.value()->m_host = nullptr;
    Release(i.value());
  }
  for (auto& i : m_property) {
    delete i.value();
  }
  m_frames.clear();
  m_property.clear();
  
  if ( m_duration ) {
    _inl_action(this)->update_duration( m_delay - m_duration );
  }
}

bool KeyframeAction::has_property(PropertyName name) {
  return m_property.has(name);
}

/**
 * @func match_property
 */
bool KeyframeAction::match_property(PropertyName name) {
  return PropertysAccessor::shared()->has_accessor(m_bind_view_type, name);
}
// ----------------------- ActionCenter -----------------------

static ActionCenter* action_center_shared = nullptr;

ActionCenter::ActionCenter()
: m_prev_sys_time(0) {
  av_assert(!action_center_shared); action_center_shared = this;
}

ActionCenter::~ActionCenter() {
  action_center_shared = nullptr;
}

/**
 * @func advance
 */
void ActionCenter::advance() {
  
  uint64 sys_time = sys::time_monotonic();
  
  if ( m_actions.length() ) { // run task
    
    uint64 time_span = 0;
    
    if (m_prev_sys_time) {  // 0表示还没开始
      time_span = sys_time - m_prev_sys_time;
      if ( time_span > 300000 ) {   // 距离上一帧超过300ms重新记时(如应用程序从休眠中恢复)
        time_span = 0;
      }
    }
    
    for ( auto i = m_actions.begin(); !i.is_null(); ) {
      Action::Wrap& wrap = i.value();
      if ( wrap.value ) {
        if ( wrap.begin ) {
          if ( wrap.value->advance(time_span, false, wrap.value) ) {
            // 不能消耗所有时间表示动作已经结束
            // end action play
            _inl_action_center(this)->del(wrap.value);
          }
        } else {
          wrap.begin = true;
          wrap.value->advance(0, false, wrap.value);
        }
        i++;
      } else {
        m_actions.del(i++);
      }
    }
  }
  
  m_prev_sys_time = sys_time;
}

ActionCenter* ActionCenter::shared() {
  return action_center_shared;
}

av_gui_end
