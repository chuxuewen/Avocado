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

#ifndef __avocado__list__
#define __avocado__list__

#include "util.h"
#include "iterator.h"

av_ns(avocado)

/**
 * @class List 双向链表
 */
template<class Item> class List: public Object {
private:
  
  struct ItemWrap {
    Item _item;
    ItemWrap* _prev;
    ItemWrap* _next;
  };
  
  struct iterator {
  public:
    typedef Item Value;
    const Item& value() const;
    Item& value();
  private:
    iterator();
    iterator(List*, ItemWrap*);
    bool equals(const iterator& it) const;
    bool is_null() const;
    void prev();
    void next();
    List* _host;
    ItemWrap* _item;
    friend class List;
    friend class ConstIteratorTemplate<iterator>;
    friend class IteratorTemplate<iterator>;
  };
  
public:
  
  typedef ConstIteratorTemplate<iterator> ConstIterator;
  typedef IteratorTemplate<iterator> Iterator;
  
  List();
  List(const List&);
  List(List&&);
  
  virtual ~List();
  
  /**
   * @func operator=
   */
  List& operator=(const List&);
  
  /**
   * @func operator=
   */
  List& operator=(List&&);
  
  /**
   * @func push
   */
  Iterator push(const Item& item);
  
  /**
   * func push
   */
  Iterator push(Item&& item);
  
  /**
   * func push
   */
  void push(const List& ls);
  
  /**
   * func push
   */
  void push(List&& ls);
  
  /**
   * @func unshift
   */
  Iterator unshift(const Item& item);
  
  /**
   * @func unshift
   */
  Iterator unshift(Item&& item);
  
  /**
   * func unshift
   */
  void unshift(const List& ls);
  
  /**
   * func unshift
   */
  void unshift(List&& ls);

  /**
   * @func pop
   */
  void pop();
  
  /**
   * @func shift
   */
  void shift();
  
  /**
   * @func pop
   */
  void pop(uint count);
  
  /**
   * @func shift
   */
  void shift(uint count);
  
  /**
   * @func before insert
   */
  Iterator before(ConstIterator it, const Item& item);
  
  /**
   * @func before insert
   */
  Iterator before(ConstIterator it, Item&& item);
  
  /**
   * @func after insert
   */
  Iterator after(ConstIterator it, const Item& item);
  
  /**
   * @func after insert
   */
  Iterator after(ConstIterator it, Item&& item);
  
  /**
   * @func del
   */
  void del(ConstIterator it);
  
  /**
   * @func clear
   */
  void clear();
  
  /**
   * @func first
   */
  const Item& first()const;
  
  /**
   * @func last
   */
  const Item& last()const;
  
  /**
   * @func first
   */
  Item& first();
  
  /**
   * @func last
   */
  Item& last();
  
  /**
   * @func find
   */
  ConstIterator find(uint offset) const;
  
  /**
   * @func find
   */
  Iterator find(uint offset);
  
  /**
   * @func find
   */
  ConstIterator find(ConstIterator it, int offset = 0) const;
  
  /**
   * @func find
   */
  Iterator find(ConstIterator it, int offset = 0);
  
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
  
private:
  
  ItemWrap* _first;
  ItemWrap* _last;
  uint      _length;
};

av_end

#include "list.h.inl"

#endif
