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

var util = require('avocado-tools/util');
var fs = require('avocado-tools/fs');

var root = util.format(__dirname, '..');
var product = root + '/out/product';
var include = product + '/avocado-tools/product/include';

fs.cp_sync(root + '/tools/node_modules/avocado-tools', product + '/avocado-tools');
fs.cp_sync(root + '/tools/node_modules/install', product + '/install');
fs.chmodSync(product + '/install', 0755);
fs.chmodSync(product + '/avocado-tools/bin/linux/jsa-shell', 0755);
fs.chmodSync(product + '/avocado-tools/bin/mac/jsa-shell', 0755);
fs.chmodSync(product + '/avocado-tools/gyp/gyp', 0755);

fs.mkdir_p_sync(include + '/autil');
fs.mkdir_p_sync(include + '/agui');
fs.mkdir_p_sync(include + '/av8');

function export_header(source, target) {
	fs.ls_sync(source).forEach(function(stat) {
		var name = stat.name;
		var source1 = source + '/' + name;
		var target1 = target + '/' + name;

		if ( stat.isDirectory() ) {
			export_header(source1, target1);
		} else if ( stat.isFile() ) {
			if ( /[a-zA-Z][0-9]?\.h(\.inl)?$/.test(name) ) {
				fs.cp_sync(source1, target1);
			}
		}
	});
}

export_header(root + '/autil', include + '/autil');
export_header(root + '/agui', include + '/agui');
export_header(root + '/av8', include + '/av8');

fs.cp_sync(root + '/depe/v8/include/v8.h', include + '/v8.h');
fs.cp_sync(root + '/depe/v8/include/v8-version.h', include + '/v8-version.h');
fs.cp_sync(root + '/depe/v8/include/v8config.h', include + '/v8config.h');
fs.cp_sync(root + '/demo/examples', product + '/avocado-tools/product/examples');
fs.cp_sync(root + '/libs', product + '/avocado-tools/product/libs');
fs.cp_sync(root + '/autil/cacert.pem', product + '/avocado-tools/product/cacert.pem');
fs.cp_sync(root + '/tools/product.gypi', product + '/avocado-tools/product/avocado.gypi');

