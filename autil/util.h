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

#ifndef __avocado__util__
#define __avocado__util__

#include "object.h"

/**
 * @ns avocado
 */

av_ns(avocado)

namespace console {
  void log(char);
  void log(byte);
  void log(int16);
  void log(uint16);
  void log(int);
  void log(uint);
#if av_arch_32bit
  void log(long);
  void log(ulong);
#endif
  void log(float);
  void log(double);
  void log(int64);
  void log(uint64);
  void log(bool);
  void log(cchar*, ...);
  void log(cString&);
  void log_ucs2(cUcs2String&);
  void warn(cchar*, ...);
  void warn(cString&);
  void error(cchar*, ...);
  void error(cString&);
  void tag(cchar*, cchar*, ...);
  void print(cchar*, ...);
  void print(cString&);
  void print_err(cchar*, ...);
  void print_err(cString&);
  void clear();
}

/**
 * @class Console # util log
 */
class Console: public Object {
public:
  virtual void log(cString& str);
  virtual void warn(cString& str);
  virtual void error(cString& str);
  virtual void print(cString& str);
  virtual void print_err(cString& str);
  virtual void clear();
  void set_to_default();
};

// ----------------- Number Object -----------------

/**
 * @class Number
 */
template <typename T> class Number: public Object {
public:
  inline Number(T v): value(v) { }
  inline T operator*() { return value; }
  inline Number& operator++() { value++; return *this; } // ++i
  inline Number& operator--() { value--; return *this; } // --i
  inline Number  operator++(int) { T v = value; value++; return v; } // i++
  inline Number  operator--(int) { T v = value; value--; return v; } // i--
  template <typename T2> inline T operator=(T2 v) { value = v.value; return value; }
  template <typename T2> inline bool operator==(T2 v) { return value == v.value; }
  template <typename T2> inline bool operator!=(T2 v) { return value != v.value; }
  template <typename T2> inline bool operator<(T2 v) { return value < v.value; }
  template <typename T2> inline bool operator>(T2 v) { return value > v.value; }
  template <typename T2> inline bool operator<=(T2 v) { return value <= v.value; }
  template <typename T2> inline bool operator>=(T2 v) { return value >= v.value; }
  T value;
  static const T min;
  static const T max;
};

#define define_number(NAME, T) \
typedef Number<T> NAME; \
template<> const T NAME::min; template<> const T NAME::max;
define_number(Bool, bool);
define_number(Float, float);
define_number(Double, double);
define_number(Char, char);
define_number(Byte, byte);
define_number(Int16, int16);
define_number(Uint16, uint16);
define_number(Int, int);    typedef Number<int>   Int32;
define_number(Uint, uint);  typedef Number<uint>  Uint32;
define_number(Int64, int64);
define_number(Uint64, uint64);
#undef define_number

// ----------------- Number Object END -----------------

/**
 * @class SimpleHash
 */
class SimpleHash: public Object {
  uint _hash;
public:
  inline SimpleHash(): _hash(5381) { }
  inline uint hash_code() { return _hash; }
  inline void clear() { _hash = 5381; }
  String digest();
  template<class T>
  void update(const T* data, uint len) {
    while (len--) _hash += (_hash << 5) + data[len];
  }
};

extern uint hash_code(cchar* data, uint len);
extern String hash(cchar* data, uint len);
extern String hash(cString& str);
extern int random(uint start = 0, uint end = 0x7fffffff);
extern int fix_random(uint a, ...);
extern void fatal(cchar* file, uint line, cchar* func, cchar* format, ...);
extern void abort();
extern void exit(int signal);
extern uint64 iid();
extern uint   iid32();
extern String version();
extern int64  parse_time(cString& str);
extern String gmt_time_string(int64 second);

namespace _right_reference {
  // remove_reference
  template <class Tp> struct _remove_reference       { typedef Tp type; };
  template <class Tp> struct _remove_reference<Tp&>  { typedef Tp type; };
  template <class Tp> struct _remove_reference<Tp&&> { typedef Tp type; };

  // is_reference
  template <class Tp> struct _is_lvalue_reference      { static const bool value = false; };
  template <class Tp> struct _is_lvalue_reference<Tp&> { static const bool value = true; };
}

// move

template <class Tp>
av_inline constexpr typename _right_reference::_remove_reference<Tp>::type&& move(Tp&& t) {
  typedef typename _right_reference::_remove_reference<Tp>::type Up;
  return static_cast<Up&&>(t);
}

template <class Tp>
av_inline constexpr Tp&& forward(typename _right_reference::_remove_reference<Tp>::type& t) {
  return static_cast<Tp&&>(t);
}

template <class Tp>
av_inline constexpr Tp&& forward(typename _right_reference::_remove_reference<Tp>::type&& t) {
  typedef typename _right_reference::_is_lvalue_reference<Tp> _is_lvalue_reference;
  static_assert(!_is_lvalue_reference::value, "Can not forward an rvalue as an lvalue.");
  return static_cast<Tp&&>(t);
}

av_end

#endif
