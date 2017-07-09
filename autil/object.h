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

#ifndef __avocado__object__
#define __avocado__object__

#include "macros.h"
#include <atomic>

#ifndef av_memory_trace_mark
# if DEBUG
# define av_memory_trace_mark 1
# else
# define av_memory_trace_mark 0
# endif
#endif

#if av_memory_trace_mark
#include <vector>
#endif

/**
 * @ns avocado
 */

av_ns(avocado)

typedef const char  cchar;
typedef unsigned char byte;
typedef const unsigned char cbyte;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

#if av_unix
# if av_apple
typedef unsigned int uint;
typedef unsigned long ulong;
# elif av_linux
typedef unsigned int uint;
typedef unsigned long ulong;
# endif
#else
typedef unsigned int uint;
typedef unsigned long ulong;
#endif

#if av_arch_64bit
typedef long int int64;
typedef unsigned long int uint64;
#else
typedef long long int int64;
typedef unsigned long long int uint64;
#endif

#define use_default_allocator() \
static void* operator new(ulong size) { return ::operator new(size); } \
static void  operator delete(void* p) { ::operator delete(p); } \
virtual void release() { static_assert(!Characteristic::is_reference, ""); ::delete this; }

// -------------------------------------------------------------------------------

class Object;
class Reference;

template <class T> class Container;
template<class T, class Container = Container<T>> class Array;
template <class Char = char, class _Container = Container<Char>> class BasicString;
typedef BasicString<> String;
typedef const String cString;
typedef BasicString<uint16, Container<uint16>> Ucs2String;
typedef const Ucs2String cUcs2String;
typedef BasicString<uint32, Container<uint32>> Ucs4String;
typedef const Ucs4String cUcs4String;

void set_default_allocator();
void set_allocator(void* (*alloc)(ulong size),
                   void (*release)(Object* obj), void (*retain)(Object* obj));
uint Retain(Object* obj);
void Release(Object* obj);

template<class T, typename... Args>
av_inline T* New(Args... args) { return new T(args...); }

template<class T, typename... Args>
av_inline T* NewRetain(Args... args) { T* r = new T(args...); r->retain(); return r; }

class DefaultCharacteristic;
class ReferenceCharacteristic;

/**
 * @class Object
 */
class Object {
public:
  typedef DefaultCharacteristic Characteristic;
  virtual bool equals(const Object&) const;
  virtual uint hash_code() const;
  virtual String to_string() const;
  virtual bool is_reference() const { return false; }
  virtual bool retain();
  virtual void release();
  static void* operator new(ulong size);
  static void* operator new(ulong size, void* p);
  static void  operator delete(void* p);
#if av_memory_trace_mark
  static std::vector<Object*> mark_objects();
  static int mark_objects_count();
  Object();
  virtual ~Object();
private:
  int initialize_mark_();
  int mark_index_;
#else
  virtual ~Object() { }
#endif
};

/**
 * @class Reference
 */
class Reference: public Object {
public:
  typedef ReferenceCharacteristic Characteristic;
  inline Reference(): m_ref_count(0) { }
  inline Reference(const Reference& ref): m_ref_count(0) { }
  inline Reference& operator=(const Reference& ref) { return *this; }
  virtual ~Reference();
  virtual bool retain();
  virtual void release();
  virtual bool is_reference() const { return true; }
  inline int ref_count() const { return m_ref_count; }
protected:
  std::atomic_int m_ref_count;
};

/**
 * @class DefaultCharacteristic
 */
class DefaultCharacteristic {
public:
  inline static bool Retain(Object* obj) { return obj ? obj->retain() : 0; }
  inline static void Release(Object* obj) { if (obj) obj->release(); }
  static constexpr bool is_reference = false;
  static constexpr bool is_object = true;
};

/**
 * @class ReferenceCharacteristic
 */
class ReferenceCharacteristic: public DefaultCharacteristic {
public:
  static constexpr bool is_reference = true;
};

/**
 * @class ProtocolCharacteristic
 */
class ProtocolCharacteristic {
public:
  template<class T> inline static bool Retain(T* obj) {
    return obj ? dynamic_cast<Object*>(obj)->retain() : 0;
  }
  template<class T> inline static void Release(T* obj) {
    if (obj) dynamic_cast<Object*>(obj)->release();
  }
  static constexpr bool is_reference = false;
};

/**
 * @class NonObjectCharacteristic
 */
class NonObjectCharacteristic {
public:
  template<class T> inline static bool Retain(T* obj) {
    /* Non referential pairs need not be Retain */ return 0;
  }
  template<class T> inline static void Release(T* obj) { delete obj; }
  static constexpr bool is_reference = false;
};

av_end
#endif
