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
  CSS, ViewController, Div, Button, Indep, TextNode 
} from 'gui';

CSS({
  
  // checkbox
  
  '.av_checkbox': {
    width: 20,
    height: 20,
    background_color: '#fff',
    border: '1 #aaa',
    border_radius: 20,
    opacity: 1,
  },
  
  '.av_checkbox:hover': {
    background_color: '#0079ff',
    border: '1 #0079ff',
    opacity: 0.7,
  },
  
  '.av_checkbox:down': {
    background_color: '#0079ff',
    border: '1 #0079ff',
    opacity: 0.35,
  },
    
  '.av_checkbox.on': {
    background_color: '#0079ff',
    border: '1 #0079ff',
  },
    
  '.av_checkbox .mark': {
    visible: 0,
    text_family: 'icon',
    text_color: '#fff',
    text_size: 14,
    text_line_height: 20,
    opacity: 1,
  },
  
  '.av_checkbox.on .mark': {
    visible: 1,
  },
    
  // switch
  
  '.av_switch': {
    width: 50,
    height: 31,
    background_color: '#ddd',
    border_radius: 16,
    time: 300,
  },
  
  '.av_switch.on': {
    background_color: '#4dd865',
    time: 300,
  },
    
  '.av_switch .background': {
    background_color: '#eee',
    border_radius: 16,
    align: 'center',
    width: 46,
    height: 27,
    opacity: 1,
    time: 300,
  },

  '.av_switch.on .background, \
  .av_switch:down .background, \
  .av_switch:hover .background': {
    width: 0,
    height: 0,
    opacity: 0.2,
    time: 300,
  },
  
  '.av_switch .button': {
    border_radius: 16,
    background_color: '#fff',
    width: 27,
    height: 27,
    x: 2,
    y: 2,
    time: 200,
  },
  
  '.av_switch:down .button, .av_switch:hover .button': {
    width: 33,
    time: 200,
  },

  '.av_switch.on .button': {
    x: 20,
    time: 200,
  },

  '.av_switch.on:down .button, .av_switch.on:hover .button': {
    x: 14,
    time: 200,
  },
  
})

class Basic extends ViewController {
  m_selected: false
  m_disable: false
  
  event onchange;

  load_view(vx) {
    super.load_view(vx);
    this.view.onclick.on(()=>{
      if ( !this.m_disable ) {
        this.selected = !this.selected;
      }
    });
  }
  
  get disable() {
    return this.m_disable;
  }
  
  set disable(value) {
    this.receive = !value;
    this.m_disable = !!value;
  }
  
  get selected() {
    return this.m_selected;
  }
  
  set selected(value) {
    
    value = !!value;
    
    if(value != this.m_selected) {
      
      this.m_selected = value;
      
      if (value) {
        this.view.add_class('on');
      } else {
        this.view.remove_class('on');
      }
      this.trigger_change();
    }
  }
}

/**
 * @class Checkbox
 */
export class Checkbox extends Basic {

  load_view(vx) {
    super.load_view(
      <Button class="av_checkbox" default_highlighted=0>
        <TextNode class="mark" value="\ued71" />
      </Button>
    );
  }
}

/**
 * @class Switch
 */
export class Switch extends Basic {

  load_view(vx) {
    super.load_view(
      <Button class="av_switch" default_highlighted=0>
        <Indep class="background" />
        <Indep class="button" />
      </Button>
    );
  }
}
