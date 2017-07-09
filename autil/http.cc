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

#include "http.h"
#include "http-cookie.h"
#include "fs.h"
#include "version.h"
#include "string-builder.h"
#include "sys.h"
#include "net.h"
#include <http_parser.h>
#include <zlib.h>

av_ns(avocado)

typedef HttpClientRequest::Delegate HttpDelegate;

static uint   http_initialized = 0;
static String http_user_agent = "Mozilla/5.0 Avocado/" AVOCADO_VERSION " (KHTML, like Gecko)";
static String http_cache_path = String::empty;
static const String string_method[5] = { "GET", "POST", "HEAD", "DELETE", "PUT" };
static const String string_colon(": ");
static const String string_space(" ");
static const String string_end("\r\n");
static const String string_max_age("max-age=");
static HttpDelegate default_delegate;
static const String content_type_form("application/x-www-form-urlencoded; charset=utf-8");
static const String content_type_multipart_form("multipart/form-data; "
                                                "boundary=----AvocadoFormBoundaryrGKCBY7qhFd3TrwA");
static const String multipart_boundary_start("------AvocadoFormBoundaryrGKCBY7qhFd3TrwA\r\n");
static const String multipart_boundary_end  ("------AvocadoFormBoundaryrGKCBY7qhFd3TrwA--");

#define MAX_CONNECT_COUNT (5)
#define BUFFER_SIZE (65536)

// ----------------------------- URL -----------------------------

URI::URI(): _uritype(URI_UNKNOWN), _port(0) { }

URI::URI(cString& src): _uritype(URI_UNKNOWN), _href(src), _port(0) {
  
  String s = src.substr(0, 9).to_lower_case();
  
  if ( s.index_of("file:///") == 0 ) {
    _uritype = URI_FILE;
    _pathname = src.substr(7);
    _origin = "file://";
  } else if ( s.index_of("zip:///") == 0 ) {
    _uritype = URI_ZIP;
    _pathname = src.substr(6);
    _origin = "zip://";
  }
  else {
    int start = 0;
    
    if ( s.index_of("http://") == 0 ) {
      start = 7;
      _uritype = URI_HTTP;
      _origin = "http://";
    } else if ( s.index_of("https://") == 0 ) {
      start = 8;
      _uritype = URI_HTTPS;
      _origin = "https://";
    } else if ( s.index_of("ws://") == 0 ) {
      start = 5;
      _uritype = URI_WS;
      _origin = "ws://";
    } else if ( s.index_of("wss://") == 0 ) {
      start = 6;
      _uritype = URI_WSS;
      _origin = "wss://";
    } else if ( s.index_of("ftp://") == 0 ) {
      start = 6;
      _uritype = URI_FTP;
      _origin = "ftp://";
    } else if ( s.index_of("ftps://") == 0 ) {
      start = 7;
      _uritype = URI_FTPS;
      _origin = "ftps://";
    } else if ( s.index_of("sftp://") == 0 ) {
      start = 7;
      _uritype = URI_SFTP;
      _origin = "sftp://";
    } else {
      return;
    }
    
    bool abort = false;
    int index = src.index_of('/', start);
    
    if (index == -1) {
      abort = true;
      _pathname = '/';
      _hostname = _host = src.substr(start);
    }
    else{
      _pathname = src.substr(index);
      _hostname = _host = src.substring(start, index);
    }
    
    _origin += _host;
    
    index = _hostname.last_index_of(':');
    if ( index != -1 ) {
      _port = _hostname.substr(index + 1).to_uint();
      _hostname = _hostname.substr(0, index);
    }
    
    _domain = _hostname;
    
    index = _domain.last_index_of('.');
    
    if (index != -1) {
      index = _domain.last_index_of('.', index);
      if (index != -1) {
        _domain = _domain.substr(index + 1);
      }
    }
    
    if (abort) return;
  }
  
  int index = _pathname.index_of('?');
  if (index != -1) {
    _search = _pathname.substr(index + 1);
    _pathname = _pathname.substr(0, index);
  }
  
  index = _pathname.last_index_of('/');
  _dir = _pathname.substr(0, index);
  _basename = _pathname.substr(index + 1);
  
  index = _basename.last_index_of('.');
  if (index != -1) {
    _extname = _basename.substr(index).lower_case();
  }
}

String URI::decode(cString& url) {
  int i = 0;
  size_t size = 0;
  size_t len = 0;
  char *dec = NULL;
  char tmp[3];
  char ch = 0;
  
  // chars len
  len = url.length();
  cchar* src = *url;
  
  // alloc
  dec = (char *) malloc(len + 1);
  
#define push(c) (dec[size++] = c)
  
  // decode
  while (len--) {
    ch = src[i++];
    
    // if prefix `%' then read byte and decode
    if ('%' == ch) {
      tmp[0] = src[i++];
      tmp[1] = src[i++];
      tmp[2] = '\0';
      push(strtol(tmp, NULL, 16));
    } else {
      push(ch);
    }
  }
  
  dec[size] = '\0';
  
#undef push
  
  return Buffer(dec, uint(size));
}

#define IN_URANGE(a,b,c) (a >= (unsigned int) b && a <= (unsigned int) c)

static inline int needs_encoding(char ch, char next
                                 , bool component
                                 , bool secondary) {
  if (IN_URANGE(ch, 0xD800, 0xDBFF)) {
    if (!IN_URANGE(next, 0xDC00, 0xDFFF)) {
      return -1;
    }
  }
  
  // alpha capital/small
  if (IN_URANGE(ch, 0x0041, 0x005A) || IN_URANGE(ch, 0x061, 0x007A)) {
    return 0;
  }
  
  // decimal digits
  if (IN_URANGE(ch, 0x0030, 0x0039)) {
    return 0;
  }
  
  // reserved chars
  // - _ . ! ~ * ' ( )
  switch (ch) {
    case '-':
    case '_':
    case '.':
    case '!':
    case '~':
    case '*':
    case '(':
    case ')':
      return 0;
    default: break;
  }
  
  if ( !component ) {
    
    switch (ch) {
      case ';':
      case '/':
      case '?':
      case ':':
      case '@':
      case '&':
      case '=':
      case '+':
      case '$':
      case ',':
      case '#': return 0;
      case '%':
        if ( secondary ) return 0; // secondary encoding
      default: break;
    }
  }
  
  return 1;
}

static String uri_encode(cString& url, bool component = false, bool secondary = false) {
  
  byte ch = 0;
  uint len = url.length();
  cchar* src = *url;
  size_t msize = 0;
  
  for (int i = 0; i < len; ++i) {
    switch (needs_encoding(src[i], src[i+1], component, secondary)) {
      case -1: return String();
      case 0: msize++; break;
      case 1: msize = (msize + 3); break;
    }
  }
  
  // alloc with probable size
  char* enc = (char*)malloc((sizeof(char) * msize) + 1);
  if ( !enc ) {
    return String();
  }
  
  uint size = 0;
  cchar* hex = "0123456789ABCDEF";
  
  for ( int i = 0; i < len; i++ ) {
    ch = src[i];
    if (needs_encoding(ch, src[i], component, secondary)) {
      enc[size++] = '%';
      enc[size++] = hex[ch >> 4];
      enc[size++] = hex[ch & 0xf];
    } else {
      enc[size++] = ch;
    }
  }
  
  enc[size] = '\0';
  
  return Buffer(enc, uint(size));
}

String URI::encode(cString &url) {
  return uri_encode(url, true, false);
}

static String secondary_uri_encode(cString& url) {
  return uri_encode(url, false, true);
}

enum FormType {
  FORM_TYPE_TEXT,
  FORM_TYPE_FILE,
};

struct FormValue {
  FormType type;
  String   data;
  String   name;
};

/**
 * @class HttpClientRequest::Inl
 */
class HttpClientRequest::Inl: public Reference, public Socket::Delegate {
public:
  
  typedef HttpClientRequest::Inl Client;
  
  Inl(HttpClientRequest* host, RunLoop* loop)
  : _host(host)
  , _loop( loop ? loop->keep_alive() : nullptr )
  , _delegate(&default_delegate)
  , _upload_total(0)
  , _upload_size(0)
  , _download_total(0)
  , _download_size(0)
  , _ready_state(HTTP_READY_STATE_INITIAL)
  , _status_code(0)
  , _method(HTTP_METHOD_GET)
  , _connect(nullptr)
  , _cache_reader(nullptr)
  , _file_writer(nullptr)
  , _disable_cache(0)
  , _disable_cookie(0)
  , _disable_send_cookie(0)
  , _disable_ssl_verify(0)
  , _disable_ssl_verify_host(0)
  , _keep_alive(1)
  , _sending(0)
  , _timeout(0), _pause(false), _url_no_cache_arg(false), _wait_connect_id(0)
  {
    HttpHelper::initializ();
  }
  
  ~Inl() {
    av_assert( !_connect );
    av_assert( !_cache_reader );
    av_assert( !_file_writer );
    Release(_loop); _loop = nullptr;
  }
  
  static void post_io(Callback cb) {
    RunLoop::post_io(cb, true);
  }
  
  RunLoop* loop2() { return _loop ? _loop->host() : nullptr; }
  PostMessage* loop() { return _loop; }
  
  void set_delegate(HttpDelegate* delegate) {
    _delegate = delegate ? delegate : &default_delegate;
  }
  
  class Sending {
  public:
    typedef NonObjectCharacteristic Characteristic;
    Sending(Inl* host): _host(host) { Retain(host); }
    ~Sending() { Release(_host); }
    Inl* _host;
  };
  
  class Reader {
  public:
    virtual void read_advance() = 0;
    virtual void read_pause() = 0;
    virtual bool is_cache() = 0;
  };
  
  class ConnectPool;
  class Connect;
  typedef List<Connect*>::Iterator ConnectID;
  
  class Connect: public Object
  , public Socket::Delegate
  , public Socket::SocketDelegate
  , public Reader, public AsyncFile::Delegate {
  public:
    
    typedef List<Connect*>::Iterator ID;
    
    Connect(cString& hostname, uint16 port, bool ssl)
    : _ssl(ssl)
    , _socket(nullptr)
    , _client(nullptr)
    , _use(false), _upload_file(nullptr), _z_gzip(0) { //
      
      if ( _ssl ) {
        _socket = new SSLSocket(hostname, port, nullptr);
      } else {
        _socket = new Socket(hostname, port, nullptr);
      }
      
      av_assert(_socket);
      
      _socket->set_delegate(this);
      _socket->set_socket_delegate(this);
      
      _parser.data = this;
      http_parser_settings_init(&_settings);
      _settings.on_message_begin = &on_message_begin;
      _settings.on_status = &on_status;
      _settings.on_header_field = &on_header_field;
      _settings.on_header_value = &on_header_value;
      _settings.on_headers_complete = &on_headers_complete;
      _settings.on_body = &on_body;
      _settings.on_message_complete = &on_message_complete;
    }
    
    ~Connect() {
      av_assert(_id.is_null());
      Release(_socket);     _socket = nullptr;
      Release(_upload_file);_upload_file = nullptr;
    }
    
    void bind_client_and_send(Client* client) {
      av_assert(client);
      av_assert( !_client );
      
      _client = client;
      _socket->set_timeout(_client->_timeout); // set timeout
      
      if ( _socket->is_open() ) {
        send_http_request(); // send request
      } else {
        _socket->open();
      }
    }
    
    static int on_message_begin(http_parser* parser) {
      //av_debug("--http response parser on_message_begin");
      Connect* self = static_cast<Connect*>(parser->data);
      self->_client->trigger_http_readystate_change(HTTP_READY_STATE_RESPONSE);
      return 0;
    }
    
    static int on_status(http_parser* parser, const char *at, size_t length) {
      //av_debug("http response parser on_status, %s %s", String(at - 4, 3).c(), String(at, uint(length)).c());
      Connect* self = static_cast<Connect*>(parser->data);
      self->_client->_status_code = String(at - 4, 3).to_uint();
      return 0;
    }
    
    static int on_header_field(http_parser* parser, const char *at, size_t length) {
      //av_debug("http response parser on_header_field, %s", String(at, uint(length)).c());
      static_cast<Connect*>(parser->data)->_header_field = String(at, uint(length)).lower_case();
      return 0;
    }
    
    static int on_header_value(http_parser* parser, const char *at, size_t length) {
      //av_debug("http response parser on_header_value, %s", String(at, uint(length)).c());
      Connect* self = static_cast<Connect*>(parser->data);
      String value(at, uint(length));
      self->_header.set( move(self->_header_field), value );
      if ( !self->_client->_disable_cookie ) {
        if ( self->_header_field == "set-cookie" ) {
          http_cookie_set_with_expression(self->_client->_uri.domain(), value);
        }
      }
      return 0;
    }
    
    static int on_headers_complete(http_parser* parser) {
      //av_debug("--http response parser on_headers_complete");
      Connect* self = static_cast<Connect*>(parser->data);
      Client* cli = self->_client;
      if ( self->_header.has("content-length") ) {
        cli->_download_total = self->_header.get("content-length").to_int64();
      }
      self->init_gzip_parser();
      cli->trigger_http_header(cli->_status_code, move(self->_header), 0);
      return 0;
    }
    
    void init_gzip_parser() {
      if ( _header.has("content-encoding") ) {

        _z_strm.zalloc = Z_NULL;
        _z_strm.zfree = Z_NULL;
        _z_strm.opaque = Z_NULL;
        _z_strm.next_in = Z_NULL;
        _z_strm.avail_in = 0;
        
        String encoding = _header.get("content-encoding");
        if ( encoding.index_of("gzip") != -1 ) {
          _z_gzip = 2;
          inflateInit2(&_z_strm, 47);
        } else if ( encoding.index_of("deflate") != -1 ) {
          _z_gzip = 1;
          inflateInit(&_z_strm);
        }
      }
    }
    
    int gzip_inflate(cchar* data, uint len, Buffer& out) {
      static Buffer _z_strm_buff(16384); // 16k
      
      int r = 0;
      
      _z_strm.next_in = (byte*)data;
      _z_strm.avail_in = len;
      do {
        _z_strm.next_out = (byte*)*_z_strm_buff;
        _z_strm.avail_out = _z_strm_buff.length();
        r = inflate(&_z_strm, Z_NO_FLUSH);
        out.write(_z_strm_buff, -1, _z_strm_buff.length() - _z_strm.avail_out);
      } while(_z_strm.avail_out == 0);
      
      if ( r == Z_STREAM_END ) {
        inflateEnd(&_z_strm);
      }
      
      return r;
    }
    
    static int on_body(http_parser* parser, const char *at, size_t length) {
      //av_debug("--http response parser on_body, %d", length);
      Connect* self = static_cast<Connect*>(parser->data);
      self->_client->_download_size += length;
      Buffer buff;
      if ( self->_z_gzip ) {
        int r = self->gzip_inflate(at, uint(length), buff);
        if (r < 0) {
          av_err("un gzip err, %d", r);
        }
      } else {
        buff = WeakBuffer(at, uint(length)).copy();
      }
      if ( buff.length() ) {
        self->_client->trigger_http_data(buff, 0);
      }
      return 0;
    }
    
    static int on_message_complete(http_parser* parser) {
      //av_debug("--http response parser on_message_complete");
      static_cast<Connect*>(parser->data)->_client->http_response_complete();
      return 0;
    }
    
    void send_http_request() {
      
      http_parser_init(&_parser, HTTP_RESPONSE);
      Release(_upload_file); _upload_file = nullptr;
      _is_multipart_form_data = false;
      _send_data = false;
      _multipart_form_data.clear();
      _z_gzip = 0;
      _header.clear();
      //
      
      Map<String, String> header = _client->_request_header;
      
      header.set("Host", _client->_uri.host());
      header.set("Connection", _client->_keep_alive ? "keep-alive" : "close");
      header.set("Accept-Encoding", "gzip, deflate");
      header.set("Date", gmt_time_string(sys::time_second()));
      
      if ( !header.has("Cache-Control") )   header.set("Cache-Control", "max-age=0");
      if ( !header.has("User-Agent") )      header.set("User-Agent", http_user_agent);
      if ( !header.has("Accept-Charset") )  header.set("Accept-Charset", "utf-8");
      if ( !header.has("Accept") )          header.set("Accept", "*/*");
      if ( !header.has("DNT") )             header.set("DNT", "1");
      if ( !header.has("Accept-Language") ) header.set("Accept-Language", sys::languages_string());
      
      if ( !_client->_username.is_empty() && !_client->_password.is_empty() ) {
        String s = _client->_username + ':' + _client->_password;
        header.set("Authorization", Codec::encoding(Encoding::BASE64, s));
      }
      
      if ( !_client->_disable_cookie && !_client->_disable_send_cookie ) { // send cookies
        String cookies = http_cookie_get_all_string(_client->_uri.domain(),
                                                    _client->_uri.pathname(),
                                                    _client->_uri.type() == URI_HTTPS);
        if ( !cookies.is_empty() ) {
          header.set("Cookie", cookies);
        }
      }
      
      if ( _client->_cache_reader ) {
        String last_modified = _client->_cache_reader->header()["last-modified"];
        String etag = _client->_cache_reader->header()["etag"];
        if ( !last_modified.is_empty() )  {
          header.set("If-Modified-Since", move(last_modified) );
        }
        if ( !etag.is_empty() ) {
          header.set("If-None-Match", move(etag) );
        }
      }
      
      if ( _client->_method == HTTP_METHOD_POST ) {
        
        if ( _client->_post_data.length() ) { // ignore form data
          if ( _client->_post_form_data.length() ) {
            WARN("Ignore form data");
          }
          _client->_upload_total = _client->_post_data.length();
          header.set("Content-Length", _client->_upload_total);
        }
        else if ( _client->_post_form_data.length() ) { // post form data
            
          for ( auto& i : _client->_post_form_data ) {
            if ( i.value().type == FORM_TYPE_FILE ) {
              _is_multipart_form_data = true; break;
            }
          }
          
          if (_is_multipart_form_data ) {
            
            for ( auto& i : _client->_post_form_data ) {
              if ( i.value().type == FORM_TYPE_FILE ) {
                FileStat stat = FileHelper::stat_sync(i.value().data);
                if ( stat.is_valid() && stat.is_file() ) {
                  _client->_upload_total += stat.size();
                } else {
                  Error err(ERR_INVALID_PATH, "invalid upload path `%s`", i.value().data.c());
                  _client->report_error_and_abort(err);
                  return;
                }
              } else {
                _client->_upload_total += i.value().data.length();
              }
              _multipart_form_data.push( i.value() );
            }
            
            header.set("Content-Type", content_type_multipart_form);
          } else {
            
            for ( auto& i : _client->_post_form_data ) {
              _client->_upload_total += i.key().length() + i.value().data.length() + 2;
              _client->_post_data.write(i.key().c(), -1, i.key().length());
              _client->_post_data.write("=", -1, 1);
              _client->_post_data.write(i.value().data.c(), -1, i.value().data.length());
              _client->_post_data.write("&", -1, 1);
            }
            header.set("Content-Length", _client->_post_data.length());
            header.set("Content-Type", content_type_form);
          }
        }
      }
      
      StringBuilder header_str;
      bool search = _client->_uri.search().is_empty();

      header_str.push(String::format
      (
      "%s %s%s%s HTTP/1.1\r\n"
       , string_method[_client->_method].c()
       , secondary_uri_encode(_client->_uri.pathname()).c()
       , search ? "" : "?"
       , search ? "" : _client->_uri.search().c()
      ));
      
      for ( auto& i : header ) {
        header_str.push(i.key());
        header_str.push(string_colon); header_str.push(i.value()); header_str.push(string_end);
      }
      
      _client->trigger_http_readystate_change(HTTP_READY_STATE_SENDING);

      header_str.push(string_end); // \r\n
      
      _socket->resume();
      
      _socket->write(header_str.to_buffer()); // write header
      
    }
    
    virtual void trigger_socket_timeout(Socket* socket) {
      if ( _client ) {
        _client->trigger_http_timeout();
      }
    }
    
    virtual void trigger_stream_open(StreamProtocol* stream) {
      if ( _client ) {
        send_http_request();
      }
    }
    
    virtual void trigger_stream_close(StreamProtocol* stream) {
      if ( _client ) {
        Error err(ERR_CONNECT_UNEXPECTED_SHUTDOWN, "Connect unexpected shutdown");
        _client->report_error_and_abort(err);
      } else {
        pool()->release(this, true);
      }
    }
    
    virtual void trigger_stream_error(StreamProtocol* stream, cError& error) {
      if ( _client ) {
        _client->report_error_and_abort(error);
      }
    }
    
    virtual void trigger_stream_data(StreamProtocol* stream, Buffer& buffer) {
      if ( _client ) {
        http_parser_execute(&_parser, &_settings, buffer.value(), buffer.length());
      }
    }
    
    virtual void trigger_stream_write(StreamProtocol* stream, Buffer buffer, int mark) {
      if ( !_client ) return;
      if ( _send_data ) {
        if ( mark == 1 ) {
          _client->_upload_size += buffer.length();
          _client->trigger_http_write();
          
          if ( _is_multipart_form_data ) {
            _multipart_form_buffer = Buffer(buffer.collapse(), BUFFER_SIZE);
            send_multipart_form_data();
          }
        }
      }
      else if ( _client->_method == HTTP_METHOD_POST ) { // post data
        _send_data = true;
        if ( _client->_post_data.length() ) {
          _socket->write(_client->_post_data, -1, 1);
        } else if ( _is_multipart_form_data ) { // send multipart/form-data
          if ( !_multipart_form_buffer.length() ) {
            _multipart_form_buffer = Buffer(BUFFER_SIZE);
          }
          send_multipart_form_data();
        }
      }
    }
    
    virtual void trigger_async_file_open(AsyncFileProtocol* file) {
      send_multipart_form_data();
    }
    
    virtual void trigger_async_file_close(AsyncFileProtocol* file) {
      Error err(ERR_FILE_UNEXPECTED_SHUTDOWN, "File unexpected shutdown");
      _client->report_error_and_abort(err);
    }
    
    virtual void trigger_async_file_error(AsyncFileProtocol* file, cError& error) {
      _client->report_error_and_abort(error);
    }
    
    virtual void trigger_async_file_read(AsyncFileProtocol* file, Buffer buffer, int mark) {
      if ( buffer.length() ) {
        _socket->write(buffer, buffer.length(), 1);
      } else {
        av_assert(_multipart_form_data.length());
        av_assert(_upload_file);
        _socket->write(string_end.copy_buffer()); // \r\n
        _upload_file->release(); // release file
        _upload_file = nullptr;
        _multipart_form_data.shift();
        _multipart_form_buffer = Buffer(buffer.collapse(), BUFFER_SIZE);
        send_multipart_form_data();
      }
    }
    
    void send_multipart_form_data() {
      av_assert( _multipart_form_buffer.length() == BUFFER_SIZE );
      
      if ( _upload_file ) { // upload file
        av_assert( _upload_file->is_open() );
        _upload_file->read(_multipart_form_buffer);
      }
      else if ( _multipart_form_data.length() ) {
        FormValue& form = _multipart_form_data.begin().value();
        _socket->write(multipart_boundary_start.copy_buffer());
        
        if ( form.type == FORM_TYPE_FILE ) {
          _socket->write(String::format("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
                                        "Content-Type: application/octet-stream\r\n\r\n",
                                        *form.name, *Path::basename(form.data)).collapse_buffer());
          _upload_file = New<AsyncFile>(form.data, nullptr);
          _upload_file->set_delegate(this);
          _upload_file->open();
        } else {
          _socket->write(String::format("Content-Disposition: form-data;"
                                        "name=\"%s\"\r\n\r\n", *form.name).collapse_buffer());
          _multipart_form_buffer.write( form.data.c(), 0, form.data.length() );
          _socket->write(_multipart_form_buffer, form.data.length(), 1);
          _socket->write(string_end.copy_buffer());
        }
      } else {
        _socket->write(multipart_boundary_end.copy_buffer()); // end send data, wait http response
      }
    }
    
    bool  ssl() { return _ssl; }
    
    Socket* socket() { return _socket; }
    
    virtual void read_advance() {
      _socket->resume();
    }
    
    virtual void read_pause() {
      _socket->pause();
    }
    
    virtual bool is_cache() {
      return false;
    }
    
  private:
    friend class ConnectPool;
    
    bool        _ssl;
    Socket*     _socket;
    Client*     _client;
    bool        _use;
    ID          _id;
    AsyncFile*  _upload_file;
    http_parser _parser;
    http_parser_settings _settings;
    List<FormValue> _multipart_form_data;
    Buffer  _multipart_form_buffer;
    bool    _is_multipart_form_data;
    bool    _send_data;
    String  _header_field;
    Map<String, String> _header;
    z_stream _z_strm;
    int      _z_gzip;
  };
  
  class ConnectPool {
  public:
    
    struct connect_req {
      Client* client;
      Callback cb;
      uint wait_id;
      String  hostname;
      uint16  port;
      URIType uri_type;
    };
    
    void get_connect(Client* client, Callback cb) {
      av_assert(client);
      av_assert(!client->_uri.is_null());
      av_assert(!client->_uri.hostname().is_empty());
      av_assert(client->_uri.type() == URI_HTTP || client->_uri.type() == URI_HTTPS);
      
      uint16 port = client->_uri.port();
      if ( !port ) {
        port = client->_uri.type() == URI_HTTP ? 80 : 443;
      }
      
      client->_wait_connect_id = iid32();
      
      connect_req req = {
        client,
        cb,
        client->_wait_connect_id,
        client->_uri.hostname(),
        port,
        client->_uri.type(),
      };
      
      Connect* conn = get_connect2(req);
      
      if ( conn ) {
        conn->_use = true;
        SimpleEvent evt = { 0, conn };
        cb->call( evt );
      } else {
        _connect_req.push(req); // wait
      }
    }
    
    Connect* get_connect2(connect_req& req) {
      
      Connect* conn = nullptr;
      uint connect_count = 0;
      
      for ( auto& i : _pool ) {
        if ( connect_count < MAX_CONNECT_COUNT ) {
          Connect* connect = i.value();
          
          if (connect->socket()->hostname() == req.hostname &&
              connect->socket()->port() == req.port &&
              connect->ssl() == (req.uri_type == URI_HTTPS)
          ) {
            connect_count++;
            if ( !connect->_use ) {
              conn = connect; break;
            }
          }
        }
      }
      
      if (!conn && connect_count < MAX_CONNECT_COUNT) {
        conn = new Connect(req.hostname, req.port, req.uri_type == URI_HTTPS);
        conn->_id = _pool.push( conn );
      }
      
      return conn;
    }
    
    void release(Connect* connect, bool immediately) {
      if ( !connect ) return;
      
      if ( !connect->socket()->is_open() || immediately ) { // immediately release
        _pool.del(connect->_id);
        connect->_id = ConnectID();
        connect->release();
      } else {
        if ( connect->_use ) {
          av_assert( !connect->_id.is_null() );
          connect->_use = false;
          connect->_client = nullptr;
          connect->socket()->set_timeout(0);
          connect->socket()->resume();
        }
      }
      
      for ( auto& i : _connect_req ) {
        connect_req& req = i.value();
        if (req.client->_wait_connect_id == req.wait_id) {
          Connect* conn = get_connect2(req);
          if ( conn ) {
            conn->_use = true;
            SimpleEvent evt = { 0, conn };
            req.cb->call( evt );
            _connect_req.del(i);
            break;
          }
        } else {
          _connect_req.del(i); // discard req
        }
      }
    }
    
  private:
    List<Connect*>    _pool;
    List<connect_req> _connect_req;
  };
  
  class FileCacheReader: public AsyncFile, public AsyncFile::Delegate, public Reader {
  public:
    FileCacheReader(Client* client, int64 size)
    : AsyncFile(client->_cache_path, nullptr)
    , _client(client)
    , _offset(0)
    , _parse_header(true), _size(size), _read_count(0) {
      av_assert(!_client->_cache_reader);
      _client->_cache_reader = this;
      set_delegate(this);
      //
      open();
    }
    
    ~FileCacheReader() {
      _client->_cache_reader = nullptr;
    }
    
    void continue_send_and_release() {
      set_delegate(nullptr);
      _client->_cache_reader = nullptr;
      _client->send_http();
      release();
    }
    
    virtual void trigger_async_file_open(AsyncFileProtocol* file) {
      read(Buffer(512));
    }
    virtual void trigger_async_file_close(AsyncFileProtocol* file) {
      if ( _parse_header ) { // unexpected shutdown
        continue_send_and_release();
      } else {
        // throw error to http client host
        _client->report_error_and_abort(Error(ERR_FILE_UNEXPECTED_SHUTDOWN, "File unexpected shutdown"));
      }
    }
    virtual void trigger_async_file_error(AsyncFileProtocol* file, cError& error) {
      if ( _parse_header ) {
        continue_send_and_release();
      } else {
        // throw error to http client host
        _client->report_error_and_abort(error);
      }
    }
    virtual void trigger_async_file_read(AsyncFileProtocol* file, Buffer buffer, int mark) {
      if ( _parse_header ) { // parse cache header
        if ( buffer.length() ) {
          
          String str(buffer.value(), buffer.length()), s("\r\n"), s2(':');
          /*
           Expires: Thu, 27 Apr 2017 11:57:20 GMT
           Last-Modified: Fri, 18 Nov 2016 12:08:17 GMT
           
           ... Body ...
           */
          
          for ( int i = 0 ; ; ) {
            int j = str.index_of(s, i);
            if ( j != -1 && j != 0 ) {
              if ( j == i ) { // parse header end
                _parse_header = false;
                _offset += (j + 2);
                
                int64 expires = parse_time(_header.get("expires"));
                if ( expires > sys::time() ) {
                  _client->trigger_http_readystate_change(HTTP_READY_STATE_RESPONSE);
                  _client->_download_total = av_max(_size - _offset, 0);
                  _client->trigger_http_header(200, move(_header), true);
                  read_advance();
                } else {
                  int64 last_modified = parse_time(_header.get("last-modified"));
                  if ( last_modified > 0 || !_header.get("etag").is_empty() ) {
                    _client->send_http();
                  } else {
                    continue_send_and_release(); // full invalid
                  }
                }
                // parse header end
                break;
              } else {
                int k = str.index_of(s2, i);
                if ( k != -1 && k - i > 1 && j - k > 2 ) {
                  //av_debug("  %s: %s", str.substring(i, k).lower_case().c(), str.substring(k + 2, j).c());
                  _header.set(str.substring(i, k).lower_case(), str.substring(k + 2, j));
                }
              }
            } else {
              if ( i == 0 ) { // invalid cache
                continue_send_and_release();
              } else { // read next
                _offset += i;
                read(buffer, 512, _offset);
              }
              break;
            }
            i = j + 2;
          }
          
        } else {
          // no cache
          continue_send_and_release();
        }
      } else {
        // read cache
        
        _read_count--;
        av_assert(_read_count == 0);
        
        if ( buffer.length() ) {
          _offset += buffer.length();
          _client->_download_size += buffer.length();
          _client->trigger_http_data(buffer, true);
        } else { // end
          _client->trigger_http_end();
        }
      }
    }
    
    Map<String, String>& header() {
      return _header;
    }
    
    virtual void read_advance() {
      if ( !_parse_header ) {
        if ( _read_count == 0 ) {
          _read_count++;
          read(Buffer(BUFFER_SIZE), BUFFER_SIZE, _offset);
        }
      }
    }
    
    virtual void read_pause() {
      
    }
    
    virtual bool is_cache() {
      return true;
    }
    
  private:
    
    int     _read_count;
    Client* _client;
    Map<String, String> _header;
    bool _parse_header;
    uint  _offset;
    int64 _size;
  };
  
  class FileWriter: public Object, public AsyncFile::Delegate {
  public:
    FileWriter(Client* client, cString& path, bool is_cache_type)
    : _client(client)
    , _file(nullptr)
    , _is_cache_type(is_cache_type)
    , _write_count(0)
    , _completed_end(0) {
      av_assert(!_client->_file_writer);
      _client->_file_writer = this;
      
      if ( _is_cache_type ) { // verification cache is valid
        Map<String, String>& header = _client->_response_header;
        
        if ( header.has("cache-control") ) {
          String cache_control = header.get("cache-control");
          
          if ( !cache_control.is_empty() ) {
            int i = cache_control.index_of(string_max_age);
            if ( i != -1 && i + string_max_age.length() < cache_control.length() ) {
              int j = cache_control.index_of(',', i);
              String max_age = j != -1
              ? cache_control.substring(i + string_max_age.length(), j)
              : cache_control.substring(i + string_max_age.length());
              
              int64 num = max_age.trim().to_int64();
              if ( num > 0 ) {
                String gmt = gmt_time_string( sys::time_second() + num );
                header.set("expires", gmt);
              }
            }
          }
        }
        
        if ( header.has("expires") ) {
          int64 expires = parse_time(header.get("expires"));
          int64 now = sys::time();
          if ( expires > now ) {
            _file = new AsyncFile(path, nullptr);
          }
        } else if ( header.has("last-modified") || header.has("etag") ) {
          _file = new AsyncFile(path, nullptr);
        }
      } else { // download save
        _file = new AsyncFile(path, nullptr);
      }
      
      if ( _file ) {
        _file->set_delegate(this);
        _file->open(FOPEN_W);
      }
    }
    
    ~FileWriter() {
      Release(_file);
      _client->_file_writer = nullptr;
    }
    
    virtual void trigger_async_file_open(AsyncFileProtocol* file) {
      if ( _is_cache_type ) { // write header
        
        for ( auto& i : _client->_response_header ) {
          String item = i.key() + ": " + i.value() + string_end;
          _file->write( item.collapse_buffer(), -1, -1, 1 );
        }
        _file->write( string_end.copy_buffer(), -1, -1, 2 ); // end header write
        
      } else {
        _write_count++; _file->write(_buffer);
      }
    }
    
    virtual void trigger_async_file_close(AsyncFileProtocol* file) {
      // throw error to http client host
      _client->report_error_and_abort(Error(ERR_FILE_UNEXPECTED_SHUTDOWN, "File unexpected shutdown"));
    }
    
    virtual void trigger_async_file_error(AsyncFileProtocol* file, cError& error) {
      _client->report_error_and_abort(error);
    }
    
    virtual void trigger_async_file_write(AsyncFileProtocol* file, Buffer buffer, int mark) {
      if ( mark ) {
        if ( mark == 2 ) {
          _write_count++; _file->write(_buffer);
        }
      } else {
        _client->trigger_http_data2(buffer);
        _write_count--;
        av_assert(_write_count >= 0);
        if ( _write_count == 0 ) {
          if ( _completed_end ) {
            _client->trigger_http_end();
          } else {
            _client->read_advance();
          }
        }
      }
    }
    
    bool is_complete() const {
      return _write_count == 0 && _buffer.length() == 0;
    }
    
    void set_completed_end() {
      _completed_end = true;
    }
    
    void write(Buffer& buffer) {
      if ( _file ) {
        if ( _file->is_open() ) {
          _write_count++;
          if ( _write_count > 100 ) {
            _client->read_pause();
          }
          _file->write(buffer);
        } else {
          _buffer.write(buffer);
          _client->read_pause();
        }
      } else { // no file write task
        _client->trigger_http_data2(buffer);
        _client->read_advance();
      }
    }
    
  private:
    Client*     _client;
    Buffer      _buffer;
    AsyncFile*  _file;
    bool    _is_cache_type;
    int     _write_count;
    bool    _completed_end;
  };
  
  static ConnectPool* pool() {
    static ConnectPool* pool(new ConnectPool());
    return pool;
  }
  
  void trigger_http_readystate_change(HttpReadyState ready_state) {
    async_callback(Callback([this](SimpleEvent& evt) {
      HttpReadyState ready_state = HttpReadyState(static_cast<Int*>(evt.data)->value);
      if ( ready_state != _ready_state ) {
        _ready_state = ready_state;
        _delegate->trigger_http_readystate_change(_host);
      }
    }, this), Int(ready_state), loop());
  }
  
  void trigger_http_write() {
    async_callback(Callback([this](SimpleEvent& evt) {
      _delegate->trigger_http_write(_host);
    }, this), loop());
  }
  
  void trigger_http_header(uint status_code, Map<String, String>&& header, bool cache) {
    _status_code = status_code;
    //_response_header = move(header);
    async_callback(Callback([this](SimpleEvent& evt) {
      _delegate->trigger_http_header(_host);
    }, this));
  }
  
  void trigger_http_data2(Buffer& buffer) {
    async_callback(Callback([this](SimpleEvent& evt) {
      _delegate->trigger_http_data(_host, *static_cast<Buffer*>(evt.data));
    }, this), move(buffer), loop());
  }
  
  void read_advance() {
    Reader* _reader = reader(); av_assert(_reader);
    if ( _pause ) {
      _reader->read_pause();
    } else {
      _reader->read_advance();
    }
  }
  
  bool is_disable_cache() {
    return _disable_cache || _url_no_cache_arg;
  }
  
  void read_pause() {
    Reader* _reader = reader(); av_assert(_reader);
    _reader->read_pause();
  }
  
  void trigger_http_data(Buffer& buffer, bool cache) {
    
    if ( !cache ) {
      if ( _cache_reader ) {
        _cache_reader->release();
        _cache_reader = nullptr;
      }
    }
    if ( !_save_path.is_empty() ) {
      if ( !_file_writer ) {
        new FileWriter(this, _save_path, false);
      }
      _file_writer->write(buffer);
    } else if ( !is_disable_cache() && !cache ) {
      if ( !_file_writer ) {
        new FileWriter(this, _cache_path, true);
      }
      _file_writer->write(buffer);
    } else {
      trigger_http_data2(buffer);
      read_advance();
    }
  }
  
  void http_response_complete() {
    av_assert(_connect);
    pool()->release(_connect, false); _connect = nullptr;
    
    if ( _status_code == 304 ) {
      _cache_reader->read_advance(); return;
    }
    
    if ( _file_writer ) {
      if ( _file_writer->is_complete() ) { // 写入完成
        trigger_http_end();
      } else {
        _file_writer->set_completed_end(); // 写入完成后可以结束
      }
    } else {
      trigger_http_end();
    }
  }
  
  void trigger_http_end_from_loop(SimpleEvent& evt) {
    av_assert( _sending );
    _ready_state = HTTP_READY_STATE_COMPLETED;
    _delegate->trigger_http_readystate_change(_host);
    _delegate->trigger_http_end(_host);
    _ready_state = HTTP_READY_STATE_INITIAL;
    Handle<Sending> handle(_sending); _sending = nullptr;
  }
  
  void trigger_http_end() {
    
    async_callback(Callback(&Inl::trigger_http_end_from_loop, this), loop());
    
    // release all handle
    Release(_cache_reader); _cache_reader = nullptr;
    Release(_file_writer); _file_writer = nullptr;
    pool()->release(_connect, false); _connect = nullptr;
    _pause = false;
    _wait_connect_id = 0;
  }
  
  void report_error_and_abort(cError& error) {
    async_err_callback(Callback([this](SimpleEvent& evt){
      _delegate->trigger_http_error(_host, *evt.error);
    }, this), Error(error), loop());
    abort2();
  }
  
  void trigger_http_timeout() {
    async_callback(Callback([this](SimpleEvent& evt) {
      _delegate->trigger_http_timeout(_host);
    }, this), loop());
    abort2();
  }
  
  Reader* reader() {
    if ( _connect ) return _connect; else return _cache_reader;
  }
  
  void send_http() {
    av_assert(!_connect);
    pool()->get_connect(this, Callback([this](SimpleEvent& evt) {
      if ( _wait_connect_id ) {
        if ( evt.error ) {
          report_error_and_abort(*evt.error);
        } else {
          av_assert( !_connect );
          _connect = static_cast<Connect*>(evt.data);
          _connect->bind_client_and_send(this);
        }
      }
    }, this));
  }
  
  void cache_file_stat_cb(SimpleEvent& evt) {
    if ( evt.error ) {
      send_http();
    } else {
      new FileCacheReader(this, static_cast<FileStat*>(evt.data)->size());
    }
  }
  
  void send(Buffer data) av_def_err {
    av_assert_err(!_sending, ERR_REPEAT_CALL, "Sending repeat call");
    av_assert_err( !_uri.is_null(), ERR_INVALID_PATH, "Invalid path" );
    av_assert_err(_uri.type() == URI_HTTP ||
                  _uri.type() == URI_HTTPS, ERR_INVALID_PATH, "Invalid path `%s`", *_uri.href());
    _sending = new Sending(this);
    _post_data = data;
    _cache_path = http_cache_path + '/' + hash(_uri.href());
    _pause = false;
    
    _url_no_cache_arg = false;
    int i = _uri.search().index_of("_no_cache");
    if ( i == 0 || (i > 0 && _uri.search()[i-1] == '&') ) {
      _url_no_cache_arg = true;
    }
    
    post_io(Callback([this](SimpleEvent& evt) {
      trigger_http_readystate_change(HTTP_READY_STATE_READY); // ready
      _upload_total = 0; _upload_size = 0;
      _download_total = 0; _download_size = 0;
      _status_code = 0;
      _response_header.clear();
      if ( is_disable_cache() ) { // check cache
        send_http();
      } else {
        FileHelper::stat(_cache_path, Callback(&Inl::cache_file_stat_cb, this));
      }
    }, this));
  }
  
  void abort2() {
    if ( _sending ) {
      pool()->release(_connect, true);  _connect = nullptr;
      Release(_cache_reader);           _cache_reader = nullptr;
      Release(_file_writer);            _file_writer = nullptr;
      _pause = false;
      _wait_connect_id = 0;
      
      async_callback(Callback([this](SimpleEvent& evt) {
        if ( _sending ) {
          Handle<Sending> handle(_sending); _sending = nullptr;
          _delegate->trigger_http_abort(_host);
          _ready_state = HTTP_READY_STATE_INITIAL;
        }
      }, this), loop());
    }
  }
  
  void abort() {
    if ( _sending ) {
      post_io(Callback([this](SimpleEvent& evt) { abort2(); }, this));
    }
  }
  
  void check_is_can_modify() av_def_err {
    av_assert_err(!_sending, ERR_SENDING_CANNOT_MODIFY,
                  "Http request sending cannot modify property");
  }
  
  void pause() {
    _pause = true;
  }
  
  void resume() {
    if ( _pause ) {
      _pause = true;
      post_io(Callback([this](SimpleEvent& evt) {
        if ( !_pause ) {
          Reader* _reader = reader(); av_assert(_reader);
          if ( _reader ) {
            _reader->read_advance();
          }
        }
      }, this));
    }
  }
  
  // -----------------------------------attrs------------------------------------------
  
  HttpClientRequest* _host;
  LoopProxy*  _loop;
  HttpDelegate* _delegate;
  int64      _upload_total;    /* 需上传到服务器数据总量 */
  int64      _upload_size;     /* 已写上传到服务器数据尺寸 */
  int64      _download_total;  /* 需下载数据总量 */
  int64      _download_size;   /* 已下载数据量 */
  HttpReadyState _ready_state; /* 请求状态 */
  uint        _status_code;    /* 服务器响应http状态码 */
  HttpMethod  _method;
  URI         _uri;
  String      _save_path;
  Connect*    _connect;
  FileCacheReader* _cache_reader;
  FileWriter* _file_writer;
  Map<String, String> _request_header;
  Map<String, String> _response_header;
  Map<String, FormValue> _post_form_data;
  Buffer      _post_data;
  String      _username;
  String      _password;
  String      _cache_path;
  bool        _disable_cache;
  bool        _disable_cookie;
  bool        _disable_send_cookie;
  bool        _disable_ssl_verify;
  bool        _disable_ssl_verify_host; //
  bool        _keep_alive;
  Sending*    _sending;
  uint64      _timeout;
  bool        _pause;
  bool        _url_no_cache_arg;
  uint        _wait_connect_id;
};

HttpClientRequest::HttpClientRequest(RunLoop* loop): m_inl(NewRetain<Inl>(this, loop))
{
}

HttpClientRequest::~HttpClientRequest() {
  m_inl->set_delegate(nullptr);
  m_inl->abort();
  m_inl->release();
}

void HttpClientRequest::set_delegate(HttpDelegate* delegate) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->set_delegate(delegate);
}

void HttpClientRequest::set_method(HttpMethod method) av_def_err {
  m_inl->check_is_can_modify();
  if ( method < HTTP_METHOD_GET || method > HTTP_METHOD_PUT ) {
    method = HTTP_METHOD_GET;
  }
  m_inl->_method = method;
}

void HttpClientRequest::set_url(cString& path) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_uri = URI(path);
}

void HttpClientRequest::set_save_path(cString& path) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_save_path = path;
}

void HttpClientRequest::set_username(cString& username) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_username = username;
}

void HttpClientRequest::set_password(cString& password) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_password = password;
}

void HttpClientRequest::disable_cache(bool disable) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_disable_cache = disable;
}

void HttpClientRequest::disable_cookie(bool disable) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_disable_cookie = disable;
}

void HttpClientRequest::disable_send_cookie(bool disable) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_disable_send_cookie = disable;
}

void HttpClientRequest::disable_ssl_verify(bool disable) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_disable_ssl_verify = disable;
}

void HttpClientRequest::set_keep_alive(bool keep_alive) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_keep_alive = keep_alive;
}

void HttpClientRequest::set_timeout(uint64 timeout) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_timeout = timeout;
}

void HttpClientRequest::set_request_header(cString& name, cString& value) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_request_header.set(name, value);
}

void HttpClientRequest::set_form(cString& form_name, cString& value) av_def_err {
  m_inl->check_is_can_modify();
  av_assert_err( value.length() <= BUFFER_SIZE,
                ERR_HTTP_FORM_SIZE_LIMIT, "Http form field size limit <= %d", BUFFER_SIZE);
  m_inl->_post_form_data.set(form_name, { FORM_TYPE_TEXT, value, form_name });
}

void HttpClientRequest::set_upload_file(cString& form_name, cString& path) av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_post_form_data.set(form_name, { FORM_TYPE_FILE, path, form_name });
}

void HttpClientRequest::clear_request_header() av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_request_header.clear();
}

void HttpClientRequest::clear_form_data() av_def_err {
  m_inl->check_is_can_modify();
  m_inl->_post_form_data.clear();
}

String HttpClientRequest::get_response_header(cString& name) {
  return m_inl->_response_header[name];
}

const Map<String, String>& HttpClientRequest::get_all_response_headers() {
  return m_inl->_response_header;
}

int64 HttpClientRequest::upload_total() const {
  return m_inl->_upload_total;
}

int64 HttpClientRequest::upload_size() const {
  return m_inl->_upload_size;
}

int64 HttpClientRequest::download_total() const {
  return m_inl->_download_total;
}

int64 HttpClientRequest::download_size() const {
  return m_inl->_download_size;
}

HttpReadyState HttpClientRequest::ready_state() const {
  return m_inl->_ready_state;
}

uint HttpClientRequest::status_code() const {
  return m_inl->_status_code;
}

String HttpClientRequest::url() const {
  return m_inl->_uri.href();
}

void HttpClientRequest::send(Buffer data) av_def_err { // thread safe
  m_inl->send(data);
}

void HttpClientRequest::send(cString& data) av_def_err { // thread safe
  m_inl->send(data.copy_buffer());
}

void HttpClientRequest::pause() { // thread safe
  m_inl->pause();
}

void HttpClientRequest::resume() { // thread safe
  m_inl->resume();
}

void HttpClientRequest::abort() { // thread safe
  m_inl->abort();
}

// ----------------------------- HttpHelper -----------------------------

typedef HttpHelper::RequestOptions RequestOptions;

static uint http_request(RequestOptions& options, Callback cb, bool stream) throw(HttpError) {
  
  class Task: public AsyncIOTask, public HttpClientRequest::Delegate, public SimpleStream {
  public:
    
    Task() {
      client = new HttpClientRequest();
      client->set_delegate(this);
      full_data = 1;
    }
    
    ~Task() {
      Release(client);
      // av_debug("END http request");
    }
    
    virtual void trigger_http_error(HttpClientRequest* req, cError& error) {
      HttpError e(error.code(),
                  error.message() + ", " + req->url().c(), req->status_code(), req->url());
      sync_callback(cb, &e, nullptr);
      abort_and_release(); // abort and release
    }
    
    virtual void trigger_http_data(HttpClientRequest* req, Buffer buffer) {
      if ( stream ) {
        IOStreamData data(buffer, 0, id(),
                          client->download_size(),
                          client->download_total(), this);
        sync_callback(cb, nullptr, &data);
      } else {
        if ( full_data ) {
          data.push(buffer.collapse_string());
        }
      }
    }
    
    virtual void trigger_http_end(HttpClientRequest* req) {
      /*
       100-199 用于指定客户端应相应的某些动作。
       200-299 用于表示请求成功。
       300-399 用于已经移动的文件并且常被包含在定位头信息中指定新的地址信息。
       400-499 用于指出客户端的错误。
       500-599 用于支持服务器错误。
       */
      
      if ( client->status_code() > 399 || client->status_code() < 100 ) {
        HttpError e(ERR_HTTP_STATUS_ERROR,
                    String::format("Http status error, status code:%d, %s",
                                   req->status_code(), req->url().c()),
                    req->status_code(), req->url());
        sync_callback(cb, &e);
      } else {
        if ( stream ) {
          IOStreamData data(Buffer(), 1, id(),
                            client->download_size(),
                            client->download_total(), client);
          sync_callback(cb, nullptr, &data);
        } else {
          Buffer buff = data.to_buffer();
          sync_callback(cb, nullptr, &buff);
        }
      }
      abort_and_release(); // abort and release
    }
    
    virtual void trigger_http_abort(HttpClientRequest* req) {
#if DEBUG
      printf("request async abort\n");
#endif
    }
    
    virtual void abort_and_release() {
      Release(client); client = nullptr;
      AsyncIOTask::abort_and_release();
    }
    
    virtual void pause() {
      if ( client ) client->pause();
    }
    
    virtual void resume() {
      if ( client ) client->resume();
    }
    
    Callback      cb;
    bool          stream;
    bool          full_data;
    StringBuilder data;
    HttpClientRequest* client;
  };
  
  Handle<Task> task(new Task());
  
  HttpClientRequest* req = task->client;
  
  try {
    req->set_url(options.url);
    req->set_method(options.method);
    req->disable_cache(options.disable_cache);
    req->disable_ssl_verify(options.disable_ssl_verify);
    req->disable_cookie(options.disable_cookie);
    task->cb = cb;
    task->stream = stream;
    
    if ( ! options.upload.is_empty() ) { // 需要上传文件
      req->set_upload_file("file", options.upload);
    }
    
    if ( !options.save.is_empty() ) {
      task->full_data = 0;
      req->set_save_path(options.save);
    }
    
    for ( auto& i : options.headers ) {
      req->set_request_header(i.key(), i.value());
    }
    
    req->send(options.post_data);
  } catch (cError& e) {
    throw HttpError(e);
  }
  
  return task.collapse()->id();
}

/**
 * @func request
 */
uint HttpHelper::request(RequestOptions& options, Callback cb) throw(HttpError) {
  return http_request(options, cb, false);
}

uint HttpHelper::request_stream(RequestOptions& options, Callback cb) throw(HttpError) {
  return http_request(options, cb, true);
}

/**
 * @func request_sync
 */
Buffer HttpHelper::request_sync(RequestOptions& options) throw(HttpError) {
  
  if ( Thread::is_io_thread() ) {
    throw HttpError(ERR_CANNOT_RUN_SYNC_IO,
                    String::format("cannot send sync http request, %s"
                                   , options.url.c()), 0, options.url);
  }
  
  class MyClient: public HttpClientRequest, public HttpClientRequest::Delegate {
  public:
    
    MyClient() : HttpClientRequest(nullptr) {
      full_data = 1; is_error = 0;
      set_delegate(this);
    }
    
    virtual void trigger_http_error(HttpClientRequest* req, cError& err) {
      ScopeLock scope(mutex);
      error = err;
      is_error = 1;
      cond.notify_one();
    }
    virtual void trigger_http_data(HttpClientRequest* req, Buffer buffer) {
      if ( full_data ) {
        data.push(buffer.collapse_string());
      }
    }
    virtual void trigger_http_end(HttpClientRequest* req) {
      ScopeLock scope(mutex);
      cond.notify_one();
    }
    virtual void trigger_http_abort(HttpClientRequest* req) {
      av_debug("request_sync abort");
    }
    
    void wait() {
      Lock ul(mutex);
      cond.wait(ul);
    }
    
    bool          full_data;
    bool          is_error;
    Error         error;
    StringBuilder data;
    Condition     cond;
    Mutex         mutex;
  };
  
  Handle<MyClient> req = new MyClient();
  
  try {
    req->set_url(options.url);
    req->set_method(options.method);
    req->disable_cache(options.disable_cache);
    req->disable_ssl_verify(options.disable_ssl_verify);
    
    if ( ! options.upload.is_empty() ) { // 需要上传文件
      req->set_upload_file("file", options.upload);
    }
    
    if ( !options.save.is_empty() ) {
      req->full_data = 0;
      req->set_save_path(options.save);
    }
    
    for ( auto& i : options.headers ) {
      req->set_request_header(i.key(), i.value());
    }
    
    req->send(options.post_data);
    
  } catch(cError& e) {
    throw HttpError(e);
  }
  
  req->wait();
  
  if ( req->is_error ) {
    HttpError e(req->error.code(),
                req->error.message() + ", " + req->url().c(),
                req->status_code(), req->url());
    throw e;
  } else {
    if ( req->status_code() > 399 || req->status_code() < 100 ) {
      String msg = String::format("Http status error, status code:%d, %s",
                                  req->status_code(), req->url().c());
      HttpError e(ERR_HTTP_STATUS_ERROR, msg, req->status_code(), req->url());
      throw e;
    }
  }
  
  return req->data.to_buffer();
}

static RequestOptions default_request_options(cString& url) {
  return {
    url,
    HTTP_METHOD_GET,
    Map<String, String>(),
    Buffer(),
    String::empty,
    String::empty,
    false,
    false,
    false,
  };
}

/**
 * @func download
 */
uint HttpHelper::download(cString& url, cString& save, Callback cb) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  options.save = save;
  return http_request(options, cb, false);
}

/**
 * @func download_sync
 */
void HttpHelper::download_sync(cString& url, cString& save) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  options.save = save;
  request_sync(options);
}

/**
 * @func upload
 */
uint HttpHelper::upload(cString& url, cString& file, Callback cb) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  options.upload = file;
  options.disable_cache = true;
  return http_request(options, cb, false);
}

/**
 * @func upload
 */
Buffer HttpHelper::upload_sync(cString& url, cString& file) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  options.save = file;
  options.disable_cache = true;
  return request_sync(options);
}

/**
 * @func get
 */
uint HttpHelper::get(cString& url, Callback cb) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  return http_request(options, cb, false);
}

/**
 * @func get_stream
 */
uint HttpHelper::get_stream(cString& url, Callback cb) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  return http_request(options, cb, true);
}

/**
 * @func post
 */
uint HttpHelper::post(cString& url, Buffer data, Callback cb) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  options.method = HTTP_METHOD_POST;
  options.post_data = data;
  return http_request(options, cb, false);
}

/**
 * @func get_sync
 */
Buffer HttpHelper::get_sync(cString& url) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  return request_sync(options);
}

/**
 * @func post_sync
 */
Buffer HttpHelper::post_sync(cString& url, Buffer data) throw(HttpError) {
  RequestOptions options = default_request_options(url);
  options.method = HTTP_METHOD_POST;
  options.post_data = data;
  return request_sync(options);
}

/**
 * @func abort
 */
void HttpHelper::abort(uint id) {
  AsyncIOTask::safe_abort(id);
}

/**
 * @func initialize
 */
void HttpHelper::initializ() {
  if ( ! http_initialized++ ) {
    http_user_agent = String::format("Mozilla/5.0 (%s/%s) Avocado/"
                                     AVOCADO_VERSION " (KHTML, like Gecko)", *sys::name(), *sys::version());
    set_cache_path(Path::temp("cache"));
  }
}

/**
 * @func user_agent
 */
String HttpHelper::user_agent() {
  return http_user_agent;
}

/**
 * @func set_user_agent
 */
void HttpHelper::set_user_agent(cString& user_agent) {
  http_user_agent = user_agent;
}

/**
 * @func cache_path
 */
String HttpHelper::cache_path() {
  return http_cache_path;
}

/**
 * @func set_cache_path 设置缓存文件路径
 */
void HttpHelper::set_cache_path(cString& path) {
  if ( FileHelper::mkdir_p_sync(path) ) {
    http_cache_path = path;
  }
}

/**
 * @func clean_cache 清理web缓存
 */
void HttpHelper::clear_cache() {
  // delete cache files
  if ( ! http_cache_path.is_empty() ) {
    FileHelper::rm_r_sync(http_cache_path);
    set_cache_path(http_cache_path);
  }
}

void HttpHelper::clear_cookie() {
  http_cookie_clear();
}

av_end
