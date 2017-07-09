#!/usr/bin/env node
/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015-2016, Louis.chu
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

var fs = require('fs');
var path = require('path');
var inputs = process.argv.slice(2);
var output = inputs.pop();
var Buffer = require('buffer').Buffer;
var check_file_is_change = require('./check').check_file_is_change;

function format_string() {
  var rev = arguments[0];
  for (var i = 1, len = arguments.length; i < len; i++)
    rev = rev.replace(new RegExp('\\{' + (i - 1) + '\\}', 'g'), arguments[i]);
  return rev;
}

/*
struct Native_font_data_ {
  uint count;
  const byte* data;
};

const byte native_font_data_aa_ttf_[] = { 0, 1, 2, 3, 4, 5 };
 const byte native_font_data_bb_ttf_[] = { 0, 1, 2, 3, 4, 5 };

const Native_font_data_ native_fonts_[] = {
  { 6, native_font_data_aa_ttf_ },
  { 6, native_font_data_bb_ttf_ },
};
*/

var fd = null;

function write() {
  for (var i = 0; i < arguments.length; i++) {
    fs.writeSync(fd, arguments[i], 'utf-8');
    fs.writeSync(fd, '\n', 'utf-8');
  }
}

function main() {
  // console.log(inputs, output);

  if (!check_file_is_change(inputs, [output])) {
    return;
  }
  
  fd = fs.openSync(output, 'w');
  
  if (!fd) {
    throw "Output error";
  }
  
  write(
   'struct Native_font_data_ {',
        'uint count;',
        'const byte* data;',
   '};'
  );
  
  var fonts = [];
  
  for (var i = 0; i < inputs.length; i++) {
    
    var filename = inputs[i];
    var name = format_string('native_font_data_{0}_',
                             path.basename(filename).replace(/[\.-]/gm, '_'));
    var arr = fs.readFileSync(filename).toJSON().data;
    
    fonts.push({ name: name, count: arr.length });
    
    write(format_string('const byte {0}[] = {', name));
    write(arr.join(','));
    write('};');
  }
  
  write('const Native_font_data_ native_fonts_[] = {');
          
  for (var i = 0; i < fonts.length; i++) {
    write('{ ' + fonts[i].count + ', ' + fonts[i].name + ' },');
  }
  
  write('};');
  
  fs.closeSync(fd);
}

main();