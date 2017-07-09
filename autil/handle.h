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

#ifndef __avocado__handle__
#define __avocado__handle__

#include <functional>
#include "util.h"

/**
 * @ns avocado
 */

av_ns(avocado)

/**
 * @class Handle RAII, Resource Acquisition Is Initialization
 */
template<class T, class _Allocator = typename T::Characteristic> class Handle {
  //! Copy constructor is not permitted.
  av_hidden_all_copy(Handle);
  
  inline T* shift() {
    return Allocator::Retain(m_data) ? m_data : collapse();
  }
  
public:
  typedef T Type;
  typedef _Allocator Allocator;
  
  inline Handle(): m_data(nullptr) { }
  inline Handle(T* data): m_data(data) { Allocator::Retain(data); }
  inline Handle(Handle& handle) { m_data = handle.shift(); }
  inline Handle(Handle&& handle) { m_data = handle.shift(); }
  
  ~Handle() { clear(); }

  /**
   * @operator=
   */
  inline Handle& operator=(Handle& handle) {
    clear();
    m_data = handle.shift();
    return *this;
  }
  
  /**
   * @operator=
   */
  inline Handle& operator=(Handle&& handle) {
    clear();
    m_data = handle.shift();
    return *this;
  }
  
  inline T* operator->() { return m_data; }
  inline const T* operator->() const { return m_data; }
  inline T* operator*() { return m_data; }
  inline const T* operator*() const { return m_data; }
  inline const T* value() const { return m_data; }
  inline T* value() { return m_data; }
  
  /**
   * @func is_null # Whether the air data
   */
  inline bool is_null() const { return m_data == nullptr; }
  
  /**
   * @func collapse # 解绑数据
   *                # 调用函数失去对数据的管理权,数据被移走
   */
  inline T* collapse() {
    T* data = m_data; m_data = nullptr;
    return data;
  }
  
  /**
   * @func clear
   * */
  inline void clear() {
    Allocator::Release(m_data); m_data = nullptr;
  }
  
private:
  
  T* m_data;
};

/**
 * @class ScopeClear
 */
class ScopeClear {
public:
  typedef std::function<void()> Clear;
  ScopeClear(Clear clear): m_clear(clear) { }
  ~ScopeClear() { m_clear(); }
  inline void cancel() { m_clear = [](){ }; }
private:
  Clear m_clear;
};

av_end
#endif
