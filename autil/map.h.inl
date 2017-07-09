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

av_ns(avocado)

// Compare

template<class T> uint Compare<T>::hash(const T& key) {
  return key.hash_code();
}

template<class T> bool Compare<T>::equals(const T& a, const T& b, uint code_a, uint code_b) {
  return a.equals(b);
}
// hash
template<> uint Compare<char>::hash(const char& key);
template<> uint Compare<byte>::hash(const byte& key);
template<> uint Compare<int16>::hash(const int16& key);
template<> uint Compare<uint16>::hash(const uint16& key);
template<> uint Compare<int>::hash(const int& key);
template<> uint Compare<uint>::hash(const uint& key);
template<> uint Compare<int64>::hash(const int64& key);
template<> uint Compare<uint64>::hash(const uint64& key);
template<> uint Compare<float>::hash(const float& key);
template<> uint Compare<double>::hash(const double& key);
template<> uint Compare<bool>::hash(const bool& key);
// equals
template<> bool Compare<String>::equals(const String& a, const String& b, uint ha, uint hb);
template<> bool Compare<char>::equals(const char& a, const char& b, uint ha, uint hb);
template<> bool Compare<byte>::equals(const byte& a, const byte& b, uint ha, uint hb);
template<> bool Compare<int16>::equals(const int16& a, const int16& b, uint ha, uint hb);
template<> bool Compare<uint16>::equals(const uint16& a, const uint16& b, uint ha, uint hb);
template<> bool Compare<int>::equals(const int& a, const int& b, uint ha, uint hb);
template<> bool Compare<uint>::equals(const uint& a, const uint& b, uint ha, uint hb);
template<> bool Compare<int64>::equals(const int64& a, const int64& b, uint ha, uint hb);
template<> bool Compare<uint64>::equals(const uint64& a, const uint64& b, uint ha, uint hb);
template<> bool Compare<float>::equals(const float& a, const float& b, uint ha, uint hb);
template<> bool Compare<double>::equals(const double& a, const double& b, uint ha, uint hb);
template<> bool Compare<bool>::equals(const bool& a, const bool& b, uint ha, uint hb);

// ConstIterator

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Buckets::realloc(uint capacity, bool retain_data) {
  av_assert(capacity >= ceilf(_host->_length / 0.7) );
  
  if (capacity == 0) {
    ::free(this->m_value);
    this->m_capacity = 0;
    this->m_value = NULL;
    return;
  }
  
  uint size = sizeof(Bucket) * capacity;
  Bucket* value = static_cast<Bucket*>(::malloc(size));
  memset(value, 0, size);
  
  if (_host->_length) { // 调整容量
    Bucket* i = this->m_value;
    Bucket* end = this->m_value + this->capacity();
    
    while (i < end) {
      if (i->_first) { // 非空
        
        Item* item = i->_first;
        while (item) { // 移动item
          Item* next = item->_next;
          uint index = item->_hash % capacity;
          Bucket* buk = value + index;
          
          if (buk->_first) { // 冲突
            buk->_last->_next = item;
            item->_prev = buk->_last;
            item->_next = NULL;
            buk->_last = item;
          } else {
            buk->_first = item;
            buk->_last = item;
            item->_prev = NULL;
            item->_next = NULL;
          }
          item = next;
        }
      }
      i++;
    }
    
    ::free(this->m_value);
  } else {
    av_assert(!this->m_capacity);
  }
  
  this->m_capacity = capacity;
  this->m_value = value;
}

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Buckets::auto_realloc() {
  // 使用超过70%需要增加容量
  Container<Bucket>::auto_realloc( ceilf(_host->_length / 0.7f) );
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>::iterator::iterator(): _host(NULL), _item(NULL) {
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>::iterator::iterator(Map* host, Item* item) : _host(host), _item(item) {
}

template<class Key, class Value, class Compare>
const Key& Map<Key, Value, Compare>::iterator::key() const {
  av_assert(_item);
  return _item->_key;
}

template<class Key, class Value, class Compare>
Key& Map<Key, Value, Compare>::iterator::key() {
  av_assert(_item);
  return _item->_key;
}

template<class Key, class Value, class Compare>
const Value& Map<Key, Value, Compare>::iterator::value() const {
  av_assert(_item);
  return _item->_value;
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::iterator::value() {
  av_assert(_item);
  return _item->_value;
}

template<class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::iterator::equals(const iterator& it) const {
  return _item == it._item;
}

template<class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::iterator::is_null() const {
  return _item == nullptr;
}

template<class Key, class Value, class Compare> void Map<Key, Value, Compare>::iterator::begen() {
  Bucket* i = *_host->_buckets;
  Bucket* end = i + _host->_buckets.capacity();
  while (i < end) {
    if (i->_first) {
      _item = i->_first; return;
    }
    i++;
  }
  _item = NULL;
}

template<class Key, class Value, class Compare> void Map<Key, Value, Compare>::iterator::prev() {
  Bucket* buk = NULL;
  
  if (_item) {
    if (_item->_prev) {
      _item = _item->_prev; return;
    }
    else {
      buk = *_host->_buckets + ((_item->_hash % _host->_buckets.capacity()) - 1);
    }
  } else { // 查找最后一个
    buk = *_host->_buckets + (_host->_buckets.capacity() - 1);
  }
  
  // 向上查找
  Bucket* begin = *_host->_buckets;
  while (buk >= begin) {
    if (buk->_last) {
      _item = buk->_last; return;
    }
    buk--;
  }
}

template<class Key, class Value, class Compare> void Map<Key, Value, Compare>::iterator::next() {
  if (_item) {
    if (_item->_next) {
      _item = _item->_next; return;
    } else {
      uint capacity = _host->_buckets.capacity();
      Bucket* buk = *_host->_buckets;
      Bucket* i = buk + ((_item->_hash % capacity) + 1);
      buk = buk + capacity;
      
      while (i < buk) {
        if (i->_first) {
          _item = i->_first; return;
        }
        i++;
      }
    }
    _item = NULL;
  }
}

// ====================== Map ======================

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map()
: _length(0), _buckets(), _marks() {
  _buckets._host = this;
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map(const Map& value)
: _length(0), _buckets(), _marks() {
  _buckets._host = this;
  operator=(value);
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map(Map&& value)
: _length(value._length)
, _buckets(move(value._buckets))
, _marks(move(value._marks))
{
  value._length = 0;
  _buckets._host = this;
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>::~Map() {
  clear();
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>& Map<Key, Value, Compare>::operator=(const Map& value) {
  clear();
  
  if (value._length) {
    _buckets.realloc(value._buckets.capacity(), true);
    _length = value._length;
    
    const Bucket* i = *value._buckets;
    const Bucket* end = i + value._buckets.capacity();
    Bucket* buk = *_buckets;

    while ( i < end ) {
      if ( i->_first ) {
        Item* item = i->_first;
        Item* s_item = buk->_first = new Item(*item); // 复制
        s_item->_next = NULL;
        buk->_last = s_item;
        item = item->_next;
        
        while (item) {
          Item* i_item = new Item(*item);
          s_item->_next = i_item;
          i_item->_prev = s_item;
          i_item->_next = NULL;
          buk->_last = i_item;
          //
          s_item = i_item;
          item = item->_next;
        }
      }
      buk++; i++;
    }
  }

  return *this;
}

template<class Key, class Value, class Compare>
Map<Key, Value, Compare>& Map<Key, Value, Compare>::operator=(Map&& value) {
  clear();
  _length = value._length;
  _buckets = avocado::move(value._buckets);
  _buckets._host = this;
  _marks = move(value._marks);
  value._length = 0;
  return *this;
}

template<class Key, class Value, class Compare>
const Value& Map<Key, Value, Compare>::operator[](const Key& key) const {
  return find(key).value();
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::operator[](const Key& key) {
  bool is = false;
  Item* item = _find_set(key, &is);
  if (is) { // 新的
    new(&item->_key) Key(key);
    new(&item->_value) Value();
  }
  return item->_value;
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::operator[](Key&& key) {
  bool is = false;
  Item* item = _find_set(key, &is);
  if (is) { // 新的
    new(&item->_key) Key(move(key));
    new(&item->_value) Value();
  }
  return item->_value;
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Item* Map<Key, Value, Compare>::_find(const Key& key) {
  
  if (_length) {
    
    uint hash = Compare::hash(key);
    Item* item = ( *_buckets + (hash % _buckets.capacity()) )->_first;
    
    while (item) {
      // 用equals做比较,冲突越多(链表长度)会越慢.相同才返回
      if ( Compare::equals(item->_key, key, item->_hash, hash) ) {
        return item;
      } else { // 不相同继续比较
        item = item->_next;
      }
    }
  }
  return nullptr;
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Item*
Map<Key, Value, Compare>::_find_set(const Key& key, bool* is_new) {
  uint hash = Compare::hash(key); // hash code
  
  if (_length) {
    uint index = hash % _buckets.capacity();
    Bucket* buk = *_buckets + index;
    Item* item = buk->_first;
    
    while (item) {
      // 用equals做比较,冲突越多(链表长度)会越慢.相同才返回
      if (Compare::equals(item->_key, key, item->_hash, hash)) {
        return item;
      } else { // 不相同继续比较
        item = item->_next;
      }
    }
  }
  
  // 添加一个新的Item
  _length++;
  _buckets.auto_realloc();  // 自动调整容量
  
  uint index = hash % _buckets.capacity();
  Bucket* buk = *_buckets + index;
  Item* item = static_cast<Item*>(::malloc(sizeof(Item)));
  
  if (buk->_first) { // 有冲突,插入到链表最开始
    buk->_first->_prev = item;
    item->_next = buk->_first;
    buk->_first = item;
  } else {
    item->_next = NULL;
    buk->_first = item;
    buk->_last = item;
  }
  
  item->_prev = NULL;
  item->_hash = hash;
  item->_mark = false;
  *is_new = true;
  
  return item;
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::ConstIterator
Map<Key, Value, Compare>::find(const Key& key) const {
  Item* item = const_cast<Map*>(this)->_find(key);
  return ConstIterator(iterator(const_cast<Map*>(this), item));
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::find(const Key& key) {
  return Iterator(iterator(this, _find(key)));
}

template<class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::has(const Key& key) const {
  return const_cast<Map*>(this)->_find(key) != NULL;
}

template<class Key, class Value, class Compare>
Array<Key> Map<Key, Value, Compare>::keys() const {
  Array<Key> rev;
  for (auto i = begin(), e = end(); i != e; i++)
    rev.push(i.data().key());
  return rev;
}

template<class Key, class Value, class Compare>
Array<Value> Map<Key, Value, Compare>::values() const {
  Array<Value> rev;
  for (auto i = begin(), e = end(); i != e; i++)
    rev.push(i.value());
  return rev;
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::set(const Key& key, const Value& value) {
  bool is = false;
  Item* item = _find_set(key, &is);
  if (is) { // 新的
    new(&item->_key) Key(key);
    new(&item->_value) Value(value);
  } else {
    item->_value = value;
  }
  return item->_value;
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::set(Key&& key, Value&& value) {
  bool is = false;
  Item* item = _find_set(key, &is);
  if (is) { // 新的
    new(&item->_key) Key(move(key));
    new(&item->_value) Value(move(value));
  } else {
    item->_value = move(value);
  }
  return item->_value;
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::set(Key&& key, const Value& value) {
  bool is = false;
  Item* item = _find_set(key, &is);
  if (is) { // 新的
    new(&item->_key) Key(move(key));
    new(&item->_value) Value(value);
  } else {
    item->_value = value;
  }
  return item->_value;
}

template<class Key, class Value, class Compare>
Value& Map<Key, Value, Compare>::set(const Key& key, Value&& value) {
  bool is = false;
  Item* item = _find_set(key, &is);
  if (is) { // 新的
    new(&item->_key) Key(key);
    new(&item->_value) Value(move(value));
  } else {
    item->_value = move(value);
  }
  return item->_value;
}

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::_del(Item* item) {
  Bucket* buk = *_buckets + (item->_hash % _buckets.capacity());
  
  av_assert(!item->_mark); // 有标记的不能删除
  
  if (item->_prev || item->_next) {
    Item* prev = item->_prev;
    Item* next = item->_next;
    
    if (prev) {
      prev->_next = next;
    } else {
      buk->_first = next;
    }
    if (next) {
      next->_prev = prev;
    } else {
      buk->_last = prev;
    }
  } else { // 清空一个存储桶
    buk->_first = NULL;
    buk->_last = NULL;
  }
  
  _length--;
  if (_length) { // 自动调整存储桶长度
    _buckets.auto_realloc();
  } else { // 已不需要任何存储桶
    _buckets.free();
  }
  
  delete item;
}

template<class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::del(const Key& key) {
  Item* item = _find(key);
  if (item) {
    _del(item); return 1;
  }
   return 0;
}

template<class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::del(ConstIterator it) {
  Item* item = it.data()._item;
  if (item) {
    _del(item); return 1;
  }
   return 0;
}

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::clear() {
  if (_length) {
    Bucket* i = *_buckets;
    Bucket* end = i + _buckets.capacity();
    
    while (i < end) {
      Item* item = i->_first;
      while (item) {
        Item* tmp = item;
        item = item->_next;
        delete tmp;
      }
      i++;
    }
    _length = 0;
    _buckets.free(); // 释放全部存储桶
    _marks.clear();
  }
}

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::mark(const Key& key) {
  Item* item = _find(key);
  if (item && !item->_mark) {
    item->_mark = true;
    _marks.push(item);
  }
}

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::mark(ConstIterator it) {
  const iterator& data = it.data();
  Item* item = data._item;
  if (item && !item->_mark) {
    av_assert(data._host == this);
    item->_mark = true;
    _marks.push(item);
  }
}

template<class Key, class Value, class Compare>
void Map<Key, Value, Compare>::del_mark() {
  auto i = _marks.begin();
  auto end = _marks.end();
  while (i != end) {
    (*i)->_mark = false;
    _del(*i);
    i++;
  }
  _marks.clear();
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::ConstIterator Map<Key, Value, Compare>::begin() const {
  iterator it(const_cast<Map*>(this), NULL); it.begen();
  return ConstIterator(it);
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::ConstIterator Map<Key, Value, Compare>::end() const {
  return ConstIterator(iterator(const_cast<Map*>(this), NULL));
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::begin() {
  iterator it(this, NULL); it.begen();
  return Iterator(it);
}

template<class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::end() {
  return Iterator(iterator(this, NULL));
}

template<class Key, class Value, class Compare>
uint Map<Key, Value, Compare>::length() const {
  return _length;
}

av_end
