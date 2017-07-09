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

#include "autil/fs.h"
#include "./av8.h"
#include "./fs-1.h"

/**
 * @ns avocado::av8
 */

av8_begin

static const String chars("ABCDEFGHIJKMLNOPQRSTUVWXYZabcdefghijkmlnopqrstuvwxyz_$");

class JSFileReader: public FileReader {
public:
  
  JSFileReader(Worker* worker)
    : FileReader( move(*FileReader::shared()) )
    , _worker_thread_id( Thread::current_id() )
    , _worker(worker)
  {
  
  }
  
  String parse_avocado_lib_inl_path(cString& path) {
    
    if ( Thread::current_id() != _worker_thread_id ) {
      av_err("%s", "`get_path()` must be called in the V8 working thread");
      return path;
    }
    
    if (_get_path_func.IsEmpty()) {
      Local<v8::Object> util = _worker->binding("_util");
      Local<v8::Value> func = util->Get(_worker->strs()->get_path());
      _get_path_func.Reset(_worker->isolate(), func.As<Function>());
    }
    
    Local<v8::Value> rv;
    Local<v8::Value> arg = _worker->New(path);
    Local<v8::Function> func = _worker->strong(_get_path_func);
    if ( _worker->call(func, _worker->Undefined(), 1, &arg).ToLocal(&rv) ) {
      return _worker->to_string_utf8(rv);
    }
    return String::empty;
  }
  
  bool is_lib_path(cString& path) {
    if (path[0] == ':' && chars.index_of(path[1]) != -1) {
      return true;
    }
    if ((path[0] == 'l' || path[0] == 'L') &&
        (path[1] == 'i' || path[1] == 'I') &&
        (path[2] == 'b' || path[2] == 'B') &&
        path[3] == ':' &&
        path[4] == '/' &&
        path[5] == '/' && chars.index_of(path[6]) != -1) {
      return true;
    }
    return false;
  }
  
  String get_path(cString& path) {
    return is_lib_path(path) ? parse_avocado_lib_inl_path(path) : path;
  }
  
  virtual uint read(cString& path, Callback cb) {
    String p = get_path(path);
    if (p.is_empty()) {
      Error err("Invalid file path, \"%s\"", *path);
      RunLoop::next_tick(Callback([err, cb](SimpleEvent& d) { sync_callback(cb, &err); }));
      return 0;
    }
    return FileReader::read(p, cb);
  }
  
  virtual Buffer read_sync(cString& path) av_def_err {
    String p = get_path(path);
    av_assert_err(!p.is_empty(), "Invalid file path, \"%s\"", *path);
    return FileReader::read_sync(p);
  }
  
  virtual bool exists(cString& path) {
    String p = get_path(path);
    return p.is_empty() ? false : FileReader::exists(p);
  }
  
  virtual Array<Dirent> ls(cString& path) {
    String p = get_path(path);
    return p.is_empty() ? Array<Dirent>() : FileReader::ls(p);
  }
  
  virtual String format(cString& path) {
    if (is_lib_path(path)) {
      return parse_avocado_lib_inl_path(path);
    } else {
      return FileReader::format(path);
    }
  }
  
private:
  ThreadID  _worker_thread_id;
  Worker*   _worker;
  v8::Persistent<v8::Function> _get_path_func;
};

/**
 * @func set_avocado_js_framework_file_reader
 */
void set_avocado_js_framework_file_reader(Worker* worker) {
  FileReader::set_shared_instance(new JSFileReader(worker));
}

/**
 * @func recovery_default_file_reader
 */
void recovery_default_file_reader() {
  FileReader::set_shared_instance(new FileReader());
}

av8_end
