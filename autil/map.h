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

#ifndef __avocado__map__
#define __avocado__map__

#include "util.h"
#include "container.h"
#include "iterator.h"
#include "array.h"

av_ns(avocado)

/**
 * @ns
 */

/**
 * @class Compare
 */
template<class T> class Compare {
public:
  static uint hash(const T& key);
  static bool equals(const T& a, const T& b, uint ha, uint hb);
};

template<class T> class PrtKey {
public:
  inline PrtKey(T* p): _ptr(p) { }
  inline bool equals(const PrtKey& o) const { return o._ptr == _ptr; }
  inline uint hash_code() const { return size_t(_ptr) % Uint::max; }
private:
  T* _ptr;
};

/**
 * @class Map Hash map
 * @template<class Key, class Value>
 */
template<
  class _Key,
  class _Value,
  class Compare = Compare<_Key>
>
class Map: public Object {
public:
  typedef _Key Key;
  typedef _Value Value;
  
private:
  
  struct Item {
    Key   _key;
    Value _value;
    Item* _prev;
    Item* _next;
    uint  _hash;
    bool  _mark;
  };
  
  struct Bucket {
    Item* _first;
    Item* _last;
  };
  
  struct iterator {
  public:
    typedef _Value Value;
    const Key& key() const;
    Key& key();
    const Value& value() const;
    Value& value();
  private:
    iterator();
    iterator(Map* host, Item* item);
    bool equals(const iterator& it) const;
    bool is_null() const;
    void begen();
    void prev();
    void next();
    Map* _host;
    Item* _item;
    friend class Map;
    friend class ConstIteratorTemplate<iterator>;
    friend class IteratorTemplate<iterator>;
  };
  
  class Buckets: public Container<Bucket> {
  public:
    void realloc(uint capacity, bool retain_data);
    void auto_realloc();
    Map* _host;
  };
  
  typedef Array<Item*> Marks;
  
  friend class Buckets;
  friend class iterator;

public:
  
  typedef ConstIteratorTemplate<iterator> ConstIterator;
  typedef IteratorTemplate<iterator> Iterator;
  
  Map();
  Map(const Map& value);
  Map(Map&& value);
  
  virtual ~Map();
  
  /**
   * @func operator=
   */
  Map& operator=(const Map& value);
  
  /**
   * @func operator=
   */
  Map& operator=(Map&& value);
  
  /**
   * @func operator[]
   */
  const Value& operator[](const Key& key) const;
  
  /**
   * @func operator[]
   */
  Value& operator[](const Key& key);
  
  /**
   * @func operator[]
   */
  Value& operator[](Key&& key);
  
  /**
   * @func find
   */
  ConstIterator find(const Key& key) const;
  
  /**
   * @func find
   */
  Iterator find(const Key& key);
  
  /**
   * @func has
   */
  bool has(const Key& key) const;
  
  /**
   * @func keys
   */
  Array<Key> keys() const;
  
  /**
   * @func values
   */
  Array<Value> values() const;
  
  /**
   * @func get
   */
  inline const Value& get(const Key& key) const { return operator[](key); }
  
  /**
   * @func get
   */
  inline Value& get(const Key& key) { return operator[](key); }
  
  /**
   * @func get
   */
  inline Value& get(Key&& key) { return operator[](move(key)); }
  
  /**
   * @func set
   */
  Value& set(const Key& key, const Value& value);
  
  /**
   * @func set
   */
  Value& set(Key&& key, Value&& value);
  
  /**
   * @func set
   */
  Value& set(Key&& key, const Value& value);
  
  /**
   * @func set
   */
  Value& set(const Key& key, Value&& value);
  
  /**
   * @func del
   */
  bool del(const Key& key);
  
  /**
   * @func del
   */
  bool del(ConstIterator it);
  
  /**
   * @func clear
   */
  void clear();
  
  /**
   * @func mark
   */
  void mark(const Key& key);
  
  /**
   * @func mark
   */
  void mark(ConstIterator it);
  
  /**
   * @func del_mark
   */
  void del_mark();
  
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
   * @func length {uint}
   */
  uint length() const;
  
private:
  
  Item* _find(const Key& key);
  Item* _find_set(const Key& key, bool* is_new);
  void  _del(Item* item);
  
  uint    _length;
  Buckets _buckets;
  Marks   _marks;
};

av_end

#include "map.h.inl"

#endif
