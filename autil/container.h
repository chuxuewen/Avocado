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

#ifndef __avocado__container__
#define __avocado__container__

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"

/**
 * @ns avocado
 */

av_ns(avocado)

#ifndef av_min_capacity
#define av_min_capacity (8)
#endif

/**
 * @class Container
 */
template <class T> class Container {
public:
  
  Container (uint capacity = 0): m_capacity(0), m_value(nullptr) {
    if ( capacity ) {
      m_capacity = powf(2, ceil(log2(av_max(av_min_capacity, capacity))));
      m_value = static_cast<T*>(::malloc(size()));
      av_assert(m_value);
    }
  }
  
  Container(uint capacity, T* value, bool protect = false)
  : m_capacity(capacity), m_value(value)
  { }
  
  Container(const Container& container) : m_capacity(0), m_value(nullptr) {
    m_capacity = container.m_capacity;
    if (m_capacity) {
      m_value = static_cast<T*>(::malloc(container.size()));
      ::memcpy((void*)m_value, (void*)*container, container.size());
    }
  }
  
  Container(Container&& container) : m_capacity(0), m_value(nullptr) {
    Container::move(avocado::move(container));
  }
  
  virtual ~Container() {
    Container::free();
  }
  
  inline uint capacity() const { return m_capacity; }
  inline uint size() const { return sizeof(T) * m_capacity; }
  inline T* operator*() { return m_value; }
  inline const T* operator*() const { return m_value; }
  
  inline Container& operator=(const Container& container) {
    auto_realloc(container.m_capacity, false);
    ::memcpy((void*)m_value, (void*)*container, container.size());
    return *this;
  }
  
  inline Container& operator=(Container&& container) {
    move(avocado::move(container));
    return *this;
  }
  
  /**
   * @func auto_realloc # auro realloc
   * @arg capacity {uint}
   * @arg [retain_data = true] {bool} # 保留数据
   */
  void auto_realloc(uint capacity, bool retain_data = true) {
    capacity = av_max(av_min_capacity, capacity);
    if ( capacity > m_capacity || capacity < m_capacity / 4.0 ) {
      capacity = powf(2, ceil(log2(capacity)));
      realloc(capacity, retain_data);
    }
  }
  
  // -------------------- virtual --------------------
  
  virtual bool move(Container&& container) {
    uint capacity = container.m_capacity;
    if ( capacity ) {
      T* value = container.collapse();
      if ( value ) {
        Container::free();
        m_capacity = capacity;
        m_value = value;
        return true;
      }
    } else {
      Container::free();
      return true;
    }
    return false;
  }
  
  /**
   * @func realloc
   * @arg capacity {uint}
   * @arg [retain_data = true] {bool} # 如果重新分配的空间小于当前,会有丢失部分数据
   */
  virtual void realloc(uint capacity, bool retain_data = true) {
    if (capacity == 0) {
      Container::free();
    } else if ( capacity != m_capacity ) {
      uint size = sizeof(T) * capacity;
      if (retain_data) {
        m_value = static_cast<T*>(m_value ? ::realloc(m_value, size) : ::malloc(size));
      } else {
        ::free(m_value);
        m_value = static_cast<T*>(::malloc(size));
      }
      m_capacity = capacity;
    }
  }
  
  /**
   * @func collapse
   */
  virtual T* collapse() {
    T* rev = m_value;
    m_capacity = 0;
    m_value = nullptr;
    return rev;
  }
  
  virtual void free() {
    ::free(m_value);
    m_capacity = 0;
    m_value = nullptr;
  }

protected:
  
  uint  m_capacity;
  T*    m_value;
};

av_end
#endif
