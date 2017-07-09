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

export binding('_fs');

 /**
	* @enum FileOpenMode
	* FOPEN_READ
	* FOPEN_WRITE
	* FOPEN_APPEND
	* FOPEN_READ_PLUS
	* FOPEN_WRITE_PLUS
	* FOPEN_APPEND_PLUS
	* FOPEN_R
	* FOPEN_W
	* FOPEN_A
	* FOPEN_RP
	* FOPEN_WP
	* FOPEN_AP
	* @end
  */

 /**
 	* @enum FileType
 	* FILE_UNKNOWN,
  * FILE_FILE,
  * FILE_DIR,
  * FILE_LINK,
  * FILE_FIFO,
  * FILE_SOCKET,
  * FILE_CHAR,
  * FILE_BLOCK
 	* @end
  */

 /**
  * DEFAULT_MODE
  */

 /**
 	* @object Dirent
 	* name     {String}
 	*	pathname {String}
 	*	type     {FileType}
 	* @end
  */

 /**
  * @class FileStat
	*
	* @constructor([path])
	* @arg [path] {String}
	*
	* @func is_valid()
	* @ret {bool}
	*
	* @func is_file()
	* @ret {bool}
	*
	* @func is_dir()
	* @ret {bool}
	*
	* @func is_link()
	* @ret {bool}
	*
	* @func is_sock()
	* @ret {bool}
	*
	* @func mode()
	* @ret {uint64}
	*
	* @func type()
	* @ret {FileType}
	*
	* @func group()
	* @ret {uint64}
	*
	* @func owner()
	* @ret {uint64}
	*
	* @func size()
	* @ret {uint64}
	*
	* @func nlink()
	* @ret {uint64}
	*
	* @func ino()
	* @ret {uint64}
	*
	* @func blksize()
	* @ret {uint64}
	*
	* @func blocks()
	* @ret {uint64}
	*
	* @func flags()
	* @ret {uint64}
	*
	* @func gen()
	* @ret {uint64}
	*
	* @func dev()
	* @ret {uint64}
	*
	* @func rdev()
	* @ret {uint64}
	*
	* @func atime()
	* @ret {uint64}
	*
	* @func mtime()
	* @ret {uint64}
	*
	* @func ctime()
	* @ret {uint64}
	*
	* @func birthtime()
	* @ret {uint64}
	*
  * @end
  */
 
 /**
 	* @class File
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
	* @func reader.read_stream(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	* @ret {uint} return read id
	*
	* @func reader.read(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	* @ret {uint} return read id
	*
	* @func reader.read_sync(path)
	* @arg path {String}
	* @ret {Buffer} return read Buffer
	*
	* @func reader.exists(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func reader.is_absolute(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func reader.format(path)
	* @arg path {String}
	* @ret {String} return new path
	*
	* @func reader.abort(id)
	* @arg id {uint} abort id
	*
	* @func clear()
	*/

 /**
  *
	* @func chmod(path[,mode[,cb]][,cb])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @arg [cb] {Function}
	*
	* @func chmod_r(path[,mode[,cb]][,cb])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @arg [cb] {Function}
	* @ret {uint} return id
	*
	* @func chown(path, owner, group[,cb])
	* @arg path {String}
	* @arg owner {uint}
	* @arg group {uint}
	* @arg [cb] {Function}
	*
	* @func chown_r(path, owner, group[,cb])
	* @arg path {String}
	* @arg owner {uint}
	* @arg group {uint}
	* @arg [cb] {Function}
	* @ret {uint} return id
	*
	* @func mkdir(path[,mode[,cb]][,cb])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @arg [cb] {Function}
	*
	* @func mkdir_p(path[,mode[,cb]][,cb])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @arg [cb] {Function}
	* @ret {uint} return id
	*
	* @func rename(name,new_name[,cb])
	* @arg name {String}
	* @arg new_name {String}
	* @arg [cb] {Function}
	*
	* @func unlink(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func rmdir(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func rm_r(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	* @ret {uint} return id
	*
	* @func cp(path,target[,cb])
	* @arg path {String}
	* @arg target {String}
	* @arg [cb] {Function}
	* @ret {uint} return id
	*
	* @func cp_r(path, target[,cb])
	* @arg path {String}
	* @arg target {String}
	* @arg [cb] {Function}
	* @ret {uint} return id
	*
	* @func ls(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func ls_l(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func write_file(path,buffer[,size[,offset[,cb]][,cb]][,cb])
	* @arg path {String}
	* @arg buffer {String|Buffer|ArrayBuffer}
	* @arg [size=buffer.length] {uint64}
	* @arg [offset=-1] {int64}
	* @arg [cb] {Function}
	*
	* @func read_file(path[,size[,offset[,cb]][,cb]][,cb])
	* @arg [size=-1] {int64}
	* @arg [offset=-1] {int64}
	* @arg [cb] {Function}
	*
	* @func is_file(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func is_dir(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func stat(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func exists(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func readable(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func writable(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func executable(path[,cb])
	* @arg path {String}
	* @arg [cb] {Function}
	*
	* @func abort(id) abort async io
	* @arg id {uint}
	*
	* @func read_file_stream(path[,offset[,cb]][,cb])
	* @arg [offset=-1] {int64}
	* @arg [cb] {Function}
	*
	* @func chmod_sync(path[,mode])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @ret {bool}
	*
	* @func chmod_r_sync(path[,mode])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @ret {bool}
	*
	* @func chown_sync(path, owner, group)
	* @arg path {String}
	* @arg owner {uint}
	* @arg group {uint}
	* @ret {bool}
	*
	* @func chown_r_sync(path, owner, group)
	* @arg path {String}
	* @arg owner {uint}
	* @arg group {uint}
	* @ret {bool}
	*
	* @func mkdir_sync(path[,mode])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @ret {bool}
	*
	* @func mkdir_p_sync(path[,mode])
	* @arg path {String}
	* @arg [mode=default_mode] {uint}
	* @ret {bool}
	*
	* @func rename_sync(name,new_name)
	* @arg name {String}
	* @arg new_name {String}
	* @ret {bool}
	*
	* @func unlink_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func rmdir_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func rm_r_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func cp_sync(path, target)
	* @arg path {String}
	* @arg target {String}
	* @ret {bool}
	*
	* @func cp_r_sync(path, target)
	* @arg path {String}
	* @arg target {String}
	* @ret {bool}
	*
	* @func ls_sync(path)
	* @arg path {String}
	* @ret {Array} return Array<Dirent>
	*
	* @func ls_l_sync(path)
	* @arg path {String}
	* @ret {Array} return Array<FileStat>
	*
	* @func write_file_sync(path,buffer[,size[,offset]])
	* @arg path {String}
	* @arg buffer {String|Buffer|ArrayBuffer}
	* @arg [size=buffer.length] {uint64}
	* @arg [offset=-1] {int64}
	* @ret {bool}
	*
	* @func read_file_sync(path[,size[,offset]])
	* @arg [size=-1] {int64}
	* @arg [offset=-1] {int64}
	* @ret {Buffer} return file buffer
	*
	* @func is_file_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func is_dir_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func stat_sync(path)
	* @arg path {String}
	* @ret {FileStat}
	*
	* @func exists_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func readable_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func writable_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
	* @func executable_sync(path)
	* @arg path {String}
	* @ret {bool}
	*
  */
