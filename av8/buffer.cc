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

#include "./av8.h"
#include "autil/buffer.h"

/**
 * @ns avocado::av8
 */

av8_begin

/**
 * @class NativeBuffer
 */
class WrapBuffer: public WrapBase {
  
  static void create_with_string(FunctionCall args, Encoding en) {
    av8_worker(args);
    
    av_assert( en != Encoding::unknown );

    if (en == Encoding::base64 || en == Encoding::hex) { // base64/hex 对其解码
      String str = worker->to_string_ascii(args[0]);
      New<WrapBuffer>(args, new Buffer(Coder::decoding_to_byte(en, str)));
    } else { // 编码
      Ucs2String str = worker->to_string_ucs2(args[0]);
      New<WrapBuffer>(args, new Buffer(Coder::encoding(en, str)));
    }
  }
  
  static void create_with_v8buffer(FunctionCall args) {
    av8_worker(args);
    v8::ArrayBuffer::Contents contents = args[0].As<v8::ArrayBuffer>()->GetContents();
    WeakBuffer buff((cchar*)contents.Data(), (uint)contents.ByteLength());
    New<WrapBuffer>(args, new Buffer(buff.copy()));
  }
  
  /**
   * @constructor([arg[,encoding]])
   * @arg [arg=0] {uint|String|ArrayBuffer}
   * @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);

    cchar* argument = 
    
    "* @constructor([arg[,encoding]])\n"
    "* @arg [arg=0] {uint|String|ArrayBuffer}\n"
    "* @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}\n"
    
    ;
    
    if (args.Length() < 1) {
      New<WrapBuffer>(args, new Buffer());
    }
    else if (args.Length() == 1) {
      if (args[0]->IsUint32()) {
        New<WrapBuffer>(args, new Buffer(args[0]->ToUint32()->Value()));
      }
      if (args[0]->IsString()) {
        create_with_string(args, Encoding::utf8);
      }
      else if (args[0]->IsArrayBuffer()) {
        create_with_v8buffer(args);
      } else {
        av8_throw_err(argument);
      }
    } else if (args.Length() > 1 && args[0]->IsString() && args[1]->IsString()) {
      Encoding en;
      if ( parse_encoding(args, args[1], en) ) {
        create_with_string(args, en );
      }
    } else {
      av8_throw_err(argument);
    }
  }
  
  /**
   * @get length {uint}
   */
  static void length(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Buffer);
    av8_return( self->length() );
  }
  
  /**
   * @indexed[] getter(uint index)
   * @ret {uint}
   */
  static void indexed_getter(uint index, PropertyCall args) {
    av8_worker(args);
    av8_self(Buffer);
    if ( index < self->length() ) {
      av8_return( (byte)self->item(index) );
    } else {
      av8_throw_err("Buffer access out of bounds.");
    }
  }
  
  /**
   * @indexed[] setter(uint index, uint value)
   */
  static void indexed_setter(uint index, Local<Value> value, PropertyCall args) {
    av8_worker(args);
    av8_self(Buffer);
    if ( index < self->length() ) {
      if (value->IsUint32()) {
        (*self)[index] = value->ToUint32()->Value();
        av8_return( (byte)self->item(index) );
      } else {
        av8_throw_err(
          
          "* @indexed setter[](uint index, uint value)\n"
          
        );
      }
    } else {
      av8_throw_err("Buffer access out of bounds.");
    }
  }
  
  /**
   * @func copy()
   * @ret {Buffer} return new Buffer
   */
  static void copy(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    av8_return( self->copy() );
  }
  
  /**
   * @func is_null()
   * @ret {bool}
   */
  static void is_null(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    av8_return( self->is_null() );
  }
  
  static bool parse_encoding(FunctionCall args, const Local<Value>& arg, Encoding& en) {
    av8_worker(args);
    String s = worker->to_string_utf8( arg );
    en = Coder::parse_encoding( s );
    if ( en == Encoding::unknown ) {
      worker->throw_err( 
        "Unknown encoding \"%s\", the optional value is "
        "[binary|ascii|base64|hex|utf8|ucs2|utf16|utf32]", *s ); return false;
    }
    return true;
  }

  /**
   * @func write(src[,to[,size[,form]][,encoding]][,encoding])
   * @arg src {Buffer|ArrayBuffer|String}
   * @arg [to=-1]   {int}  当前Buffer开始写入的位置
   * @arg [size=-1] {int}  需要写入项目数量,超过要写入数据的长度自动取写入数据长度
   * @arg [form=0]  {uint} 从要写入数据的form位置开始取数据
   * @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}
   * @ret {uint} return new length
   */
  static void write(FunctionCall args) {
    av8_worker(args);

    if (  args.Length() == 0 ||
        !(args[0]->IsString() || 
          args[0]->IsArrayBuffer() || 
          worker->has_buffer(args[0])
         )
    ) { // 参数错误
      av8_throw_err(
        
        "* @func write(src[,to[,size[,form]][,encoding]][,encoding])\n"
        "* @arg src {Buffer|ArrayBuffer|String}\n"
        "* @arg [to=-1]    {int} 当前Buffer开始写入的位置,-1从结尾开始写入\n"
        "* @arg [size=-1]  {int} 需要写入项目数量,超过要写入数据的长度自动取写入数据长度,-1为src源长度\n"
        "* @arg [form=0]   {uint} 从要写入数据的form位置开始取数据,默认为0\n"
        "* @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}\n"
        "* @ret {uint} return new length\n"
        
      );
    }
    
    av8_self(Buffer);
    
    int  to = -1;
    int  size = -1;
    uint form = 0;
    Encoding en = Encoding::utf8;
    
    if ( args.Length() > 1 ) {
      if ( args[1]->IsInt32() ) { // 写入到目标位置
        to = args[1]->ToInt32()->Value();
        if ( args.Length() > 2 ) {
          if ( args[2]->IsInt32() ) { // 写入大小
            size = args[2]->ToInt32()->Value();
            if ( args.Length() > 3 ) { // 编码格式
              if ( args[3]->IsUint32() ) {
                form = args[3]->ToInt32()->Value();
              }
            }
          } else if (args[2]->IsString()) { // 编码格式
            if ( ! parse_encoding(args, args[2], en) ) return;
          }
        }
      } else if (args[1]->IsString()) { // 编码格式
        if ( ! parse_encoding(args, args[1], en) ) return;
      }
    }
    
    if ( args[0]->IsString() ) { // 写入字符串
      Buffer buff;
      switch (en) {
        case Encoding::hex: // 解码 hex and base64
        case Encoding::base64: {
          buff = Coder::decoding_to_byte(en, worker->to_string_ascii( args[0] )); break;
        } default: { // 编码
          buff = Coder::encoding(en, worker->to_string_ucs2( args[0] )); break;
        }
      }
      av8_return( self->write(buff, to) );
    }
    else if (args[0]->IsArrayBuffer()) { // 写入原生 ArrayBuffer
      Local<v8::ArrayBuffer> ab = args[0].As<v8::ArrayBuffer>();
      v8::ArrayBuffer::Contents con = ab->GetContents();
      WeakBuffer buff((cchar*)con.Data(), (uint)con.ByteLength());
      av8_return( self->write(buff, to, size == -1 ? buff.length() : size, form) );
    }
    else { // Buffer
      Buffer* buff = Wrap<Buffer>::unwrap(args[0]->ToObject())->self();
      av8_return( self->write(*buff, to, size == -1 ? buff->length() : size, form) );
    }
  }

  /**
   * @func to_string([encoding])
   * @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}
   */
  static void to_string(FunctionCall args) {
    av8_worker(args);
    
    Encoding en = Encoding::utf8;
    if (args.Length() > 0 && args[0]->IsString()) {
      if ( ! parse_encoding(args, args[0], en) ) return;
    }
    
    av8_self(Buffer);
    
    switch (en) {
      case Encoding::hex: // 编码
      case Encoding::base64: {
        Buffer buff = Coder::encoding(en, *self);
        av8_return( worker->NewString(buff) );
        break;
      } default: {// 解码to ucs2
        Ucs2String str( Coder::decoding_to_uint16(en, *self) );
        av8_return( worker->New(str) );
        break;
      }
    }
  }
  
  /**
   * @func collapse()
   * @ret {ArrayBuffer}
   */
  static void collapse(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    uint len = self->length();
    char* s = self->collapse();
    Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(worker->isolate(), s, len);
    av8_return( ab );
  }
  
  /**
   * @func slice([start,[end]])
   * @arg [start = 0] {uint}
   * @arg [end = length] {uint}
   * @ret {Buffer}
   */
  static void slice(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    if (args.Length() == 0) {
      av8_return( self->copy() );
    }

    cchar* argument = 
    
    "* @func slice([start,[end]])\n"
    "* @arg [start = 0] {uint}\n"
    "* @arg [end = length] {uint}\n"
    "* @ret {Buffer}\n"
    
    ;
    
    uint start = 0;
    uint end = self->length();
    
    if (args.Length() == 1) {
      if (args[0]->IsUint32()) {
        start = args[0]->ToUint32()->Value();
      } else {
        av8_throw_err(argument);
      }
    } else {
      if (args[0]->IsUint32() && args[1]->IsUint32()) {
        start = args[0]->ToUint32()->Value();
        end = args[1]->ToUint32()->Value();
      } else {
        av8_throw_err(argument);
      }
    }
    
    Array<char, BufferContainer<char>> arr = self->slice(start, end);
    av8_return(  move(*static_cast<Buffer*>(&arr)) );
  }
  
  /**
   * @func clear()
   */
  static void clear(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    self->clear();
    av8_return( args.Holder() );
  }
  
  /**
   * @func join([sp])
   * @arg [sp=","] {String}
   * @ret {Buffer}
   */
  static void join(FunctionCall args) {
    av8_worker(args);
    String sp = ',';
    if ( args.Length() > 0 ) {
      if ( !args[0]->IsUndefined() ) {
        Local<v8::String> str = args[0]->ToString();
        if ( !str.IsEmpty() ) {
          sp = worker->to_string_utf8(str);
        }
      }
    }
    av8_self(Buffer);
    av8_return( self->join(sp) );
  }
  
  /**
   * @func push(item[,encoding])
   * @arg item {String|ArrayBuffer|Buffer}
   * @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}
   * @ret {uint} return buffer length
   */
  static void push(FunctionCall args) {
    av8_worker(args);

    if (  args.Length() == 0 ||
        !(args[0]->IsString() || 
          args[0]->IsArrayBuffer() || 
          worker->has_buffer(args[0])
        )
    ) { // 参数错误
      av8_throw_err(
        
        "* @func push(item[,encoding])\n"
        "* @arg item {String|ArrayBuffer|Buffer}\n"
        "* @arg [encoding=utf8] {binary|ascii|base64|hex|utf8|ucs2|utf16|utf32}\n"
        "* @ret {uint} return buffer length\n"
        
      );
    }
    
    Encoding en = Encoding::utf8;
    
    if (args.Length() > 1 && args[1]->IsString()) { // 第二个参数为编码格式
      if ( ! parse_encoding(args, args[1], en) ) {
        return;
      }
    }
    
    av8_self(Buffer);
    
    if (args[0]->IsString()) { // 写入字符串
      Buffer buff;
      switch (en) {
        case Encoding::hex: // 解码 hex and base64
        case Encoding::base64: {
          buff = Coder::decoding_to_byte(en, worker->to_string_ascii( args[0] )); break;
        } default: { // 编码
          buff = Coder::encoding(en, worker->to_string_ucs2( args[0] )); break;
        }
      }
      av8_return( self->push(buff) );
    }
    else if (args[0]->IsArrayBuffer()) { // 写入原生 ArrayBuffer
      Local<v8::ArrayBuffer> ab = args[0].As<v8::ArrayBuffer>();
      v8::ArrayBuffer::Contents con = ab->GetContents();
      WeakBuffer buff((cchar*)con.Data(), (uint)con.ByteLength());
      av8_return( self->push(buff) );
    }
    else { // Buffer
      Buffer* src = Wrap<Buffer>::unwrap(args[0]->ToObject())->self();
      av8_return( self->push(*src) );
    }
  }
  
  /**
   * @func pop([count])
   * @arg [count=1] {uint}
   */
  static void pop(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    if (args.Length() > 0 && args[0]->IsUint32()) { // 参数错误
      self->pop( args[0]->ToUint32()->Value() );
    } else {
      self->pop();
    }
    av8_return( args.Holder() );
  }
  
  /**
   * @func to_array()
   * @ret {Array}
   */
  static void to_array(FunctionCall args) {
    av8_worker(args);
    av8_self(Buffer);
    Buffer& buff = *self;
    Isolate* iso = worker->isolate();
    Local<v8::Array> arr = v8::Array::New(iso, buff.length());
    for (uint i = 0, len = buff.length(); i < len; i++) {
      arr->Set(i, v8::Uint32::New(iso, (byte)buff[i]) );
    }
    av8_return( arr );
  }
  
  /**
   * @func fill(value)
   * @arg value {uint}
   */
  static void fill(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsUint32()) {
      av8_throw_err(
      
      "* @func fill(value)\n"
      "* @arg value {uint}\n"
      
      );
    }
    av8_self(Buffer);
    memset(**self, args[0]->ToUint32()->Value(), self->size());
    av8_return( args.Holder() );
  }
  
  /**
   * @func for_each(Function)
   */
  static void for_each(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction() ) {
      av8_throw_err(
        
        "* @func for_each(Function)\n"
        
      );
    }
    av8_handle_scope();
    Isolate* iso = worker->isolate();
    Local<Value> recv = args.Length() > 1 ? args[1] : args.Holder().As<Value>();
    Local<Function> cb = args[0].As<Function>();
    Local<Context> ctx = worker->context();
    
    av8_self(Buffer);
    Buffer& buff = *self;
    Array<Local<Value>> arr(3);
    arr[2] = args.Holder();
    
    for (uint i = 0, len = buff.length(); i < len; i++) {
      arr[0] = v8::Uint32::New(iso, buff[i]);
      arr[1] = v8::Uint32::New(iso, i);
      if ( worker->call(cb, recv, 3, &arr[0]).IsEmpty() ) {
        return;
      }
    }
    
    av8_return( args.Holder() );
  }
  
  /**
   * @func filter(Function)
   * @ret {Buffer} return new Buffer
   */
  static void filter(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction() ) {
      av8_throw_err(
        
        "* @func filter(Function)\n"
        "* @ret {Buffer} return new Buffer\n"
        
      );
    }
    av8_handle_scope();
    Isolate* iso = worker->isolate();
    Local<Value> recv = args.Length() > 1 ? args[1] : args.Holder().As<Value>();
    Local<Function> cb = args[0].As<Function>();
    Local<Context> ctx = worker->context();
    
    av8_self(Buffer);
    Buffer& buff = *self;
    Array<Local<Value>> arr(3);
    arr[2] = args.Holder();
    Buffer result;
    
    for (uint i = 0, len = buff.length(); i < len; i++) {
      arr[0] = v8::Uint32::New(iso, buff[i]);
      arr[1] = v8::Uint32::New(iso, i);
      
      Local<Value> rev;
      if ( worker->call(cb, recv, 3, &arr[0]).ToLocal(&rev) ) { // 异常结束
        if ( rev->IsTrue() ) {
          result.push( buff[i] );
        }
      } else {
        return;
      }
    }
    av8_return( move(result) );
  }
  
  /**
   * @func some(Function)
   * @ret {bool} 
   */
  static void some(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction() ) {
      av8_throw_err(
        
        "* @func some(Function)\n"
        "* @ret {bool}\n"
        
      );
    }
    av8_handle_scope();
    Isolate* iso = worker->isolate();
    Local<Value> recv = args.Length() > 1 ? args[1] : args.Holder().As<Value>();
    Local<Function> cb = args[0].As<Function>();
    Local<Context> ctx = worker->context();
    
    av8_self(Buffer);
    Buffer& buff = *self;
    Array<Local<Value>> arr(3);
    arr[2] = args.Holder();
    Buffer result;
    
    for (uint i = 0, len = buff.length(); i < len; i++) {
      arr[0] = v8::Uint32::New(iso, buff[i]);
      arr[1] = v8::Uint32::New(iso, i);
      Local<v8::Value> rv;
      if ( worker->call(cb, recv, 3, &arr[0]).ToLocal(&rv) ) { // 异常结束
        if (rv->IsTrue()) {
          av8_return( true );
        }
      } else {
        return;
      }
    }
    av8_return( false );
  }
  
  /**
   * @func every(Function)
   * @ret {bool}
   */
  static void every(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction() ) {
      av8_throw_err(
        
        "* @func every(Function)\n"
        "* @ret {bool}\n"
        
      );
    }
    av8_handle_scope();
    Isolate* iso = worker->isolate();
    Local<Value> recv = args.Length() > 1 ? args[1] : args.Holder().As<Value>();
    Local<Function> cb = args[0].As<Function>();
    Local<Context> ctx = worker->context();
    
    av8_self(Buffer);
    Buffer& buff = *self;
    Array<Local<Value>> arr(3);
    arr[2] = args.Holder();
    Buffer result;
    
    for (uint i = 0, len = buff.length(); i < len; i++) {
      arr[0] = v8::Uint32::New(iso, buff[i]);
      arr[1] = v8::Uint32::New(iso, i);
      Local<Value> rv;
      if ( worker->call(cb, recv, 3, &arr[0]).ToLocal(&rv) ) { // 异常结束
        if ( rv->IsFalse() ) {
          av8_return( false );
        }
      } else {
        return;
      }
    }
    av8_return( true );
  }
  
  /**
   * @func map(Function)
   * @ret {Buffer} return new Buffer
   */
  static void map(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsFunction() ) {
      av8_throw_err(
        
        "* @func map(Function)\n"
        "* @ret {Buffer} return new Buffer\n"
        
      );
    }
    av8_handle_scope();
    Isolate* iso = worker->isolate();
    Local<Value> recv = args.Length() > 1 ? args[1] : args.Holder().As<Value>();
    Local<Function> cb = args[0].As<Function>();
    Local<Context> ctx = worker->context();
    
    av8_self(Buffer);
    Buffer& buff = *self;
    Array<Local<Value>> arr(3);
    arr[2] = args.Holder();
    Buffer result;
    
    for (uint i = 0, len = buff.length(); i < len; i++) {
      arr[0] = v8::Uint32::New(iso, buff[i]);
      arr[1] = v8::Uint32::New(iso, i);
      Local<Value> rv;
      if ( worker->call(cb, recv, 3, &arr[0]).ToLocal(&rv) ) { // 异常结束
        result.push( rv->IsInt32() ? rv->ToUint32()->Value() : 0 );
      } else {
        return;
      }
    }
    av8_return( move( result ) );
  }
  
public:
  /**
   * @func binding
   */
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Buffer, constructor, {
      av8_set_cls_property(length, length);
      av8_set_cls_indexed(indexed_getter, indexed_setter);
      av8_set_cls_method(copy, copy);
      av8_set_cls_method(is_null, is_null);
      av8_set_cls_method(write, write);
      av8_set_cls_method(toString, to_string);
      av8_set_cls_method(to_string, to_string);
      av8_set_cls_method(collapse, collapse);
      av8_set_cls_method(slice, slice);
      av8_set_cls_method(clear, clear);
      av8_set_cls_method(join, join);
      av8_set_cls_method(push, push);
      av8_set_cls_method(pop, pop);
      av8_set_cls_method(to_array, to_array);
      av8_set_cls_method(fill, fill);
      av8_set_cls_method(forEach, for_each);
      av8_set_cls_method(for_each, for_each);
      av8_set_cls_method(filter, filter);
      av8_set_cls_method(some, some);
      av8_set_cls_method(every, every);
      av8_set_cls_method(map, map);
    }, nullptr);
  }
};

av8_reg_module(_buffer, WrapBuffer);
av8_end
