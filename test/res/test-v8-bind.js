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

import ':util';
import ':util/buffer';
import ':util/http';
var value = binding('_value');

console.log('args:', util.args, util.libs.main);

class Test {
  
  aa : 'ABCDEFG'
  bb : function() {
    return 'bb'
  }
  cc() {
    const aa = 100;
    return 'cc' + aa;
  }
  
  dd() {

    var color = new value.Color(100, 200, 300, 400);
    
    console.log(color.to_string());
    console.log(color.to_hex24_string());
    console.log(color.to_rgb_string());
    console.log(color.to_rgba_string());
    console.log(this.aa);
    console.log(this.bb());
    console.log(this.cc());
    
    var buff0 = new buffer.Buffer('e6a59ae5ada6e6968741', 'hex');
    var buff = new buffer.Buffer('5qWa5a2m5paHQQ==', 'base64');
    var buff2 = new buffer.Buffer('你好sadasdfsadasdassdasd基本面a基本基本\
基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本基本');
    
    console.log(buff.to_string());
    console.log(buff.to_string('hex'));
    console.log(buff.to_string('base64'));
    
    console.log(util.sys_info());
    // console.log(http.get_sync('http://www.baidu.com/').to_string());
    
    // http.get('http://www.baidu.com/', function(buff) {
    http.get('http://fanyi.baidu.com/#en/zh/ELLIPSIS', function(buff) {
      console.log(buff.to_string(), buff.length);
    });
    
    var o = { a: 1000, b: buff0, c: buff, d: buff2, e: this };
    
    o.u = o;
    
    console.log(o);
    
    var i = 0;
    
    var id = setInterval(function () {
      console.log(++i);
                
      if (i == 20) {
        clearInterval(id);
      }
    }, 1000.3);
    
  }
}

new Test().dd();
