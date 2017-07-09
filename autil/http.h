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

#ifndef __avocado__http__
#define __avocado__http__

#include "handle.h"
#include "string.h"
#include "map.h"
#include "event.h"
#include "error.h"
#include "thread.h"
#include "fs.h"

av_ns(avocado)

/**
 * @enum URIType
 */
enum URIType {
  URI_UNKNOWN,
  URI_FILE,
  URI_ZIP,
  URI_HTTP,
  URI_HTTPS,
  URI_WS,
  URI_WSS,
  URI_FTP,
  URI_FTPS,
  URI_SFTP,
};

/**
 * @enum HttpMethod
 */
enum HttpMethod {
  HTTP_METHOD_GET = 0,
  HTTP_METHOD_POST = 1,
  HTTP_METHOD_HEAD = 2,
  HTTP_METHOD_DELETE = 3,
  HTTP_METHOD_PUT = 4,
};

/**
 * @enum HttpReadyState
 */
enum HttpReadyState {
  HTTP_READY_STATE_INITIAL = 0,        //（初始） 还没有调用send发送请求
  HTTP_READY_STATE_READY   = 1,        // (准备) 已经调用send进入就绪状态等待打开连接或缓存文件
  HTTP_READY_STATE_SENDING = 2,        //（发送） http socket连接打开,开始发送请求
  HTTP_READY_STATE_RESPONSE = 3,       //（接收） 数据发送完成，正在接收数据
  HTTP_READY_STATE_COMPLETED = 4       //（完成） 响应完成,请求结束
};

/**
 * @class URI
 */
class URI: public Object {
public:
  
  URI();
  URI(cString& src);
  
  inline bool operator==(const URI& uri) const { return _href == uri.href(); }
  inline bool operator!=(const URI& uri) const { return _href != uri.href(); }
  inline bool is_null() const { return _href.is_empty(); }

  // attributes
  inline URIType type() const { return _uritype; }
  inline cString& href() const { return _href; }
  inline cString& host() const { return _host; }
  inline uint     port() const { return _port; }
  inline cString& hostname() const { return _hostname; }
  inline cString& domain() const { return _domain; }
  inline cString& origin() const { return _origin; }
  inline cString& pathname() const { return _pathname; }
  inline cString& dir() const { return _dir; }
  inline cString& basename() const { return _basename; }
  inline cString& extname() const { return _extname; }
  inline cString& search() const { return _search; }
  
  static String encode(cString& url);
  static String decode(cString& url);
  
private:
  URIType _uritype;
  String _href;
  String _host;
  uint16 _port;
  String _hostname;
  String _domain;
  String _origin;
  String _pathname;
  String _dir;
  String _basename;
  String _extname;
  String _search;
};

/**
 * @class HttpClientRequest 需注意线程安全问题
 */
class HttpClientRequest: public Object, public SimpleStream {
  av_hidden_all_copy(HttpClientRequest);
public:
  
  class Delegate {
  public:
    virtual void trigger_http_error(HttpClientRequest* req, cError& error) { }
    virtual void trigger_http_write(HttpClientRequest* req) { }
    virtual void trigger_http_header(HttpClientRequest* req) { }
    virtual void trigger_http_data(HttpClientRequest* req, Buffer buffer) { }
    virtual void trigger_http_end(HttpClientRequest* req) { }
    virtual void trigger_http_readystate_change(HttpClientRequest* req) { }
    virtual void trigger_http_timeout(HttpClientRequest* req) { }
    virtual void trigger_http_abort(HttpClientRequest* req) { }
  };
  
  HttpClientRequest(RunLoop* loop = RunLoop::current());
  
  virtual ~HttpClientRequest();
  
  /**
   * @func set_delegate
   */
  void set_delegate(Delegate* delegate) av_def_err;
  
  /**
   * @func set_method
   */
  void set_method(HttpMethod method) av_def_err;
  
  /**
   * @func set_url
   */
  void set_url(cString& path) av_def_err;
  
  /**
   * @func set_save_path
   */
  void set_save_path(cString& path) av_def_err;
  
  /**
   * @func set_username
   */
  void set_username(cString& username) av_def_err;
  
  /**
   * @func set_password
   */
  void set_password(cString& password) av_def_err;
  
  /**
   * @func disable_cache
   */
  void disable_cache(bool disable) av_def_err;
  
  /**
   * @func disable_cookie
   */
  void disable_cookie(bool disable) av_def_err;
  
  /**
   * @func disable_send_cookie
   */
  void disable_send_cookie(bool disable) av_def_err;
  
  /**
   * @func disable_ssl_verify
   */
  void disable_ssl_verify(bool disable) av_def_err;
  
  /**
   * @func set_keep_alive
   */
  void set_keep_alive(bool keep_alive) av_def_err;
  
  /**
   * @func set_timeout 超时未完成终止请求
   */
  void set_timeout(uint64 timeout_us) av_def_err;
  
  /**
   * @func set_request_header
   */
  void set_request_header(cString& name, cString& value) av_def_err;
  
  /**
   * @func set_form # 设置表单
   */
  void set_form(cString& form_name, cString& value) av_def_err;
  
  /**
   * @func set_upload # 设置上传文件路径
   */
  void set_upload_file(cString& form_name, cString& path) av_def_err;
  
  /**
   * @func clear_request_header
   */
  void clear_request_header() av_def_err;
  
  /**
   * @func clear_form_data
   */
  void clear_form_data() av_def_err;
  
  /**
   * @func get_response_header
   */
  String get_response_header(cString& name);
  
  /**
   * @func get_all_response_headers
   */
  const Map<String, String>& get_all_response_headers();
  
  /**
   * @func upload_total
   */
  int64 upload_total() const;
  
  /**
   * @func upload_size
   */
  int64 upload_size() const;
  
  /**
   * @func download_total
   */
  int64 download_total() const;
  
  /**
   * @func download_size
   */
  int64 download_size() const;
  
  /**
   * @func ready_state
   */
  HttpReadyState ready_state() const;
  
  /**
   * @func status_code
   */
  uint status_code() const;
  
  /**
   * @func url
   */
  String url() const;
  
  /**
   * @func send 发送请求,如果设置data参数会覆盖之前设置的表单数据
   */
  void send(Buffer data = Buffer()) av_def_err;
  
  /**
   * @func send
   */
  void send(cString& data) av_def_err;
  
  /**
   * @overwrite
   */
  virtual void pause();
  
  /**
   * @overwrite
   */
  virtual void resume();
  
  /**
   * @func abort current sending request
   */
  void abort();
  
private:
  
  av_def_inl_cls(Inl);
  
  Inl* m_inl;
};

/**
 * @class HttpError
 */
class HttpError: public Error {
public:
  
  inline HttpError(int rc, cString& msg, uint status, cString& url)
  : Error(rc, msg), m_status(status), m_url(url) {
  }
  
  inline HttpError(const Error& err): Error(err), m_status(0), m_url() { }
  inline uint status() const { return m_status; }
  inline String url() const { return m_url; }
  
private:
  uint   m_status;
  String m_url;
};

/**
 * @class HttpHelper
 */
class HttpHelper {
public:

  /**
   * @struct RequestOptions
   */
  struct RequestOptions {
    String              url;
    HttpMethod          method;
    Map<String, String> headers;    /* setting custom request headers */
    Buffer              post_data;  /* Non post requests ignore this option */
    String              save;       /* save body content to local disk */
    String              upload;     /* upload loacl file */
    bool                disable_ssl_verify;
    bool                disable_cache;
    bool                disable_cookie;
    //@end
  };
  
  /**
   * @func request
   */
  static uint request(RequestOptions& options, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func request_stream
   */
  static uint request_stream(RequestOptions& options, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func request_sync
   */
  static Buffer request_sync(RequestOptions& options) throw(HttpError);
  
  /**
   * @func download
   */
  static uint download(cString& url, cString& save, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func download_sync
   */
  static void download_sync(cString& url, cString& save) throw(HttpError);
  
  /**
   * @func upload
   */
  static uint upload(cString& url, cString& file, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func upload
   */
  static Buffer upload_sync(cString& url, cString& file) throw(HttpError);
  
  /**
   * @func get
   */
  static uint get(cString& url, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func get_stream
   */
  static uint get_stream(cString& url, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func post
   */
  static  uint post(cString& url, Buffer data, Callback cb = Callback()) throw(HttpError);
  
  /**
   * @func get_sync
   */
  static Buffer get_sync(cString& url) throw(HttpError);
  
  /**
   * @func post_sync
   */
  static Buffer post_sync(cString& url, Buffer data) throw(HttpError);
  
  /**
   * @func abort
   */
  static void abort(uint id);
  
  /**
   * @func initialize
   */
  static void initializ();
  
  /**
   * @func user_agent
   */
  static String user_agent();

  /**
   * @func set_user_agent
   */
  static void set_user_agent(cString& user_agent);
  
  /**
   * @func ssl_cacert_file
   */
  static String ssl_cacert_file();
  
  /**
   * @func set_ssl_cacert_file
   */
  static void set_ssl_cacert_file(cString& path);
  
  /**
   * @func set_ssl_client_key_file
   */
  static void set_ssl_client_key_file(cString& path);
  
  /**
   * @func set_ssl_client_keypasswd
   */
  static void set_ssl_client_keypasswd(cString& passwd);
  
  /**
   * @func cache_path 
   */
  static String cache_path();
  
  /**
   * @func set_cache_path setting cache file path
   */
  static void set_cache_path(cString& path);
  
  /**
   * @func clear_cache clear web cache
   */
  static void clear_cache();
  
  /**
   * @func clear_cookie
   */
  static void clear_cookie();
  
};

av_end
#endif
