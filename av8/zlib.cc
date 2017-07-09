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

#include "autil/zlib.h"
#include "./fs-1.h"

/**
 * @ns avocado::av8
 */

av8_begin

/**
 * @class WrapGZip
 */
class WrapGZip {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    typedef WrapFileImpl<GZip> WrapGZip;
    av8_binding_class(GZip, WrapGZip::constructor, {
      av8_set_cls_property(path, WrapGZip::path);
      av8_set_cls_method(is_open, WrapGZip::is_open);
      av8_set_cls_method(open, WrapGZip::open);
      av8_set_cls_method(close, WrapGZip::close);
      av8_set_cls_method(read, WrapGZip::read);
      av8_set_cls_method(write, WrapGZip::write);
    }, nullptr);
  }
};

/**
 * @class WrapZipReader
 */
class WrapZipReader: public WrapBase {
public:

  /**
   * @constructor(path[,passwd]) 
   * @arg path {String}
   * @arg [passwd] {String}
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || !args[0]->IsString() ) {
      av8_throw_err(
        "* @constructor(path[,passwd])\n"
        "* @arg path {String}\n"
        "* @arg [passwd] {String}\n"
      );
    }
    String path = worker->to_string_utf8(args[0]);
    String passwd;
    if ( args.Length() > 1 ) {
      passwd = worker->to_string_utf8(args[1]);
    }
    New<WrapZipReader>(args, new ZipReader(path, passwd));
  }
  
  /**
   * @func open()
   */
  static void open(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->open() );
  }
  
  /**
   * @func close()
   */
  static void close(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->close() );
  }
  
  /**
   * @get path {String}
   */
  static void path(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->path() );
  }
  
  /**
   * @get passwd {String}
   */
  static void passwd(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->passwd() );
  }
  
  /**
   * @func exists(in_path)
   * @arg in_path {String}
   * @ret {bool}
   */
  static void exists(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func exists(in_path)\n"
        "* @arg in_path {String}\n"
        "* @ret {bool}\n"
      );
    }
    av8_self(ZipReader);
    av8_return( self->exists(worker->to_string_utf8(args[0])) );
  }
  
  /**
   * @func ls(in_path)
   * @arg in_path {String}
   * @ret {Array}
   */
  static void ls(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
                    "* @func ls(in_path)\n"
                    "* @arg in_path {String}\n"
                    "* @ret {Array}\n"
                    );
    }
    av8_self(ZipReader);
    av8_return( self->ls(worker->to_string_utf8(args[0])) );
  }
  
  /**
   * @func jump(in_path)
   * @arg in_path {String}
   * @ret {bool}
   */
  static void jump(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func jump(in_path)"
        "* @arg in_path {String}"
        "* @ret {bool}"
      );
    }
    av8_self(ZipReader);
    av8_return( self->jump(worker->to_string_utf8(args[0])) );
  }
  
  /**
   * @func first() jump to first
   * @ret {bool}
   */
  static void first(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->first() );
  }
  
  /**
   * @func next() jump to next file
   * @ret {bool}
   */
  static void next(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->next() );
  }
  
  /**
   * @func read(buffer[,size])
   * @arg buffer {Buffer}
   * @arg [size=buffer.length] {uint}
   * @ret {int} 
   */
  static void read(FunctionCall args) {

    cchar* argument = 
    "* @func read(buffer[,size])\n"
    "* @arg buffer {Buffer}\n"
    "* @arg [size=buffer.length] {uint}\n"
    "* @ret {int}\n"
    ;

    av8_worker(args);
    if (args.Length() == 0 || !worker->has_buffer(args[0])) {
      av8_throw_err(argument);
    }
    Buffer* buff = Wrap<Buffer>::unwrap(args[0]->ToObject())->self();
    uint length = buff->length();
    if (args.Length() > 1) {
      if (args[1]->IsUint32()) {
        uint len = args[1]->ToUint32()->Value();
        length = av_min(len, length);
      } else {
        av8_throw_err(argument);
      }
    }
    av8_self(ZipReader);
    av8_return( self->read(**buff, length) );
  }
  
  /**
   * @func name()
   * @ret {String} return current file name
   */
  static void name(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->name() );
  }
  
  /**
   * @func compressed_size()
   * @ret {uint} return current file compressed size
   */
  static void compressed_size(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->compressed_size() );
  }
  
  /**
   * @func uncompressed_size()
   * @ret {uint} return current file uncompressed size
   */
  static void uncompressed_size(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipReader);
    av8_return( self->uncompressed_size() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(ZipReader, constructor, {
      av8_set_cls_method(open, open);
      av8_set_cls_method(close, close);
      av8_set_cls_property(path, path);
      av8_set_cls_property(passwd, passwd);
      av8_set_cls_method(exists, exists);
      av8_set_cls_method(jump, jump);
      av8_set_cls_method(first, first);
      av8_set_cls_method(next, next);
      av8_set_cls_method(read, read);
      av8_set_cls_method(name, name);
      av8_set_cls_method(compressed_size, compressed_size);
      av8_set_cls_method(uncompressed_size, uncompressed_size);
    }, nullptr);
  }
};

/**
 * @class WrapZipWriter
 */
class WrapZipWriter: public WrapBase {
public:
  
  /**
   * @constructor(path[,passwd])
   * @arg path {String}
   * @arg [passwd] {String}
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(
        "* @constructor(path[,passwd])\n"
        "* @arg path {String}\n"
        "* @arg [passwd] {String}\n"
      );
    }
    String path = worker->to_string_utf8(args[0]);
    String passwd;
    if ( args.Length() > 1 ) {
      passwd = worker->to_string_utf8(args[1]);
    }
    New<WrapZipWriter>(args, new ZipWriter(path, passwd));
  }
  
  /**
   * @func open([mode])
   * @arg [mode=OPEN_MODE_CREATE] {OpenMode}
   */
  static void open(FunctionCall args) {
    av8_worker(args);
    ZipWriter::OpenMode mode = ZipWriter::OPEN_MODE_CREATE;
    if ( args.Length() > 0 && ! args[0]->IsUint32() ) {
      uint arg = args[0]->ToUint32()->Value();
      if ( arg < 3 ) {
        mode = (ZipWriter::OpenMode)arg;
      }
    }
    av8_self(ZipWriter);
    av8_return( self->open(mode) );
  }
  
  /**
   * @func close()
   */
  static void close(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipWriter);
    av8_return( self->close() );
  }
  
  /**
   * @get path {String}
   */
  static void path(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ZipWriter);
    av8_return( self->path() );
  }
  
  /**
   * @get passwd {String}
   */
  static void passwd(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ZipWriter);
    av8_return( self->passwd() );
  }
  
  /**
   * 获取压缩等级
   * 0 - 9 个压缩等级, 数字越大需要更多处理时间
   * -1自动,0为不压缩,1最佳速度,9最佳压缩
   * 默认为-1
   * @get level {int}
   */
  static void level(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(ZipWriter);
    av8_return( self->level() );
  }
  
  /**
   * @set level {int}
   */
  static void set_level(Local<v8::String> name, Local<Value> value, PropertySetCall args) {
    if (value->IsInt32()) {
      av8_worker(args);
      av8_self(ZipWriter);
      self->set_level( value->ToInt32()->Value() );
    }
  }
  
  /**
   * @func add_file(in_path)
   * @arg in_path {String}
   * @ret {bool}
   */
  static void add_file(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func add_file(in_path)\n"
        "* @arg in_path {String}\n"
        "* @ret {bool}\n"
      );
    }
    av8_self(ZipWriter);
    av8_return( self->add_file( worker->to_string_utf8(args[0]) ));
  }
  
  /**
   * @func write(buffer[,size])
   * @arg buffer {Buffer}
   * @arg [size=buffer.length] {uint}
   * @ret {bool}
   */
  static void write(FunctionCall args) {

    cchar* argument =
    "* @func write(buffer[,size])\n"
    "* @arg buffer {Buffer}\n"
    "* @arg [size=buffer.length] {uint}\n"
    "* @ret {bool}\n"
    ;

    av8_worker(args);
    if ( args.Length() == 0 || !worker->has_buffer(args[0]) ) {
      av8_throw_err(argument);
    }
    Buffer* buff = Wrap<Buffer>::unwrap(args[0]->ToObject())->self();
    uint size = buff->length();
    
    if ( args.Length() > 1 ) {
      if (args[1]->IsUint32()) {
        uint len = args[1]->ToUint32()->Value();
        size = av_min(len, size);
      } else {
        av8_throw_err(argument);
      }
    }
    av8_self(ZipWriter);
    av8_return( self->write(WeakBuffer(**buff, size)) );
  }
  
  /**
   * @func name()
   * @ret {String} return current file name
   */
  static void name(FunctionCall args) {
    av8_worker(args);
    av8_self(ZipWriter);
    av8_return( self->name() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(ZipWriter, constructor, {
      av8_set_cls_method(open, open);
      av8_set_cls_method(close, close);
      av8_set_cls_property(path, path);
      av8_set_cls_property(passwd, passwd);
      av8_set_cls_property(level, level, set_level);
      av8_set_cls_method(add_file, add_file);
      av8_set_cls_method(write, write);
      av8_set_cls_method(name, name);
    }, nullptr);
  }
};

/**
 * @class NativeZLIB
 */
class NativeZLIB {
public:

  /**
   * @func compress_sync(data)
   * @arg data {String|ArrayBuffer|Buffer}
   * @ret {Buffer}
   */
  static void compress_sync(FunctionCall args) {
    av8_worker(args);
    if (  args.Length() < 1 ||
        !(args[0]->IsString() ||
          args[0]->IsArrayBuffer() || worker->has_buffer(args[0]))
    ) {
      av8_throw_err(
        "* @func compress_sync(data)"
        "* @arg data {String|ArrayBuffer|Buffer}"
        "* @ret {Buffer}"
      );
    }
    int level = -1;
    if ( args.Length() > 1 && args[1]->IsUint32() ) {
      level = args[1]->ToUint32()->Value();
    }
    
    if (args[0]->IsString()) {
      av8_return( ZLib::compress( worker->to_string_utf8(args[0]), level) );
    } else if (args[0]->IsArrayBuffer()) {
      v8::ArrayBuffer::Contents con = args[0].As<v8::ArrayBuffer>()->GetContents();
      WeakBuffer buff((cchar*)con.Data(), (uint)con.ByteLength());
      av8_return( ZLib::compress(buff, level) );
    } else {
      av8_return( ZLib::compress( *Wrap<Buffer>::Self(args[0].As<v8::Object>()), level) );
    }
  }

  /**
   * @func uncompress_sync(data)
   * @arg data {String|ArrayBuffer|Buffer}
   * @ret {Buffer}
   */
  static void uncompress_sync(FunctionCall args) {
    av8_worker(args);
    if (  args.Length() < 1 ||
        !(args[0]->IsString() ||
          args[0]->IsArrayBuffer() || worker->has_buffer(args[0]))
    ) {
      av8_throw_err(
        "* @func uncompress_sync(data)"
        "* @arg data {String|ArrayBuffer|Buffer}"
        "* @ret {Buffer}"
      );
    }
    if (args[0]->IsString()) {
      av8_return( ZLib::uncompress( worker->to_string_utf8(args[0])) );
    } else if (args[0]->IsArrayBuffer()) {
      v8::ArrayBuffer::Contents con = args[0].As<v8::ArrayBuffer>()->GetContents();
      WeakBuffer buff((cchar*)con.Data(), (uint)con.ByteLength());
      av8_return( ZLib::uncompress(buff) );
    } else {
      av8_return( ZLib::uncompress( *Wrap<Buffer>::Self(args[0]->ToObject())) );
    }
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    WrapGZip::binding(exports, worker);
    WrapZipReader::binding(exports, worker);
    WrapZipWriter::binding(exports, worker);
    av8_set_field(OPEN_MODE_CREATE, ZipWriter::OPEN_MODE_CREATE);
    av8_set_field(OPEN_MODE_CREATE_AFTER, ZipWriter::OPEN_MODE_CREATE_AFTER);
    av8_set_field(OPEN_MODE_ADD_IN_ZIP, ZipWriter::OPEN_MODE_ADD_IN_ZIP);
    av8_set_method(compress_sync, compress_sync);
    av8_set_method(uncompress_sync, uncompress_sync);
  }
};

av8_reg_module(_zlib, NativeZLIB);
av8_end
