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

#include "display-port.h"
#include "app.h"
#include "draw.h"
#include "pre-render.h"
#include "root.h"
#include "action.h"

av_gui_begin

class DisplayPort::Inl: public DisplayPort {
 public:
#define _inl(self) static_cast<DisplayPort::Inl*>(self)
  
  /**
   * @func m_gl_onbackingchange_handle # gl上下文事件处理器
   * @arg evt {Event<>&}
   */
  void m_gl_onbackingchange_handle(Event<>& evt) {
    Draw* ctx = static_cast<Draw*>(evt.sender());
    m_phy_size = { float(ctx->screen_width()), float(ctx->screen_height()) };
    m_update_display_port();
  }
  
  /**
   * 更新显示端口的尺寸信息
   */
  void m_update_display_port() {
    
    if (m_lock_size.width() == 0 && m_lock_size.height() == 0) { // 使用系统默认的最合适的尺寸
      m_size = { m_phy_size.width() / m_draw_ctx->best_display_scale(),
                 m_phy_size.height() / m_draw_ctx->best_display_scale() };
    }
    else if (m_lock_size.width() != 0 && m_lock_size.height() != 0) { // 尺寸全部锁定
      m_size = m_lock_size;
    }
    else if (m_lock_size.width() != 0) { // 只锁定宽度
      m_size.width(m_lock_size.width());
      m_size.height(m_size.width() / m_phy_size.width() * m_phy_size.height());
    }
    else { // m_lock_height == 0 // 只锁定高度
      m_size.height(m_lock_size.height());
      m_size.width(m_size.height() / m_phy_size.height() * m_phy_size.width());
    }
    
    m_scale_width = m_phy_size.width() / m_size.width();
    m_scale_height = m_phy_size.height() / m_size.height();
    
    m_scale = (m_scale_width + m_scale_height) / 2;
    
    m_atom_px = 1.0 / m_scale;
    
    // 计算2D视图变换矩阵
    m_root_matrix = Mat4::ortho(0, m_size.width(), 0, m_size.height(), -1.0, 1.0);
    
    Mat4 test_root_matrix = // 测试着色器视图矩阵要大一圈
    Mat4::ortho(-5, m_size.width() + 5, -5, m_size.height() + 5, -1.0, 1.0);
    
    m_draw_ctx->refresh_status_for_root_matrix(m_root_matrix, test_root_matrix);
    
    // set default draw region
    m_draw_region.first() = {
      0, 0,
      m_size.width(), m_size.height(),
      m_size.width(), m_size.height(),
    };
    
    // 标记
    m_mark_update_display_port = true;
  }
};

/**
 * @constructor
 */
DisplayPort::DisplayPort(GUIApplication* app)
: av_init_event(change)
//, av_init_event(beforerender)
, av_init_event(render)
, m_phy_size()
, m_lock_size()
, m_size()
, m_scale(1)
, m_scale_width(1)
, m_scale_height(1)
, m_pre_render(new PreRender())
, m_draw_ctx(app->draw_ctx())
, m_root_matrix()
, m_mark_update_display_port(false)
, m_atom_px(1)
, m_app(app) {
  
  m_draw_region.push({ 0,0,0,0,0,0 });
  
  // 侦听视口尺寸变化
  m_draw_ctx->av_on(screen_size_change, &DisplayPort::Inl::m_gl_onbackingchange_handle, _inl(this));
}

/**
 * @destructor
 */
DisplayPort::~DisplayPort() {
  Release(m_pre_render);
  m_draw_ctx->av_off(screen_size_change, &DisplayPort::Inl::m_gl_onbackingchange_handle, _inl(this));
}

float DisplayPort::best_scale() const {
  return m_draw_ctx->best_display_scale();
}

void DisplayPort::lock_size(float width, float height) {
  if (width >= 0.0 && height >= 0.0) {
    if (m_lock_size.width() != width || m_lock_size.height() != height) {
      m_lock_size = { width, height };
      _inl(this)->m_update_display_port();
    }
  } else {
    WARN("Lock size value can not be less than zero\n");
  }
}

/**
 * @func render # 渲染这个端口
 */
void DisplayPort::render() {
  
  // av_trigger(beforerender); // 通知事件
  
  Root* r = root();
  // 在这里发射事件,可以避onchange事件的触发与set_lock_size函数调造成的死循环
  if ( m_mark_update_display_port ) {
    m_mark_update_display_port = false;
    // TODO 设置root旋转动画 ?
    if ( r) {
      r->set_width(m_size.width());
      r->set_height(m_size.height());
    }
    av_trigger(change); // 通知事件
  }
  
  m_app->action_center()->advance(); // advance action
  
  if ( r ) {
    if ( m_pre_render->solve() || r->mark_value || r->m_child_change_flag ) {
      m_draw_ctx->begin_render();
      r->draw(); // 开始绘图
      m_draw_ctx->commit_render();
    }
  }
  
  av_trigger(render); // 通知事件
  
}

void DisplayPort::push_draw_region(CGRegion re) {
  // 计算一个交集区域
  
  CGRegion dre = m_draw_region.last();
  
  float x, x2, y, y2;
  
  y = dre.y2 > re.y2 ? re.y2 : dre.y2; // 选择一个小的
  y2 = dre.y > re.y ? dre.y : re.y; // 选择一个大的
  x = dre.x2 > re.x2 ? re.x2 : dre.x2; // 选择一个小的
  x2 = dre.x > re.x ? dre.x : re.x; // 选择一个大的
  
  if ( x > x2 ) {
    re.x = x2;
    re.x2 = x;
  } else {
    re.x = x;
    re.x2 = x2;
  }
  
  if ( y > y2 ) {
    re.y = y2;
    re.y2 = y;
  } else {
    re.y = y;
    re.y2 = y2;
  }
  
  re.w = re.x2 - re.x;
  re.h = re.y2 - re.y;
  
  m_draw_region.push(re);
}

/**
 * @func next_frame
 */
void DisplayPort::next_frame(Callback cb) {
  av_once(render, [cb](Event<>& evt) { cb->call(); });
}

av_gui_end
