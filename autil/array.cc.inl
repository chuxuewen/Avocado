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

#define av_def_array_special_implementation(T, Container)             \
\
template<> Array<T, Container<T>>::Array(uint length, uint capacity) \
: _length(length), _container(av_max(length, capacity)) { \
  if (_length) {  \
    memset(*_container, 0, sizeof(T) * _length); \
  }\
}\
\
template<> uint Array<T, Container<T>>::push(const Array& arr) { \
  if (arr._length) { \
    _length += arr._length; \
    _container.auto_realloc(_length); \
    const T* item = *arr._container; \
    T* begin = (*_container) + _length - arr._length; \
    memcpy(begin, item, arr._length * sizeof(T)); \
  } \
  return _length; \
}\
\
template<> uint Array<T, Container<T>>::push(Array&& arr) { \
  if (arr._length) { \
    _length += arr._length; \
    _container.auto_realloc(_length); \
    const T* item = *arr._container; \
    T* begin = (*_container) + _length - arr._length; \
    memcpy(begin, item, arr._length * sizeof(T)); \
  } \
  return _length; \
} \
\
template<> void Array<T, Container<T>>::pop() {           \
  if (_length) {                                          \
    _length--;                                            \
    _container.auto_realloc(_length);                     \
  }                                                       \
}                                                         \
\
template<> Array<T, Container<T>> Array<T, Container<T>>::slice(uint start, uint end) { \
  end = av_min(end, _length); \
  if (start < end) { \
    Array arr; \
    arr._length = end - start; \
    arr._container.auto_realloc(arr._length); \
    memcpy(*arr._container, (*_container) + start, arr._length * sizeof(T) ); \
  } \
  return Array(); \
}\
\
template<> uint Array<T, Container<T>>::write(const T* src, int to, uint size) { \
  if (size) { \
    if ( to == -1 ) to = _length; \
    _length = av_max(to + size, _length); \
    _container.auto_realloc(_length); \
    memcpy((*_container) + to, src, size * sizeof(T) ); \
  } \
  return size; \
} \
\
template<> void Array<T, Container<T>>::pop(uint count) { \
  int j = av_max(_length - count, 0);                     \
  if (_length > j) {                                      \
    _length = j;                                          \
    _container.auto_realloc(_length);                     \
  }                                                       \
}                                                         \
\
template<> void Array<T, Container<T>>::clear() {       \
  _length = 0;                                          \
  _container.free();                                   \
}\
