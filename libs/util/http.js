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

export binding('_http');

import 'util';
import NativeResponder from 'event';

 /**
  * @enum HttpMethod
  * HTTP_METHOD_GET = 0
  * HTTP_METHOD_POST = 1
  * HTTP_METHOD_HEAD = 2
  * HTTP_METHOD_DELETE = 3
  * HTTP_METHOD_PUT = 4
  * @end
  */

 /**
  * @enum  HttpReadyState
  * HTTP_READY_STATE_INITIAL = 0
  * HTTP_READY_STATE_READY = 1
  * HTTP_READY_STATE_SENDING = 2
  * HTTP_READY_STATE_RESPONSE = 3
  * HTTP_READY_STATE_COMPLETED = 4
  * @end
  */
 
 /**
  * @class NativeHttpClientRequest
  *
  * @func set_method(method)
  * @arg method {HttpMethod}
  *
  * @func set_url(url)
  * @arg url {String}
  *
  * @func set_save_path(path)
  * @arg path {String}
  *
  * @func set_username(username)
  * @arg username {String}
  *
  * @func set_password(password)
  * @arg password {String}
  *
  * @func disable_cache(disable)
  * @arg disable {bool}
  *
  * @func disable_cookie(disable)
  * @arg disable {bool}
  *
  * @func disable_send_cookie(disable)
  * @arg disable {bool}
  *
  * @func disable_ssl_verify(disable)
  * @arg disable {bool}
  *
  * @func set_request_header(header_name, value)
  * @arg header_name {String} ascii string
  * @arg value {String}
  *
  * @func set_form(form_name, value)
  * @arg form_name {String}
  * @arg value {String}
  *
  * @func set_upload_file(form_name, local_path)
  * @arg form_name {String}
  * @arg local_path {String}
  *
  * @func clear_request_header()
  *
  * @func clear_form_data()
  *
  * @func get_response_header(header_name)
  * @arg header_name {String}
  * @ret {String}
  *
  * @func get_all_response_headers()
  * @ret {Object}
  *
  * @func set_keep_alive(keep_alive)
  * @arg keep_alive {bool}
  *
  * @func set_timeout(time)
  * @arg time {uint} ms
  *
  * @func send([data])
  * @arg [data] {String|ArrayBuffer|Buffer}
  *
  * @func pause()
  *
  * @func resume()
  *
  * @func abort()
  *
  * @get upload_total {uint}
  * @get upload_size {uint}
  * @get download_total {uint}
  * @get download_size {uint}
  * @get download_size {HttpReadyState}
  * @get status_code {int}
  * @get url {String}
  * @end
  */

/**
 * @class HttpClientRequest
 * @bases NativeHttpClientRequest
 */
export class HttpClientRequest extends exports.NativeHttpClientRequest {
  event onerror;
  event onwrite;
  event onheader;
  event ondata;
  event onend;
  event onreadystate_change;
  event ontimeout;
  event onabort;
}

util.ext_class(HttpClientRequest, NativeResponder);

 /**
  * @object RequestOptions
  * url                 {String}
  * method              {HttpMethod}
  * headers             {Object}    setting custom request headers
  * post_data           {Buffer}    Non post requests ignore this option
  * save                {String}    save body content to local disk
  * upload              {String}    upload loacl file  
  * disable_ssl_verify  {bool}
  * disable_cache       {bool}
  * disable_cookie      {bool}
  * @end
  */

 /**
  * @func request(options[,cb])
  * @arg options {RequestOptions}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func request_stream(options[,cb])
  * @arg options {RequestOptions}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func request_sync(url)
  * @arg url {String}
  * @ret {Buffer}
  *
  * @func download(url,save[,cb])
  * @arg url {String}
  * @arg save {String}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func download_sync(url,save)
  * @arg url {String}
  * @arg save {String}
  *
  * @func upload(url,local_path[,cb])
  * @arg url {String}
  * @arg local_path {String}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func upload_sync(url,local_path)
  * @arg url {String}
  * @arg local_path {String}
  * @ret {Buffer}
  *
  * @func get(url[,cb])
  * @arg url {String}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func get_stream(url[,cb])
  * @arg url {String}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func post(url,data[,cb])
  * @arg url {String}
  * @arg data {String|ArrayBuffer|Buffer}
  * @arg [cb] {Function}
  * @ret {uint} return req id
  *
  * @func get_sync(url)
  * @arg url {String}
  * @ret {Buffer}
  *
  * @func post_sync(url,data)
  * @arg url {String}
  * @arg data {String|ArrayBuffer|Buffer}
  * @ret {Buffer}
  *
  * @func abort(id)
  * @arg id {uint} abort id
  *
  * @func user_agent()
  * @ret {String}
  *
  * @func set_user_agent(user_agent)
  * @arg user_agent {String}
  *
  * @func ssl_cacert_file()
  * @ret {String} return cacert file path
  *
  * @func ssl_cacert_file(path)
  * @arg path {String}
  *
  * @func set_ssl_client_key_file(path)
  * @arg path {String}
  *
  * @func set_ssl_client_keypasswd(password)
  * @arg password {String}
  *
  * @func cache_path()
  * @ret {String}
  *
  * @func set_cache_path(path)
  * @arg path {String}
  *
  * @func clear_cache()
  *
  * @func clear_cookie()
  *
  */
