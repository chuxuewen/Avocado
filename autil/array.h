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

#ifndef __avocado__array__
#define __avocado__array__

#include "./util.h"
#include "./container.h"
#include "./iterator.h"
#include "./error.h"

/**
 * @ns avocado
 */

av_ns(avocado)

template<class T> class ArrayBuffer;
template<class T> class WeakArrayBuffer;

/**
 * @class Array
 */
template<class T, class Container>
class Array: public Object {
private:
  
  struct Wrap { T _item; };
  
  struct iterator {
  public:
    typedef T Value;
    const T& value() const;
    T& value();
    inline int index() const { return _index; }
  private:
    iterator();
    iterator(Array* host, uint index);
    bool equals(const iterator& it) const;
    bool is_null() const;
    void prev();
    void next();
    Array* _host;
    int _index;
    friend class Array;
    friend class ConstIteratorTemplate<iterator>;
    friend class IteratorTemplate<iterator>;
  };
  
protected:
  
  Array(T* data, uint length);

public:
  
  typedef ConstIteratorTemplate<iterator> ConstIterator;
  typedef IteratorTemplate<iterator> Iterator;
  
  Array(const Array&);
  Array(Array&&);
  Array(uint length = 0, uint capacity = 0);
  
  virtual ~Array();
  
public:
  
  /**
   * @func operator=
   */
  Array& operator=(const Array&);
  
  /**
   * @func operator=
   */
  Array& operator=(Array&&);
  
  /**
   * @func operator[]
   */
  const T& operator[](uint index) const;
  
  /**
   * @func operator[]
   */
  T& operator[](uint index);
  
  /**
   * @func item
   */
  const T& item(uint index) const;
  
  /**
   * @func item
   */
  T& item(uint index);
  
  /**
   * @func set
   */
  T& set(uint index, const T& item);
  
  /**
   * @func set
   */
  T& set(uint index, T&& item);
  
  /**
   * @func push
   */
  uint push(const T& item);
  
  /**
   * func push
   */
  uint push(T&& item);
  
  /**
   * @func push
   */
  uint push(const Array& arr);
  
  /**
   * @func push
   */
  uint push(Array&& arr);
  
  /**
   * @func pop
   */
  void pop();
  
  /**
   * @func pop
   */
  void pop(uint count);
  
  /**
   * @func slice
   */
  Array slice(uint start);
  
  /**
   * @func slice
   */
  Array slice(uint start, uint end);
  
  /**
   * @func write
   * @arg src 
   * @arg to {int=-1} 当前数组开始写入的位置,-1从结尾开始写入
   * @arg size {int=-1} 需要写入项目数量,超过要写入数组的长度自动取写入数组长度,-1写入全部
   * @arg form {int=0} 从要写入数组的form位置开始取数据
   */
  uint write(const Array& src, int to = -1, int size = -1, uint form = 0);
  
  /**
   * @func write
   */
  uint write(const T* src, int to, uint size);
    
  /**
   * @func clear
   */
  virtual void clear();
  
  /**
   * @func join
   */
  String join(cString& sp) const;
  
  /**
   * @func begin
   */
  ConstIterator begin() const;
  
  /**
   * @func end
   */
  ConstIterator end() const;
  
  /**
   * @func begin
   */
  Iterator begin();
  
  /**
   * @func end
   */
  Iterator end();
  
  /**
   * @func length
   */
  uint length() const;
  
  /**
   * @func capacity
   */
  uint capacity() const;
  
private:
  
  friend class ArrayBuffer<T>;
  friend class WeakArrayBuffer<T>;
  
  uint        _length;
  Container   _container;
};

av_end

#include "array.h.inl"

#endif
