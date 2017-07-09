/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, Louis.chu
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

import ':gui';
import ':util/sys';
import {
  GUIApplication,
  Root, Div, Image, Sprite,
  Text, Label, Limit, Hybrid,
  Span, Video, Panel, Button, TextNode, Scroll
} from ':gui';

const action1 = {
  delay: 1,
  loop: 10,
  speed: 1,
  frame: [
    { time:0, x:100, y:50 },
    { time:2, x:200, y:100 },
  ]
};

const action2 = {
  spawn: [
    action1,
  ]
}

const action3 = {
  seq: [ 
    action2, 
    [
      { time:0, x:100, y:50 },
      { time:2, x:200, y:100 },
    ],
  ]
}

const style1 = {
  width: "full",
  height: "80%",
  border_radius: 20,
  border: '4 #888',
  background_color: '#fff',
  margin: 20,
  action: [
    { time:0,     width: "100%", height:"85%",  rotate_z:0,  scale:"1 1",      opacity:1,   curve:'linear' },
    { time:1e3,   width: "50%",  height:"40%",  rotate_z:-5, scale:"0.8 0.8",  opacity:0.5, curve:'ease' },
    { time:15e3,  width: "100%", height:"85%",  rotate_z:0,  scale:"1 1",     opacity:1,   curve:'linear' },
    { time:25e3,  width: "100%", height:"85%",  rotate_z:0,  scale:"1 1",     opacity:1,   curve:'linear' },
  ],
};

const style2 = {
  width: 100,
  height: 100,
  receive: 1,
  scale: '1 1',
  origin: '50 50',
  border_radius: 20,
  //normal: { time: 200, scale: '1 1' },
  //hover: { time: 200, scale: '1.2 1.2' },
  //down: { time: 200, scale: '1.1 1.1' },
};

const hao = '你好吗!';

const box = (
<Div width="100%">
  <Div background_color="#f70" width=100 height=100 receive=1>
    <Label text_align="left_reverse" text_background_color="#ff0" value=`哈${hao}俣` />
    <Label x=50 y=25 text_align="center_reverse" text_background_color="#f0f" value=%{hao} />
    <Label x=100 y=50 text_align="right_reverse" text_background_color="#0ff">你好吗?</Label>
  </Div>
  <Div background_color="#f80" width=100 height=100 receive=1>
    <Label text_align="left" text_color="#fff">你好吗？</Label>
    <Label x=50 y=25 text_align="center" text_color="#fff">你好吗？</Label>
    <Label x=100 y=50 text_align="right" text_color="#fff">你好吗？</Label>
  </Div>
  <Div background_color="#f90" width=100 height=100 receive=1>
    <Limit width=50 height=50 max_width=70 max_height=70 background_color="#f0f">
      <Div background_color="#00f5" width=90 height=90>Limit</Div>
    </Limit>
  </Div>
  <Div background_color="#fb0" width=100 height=100 receive=1 />
    <Div background_color="#fc0" width=100 height=100 receive=1>
    <Sprite width=100 height=100 src=":simple/10440501.jpg"
      ratio="vec2(0.413,0.413)" repeat="mirrored_repeat" start="vec2(50,50)" />
  </Div>
  <Div background_color="#fd0" width=100 height=100 receive=1 />
    <Text background_color="#fff" width=200 height=100
          text_background_color="#0fff"
          text_color="#000"
          text_size=12
          text_align="center"
          id="text">@@
Touch Code Pro
(9:23)
ABCDABCDAKCDABCD
中国联通
</Text>
</Div>);

const test_scroll = (
  <Scroll style=style1 action.loop=1e8 action.playing=0>
    <Button style=style2 background_color="#f10" />
    <Button style=style2 background_color="#f30" />
    <Button style=style2 background_color="#f50" />
    <Button style=style2 background_color="#f70" />
    <Button style=style2 background_color="#f90" />
    <Button style=style2 background_color="#f0a" />
    <Div background_color="#0f0" width="100%" height=100 />
    <vx:box />
    <vx:box />
    <vx:box />
    <vx:box />
    <vx:box />
  </Scroll>
)

console.log(__path('bb.jpg'));
console.log(__path('cc.jpg'));

const div1 = (
  <Div width="full">
    <Div background_color="#0f0" width="20%" height=80 receive=1 />
    <Image width="20%" height=80 src=":simple/bb.jpg" receive=1 />
    <Div background_color="#f30" width="20%" height=80 receive=1 />
    <Div background_color="#f50" width="20%" height=80 />
    <Div background_color="#f60" width="20%" height=80 />
    <Div background_color="#0f0" width="20%" height=80 />
    <Div background_color="#f80" width="20%" height=80 />
    <Div background_color="#f90" width="20%" height=80 />
    <Div background_color="#fa0" width="20%" height=80 />
    <Div background_color="#fb0" width="20%" height=80 />
    <Div background_color="#fc0" width="20%" height=80 />
    <Div background_color="#0f0" width="20%" height=80 />
    <Div background_color="#fe0" width="20%" height=80 />
    <Div background_color="#ff0" width="20%" height=80 />
    <Div background_color="#fe0" width="20%" height=80 />
    <Div background_color="#fd0" width="20%" height=80 />
    <Div background_color="#fc0" width="20%" height=80 />
    <Div background_color="#0f0" width="20%" height=80 />
    <Div background_color="#fc0" width="20%" height=80 />
    <Div background_color="#f90" width="20%" height=80 />
    <Div background_color="#f80" width="20%" height=80 />
    <Div background_color="#f70" width="20%" height=80 />
    <Div background_color="#f60" width="20%" height=80 />
    <Div background_color="#f50" width="20%" height=80 />
    <Div background_color="#f40" width="20%" height=80 />
    <Div background_color="#f30" width="20%" height=80 />
    <Div background_color="#f20" width="20%" height=80 />
    <Div background_color="#f10" width="20%" height=80 />
  </Div>
)

// start gui application
new GUIApplication({ multisample: 4 }).start(
  <Root background_color="#0ff">
    <vx:test_scroll id="view1" />
    %{ div1 }
  </Root>
).onload = function() {
  
  var r = gui.root;

  for ( var i in sys ) {
    if ( typeof sys[i] == 'function' ) {
      console.log(i, sys[i]());
    }
  }

  // console.log('======================');
  // console.log(gui.root_ctr.find('text').text_background_color);
  // console.log(r);
  // console.log('======= OK start gui =======');

  gui.root_ctr.find("view1").onaction_loop = (evt)=>{
    console.log('action loop:', evt.loop, 'delay:', evt.delay);
  };

  gui.root_ctr.find("view1").onaction_keyframe = function(evt) {
    console.log('action keyframe:', evt.frame, 'loop:', evt.loop, 'delay:', evt.delay);
  };

  r.onclick = function(evt) {/*
    if ( !(evt.origin instanceof Button) ) {
      evt.origin.background_color = '#fff';
      console.log(evt.x, evt.y, evt.data, typeof evt.noticer);
    }*/
  };

  r.onhighlighted = function(evt) {/*
    if ( !(evt.origin instanceof Button) ) {
      if ( evt.status == gui.HIGHLIGHTED_DOWN ) {
        evt.origin.background_color = '#f0f';
      } else {
        evt.origin.background_color = '#ff0';
      }
    }*/
  };

  r.onkeydown = function(evt) {
    console.log('keydown, keycode:', evt.keycode, ', repeat:', evt.repeat);
  };

  r.onkeyup = function(evt) {
    console.log('keyup, keycode:', evt.keycode, ', repeat:', evt.repeat);
  };

};
