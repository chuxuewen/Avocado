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

import 'gui';
import {
  ViewController, View, Div, Text, Clip, Indep,
} from 'gui';

var arrow_size = { width: 30, height: 12 };

/**
 * 获取left
 */
function get_left(self, x, offset_x) {
  
  x -= 10; // 留出10像素边距
  var screen_width = gui.display_port.width - 20;
  var width = self.find('inl').client_width;
  
  if (screen_width < width) {
    return (screen_width - width) / 2 + 10;
  }
  else {
    var left = x + offset_x / 2 - width / 2;
    if (left < 0) {
      left = 0;
    }
    else if(left + width > screen_width){
      left = screen_width - width;
    }
    return left + 10;
  }
}

/**
 * 获取top
 */
function get_top(self, y, offset_y) {

  y -= 10; // 留出10像素边距
  var screen_height = gui.display_port.height - 20;
  var height = self.find('inl').client_height;
  
  if (screen_height < height) {
    return (screen_height - height) / 2 + 10;
  }
  else{
    var top = y + offset_y / 2 - height / 2;
    if (top < 0) {
      top = 0;
    }
    else if (top + height > screen_height) {
      top = screen_height - height;
    }
    return top + 10;
  }
}

/**
 * 获取arrowtop
 */
function get_arrow_top(self, top, y, offset_y) {
  var height = self.find('inl').client_height;
  y += offset_y / 2;
  var min = 8 + arrow_size.width / 2;
  var max = height - 8 - arrow_size.width / 2;
  if (min > max) {
    return height / 2;
  }
  return Math.min(Math.max(min, y - top), max);
}

/**
 * 获取arrowleft
 */
function get_arrow_left(self, left, x, offset_x) {
  var width = self.find('inl').client_width;
  x += offset_x / 2;
  var min = 8 + arrow_size.width / 2;
  var max = width - 8 - arrow_size.width / 2;
  if (min > max) {
    return width / 2;
  }
  return Math.min(Math.max(min, x - left), max);
}

/**
 * 尝试在目标的top显示
 */
function attempt_top(self, x, y, offset_x, offset_y, force) {

  var height = self.find('inl').client_height;
  var top = y - height - arrow_size.height;
  
  if (top - 10 > 0 || force) {
    var left = get_left(self, x, offset_x);
    var arrow_left = get_arrow_left(self, left, x, offset_x) - arrow_size.width / 2;
    self.find('inl').style = { y: top, x: left };
    self.find('arrow').style = { 
      align_x: 'left',
      align_y: 'bottom',
      y: arrow_size.height,// + 0.5, 
      x: arrow_left,
      rotate_z: 180,
    };
    return true;
  }
  return false;
}

/**
 * 尝试在目标的right显示
 */
function attempt_right(self, x, y, offset_x, offset_y, force) {
  
  var width = self.find('inl').client_width;
  
  var left = x + offset_x + arrow_size.height;
  
  if (left + width + 10 <= gui.display_port.width || force) {
    var top = get_top(self, y, offset_y);
    var arrow_top = get_arrow_top(self, top, y, offset_y) - arrow_size.height / 2;
    self.find('inl').style = { y: top, x: left };
    self.find('arrow').style = { 
      align_x: 'left',
      align_y: 'top',
      x: -(arrow_size.width / 2 + arrow_size.height / 2),
      y: arrow_top, 
      rotate_z: -90,
    };
    return true;
  }
  return false;
}

/**
 * 尝试在目标的bottom显示
 */
function attempt_bottom(self, x, y, offset_x, offset_y, force){
  
  var height = self.find('inl').client_height;
  
  var top = y + offset_y + arrow_size.height;
  
  if (top + height + 10 <= gui.display_port.height || force) {
    var left = get_left(self, x, offset_x);
    var arrow_left = get_arrow_left(self, left, x, offset_x) - arrow_size.width / 2;
    self.find('inl').style = { y: top, x: left };
    self.find('arrow').style = {
      align_x: 'left',
      align_y: 'top',
      x: arrow_left,
      y: -arrow_size.height,
      rotate_z: 0,
    };
    return true;
  }
  return false;
}

/**
 * 尝试在目标的left显示
 */
function attempt_left(self, x, y, offset_x, offset_y, force) { 
  
  var width = self.find('inl').client_width;
  var left = x - width - arrow_size.height;
  
  if (left - 10 > 0 || force) {
    
    var top = get_top(self, y, offset_y);
    var arrow_top = get_arrow_top(self, top, y, offset_y) - arrow_size.height / 2;
    self.find('inl').style = { y: top, x: left };
    self.find('arrow').style = {
      align_x: 'right',
      align_y: 'top',
      x: arrow_size.width / 2 + arrow_size.height / 2,
      y: arrow_top,
      rotate_z: 90,
    };
    return true;
  }
  return false;
}

function show_by_pos(self, x, y, offset_x, offset_y) {

  switch (self.priority) {
    case 'top':
      attempt_top(self, x, y, offset_x, offset_y) ||
      attempt_bottom(self, x, y, offset_x, offset_y) ||
      attempt_right(self, x, y, offset_x, offset_y) ||
      attempt_left(self, x, y, offset_x, offset_y) ||
      attempt_top(self, x, y, offset_x, offset_y, true);
      break;
    case 'right':
      attempt_right(self, x, y, offset_x, offset_y) ||
      attempt_left(self, x, y, offset_x, offset_y) ||
      attempt_bottom(self, x, y, offset_x, offset_y) ||
      attempt_top(self, x, y, offset_x, offset_y) ||
      attempt_right(self, x, y, offset_x, offset_y, true);
      break;
    case 'bottom':
      attempt_bottom(self, x, y, offset_x, offset_y) ||
      attempt_top(self, x, y, offset_x, offset_y) ||
      attempt_right(self, x, y, offset_x, offset_y) ||
      attempt_left(self, x, y, offset_x, offset_y) ||
      attempt_bottom(self, x, y, offset_x, offset_y, true);
      break;
    default:
      attempt_left(self, x, y, offset_x, offset_y) ||
      attempt_right(self, x, y, offset_x, offset_y) ||
      attempt_bottom(self, x, y, offset_x, offset_y) ||
      attempt_top(self, x, y, offset_x, offset_y) ||
      attempt_left(self, x, y, offset_x, offset_y, true);
      break;
  }
}

/**
 * @class Overlay
 */
export class Overlay extends ViewController {
  
  m_is_activate: false
  
  /**
   * 默认为点击屏幕任何位置都会消失
   */
  frail: true
  
  m_pos_x: 0
  m_pos_y: 0
  m_offset_x: 0
  m_offset_y: 0
  
  /**
   * 优先显示的位置
   */
  priority: 'bottom' // top | right | bottom | left
  
  get background_color() { 
    return this.find('content').background_color; 
  }
  set background_color(value) { 
    this.find('arrow_text').text_color = value;
    this.find('content').background_color = value;
  }
  
  /**
   * @overwrite
   */
  load_view(vx) {
    super.load_view(
      <Indep visible=0 width="full" height="full" background_color="#0003" opacity=0>
        <Div width="full" height="full" 
          ontouchstart="fade_out" onmousedown="fade_out" id="mask" />
        <Indep id="inl">
          <Indep id="arrow" 
            width=arrow_size.width 
            height=arrow_size.height 
            origin_x=(arrow_size.width/2) origin_y=(arrow_size.height/2)>
            <Text id="arrow_text" 
              y=-15.2 x=-3
              text_family='icon' 
              text_size=36 text_color="#fff" value="\uedc7" />
          </Indep>
          <Clip id="content" background_color="#fff" border_radius=8>
            ${vx}
          </Clip>
        </Indep>
      </Indep>
    );
    
    //gui.display_port.onchange.on(self.remove, self);
    
    this.find('inl').onclick.on(()=>{
      if ( this.frail ) {
        this.fade_out();
      }
    });
    
    this.view.append_to(gui.root);
  }
  
  fade_out() {
    this.transition({ opacity: 0, time: 200 }, ()=>{
      this.remove();
    });
  }
  
  /**
   * @funactivate # 通过目标视图显示 Overlay
   * @arg target_view {View} # 参数可提供要显示的位置信息
   * @arg [offset] {Object} # 显示目标位置的偏移
   */
  show_by_view(target_view, offset_x, offset_y) {
    offset_x = offset_x || 0;
    offset_y = offset_y || 0;
    var rect = target_view.screen_rect();
    this.show_by_pos(
      rect.x + offset_x, rect.y + offset_y, 
      rect.width - offset_x * 2, rect.height - offset_y * 2);
  }
  
  /**
   * 通过位置
   */
  show_by_pos(pos_x, pos_y, offset_x, offset_y) {
    
    var self = this;
    
    var x = Math.max(0, Math.min(gui.display_port.width, pos_x));
    var y = Math.max(0, Math.min(gui.display_port.height, pos_y));
    
    offset_x = offset_x || 0;
    offset_y = offset_y || 0;
    
    self.show();
    
    self.m_x = x;
    self.m_y = y;
    self.m_offset_x = offset_x;
    self.m_offset_y = offset_y;
    
    gui.next_frame(function() {
      show_by_pos(self, x, y, offset_x, offset_y);
      self.transition({ opacity: 1, time: 200 });
    });
    
    self.m_is_activate = true;
  }
  
  /**
   * 重新设置位置
   */
  reset() {
    if (this.m_is_activate) {
      show_by_pos(this, this.m_pos_x, this.m_pos_y, this.m_offset_x, this.m_offset_y);
    }
  }
}
