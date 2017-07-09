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

#include <new>

#define av_def_array_special(T, Container) \
template<>                        Array<T, Container<T>>::Array(uint length, uint capacity); \
template<> uint                   Array<T, Container<T>>::push(const Array& arr); \
template<> uint                   Array<T, Container<T>>::push(Array&& arr); \
template<> Array<T, Container<T>> Array<T, Container<T>>::slice(uint start, uint end); \
template<> uint Array<T, Container<T>>::write(const T* src, int to, uint size);\
template<> void                   Array<T, Container<T>>::pop();            \
template<> void                   Array<T, Container<T>>::pop(uint count);  \
template<> void                   Array<T, Container<T>>::clear()

av_ns(avocado)

// iterator

template<class Item, class Container>
Array<Item, Container>::iterator::iterator() : _host(NULL), _index(0) { }

template<class Item, class Container>
Array<Item, Container>::iterator::iterator(Array* host, uint index)
: _host(host), _index(index) { }

template<class Item, class Container>
const Item& Array<Item, Container>::iterator::value() const {
  av_assert(_host);
  return (*_host)[_index];
}

template<class Item, class Container>
Item& Array<Item, Container>::iterator::value() {
  av_assert(_host);
  return (*_host)[_index];
}

template<class Item, class Container>
bool Array<Item, Container>::iterator::equals(const iterator& it) const {
  return it._index == _index;
}

template<class Item, class Container>
bool Array<Item, Container>::iterator::is_null() const {
  return !_host || _index == _host->_length;
}

template<class Item, class Container>
void Array<Item, Container>::iterator::prev() { // --
  if (_host)
    _index = av_max(0, _index - 1);
}

template<class Item, class Container>
void Array<Item, Container>::iterator::next() { // ++
  if (_host)
    _index = av_min(_host->_length, _index + 1);
}

// Array

template<class Item, class Container>
Array<Item, Container>::Array(uint length, uint capacity)
: _length(length), _container(av_max(length, capacity))
{ 
  if (_length) {
    Item* begin = *_container;
    Item* end = begin + _length;
    
    while (begin < end) {
      new(begin) Item(); // 调用默认构造
      begin++;
    }
  }
}

template<class Item, class Container>
Array<Item, Container>::Array(Item* data, uint length) : _length(length), _container(length, data)
{
  
}

template<class Item, class Container>
Array<Item, Container>::Array(const Array& arr) : _length(0), _container(0)
{
  push(arr);
}

template<class Item, class Container>
Array<Item, Container>::Array(Array&& arr) : _length(0), _container(0)
{
  if ( _container.move(move(arr._container)) ) {
    _length = arr._length;
    arr._length = 0;
  }
}

template<class Item, class Container> Array<Item, Container>::~Array() {
  clear();
}

template<class Item, class Container>
Array<Item, Container>& Array<Item, Container>::operator=(const Array& arr) {
  clear(); push(arr);
  return *this;
}

template<class Item, class Container>
Array<Item, Container>& Array<Item, Container>::operator=(Array&& arr) {
  clear();
  if ( _container.move(move(arr._container)) ) {
    _length = arr._length;
    arr._length = 0;
  }
  return *this;
}

template<class Item, class Container>
const Item& Array<Item, Container>::operator[](uint index) const {
  // av_assert_err(index < _length, "Array access violation.");
  av_assert(index < _length);
  return (*_container)[index];
}

template<class Item, class Container>
Item& Array<Item, Container>::operator[](uint index) {
  av_assert(index < _length);
  return (*_container)[index];
}

template<class Item, class Container>
const Item& Array<Item, Container>::item(uint index) const {
  return operator[](index);
}

template<class Item, class Container>
inline Item& Array<Item, Container>::item(uint index) {
  return operator[](index);
}

template<class Item, class Container>
Item& Array<Item, Container>::set(uint index, const Item& item) {
  av_assert(index <= _length);
  if ( index < _length ) {
    return ((*_container)[index] = item);
  }
  return (*_container)[push(item) - 1];
}

template<class Item, class Container>
Item& Array<Item, Container>::set(uint index, Item&& item) {
  av_assert(index <= _length);
  if ( index < _length ) {
    return ((*_container)[index] = move(item));
  }
  return (*_container)[push(move(item)) - 1];
}

template<class Item, class Container>
uint Array<Item, Container>::push(const Item& item) {
  _length++;
  _container.auto_realloc(_length);
  new((*_container) + _length - 1) Item(item);
  return _length;
}

template<class Item, class Container>
uint Array<Item, Container>::push(Item&& item) {
  _length++;
  _container.auto_realloc(_length);
  new((*_container) + _length - 1) Item(avocado::move(item));
  return _length;
}

template<class Item, class Container>
uint Array<Item, Container>::push(const Array& arr) {
  if (arr._length) {
    _length += arr._length;
    _container.auto_realloc(_length);
    
    const Item* source = *arr._container;
    Item* end = (*_container) + _length;
    Item* target = end - arr._length;
    
    while (target < end) {
      new(target) Item(*source); // 调用复制构造
      source++; target++;
    }
  }
  return _length;
}

template<class Item, class Container>
uint Array<Item, Container>::push(Array&& arr) {
  if (arr._length) {
    _length += arr._length;
    _container.auto_realloc(_length);
    
    const Item* item = *arr._container;
    Item* end = (*_container) + _length;
    Item* begin = end - arr._length;
    
    while (begin < end) {
      new(begin) Item(move(*item)); // 调用复制构造
      item++; begin++;
    }
  }
  return _length;
}

template<class Item, class Container>
inline Array<Item, Container> Array<Item, Container>::slice(uint start) {
  return slice(start, _length);
}

template<class Item, class Container>
Array<Item, Container> Array<Item, Container>::slice(uint start, uint end) {
  end = av_min(end, _length);
  if (start < end) {
    Array arr;
    arr._length = end - start;
    arr._container.auto_realloc(arr._length);
    Item* tar = *arr._container;
    Item* e = tar + arr._length;
    const Item* src = *_container + start;
    while (tar < e) {
      new(tar) Item(*src);
      tar++; src++;
    }
  }
  return Array();
}

template<class Item, class Container>
uint Array<Item, Container>::write(const Array& arr, int to, int size, uint form) {
  int s = av_min(arr._length - form, size < 0 ? arr._length : size);
  if (s > 0) {
    return write((*arr._container) + form, to, s);
  }
  return 0;
}

/**
 * @func write
 */
template<class Item, class Container>
uint Array<Item, Container>::write(const Item* src, int to, uint size) {
  if (size) {
    if ( to == -1 ) to = _length;
    uint old_len = _length;
    uint end = to + size;
    _length = av_max(end, _length);
    _container.auto_realloc(_length);
    Item* tar = (*_container) + to;
    
    for (int i = to; i < end; i++) {
      if (i < old_len) {
        reinterpret_cast<Wrap*>(tar)->~Wrap(); // 先释放原对像
      }
      new(tar) Item(*src);
      tar++; src++;
    }
  }
  return size;
}

template<class Item, class Container>
void Array<Item, Container>::pop() {
  if (_length) {
    _length--;
    reinterpret_cast<Wrap*>((*_container) + _length)->~Wrap(); // 释放
    _container.auto_realloc(_length);
  }
}

template<class Item, class Container>
void Array<Item, Container>::pop(uint count) {
  
  int j = av_max(_length - count, 0);
  
  if (_length > j) {
    do {
      _length--;
      reinterpret_cast<Wrap*>((*_container) + _length)->~Wrap(); // 释放
    } while (_length > j);
    
    _container.auto_realloc(_length);
  }
}

template<class Item, class Container> void Array<Item, Container>::clear() {
  if (_length) {
    Item* item = *_container;
    Item* end = item + _length;
    while (item < end) {
      reinterpret_cast<Wrap*>(item)->~Wrap(); // 释放
      item++;
    }
    _length = 0;
  }
  _container.free();
}

template<class Item, class Container>
typename Array<Item, Container>::ConstIterator Array<Item, Container>::begin() const {
  return ConstIterator(iterator(const_cast<Array*>(this), 0));
}

template<class Item, class Container>
typename Array<Item, Container>::ConstIterator Array<Item, Container>::end() const {
  return ConstIterator(iterator(const_cast<Array*>(this), _length));
}

template<class Item, class Container>
typename Array<Item, Container>::Iterator Array<Item, Container>::begin() {
  return Iterator(iterator(this, 0));
}

template<class Item, class Container>
typename Array<Item, Container>::Iterator Array<Item, Container>::end() {
  return Iterator(iterator(this, _length));
}

template<class Item, class Container>
inline uint Array<Item, Container>::length() const {
  return _length;
}

template<class Item, class Container>
inline uint Array<Item, Container>::capacity() const {
  return _container.capacity();
}

av_def_array_special(char, Container);
av_def_array_special(byte, Container);
av_def_array_special(int16, Container);
av_def_array_special(uint16, Container);
av_def_array_special(int, Container);
av_def_array_special(uint, Container);
av_def_array_special(int64, Container);
av_def_array_special(uint64, Container);
av_def_array_special(float, Container);
av_def_array_special(double, Container);
av_def_array_special(bool, Container);

av_end
