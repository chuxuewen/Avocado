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

#ifndef __avocado__gui__text__
#define __avocado__gui__text__

#include "hybrid.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

/**
 * @class Text
 */
class Text: public Hybrid {
public:
  av_def_gui_view(TEXT, Text, text);
  
  /**
   * @overwrite
   */
  virtual void  prepend(View* child) av_def_err;
  virtual void  append(View* child) av_def_err;
  virtual View* append_text(cUcs2String& str) av_def_err;
  virtual void remove_all_child();
  
  /**
   * @get value
   */
  inline Ucs2String value() const { return m_data.string; }
  
  /**
   * @set value
   */
  virtual void set_value(cUcs2String& str);
  
  /**
   * @get length
   */
  inline uint length() const { return m_data.string.length(); }
  
  /**
   * @get cells
   */
  inline const Array<Cell>& cells() const { return m_data.cells; }
  
  /**
   * @func text_hori_bearing
   */
  inline float text_hori_bearing() const { return m_data.text_hori_bearing; }
  
  /**
   * @func text_height
   */
  inline float text_height() const { return m_data.text_height; }

protected:
  
  /**
   * @func set_text_align_offset
   */
  void set_text_align_offset(float text_margin = 0);
  
  /**
   * @func set_visible_draw
   */
  virtual void set_visible_draw();

  /**
   * @overwrite
   */
  virtual void draw();
  virtual void accept_text(Ucs2StringBuilder& output) const;
  virtual void set_layout_content_offset();
  
  Data  m_data;
  
  av_def_inl_cls(Inl);
  friend class Hybrid;
};

av_gui_end
#endif
