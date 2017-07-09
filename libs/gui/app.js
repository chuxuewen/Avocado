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

import ':util';
import 'display_port';
import NativeResponder from ':util/event';
import ViewController from 'ctr';

var gui = binding('_gui');
var Root = gui.Root;
var cur = null;
var cur_root = null;
var cur_root_ctr = null;

function start(self, vx) {
  var ctr = new vx.__tag__();
  
  if ( vx.vdata ) {
    ctr.vdata = vx.vdata;
  }
  
  ctr.load_view(vx.__child__);

  // TODO set view controller arts
  for (var name in vx) {
    if ( name != 'vdata' ) {
      util.set(name, vx[name], ctr); // none bind data
    }
  }
  cur_root_ctr = ctr;
  cur_root = ctr.view;
  
  util.assert(cur_root instanceof Root, 'Bad vx data. Root controller view must be Root');
}

 /**
  * @class NativeGUIApplication
  *
  * @constructor([options])
  * @arg [options] {Object} { anisotropic {bool}, mipmap {bool}, multisample {0-4} }
  *
  * @func clear() clear gui application resources
  * @get is_load {bool}
  * @get display_port {DisplayPort}
  * @get root {Root}
  * @get first_responder {View}
  * @get,set default_text_background_color {ColorValue}
  * @get,set default_text_color {ColorValue}
  * @get,set default_text_size {TextSizeValue}
  * @get,set default_text_style {TextStyleValue}
  * @get,set default_text_family {TextFamilyValue}
  * @get,set default_text_shadow {TextShadowValue}
  * @get,set default_text_line_height {TextLineHeightValue}
  * @get,set default_text_decoration {TextDecorationValue}
  * @get,set default_text_overflow {TextOverflowValue}
  * @get,set default_text_white_space {TextWhiteSpaceValue}
  *
  * @end
  */

/**
 * @class GUIApplication
 * @bases NativeGUIApplication,NativeResponder
 */
export class GUIApplication extends gui.NativeGUIApplication {
  
  event onload;
  event onunload;
  event onbackground;
  event onforeground;
  event onpause;
  event onresume;
  event onmemorywarning;
  
  /**
  * @constructor([options])
  * @arg [options] {Object} { anisotropic {bool}, mipmap {bool}, multisample {0-4} }
  */
  constructor(options) {
    super(options); 
    cur = this;
  }
  
  /**
   * @func start(vx)
   * @arg vx {Object}
   */
  start(vx) { 
    util.assert(vx instanceof Object, 'Bad argument.');
    
    if (util.equals_class(Root, vx.__tag__)) {
      if ( this.is_load ) {
        cur_root_ctr = new ViewController();
        cur_root_ctr.load_view(vx);
        cur_root = cur_root_ctr.view;
      } else {
        this.onload.on(()=>{
          cur_root_ctr = new ViewController();
          cur_root_ctr.load_view(vx);
          cur_root = cur_root_ctr.view;
        });
      }
    }
    else if (util.equals_class(ViewController, vx.__tag__)) {
      if ( this.is_load ) {
        start(this, vx);
      } else {
        this.onload.on(()=>{ start(this, vx) });
      }
    } else {
      throw new Error('Bad argument.');
    }
    return this;
  }
  
  //@end
}

util.ext_class(GUIApplication, NativeResponder);

export {

  /**
   * @get currend {GUIApplication} 
   */
  get current() { return cur },

  /**
   * @get root {Root} 
   */
  get root() { return cur_root },

  /**
   * @get root_ctr {ViewController}
   */
  get root_ctr() { return cur_root_ctr },
};
