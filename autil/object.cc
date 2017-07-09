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

#include "object.h"
#include "string.h"
#include "thread.h"
#include "array.h"
#include <vector>

av_ns(avocado)

static void* default_alloc(ulong size) {
  return malloc(size);
}

static void default_release(Object* obj) {
  obj->~Object();
  free(obj);
}

static void default_retain(Object* obj) {
  // NOOP
}

struct Allocator {
  Allocator() {
    alloc = &default_alloc;
    release = &default_release;
    retain = &default_retain;
  }
  void* (*alloc)(ulong size);
  void (*release)(Object* obj);
  void (*retain)(Object* obj);
};

static Allocator _allocator;

bool Object::equals(const Object& obj) const {
  return this == &obj;
}

uint Object::hash_code() const {
  /*
   * java hashCode
   * 如果该域是boolean类型，则计算(f?1:0)
   * 如果该域是byte、char、short或者int类型，则计算(int)f
   * 如果该域是long类型，则计算(int)(f^(f>>>32))
   * 如果该域是float类型，则计算Float.floatToIntBits(f)
   * 如果该域是double类型，则计算Double.doubleToLongBits(f)，然后重复第三个步骤。
   * 如果该域是一个对象引用，并且该类的equals方法通过递归调用equals方法来比较这个域，
   同样为这个域递归的调用hashCode，如果这个域为null，则返回0。
   * 如果该域是数组，则要把每一个元素当作单独的域来处理，递归的运用上述规则，
   如果数组域中的每个元素都很重要，那么可以使用Arrays.hashCode方法。
   */
  return 0;
}

String Object::to_string() const {
  static String str("[Object]");
  return str;
}

#if av_memory_trace_mark

static int active_mark_objects_count_ = 0;
static Mutex mark_objects_mutex;
static Array<Object*> mark_objects_([](){
  Array<Object*> rv;
  return rv;
}());

int Object::initialize_mark_() {
  if ( mark_index_ == 123456 ) {
    ScopeLock scope(mark_objects_mutex);
    uint index = mark_objects_.length();
    mark_objects_.push(this);
    active_mark_objects_count_++;
    return index;
  }
  return -1;
}

Object::Object(): mark_index_(initialize_mark_()) {

}

Object::~Object() {
  if ( mark_index_ > -1 ) {
    ScopeLock scope(mark_objects_mutex);
    mark_objects_[mark_index_] = nullptr;
    av_assert(active_mark_objects_count_);
    active_mark_objects_count_--;
  }
}

std::vector<Object*> Object::mark_objects() {
  ScopeLock scope(mark_objects_mutex);
  std::vector<Object*> rv;
  Array<Object*> new_mark_objects_;
  
  for ( auto& i : mark_objects_ ) {
    Object* obj = i.value();
    if ( i.value() ) {
      obj->mark_index_ = new_mark_objects_.length();
      new_mark_objects_.push(obj);
      rv.push_back(obj);
    }
  }
  
  av_assert( rv.size() == active_mark_objects_count_ );
  
  mark_objects_ = move(new_mark_objects_);
  return rv;
}

int Object::mark_objects_count() {
  return active_mark_objects_count_;
}

#endif

bool Object::retain() {
  return 0;
}

void Object::release() {
  _allocator.release(this);
}

void* Object::operator new(ulong size) {
  
#if av_memory_trace_mark
  void* p = _allocator.alloc(size);
  ((Object*)p)->mark_index_ = 123456;
  return p;
#else
  return _allocator.alloc(size);
#endif
}

void* Object::operator new(ulong size, void* p) {
  return p;
}

void Object::operator delete(void* p) {
  av_unreachable();
}

void set_default_allocator() {
  _allocator = Allocator();
}

av_init_block(init_allocator) {
  set_default_allocator();
}

void set_allocator(void* (*alloc)(ulong size),
                   void (*release)(Object* obj), void (*retain)(Object* obj)) {
  _allocator.alloc = alloc ? alloc : default_alloc;
  _allocator.release = release ? release : default_release;
  _allocator.retain = retain ? retain : default_retain;
}

uint Retain(Object* obj) {
  return obj ? obj->retain() : 0;
}
void Release(Object* obj) {
  if ( obj )
    obj->release();
}

Reference::~Reference() {
  av_assert( m_ref_count <= 0 );
}

bool Reference::retain() {
  av_assert(m_ref_count >= 0);
  if ( m_ref_count++ == 0 ) {
    _allocator.retain(this);
  }
  return 1;
}

void Reference::release() {
  av_assert(m_ref_count >= 0);
  if ( --m_ref_count <= 0 ) { // 当引用记数小宇等于0释放
    _allocator.release(this);
  }
}

av_end
