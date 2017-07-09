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

#ifndef __avocado__string_list__
#define __avocado__string_list__

#include "list.h"
#include "string.h"
#include "buffer.h"

av_ns(avocado)

template <class Char, class Container> class BasicStringBuilder;
typedef BasicStringBuilder<char, Container<char>> StringBuilder;
typedef BasicStringBuilder<uint16, Container<uint16>> Ucs2StringBuilder;

/**
 * @class BasicLongString # 字符串链表
 */
template <class Char, class Container>
class BasicStringBuilder: public List<BasicString<Char, Container>> {
public:
  typedef BasicString<Char, Container> Item;
  typedef typename List<Item>::Iterator Iterator;
  typedef typename List<Item>::ConstIterator ConstIterator;
  
  BasicStringBuilder();
  BasicStringBuilder(const BasicStringBuilder&);
  BasicStringBuilder(BasicStringBuilder&&);
  
  virtual ~BasicStringBuilder();
  
  /**
   * @func operator=
   */
  BasicStringBuilder& operator=(const BasicStringBuilder&);
  
  /**
   * @func operator=
   */
  BasicStringBuilder& operator=(BasicStringBuilder&&);
  
  /**
   * @func push
   */
  void push(const Item& item);
  
  /**
   * func push
   */
  void push(Item&& item);
  
  /**
   * func push
   */
  void push(const BasicStringBuilder& ls);
  
  /**
   * func push
   */
  void push(BasicStringBuilder&& ls);
  
  /**
   * @func unshift
   */
  void unshift(const Item& item);
  
  /**
   * @func unshift
   */
  void unshift(Item&& item);
  
  /**
   * func unshift
   */
  void unshift(const BasicStringBuilder& ls);
  
  /**
   * func unshift
   */
  void unshift(BasicStringBuilder&& ls);
  
  /**
   * @func pop
   */
  void pop();
  
  /**
   * @func shift
   */
  void shift();
  
  /**
   * @func insert
   */
  void insert(ConstIterator it, const Item& item);
  
  /**
   * @func insert
   */
  void insert(ConstIterator it, Item&& item);
  
  /**
   * @func del
   */
  void del(ConstIterator it);
  
  /**
   * @func clear
   */
  void clear();
  
  /**
   * @func join
   */
  String join(cString& sp) const;
  
  /**
   * @overwrite
   */
  virtual String to_string() const;
  
  /**
   * @func to_basic_string
   */
  BasicString<Char, Container> to_basic_string() const;
  
  /**
   * @func to_buffer
   */
  ArrayBuffer<Char> to_buffer() const;
  
  /**
   * @func string_length
   */
  uint string_length() const;
  
private:
  
  uint m_string_length;
};

av_end

#include "string-builder.h.inl"

#endif
