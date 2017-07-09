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

#include "string.h"

av_ns(avocado)

template<class Item> List<Item>::iterator::iterator(): _host(nullptr), _item(nullptr) {
}

template<class Item>
List<Item>::iterator::iterator(List* host, ItemWrap* item): _host(host), _item(item) {
}

template<class Item> bool List<Item>::iterator::equals(const iterator& it) const {
  return _item == it._item;
}

template<class Item> bool List<Item>::iterator::is_null() const {
  return _item == nullptr;
}

template<class Item> const Item& List<Item>::iterator::value() const {
  av_assert(_item);
  return _item->_item;
}

template<class Item> Item& List<Item>::iterator::value() {
  av_assert(_item);
  return _item->_item;
}

template<class Item> void List<Item>::iterator::prev() {
  if (_item) {
    if (_item != _host->_first) {
      _item = _item->_prev;
    }
  } else {
    if (_host) {
      _item = _host->_last;
    }
  }
}

template<class Item> void List<Item>::iterator::next() {
  if (_item) {
    _item = _item->_next;
  }
}

// List

template<class Item>
List<Item>::List() : _first(nullptr), _last(nullptr), _length(0)
{ }

template<class Item>
List<Item>::List(const List& list): _first(nullptr), _last(nullptr), _length(0) {
  auto i = list.begin();
  auto end = list.end();
  while (i != end) {
    push(*i);
    i++;
  }
}

template<class Item>
List<Item>::List(List&& list)
: _first(list._first), _last(list._last), _length(list._length)
{
  list._first = nullptr;
  list._last = nullptr;
  list._length = 0;
}

template<class Item> List<Item>::~List() {
  clear();
}

template<class Item> List<Item>& List<Item>::operator=(const List& list) {
  clear();
  auto i = list.begin();
  auto end = list.end();
  while (i != end) {
    push(*i);
    i++;
  }
  return *this;
}

template<class Item> List<Item>& List<Item>::operator=(List&& list) {
  clear();
  _first = list._first;
  _last = list._last;
  _length = list._length;
  list._first = nullptr;
  list._last = nullptr;
  list._length = 0;
  return *this;
}

#define list_unshift_item_(item) \
ItemWrap* w = item; \
if (_first) { \
  _first->_prev = w; \
  _first = w; \
} else { \
  _first = w; \
  _last = w; \
} \
_length++; \
return Iterator(iterator(this, w))

template<class Item> typename List<Item>::Iterator List<Item>::unshift(const Item& item) {
  list_unshift_item_( new ItemWrap({ item, nullptr, _first }) );
}

template<class Item> typename List<Item>::Iterator List<Item>::unshift(Item&& item) {
  list_unshift_item_( new ItemWrap({ move(item), nullptr, _first }) );
}

template<class Item> void List<Item>::unshift(const List& ls) {
  for ( auto& i : ls ) unshift( i.value() );
}

template<class Item> void List<Item>::unshift(List&& ls) {
  if (ls._first) {
    if ( _first ) {
      _first->_prev = ls._last;
      ls._last->_next = _first;
      _first = ls._first;
      _length += ls._length;
    } else {
      _first = ls._first;
      _last = ls._last;
      _length = ls._length;
    }
    ls._first = nullptr;
    ls._last = nullptr;
    ls._length = 0;
  }
}

#define list_push_item_(item) \
ItemWrap* w = item; \
  if (_last) { \
  _last->_next = w; \
  _last = w; \
} else { \
  _first = _last = w; \
} \
_length++; \
return Iterator(iterator(this, w))

template<class Item> typename List<Item>::Iterator List<Item>::push(const Item& item) {
  list_push_item_( new ItemWrap({ item, _last, nullptr }) );
}

template<class Item> typename List<Item>::Iterator List<Item>::push(Item&& item) {
  list_push_item_( new ItemWrap({ move(item), _last, nullptr }) );
}

template<class Item> void List<Item>::push(const List& ls) {
  for ( auto& i : ls ) push( i.value() );
}

template<class Item> void List<Item>::push(List&& ls) {
  if (ls._first) {
    if ( _first ) {
      _last->_next = ls._first;
      ls._first->_prev = _last;
      _last = ls._last;
      _length += ls._length;
    } else {
      _first = ls._first;
      _last = ls._last;
      _length = ls._length;
    }
    ls._first = nullptr;
    ls._last = nullptr;
    ls._length = 0;
  }
}

template<class Item> void List<Item>::pop() {
  if (_last) {
    ItemWrap* w = _last;
    if ( _last == _first ) {
      _last = nullptr;
      _first = nullptr;
    } else {
      _last = _last->_prev;
      if (_last) {
        _last->_next = nullptr;
      }
    }
    _length--;
    delete w;
  }
}

template<class Item> void List<Item>::shift() {
  if (_first) {
    ItemWrap* w = _first;
    if ( _first == _last ) {
      _first = nullptr;
      _last = nullptr;
    } else {
      _first = _first->_next;
      if (_first) {
        _first->_prev = nullptr;
      }
    }
    _length--;
    delete w;
  }
}

template<class Item> void List<Item>::pop(uint count) {
  for ( int i = 0; i < count && _first; i++ ) pop();
}

template<class Item> void List<Item>::shift(uint count) {
  for ( int i = 0; i < count && _first; i++ ) shift();
}

#define list_before_item_(item)\
if ((*it)._item->_prev) {\
  ItemWrap* w = item;\
  (*it)._item->_prev = w;\
  w->_prev->_next = w;\
  _length++;\
  return Iterator(iterator(this, w));\
} else {\
  list_unshift_item_( item );\
}

template<class Item>
typename List<Item>::Iterator List<Item>::before(ConstIterator it, const Item& item) {
  list_before_item_( new ItemWrap({ item, (*it)._item->_prev, (*it)._item }) );
}

template<class Item>
typename List<Item>::Iterator List<Item>::before(ConstIterator it, Item&& item) {
  list_before_item_( new ItemWrap({ move(item), (*it)._item->_prev, (*it)._item }) );
}

#define list_after_item_(item)\
if ((*it)._item->_next) {\
  ItemWrap* w = item;\
  (*it)._item->_next = w;\
  w->_next->_prev = w;\
  _length++;\
  return Iterator(iterator(this, w));\
} else {\
  list_push_item_( item );\
}

template<class Item>
typename List<Item>::Iterator List<Item>::after(ConstIterator it, const Item& item) {
  list_after_item_( new ItemWrap({ item, (*it)._item, (*it)._item->_next }) );
}

template<class Item>
typename List<Item>::Iterator List<Item>::after(ConstIterator it, Item&& item) {
  list_after_item_( new ItemWrap({ move(item), (*it)._item, (*it)._item->_next }) );
}

#undef list_push_item_
#undef list_unshift_item_
#undef list_before_item_
#undef list_after_item_

template<class Item> void List<Item>::del(ConstIterator it) {
  ItemWrap* item = it.data()._item;
  if (item) {
    av_assert( it.data()._host == this);
    ItemWrap* prev = item->_prev;
    ItemWrap* next = item->_next;
    
    if (prev) {
      prev->_next = next;
    } else {
      _first = next;
    }
    if (next) {
      next->_prev = prev;
    } else {
      _last = prev;
    }
    
    _length--;
    delete item;
  }
}

template<class Item> void List<Item>::clear() {
  ItemWrap* item = _first;
  while (item) {
    ItemWrap* tmp = item;
    item = item->_next;
    delete tmp;
  }
  _length = 0;
  _first = _last = nullptr;
}

#define list_find_iterator0_(offset)  \
if (offset < _length) { \
  if (offset <= _length / 2.0) {  \
    return find(begin(), offset); \
  } else {  \
    return find(end(), offset - _length); \
  } \
} \
return end()

template<class Item>
typename List<Item>::ConstIterator List<Item>::find(uint offset) const {
  list_find_iterator0_(offset);
}

template<class Item>
typename List<Item>::Iterator List<Item>::find(uint offset) {
  list_find_iterator0_(offset);
}

#define list_find_iterator_(it, offset)  \
ItemWrap* item = (*it)._item; \
if (offset > 0) { \
  while (offset) {  \
    if (item) { \
      item = item->_next; \
      offset--; \
    } else {  \
      break;  \
    } \
  } \
} else if (offset < 0) {  \
  if (!item) {  \
    item = _last; \
    offset++; \
  } \
  while (offset) {  \
    if (item) { \
      item = item->_prev; \
      offset++; \
    } else {  \
      break;  \
    } \
  } \
}

template<class Item>
typename List<Item>::ConstIterator List<Item>::find(ConstIterator it, int offset) const {
  list_find_iterator_(it, offset);
  return ConstIterator(iterator(this, item));
}

template<class Item>
typename List<Item>::Iterator List<Item>::find(ConstIterator it, int offset) {
  list_find_iterator_(it, offset);
  return Iterator(iterator(this, item));
}

#undef _list_find_iterator0
#undef _list_find_iterator

// List join

template<class Item> String List<Item>::join(cString& sp) const {
  String rev;
  ItemWrap* item = _first;
  
  if (item) {
    rev.push(String(item->_item));
    item = item->_next;
  }
  
  while (item) {
    rev.push(sp);
    rev.push(String(item->_item));
    item = item->_next;
  }
  return rev;
}

template<class Item> const Item& List<Item>::first()const {
  return _first->_item;
}

template<class Item> const Item& List<Item>::last()const {
  return _last->_item;
}

template<class Item> Item& List<Item>::first() {
  return _first->_item;
}

template<class Item> Item& List<Item>::last() {
  return _last->_item;
}

template<class Item>
typename List<Item>::ConstIterator List<Item>::begin() const {
  return ConstIterator(iterator(const_cast<List*>(this), _first));
}

template<class Item>
typename List<Item>::ConstIterator List<Item>::end() const {
  return ConstIterator(iterator(const_cast<List*>(this), NULL));
}

template<class Item>
typename List<Item>::Iterator List<Item>::begin() {
  return Iterator(iterator(this, _first));
}

template<class Item>
typename List<Item>::Iterator List<Item>::end() {
  return Iterator(iterator(this, NULL));
}

template<class Item>
uint List<Item>::length() const {
  return _length;
}

av_end
