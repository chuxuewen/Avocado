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

#include "fs.h"
#include "zlib.h"
#include "handle.h"
#include "error.h"
#include "http.h"

av_ns(avocado)

String inl__format_part_path(cString& path);

class FileReader::Core {
public:
  
  enum Protocol {
    FILE = 0,
    ZIP,
    HTTP,
    HTTPS,
    FTP,
    FTPS,
    Unknown,
  };
  
  ~Core() {
    ScopeLock lock(zip_mutex_);
    for (auto i = zips_.begin(), e = zips_.end(); i != e; i++) {
      Release(i.value());
    }
  }
  
  Protocol protocol(cString& path) {
    if ( Path::is_local_file( path ) ) {
      return FILE;
    }
    if ( Path::is_local_zip( path ) ) {
      return ZIP;
    }
    if ((path[0] == 'h' || path[0] == 'H') &&
        (path[1] == 't' || path[1] == 'T') &&
        (path[2] == 't' || path[2] == 'T') &&
        (path[3] == 'p' || path[3] == 'P')) {
      if (path[4] == ':' &&
          path[5] == '/' &&
          path[6] == '/') {
        return HTTP;
      }
      if ((path[4] == 's' || path[4] == 'S') &&
          path[5] == ':' &&
          path[6] == '/' &&
          path[7] == '/') {
        return HTTPS;
      }
    }
    if ((path[0] == 'f' || path[0] == 'F') &&
        (path[1] == 't' || path[1] == 'T') &&
        (path[2] == 'p' || path[2] == 'P')) {
      if (path[3] == ':' &&
          path[4] == '/' &&
          path[5] == '/') {
        return FTP;
      }
      if ((path[3] == 's' || path[3] == 'S') &&
          path[4] == ':' &&
          path[5] == '/' &&
          path[6] == '/') {
        return FTPS;
      }
    }
    return Unknown;
  }
  
  String zip_path(cString& path) {
    int  i = path.index_of('@');
    if (i != -1) {
      return path.substr(0, i);
    }
    return String::empty;
  }
  
  ZipReader* zip_reader(cString& path) av_def_err {
    ZipReader* reader = zips_.get(path);
    if (reader) {
      return reader;
    }
    reader = new ZipReader(path);
    av_assert_err( reader->open(), ERR_FILE_NOT_EXISTS, "Cannot open zip file, `%s`", *path);
    zips_.set(path, reader);
    return reader;
  }
  
  /**
   * @func read
   */
  uint read(cString& path, Callback cb, bool stream) {
    
    Protocol p = protocol(path);
    uint id = 0;
    
    PostMessage* loop = RunLoop::current();
    
    switch (p) {
      default:
      case FILE: {
        if ( stream ) {
          id = FileHelper::read_file_stream(path, cb);
        } else {
          FileHelper::read_file(path, cb);
        }
        break;
      }
      case ZIP: {
        String zip = zip_path(path);
        if ( zip.is_empty() ) {
          async_err_callback(cb, Error("Invalid file path, \"%s\"", *path), loop);
        } else {
          
          RunLoop::post_io(Callback([loop, zip, this, path, cb, stream](SimpleEvent& evt) {
            Buffer buffer;
            ScopeLock lock(zip_mutex_);
            try {
              ZipReader* read = zip_reader(zip);
              String inl_path = inl__format_part_path(path.substr(zip.length() + 1));
              
              if ( read->jump(inl_path) ) {
                buffer = read->read();
              } else {
                Error err("Zip package internal file does not exist, %s", *path);
                async_err_callback(cb, move(err), loop); return;
              }
            } catch (cError& err) {
              Error e(err);
              async_err_callback(cb, Error(err), loop); return;
            }
            
            if ( stream ) {
              uint len = buffer.length();
              async_callback(cb, IOStreamData(buffer, 1, 0, len, len, nullptr), loop);
            } else {
              async_callback(cb, move(buffer), loop);
            }
          }));
        }
        break;
      }
      case FTP:
      case FTPS:
        async_err_callback(cb, Error(ERR_NOT_SUPPORTED_FILE_PROTOCOL, "This file protocol is not supported"), loop);
        break;
      case HTTP:
      case HTTPS:
        try {
          if ( stream ) {
            id = HttpHelper::get_stream(path, cb);
          } else {
            id = HttpHelper::get(path, cb);
          }
        } catch(cError& err) {
          async_err_callback(cb, Error(err), loop);
        }
        break;
    }
    return id;
  }
  
  /**
   * @func read_sync
   */
  Buffer read_sync(cString& path) av_def_err {
    Buffer rv;
    
    switch ( protocol(path) ) {
      default:
      case FILE:
        av_assert_err(FileHelper::exists_sync(path),
                      ERR_FILE_NOT_EXISTS, "Unable to read file contents, \"%s\"", *path);
        rv = FileHelper::read_file_sync(path);
        break;
      case ZIP: {
        String zip = zip_path(path);
        av_assert_err(!zip.is_empty(), ERR_FILE_NOT_EXISTS, "Invalid file path, \"%s\"", *path);
        
        ScopeLock lock(zip_mutex_);
        
        ZipReader* read = zip_reader(zip);
        String inl_path = inl__format_part_path( path.substr(zip.length() + 1) );
        
        if ( read->jump(inl_path) ) {
          rv = read->read();
        } else {
          av_throw(ERR_ZIP_IN_FILE_NOT_EXISTS, "Zip package internal file does not exist, %s", *path);
        }
        break;
      }
      case FTP:
      case FTPS:
        av_throw(ERR_NOT_SUPPORTED_FILE_PROTOCOL, "This file protocol is not supported");
        break;
      case HTTP:
      case HTTPS: rv = HttpHelper::get_sync(path); break;
    }
    return rv;
  }
  
  /**
   * @func abort
   */
  void abort(uint id) {
    AsyncIOTask::safe_abort(id);
  }
  
  /**
   * @func exists
   */
  bool exists(cString& path) {
    switch ( protocol(path) ) {
      default:
      case FILE:
        return FileHelper::exists_sync(path);
      case ZIP: {
        String zip = zip_path(path);
        if ( zip.is_empty() ) return false;
        
        av_ignore_err({
          ScopeLock lock(zip_mutex_);
          ZipReader* read = zip_reader(zip);
          String inl_path = inl__format_part_path( path.substr(zip.length() + 1) );
          return read->exists( inl_path );
        });
        return false;
      }
      case FTP:
      case FTPS:
      case HTTP:
      case HTTPS: return false;
    }
    return false;
  }
  
  Array<Dirent> ls(cString& path) {
    Array<Dirent> rv;
    
    switch ( protocol(path) ) {
      default:
      case FILE:
        return FileHelper::ls_sync(path);
      case ZIP: {
        String zip = zip_path(path);
        
        if ( zip.is_empty() ) {
          return rv;
        }
        
        av_ignore_err({
          ScopeLock lock(zip_mutex_);
          ZipReader* read = zip_reader(zip);
          String inl_path = inl__format_part_path( path.substr(zip.length() + 1) );
          return read->ls(inl_path);
        });
        
        return rv;
      }
      case FTP:
      case FTPS:
      case HTTP:
      case HTTPS: return rv;
    }
    return rv;
  }
  
  /**
   * @func format
   */
  String format(cString& path) {
    int index = -1;
    switch ( protocol(path) ) {
      default:
      case ZIP:
      case FILE: return Path::format("%s", *path);
      case HTTP: index = path.index_of('/', 8); break;
      case HTTPS:index = path.index_of('/', 9); break;
      case FTP:  index = path.index_of('/', 7); break;
      case FTPS: index = path.index_of('/', 8); break;
    }
    if (index == -1) {
      return path;
    }
    String s = inl__format_part_path(path.substr(index));
    if (s.is_empty()) {
      return path.substr(0, index);
    } else {
      return path.substr(0, index + 1) + s;
    }
  }
  
  bool is_absolute(cString& path) {
    
    if ( Path::is_local_absolute(path) ) {
      return true;
    } else {
      switch ( protocol(path) ) {
        case ZIP: 
        case FILE: 
        case HTTP: 
        case HTTPS: 
        case FTP: 
        case FTPS: return true;
        default: return false;
      }
    }
  }
  
  void clear() {
    ScopeLock lock(zip_mutex_);
    for ( auto& i: zips_ ) {
      Release(i.value());
    }
    zips_.clear();
  }
  
private:
  Mutex zip_mutex_;
  Map<String, ZipReader*> zips_;
};

/**
 * @constructor
 */
FileReader::FileReader(): core_(new Core()) { }

/**
 * @constructor
 */
FileReader::FileReader(FileReader&& reader): core_(reader.core_) {
  reader.core_ = nullptr;
}

/**
 * @destructor
 */
FileReader::~FileReader() {
  delete core_;
  core_ = nullptr;
}

uint FileReader::read(cString& path, Callback cb) {
  return core_->read(path, cb, false);
}
uint FileReader::read_stream(cString& path, Callback cb) {
  return core_->read(path, cb, true);
}
Buffer FileReader::read_sync(cString& path) av_def_err {
  return core_->read_sync(path);
}
void FileReader::abort(uint id) {
  core_->abort(id);
}
bool FileReader::exists(cString& path) {
  return core_->exists(path);
}
Array<Dirent> FileReader::ls(cString& path) {
  return core_->ls(path);
}
String FileReader::format(cString& path) {
  return core_->format(path);
}
bool FileReader::is_absolute(cString& path) {
  return core_->is_absolute(path);
}
void FileReader::clear() {
  return core_->clear();
}

static FileReader* shared_instance = nullptr;

/**
 * @func set_to_share
 */
void FileReader::set_shared_instance(FileReader* reader) {
  if (shared_instance != reader) {
    Release(shared_instance);
    shared_instance = reader;
  }
}

/**
 * @func share
 */
FileReader* FileReader::shared() {
  if ( !shared_instance ) {
    shared_instance = new FileReader();
  }
  return shared_instance;
}

av_end
