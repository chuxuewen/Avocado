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

#ifndef __avocado__fs__
#define __avocado__fs__

#include "cb.h"
#include "string.h"
#include "handle.h"
#include "list.h"
#include "thread.h"

/**
 * @ns avocado
 */

av_ns(avocado)

/**
 * @enum FileOpenMode # File open mode
 */
enum FileOpenMode {
  FOPEN_READ = 0,   // r  以只读方式打开文件，该文件必须存在。
  FOPEN_WRITE,      // w  打开只写文件，若文件存在则文件长度清为零，即该文件内容会消失。
                    //    若文件不存在则建立该文件。
  FOPEN_APPEND,     // a  以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，
                    //    写入的数据会被加到文件尾，即文件原先的内容会被保留。
  FOPEN_READ_PLUS,  // r+ 以可读写方式打开文件，该文件必须存在。
  FOPEN_WRITE_PLUS, // w+ 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。
                    //    若文件不存在则建立该文件。
  FOPEN_APPEND_PLUS,// a+	以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，
                    //    写入的数据会被加到文件尾后，即文件原先的内容会被保留。
  FOPEN_NUM,        // num
  FOPEN_R = FOPEN_READ,                   // r
  FOPEN_W = FOPEN_WRITE,                  // w
  FOPEN_A = FOPEN_APPEND,                 // a
  FOPEN_RP = FOPEN_READ_PLUS,             // r+
  FOPEN_WP = FOPEN_WRITE_PLUS,            // w+
  FOPEN_AP = FOPEN_APPEND_PLUS,           // a+
};

enum FileType {
  FILE_UNKNOWN,
  FILE_FILE,
  FILE_DIR,
  FILE_LINK,
  FILE_FIFO,
  FILE_SOCKET,
  FILE_CHAR,
  FILE_BLOCK
};

class Dirent: public Object {
public:
  Dirent() { }
  Dirent(cString& n,
         cString& p, FileType t): name(n), pathname(p), type(t) { }
  String name;
  String pathname;
  FileType type;
};

/**
 * @class FileStat
 */
class FileStat: public Object {
  av_hidden_all_copy(FileStat);
public:
  FileStat();
  FileStat(cString& path);
  FileStat(FileStat&& stat);
  FileStat& operator=(FileStat&& stat);
  virtual ~FileStat();
  bool is_valid() const;
  bool is_file() const;
  bool is_dir() const;
  bool is_link() const;
  bool is_sock() const;
  uint64 mode() const;
  FileType type() const;
  uint64 group() const;
  uint64 owner() const;
  uint64 nlink() const;
  uint64 ino() const;
  uint64 blksize() const;
  uint64 blocks() const;
  uint64 flags() const;
  uint64 gen() const;
  uint64 dev() const;
  uint64 rdev() const;
  uint64 size() const;
  uint64 atime() const;
  uint64 mtime() const;
  uint64 ctime() const;
  uint64 birthtime() const;
private:
  av_def_inl_cls(Inl);
  void* m_stat;
};

/**
 * @class FileProtocol
 */
class FileProtocol {
public:
  typedef ProtocolCharacteristic Characteristic;
  virtual bool is_open() = 0;
  virtual bool open(FileOpenMode mode = FOPEN_R) = 0;
  virtual bool close() = 0;
  virtual int read(void* buffer, uint64 size, int64 offset = -1) = 0;
  virtual int write(const void* buffer, uint64 size, int64 offset = -1) = 0;
};

/**
 * class AsyncFileProtocol
 */
class AsyncFileProtocol {
public:
  typedef ProtocolCharacteristic Characteristic;
  class Delegate {
  public:
    virtual void trigger_async_file_open(AsyncFileProtocol* file) { }
    virtual void trigger_async_file_close(AsyncFileProtocol* file) { }
    virtual void trigger_async_file_error(AsyncFileProtocol* file, cError& error) { }
    virtual void trigger_async_file_read(AsyncFileProtocol* file, Buffer buffer, int mark) { }
    virtual void trigger_async_file_write(AsyncFileProtocol* file, Buffer buffer, int mark) { }
  };
  virtual void set_delegate(Delegate* delegate) = 0;
  virtual bool is_open() = 0;
  virtual void open(FileOpenMode mode = FOPEN_READ) = 0;
  virtual void close() = 0;
  virtual void read(Buffer buffer, int64 size = -1, int64 offset = -1, int mark = 0) = 0;
  virtual void write(Buffer buffer, int64 size = -1, int64 offset = -1, int mark = 0) = 0;
};

/**
 * @class File
 */
class File: public Object, public FileProtocol {
  av_hidden_all_copy(File);
public:
  typedef DefaultCharacteristic Characteristic;

  inline File(cString& path): m_path(path), m_fp(0) { }
  virtual ~File();
  inline  String path() const { return m_path; }
  virtual bool is_open();
  virtual bool open(FileOpenMode mode = FOPEN_R);
  virtual bool close();
  virtual int read(void* buffer, uint64 size, int64 offset = -1);
  virtual int write(const void* buffer, uint64 size, int64 offset = -1);
private:
  String m_path;
  int    m_fp;
};

/**
 * @class AsyncFile
 */
class AsyncFile: public Object, public AsyncFileProtocol {
  av_hidden_all_copy(AsyncFile);
public:
  typedef DefaultCharacteristic Characteristic;

  AsyncFile(cString& path, RunLoop* loop = RunLoop::current());
  virtual ~AsyncFile();
  String path() const;
  virtual void set_delegate(Delegate* delegate);
  virtual bool is_open();
  virtual void open(FileOpenMode mode = FOPEN_R);
  virtual void close();
  virtual void read(Buffer buffer, int64 size = -1, int64 offset = -1, int mark = 0);
  virtual void write(Buffer buffer, int64 size = -1, int64 offset = -1, int mark = 0);
private:
  av_def_inl_cls(Inl);
  Inl* m_inl;
};

/**
 * @class FileHelper
 * @static
 */
class FileHelper {
public:
  static const uint default_mode;
  
  /**
   * @func each 递归遍历子子文件与子目录, 遍历回调回返0停止遍历
   */
  static bool each(cString& path, Callback cb, bool internal = false);
  
  /**
   * @func chmod_sync
   */
  static bool chmod_sync(cString& path, uint mode = default_mode);
  
  /**
   * @func chmod_p  # 递归设置
   *                # 多线程中,设置stop_signal值为true来终止操作
   */
  static bool chmod_r_sync(cString& path, uint mode = default_mode, bool* stop_signal = nullptr);
  static bool chown_sync(cString& path, uint owner, uint group);
  static bool chown_r_sync(cString& path, uint owner, uint group, bool* stop_signal = nullptr);
  static bool mkdir_sync(cString& path, uint mode = default_mode);
  static bool mkdir_p_sync(cString& path, uint mode = default_mode);
  static bool rename_sync(cString& name, cString& new_name);
  static bool mv_sync(cString& name, cString& new_name);
  static bool unlink_sync(cString& path);
  static bool rmdir_sync(cString& path);
  static bool rm_r_sync(cString& path, bool* stop_signal = nullptr);
  static bool cp_sync(cString& source, cString& target, bool* stop_signal = nullptr);
  static bool cp_r_sync(cString& source, cString& target, bool* stop_signal = nullptr);
  static Array<Dirent> ls_sync(cString& path);
  static Array<FileStat> ls_l_sync(cString& path);
  static bool write_file_sync(cString& path, const void* buffer, uint64 size, int64 offset = -1);
  static bool write_file_sync(cString& path, cString& str);
  template<class T>
  static bool write_file_sync(cString& path, const ArrayBuffer<T>& data, int64 offset = -1) {
    return write_file_sync(path, *data, data.size(), offset );
  }
  static Buffer read_file_sync(cString& path, int64 size = -1, int64 offset = -1);
  static bool is_file_sync(cString& path);
  static bool is_dir_sync(cString& path);
  static FileStat stat_sync(cString& path);
  static bool exists_sync(cString& path);
  static bool readable_sync(cString& path);
  static bool writable_sync(cString& path);
  static bool executable_sync(cString& path);
  // async
  static void abort(uint id);
  static void chmod(cString& path, uint mode = default_mode, Callback cb = Callback());
  static void chown(cString& path, uint owner, uint group, Callback cb = Callback());
  static void mkdir(cString& path, uint mode = default_mode, Callback cb = Callback());
  static void mkdir_p(cString& path, uint mode = default_mode, Callback cb = Callback());
  static void rename(cString& name, cString& new_name, Callback cb = Callback());
  static void mv(cString& name, cString& new_name, Callback cb = Callback());
  static void unlink(cString& path, Callback cb = Callback());
  static void rmdir(cString& path, Callback cb = Callback());
  static void ls(cString& path, Callback cb = Callback());
  static void is_file(cString& path, Callback cb = Callback());
  static void is_dir(cString& path, Callback cb = Callback());
  static void stat(cString& path, Callback cb = Callback());
  static void exists(cString& path, Callback cb = Callback());
  static void readable(cString& path, Callback cb = Callback());
  static void writable(cString& path, Callback cb = Callback());
  static void executable(cString& path, Callback cb = Callback());
  static uint chmod_r(cString& path, uint mode = default_mode, Callback cb = Callback());
  static uint chown_r(cString& path, uint owner, uint group, Callback cb = Callback());
  static uint rm_r(cString& path, Callback cb = Callback());
  static uint cp(cString& source, cString& target, Callback cb = Callback());
  static uint cp_r(cString& source, cString& target, Callback cb = Callback());
  static void ls_l(cString& path, Callback cb = Callback());
  static void write_file(cString& path, Buffer buffer, uint64 size, int64 offset = -1, Callback cb = Callback());
  static void write_file(cString& path, cString& str, Callback cb = Callback());
  template<class T>
  inline static void write_file(cString& path, ArrayBuffer<T> data, int64 offset = -1, Callback cb = Callback()) {
    uint size = data.size();
    write_file(path, Buffer((char*)data.collapse(), size), size, offset, cb);
  }
  static void read_file(cString& path, int64 size, int64 offset = -1, Callback cb = Callback());
  static void read_file(cString& path, Callback cb = Callback());
  static uint read_file_stream(cString& path, int64 offset = -1, Callback cb = Callback());
  static uint read_file_stream(cString& path, Callback cb);
};

/**
 * @class Path
 * @static
 */
class Path {
public:
  /**
   * @func extname {String} # Get the path basename
   * @ret {String}
   */
  static String basename(cString& path);
  
  /**
   * @func extname {String} # Get the path dirname
   * @arg path {cString&}
   * @ret {String}
   */
  static String dirname(cString& path);
  
  /**
   * @func extname # Get the path extname
   * @arg path {cString&}
   * @ret {String}
   */
  static String extname(cString& path);
  
  /**
   * @func executable_path # Get the executable path
   * @ret {cString&}
   */
  static String executable();
  
  /**
   * @func documents_dir # Get the documents dir.
   * @ret {cString&} # The path that can be write/read a file in
   */
  static String documents(cString& path = String::empty);
  
  /**
   * @func temp_dir # Get the temp dir.
   * @ret {cString&} # The path that can be write/read a file in
   */
  static String temp(cString& path = String::empty);
  
  /**
   * @func resources_dir # Get the resoures dir
   * @ret {cString&}
   */
  static String resources(cString& path = String::empty);
  
  /**
   * @func is_absolute # Is absolute path
   * @ret {bool}
   */
  static bool is_local_absolute(cString& path);
  
  /**
   * @func is_local_zip
   */
  static bool is_local_zip(cString& path);
  
  /**
   * @func is_local_file
   */
  static bool is_local_file(cString& path);
  
  /**
   * @func format
   * @arg format {cchar*}
   * @arg [...] {cchar*}
   * @ret {String}
   */
  static String format(cchar* path, ...);
  
  /**
   * @func format
   */
  static String format(cString& path);
  
  /**
   * @func restore
   */
  static String restore(cString& path);
  
  /**
   * @func restore_c
   */
  static cchar* restore_c(cString& path);
  
  /**
   * @func set_cwd_for_app_dir # 设置cwd为当前程序目录
   */
  static void set_cwd_for_app_dir();
  
  /**
   * @func cwd # Getting current working directory
   * @ret {String}
   * @static
   */
  static String cwd();
  
  /**
   * @func set_cwd # Setting current working directory
   * @arg path {cString&}
   * @ret {int}
   * @static
   */
  static bool set_cwd(cString& path);
  
};

/**
 * @class FileSearch # Resources files search
 */
class FileSearch: public Object {
  av_hidden_all_copy(FileSearch);
public:
  
  FileSearch();
  
  /**
   * @destructor
   */
  virtual ~FileSearch();
  
  /**
   * 添加一个搜索路径,路径必需存在
   */
  void add_search_path(cString& path);
  
  /**
   * @func add_zip_search_path # 添加一个zip包内的搜索路径,只能添加没加密的zip包
   */
  void add_zip_search_path(cString& zip_path, cString& path);
  
  /**
   * @func get_search_paths # Gets the array of search paths.
   */
  Array<String> get_search_paths() const;
  
  /**
   * @func remove_search_path # remove search path
   */
  void remove_search_path(cString& path);
  
  /**
   * @func func # Removes all search paths.
   */
  void remove_all_search_path();
  
  /**
   *
   * To obtain the absolute path to the file already exists.
   * If no such file returns the empty string ""
   * If it is a zip package path, will return with the prefix "zip:///home/xxx/test.apk@/assets/bb.jpg"
   * @func get_absolute_path
   */
  String get_absolute_path(cString& path) const;
  
  /**
   * @func exists # Find the file exists
   */
  bool exists(cString& path) const;
  
  /**
   * @func read_file Read the all file data and return Data
   */
  Buffer read(cString& path) const;
  
  /**
   * @func share # Gets the instance of FileSearch.
   */
  static FileSearch* shared();
  
private:
  class SearchPath;
  class ZipInSearchPath;
  List<SearchPath*> m_search_paths; // Search path list
};

/**
 * @class FileReader
 */
class FileReader: public Object {
  av_hidden_all_copy(FileReader);
public:
  
  FileReader();
  FileReader(FileReader&& reader);
  
  /**
   * @destructor
   */
  virtual ~FileReader();
  
  /**
   * @func read
   */
  virtual uint read(cString& path, Callback cb = Callback());
  
  /**
   * @func read_stream
   */
  virtual uint read_stream(cString& path, Callback cb = Callback());
  
  /**
   * @func read_sync
   */
  virtual Buffer read_sync(cString& path) av_def_err;
  
  /**
   * @func abort
   */
  virtual void abort(uint id);
  
  /**
   * @func exists
   */
  virtual bool exists(cString& path);
  
  /**
   * @func ls(path)
   */
  virtual Array<Dirent> ls(cString& path);
  
  /**
   * @func is_absolute
   */
  virtual bool is_absolute(cString& path);
  
  /**
   * @func format
   */
  virtual String format(cString& path);
  
  /**
   * @func clear
   */
  virtual void clear();

  /**
   * @func set_shared_instance
   */
  static void set_shared_instance(FileReader* reader);
  
  /**
   * @func shared
   */
  static FileReader* shared();
  
private:
  
  class Core; Core* core_;
};

av_inline FileSearch* fsearch() {
  return FileSearch::shared();
}

av_inline FileReader* freader() {
  return FileReader::shared();
}

av_end
#endif
