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

typedef KeyframeAction::Frame Frame;
typedef KeyframeAction::Property Property;

/**
 * @class Property2
 */
template<class T> class Property2: public Property {
public:
  typedef T    (View::*GetPropertyFunc)() const;
  typedef void (View::*SetPropertyFunc)(T value);
  
  inline Property2(uint frame_count)
  : m_frames(frame_count)
  , m_get_property_func(nullptr)
  , m_set_property_func(nullptr) { }
  
  inline void set_property(List<View*>& views) {
    for ( auto& i : views ) {
      if ( i.value() ) {
        (i.value()->*m_set_property_func)(m_transition);
      }
    }
  }
  
  inline T get_property(View* view) {
    return (view->*m_get_property_func)();
  }
  
  virtual void transition(uint f1, Action* root) {
    if ( m_set_property_func ) {
      m_transition = m_frames[f1];
      set_property(_inl_action(root)->views());
    }
  }
  
  virtual void transition(uint f1, uint f2, float x, float y, Action* root) {
    if ( m_set_property_func ) {
      T v1 = m_frames[f1], v2 = m_frames[f2];
      m_transition = v1 - (v1 - v2) * y;
      set_property(_inl_action(root)->views());
    }
  }
  
  virtual void add_frame() {
    m_frames.push( T() );
  }
  
  virtual void fetch(uint frame, View* view) {
    if ( m_get_property_func ) {
      m_frames[frame] = get_property(view);
    }
  }
  
  virtual void default_value(uint frame) {
    m_frames[frame] = T();
  }
  
  inline T operator[](uint index) {
    return m_frames[index];
  }
  
  inline T frame(uint index) {
    return m_frames[index];
  }
  
  inline void frame(uint index, T value) {
    m_frames[index] = value;
  } 
  
protected:
  
  Array<T>        m_frames;
  T               m_transition;
  GetPropertyFunc m_get_property_func;
  SetPropertyFunc m_set_property_func;
};

// set_property
template<> inline void Property2<ColorValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(ColorValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextSizeValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(TextSizeValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextStyleValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(TextStyleValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextFamilyValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(TextFamilyValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextLineHeightValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(TextLineHeightValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextShadowValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(TextShadowValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextDecorationValue>::set_property(List<View*>& views) {
  typedef void (TextFont::*Func)(TextDecorationValue);
  for ( auto& i : views ) {
    (i.value()->as_text_font()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextOverflowValue>::set_property(List<View*>& views) {
  typedef void (TextLayout::*Func)(TextOverflowValue);
  for ( auto& i : views ) {
    (i.value()->as_text_layout()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}
template<> inline void Property2<TextWhiteSpaceValue>::set_property(List<View*>& views) {
  typedef void (TextLayout::*Func)(TextWhiteSpaceValue);
  for ( auto& i : views ) {
    (i.value()->as_text_layout()->*reinterpret_cast<Func>(m_set_property_func))(m_transition);
  }
}

// get_property
template<> inline ColorValue Property2<ColorValue>::get_property(View* view) {
  typedef ColorValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextSizeValue Property2<TextSizeValue>::get_property(View* view) {
  typedef TextSizeValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextStyleValue Property2<TextStyleValue>::get_property(View* view) {
  typedef TextStyleValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextFamilyValue Property2<TextFamilyValue>::get_property(View* view) {
  typedef TextFamilyValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextLineHeightValue Property2<TextLineHeightValue>::get_property(View* view) {
  typedef TextLineHeightValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextShadowValue Property2<TextShadowValue>::get_property(View* view) {
  typedef TextShadowValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextDecorationValue Property2<TextDecorationValue>::get_property(View* view) {
  typedef TextDecorationValue (TextFont::*Func)() const;
  return (view->as_text_font()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextOverflowValue Property2<TextOverflowValue>::get_property(View* view) {
  typedef TextOverflowValue (TextLayout::*Func)() const;
  return (view->as_text_layout()->*reinterpret_cast<Func>(m_get_property_func))();
}
template<> inline TextWhiteSpaceValue Property2<TextWhiteSpaceValue>::get_property(View* view) {
  typedef TextWhiteSpaceValue (TextLayout::*Func)() const;
  return (view->as_text_layout()->*reinterpret_cast<Func>(m_get_property_func))();
}

// transition

template<> void Property2<bool>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = t < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<Vec2>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    Vec2 v1 = m_frames[f1], v2 = m_frames[f2];
    m_transition = Vec2(v1.x() - (v1.x() - v2.x()) * t, v1.y() - (v1.y() - v2.y()) * t);
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<Color>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    Color v1 = m_frames[f1], v2 = m_frames[f2];
    m_transition = Color(v1.r() - (v1.r() - v2.r()) * t,
                         v1.g() - (v1.g() - v2.g()) * t,
                         v1.b() - (v1.b() - v2.b()) * t, v1.a() - (v1.a() - v2.a()) * t);
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextAlign>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = t < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<Align>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = t < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<ContentAlign>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = t < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<Repeat>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = t < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<Border>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    Border v1 = m_frames[f1], v2 = m_frames[f2];
    float width = v1.width - (v1.width - v2.width) * t;
    Color color(v1.color.r() - (v1.color.r() - v2.color.r()) * t,
                v1.color.g() - (v1.color.g() - v2.color.g()) * t,
                v1.color.b() - (v1.color.b() - v2.color.b()) * t,
                v1.color.a() - (v1.color.a() - v2.color.a()) * t);
    m_transition = Border(width, color);
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<ShadowValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    ShadowValue v1 = m_frames[f1], v2 = m_frames[f2];
    float offset_x = v1.offset_x - (v1.offset_x - v2.offset_x) * t;
    float offset_y = v1.offset_y - (v1.offset_y - v2.offset_y) * t;
    float size = v1.size - (v1.size - v2.size) * t;
    Color color(v1.color.r() - (v1.color.r() - v2.color.r()) * t,
                v1.color.g() - (v1.color.g() - v2.color.g()) * t,
                v1.color.b() - (v1.color.b() - v2.color.b()) * t,
                v1.color.a() - (v1.color.a() - v2.color.a()) * t);
    m_transition = { offset_x, offset_y, size, color };
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<Value>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    Value v1 = m_frames[f1], v2 = m_frames[f2];
    if ( v1.type == v2.type ) {
      m_transition = Value(v1.type, v1.value - (v1.value - v2.value) * t);
    } else {
      m_transition = x < 1.0 ? v1 : v2;
    }
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<ColorValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    ColorValue v1 = m_frames[f1], v2 = m_frames[f2];
    if ( v1.type == TextArrtsType::VALUE && v2.type == TextArrtsType::VALUE ) {
      Color color(v1.value.r() - (v1.value.r() - v2.value.r()) * t,
                  v1.value.g() - (v1.value.g() - v2.value.g()) * t,
                  v1.value.b() - (v1.value.b() - v2.value.b()) * t,
                  v1.value.a() - (v1.value.a() - v2.value.a()) * t);
      m_transition = { TextArrtsType::VALUE, color };
    } else {
      m_transition = x < 1.0 ? v1 : v2;
    }
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextSizeValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    TextSizeValue v1 = m_frames[f1], v2 = m_frames[f2];
    if ( v1.type == TextArrtsType::VALUE && v2.type == TextArrtsType::VALUE ) {
      m_transition = { TextArrtsType::VALUE, v1.value - (v1.value - v2.value) * t };
    } else {
      m_transition = x < 1.0 ? v1 : v2;
    }
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextStyleValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = x < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextFamilyValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = x < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextLineHeightValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    TextLineHeightValue v1 = m_frames[f1], v2 = m_frames[f2];
    if (v1.type == TextArrtsType::VALUE &&
        v2.type == TextArrtsType::VALUE && !v1.value.is_auto && !v2.value.is_auto) {
      m_transition = {
        TextArrtsType::VALUE,
        false,
        v1.value.height - (v1.value.height - v2.value.height) * t
      };
    } else {
      m_transition = x < 1.0 ? v1 : v2;
    }
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextShadowValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    TextShadowValue v1 = m_frames[f1], v2 = m_frames[f2];
    if ( v1.type == TextArrtsType::VALUE && v2.type == TextArrtsType::VALUE ) {
      float offset_x = v1.value.offset_x - (v1.value.offset_x - v2.value.offset_x) * t;
      float offset_y = v1.value.offset_y - (v1.value.offset_y - v2.value.offset_y) * t;
      float size = v1.value.size - (v1.value.size - v2.value.size) * t;
      Color color(v1.value.color.r() - (v1.value.color.r() - v2.value.color.r()) * t,
                  v1.value.color.g() - (v1.value.color.g() - v2.value.color.g()) * t,
                  v1.value.color.b() - (v1.value.color.b() - v2.value.color.b()) * t,
                  v1.value.color.a() - (v1.value.color.a() - v2.value.color.a()) * t);
      m_transition = { v1.type, offset_x, offset_y, size, color };
    } else {
      m_transition = x < 1.0 ? v1 : v2;
    }
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextDecorationValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = x < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextOverflowValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = x < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<TextWhiteSpaceValue>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = x < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}
template<> void Property2<String>::transition(uint f1, uint f2, float x, float t, Action* root) {
  if ( m_set_property_func ) {
    m_transition = x < 1.0 ? m_frames[f1] : m_frames[f2];
    set_property(_inl_action(root)->views());
  }
}

//-------------------------------------------------------------------------------

/**
 * @class Property3
 */
template<class T, PropertyName Name> class Property3: public Property2<T> {
public:
  
  typedef typename Property2<T>::GetPropertyFunc GetPropertyFunc;
  typedef typename Property2<T>::SetPropertyFunc SetPropertyFunc;
  
  inline Property3(uint frame_count): Property2<T>(frame_count) { }
  
  virtual void bind_view(int view_type) {
    typedef PropertysAccessor::Accessor PropertyFunc;
    PropertyFunc func = PropertysAccessor::shared()->accessor(view_type, Name);
    this->m_get_property_func = reinterpret_cast<GetPropertyFunc>(func.get_accessor);
    this->m_set_property_func = reinterpret_cast<SetPropertyFunc>(func.set_accessor);
  }
};

template<> void Property3<float, PROPERTY_X>::bind_view(int type) {
  this->m_get_property_func = &View::x;
  this->m_set_property_func = &View::set_x;
}
template<> void Property3<float, PROPERTY_Y>::bind_view(int type) {
  this->m_get_property_func = &View::y;
  this->m_set_property_func = &View::set_y;
}
template<> void Property3<float, PROPERTY_SCALE_X>::bind_view(int type) {
  this->m_get_property_func = &View::scale_x;
  this->m_set_property_func = &View::set_scale_x;
}
template<> void Property3<float, PROPERTY_SCALE_Y>::bind_view(int type) {
  this->m_get_property_func = &View::scale_y;
  this->m_set_property_func = &View::set_scale_y;
}
template<> void Property3<float, PROPERTY_SKEW_X>::bind_view(int type) {
  this->m_get_property_func = &View::skew_x;
  this->m_set_property_func = &View::set_skew_x;
}
template<> void Property3<float, PROPERTY_SKEW_Y>::bind_view(int type) {
  this->m_get_property_func = &View::skew_y;
  this->m_set_property_func = &View::set_skew_y;
}
template<> void Property3<float, PROPERTY_ORIGIN_X>::bind_view(int type) {
  this->m_get_property_func = &View::origin_x;
  this->m_set_property_func = &View::set_origin_x;
}
template<> void Property3<float, PROPERTY_ORIGIN_Y>::bind_view(int type) {
  this->m_get_property_func = &View::origin_y;
  this->m_set_property_func = &View::set_origin_y;
}
template<> void Property3<float, PROPERTY_ROTATE_Z>::bind_view(int type) {
  this->m_get_property_func = &View::rotate_z;
  this->m_set_property_func = &View::set_rotate_z;
}
template<> void Property3<float, PROPERTY_OPACITY>::bind_view(int type) {
  this->m_get_property_func = &View::opacity;
  this->m_set_property_func = &View::set_opacity;
}
template<> void Property3<bool, PROPERTY_VISIBLE>::bind_view(int type) {
  this->m_get_property_func = &View::visible;
  this->m_set_property_func = &View::set_visible0;
}
