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

#ifndef __avocado__buffer__
#define __avocado__buffer__

#include "array.h"
#include "codec.h"

/**
 * @ns avocado
 */

av_ns(avocado)

/**
 * @class BufferContainer
 */
template<class T>
class BufferContainer: public Container<T> {
public:
  
  inline BufferContainer(uint capacity = 0)
  : Container<T>(capacity), m_weak(false) { }
  
  inline BufferContainer(uint capacity, T* value)
  : Container<T>(capacity, value), m_weak(false) { }
  
  virtual ~BufferContainer() {
    free();
  }
  
  inline BufferContainer& operator=(const BufferContainer& container) {
    if ( m_weak ) {
      this->m_capacity = container.capacity();
      this->m_value = const_cast<T*>(*container);
    } else {
      Container<T>::operator=(container);
    }
    return *this;
  }
  
  /**
   * @overwrite
   */
  virtual bool move(Container<T>&& container) {
    if ( m_weak ) {
      return false;
    } else {
      return Container<T>::move(avocado::move(container));
    }
  }
  
  virtual void realloc(uint capacity, bool retain_data = true) {
    if ( m_weak ) {
      av_unreachable();
    } else {
      Container<T>::realloc(capacity, retain_data);
    }
  }
  
  virtual T* collapse() {
    if ( m_weak ) {
      return nullptr;
    }
    return Container<T>::collapse();
  }
  
  virtual void free() {
    if ( !m_weak ) {
      ::free(this->m_value);
    }
    this->m_capacity = 0;
    this->m_value = nullptr;
  }
  
private:
  
  bool m_weak;
  
  friend class WeakArrayBuffer<T>;
  
};

av_def_array_special(char, BufferContainer);
av_def_array_special(byte, BufferContainer);
av_def_array_special(int16, BufferContainer);
av_def_array_special(uint16, BufferContainer);
av_def_array_special(int, BufferContainer);
av_def_array_special(uint, BufferContainer);
av_def_array_special(int64, BufferContainer);
av_def_array_special(uint64, BufferContainer);
av_def_array_special(float, BufferContainer);
av_def_array_special(double, BufferContainer);
av_def_array_special(bool, BufferContainer);

/**
 * @class ArrayBuffer
 */
template<class T>
class ArrayBuffer: public Array<T, BufferContainer<T>> {
private:
  
  ArrayBuffer(const ArrayBuffer& arr): Array<T, BufferContainer<T>>(arr) { }
  
  /**
   * @func operator=
   */
  inline ArrayBuffer& operator=(const ArrayBuffer& arr) = delete;
  
public:
  
  inline ArrayBuffer(uint length = 0, uint capacity = 0)
  : Array<T, BufferContainer<T>>(length, capacity) {
    
  }
  
  inline ArrayBuffer(T* data, uint length) : Array<T, BufferContainer<T>>(data, length) {
  
  }
  
  inline ArrayBuffer(ArrayBuffer& arr): Array<T, BufferContainer<T>>( move(arr) ) {
  
  }
  
  inline ArrayBuffer(ArrayBuffer&& arr): Array<T, BufferContainer<T>>( move(arr) ) {
  
  }
  
  /**
   * @func operator=
   */
  inline ArrayBuffer& operator=(ArrayBuffer& arr) {
    Array<T, BufferContainer<T>>::operator=(avocado::move(arr));
    return *this;
  }
  
  /**
   * @func operator=
   */
  inline ArrayBuffer& operator=(ArrayBuffer&& arr) {
    Array<T, BufferContainer<T>>::operator=(avocado::move(arr));
    return *this;
  }
  
  inline bool is_weak() const { return this->_container.m_weak; }
  
  /**
   * @func operator*
   */
  inline T* operator*() { return *this->_container; }
  
  /**
   * @func operator*
   */
  inline const T* operator*() const { return *this->_container; }
  
  /**
   * @func value
   */
  inline const T* value() const { return *this->_container; }
  
  /**
   * @func value
   */
  inline T* value() { return *this->_container; }
  
  /**
   * @func size 获取数据占用内存大小
   */
  inline uint size() const { return this->_length * sizeof(T); }
  
  /**
   * @func copy
   */
  inline ArrayBuffer copy() const {
    return ArrayBuffer(*(const ArrayBuffer*)this);
  }
  
  /**
   * @func is_null # Whether the air data
   */
  inline bool is_null() const {
    return *this->_container == nullptr;
  }
  
  /**
   * @func collapse
   */
  T* collapse() {
    T* value = this->_container.collapse();
    if ( value ) {
      this->_length = 0;
    }
    return value;
  }

  /**
   * @func collapse_string
   */
  inline BasicString<T> collapse_string() {
    return BasicString<T>(move(*this));
  }
  
};

/**
 * @class WeakArrayBuffer
 */
template<class T> class WeakArrayBuffer: public ArrayBuffer<T> {
public:

  inline WeakArrayBuffer(): ArrayBuffer<T>(nullptr, 0) {
    this->_container.m_weak = true;
  }
  
  inline WeakArrayBuffer(const T* data, uint length)
  : ArrayBuffer<T>(const_cast<T*>(data), length) {
    this->_container.m_weak = true;
  }
  
  inline WeakArrayBuffer(const WeakArrayBuffer& arr)
  : ArrayBuffer<T>(const_cast<T*>(*arr), arr._length) {
    this->_container.m_weak = true;
  }
  
  inline WeakArrayBuffer(const ArrayBuffer<T>& arr)
  : ArrayBuffer<T>(const_cast<T*>(*arr), arr._length) {
    this->_container.m_weak = true;
  }
  
  template<class T2>
  inline WeakArrayBuffer(const Array<T, T2>& arr)
  : ArrayBuffer<T>(const_cast<T*>(*arr), arr._length) {
    this->_container.m_weak = true;
  }
  
  virtual ~WeakArrayBuffer() {
    clear();
  }
  
  /**
   * @func operator=
   */
  inline WeakArrayBuffer& operator=(const WeakArrayBuffer& arr) {
    this->_length = arr._length;
    this->_container = arr._container;
    this->_container.m_weak = true;
    return *this;
  }
  
  /**
   * @overwrite
   */
  virtual void clear() {
    this->_length = 0;
    this->_container.free();
  }
  
};

typedef ArrayBuffer<char> Buffer;
typedef const ArrayBuffer<char> cBuffer;
typedef WeakArrayBuffer<char> WeakBuffer;
typedef const WeakArrayBuffer<char> cWeakBuffer;

av_end

#endif
