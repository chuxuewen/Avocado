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

#ifndef __avocado__gui__text_font__
#define __avocado__gui__text_font__

#include "value.h"
#include "font.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

class TextRows;
class View;
class Layout;

/**
 * @class TextFont
 */
class TextFont {
public:
  typedef ProtocolCharacteristic Characteristic;
  typedef FontGlyph::TexureLevel TexureLevel;
  
  TextFont();
  
  struct Cell {
    uint    line_num;       // 行号
    float   baseline;       // 基线
    float   offset_start;   // 偏移开始
    uint    begin;          // 在字符中有开始索引
    Array<float>  offset;   // 偏移表
    Array<uint16> chars;    // 字符表
    bool    reverse;        // 反向排列
  };
  
  struct Data {
    Data();
    Array<Cell> cells;
    Ucs2String  string;
    TexureLevel texture_level;        // 文本的纹理等级
    float       texture_scale;        // 文本的纹理对应等级与实际文本的缩放比
    float       text_ascender;        // 基线距离行顶
    float       text_descender;       // 基线距离行底
    float       text_hori_bearing;    // 基线距离文本顶部
    float       text_height;          // 文本高度
    uint        cell_draw_begin;      // 需要绘制cell开始
    uint        cell_draw_end;        // 需要绘制cell结束
  };
  
  // get attrs
  inline ColorValue text_background_color() const { return m_text_background_color; }
  inline ColorValue text_color() const { return m_text_color; }
  inline TextSizeValue text_size() const { return m_text_size; }
  inline TextStyleValue text_style() const { return m_text_style; }
  inline TextFamilyValue text_family() const { return m_text_family; }
  inline TextShadowValue text_shadow() const { return m_text_shadow; }
  inline TextLineHeightValue text_line_height() const { return m_text_line_height; }
  inline TextDecorationValue text_decoration() const { return m_text_decoration; }
  // set attrs
  void set_text_background_color(ColorValue value);
  void set_text_color(ColorValue value);
  void set_text_size(TextSizeValue value);
  void set_text_style(TextStyleValue value);
  void set_text_family(TextFamilyValue value);
  void set_text_shadow(TextShadowValue value);
  void set_text_line_height(TextLineHeightValue value);
  void set_text_decoration(TextDecorationValue value);
  
  /**
   * @func view
   */
  virtual View* view() = 0;
  
  /**
   * @func get_font_glyph_table_and_height
   * @arg data {Data} 返回当前字体最大文本顶部与基线的距离,返回前字体最大文本高度。。
   * @arg line_height {TextLineHeight}
   * @ret {FontGlyphTable*} 返回字型表
   */
  FontGlyphTable* get_font_glyph_table_and_height(Data& data, TextLineHeight line_height);
  
  /**
   * @func get_font_glyph_table
   */
  inline FontGlyphTable* get_font_glyph_table_and_height(Data& data) {
    return get_font_glyph_table_and_height(data, m_text_line_height.value);
  }
  
  /**
   * @func simple_layout_width
   */
  float simple_layout_width(cString& text);
  
  /**
   * @func simple_layout_width
   */
  float simple_layout_width(cUcs2String& text);
  
protected:
  
  /**
   * @func set_glyph_texture_level
   */
  void set_glyph_texture_level(Data& data);
  
  /**
   * @func compute_text_visible_draw
   * @arg internal_only 只处理内部顶点内部的cell外部有都不设置为隐藏
   */
  bool compute_text_visible_draw(Vec2 vertex[4],
                                 Data& data,
                                 float x1, float x2, float offset_y);
  /**
   * @func mark_text
   */
  virtual void mark_text(uint value) = 0;
  
protected:
  
  ColorValue      m_text_background_color;  // 文本背景颜色
  ColorValue      m_text_color;             // 字体颜色
  TextSizeValue   m_text_size;              // 字体尺寸
  TextStyleValue  m_text_style;             // 字体样式
  TextFamilyValue m_text_family;            // 字体名称
  TextLineHeightValue  m_text_line_height;  // 行高
  TextShadowValue      m_text_shadow;       // 文本阴影
  TextDecorationValue  m_text_decoration;   // 文本装饰
  
  friend class GLDraw;
};

/**
 * @class TextLayout
 */
class TextLayout: public TextFont {
public:
  
  TextLayout();
  
  inline TextOverflowValue text_overflow() const { return m_text_overflow; }
  inline TextWhiteSpaceValue text_white_space() const { return m_text_white_space; }
  
  void set_text_overflow(TextOverflowValue value);
  void set_text_white_space(TextWhiteSpaceValue value);
  
  struct Options {
    struct SpaceWrap {
      bool  auto_wrap;        // 使用自动wrap(自动换行)
      bool  merge_space;      // 合并空白序列
      bool  merge_line_feed;  // 合并换行符
    } space_wrap;
    TextOverflow overflow;
    TextLineHeight text_line_height; // 行高
  };
  
  /**
   * @func get_options
   */
  Options get_options(TextLayout* text = nullptr);
  
  /**
   * @func is_auto_wrap
   */
  static bool is_auto_wrap(TextLayout* text);
  
protected:
  
  /**
   * @func set_text_layout_offset
   */
  void set_text_layout_offset(TextRows* rows, Vec2 limit,
                              Data& data, cUcs2String& string,
                              uint begin, uint end,
                              Options* opts = nullptr, bool ignore_empty_cell = true);
  
  /**
   * @func set_text_layout_offset
   */
  inline void set_text_layout_offset(TextRows* rows, Vec2 limit, Data& data,
                                     Options* opts = nullptr, bool ignore_empty_cell = true) {
    set_text_layout_offset(rows, limit, data, data.string,
                           0, data.string.length(), opts, ignore_empty_cell);
  }
  
  /**
   * @func set_text_layout_offset
   */
  void set_text_layout_offset(TextRows* rows, Vec2 limit,
                              Data& data, uint16 security, uint count, Options* opts = nullptr);
  
  /**
   * @func mark_text
   */
  virtual void mark_text(uint value);
  
  /**
   * @func solve_text_layout_mark
   */
  void solve_text_layout_mark();
  
protected:
  
  TextOverflowValue    m_text_overflow;     // 文本溢出
  TextWhiteSpaceValue  m_text_white_space;  // 空白处理
  
  av_def_inl_cls(Inl);
  
};

av_gui_end
#endif
