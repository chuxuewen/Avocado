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

#ifndef __avocado__gui__display_port__
#define __avocado__gui__display_port__

#include "autil/util.h"
#include "autil/event.h"
#include "autil/list.h"
#include "mathe.h"
#include "value.h"
#include "autil/cb.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

class Draw;
class Root;
class GUIApplication;
class PreRender;

/**
 * 显示端口
 * 如果当前的显示端口为窗口,那么显示的范围就是整个窗口的所有区域
 * 如果当前的显示端口为全屏,那么它也是全屏的
 * @class DisplayPort
 */
class DisplayPort: public Reference {
  av_hidden_all_copy(DisplayPort);
public:
  
  DisplayPort(GUIApplication* app);
  
  /**
   * @destructor
   */
  virtual ~DisplayPort();
  
  /**
   * @event onchange 显示端口变化事件
   */
  av_event(onchange);
  
  /**
   * @event onrender 绘制每一帧后触发该事件
   */
  av_event(onrender);
  
  /**
   * @func phy_size 视口在屏幕上所占的实际物理像素的尺寸
   */
  inline Vec2 phy_size() const { return m_phy_size; }
  
  /**
   * @func size 当前视口尺寸
   */
  inline Vec2 size() const { return m_size; }
  
  /**
   * @func best_scale 最好的视口缩放
   */
  float best_scale() const;
  
  /**
   * @func scale 当前视口缩放
   */
  inline float scale() const { return m_scale; }
  
  /**
   * @func scale_width
   */
  inline float scale_width() const { return m_scale_width; }
  
  /**
   * @func scale_height
   */
  inline float scale_height() const { return m_scale_height; }
  
  /**
   * @func set_lock_size()
   *
   * width与height都设置为0时自动设置一个最舒适的默认显示尺寸
   *
   * 设置锁定视口为一个固定的逻辑尺寸,这个值改变时会触发change事件
   *
   * 如果width设置为零表示不锁定宽度,系统会自动根据height值设置一个同等比例的宽度
   * 如果设置为非零表示锁定宽度,不管display_port_size怎么变化对于编程者来说,这个值永远保持不变
   *
   * 如果height设置为零表示不锁定,系统会自动根据width值设置一个同等比例的高度
   * 如果设置为非零表示锁定高度,不管display_port_size怎么变化对于编程者来说,这个值永远保持不变
   *
   */
  void lock_size(float width = 0, float height = 0);
  
  /**
   * @func root_matrix
   */
  inline const Mat4& root_matrix() const { return m_root_matrix; }
  
  /**
   * @func draw_region
   */
  inline const CGRegion& draw_region() const { return m_draw_region.last(); }

  /**
   * @func push_draw_region
   */
  void push_draw_region(CGRegion value);
  
  /**
   * @func pop_draw_region
   */
  inline void pop_draw_region() {
    av_assert( m_draw_region.length() > 1 );
    m_draw_region.pop();
  }
  
  /**
   * @func atom_px
   */
  inline float atom_px() const { return m_atom_px; }
  
  /**
   * @func next_frame
   */
  void next_frame(Callback cb);
  
  /**
   * @func default_atom_px
   */
  static float default_atom_px();
  
private:
  
  /**
   * @func render 渲染这个端口
   */
  void render();
  
  Vec2              m_phy_size;   // 视口在屏幕上所占的实际像素的尺寸
  Vec2              m_lock_size;  // 锁定视口的尺寸
  Vec2              m_size;       // 当前视口尺寸
  float             m_scale;      // 当前屏幕显示缩放比,这个值越大size越小显示的内容也越少
  float             m_scale_width;
  float             m_scale_height;
  PreRender*        m_pre_render;
  Draw*             m_draw_ctx;
  Mat4              m_root_matrix;
  bool              m_mark_update_display_port; // 标记更新
  float             m_atom_px;
  List<CGRegion>    m_draw_region;
  GUIApplication*   m_app;
  
  av_def_inl_cls(Inl);
  friend class  GUIApplication; // 友元类
};

av_gui_end
#endif

