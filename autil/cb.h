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

#ifndef __avocado__cb__
#define __avocado__cb__

#include "util.h"
#include "error.h"
#include "buffer.h"
#include "handle.h"
#include <functional>

/**
 * @ns avocado
 */

av_ns(avocado)

class PostMessage;

struct SimpleEvent {
  cError* error;
  Object* data;
  int return_value;
};

class SimpleStream {
public:
  virtual void pause() = 0;
  virtual void resume() = 0;
};

/**
 * @class IOStreamData
 */
class IOStreamData: public Object {
public:
  inline IOStreamData(Buffer buffer
                      , bool complete = 0
                      , uint id = 0
                      , uint64 size = 0
                      , uint64 total = 0, SimpleStream* stream = nullptr)
  : _buffer(buffer)
  , _complete(complete)
  , _size(size), _total(total), _id(id), _stream(stream) {
  }
  inline bool complete() const { return _complete; }
  inline int64 size() const { return _size; }
  inline int64 total() const { return _total; }
  inline Buffer& buffer() { return _buffer; }
  inline cBuffer& buffer() const { return _buffer; }
  inline uint id() const { return _id; }
  inline SimpleStream* stream() const { return _stream; }
  inline void pause() { if ( _stream ) _stream->pause(); }
  inline void resume() { if ( _stream ) _stream->resume(); }
  
private:
  Buffer    _buffer;
  bool      _complete;
  int64     _size;
  int64     _total;
  uint      _id;
  SimpleStream* _stream;
};

class CallbackCore: public Reference {
  av_hidden_all_copy(CallbackCore);
public:
  inline CallbackCore() { }
  virtual void call(SimpleEvent& event) const = 0;
  inline  void call() const { SimpleEvent evt = { 0,0,0 }; call(evt); }
};

template<class T> class CallbackCore2: public CallbackCore {
public:
  inline CallbackCore2(T* ctx): m_ctx(ctx) {
    if ( T::Characteristic::is_reference ) {
      T::Characteristic::Retain(m_ctx);
    }
  }
  virtual ~CallbackCore2() {
    if ( T::Characteristic::is_reference ) {
      T::Characteristic::Release(m_ctx);
    }
  }
protected:
  T* m_ctx;
};

template<class T = Object> class LambdaCallback: public CallbackCore2<T> {
public:
  typedef std::function<void(SimpleEvent& evt)> Func;
  inline LambdaCallback(Func func, T* ctx = nullptr): CallbackCore2<T>(ctx), m_func(func) { }
  virtual void call(SimpleEvent& evt) const { m_func(evt); }
private:
  Func m_func;
};

template<class T> class StaticCallback: public CallbackCore2<T> {
public:
  typedef void (*Func)(SimpleEvent& evt, T* ctx);
  inline StaticCallback(Func func, T* ctx = nullptr): CallbackCore2<T>(ctx), m_func(func) { }
  virtual void call(SimpleEvent& evt) const { m_func(evt, this->m_ctx); }
private:
  Func  m_func;
};

template<class T> class MemberCallback: public CallbackCore2<T> {
public:
  typedef void (T::*Func)(SimpleEvent& evt);
  inline MemberCallback(Func func, T* ctx): CallbackCore2<T>(ctx), m_func(func) { }
  virtual void call(SimpleEvent& evt) const { (this->m_ctx->*m_func)(evt); }
private:
  Func  m_func;
};

class Callback: public Handle<CallbackCore> {
public:
  Callback();
  inline Callback(Type* cb): Handle(cb) { }
  inline Callback(const Callback& handle): Handle(*const_cast<Callback*>(&handle)) { }
  inline Callback(Callback& handle): Handle(handle) { }
  inline Callback(Callback&& handle): Handle(handle) { }
  template<class T = Object>
  inline Callback(typename LambdaCallback<T>::Func func, T* ctx = nullptr): Handle(new LambdaCallback<T>(func, ctx)) { }
  template<class T>
  inline Callback(void (*func)(SimpleEvent& evt, T* ctx), T* ctx = nullptr): Handle(new StaticCallback<T>(func, ctx)) { }
  template<class T = Object>
  inline Callback(typename MemberCallback<T>::Func func, T* ctx): Handle(new MemberCallback<T>(func, ctx)) { }
  inline Callback& operator=(Callback& handle) { Handle::operator=(handle); return *this; }
  inline Callback& operator=(Callback&& handle) { Handle::operator=(handle); return *this; }
  inline Type* collapse() { return nullptr; }
};

/**
 * @func sync_callback
 */
int sync_callback(Callback cb, cError* e = nullptr, Object* data = nullptr);

/**
 * @func async_callback
 */
void async_callback(Callback cb, PostMessage* loop = nullptr);

/**
 * @func async_callback_and_dealloc
 */
void async_callback_and_dealloc(Callback cb, Error* e, Object* d, PostMessage* loop);

/**
 * @func async_callback
 */
template<class T>
void async_callback(Callback cb, T&& data, PostMessage* loop = nullptr) {
  if ( loop ) {
    async_callback_and_dealloc(cb, nullptr, new T(move(data)), loop);
  } else {
    sync_callback(cb, nullptr, &data);
  }
}

/**
 * @func async_err_callback
 */
template<class T>
void async_err_callback(Callback cb, T&& err, PostMessage* loop = nullptr) {
  if ( loop ) {
    async_callback_and_dealloc(cb, new T(move(err)), nullptr, loop);
  } else {
    sync_callback(cb, &err);
  }
}

/**
 * @func async_callback
 */
template<class T, class T2>
void async_callback(Callback cb, T&& err, T2&& data, PostMessage* loop = nullptr) {
  if ( loop ) {
    async_callback_and_dealloc(cb, new T(move(err)), new T2(move(data)), loop);
  } else {
    sync_callback(cb, &err);
  }
}

/**
 * @class AsyncIOTask
 */
class AsyncIOTask: public Reference {
  av_hidden_all_copy(AsyncIOTask);
public:
  AsyncIOTask();
  virtual ~AsyncIOTask();
  virtual void abort_and_release();
  static void safe_abort(uint id);
  inline bool is_abort() const { return !m_id; }
  inline uint id() const { return m_id; }
private:
  uint m_id;
  bool m_abort;
};

av_end
#endif
