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

import { 
  CSS, Div, Hybrid, Button, ViewController, atom_px 
} from 'gui';

CSS({
  
  '.av_stepper': {
    width: 45 * 2 + 3,
    text_family: 'icon',
    text_line_height: 29,
    text_size: 20,
    text_color: '#727272',
  },
  
  '.av_stepper .minus, .av_stepper .plus': {
    width: 45,
    height: 28,
    border: `${atom_px} #727272`,
    border_color: '#727272',
    background_color: '#fff',
  },
  
  '.av_stepper .minus': {
    border_radius_left_top: 6,
    border_radius_left_bottom: 6,
  },
  
  '.av_stepper .plus': {
    border_left_width: 0,
    border_radius_right_top: 6,
    border_radius_right_bottom: 6,
  },
  
  '.av_stepper .minus:down, .av_stepper .plus:down': {
    background_color: '#eee',
  },
})

/**
 * @class Stepper
 */
export class Stepper extends ViewController {
  m_value: 0
  
  min: 0
  max: Infinity
  step: 1
  
  event onchange;
  
  m_minus_click_handle() {
    this.value = this.m_value - this.step;
  }
  
  m_plus_click_handle() {
    this.value = this.m_value + this.step;
  }
  
  load_view(vx) {
    super.load_view(
      <Hybrid class="av_stepper">
        <Button class="minus" 
          onclick="m_minus_click_handle" default_highlighted=0>\ued5e</Button>
        <Button class="plus" 
          onclick="m_plus_click_handle" default_highlighted=0>\ued5d</Button>
      </Hybrid>  
    )
  }
  
  get value() {
    return this.m_value;
  }
  
  set value(value) {
    
    if ( typeof value == 'number') {
      
      value = Math.min(this.max, Math.max(this.min, value));
      
      if ( value != this.m_value ) {
        this.m_value = value;
        this.trigger_change();
      }
    }
  }
}
