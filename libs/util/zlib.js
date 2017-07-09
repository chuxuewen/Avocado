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
 *     * Neither the name of xuewen.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL xuewen.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

export binding('_zlib');

/**
 * @enum OpenMode
 * OPEN_MODE_CREATE = 0
 * OPEN_MODE_CREATE_AFTER = 1
 * OPEN_MODE_ADD_IN_ZIP = 2
 * @end
 */

 /**
	* @func compress_sync(data)
	* @arg data {String|ArrayBuffer|Buffer}
	* @ret {Buffer}
	*
	* @func uncompress_sync(data)
	* @arg data {String|ArrayBuffer|Buffer}
	* @ret {Buffer}
  */

 /**
 	* @class GZip
	*
	* @constructor(path)
	* @arg path {String}
	*
	* @get path {String}
	*
	* @get is_open()
	* @ret {bool}
	*
	* @get open([mode])
	* @arg [mode=FOPEN_R] {emun FileOpenMode}
	* @ret {bool}
	*
	* @get close()
	* @ret {bool}
	*
	* @get read(buffer[,size[,offset]])
	* arg buffer {Buffer}
	* arg [size=buffer.length] {uint} 默认使用buffer的长度
	* arg [offset=-1] {int} -1为上次读取的结尾开始读取
	* @ret {int} 返回读取的真实长度,小于0为异常
	*
	* @get write(buffer[,size[,offset]])
	* arg buffer {Buffer}
	* arg [size=buffer.length] {uint} 默认使用buffer的长度
	* arg [offset=-1] {int} -1为从最后开始写入
	* @ret {int} 返回写入的真实长度,小于0为异常
	*
  * @end
  */

 /**
  * @class ZipReader
	*
	* @constructor(path[,passwd]) 
	* @arg path {String}
	* @arg [passwd] {String}
	*
	* @get path {String}
	* @get passwd {String}
	*
	* @func open()
	*
	* @func close()
	*
	* @func exists(in_path)
	* @arg in_path {String}
	* @ret {bool}
	*
  * @func ls(in_path)
  * @arg in_path {String}
  * @ret {Array}
  *
	* @func jump(in_path)
	* @arg in_path {String}
	* @ret {bool}
	*
	* @func first() jump to first
	* @ret {bool}
	*
	* @func next() jump to next file
	* @ret {bool}
	*
	* @func read(buffer[,size])
	* @arg buffer {Buffer}
	* @arg [size=buffer.length] {uint}
	* @ret {int} 
	*
	* @func name()
	* @ret {String} return current file name
	*
	* @func compressed_size()
	* @ret {uint} return current file compressed size
	*
	* @func uncompressed_size()
	* @ret {uint} return current file uncompressed size
  * @end
  */

 /**
  * @class ZipWriter
  *
	* @constructor(path[,passwd])
	* @arg path {String}
	* @arg [passwd] {String}
	*
	* @get path {String}
	* @get passwd {String}
	*
	* @func open([mode])
	* @arg [mode=OPEN_MODE_CREATE] {OpenMode}
	*
	* @func close()
	*
	* 获取压缩等级
	* 0 - 9 个压缩等级, 数字越大需要更多处理时间
	* -1自动,0为不压缩,1最佳速度,9最佳压缩
	* 默认为-1
	* @get,set level {int}
	*
	* @func add_file(in_path)
	* @arg in_path {String}
	* @ret {bool}
	*
	* @func write(buffer[,size])
	* @arg buffer {Buffer}
	* @arg [size=buffer.length] {uint}
	* @ret {bool}
	*
	* @func name()
	* @ret {String} return current file name
  * @end
  */

