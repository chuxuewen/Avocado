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

#ifndef __avocado__gui__action__
#define __avocado__gui__action__

#include "value.h"
#include "autil/array.h"
#include "autil/list.h"
#include "autil/map.h"
#include "bezier.h"
#include "event.h"
#include "property.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

class View;
class ActionCenter;
class GroupAction;
class SpawnAction;
class SequenceAction;
class KeyframeAction;

/**
 * @class Action
 */
class Action: public Reference {
public:

  Action();
  
  /**
   * @destructor
   */
  virtual ~Action();
  
  /**
   * @func play
   */
  void play();
  
  /**
   * @func stop
   */
  void stop();
  
  /**
   * @func seek
   */
  void seek(uint64 time);
  
  /**
   * @func seek_play
   */
  void seek_play(uint64 time);
  
  /**
   * @func seek_play
   */
  void seek_stop(uint64 time);
  
  /**
   * @func loop get
   */
  inline uint loop() const { return m_loop; }
  
  /**
   * @func delay get
   */
  inline uint64 delay() const { return m_delay; }
  
  /**
   * @func delayed get
   */
  int64 delayed() const { return m_delayed; }
  
  /**
   * @func speed get
   */
  inline float speed() const { return m_speed; }
  
  /**
   * @func playing
   */
  bool playing() const;
  
  /**
   * @func playing
   */
  void playing(bool value);
  
  /**
   * @func parent get
   */
  inline Action* parent() { return m_parent; }
  
  /**
   * @func loop set
   */
  inline void loop(uint value) { m_loop = value; }
  
  /**
   * @func delay set
   */
  void delay(uint64 value);
  
  /**
   * @func speed set
   */
  inline void speed(float value) {
    m_speed = av_min(10, av_max(value, 0.1));
  }
  
  /**
   * @func duration
   */
  inline uint64 duration() { return m_duration; }
  
  /**
   * @func clear
   */
  virtual void clear() = 0;
  
  /**
   * @func as_keyframe
   */
  virtual GroupAction* as_group() { return nullptr; }
  
  /**
   * @func as_spawn
   */
  virtual SpawnAction* as_spawn() { return nullptr; }
  
  /**
   * @func as_sequence
   */
  virtual SequenceAction* as_sequence() { return nullptr; }
  
  /**
   * @func as_keyframe
   */
  virtual KeyframeAction* as_keyframe() { return nullptr; }
  
private:
  
  /**
   * @func advance
   */
  virtual uint64 advance(uint64 time_span, bool restart, Action* root) = 0;
  
  /**
   * @func seek_time
   */
  virtual void seek_time(uint64 time, Action* root) = 0;
  
  /**
   * @func start_seek to root action
   */
  virtual void start_seek(uint64 time, Action* child) = 0;
  
  /**
   * @func bind_view
   */
  virtual void bind_view(View* view) = 0;
  
protected:
  
  struct Wrap {
    Action* value;
    bool    begin;
  };
  
  Action* m_parent;
  uint    m_loop;
  uint64  m_duration;
  uint64  m_delay;
  int64   m_delayed;
  float   m_speed;
  bool    m_begin_advance;
  List<View*> m_views;
  List<Wrap>::Iterator m_action_center_id;
  
  av_def_inl_cls(Inl);
  
  friend class ActionCenter;
  friend class GroupAction;
  friend class SpawnAction;
  friend class SequenceAction;
  friend class KeyframeAction;
};

/**
 * @class GroupAction
 */
class GroupAction: public Action {
public:
  
  /**
   * @func operator[]
   */
  Action* operator[](uint index);
  
  /**
   * @func length
   */
  inline uint length() const { return m_actions.length(); }
  
  /**
   * @func append
   */
  virtual void append(Action* action) av_def_err;
  
  /**
   * @func insert
   */
  virtual void insert(uint index, Action* action) av_def_err;
  
  /**
   * @func remove_child
   */
  virtual void remove_child(uint index);
  
  /**
   * @overwrite
   */
  virtual void clear();
  virtual GroupAction* as_group() { return this; }
  
protected:
  
  virtual ~GroupAction();
  
  /**
   * @overwrite
   */
  virtual void bind_view(View* view);
  
  typedef List<Action*>::Iterator Iterator;
  List<Action*>   m_actions;
  Array<Iterator> m_actions_index;
  
  av_def_inl_cls(Inl);
};

/**
 * @class SpawnAction
 */
class SpawnAction: public GroupAction {
public:
  
  /**
   * @func spawn
   */
  inline Action* spawn(uint index) { return (*this)[index]; }
  
  /**
   * @overwrite
   */
  virtual SpawnAction* as_spawn() { return this; }
  virtual void append(Action* action) av_def_err;
  virtual void insert(uint index, Action* action) av_def_err;
  virtual void remove_child(uint index);

private:
  
  /**
   * @overwrite
   */
  virtual uint64 advance(uint64 time_span, bool restart, Action* root);
  virtual void seek_time(uint64 time, Action* root);
  virtual void start_seek(uint64 time, Action* child);
  
};

/**
 * @class SequenceAction
 */
class SequenceAction: public GroupAction {
public:
  
  /**
   * @func seq
   */
  inline Action* seq(uint index) { return (*this)[index]; }
  
  /**
   * @overwrite
   */
  virtual SequenceAction* as_sequence() { return this; }
  virtual void append(Action* action) av_def_err;
  virtual void insert(uint index, Action* action) av_def_err;
  virtual void remove_child(uint index);
  virtual void clear();
  
private:
  
  /**
   * @overwrite
   */
  virtual uint64 advance(uint64 time_span, bool restart, Action* root);
  virtual void seek_time(uint64 time, Action* root);
  virtual void start_seek(uint64 time, Action* child);
  
  Iterator m_action;
  
  friend class GroupAction::Inl;
  
};

/**
 * @class KeyframeAction
 */
class KeyframeAction: public Action {
public:
  
  class Property {
  public:
    virtual ~Property() { }
    virtual void bind_view(int view_type) = 0;
    virtual void transition(uint frame1, Action* root) = 0;
    virtual void transition(uint frame1, uint frame2,
                            float x, float t, Action* root) = 0;
    virtual void add_frame() = 0;
    virtual void fetch(uint frame, View* view) = 0;
    virtual void default_value(uint frame) = 0;
  };
  
  class Frame: public Object {
    av_hidden_all_copy(Frame);
  public:
    
    inline Frame(KeyframeAction* host, uint index, const FixedCubicBezier& curve)
    : m_host(host)
    , m_index(index)
    , m_curve(curve), m_time(0) {
      
    }
    
    /**
     * @func index
     */
    inline uint index() const { return m_index; }
    
    /**
     * @func time get
     */
    inline uint64 time() const { return m_time; }
    
    /**
     * @func time set
     */
    void set_time(uint64 value);
    
    /*
     * @func host
     */
    inline KeyframeAction* host() { return m_host; }
    
    /**
     * @func curve get
     */
    inline FixedCubicBezier& curve() { return m_curve; }
    
    /**
     * @func curve get
     */
    inline const FixedCubicBezier& curve() const { return m_curve; }
    
    /**
     * @func curve set
     */
    inline void set_curve(const FixedCubicBezier& value) { m_curve = value; }
    
    /**
     * @func fetch property value
     */
    void fetch(View* view = nullptr);
    
    /**
     * @func flush recovery default property value
     */
    void flush();
    
#define av_def_property(ENUM, TYPE, NAME) void set_##NAME(TYPE value); TYPE NAME();
    av_each_property_table(av_def_property)
#undef av_def_property
    
    Vec2  translate() { return Vec2(x(), y()); }
    Vec2  scale() { return Vec2(scale_x(), scale_y()); }
    Vec2  skew() { return Vec2(skew_x(), skew_y()); }
    Vec2  origin() { return Vec2(origin_x(), origin_y()); }
    Value min_width() { return width(); }
    Value min_height() { return height(); }
    Vec2  start() { return Vec2(start_x(), start_y()); }
    Vec2  ratio() { return Vec2(ratio_x(), ratio_y()); }
    
    void set_translate(Vec2 value) { set_x(value.x()); set_y(value.y()); }
    void set_scale(Vec2 value) { set_scale_x(value.x()); set_scale_y(value.y()); }
    void set_skew(Vec2 value) { set_skew_x(value.x()); set_skew_y(value.y()); }
    void set_origin(Vec2 value) { set_origin_x(value.x()); set_origin_y(value.y()); }
    void set_start(Vec2 value) { set_start_x(value.x()); set_start_y(value.y()); }
    void set_ratio(Vec2 value) { set_ratio_x(value.x()); set_ratio_y(value.y()); }
    void set_align(Align value) { set_align_x(value); set_align_y(value); }
    void set_margin(Value value);
    void set_border(Border value);
    void set_border_width(float value);
    void set_border_color(Color value);
    void set_border_radius(float value);
    void set_min_width(Value value) { set_width(value); }
    void set_min_height(Value value) { set_height(value); }
    
  private:
    
    KeyframeAction*   m_host;
    uint              m_index;
    FixedCubicBezier  m_curve;
    uint64            m_time;
    
    av_def_inl_cls(Inl);
    friend class KeyframeAction;
  };
  
  /**
   * @constructor
   */
  inline KeyframeAction(): m_frame(-1), m_time(0), m_bind_view_type(0) { }
  
  /**
   * @destructor
   */
  virtual ~KeyframeAction();
  
  /**
   * @overwrite
   */
  virtual KeyframeAction* as_keyframe() { return this; }
  
  /**
   * @func has_property
   */
  bool has_property(PropertyName name);
  
  /**
   * @func match_property
   */
  bool match_property(PropertyName name);
  
  /**
   * @func first
   */
  inline Frame* first() { return m_frames[0]; }
  
  /**
   * @func last
   */
  inline Frame* last() { return m_frames[m_frames.length() - 1]; }
  
  /**
   * @func frame
   */
  inline Frame* frame(uint index) { return m_frames[index]; }
  
  /**
   * @func operator[]
   */
  inline Frame* operator[](uint index) { return m_frames[index]; }
  
  /**
   * @func length
   */
  inline uint length() const { return m_frames.length(); }
  
  /**
   * @func position get play frame position
   */
  inline int position() const { return m_frame; }
  
  /**
   * @func time get play time position
   */
  inline int64 time() const { return m_time; }
  
  /**
   * @func add new frame
   */
  Frame* add(uint64 time, const FixedCubicBezier& curve = EASE);
  
  /**
   * @func clear all frame and property
   */
  virtual void clear();
  
  /**
   * @func is_bind_view
   */
  inline bool is_bind_view() { return m_bind_view_type; }
  
private:
  
  /**
   * @overwrite
   */
  virtual uint64 advance(uint64 time_span, bool restart, Action* root);
  virtual void seek_time(uint64 time, Action* root);
  virtual void start_seek(uint64 time, Action* child);
  virtual void bind_view(View* view);
  
  typedef Map<PropertyName, Property*> Propertys;
  
  int           m_frame;
  int64         m_time;
  Array<Frame*> m_frames;
  int           m_bind_view_type;
  Propertys     m_property;
  
  av_def_inl_cls(Inl);
};

/**
 * @class ActionCenter
 */
class ActionCenter: public Object {
public:
  
  ActionCenter();
  
  /**
   * @destructor
   */
  virtual ~ActionCenter();
  
  /**
   * @func advance
   */
  void advance();
  
  /**
   * @func shared
   */
  static ActionCenter* shared();
  
private:
  
  typedef List<Action::Wrap> Actions;
  
  uint64  m_prev_sys_time;
  Actions m_actions;
  
  av_def_inl_cls(Inl);
};

av_gui_end
#endif
