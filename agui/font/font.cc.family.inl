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

class FontFamily::Inl: public FontFamily {
  public:
#define _inl_family(self) static_cast<FontFamily::Inl*>(self)
  
  /**
   * @func set_font
   */
  void set_font(Font* font) {
    
    m_all_fonts.push(font);
    
    // LOG("%d,%d,%s,%s", font->style_flags(),
    //       m_blend_fonts.length(), *font->family()->name(), *font->name());
    
    if (m_all_fonts.length() == 1) {
      m_fonts[0] = m_fonts[1] = m_fonts[2] = m_fonts[3] = font; return;
    }
    
    uint style_flags = font->style_flags();
    
    if ( (m_fonts[0]->style_flags() != FSF_LIGHT && style_flags == FSF_LIGHT) ||
        (!(m_fonts[0]->style_flags() & FSF_LIGHT) && (style_flags & FSF_LIGHT))
    ) {
      m_fonts[0] = font;
    }
    if ( (m_fonts[1]->style_flags() != FSF_REGULAR && style_flags == FSF_REGULAR) ||
        (!(m_fonts[1]->style_flags() & FSF_REGULAR) && (style_flags & FSF_REGULAR))
    ) {
      m_fonts[1] = font;
    }
    if ( (m_fonts[2]->style_flags() != FSF_BOLD && style_flags == FSF_BOLD) ||
        (!(m_fonts[2]->style_flags() & FSF_BOLD) && (style_flags & FSF_BOLD))
    ) {
      m_fonts[2] = font;
    }
    
    m_fonts[3] = m_fonts[1];
  }
};

/**
 * @constructor
 */
FontFamily::FontFamily(cString& family_name)
: m_family_name(family_name)
, m_fonts()
{
  memset(m_fonts, 0, sizeof(m_fonts));
}

/**
 * @func font_names
 */
Array<String> FontFamily::font_names() const {
  Array<String> rev;
  
  for (auto i = m_all_fonts.begin(),
            e = m_all_fonts.end(); i != e; i++) {
    rev.push(i.value()->font_name());
  }
  return rev;
}

/**
 * @overwrite
 */
Font* FontFamily::font(TextStyle style) {
  switch (style) {
    case TextStyle::LIGHT: return m_fonts[0];
    case TextStyle::REGULAR: return m_fonts[1];
    case TextStyle::BOLD: return m_fonts[2];
    default: return m_fonts[3];
  }
}
