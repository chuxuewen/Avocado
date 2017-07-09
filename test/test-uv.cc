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

#include "autil/util.h"
#include "autil/fs.h"
#include <uv.h>

using namespace avocado;

namespace avocado {
  uv_loop_t* _uv_loop();
}

static uv_fs_t open_req;
static uv_fs_t read_req;
static uv_buf_t buffer;
static uv_idle_t idler;
static uv_async_t uv_async;
static uv_check_t uv_check_req;

void on_read(uv_fs_t *req);
void on_open(uv_fs_t *req);

void on_read(uv_fs_t *req) {
  if (req->result < 0) {
    av_err("Read error: %s, %s\n", uv_err_name(req->result), uv_strerror(req->result));
  }
  else if (req->result == 0) {
    uv_fs_t close_req;
    // synchronous
    uv_fs_close(_uv_loop(), &close_req, open_req.result, nullptr);
  }
  else {
    LOG(String(buffer.base, req->result));
    uv_fs_read(_uv_loop(), &read_req, open_req.result, &buffer, 1, -1, on_read);
  }
  uv_fs_req_cleanup(req);
}

void on_open(uv_fs_t *req) {
  if (req->result > 0) {
    uv_fs_read(_uv_loop(), &read_req, open_req.result, &buffer, 1, -1, on_read);
  }
  else {
    av_err("error opening file: %s, %s\n", uv_err_name(req->result), uv_strerror(req->result));
  }
  uv_fs_req_cleanup(req);
}

void uv_idle_cb_(uv_idle_t* handle) {
  LOG("idle");
  uv_idle_stop(handle);
}

void test_uv_idle() {
  uv_idle_init(_uv_loop(), &idler);
  uv_idle_start(&idler, &uv_idle_cb_);
}

void uv_async_cb_(uv_async_t* handle) {
  static int i = 10;
  LOG("ASYNC");
  
  if ( i ) {
    uv_async_send(&uv_async);
  } else {
    uv_close((uv_handle_t*)&uv_async, nullptr);
  }
  i--;
}

void test_uv_async() {
  uv_async_init(_uv_loop(), &uv_async, uv_async_cb_);
  uv_async_send(&uv_async);
}

void test_uv_check_cb(uv_check_t* handle) {
  LOG("CHECK");
  uv_check_stop(handle);
}

void test_uv_check() {
  uv_check_init(_uv_loop(), &uv_check_req);
  uv_check_start(&uv_check_req, test_uv_check_cb);
}

void test_uv() {
  buffer.base = (char*)malloc(1024);
  buffer.len = 1024;
  LOG(FileHelper::read_file_sync(Path::resources("res/bg.svg")));
  uv_fs_open(_uv_loop(), &open_req, Path::restore_c(Path::resources("res/bg.svg")), O_RDONLY, 0, on_open);
  
  test_uv_idle();
  test_uv_async();
  test_uv_check();
  
  RunLoop::post_io(Callback([](SimpleEvent& evt) {
    LOG("post_io_loop");
  }));
  
}
