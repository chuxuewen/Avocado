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

#ifndef __avocado__av8__fs_action__
#define __avocado__av8__fs_action__

#include "./av8.h"

/**
 * @ns avocado::av8
 */

av8_begin

/**
 * @class WrapFileImpl
 */
template<class T> class WrapFileImpl: public WrapBase {
public:
  
  /**
   * @constructor(path)
   * @arg path {String}
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsString()) {
      av8_throw_err(
        "* @constructor(path)\n"
        "* @arg path {String}\n"
      );
    }
    New<WrapFileImpl>(args, new T(worker->to_string_utf8(args[0])));
  }
  
  /**
   * @get path {String}
   */
  static void path(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( self->path() );
  }
  
  /**
   * @get is_open()
   * @ret {bool}
   */
  static void is_open(FunctionCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( self->is_open() );
  }
  
  /**
   * @get open([mode])
   * @arg [mode=FOPEN_R] {emun FileOpenMode}
   * @ret {bool}
   */
  static void open(FunctionCall args) {
    av8_worker(args);
    av8_self(T);
    if (args.Length() == 0 || ! args[0]->IsUint32()) {
      av8_return( self->open() );
    } else {
      uint num = args[0]->ToUint32()->Value();
      FileOpenMode type = num < FOPEN_NUM ? (FileOpenMode)num : FOPEN_R;
      av8_return( self->open(type) );
    }
  }
  
  /**
   * @get close()
   * @ret {bool}
   */
  static void close(FunctionCall args) {
    av8_worker(args);
    av8_self(T);
    av8_return( self->close() );
  }
  
  /**
   * @get read(buffer[,size[,offset]])
   * arg buffer {Buffer}
   * arg [size=buffer.length] {uint} 默认使用buffer的长度
   * arg [offset=-1] {int} -1为上次读取的结尾开始读取
   * @ret {int} 返回读取的真实长度,小于0为异常
   */
  static void read(FunctionCall args) {
    av8_worker(args);

    cchar* argument = 
    "* @get read(buffer[,size[,offset]])\n"
    "* arg buffer {Buffer}\n"
    "* arg [size=buffer.length] {uint} 默认使用buffer的长度\n"
    "* arg [offset=-1] {int} -1为上次读取的结尾开始读取\n"
    "* @ret {int64} 返回读取的真实长度,小于0为异常\n"
    ;

    if (args.Length() == 0 || ! worker->has_buffer(args[0])) {
      av8_throw_err(argument);
    }
    Buffer* buff = Wrap<Buffer>::Self(args[0]->ToObject());
    uint64 size = buff->length();
    int64 offset = -1;
    
    if ( args.Length() > 1 ) {
      if ( args[1]->IsUint32() ) {
        uint64 length_ = args[1]->ToUint32()->Value();
        size = av_min(length_, size);
        
        if ( args.Length() > 2 ) {
          if ( args[2]->IsInt32() ) {
            int64 offset_ = args[1]->ToInt32()->Value();
            offset = av_max(offset_, offset);
          } else {
            av8_throw_err(argument);
          }
        }
      } else {
        av8_throw_err(argument);
      }
    }
    
    av8_self(T);
    av8_return( self->read(**buff, size, offset) );
  }
  
  /**
   * @get write(buffer[,size[,offset]])
   * arg buffer {Buffer}
   * arg [size=buffer.length] {uint} 默认使用buffer的长度
   * arg [offset=-1] {int} -1为从最后开始写入
   * @ret {int} 返回写入的真实长度,小于0为异常
   */
  static void write(FunctionCall args) {
    av8_worker(args);

    cchar* argument = 
    "* @get write(buffer[,size[,offset]])\n"
    "* arg buffer {Buffer}\n"
    "* arg [size=buffer.length] {uint} 默认使用buffer的长度\n"
    "* arg [offset=-1] {int} -1为从最后开始写入\n"
    "* @ret {int} 返回写入的真实长度,小于0为异常\n"
    ;

    if (args.Length() == 0 || ! worker->has_buffer(args[0])) {
      av8_throw_err(argument);
    }
    Buffer* buff = Wrap<Buffer>::Self(args[0]->ToObject());
    uint64 size = buff->length();
    int64 offset = -1;
    
    if ( args.Length() > 1 ) {
      if (args[1]->IsUint32()) {
        uint64 length_ = args[1]->ToUint32()->Value();
        size = av_min(length_, size);
        
        if ( args.Length() > 2 ) {
          if ( args[2]->IsInt32() ) {
            int64 offset_ = args[1]->ToInt32()->Value();
            offset = av_max(offset_, offset);
          } else {
            av8_throw_err(argument);
          }
        }
        
      } else {
        av8_throw_err(argument);
      }
    }
    av8_self(T);
    av8_return( self->write(**buff, size, offset) );
  }
};

Callback get_callback_for_buffer(Worker* worker, Local<Value> cb);
Callback get_callback_for_io_stream(Worker* worker, Local<Value> cb);
Callback get_callback_for_buffer_http_error(Worker* worker, Local<Value> cb);
Callback get_callback_for_io_stream_http_error(Worker* worker, Local<Value> cb);

av8_end
#endif
