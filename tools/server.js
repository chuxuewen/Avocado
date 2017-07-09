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

var util = require('avocado-tools');
var fs = require('avocado-tools/fs');
var keys = require('avocado-tools/keys');
var service = require('avocado-tools/service');
var server = require('avocado-tools/server');
var HttpService = require('avocado-tools/http_service').HttpService;
var export_ = require('avocado-tools/export');
require('./test-multiple-service');

var config = keys.parse_file(__dirname + '/server.keys');
var ser = new server.Server( config.server );
ser.start();
server.set_shared(ser);

setTimeout(function() {
  console.log( 'start web server:' );
  export_.get_local_network_host().forEach(function(address) {
    console.log('    http://' + address + ':' + ser.port + '/');
  });
}, 200);

// Tools service
// http://127.0.0.1:1026/Tools/upload_file

var Tools = util.class('Tools', HttpService, {
  
  constructor: function(req, res) {
    HttpService.call(this, req, res);
    if ( this.form )
      this.form.is_upload = true;
  },
  
  upload_file: function () {

    console.log(this.request.headers);
    console.log('params', this.params);
    console.log('data', this.data);
    
    if (this.form) {
      for (var name in this.form.files) {
        var file = this.form.files[name];
        for (var i = 0; i < file.length; i++) {
          if ( file[i].path ) {
            fs.renameSync(file[i].path, this.server.temp + '/' + file[i].filename);
          }
        }
      }
    }
    
    console.log('\n\n---------------------------');
    
    this.cookie.set('Hello', 'Hello', new Date(2088, 1, 1), '/');
    this.cookie.set('mark', 1, new Date(2120, 1, 1), '/');
    
    this.ret_html(
      '<!doctype html>\
      <html>\
      <body>\
        <form method="post" action="/Tools/upload_file" enctype="multipart/form-data"> \
          <h2>upload file</h2>\
          <input type="file" name="upload" multiple="" />\
          <input type="submit" name="submit" value="send" />\
        </form>\
      </body>\
      </html>'
    );
  }
});

service.set('Tools', Tools);

