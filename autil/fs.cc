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

#include <errno.h>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include "fs.h"
#include "zlib.h"
#include "handle.h"
#include "error.h"
#include "thread.h"
#include <uv.h>

#if av_win
  #include <io.h>
  #include <direct.h>
#else
  #include <unistd.h>
#endif

av_ns(avocado)

#include "uv-1.h"

extern uv_loop_t* _uv_loop();

av_def_inl_members(FileStat, Inl) {
public:
  void set_m_stat(uv_stat_t* stat) {
    if ( !m_stat ) {
      m_stat = malloc(sizeof(uv_stat_t));
    }
    memcpy(m_stat, stat, sizeof(uv_stat_t));
  }
};

FileStat::FileStat(): m_stat(nullptr) { }

FileStat::FileStat(cString& path): FileStat(FileHelper::stat_sync(path)) { }

FileStat::FileStat(FileStat&& stat): m_stat(stat.m_stat) {
  stat.m_stat = nullptr;
}

FileStat& FileStat::operator=(FileStat&& stat) {
  if ( m_stat ) {
    free(m_stat);
  }
  stat.m_stat = stat.m_stat;
  return *this;
}

FileStat::~FileStat() {
  free(m_stat);
}

uv_stat_t stat;

#define STAT (m_stat ? (uv_stat_t*)m_stat : &stat)

bool FileStat::is_valid() const { return m_stat; }
bool FileStat::is_file() const { return S_ISREG(STAT->st_mode); }
bool FileStat::is_dir() const { return S_ISDIR(STAT->st_mode); }
bool FileStat::is_link() const { return S_ISLNK(STAT->st_mode); }
bool FileStat::is_sock() const { return S_ISSOCK(STAT->st_mode); }
uint64 FileStat::mode() const { return STAT->st_mode; }

FileType FileStat::type() const {
  if ( m_stat ) {
    if ( S_ISREG(((uv_stat_t*)m_stat)->st_mode) ) return FILE_FILE;
    if ( S_ISDIR(((uv_stat_t*)m_stat)->st_mode) ) return FILE_DIR;
    if ( S_ISLNK(((uv_stat_t*)m_stat)->st_mode) ) return FILE_LINK;
    if ( S_ISFIFO(((uv_stat_t*)m_stat)->st_mode) ) return FILE_FIFO;
    if ( S_ISSOCK(((uv_stat_t*)m_stat)->st_mode) ) return FILE_SOCKET;
    if ( S_ISCHR(((uv_stat_t*)m_stat)->st_mode) ) return FILE_CHAR;
    if ( S_ISBLK(((uv_stat_t*)m_stat)->st_mode) ) return FILE_BLOCK;
  }
  return FILE_UNKNOWN;
}

uint64 FileStat::group() const { return STAT->st_gid; }
uint64 FileStat::owner() const { return STAT->st_uid; }
uint64 FileStat::nlink() const { return STAT->st_nlink; }
uint64 FileStat::ino() const { return STAT->st_ino; }
uint64 FileStat::blksize() const { return STAT->st_blksize; }
uint64 FileStat::blocks() const { return STAT->st_blocks; }
uint64 FileStat::flags() const { return STAT->st_flags; }
uint64 FileStat::gen() const { return STAT->st_gen; }
uint64 FileStat::dev() const { return STAT->st_dev; }
uint64 FileStat::rdev() const { return STAT->st_rdev; }
uint64 FileStat::size() const { return STAT->st_size; }
uint64 FileStat::atime() const {
  return (int64)STAT->st_atim.tv_sec * 1000000 + STAT->st_atim.tv_nsec / 1000;
}
uint64 FileStat::mtime() const {
  return (int64)STAT->st_mtim.tv_sec * 1000000 + STAT->st_mtim.tv_nsec / 1000;
}
uint64 FileStat::ctime() const {
  return (int64)STAT->st_ctim.tv_sec * 1000000 + STAT->st_ctim.tv_nsec / 1000;
}
uint64 FileStat::birthtime() const {
  return (int64)STAT->st_birthtim.tv_sec * 1000000 + STAT->st_birthtim.tv_nsec / 1000;
}

#undef STAT

/**
 * C file mode mask
 */
const int c_file_mask[FOPEN_NUM] = {
  O_RDONLY,                           // r
  O_WRONLY | O_CREAT | O_TRUNC,       // w
  O_WRONLY | O_CREAT | O_APPEND,      // a
  O_RDWR,                             // r+
  O_RDWR | O_CREAT | O_TRUNC ,        // w+
  O_RDWR | O_CREAT | O_APPEND         // a+
};

File::~File() {
  close();
}

bool File::is_open() {
  return m_fp;
}

bool File::open(FileOpenMode mode) {
  if ( m_fp ) { // 文件已经打开
    av_err( "file already open" );
    return false;
  }
  uv_fs_t req;
  m_fp = uv_fs_open(uv_default_loop(), &req, *m_path, c_file_mask[mode], 0, nullptr);
  if ( m_fp ) {
    return true;
  }
  return false;
}

bool File::close() {
  if ( m_fp ) {
    uv_fs_t req;
    if ( uv_fs_close(uv_default_loop(), &req, m_fp, nullptr) == 0 ) {
      m_fp = 0;
    } else {
      return false;
    }
  }
  return true;
}

int File::read(void* buffer, uint64 size, int64 offset) {
  uv_fs_t req;
  uv_buf_t buf;
  buf.base = (char*)buffer;
  buf.len = size;
  return uv_fs_read(uv_default_loop(), &req, m_fp, &buf, 1, offset, nullptr);
}

int File::write(const void* buffer, uint64 size, int64 offset) {
  uv_fs_t req;
  uv_buf_t buf;
  buf.base = (char*)buffer;
  buf.len = size;
  return uv_fs_write(uv_default_loop(), &req, m_fp, &buf, 1, offset, nullptr);
}

// ----------------------------------- FileAsync -----------------------------------------

struct FileStreamData {
  Buffer buffer;
  int mark;
};

typedef UVRequestWrap<uv_fs_t, AsyncFile::Inl> FileReq;
typedef UVRequestWrap<uv_fs_t, AsyncFile::Inl, FileStreamData> FileStreamReq;

static AsyncFile::Delegate default_delegate;

class AsyncFile::Inl: public Reference {
public:
  
  typedef AsyncFile::Delegate Delegate;
  
  void set_delegate(Delegate* delegate) {
    if ( delegate ) {
      m_delegate = delegate;
    } else {
      m_delegate = &default_delegate;
    }
  }
  
  Inl(AsyncFile* host, cString& path, RunLoop* loop)
  : m_path(path)
  , m_fp(0), m_loop(loop ? loop->keep_alive() : nullptr)
  , m_delegate(nullptr)
  , m_host(host)
  { //
  }
  
  virtual ~Inl() {
    if ( m_fp ) {
      uv_fs_t req;
      int res = uv_fs_close(_uv_loop(), &req, m_fp, nullptr); // sync
      av_assert( res == 0 );
    }
    Release( m_loop );
  }
  
  inline int fd() { return m_fp; }
  
  static inline PostMessage* loop(uv_fs_t* req) {
    return FileReq::cast(req)->ctx()->m_loop;
  }
  
  static inline Delegate* del(Object* ctx) {
    return static_cast<FileReq*>(ctx)->ctx()->m_delegate;
  }
  
  static inline AsyncFile* host(Object* ctx) {
    return static_cast<FileReq*>(ctx)->ctx()->m_host;
  }

  void fs_open_err_cb3(SimpleEvent& evt) {
    m_delegate->trigger_async_file_error(m_host, *evt.error);
  }
  
  //void fs_close_err_cb_from_loop(SimpleEvent& evt) {
  //  m_delegate->trigger_async_file_error(m_host, *evt.error);
  //}
  
  static void fs_open_cb_from_loop(SimpleEvent& evt, FileReq* ctx) {
    Handle<FileReq> handle(ctx);
    uv_fs_t* req = handle->req();
    if ( req->result > 0 ) {
      if ( handle->ctx()->m_fp ) {
        uv_fs_t close_req;
        uv_fs_close(req->loop, &close_req, req->result, nullptr); // sync
        Error err(ERR_FILE_ALREADY_OPEN, "file already open");
        del(ctx)->trigger_async_file_error(host(ctx), err);
      } else {
        handle->ctx()->m_fp = req->result;
        del(ctx)->trigger_async_file_open(host(ctx));
      }
    } else {
      Error err(req->result, "%s, %s", uv_err_name(req->result), uv_strerror(req->result));
      del(ctx)->trigger_async_file_error(host(ctx), err);
    }
  }
  
  static void fs_open_cb(uv_fs_t* req) {
    uv_fs_req_cleanup(req);
    async_callback(Callback(&fs_open_cb_from_loop, FileReq::cast(req)), loop(req));
  }
  
  static void fs_close_cb_from_loop(SimpleEvent& evt, FileReq* ctx) {
    Handle<FileReq> handle(ctx);
    uv_fs_t* req = handle->req();
    if ( req->result == 0 ) { // ok
      handle->ctx()->m_fp = 0;
      del(ctx)->trigger_async_file_close(host(ctx));
    } else {
      Error err(req->result, "%s, %s", uv_err_name(req->result), uv_strerror(req->result));
      del(ctx)->trigger_async_file_error(host(ctx), err);
    }
  }
  
  static void fs_close_cb(uv_fs_t* req) {
    uv_fs_req_cleanup(req);
    async_callback(Callback(&fs_close_cb_from_loop, FileReq::cast(req)), loop(req));
  }
  
  static void fs_read_cb_from_loop(SimpleEvent& evt, FileStreamReq* ctx) {
    Handle<FileStreamReq> handle(ctx);
    uv_fs_t* req = handle->req();
    if ( req->result < 0 ) {
      Error err(req->result, "%s, %s", uv_err_name(req->result), uv_strerror(req->result));
      del(ctx)->trigger_async_file_error(host(ctx), err);
    } else {
      del(ctx)->trigger_async_file_read(host(ctx),
                                        Buffer(handle->data().buffer.collapse(), req->result),
                                        handle->data().mark );
    }
  }
  
  static void fs_read_cb(uv_fs_t* req) {
    uv_fs_req_cleanup(req);
    async_callback(Callback(&fs_read_cb_from_loop, FileStreamReq::cast(req)), loop(req));
  }
  
  static void fs_write_cb_from_loop(SimpleEvent& evt, FileStreamReq* ctx) {
    Handle<FileStreamReq> handle(ctx);
    uv_fs_t* req = handle->req();
    if ( req->result != handle->data().buffer.length() ) {
      Error err(req->result, "%s, %s", uv_err_name(req->result), uv_strerror(req->result));
      del(ctx)->trigger_async_file_error(host(ctx), err);
    } else {
      del(ctx)->trigger_async_file_write(host(ctx), handle->data().buffer, handle->data().mark);
    }
  }
  
  static void fs_write_cb(uv_fs_t* req) {
    uv_fs_req_cleanup(req);
    async_callback(Callback(&fs_write_cb_from_loop, FileStreamReq::cast(req)), loop(req));
  }
  
  String      m_path;
  int         m_fp;
  LoopProxy*  m_loop;
  Delegate*   m_delegate;
  AsyncFile*  m_host;
};

AsyncFile::AsyncFile(cString& path, RunLoop* loop)
: m_inl( NewRetain<AsyncFile::Inl>(this, path, loop) )
{
}

AsyncFile::~AsyncFile() {
  m_inl->set_delegate(nullptr);
  if ( is_open() ) {
    close();
  }
  m_inl->release();
}

String AsyncFile::path() const { return m_inl->m_path; }

void AsyncFile::set_delegate(Delegate* delegate) {
  m_inl->set_delegate(delegate);
}

bool AsyncFile::is_open() {
  return m_inl->m_fp;
}

void AsyncFile::open(FileOpenMode mode) {
  if ( m_inl->m_fp ) {
    Error e(ERR_FILE_ALREADY_OPEN, "File already open");
    async_err_callback(Callback(&AsyncFile::Inl::fs_open_err_cb3, m_inl), move(e), m_inl->m_loop);
  } else {
    auto req = new FileReq(m_inl);
    uv_fs_open(_uv_loop(), req->req(),
               Path::restore_c(m_inl->m_path),
               c_file_mask[mode],
               FileHelper::default_mode, &AsyncFile::Inl::fs_open_cb);
  }
}

void AsyncFile::close() {
  if ( m_inl->m_fp ) {
    auto req = new FileReq(m_inl);
    uv_fs_close(_uv_loop(), req->req(), m_inl->m_fp, &AsyncFile::Inl::fs_close_cb);
  }
  else {
    // Error e(ERR_FILE_NOT_OPEN, "File not open");
    // async_err_callback(Callback(&AsyncFile::Inl::fs_close_err_cb3, m_inl), e, m_inl->m_loop);
  }
}

void AsyncFile::read(Buffer buffer, int64 size, int64 offset, int mark) {
  if ( size < 0 ) {
    size = buffer.length();
  } else {
    buffer = Buffer(buffer.collapse(), uint(size));
  }
  auto req = new FileStreamReq(m_inl, Callback(), { buffer, mark });
  uv_buf_t buf;
  buf.base = req->data().buffer.value();
  buf.len = size;
  uv_fs_read(_uv_loop(), req->req(), m_inl->m_fp, &buf, 1, offset, &AsyncFile::Inl::fs_read_cb);
}

void AsyncFile::write(Buffer buffer, int64 size, int64 offset, int mark) {
  if ( size < 0 ) {
    size = buffer.length();
  } else {
    buffer = Buffer(buffer.collapse(), uint(size));
  }
  auto req = new FileStreamReq(m_inl, Callback(), { buffer, mark });
  uv_buf_t buf;
  buf.base = req->data().buffer.value();
  buf.len = size;
  uv_fs_write(_uv_loop(), req->req(), m_inl->m_fp, &buf, 1, offset, &AsyncFile::Inl::fs_write_cb);
}

// ---------------------------------------FileHelper------------------------------------------

#if av_win
const uint FileHelper::default_mode(0);
#else
const uint FileHelper::default_mode([]() {
  uint mask = umask(0);
  umask(mask);
  return 0777 & ~mask;
}());
#endif

bool FileHelper::each(cString& path, Callback cb, bool internal) {
  
  FileStat stat = stat_sync(path);
  
  if ( stat.is_dir() ) {
    
    for ( auto& i : ls_sync(path) ) {
      
      Dirent& dirent = i.value();
      
      if ( internal ) { // 内部优先
        
        if ( dirent.type == FILE_DIR ) { // 目录
          if ( !each(dirent.pathname, cb, internal) ) {    // 停止遍历
            return 0;
          }
        }
        
        if ( !sync_callback(cb, nullptr, &dirent) ) {          // 停止遍历
          return 0;
        }
      }
      else {
        
        if ( !sync_callback(cb, nullptr, &dirent) ) {          // 停止遍历
          return 0;
        }
        
        if ( dirent.type == FILE_DIR ) { // 目录
          if ( !each(dirent.pathname, cb, internal) ) {    // 停止遍历
            return 0;
          }
        }
      }
    }
  } else {
    Dirent dirent(Path::basename(path), Path::format("%s", *path), stat.type());
    if ( !sync_callback(cb, nullptr, &dirent) ) {
      return 0;
    }
  }
  
  return 1;
}

bool FileHelper::mkdir_p_sync(cString& path, uint mode) {
  
  cchar* path2 = Path::restore_c(path);
  
  uv_fs_t req;
  
  if ( uv_fs_access(uv_default_loop(), &req, path2, F_OK, nullptr) == 0 ) {
    return true;
  }

  uint len = (uint)strlen(path2);
  char c = path2[len - 1];
  if (c == '/' || c == '\\') {
    len--;
  }
  char p[len + 1];
  memcpy(p, path2, len);
  p[len] = '\0';
  
  int i = len;

  for ( ; i > 0; i-- ) {

    c = p[i];

    if (c == '/' || c == '\\') {
      p[i] = '\0';
      
      if ( uv_fs_access(uv_default_loop(), &req, p, F_OK, nullptr) == 0 ) { // ok
        p[i] = '/';
        break;
      }
      
      while ( i > 1 ) {
        c = p[i - 1];
        if ( c == '/' || c == '\\') {
          i--;
        }
        else {
          break;
        }
      }
    }
  }
  
  i++;
  len++;
  
  for ( ; i < len; i++ ) {
    if (p[i] == '\0') {
      if ( uv_fs_mkdir(uv_default_loop(), &req, p, mode, nullptr) == 0 ) {
        if (i + 1 == len) {
          return true;
        }
        else {
          p[i] = '/';
        }
      }
      else {
        return false;
      }
    }
  }
  return false;
}

bool FileHelper::chmod_sync(cString& path, uint mode) {
  uv_fs_t req;
  return uv_fs_chmod(uv_default_loop(), &req, Path::restore_c(path), mode, nullptr) == 0;
}

bool FileHelper::chown_sync(cString& path, uint owner, uint group) {
  uv_fs_t req;
  return uv_fs_chown(uv_default_loop(), &req, Path::restore_c(path), owner, group, nullptr) == 0;
}

static bool default_stop_signal = false;

bool FileHelper::chmod_r_sync(cString& path, uint mode, bool* stop_signal) {
  
  if ( stop_signal == nullptr ) {
    stop_signal = &default_stop_signal;
  }
  
  uv_fs_t req;
  
  return each(path, Callback([&](SimpleEvent& d) {
    if ( *stop_signal ) { // 停止信号
      d.return_value = false;
    } else {
      Dirent* dirent = static_cast<Dirent*>(d.data);
      int r = uv_fs_chmod(uv_default_loop(), &req, Path::restore_c(dirent->pathname), mode, nullptr);
      d.return_value = (r == 0);
    }
  }));
}

bool FileHelper::chown_r_sync(cString& path, uint owner, uint group, bool* stop_signal) {
  
  if (stop_signal == nullptr) {
    stop_signal = &default_stop_signal;
  }
  
  uv_fs_t req;
  
  return each(path, Callback([&](SimpleEvent& d) {
    if (*stop_signal) { // 停止信号
      d.return_value = false;
    } else {
      Dirent* dirent = static_cast<Dirent*>(d.data);
      int r = uv_fs_chown(uv_default_loop(), &req,
                          Path::restore_c(dirent->pathname), owner, group, nullptr);
      d.return_value = (r == 0);
    }
  }));
}

bool FileHelper::mkdir_sync(cString& path, uint mode) {
  uv_fs_t req;
  return uv_fs_mkdir(uv_default_loop(), &req, Path::restore_c(path), mode, nullptr) == 0;
}

bool FileHelper::rename_sync(cString& name, cString& new_name) {
  uv_fs_t req;
  return uv_fs_rename(uv_default_loop(), &req,
                      Path::restore_c(name), Path::restore_c(new_name), nullptr) == 0;
}

bool FileHelper::mv_sync(cString& name, cString& new_name) {
  return rename_sync(name, new_name);
}

bool FileHelper::unlink_sync(cString& path) {
  uv_fs_t req;
  return uv_fs_unlink(uv_default_loop(), &req, Path::restore_c(path), nullptr) == 0;
}

bool FileHelper::rmdir_sync(cString& path) {
  uv_fs_t req;
  return uv_fs_rmdir(uv_default_loop(), &req, Path::restore_c(path), nullptr) == 0;
}

bool FileHelper::rm_r_sync(cString& path, bool* stop_signal) {
  
  if ( stop_signal == nullptr ) {
    stop_signal = &default_stop_signal;
  }
  
  uv_fs_t req;
  
  return each(path, Callback([&](SimpleEvent& d) {
    if ( *stop_signal ) { // 停止信号
      d.return_value = false;
    } else {
      Dirent* dirent = static_cast<Dirent*>(d.data);
      cchar* p = Path::restore_c(dirent->pathname);
      if ( dirent->type == FILE_DIR ) {
        d.return_value = uv_fs_rmdir(uv_default_loop(), &req, p, nullptr) == 0;
      } else {
        d.return_value = uv_fs_unlink(uv_default_loop(), &req, p, nullptr) == 0;
      }
    }
  }), true);
}

static bool cp_sync2(cString& source, cString& target, bool* stop_signal) {
  
  File source_file(source);
  File target_file(target);
  
  if ( source_file.open(FOPEN_R) && target_file.open(FOPEN_W) ) {

    int size = 1024 * 512; // 512 kb
    Buffer data(size);
    
    int64 len = source_file.read(*data, size);
    
    while ( len > 0 ) {
      
      if ( target_file.write(*data, len) != len ) { // 写入数据失败
        return false;
      }
      
      if ( *stop_signal ) { // 停止信号
        return  false;
      }
      
      len = source_file.read(*data, size);
    }
    
    return true;
  }

  return false;
}

bool FileHelper::cp_sync(cString& source, cString& target, bool* stop_signal) {
  return cp_sync2(source, target, stop_signal ? stop_signal : &default_stop_signal);
}

bool FileHelper::cp_r_sync(cString& source, cString& target, bool* stop_signal) {
  
  if ( !is_dir_sync(Path::dirname(target)) ) { // 没有父目录,无法复制
    return false;
  }
  
  if ( stop_signal == nullptr ) {
    stop_signal = &default_stop_signal;
  }
  
  uint s_len = Path::format("%s", *source).length();
  String av_path = Path::format("%s", *target);
  
  return each(source, Callback([&](SimpleEvent& d) {
    
    Dirent* dirent = static_cast<Dirent*>(d.data);
    String target = av_path + dirent->pathname.substr(s_len); // 目标文件
    
    switch (dirent->type) {
      case FILE_DIR: d.return_value = mkdir_sync(target); /* create dir */ break;
      case FILE_FILE:
        d.return_value = cp_sync2(dirent->pathname, target, stop_signal);
        break;
      default: break;
    }
    if ( *stop_signal ) { // 停止信号
      d.return_value = false;
    }
  }));
}

Array<Dirent> FileHelper::ls_sync(cString& path) {
  Array<Dirent> ls;
  uv_fs_t req;
  String p = Path::format("%s", *path) + '/';
  if ( uv_fs_scandir(uv_default_loop(), &req, Path::restore_c(path), 1, nullptr) ) {
    uv_dirent_t ent;
    while ( uv_fs_scandir_next(&req, &ent) == 0 ) {
      ls.push( Dirent(ent.name, p + ent.name, FileType(ent.type)) );
    }
  }
  uv_fs_req_cleanup(&req);
  return ls;
}

Array<FileStat> FileHelper::ls_l_sync(cString& path) {
  Array<FileStat> ls;
  uv_fs_t req;
  if ( uv_fs_scandir(uv_default_loop(), &req, Path::restore_c(path), 1, nullptr) ) {
    String p = Path::format("%s", *path) + '/';
    uv_dirent_t ent;
    while ( uv_fs_scandir_next(&req, &ent) == 0 ) {
      ls.push( stat_sync(p + ent.name) );
    }
  }
  uv_fs_req_cleanup(&req);
  return ls;
}

bool FileHelper::write_file_sync(cString& path, const void* buffer, uint64 size, int64 offset) {
  uv_fs_t req;
  int fp = uv_fs_open(uv_default_loop(), &req,
                      Path::restore_c(path),
                      O_WRONLY | O_CREAT | O_TRUNC, default_mode, nullptr);
  if ( fp ) {
    uv_buf_t buf;
    buf.base = (char*)buffer;
    buf.len = size;
    int rev = uv_fs_write(uv_default_loop(), &req, fp, &buf, 1, offset, nullptr);
    uv_fs_close(uv_default_loop(), &req, fp, nullptr);
    
    return (size == rev);
  }
  return false;
}

bool FileHelper::write_file_sync(cString& path, cString& str) {
  return write_file_sync(path, WeakBuffer(*str, str.length()) );
}

Buffer FileHelper::read_file_sync(cString& path, int64 size, int64 offset) {
  
  Buffer buff;
  uv_fs_t req;
  int fp = uv_fs_open(uv_default_loop(), &req, Path::restore_c(path), O_RDONLY, 0, nullptr);
  
  while ( fp ) {
    if ( size < 0 ) {
      if (uv_fs_fstat(uv_default_loop(), &req, fp, nullptr) != 0) {
        break;
      }
      size = req.statbuf.st_size;
    }
    char* buffer = (char*)::malloc(size + 1); // 为兼容C字符串多加1位0
    if ( buffer ) {
      uv_buf_t buf;
      buf.base = buffer;
      buf.len = size;
      size = uv_fs_read(uv_default_loop(), &req, fp, &buf, 1, offset, nullptr);
      
      if ( size > 0 ) {
        buffer[size] = '\0';
        buff = Buffer(buffer, uint(size));
      } else {
        free(buffer);
      }
    }
    break;
  }
  
  if ( fp ) {
    uv_fs_close(uv_default_loop(), &req, fp, nullptr);
  }
  return buff;
}

bool FileHelper::is_file_sync(cString& path) {
  uv_fs_t req;
  if (uv_fs_stat(uv_default_loop(), &req, Path::restore_c(path), nullptr) == 0) {
    return !S_ISDIR(req.statbuf.st_mode);
  }
  return false;
}

bool FileHelper::is_dir_sync(cString& path) {
  uv_fs_t req;
  if (uv_fs_stat(uv_default_loop(), &req, Path::restore_c(path), nullptr) == 0) {
    return S_ISDIR(req.statbuf.st_mode);
  }
  return false;
}

FileStat FileHelper::stat_sync(cString& path) {
  FileStat stat;
  uv_fs_t req;
  if (uv_fs_stat(uv_default_loop(), &req, Path::restore_c(path), nullptr) == 0) {
    Inl_FileStat(&stat)->set_m_stat(&req.statbuf);
  }
  return stat;
}

bool FileHelper::exists_sync(cString& path) {
  uv_fs_t req;
  return uv_fs_access(uv_default_loop(), &req, Path::restore_c(path), F_OK, nullptr) == 0;
}

bool FileHelper::readable_sync(cString& path) {
  uv_fs_t req;
  return uv_fs_access(uv_default_loop(), &req, Path::restore_c(path), W_OK, nullptr) == 0;
}

bool FileHelper::writable_sync(cString& path) {
  uv_fs_t req;
  return uv_fs_access(uv_default_loop(), &req, Path::restore_c(path), W_OK, nullptr) == 0;
}

bool FileHelper::executable_sync(cString& path) {
  uv_fs_t req;
  return uv_fs_access(uv_default_loop(), &req, Path::restore_c(path), X_OK, nullptr) == 0;
}

// --------------------------------- async -----------------------------------

template<class uv_req, class Data = Object>
class AsyncReqNonCtx: public UVRequestWrap<uv_req, Reference, Data> {
public:
  AsyncReqNonCtx(Callback cb, bool sync_loop = true, Data data = Data())
  : UVRequestWrap<uv_req, Reference, Data>(nullptr, cb, move(data))
  , m_loop(sync_loop ? RunLoop::keep_alive_current() : nullptr) { }
  ~AsyncReqNonCtx() { Release(m_loop); }
  static inline AsyncReqNonCtx* cast(uv_req* req) { return (AsyncReqNonCtx*)req->data; }
  inline PostMessage* loop() { return m_loop ? m_loop->host(): nullptr; }
  inline static PostMessage* loop(LoopProxy* loop) { return loop ? loop->host(): nullptr; }
private:
  LoopProxy* m_loop;
};

typedef AsyncReqNonCtx<uv_fs_t> FileReq2;

static void async_ret_err_callback(FileReq2* req) {
  Error err(req->req()->result, "%s, %s",
            uv_err_name(req->req()->result), uv_strerror(req->req()->result));
  async_err_callback(req->cb(), move(err), req->loop());
}

static void async_ret_err_callback(Callback cb, uv_fs_t* req, PostMessage* loop) {
  Error err(req->result, "%s, %s", uv_err_name(req->result), uv_strerror(req->result));
  async_err_callback(cb, move(err), loop);
}

static void uv_fs_async_cb(uv_fs_t* req) {
  uv_fs_req_cleanup(req);
  Handle<FileReq2> handle(FileReq2::cast(req));
  if ( req->result == 0 ) { // ok
    async_callback(handle->cb(), handle->loop());
  } else { // err
    async_ret_err_callback(*handle);
  }
}

static void uv_fs_access_cb(uv_fs_t* req) {
  uv_fs_req_cleanup(req);
  Handle<FileReq2> handle(FileReq2::cast(req));
  async_callback(handle->cb(), Bool(req->result == 0), handle->loop());
}

static void ls_cb(uv_fs_t* req) {
  Handle<FileReq2> handle(FileReq2::cast(req));
  Array<String> ls;
  if ( req->result ) {
    uv_dirent_t ent;
    while ( uv_fs_scandir_next(req, &ent) == 0 ) {
      ls.push(ent.name);
    }
  }
  uv_fs_req_cleanup(req);
  async_callback(handle->cb(), move(ls), handle->loop());
}

static void is_file_cb(uv_fs_t* req) {
  uv_fs_req_cleanup(req);
  Handle<FileReq2> handle(FileReq2::cast(req));
  if ( req->result == 0 ) { // ok
    async_callback(handle->cb(), Bool(!S_ISDIR(req->statbuf.st_mode)), handle->loop());
  } else { // err
    async_callback(handle->cb(), Bool(false), handle->loop());
  }
}

static void is_dir_cb(uv_fs_t* req) {
  uv_fs_req_cleanup(req);
  Handle<FileReq2> handle(FileReq2::cast(req));
  if ( req->result == 0 ) { // ok
    async_callback(handle->cb(), Bool(S_ISDIR(req->statbuf.st_mode)), handle->loop());
  } else { // err
    async_callback(handle->cb(), Bool(false), handle->loop());
  }
}

static void stat_cb(uv_fs_t* req) {
  uv_fs_req_cleanup(req);
  Handle<FileReq2> handle(FileReq2::cast(req));
  if ( req->result == 0 ) { // ok
    FileStat stat;
    Inl_FileStat(&stat)->set_m_stat(&req->statbuf);
    async_callback(handle->cb(), move(stat), handle->loop());
  } else { // err
    async_ret_err_callback(*handle);
  }
}

static void exists2(cString& path, Callback cb, bool sync_loop) {
  uv_fs_access(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), F_OK, &uv_fs_access_cb);
}

static void ls2(cString& path, Callback cb, bool sync_loop) {
  uv_fs_scandir(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), 1, &ls_cb);
}

void file_helper_stat2(cString& path, Callback cb, bool sync_loop) {
  uv_fs_stat(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), &stat_cb);
}

static void is_dir2(cString& path, Callback cb, bool sync_loop) {
  uv_fs_stat(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), &is_file_cb);
}

#define BUFFER_SIZE (1024 * 64) // 64kb

static AsyncIOTask* cp2(cString& source, cString& target, Callback cb, bool sync_loop) {
  
  class Task: public AsyncIOTask, AsyncFile::Delegate {
  public:
    
    Task(cString& source, cString& target, Callback cb, bool sync_loop)
    : _source_file( new AsyncFile(source, nullptr) )
    , _target_file( new AsyncFile(target, nullptr) )
    , _end(cb), _loop(sync_loop ? RunLoop::keep_alive_current() : nullptr)
    , _reading_count(0), _writeing_count(0), _read_end(false)
    {//
      _buffer[0] = Buffer(BUFFER_SIZE);
      _buffer[1] = Buffer(BUFFER_SIZE);
      _buffer[2] = Buffer(BUFFER_SIZE);
      _buffer[3] = Buffer(BUFFER_SIZE);
      _source_file->set_delegate(this);
      _target_file->set_delegate(this);
      _source_file->open(FOPEN_R);
      _target_file->open(FOPEN_W);
    }
    
    virtual ~Task() {
      release_file_handle();
      Release(_loop);
    }
    
    void release_file_handle() {
      if ( _source_file ) {
        _source_file->set_delegate(nullptr);
        _target_file->set_delegate(nullptr);
        if ( _source_file->is_open() ) _source_file->close();
        if ( _target_file->is_open() ) _target_file->close();
        Release(_source_file);
        Release(_target_file);
        _source_file = nullptr;
        _target_file = nullptr;
      }
    }
    
    inline PostMessage* loop() { return _loop ? _loop->host() : nullptr; }
    
    Buffer alloc_buffer() {
      for ( int i = 0; i < 4; i++ ) {
        if (_buffer[i].length()) {
          return _buffer[i];
        }
      }
      return Buffer();
    }
    
    void release_buffer(Buffer buffer) {
      for ( int i = 0; i < 4; i++ ) {
        if (_buffer[i].length() == 0) {
          _buffer[i] = Buffer(buffer.collapse(), BUFFER_SIZE);
        }
      }
      av_assert(buffer.length() == 0);
    }
    
    void read_next() {
      Buffer buff = alloc_buffer();
      if ( buff.length() ) {
        _reading_count++;
        _source_file->read(buff, BUFFER_SIZE);
      }
    }
    
    virtual void abort_and_release() {
      release_file_handle();
      AsyncIOTask::abort_and_release();
    }
    
    virtual void trigger_async_file_error(AsyncFileProtocol* file, cError& error) {
      Handle<Task> handle(this); //
      abort_and_release();
      async_err_callback(_end, Error(error), loop());
    }
    
    virtual void trigger_async_file_open(AsyncFileProtocol* file) {
      if ( _source_file->is_open() && _target_file->is_open() ) {
        read_next();
      }
    }
    
    virtual void trigger_async_file_read(AsyncFileProtocol* file, Buffer buffer) {
      av_assert( file == _source_file );
      av_assert( _reading_count > 0 );
      _reading_count--;
      if ( buffer.length() ) {
        _writeing_count++;
        _target_file->write(buffer, buffer.length());
        read_next();
      } else {
        _read_end = true;
      }
    }
    
    virtual void trigger_async_file_write(AsyncFileProtocol* file, Buffer buffer) {
      av_assert( file == _target_file );
      av_assert( _writeing_count > 0 );
      _writeing_count--;
      release_buffer(buffer);
      if ( _read_end ) {
        if ( _writeing_count == 0 ) {
          // copy complete
          Handle<Task> handle(this);
          abort_and_release();
          async_callback(_end, loop());
        }
      } else {
        if ( _reading_count == 0 ) {
          read_next();
        }
      }
    }
    
    AsyncFile* _source_file;
    AsyncFile* _target_file;
    Callback   _end;
    LoopProxy* _loop;
    Buffer     _buffer[4];
    int        _reading_count;
    int        _writeing_count;
    bool       _read_end;
  };
  
  return NewRetain<Task>(source, target, cb, sync_loop);
}

static void mkdir2(cString& path, uint mode, Callback cb, bool sync_loop) {
  uv_fs_mkdir(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), mode, &uv_fs_async_cb);
}

static void chmod2(cString& path, uint mode, Callback cb, bool sync_loop) {
  uv_fs_chmod(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), mode, &uv_fs_async_cb);
}

static void chown2(cString& path, uint owner, uint group, Callback cb, bool sync_loop) {
  uv_fs_chown(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), owner, group, &uv_fs_async_cb);
}

static void unlink2(cString& path, Callback cb, bool sync_loop) {
  uv_fs_unlink(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), &uv_fs_async_cb);
}

static void rmdir2(cString& path, Callback cb, bool sync_loop) {
  uv_fs_rmdir(_uv_loop(), New<FileReq2>(cb, sync_loop)->req(), Path::restore_c(path), &uv_fs_async_cb);
}

/**
 * @class AsyncEach
 */
class AsyncEach: public AsyncIOTask {
public:
  
  AsyncEach(cString& path, Callback cb, Callback end, bool sync_loop, bool internal = false)
  : _path(Path::format(path))
    , _cb(cb)
    , _end(end)
    , _dirent(nullptr)
    , _last(nullptr), _internal(internal)
    , _loop( sync_loop ? RunLoop::keep_alive_current() : nullptr )
    , _start(false)
  {
  
  }
  
  virtual ~AsyncEach() {
    Release(_loop);
  }
  
  void advance() {
    
    if ( is_abort() ) return;
    
    if ( _last->index < _last->dirents.length() ) {
      
      _dirent = &_last->dirents[_last->index];
      
      if ( _internal ) { // 内部优先
        if ( _dirent->type == FILE_DIR && _last->mask == 0 ) { // 目录
          _last->mask = 1;
          into(_dirent->pathname);
        } else {
          _last->index++; //
          _last->mask = 0;
          sync_callback(_cb, nullptr, this);
        }
      }
      else {
        
        if ( _dirent->type == FILE_DIR ) {
          if ( _last->mask == 0 ) {
            _last->mask = 1;
            sync_callback(_cb, nullptr, this);
          } else {
            _last->index++;
            _last->mask = 0;
            into(_dirent->pathname);
          }
        } else {
          _last->index++;
          _last->mask = 0;
          sync_callback(_cb, nullptr, this);
        }
      }
      
    } else { // end
      if ( _stack.length() > 1 ) {
        _stack.pop();
        advance();
      } else {
        Handle<AsyncEach> handle(this); // retain scope
        abort_and_release();
        async_callback(_end, loop()); // end exit
      }
    }
  }
  
  inline Dirent& dirent() { return *_dirent; }
  
  uint start() {
    if ( !_start ) {
      _start = true;
      file_helper_stat2(_path, Callback(&AsyncEach::start_cb, this), false);
    }
    return id();
  }

  inline PostMessage* loop() { return _loop ? _loop->host(): nullptr; }

private:
  
  inline void into(cString& path) {
    ls2(path, Callback(&AsyncEach::into_cb, this), false);
  }
  
  void into_cb(SimpleEvent& evt) {
    if ( !is_abort() ) {
      if ( evt.error ) { // err
        abort_and_release();
        async_err_callback(_end, Error(*evt.error), loop());
      } else {
        _stack.push( { move(*static_cast<Array<Dirent>*>(evt.data)), 0, false } );
        _last = &_stack[_stack.length() - 1];
        advance();
      }
    }
  }
  
  void start_cb(SimpleEvent& evt) {
    if ( !is_abort() ) {
      if ( evt.error ) { // err
        abort_and_release();
        async_err_callback(_end, Error(*evt.error), loop());
      } else {
        FileStat* stat = static_cast<FileStat*>(evt.data);
        if ( stat->is_dir() ) {
          into(_path); // into dir
        } else {
          abort_and_release();
          Dirent dirent(Path::basename(_path), _path, stat->type());
          _dirent = &dirent;
          sync_callback(_cb, nullptr, this);
          async_callback(_end, loop());
          // end
        }
      }
    }
  }

  struct DirentLS {
    Array<Dirent> dirents;
    int index;
    int mask;
  };
  
  String _path;
  Callback _cb;
  Callback _end;
  Array<DirentLS> _stack;
  Dirent* _dirent;
  DirentLS* _last;
  bool _internal;
  bool _start;
  LoopProxy* _loop;
};

void FileHelper::abort(uint id) {
  AsyncIOTask::safe_abort(id);
}

void FileHelper::chmod(cString& path, uint mode, Callback cb) {
  chmod2(path, mode, cb, true);
}

uint FileHelper::chmod_r(cString& path, uint mode, Callback cb) {
  AsyncEach* each = NewRetain<AsyncEach>(path, Callback([each, mode, cb](SimpleEvent& evt) {
    each->retain();
    chmod2(each->dirent().pathname, mode, Callback([each, cb](SimpleEvent& evt) {
      Handle<AsyncEach> handle(each); each->release();
      if ( !each->is_abort() ) {
        if ( evt.error ) {
          each->abort_and_release();
          async_err_callback(cb, Error(*evt.error), each->loop());
        } else {
          each->advance();
        }
      }
    }), false);
  }), cb, true);
  return each->start();
}

void FileHelper::chown(cString& path, uint owner, uint group, Callback cb) {
  chown2(path, owner, group, cb, true);
}

uint FileHelper::chown_r(cString& path, uint owner, uint group, Callback cb) {
  AsyncEach* each = NewRetain<AsyncEach>(path, Callback([each, owner, group, cb](SimpleEvent& evt) {
    each->retain();
    chown2(each->dirent().pathname, owner, group, Callback([each, cb](SimpleEvent& evt) {
      Handle<AsyncEach> handle(each); each->release();
      if ( !each->is_abort() ) {
        if ( evt.error ) {
          each->abort_and_release();
          async_err_callback(cb, Error(*evt.error), each->loop());
        } else {
          each->advance();
        }
      }
    }), false);
  }), cb, true);
  return each->start();
}

void FileHelper::mkdir(cString& path, uint mode, Callback cb) {
  mkdir2(path, mode, cb, true);
}

void FileHelper::mkdir_p(cString& path, uint mode, Callback cb) {
  LoopProxy* loop = RunLoop::keep_alive_current();
  
  exists2(path, Callback([=](SimpleEvent& evt) {
    Handle<LoopProxy> h(loop);
    if ( static_cast<Bool*>(evt.data)->value ) { // ok
      async_callback(cb, FileReq2::loop(loop));
    } else {
      if ( mkdir_p_sync(path, mode) ) {
        async_callback(cb, FileReq2::loop(loop));
      } else {
        Error e(ERR_UNKNOWN_ERROR, "unknown error");
        async_err_callback(cb, move(e), FileReq2::loop(loop));
      }
    }
  }), false);
}

void FileHelper::rename(cString& name, cString& new_name, Callback cb) {
  uv_fs_rename(_uv_loop(), New<FileReq2>(cb)->req(),
               Path::restore_c(name), Path::restore_c(new_name), &uv_fs_async_cb);
}

void FileHelper::mv(cString& name, cString& new_name, Callback cb) {
  return rename(name, new_name, cb);
}

void FileHelper::unlink(cString& path, Callback cb) {
  unlink2(path, cb, true);
}

void FileHelper::rmdir(cString& path, Callback cb) {
  rmdir2(path, cb, true);
}

uint FileHelper::rm_r(cString& path, Callback cb) {
  AsyncEach* each = NewRetain<AsyncEach>(path, Callback([each, cb](SimpleEvent& evt) {
    each->retain();
    
    Callback _cb([each, cb](SimpleEvent& evt) {
      Handle<AsyncEach> handle(each); each->release();
      if ( !each->is_abort() ) {
        if ( evt.error ) {
          each->abort_and_release();
          async_err_callback(cb, Error(*evt.error), each->loop());
        } else {
          each->advance();
        }
      }
    });
    
    if ( each->dirent().type == FILE_DIR ) {
      rmdir2(each->dirent().pathname, _cb, false);
    } else {
      unlink2(each->dirent().pathname, _cb, false);
    }
  }), cb, true, true);
  return each->start();
}

uint FileHelper::cp(cString& source, cString& target, Callback cb) {
  return cp2(source, target, cb, true)->id();
}

uint FileHelper::cp_r(cString& source, cString& target, Callback cb) {
  
  class Task: public AsyncEach {
  public:
    
    Task(cString& source, cString& target, Callback cb)
    : AsyncEach(source, Callback(&Task::each_cb, this), cb, true, false)
    , _end(cb)
    , s_len(Path::format("%s", *source).length())
    , av_path(Path::format("%s", *target))
    , copy_task(nullptr) {
      is_dir2(Path::dirname(target), Callback(&Task::is_dir_cb, this), false);
    }
    
    void is_dir_cb(SimpleEvent& evt) {
      if ( !is_abort() ) {
        start();
      }
    }
    
    void each_cb(SimpleEvent& d) {
      
      Dirent& ent = dirent();
      String target = av_path + ent.pathname.substr(s_len); // 目标文件
      
      switch (ent.type) {
        case FILE_DIR:
          mkdir2(target, default_mode, Callback(&Task::mkdir_cb, this), false); /* create dir */
          break;
        case FILE_FILE:
          copy_task = cp2(ent.pathname, target, Callback(&Task::cp_cb, this), false);
          break;
        default: advance(); break;
      }
    }
    
    void mkdir_cb(SimpleEvent& evt) {
      if ( !is_abort() ) {
        if ( evt.error ) {
          abort_and_release();
          async_err_callback(_end, Error(*evt.error), loop());
        } else {
          advance();
        }
      }
    }
    
    void cp_cb(SimpleEvent& evt) {
      copy_task = nullptr;
      if ( !is_abort() ) {
        if ( evt.error ) {
          abort_and_release();
          async_err_callback(_end, Error(*evt.error), loop());
        } else {
          advance();
        }
      }
    }
    
    virtual void abort_and_release() {
      if ( copy_task ) {
        copy_task->abort_and_release();
        copy_task = nullptr;
      }
      AsyncEach::abort_and_release();
    }
    
    Callback _end;
    uint   s_len;
    String av_path;
    AsyncIOTask* copy_task;
  };
  
  return NewRetain<Task>(source, target, cb)->id();
}

void FileHelper::ls(cString& path, Callback cb) {
  ls2(path, cb, true);
}

void FileHelper::ls_l(cString& path, Callback cb) {
  
  struct LSLData;
  
  typedef AsyncReqNonCtx<uv_fs_t, LSLData> FileReqLSL;
  typedef AsyncReqNonCtx<uv_fs_t, FileReqLSL*> FileReqLSL_Stat;
  
  struct LSLData {
    String  path;
    int     count;
    Array<FileStat> result;
    
    static void ls_l_cb2(uv_fs_t* req) {
      Handle<FileReqLSL_Stat> handle(FileReqLSL_Stat::cast(req));
      FileReqLSL* req_ = handle->data();
      
      req_->data().count--;
      
      if ( req->result == 0 ) {
        FileStat stat;
        Inl_FileStat(&stat)->set_m_stat(&req->statbuf);
        req_->data().result.push(move(stat));
      }
      
      if ( req_->data().count == 0 ) {
        Handle<FileReqLSL> handle(req_);
        async_callback(handle->cb(), move(handle->data().result), handle->loop());
      }
    }
    
    static void ls_l_cb(uv_fs_t* req) {
      FileReqLSL* req_ = FileReqLSL::cast(req);
      if ( req->result ) {
        uv_dirent_t ent;
        req_->data().count = int(req->result);
        while ( uv_fs_scandir_next(req, &ent) == 0 ) {
          String path = req_->data().path + ent.name;
          uv_fs_stat(_uv_loop(), New<FileReqLSL_Stat>( Callback(), req_)->req(), *path, &ls_l_cb2);
        }
      } else {
        Handle<FileReqLSL> h(req_);
        async_callback(req_->cb(), move(req_->data().result), req_->loop());
      }
    }
  };

  auto req = new FileReqLSL(cb, true, { Path::restore(Path::format("%s", *path) + '/') });
  
  uv_fs_scandir(_uv_loop(), req->req(), Path::restore_c(path), 1, &LSLData::ls_l_cb);
}

void FileHelper::write_file(cString& path, Buffer buffer, uint64 size, int64 offset, Callback cb) {
  
  struct Data;
  
  typedef AsyncReqNonCtx<uv_fs_t, Data> FileReq_;
  
  struct Data {
    uint64  size;
    int64   offset;
    Buffer  buff;
    int fd;
    
    static void cb4(uv_fs_t* req) { // close
      Release(FileReq_::cast(req));
    }
    
    static void cb3(uv_fs_t* req) { // write cb
      uv_fs_req_cleanup(req);
      FileReq_* req_ = FileReq_::cast(req);
      if ( req->result != req_->data().size ) {
        Error err(req->result, "%s, %s", uv_err_name(req->result), uv_strerror(req->result));
        async_callback(req_->cb(), move(err), move(req_->data().buff), req_->loop());
      } else {
        async_callback(req_->cb(), move(req_->data().buff), req_->loop());
      }
      uv_fs_close(_uv_loop(), req, req_->data().fd, &cb4); // close
    }
    
    static void cb1(uv_fs_t* req) { // open file
      uv_fs_req_cleanup(req);
      FileReq_* req_ = FileReq_::cast(req);
      if ( req->result ) { // open ok
        req_->data().fd = req->result;
        uv_buf_t buf;
        buf.base = req_->data().buff.value();
        buf.len = req_->data().size;
        uv_fs_write(_uv_loop(), req, req->result, &buf, 1, req_->data().offset, &cb3);
      } else { // open file fail
        Handle<FileReq_> handle(req_);
        async_ret_err_callback(req_->cb(), req, req_->loop());
      }
    }
  };
  
  auto req = new FileReq_(cb, true, Data({ size, offset, buffer }));
  
  uv_fs_open(_uv_loop(), req->req(),
             Path::restore_c(path), O_WRONLY | O_CREAT | O_TRUNC, default_mode, &Data::cb1);
}

void FileHelper::write_file(cString& path, cString& str, Callback cb) {
  write_file(path, String(str).collapse_buffer(), -1, cb);
}

void FileHelper::read_file(cString& path, Callback cb) {
  read_file(path, -1, -1, cb);
}

void FileHelper::read_file(cString& path, int64 size, int64 offset, Callback cb) {
  
  struct Data;
  
  typedef AsyncReqNonCtx<uv_fs_t, Data> FileReq;
  
  struct Data {
    int64  size;
    int64  offset;
    Buffer buff;
    int fd;
    
    static void cb4(uv_fs_t* req) { // close
      Release(FileReq::cast(req));
    }
    
    static void fstat_cb(uv_fs_t* req) { // read size
      uv_fs_req_cleanup(req);
      FileReq* req_ = FileReq::cast(req);
      if ( req->result == 0 ) {
        req_->data().size = req->statbuf.st_size;
        start_read(req_);
      } else { // err
        async_ret_err_callback(req_->cb(), req, req_->loop());
        uv_fs_close(_uv_loop(), req, req_->data().fd, &cb4); // close
      }
    }
    
    static void read_cb(uv_fs_t* req) { // read data result
      uv_fs_req_cleanup(req);
      FileReq* req_ = FileReq::cast(req);
      if ( req->result < 0 ) { // error
        async_ret_err_callback(req_->cb(), req, req_->loop());
      } else {
        if ( req->result ) {
          Buffer& buff = req_->data().buff;
          buff.value()[req->result] = '\0';
          async_callback(req_->cb(), Buffer(buff.collapse(), uint(req->result)), req_->loop());
        } else { // not data
          async_callback(req_->cb(), Buffer(), req_->loop());
        }
      }
      uv_fs_close(_uv_loop(), req, req_->data().fd, &cb4); // close
    }
    
    static void start_read(FileReq* req_) {
      int64 size = req_->data().size;
      char* buffer = (char*)::malloc(size + 1); // 为兼容C字符串多加1位0
      if ( buffer ) {
        req_->data().buff = Buffer(buffer, uint(size));
        uv_buf_t buf;
        buf.base = buffer;
        buf.len = size;
        uv_fs_read(_uv_loop(), req_->req(), req_->data().fd, &buf, 1, req_->data().offset, &read_cb);
      } else {
        async_err_callback(req_->cb(),
                           Error(ERR_ALLOCATE_MEMORY_FAIL, "allocate memory fail"), req_->loop());
        uv_fs_close(_uv_loop(), req_->req(), req_->data().fd, &cb4); // close
      }
    }
    
    static void open_cb(uv_fs_t* req) { // open file
      uv_fs_req_cleanup(req);
      FileReq* req_ = FileReq::cast(req);
      if ( req->result ) {
        req_->data().fd = req->result;
        if ( req_->data().size < 0 ) {
          uv_fs_fstat(_uv_loop(), req, req->result, &fstat_cb);
        } else {
          start_read(req_);
        }
      } else { // open file fail
        Handle<FileReq> handle(req_);
        async_ret_err_callback(req_->cb(), req, req_->loop());
      }
    }
  };
  
  auto req = new FileReq(cb, true, { size, offset });
  
  uv_fs_open(_uv_loop(), req->req(), Path::restore_c(path), O_RDONLY, 0, &Data::open_cb);
}

uint FileHelper::read_file_stream(cString& path, int64 offset, Callback cb) {
  
  class Task;
  
  typedef UVRequestWrap<uv_fs_t, Task> FileReq;
  
  class Task: public AsyncIOTask, public SimpleStream {
  public:
    
    int64      _offset;
    int        _fd;
    Callback   _cb;
    LoopProxy* _loop;
    Buffer     _buffer;
    bool       _pause;
    int        _read_count;
    FileReq*   _req;
    int64      _total;
    int64      _size;
    
    Task(int64 offset, Callback cb) {
      _loop = RunLoop::keep_alive_current();
      _offset = offset;
      _cb = cb;
      _pause = false;
      _read_count = 0;
      _total = 0;
      _size = 0;
    }
    
    ~Task() { Release(_loop); }
    
    PostMessage* loop() {
      return _loop ? _loop->host(): nullptr;
    }
    
    static PostMessage* loop(uv_fs_t* req) {
      return FileReq::cast(req)->ctx()->loop();
    }
    
    void abort_and_release() {
      AsyncIOTask::abort_and_release();
    }
    
    virtual void pause() {
      _pause = true;
    }
    
    void resume_from_io_loop(SimpleEvent& evt) {
      read_advance(_req);
    }
    
    virtual void resume() {
      if ( _pause ) {
        _pause = false;
        RunLoop::post_io(Callback(&Task::resume_from_io_loop, this), true);
      }
    }
    
    static void close_cb(uv_fs_t* req) { // close
      Release(FileReq::cast(req)); // release req
    }
    
    static void read_cb(uv_fs_t* req) { // read data result
      uv_fs_req_cleanup(req);
      auto req_ = FileReq::cast(req);
      Task* ctx = req_->ctx();
      
      ctx->_read_count--;
      av_assert(ctx->_read_count == 0);
      
      if ( req->result < 0 ) { // error
        ctx->abort_and_release();
        async_ret_err_callback(req_->cb(), req, req_->ctx()->loop());
        uv_fs_close(_uv_loop(), req, ctx->_fd, &close_cb); // close
      } else {
        if ( req->result ) {
          if ( ctx->_offset > -1 ) {
            ctx->_offset += req->result;
          }
          ctx->_size += req->result;
          IOStreamData data(Buffer(ctx->_buffer.collapse(), uint(req->result)),
                            0, ctx->id(), ctx->_size, ctx->_total, ctx);
          async_callback(ctx->_cb, move(data), ctx->loop());
          ctx->read_advance(req_);
        } else { // end
          ctx->abort_and_release();
          IOStreamData data(Buffer(), 1, ctx->id(), ctx->_size, ctx->_total, ctx);
          async_callback(ctx->_cb, move(data), ctx->loop());
          uv_fs_close(_uv_loop(), req, ctx->_fd, &close_cb); // close
        }
      }
    }
    
    void read_advance(FileReq* req) {
      if ( is_abort() ) {
        uv_fs_close(_uv_loop(), req->req(), _fd, &close_cb); // close
      } else {
        if ( !_pause && _read_count == 0 ) {
          _read_count++;
          
          if ( !_buffer.length() ) {
            _buffer = Buffer(BUFFER_SIZE);
          }
          uv_buf_t buf;
          buf.base = _buffer.value();
          buf.len = _buffer.length();
          uv_fs_read(_uv_loop(), req->req(), _fd, &buf, 1, _offset, &read_cb);
        }
      }
    }
    
    static void fstat_cb(uv_fs_t* req) { // read size
      uv_fs_req_cleanup(req);
      FileReq* req_ = FileReq::cast(req);
      if ( req->result == 0 ) {
        req_->ctx()->_total = req->statbuf.st_size;
        if ( req_->ctx()->_offset > 0 ) {
          req_->ctx()->_total -= req_->ctx()->_offset;
          req_->ctx()->_total = av_max(req_->ctx()->_total, 0);
        }
        req_->ctx()->read_advance(req_);
      } else { // err
        req_->ctx()->abort_and_release();
        async_ret_err_callback(req_->ctx()->_cb, req, req_->ctx()->loop());
        uv_fs_close(_uv_loop(), req, req_->ctx()->_fd, &close_cb); // close
      }
    }
    
    static void open_cb(uv_fs_t* req) { // open file
      uv_fs_req_cleanup(req);
      auto req_ = FileReq::cast(req);
      if ( req->result ) {
        req_->ctx()->_fd = req->result;
        uv_fs_fstat(_uv_loop(), req, req->result, &fstat_cb);
      } else { // open file fail
        Handle<FileReq> handle(req_); req_->ctx()->abort_and_release();
        async_ret_err_callback(req_->ctx()->_cb, req, req_->ctx()->loop());
      }
    }
  };
  
  auto task = NewRetain<Task>(offset, cb);
  auto req = New<FileReq>(task);
  task->_req = req;
  
  uv_fs_open(_uv_loop(), req->req(), Path::restore_c(path), O_RDONLY, 0, &Task::open_cb);
  
  return task->id();
}

uint FileHelper::read_file_stream(cString& path, Callback cb) { 
  return read_file_stream(path, -1, cb);
}

void FileHelper::is_file(cString& path, Callback cb) {
  uv_fs_stat(_uv_loop(), New<FileReq2>(cb)->req(), Path::restore_c(path), &is_file_cb);
}

void FileHelper::is_dir(cString& path, Callback cb) {
  is_dir2(path, cb, true);
}

void FileHelper::stat(cString& path, Callback cb) {
  file_helper_stat2(path, cb, true);
}

void FileHelper::exists(cString& path, Callback cb) {
  return exists2(path, cb, true);
}

void FileHelper::readable(cString& path, Callback cb) {
  uv_fs_access(_uv_loop(), New<FileReq2>(cb)->req(), Path::restore_c(path), R_OK, &uv_fs_access_cb);
}

void FileHelper::writable(cString& path, Callback cb) {
  uv_fs_access(_uv_loop(), New<FileReq2>(cb)->req(), Path::restore_c(path), W_OK, &uv_fs_access_cb);
}

void FileHelper::executable(cString& path, Callback cb) {
  uv_fs_access(_uv_loop(), New<FileReq2>(cb)->req(), Path::restore_c(path), X_OK, &uv_fs_access_cb);
}

// ---------------------------------------FileHelper END------------------------------------------

// Path implementation

static String split_path(cString& path, bool basename) {
  String s = path;
#if av_win
  s = s.replace_all('\\', '/');
#endif
  int start = 0;
  if (path[path.length() - 1] == '/') {
    start = 1;
  }
  
  int index = s.last_index_of("/", start);
  if (index != -1) {
    if (basename) {
      return s.substring(index + 1, s.length() - start);
    } else { // dirname
      return s.substring(0, index);
    }
  } else {
    if (basename) {
      return s;
    } else {
      return String::empty;
    }
  }
}

String Path::basename(cString& path) {
  return split_path(path, true);
}

String Path::dirname(cString& path) {
  return split_path(path, false);
}

String Path::extname(cString& path) {
  String s = split_path(path, true);
  int index = s.last_index_of('.');
  if (index != -1) {
    return s.substr(index);
  }
  return String::empty;
}

void Path::set_cwd_for_app_dir(){
  set_cwd(Path::dirname(executable()));
}

String Path::cwd() {
#if av_win
  char cwd[1100] = { 'f', 'i', 'l', 'e', ':', '/', '/', '/' };
  _getcwd(cwd + 8, 1024);
  String str = String(cwd).replace_all('\\', '/');
  if (str.length() == 10)
    str.push('/'); //
  return str;
#else
  char cwd[1100] = { 'f', 'i', 'l', 'e', ':', '/', '/' };
  getcwd(cwd + 7, 1024);
  return cwd;
#endif
}

bool Path::set_cwd(cString& path) {
  String str = format("%s", *path);
#if av_win
  return _chdir(*path.substr(8)) == 0;
#else
  return chdir(*path.substr(7)) == 0;
#endif
}

static const String chars("ABCDEFGHIJKMLNOPQRSTUVWXYZabcdefghijkmlnopqrstuvwxyz");

bool Path::is_local_absolute(cString& path) {
#if av_win
  if (chars.index_of(s[0]) != -1 && path[1] == ':') {
    return true;
  }
#else
  if (path[0] == '/') {
    return true;
  }
#endif
  else if ( is_local_zip(path) || is_local_file( path ) ) {
    return true;
  }
  return false;
}

bool Path::is_local_zip(cString& path) {
  if (
      (path[0] == 'z' || path[0] == 'Z') &&
      (path[1] == 'i' || path[1] == 'I') &&
      (path[2] == 'p' || path[2] == 'P') &&
      path[3] == ':' &&
      path[4] == '/' &&
      path[5] == '/' &&
      path[6] == '/') {
    return true;
  }
  return false;
}

bool Path::is_local_file(cString& path) {
  if (
      (path[0] == 'f' || path[0] == 'F') &&
      (path[1] == 'i' || path[1] == 'I') &&
      (path[2] == 'l' || path[2] == 'L') &&
      (path[3] == 'e' || path[3] == 'E') &&
      path[4] == ':' &&
      path[5] == '/' &&
      path[6] == '/' &&
      path[7] == '/') {
    return true;
  }
  return false;
}

String inl__format_part_path(cString& path) {
  
  Array<String> ls = path.split('/');
  Array<String> rev;
  int up = 0;
  for (int i = ls.length() - 1; i > -1; i--) {
    String& v = ls[i];
    if (!v.is_empty() && v != '.') {
      if (v[0] == '.' && v[1] == '.') { // set up ../
        up++;
      }
      else if (up == 0) { // no up item
        rev.push(v);
      }
      else { // un up
        up--;
      }
    }
  }
  
  String s = String::empty;
  
  if (rev.length()) {
    // reverse
    for (int i = rev.length() - 1; i > 0; i--) {
      s.push(rev[i]).push('/');
    }
    s.push(rev[0]);
  }
  return s;
}

String Path::format(cString& path) {
  
  String s = path;
  
#if av_win
  s = s.replace_all('\\', '/');
  
  String prefix;
  
  if (s[0] == '/') { // absolute path
    // add windows drive letter
    // file:///c:/
    prefix = cwd().substr(0, 11);
  }
  else if ( s.length() > 7 && is_local_zip(s) ) {
    
    if (chars.index_of(s[7]) != -1 && s[8] == ':') { // zip:///c:
      if (s.length() < 10) {
        return s.substr(0, 9).push('/');
      } else if (s[9] == '/') { // zip:///c:/
        prefix = s.substr(0, 10);
        s = s.substr(10);
      } else { // invalid windows path
        prefix = "zip:///"; // unix path ?
        s = s.substr(7);
      }
    } else {
      prefix = "zip:///"; // unix path ?
      s = s.substr(7);
    }
    
  } else if ( s.length() >= 8 && is_local_file( s ) ) { // file:///
    
    if (chars.index_of(s[8]) != -1 && s[9] == ':') { // file:///c:
      if (s.length() < 11) {
        return s.substr(0, 10).push('/');
      } else if (s[10] == '/') { // file:///c:/
        prefix = s.substr(0, 11);
        s = s.substr(11);
      } else { // invalid windows path
        prefix = "file:///"; // unix path ?
        s = s.substr(8);
      }
    } else {
      prefix = "file:///"; // unix path ?
      s = s.substr(8);
    }
  } else { // Relative path
    if (s.length() >= 2 &&
        chars.index_of(s[0]) != -1 && s[1] == ':' &&
        (s.length() < 3 || s[2] == '/')
        ) { // Windows absolute path
      prefix = String("file:///").push(*s, 2).push('/');
      s = s.substr(2);
    }
    else {
      // file:///c:/
      prefix = cwd().substr(0, 11);
      s = cwd().substr(11).push('/').push(s);
    }
  }
#else
  String prefix = "file:///";
  if (s[0] == '/') { // absolute path
    //
  } else {
    if ( s.length() > 7 && is_local_zip(s) ) {
      prefix = "zip:///";
      s = s.substr(7);
    }
    else if (s.length() >= 8 && is_local_file( s ) ) {
      s = s.substr(8);
    } else { // Relative path
      s = cwd().substr(8).push('/').push(s);
    }
  }
#endif
  
  s = inl__format_part_path(s);
  
  return prefix.push( s );
}

String Path::format(cchar* path, ...) {
  
  av_string_format(path, s);
  
  return format(s);
}

String Path::restore(cString& path) {
  return restore_c(path);
}

cchar* Path::restore_c(cString& path) {
#if av_win
  if ( is_local_zip(path) ) {
    return path.c() + 7;
  }
  else if ( is_local_file(path) ) {
    return path.c() + 8;
  }
#else
  if ( is_local_zip(path) ) {
    return path.c() + 6;
  }
  else if ( is_local_file(path) ) {
    return path.c() + 7;
  }
#endif
  return *path;
}

av_end
