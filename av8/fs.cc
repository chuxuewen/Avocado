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
#include "autil/http.h"
#include "./av8.h"
#include "fs-1.h"

/**
 * @ns avocado::av8
 */

av8_begin

template<class T, class Err = Error>
Callback get_callback_for_t(Worker* worker, Local<Value> cb) {
  if ( !cb.IsEmpty() && cb->IsFunction() ) {
    PersistentFunc func(worker->isolate(), cb.As<Function>());
    
    return Callback([worker, func](SimpleEvent& d) {
      Isolate* iso = worker->isolate();
      v8::HandleScope scope(iso);
      
      Local<Function> f = worker->local(func);
      
      if ( d.error ) {
        Local<Value> arg = worker->New(*static_cast<const Err*>(d.error));
        Local<Function> e = f->Get(worker->strs()->Throw()).As<Function>();
        worker->call(e, f, 1, &arg);
      } else {
        T* data = static_cast<T*>(d.data);
        Local<v8::Value> arg = worker->New( *data );
        worker->call(f, worker->Undefined(), 1, &arg);
      }
    });
  } else {
    return Callback();
  }
}

Callback get_callback_for_buffer(Worker* worker, Local<Value> cb) {
  return get_callback_for_t<Buffer, Error>(worker, cb);
}

Callback get_callback_for_buffer_http_error(Worker* worker, Local<Value> cb) {
  return get_callback_for_t<Buffer, HttpError>(worker, cb);
}

Callback get_callback_for_none(Worker* worker, Local<Value> cb) {
  if ( !cb.IsEmpty() && cb->IsFunction() ) {
    PersistentFunc func(worker->isolate(), cb.As<Function>());
    
    return Callback([worker, func](SimpleEvent& d) {
      Isolate* iso = worker->isolate();
      v8::HandleScope scope(iso);
      
      Local<Function> f = worker->local(func);
      
      if ( d.error ) {
        Local<Value> arg = worker->New(*static_cast<const Error*>(d.error));
        Local<Function> e = f->Get(worker->strs()->Throw()).As<Function>();
        worker->call(e, f, 1, &arg);
      }
    });
  } else {
    return Callback();
  }
}

template<class Err = Error>
Callback _get_callback_for_io_stream(Worker* worker, Local<Value> cb) {
  if ( !cb.IsEmpty() && cb->IsFunction() ) {
    PersistentFunc func(worker->isolate(), cb.As<Function>());
    
    return Callback([worker, func](SimpleEvent& d) {
      Isolate* iso = worker->isolate();
      v8::HandleScope scope(iso);
      
      Local<Function> f = worker->local(func);
      
      if ( d.error ) {
        Local<Value> arg = worker->New(*static_cast<const Err*>(d.error));
        Local<Function> e = f->Get(worker->strs()->Throw()).As<Function>();
        worker->call(e, f, 1, &arg);
      } else {
        
        IOStreamData* data = static_cast<IOStreamData*>(d.data);
        
        Local<v8::Value> args[4] = {
          worker->New( move(data->buffer()) ),
          worker->New( data->complete() ),
          worker->New( data->size() ),
          worker->New( data->total() ),
        };
        
        worker->call(f, worker->Undefined(), 4, args);
      }
    });
  } else {
    return Callback();
  }
}

Callback get_callback_for_io_stream(Worker* worker, Local<Value> cb) {
  return _get_callback_for_io_stream(worker, cb);
}

Callback get_callback_for_io_stream_http_error(Worker* worker, Local<Value> cb) {
  return _get_callback_for_io_stream<HttpError>(worker, cb);
}

class WrapFileStat: public WrapBase {
public:

  /**
   * @constructor([path])
   * @arg [path] {String}
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || ! args[0]->IsString()) {
      New<WrapFileStat>(args, new FileStat());
    } else {
      New<WrapFileStat>(args, new FileStat(worker->to_string_utf8(args[0])));
    }
  }

  /**
   * @func is_valid()
   * @ret {bool}
   */
  static void is_valid(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->is_valid() );
  }

  /**
   * @func is_file()
   * @ret {bool}
   */
  static void is_file(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->is_file() );
  }

  /**
   * @func is_dir()
   * @ret {bool}
   */
  static void is_dir(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->is_dir() );
  }

  /**
   * @func is_link()
   * @ret {bool}
   */
  static void is_link(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->is_link() );
  }

  /**
   * @func is_sock()
   * @ret {bool}
   */
  static void is_sock(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->is_sock() );
  }

  /**
   * @func mode()
   * @ret {uint64}
   */
  static void mode(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->mode() );
  }

  /**
   * @func type()
   * @ret {DirentType}
   */
  static void type(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->type() );
  }
  
  /**
   * @func group()
   * @ret {uint64}
   */
  static void group(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->group() );
  }

  /**
   * @func owner()
   * @ret {uint64}
   */
  static void owner(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->owner() );
  }

  /**
   * @func size()
   * @ret {uint64}
   */
  static void size(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->size() );
  }

  /**
   * @func nlink()
   * @ret {uint64}
   */
  static void nlink(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->nlink() );
  }

  /**
   * @func ino()
   * @ret {uint64}
   */
  static void ino(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->ino() );
  }

  /**
   * @func blksize()
   * @ret {uint64}
   */
  static void blksize(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->blksize() );
  }

  /**
   * @func blocks()
   * @ret {uint64}
   */
  static void blocks(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->blocks() );
  }

  /**
   * @func flags()
   * @ret {uint64}
   */
  static void flags(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->flags() );
  }

  /**
   * @func gen()
   * @ret {uint64}
   */
  static void gen(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->gen() );
  }

  /**
   * @func dev()
   * @ret {uint64}
   */
  static void dev(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->dev() );
  }

  /**
   * @func rdev()
   * @ret {uint64}
   */
  static void rdev(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->rdev() );
  }

  /**
   * @func atime()
   * @ret {uint64}
   */
  static void atime(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->atime() / 1000 );
  }

  /**
   * @func mtime()
   * @ret {uint64}
   */
  static void mtime(FunctionCall args) {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->mtime() / 1000 );
  }

  /**
   * @func ctime()
   * @ret {uint64}
   */
  static void ctime(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->ctime() / 1000 );
  }

  /**
   * @func birthtime()
   * @ret {uint64}
   */
  static void birthtime(FunctionCall args)  {
    av8_worker(args);
    av8_self(FileStat);
    av8_return( self->birthtime() / 1000 );
  }

  static void binding(Local<v8::Object> exports, Worker* worker) {
    
    av8_binding_class(FileStat, constructor, {
      av8_set_cls_method(is_valid, is_valid);
      av8_set_cls_method(is_file, is_file);
      av8_set_cls_method(is_dir, is_dir);
      av8_set_cls_method(is_link, is_link);
      av8_set_cls_method(is_sock, is_sock);
      av8_set_cls_method(mode, mode);
      av8_set_cls_method(group, group);
      av8_set_cls_method(owner, owner);
      av8_set_cls_method(size, size);
      av8_set_cls_method(nlink, nlink);
      av8_set_cls_method(ino, ino);
      av8_set_cls_method(blksize, blksize);
      av8_set_cls_method(blocks, blocks);
      av8_set_cls_method(flags, flags);
      av8_set_cls_method(gen, gen);
      av8_set_cls_method(dev, dev);
      av8_set_cls_method(rdev, rdev);
      av8_set_cls_method(atime, atime);
      av8_set_cls_method(mtime, mtime);
      av8_set_cls_method(ctime, ctime);
      av8_set_cls_method(birthtime, birthtime);
    }, NULL);
  }
};

class WrapFile {
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    
    typedef WrapFileImpl<File> WrapFile;
    
    av8_binding_class(File, WrapFile::constructor, {
      av8_set_cls_property(path, WrapFile::path);
      av8_set_cls_method(is_open, WrapFile::is_open);
      av8_set_cls_method(open, WrapFile::open);
      av8_set_cls_method(close, WrapFile::close);
      av8_set_cls_method(read, WrapFile::read);
      av8_set_cls_method(write, WrapFile::write);
    }, nullptr);
  }
};

class NativeFileReader {
  
  template<bool stream> static void read(FunctionCall args, cchar* argument) {
    av8_worker(args);
    if ( args.Length() == 0 || ! args[0]->IsString() ) {
      av8_throw_err(argument);
    }
    
    String path = worker->to_string_utf8(args[0]);
    Callback cb;
    
    if ( args.Length() > 1 ) {
      cb = stream ?
           get_callback_for_io_stream(worker, args[1]) :
           get_callback_for_buffer(worker, args[1]);
    }
    if ( stream ) {
      av8_return( freader()->read_stream( path, cb ) );
    } else {
      av8_return( freader()->read( path, cb ) );
    }
  }

  /**
   * @func reader.read_stream(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   * @ret {uint} return read id
   */
  static void read_stream(FunctionCall args) {
    read<true>(args, 
      "* @func reader.read_stream(path[,cb])\n"
      "* @arg path {String}\n"
      "* @arg [cb] {Function}\n"
      "* @ret {uint} return read id\n"
    );
  }

  /**
   * @func reader.read(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   * @ret {uint} return read id
   */
  static void read(FunctionCall args) {
    read<false>(args, 
      "* @func reader.read(path[,cb])\n"
      "* @arg path {String}\n"
      "* @arg [cb] {Function}\n"
      "* @ret {uint} return read id\n"
    );
  }

  /**
   * @func reader.read_sync(path)
   * @arg path {String}
   * @ret {Buffer} return read Buffer
   */
  static void read_sync(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        "* @func reader.read_sync(path)\n"
        "* @arg path {String}\n"
        "* @ret {Buffer} return read Buffer\n"
      );
    }
    
    Buffer rv;
    try {
      rv = freader()->read_sync( worker->to_string_utf8(args[0]) );
    } catch(cError& err) {
      av8_throw_err(err);
    }
    av8_return( rv );
  }

  /**
   * @func reader.exists(path)
   * @arg path {String}
   * @ret {bool}
   */
  static void exists(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || !args[0]->IsString() ) {
      av8_throw_err(
        "* @func reader.exists(path)\n"
        "* @arg path {String}\n"
        "* @ret {bool}\n"
      );
    }
    av8_return( freader()->exists( worker->to_string_utf8(args[0]) ) );
  }
  
  /**
   * @func reader.ls(path)
   * @arg path {String}
   * @ret {Array}
   */
  static void ls(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || !args[0]->IsString() ) {
      av8_throw_err(
        "* @func reader.ls(path)\n"
        "* @arg path {String}\n"
        "* @ret {Array}\n"
      );
    }
    av8_return( freader()->ls( worker->to_string_utf8(args[0]) ) );
  }
  
  /**
   * @func reader.is_absolute(path)
   * @arg path {String}
   * @ret {bool}
   */
  static void is_absolute(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || ! args[0]->IsString() ) {
      av8_throw_err(
        "* @func reader.is_absolute(path)\n"
        "* @arg path {String}\n"
        "* @ret {bool}\n"
      );
    }
    av8_return( freader()->is_absolute( worker->to_string_utf8(args[0]) ) );
  }
  
  /**
   * @func reader.format(path)
   * @arg path {String}
   * @ret {String} return new path
   */
  static void format(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || !args[0]->IsString() ) {
      av8_throw_err(
        "* @func reader.format(path)\n"
        "* @arg path {String}\n"
        "* @ret {String} return new path\n"
      );
    }
    av8_return( freader()->format( worker->to_string_utf8(args[0]) ) );
  }
  
  /**
   * @func reader.abort(id)
   * @arg id {uint} abort id
   */
  static void abort(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() == 0 || ! args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func reader.abort(id)\n"
        "* @arg id {uint} abort id\n"
      );
    }
    freader()->abort( args[0].As<v8::Uint32>()->Value() );
  }
  
  /**
   * @func reader.clear()
   */
  static void clear(FunctionCall args) {
    freader()->clear();
  }
  
public:

  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_set_method(read, read);
    av8_set_method(read_stream, read_stream);
    av8_set_method(read_sync, read_sync);
    av8_set_method(exists, exists);
    av8_set_method(ls, ls);
    av8_set_method(is_absolute, is_absolute);
    av8_set_method(format, format);
    av8_set_method(abort, abort);
    av8_set_method(clear, clear);
  }
};

/**
 * @class NativeFS
 */
class NativeFS {
public:
  
  /**
   * @func chmod_sync(path[,mode])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @ret {bool}
   */

  /**
   * @func chmod(path[,mode[,cb]][,cb])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @arg [cb] {Function}
   */
  template<bool sync> static void chmod(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          "* @func chmod_sync(path[,mode])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @ret {bool}\n"
        );
      } else {
        av8_throw_err(
          
          "* @func chmod(path[,mode[,cb]][,cb])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    int args_index = 1;
    uint mode = FileHelper::default_mode;
    if (args.Length() > 1 && args[1]->IsUint32()) {
      mode = args[1].As<v8::Uint32>()->Value();
      args_index++;
    }
    if ( sync ) {
      av8_return( FileHelper::chmod_sync(worker->to_string_utf8(args[0]), mode) );
    } else {
      Callback cb;
      if ( args.Length() > args_index ) {
        cb = get_callback_for_none(worker, args[args_index]);
      }
      FileHelper::chmod(worker->to_string_utf8(args[0]), mode, cb);
    }
  }
  
  /**
   * @func chmod_r_sync(path[,mode])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @ret {bool}
   */

  /**
   * @func chmod_r(path[,mode[,cb]][,cb])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @arg [cb] {Function}
   * @ret {uint} return id
   */
  template<bool sync> static void chmod_r(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func chmod_r_sync(path[,mode])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func chmod_r(path[,mode[,cb]][,cb])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @arg [cb] {Function}\n"
          "* @ret {uint} return id\n"
          
        );
      }
    }
    int args_index = 1;
    uint mode = FileHelper::default_mode;
    if (args.Length() > 1 && args[1]->IsUint32()) {
      mode = args[1].As<v8::Uint32>()->Value();
      args_index++;
    }
    if ( sync ) {
      av8_return( FileHelper::chmod_r_sync(worker->to_string_utf8(args[0]), mode) );
    } else {
      Callback cb;
      if ( args.Length() > args_index ) {
        cb = get_callback_for_none(worker, args[args_index]);
      }
      av8_return( FileHelper::chmod_r(worker->to_string_utf8(args[0]), mode, cb) );
    }
  }
  
  /**
   * @func chown_sync(path, owner, group)
   * @arg path {String}
   * @arg owner {uint}
   * @arg group {uint}
   * @ret {bool}
   */

  /**
   * @func chown(path, owner, group[,cb])
   * @arg path {String}
   * @arg owner {uint}
   * @arg group {uint}
   * @arg [cb] {Function}
   */
  template<bool sync> static void chown(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 3 ||
        !args[0]->IsString() ||
        !args[1]->IsUint32() || !args[2]->IsUint32() ) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func chown_sync(path, owner, group)\n"
          "* @arg path {String}\n"
          "* @arg owner {uint}\n"
          "* @arg group {uint}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func chown(path, owner, group[,cb])\n"
          "* @arg path {String}\n"
          "* @arg owner {uint}\n"
          "* @arg group {uint}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    
    if ( sync ) {
      
      av8_return( FileHelper::chown_sync(worker->to_string_utf8(args[0]),
                                        args[1].As<v8::Uint32>()->Value(),
                                        args[2].As<v8::Uint32>()->Value()) );
    } else {
      Callback cb;
      if ( args.Length() > 3 ) {
        cb = get_callback_for_none(worker, args[3]);
      }
      FileHelper::chown(worker->to_string_utf8(args[0]),
                        args[1].As<v8::Uint32>()->Value(),
                        args[2].As<v8::Uint32>()->Value(), cb);
    }
  }
  
  /**
   * @func chown_r_sync(path, owner, group)
   * @arg path {String}
   * @arg owner {uint}
   * @arg group {uint}
   * @ret {bool}
   */

  /**
   * @func chown_r(path, owner, group[,cb])
   * @arg path {String}
   * @arg owner {uint}
   * @arg group {uint}
   * @arg [cb] {Function}
   * @ret {uint} return id
   */
  template<bool sync> static void chown_r(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 3 ||
        !args[0]->IsString() ||
        !args[1]->IsUint32() || !args[2]->IsUint32() ) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func chown_r_sync(path, owner, group)\n"
          "* @arg path {String}\n"
          "* @arg owner {uint}\n"
          "* @arg group {uint}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func chown_r(path, owner, group[,cb])\n"
          "* @arg path {String}\n"
          "* @arg owner {uint}\n"
          "* @arg group {uint}\n"
          "* @arg [cb] {Function}\n"
          "* @ret {uint} return id\n"
          
        );
      }
    }
    
    if ( sync ) {
      av8_return( FileHelper::chown_r_sync(worker->to_string_utf8(args[0]),
                                          args[1].As<v8::Uint32>()->Value(),
                                          args[2].As<v8::Uint32>()->Value()) );
    } else {
      Callback cb; 
      if ( args.Length() > 3 ) {
        cb = get_callback_for_none(worker, args[3]);
      }
      av8_return( FileHelper::chown_r(worker->to_string_utf8(args[0]),
                                     args[1].As<v8::Uint32>()->Value(),
                                     args[2].As<v8::Uint32>()->Value(), cb) );
    }
  }
  
  /**
   * @func mkdir_sync(path[,mode])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @ret {bool}
   */

  /**
   * @func mkdir(path[,mode[,cb]][,cb])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @arg [cb] {Function}
   */
  template<bool sync> static void mkdir(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func mkdir_sync(path[,mode])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func mkdir(path[,mode[,cb]][,cb])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    int args_index = 1;
    uint mode = FileHelper::default_mode;
    if (args.Length() > 1 && args[1]->IsUint32()) {
      mode = args[1].As<v8::Uint32>()->Value();
      args_index++;
    }
    if ( sync ) {
      av8_return( FileHelper::mkdir_sync(worker->to_string_utf8(args[0]), mode) );
    } else {
      Callback cb;
      if ( args.Length() > args_index ) {
        cb = get_callback_for_none(worker, args[args_index]);
      }
      FileHelper::mkdir(worker->to_string_utf8(args[0]), mode, cb);
    }
  }
  
  /**
   * @func mkdir_p_sync(path[,mode])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @ret {bool}
   */
  
  /**
   * @func mkdir_p(path[,mode[,cb]][,cb])
   * @arg path {String}
   * @arg [mode=default_mode] {uint}
   * @arg [cb] {Function}
   * @ret {uint} return id
   */
  template<bool sync> static void mkdir_p(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ){
        av8_throw_err(
          
          "* @func mkdir_p_sync(path[,mode])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func mkdir_p(path[,mode[,cb]][,cb])\n"
          "* @arg path {String}\n"
          "* @arg [mode=default_mode] {uint}\n"
          "* @arg [cb] {Function}\n"
          "* @ret {uint} return id\n"
          
        );
      }
    }
    int args_index = 1;
    uint mode = FileHelper::default_mode;
    if (args.Length() > 1 && args[1]->IsUint32()) {
      mode = args[1].As<v8::Uint32>()->Value();
      args_index++;
    }
    if ( sync ) {
      av8_return( FileHelper::mkdir_p_sync(worker->to_string_utf8(args[0]), mode) );
    } else {
      Callback cb;
      if ( args.Length() > args_index ) {
        cb = get_callback_for_none(worker, args[args_index]);
      }
      FileHelper::mkdir_p(worker->to_string_utf8(args[0]), mode, cb);
    }
  }

  /**
   * @func rename_sync(name,new_name)
   * @arg name {String}
   * @arg new_name {String}
   * @ret {bool}
   */

  /**
   * @func rename(name,new_name[,cb])
   * @arg name {String}
   * @arg new_name {String}
   * @arg [cb] {Function}
   */  
  template<bool sync> static void rename(FunctionCall args) {
    av8_worker(args);

    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func rename_sync(name,new_name)\n"
          "* @arg name {String}\n"
          "* @arg new_name {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func rename(name,new_name[,cb])\n"
          "* @arg name {String}\n"
          "* @arg new_name {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }

    if ( sync ) {
      av8_return( FileHelper::rename_sync(worker->to_string_utf8(args[0]),
                                          worker->to_string_utf8(args[1])) );
    } else {
      Callback cb;
      if ( args.Length() > 2 ) {
        cb = get_callback_for_none(worker, args[2]);
      }
      FileHelper::rename(worker->to_string_utf8(args[0]), worker->to_string_utf8(args[1]), cb);
    }
  }
  
  /**
   * @func unlink_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func unlink(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void unlink(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func unlink_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func unlink(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::unlink_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_none(worker, args[1]);
      }
      FileHelper::unlink(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func rmdir_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func rmdir(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void rmdir(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func rmdir_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func rmdir_sync(path)\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::rmdir_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_none(worker, args[1]);
      }
      FileHelper::rmdir(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func rm_r_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func rm_r(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   * @ret {uint} return id
   */
  template<bool sync> static void rm_r(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func rm_r_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func rm_r_sync(path)\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          "* @ret {uint} return id\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::rm_r_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_none(worker, args[1]);
      }
      av8_return( FileHelper::rm_r(worker->to_string_utf8(args[0]), cb) );
    }
  }
  
  /**
   * @func cp_sync(path, target)
   * @arg path {String}
   * @arg target {String}
   * @ret {bool}
   */
  
  /**
   * @func cp(path, target)
   * @arg path {String}
   * @arg target {String}
   * @arg [cb] {Function}
   * @ret {uint} return id
   */
  template<bool sync> static void cp(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func cp_sync(path, target)\n"
          "* @arg path {String}\n"
          "* @arg target {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func cp(path, target)\n"
          "* @arg path {String}\n"
          "* @arg target {String}\n"
          "* @arg [cb] {Function}\n"
          "* @ret {uint} return id\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::cp_sync(worker->to_string_utf8(args[0]),
                                      worker->to_string_utf8(args[1])) );
    } else {
      Callback cb;
      if ( args.Length() > 2 ) {
        cb = get_callback_for_none(worker, args[2]);
      }
      av8_return( FileHelper::cp(worker->to_string_utf8(args[0]),
                                 worker->to_string_utf8(args[1]), cb) );
    }
  }
  
  /**
   * @func cp_r_sync(path, target)
   * @arg path {String}
   * @arg target {String}
   * @ret {bool}
   */
  
  /**
   * @func cp_r(path, target)
   * @arg path {String}
   * @arg target {String}
   * @arg [cb] {Function}
   * @ret {uint} return id
   */
  template<bool sync> static void cp_r(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func cp_r_sync(path, target)\n"
          "* @arg path {String}\n"
          "* @arg target {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func cp_r(path, target)\n"
          "* @arg path {String}\n"
          "* @arg target {String}\n"
          "* @arg [cb] {Function}\n"
          "* @ret {uint} return id\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::cp_r_sync(worker->to_string_utf8(args[0]),
                                       worker->to_string_utf8(args[1])) );
    } else {
      Callback cb;
      if ( args.Length() > 2 ) {
        cb = get_callback_for_none(worker, args[2]);
      }
      av8_return( FileHelper::cp_r(worker->to_string_utf8(args[0]),
                                  worker->to_string_utf8(args[1]), cb) );
    }
  }
  
  /**
   * @func ls_sync(path)
   * @arg path {String}
   * @ret {Array} return Array<Dirent>
   */
  
  /**
   * @func ls(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void ls(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func ls_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {Array} return Array<Dirent>\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func ls(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::ls_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Array<Dirent>, Error>(worker, args[1]);
      }
      FileHelper::ls( worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func ls_l_sync(path)
   * @arg path {String}
   * @ret {Array} return Array<FileStat>
   */
  
  /**
   * @func ls_l(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void ls_l(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func ls_l_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {Array} return Array<FileStat>\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func ls_l(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::ls_l_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Array<FileStat>, Error>(worker, args[1]);
      }
      FileHelper::ls_l( worker->to_string_utf8(args[0]), cb);
    }
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
   * @func write_file_sync(path,buffer[,size[,offset]])
   * @arg path {String}
   * @arg buffer {String|Buffer|ArrayBuffer}
   * @arg [size=buffer.length] {uint64}
   * @arg [offset=-1] {int64}
   * @ret {bool}
   */

  /**
   * @func write_file(path,buffer[,size[,offset[,cb]][,cb]][,cb])
   * @arg path {String}
   * @arg buffer {String|Buffer|ArrayBuffer}
   * @arg [size=buffer.length] {uint64}
   * @arg [offset=-1] {int64}
   * @arg [cb] {Function}
   */
  static void write_file(FunctionCall args, bool sync) {
    av8_worker(args);
    
    if ( args.Length() < 2 || !args[0]->IsString() ||
        !(args[1]->IsString() || 
          args[1]->IsArrayBuffer() || 
          worker->has_buffer(args[1])
         )
    ) { // 参数错误
      if ( sync ) {
        av8_throw_err(
          
          "* @func write_file_sync(path,buffer[,size[,offset]])\n"
          "* @arg path {String}\n"
          "* @arg buffer {String|Buffer|ArrayBuffer}\n"
          "* @arg [size=buffer.length] {uint}\n"
          "* @arg [offset=-1] {int64}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func write_file(path,buffer[,size[,offset[,cb]][,cb]][,cb])\n"
          "* @arg path {String}\n"
          "* @arg buffer {String|Buffer|ArrayBuffer}\n"
          "* @arg [size=buffer.length] {uint}\n"
          "* @arg [offset=-1] {int64}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    
    Buffer keep;
    void*  buff = nullptr;
    String path = worker->to_string_utf8(args[0]);
    uint64 size = 0;
    int64  offset = -1;
    int args_index = 2;
    
    if ( args[1]->IsString() ) { // 写入字符串
      
      Encoding en = Encoding::utf8;
      if ( args.Length() > 2 ) { // 第三个参数为编码格式
        if ( args[2]->IsString() ) {
          if ( ! parse_encoding(args, args[2], en) ) {
            return;
          }
          args_index++;
        }
      }
      
      switch (en) {
        case Encoding::hex: // 解码 hex and base64
        case Encoding::base64:
          keep = Coder::decoding_to_byte(en, worker->to_string_ascii( args[1] ));
          break;
        default: // 编码
          keep = Coder::encoding(en, worker->to_string_ucs2(args[1]));
          break;
      }
      
      buff = keep.value();
      size = keep.length();
    }
    else {
      if ( args[1]->IsArrayBuffer() ) { // 写入原生 ArrayBuffer
        Local<v8::ArrayBuffer> ab = args[1].As<v8::ArrayBuffer>();
        v8::ArrayBuffer::Contents con = ab->GetContents();
        size = con.ByteLength();
        buff = con.Data();
      } else {
        Buffer* src = Wrap<Buffer>::unwrap(args[1]->ToObject())->self();
        size = src->length();
        buff = src->value();
        if ( !sync ) { // move data
          keep = *src;
        }
      }
      
      if ( args.Length() > 2 ) { // size
        if ( args[2]->IsInt32() ) {
          int num = args[2].As<v8::Int32>()->Value();
          size = av_min( num, size );
        }
        args_index++;
      }
      
      if ( !sync && args[1]->IsArrayBuffer() ) { // copy data
        keep = WeakBuffer((char*)buff, uint(size)).copy();
      }
    }
    
    if ( args.Length() > args_index ) {
      if ( args[args_index]->IsInt32() ) {
        offset = args[args_index].As<v8::Int32>()->Value();
        args_index++;
      }
    }
    
    if ( sync ) {
      av8_return( FileHelper::write_file_sync(path, buff, size, offset) );
    } else {
      Callback cb;
      if ( args.Length() > args_index ) {
        cb = get_callback_for_buffer(worker, args[args_index]);
      }
      FileHelper::write_file(path, keep, size, offset, cb);
    }
  }
  
  static void write_file_sync(FunctionCall args) {
    write_file(args, true);
  }
  
  static void write_file(FunctionCall args) {
    write_file(args, false);
  }
  
  /**
   * @func read_file_sync(path[,size[,offset]])
   * @arg [size=-1] {int64}
   * @arg [offset=-1] {int64}
   * @ret {Buffer} return file buffer
   */

  /**
   * @func read_file(path[,size[,offset[,cb]][,cb]][,cb])
   * @arg [size=-1] {int64}
   * @arg [offset=-1] {int64}
   * @arg [cb] {Function}
   */  
  static void read_file(FunctionCall args, bool sync) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func read_file_sync(path[,size[,offset]])\n"
          "* @arg [size=-1] {int64}\n"
          "* @arg [offset=-1] {int64}\n"
          "* @ret {Buffer} return file buffer\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func read_file(path[,size[,offset[,cb]][,cb]][,cb])\n"
          "* @arg [size=-1] {int64}\n"
          "* @arg [offset=-1] {int64}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    String path = worker->to_string_utf8(args[0]);
    int64 length = -1;
    int64 offset = -1;
    int   args_index = 1;
    
    if ( args.Length() > args_index ) {
      if ( args[args_index]->IsInt32() ) {
        uint length_ = args[args_index].As<v8::Int32>()->Value();
        length = av_min(length_, length);
        args_index++;
      }
    }
    if ( args.Length() > args_index ) {
      if ( args[args_index]->IsInt32() ) {
        uint offset_ = args[args_index].As<v8::Int32>()->Value();
        offset = av_min(offset_, offset);
        args_index++;
      }
    }
    
    if ( sync ) {
      av8_return( FileHelper::read_file_sync(path, length, offset) );
    } else {
      Callback cb;
      if ( args.Length() > args_index ) {
        cb = get_callback_for_buffer(worker, args[args_index]);
      }
      FileHelper::read_file(path, length, offset, cb);
    }
  }
  
  static void read_file_sync(FunctionCall args) {
    read_file(args, 1);
  }
  
  static void read_file(FunctionCall args) {
    read_file(args, 0);
  }
  
  /**
   * @func is_file_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func is_file(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void is_file(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func is_file_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func is_file(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::is_file_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Bool, Error>(worker, args[1]);
      }
      FileHelper::is_file(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func is_dir_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func is_dir(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void is_dir(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func is_dir_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func is_dir(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::is_dir_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Bool, Error>(worker, args[1]);
      }
      FileHelper::is_dir(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func stat_sync(path)
   * @arg path {String}
   * @ret {FileStat}
   */
  
  /**
   * @func stat(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void stat(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func stat_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {FileStat}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func stat(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::stat_sync( worker->to_string_utf8(args[0]) ) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<FileStat, Error>(worker, args[1]);
      }
      FileHelper::is_dir(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func exists_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func exists(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void exists(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func exists_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func exists(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::exists_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Bool, Error>(worker, args[1]);
      }
      FileHelper::exists(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func readable_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func readable(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void readable(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func readable_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func readable(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::readable_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Bool, Error>(worker, args[1]);
      }
      FileHelper::readable(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func writable_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func writable(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void writable(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func writable_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func writable(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::writable_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Bool, Error>(worker, args[1]);
      }
      FileHelper::writable(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func executable_sync(path)
   * @arg path {String}
   * @ret {bool}
   */

  /**
   * @func executable(path[,cb])
   * @arg path {String}
   * @arg [cb] {Function}
   */
  template<bool sync> static void executable(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsString()) {
      if ( sync ) {
        av8_throw_err(
          
          "* @func executable_sync(path)\n"
          "* @arg path {String}\n"
          "* @ret {bool}\n"
          
        );
      } else {
        av8_throw_err(
          
          "* @func executable(path[,cb])\n"
          "* @arg path {String}\n"
          "* @arg [cb] {Function}\n"
          
        );
      }
    }
    if ( sync ) {
      av8_return( FileHelper::executable_sync(worker->to_string_utf8(args[0])) );
    } else {
      Callback cb;
      if ( args.Length() > 1 ) {
        cb = get_callback_for_t<Bool, Error>(worker, args[1]);
      }
      FileHelper::executable(worker->to_string_utf8(args[0]), cb);
    }
  }
  
  /**
   * @func abort(id) abort async io
   * @arg id {uint}
   */
  static void abort(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsUint32()) {
      av8_throw_err(
        
        "* @func abort(id) abort async io\n"
        "* @arg id {uint}\n"
        
      );
    }
    FileHelper::abort( args[0].As<v8::Uint32>()->Value() );
  }

  /**
   * @func read_file_stream(path[,offset[,cb]][,cb])
   * @arg [offset=-1] {int64}
   * @arg [cb] {Function}
   */
  static void read_file_stream(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsString()) {
      av8_throw_err(
        
        "* @func read_file_stream(path[,offset[,cb]][,cb])\n"
        "* @arg [offset=-1] {int64}\n"
        "* @arg [cb] {Function}\n"
        
      );
    }
    String path = worker->to_string_utf8(args[0]);
    int64 offset = -1;
    int   args_index = 1;
    
    if ( args.Length() > 1 ) {
      if ( args[1]->IsInt32() ) {
        uint offset_ = args[1].As<v8::Int32>()->Value();
        offset = av_min(offset_, offset);
        args_index++;
      }
    }
    
    Callback cb;
    if ( args.Length() > args_index ) {
      cb = get_callback_for_io_stream(worker, args[args_index]);
    }
    av8_return( FileHelper::read_file_stream(path, offset, cb) );
  }
  
  static void binding(v8::Handle<v8::Object> exports, Worker* worker) {
    WrapFileStat::binding(exports, worker);
    WrapFile::binding(exports, worker);
    //
    av8_set_field(FOPEN_READ, FOPEN_READ);
    av8_set_field(FOPEN_WRITE, FOPEN_WRITE);
    av8_set_field(FOPEN_APPEND, FOPEN_APPEND);
    av8_set_field(FOPEN_READ_PLUS, FOPEN_READ_PLUS);
    av8_set_field(FOPEN_WRITE_PLUS, FOPEN_WRITE_PLUS);
    av8_set_field(FOPEN_APPEND_PLUS, FOPEN_APPEND_PLUS);
    av8_set_field(FOPEN_R, FOPEN_R);
    av8_set_field(FOPEN_W, FOPEN_W);
    av8_set_field(FOPEN_A, FOPEN_A);
    av8_set_field(FOPEN_RP, FOPEN_RP);
    av8_set_field(FOPEN_WP, FOPEN_WP);
    av8_set_field(FOPEN_AP, FOPEN_AP);
    //
    av8_set_field(FILE_UNKNOWN, FILE_UNKNOWN);
    av8_set_field(FILE_FILE, FILE_FILE);
    av8_set_field(FILE_DIR, FILE_DIR);
    av8_set_field(FILE_LINK, FILE_LINK);
    av8_set_field(FILE_FIFO, FILE_FIFO);
    av8_set_field(FILE_SOCKET, FILE_SOCKET);
    av8_set_field(FILE_CHAR, FILE_CHAR);
    av8_set_field(FILE_BLOCK, FILE_BLOCK);
    //
    av8_set_field(DEFAULT_MODE, FileHelper::default_mode);
    // sync
    av8_set_method(chmod_sync, chmod<true>);
    av8_set_method(chmod_r_sync, chmod_r<true>);
    av8_set_method(chown_sync, chown<true>);
    av8_set_method(chown_r_sync, chown_r<true>);
    av8_set_method(mkdir_sync, mkdir<true>);
    av8_set_method(mkdir_p_sync, mkdir_p<true>);
    av8_set_method(rename_sync, rename<true>);
    av8_set_method(mv_sync, rename<true>);
    av8_set_method(unlink_sync, unlink<true>);
    av8_set_method(rmdir_sync, rmdir<true>);
    av8_set_method(rm_r_sync, rm_r<true>);
    av8_set_method(cp_sync, cp<true>);
    av8_set_method(cp_r_sync, cp_r<true>);
    av8_set_method(ls_sync, ls<true>);
    av8_set_method(ls_l_sync, ls_l<true>);
    av8_set_method(write_file_sync, write_file_sync);
    av8_set_method(read_file_sync, read_file_sync);
    av8_set_method(is_file_sync, is_file<true>);
    av8_set_method(is_dir_sync, is_dir<true>);
    av8_set_method(stat_sync, stat<true>);
    av8_set_method(exists_sync, exists<true>);
    av8_set_method(readable_sync, readable<true>);
    av8_set_method(writable_sync, writable<true>);
    av8_set_method(executable_sync, executable<true>);
    // async
    av8_set_method(abort, abort);
    av8_set_method(chmod, chmod<false>);
    av8_set_method(chown, chown<false>);
    av8_set_method(mkdir, mkdir<false>);
    av8_set_method(mkdir_p, mkdir_p<false>);
    av8_set_method(rename, rename<false>);
    av8_set_method(mv, rename<false>);
    av8_set_method(unlink, unlink<false>);
    av8_set_method(rmdir, rmdir<false>);
    av8_set_method(ls, ls<false>);
    av8_set_method(is_file, is_file<false>);
    av8_set_method(is_dir, is_dir<false>);
    av8_set_method(stat, stat<false>);
    av8_set_method(exists, exists<false>);
    av8_set_method(readable, readable<false>);
    av8_set_method(writable, writable<false>);
    av8_set_method(executable, executable<false>);
    av8_set_method(chmod_r, chmod_r<false>);
    av8_set_method(chown_r, chown_r<false>);
    av8_set_method(rm_r, rm_r<false>);
    av8_set_method(cp, cp<false>);
    av8_set_method(cp_r, cp_r<false>);
    av8_set_method(ls_l, ls_l<false>);
    av8_set_method(write_file, write_file);
    av8_set_method(read_file, read_file);
    av8_set_method(read_file_stream, read_file_stream);
    // reader
    Local<v8::Object> obj = v8::Object::New(worker->isolate());
    NativeFileReader::binding(obj, worker);
    av8_set_field(reader, obj.As<Value>());
  }
};

class NativePath {

  /**
   * @func basename(path)
   * @arg path {String}
   * @ret {String}
   */
  static void basename(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( v8::String::Empty(worker->isolate()) );
    }
    av8_return( Path::basename( worker->to_string_utf8(args[0])) );
  }

  /**
   * @func dirname(path)
   * @arg path {String}
   * @ret {String}
   */
  static void dirname(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( v8::String::Empty(worker->isolate()) );
    }
    av8_return( Path::dirname( worker->to_string_utf8(args[0])) );
  }

  /**
   * @func extname(path)
   * @arg path {String}
   * @ret {String}
   */
  static void extname(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( v8::String::Empty(worker->isolate()) );
    }
    av8_return( Path::extname( worker->to_string_utf8(args[0])) );
  }

  /**
   * @func executable()
   * @ret {String}
   */
  static void executable(FunctionCall args) {
    av8_worker(args);
    av8_return( Path::executable() );
  }

  /**
   * @func documents([path])
   * @arg path {String}
   * @ret {String}
   */
  static void documents(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( Path::documents() );
    }
    av8_return( Path::documents( worker->to_string_utf8(args[0])) );
  }

  /**
   * @func temp([path])
   * @arg path {String}
   * @ret {String}
   */
  static void temp(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( Path::temp() );
    }
    av8_return( Path::temp( worker->to_string_utf8(args[0])) );
  }

  /**
   * @func resources([path])
   * @arg path {String}
   * @ret {String}
   */
  static void resources(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( Path::resources() );
    }
    av8_return( Path::resources( worker->to_string_utf8(args[0])) );
  }

  /**
   * @func is_local_absolute(path)
   * @arg path {String}
   * @ret {String}
   */
  static void is_local_absolute(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( false );
    }
    av8_return( Path::is_local_absolute(worker->to_string_utf8(args[0])) );
  }

  /**
   * @func restore(path)
   * @arg path {String}
   * @ret {String}
   */
  static void restore(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_throw_err(
        
        "* @func restore(path)\n"
        "* @arg path {String}\n"
        "* @ret {String}\n"
        
      );
    }
    av8_return( Path::restore(worker->to_string_utf8(args[0])) );
  }

  /**
   * @func cwd()
   * @ret {String}
   */
  static void cwd(FunctionCall args) {
    av8_worker(args);
    av8_return( Path::cwd() );
  }

  /**
   * @func set_cwd(path)
   * @arg path {String}
   * @ret {bool}
   */
  static void set_cwd(FunctionCall args) {
    av8_worker(args);
    if (args.Length() == 0 || !args[0]->IsString()) {
      av8_return( false );
    }
    av8_return( Path::set_cwd(worker->to_string_utf8(args[0])) );
  }
  
public:
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_set_method(basename, basename);
    av8_set_method(dirname, dirname);
    av8_set_method(extname, extname);
    av8_set_method(executable, executable);
    av8_set_method(documents, documents);
    av8_set_method(temp, temp);
    av8_set_method(resources, resources);
    av8_set_method(is_local_absolute, is_local_absolute);
    av8_set_method(restore, restore);
    av8_set_method(cwd, cwd);
    av8_set_method(set_cwd, set_cwd);
  }
};

av8_reg_module(_fs, NativeFS);
av8_reg_module(_path, NativePath);
av8_end
