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

#include "cb.h"
#include "string.h"
#include <uv.h>
#include "thread.h"

av_ns(avocado)

class StaticCallback2: public CallbackCore {
public:
  virtual bool retain() { return 0; }
  virtual void release() { }
  virtual void call(SimpleEvent& event) const { }
};

static StaticCallback2* default_callback_p = nullptr;
static Mutex mutex;

static inline StaticCallback2* default_callback() {
  if ( !default_callback_p ) {
    ScopeLock scope(mutex);
    default_callback_p = NewRetain<StaticCallback2>();
  }
  return default_callback_p;
}

Callback::Callback(): Handle(default_callback()) {
  //
}

class WrapCallback: public CallbackCore {
public:
  inline WrapCallback(Callback cb, Error* err, Object* data)
  : _inl_cb(cb), _err(err), _data(data) { }
  virtual ~WrapCallback() {
    Release(_err);
    Release(_data);
  }
  virtual void call(SimpleEvent& evt) const {
    evt.error = _err;
    evt.data = _data;
    _inl_cb->call(evt);
  }
private:
  Callback _inl_cb;
  Error* _err;
  Object* _data;
};

void async_callback_and_dealloc(Callback cb, Error* e, Object* d, PostMessage* loop) {
  loop->post_message( Callback(new WrapCallback(cb, e, d)) );
}

/**
 * @func sync_callback
 */
int sync_callback(Callback cb, cError* err, Object* data) {
  SimpleEvent evt = { err, data, 0 };
  cb->call(evt);
  return evt.return_value;
}

/**
 * @func async_callback
 */
void async_callback(Callback cb, PostMessage* loop) {
  if ( loop ) {
    loop->post_message( cb );
  } else {
    sync_callback(cb);
  }
}

static Map<uint, AsyncIOTask*> m_tasks;

AsyncIOTask::AsyncIOTask(): m_id(iid32()), m_abort(false) {
  uint id = m_id;
  RunLoop::post_io(Callback([id, this](SimpleEvent& evt) {
    m_tasks.set(id, this);
  }), true);
}

AsyncIOTask::~AsyncIOTask() {
  uint id = m_id;
  RunLoop::post_io(Callback([id](SimpleEvent& evt) {
    m_tasks.del(id);
  }), true);
}

void AsyncIOTask::abort_and_release() {
  if ( !m_abort ) {
    m_abort = true;
    release(); // end
  }
}

void AsyncIOTask::safe_abort(uint id) {
  RunLoop::post_io(Callback([id](SimpleEvent& evt) {
    auto it = m_tasks.find(id);
    if ( !it.is_null() ) {
      it.value()->abort_and_release();
      m_tasks.del(it);
    }
  }), true);
}

av_end
