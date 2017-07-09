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

#include "autil/json.h"
#include "autil/sys.h"
#include "autil/buffer.h"
#include "font-1.h"
#include "../bezier.h"
#include "../texture.h"
#include "../display-port.h"
#include "../draw.h"
#include "../app.h"

av_gui_begin

#include <font-native.cc> // 此文件由编译脚本自动生成
#include "font.cc.1.inl"
#include "font.cc.2.inl"
#include "font.cc.3.inl"
#include "font.cc.font.inl"
#include "font.cc.glyph.inl"
#include "font.cc.family.inl"

/**
 * @class FontPool::Inl
 */
class FontPool::Inl: public FontPool {
public:
#define _inl_pool(self) static_cast<FontPool::Inl*>(self)
  
  /**
   * @fucn m_add_font
   */
  void register_font(cString& family_name,
                     cString& font_name,
                     uint style_flags,
                     uint num_glyphs,
                     uint face_index,
                     int  height,       /* text height in 26.6 frac. pixels       */
                     int  max_advance,  /* max horizontal advance, in 26.6 pixels */
                     int  ascender,     /* ascender in 26.6 frac. pixels          */
                     int  descender,    /* descender in 26.6 frac. pixels         */
                     int  underline_position,
                     int  underline_thickness,
                     cString& path,
                     cBuffer& buff) {
    
    if ( m_fonts.has(font_name) ) { // 这个字体已经存在
      return;
    }
    
    if ( ! path.is_empty() ) { // 文件
      
      if ( ! FileHelper::is_file_sync(path) )
        return;
      m_paths[path] = true;
      
    } else if ( buff.is_null() ) {
      return;
    }
    
    FontFamily* family = nullptr;
    
    auto i = m_familys.find(family_name);
    
    if ( i != m_familys.end() ) {
      family = i.value();
    } else {
      family = new FontFamily(family_name);
      m_familys[family_name] = family;
      m_blend_fonts[family_name] = family; // 替换别名
    }
    
    Font* font = nullptr;
    
    if ( path.is_empty() ) {
      font = new FontFromData(buff);
    } else {
      font = new FontFromFile(path);
    }
    
    _inl_font(font)->initializ(this, family,
                               font_name, style_flags, num_glyphs, face_index,
                               height, max_advance, ascender, descender,
                               underline_position, underline_thickness, (FT_Library)m_ft_lib);
    
    m_fonts[font_name] = font;
    
    if ( ! m_blend_fonts.has(font_name) ) {
      m_blend_fonts[font_name] = font;
    }
    
    _inl_family(family)->set_font(font);
  }
  
  /**
   * @func display_port_change_handle
   */
  void display_port_change_handle(Event<>& evt) {
    
    float scale = av_max(m_display_port->scale_width(), m_display_port->scale_height());
    
    if ( scale != m_display_port_scale ) {
      
      if ( m_display_port_scale != 0.0 ) { // 缩放改变影响字型纹理,所有全部清理
        clear(true);
      }
      m_display_port_scale = scale;
      
      m_draw_ctx->refresh_status_for_font_pool(this);
      
      Vec2 size = m_display_port->size();
      uint font_size = sqrtf(size.width() * size.height()) / 10;
      
      // 最大纹理字体不能超过上下文支持的大小
      if (font_size >= draw_ctx()->support_max_texture_font_size()) {
        m_max_glyph_texture_size = draw_ctx()->support_max_texture_font_size();
      } else {
        m_max_glyph_texture_size = font_glyph_texture_levels_idx[font_size].max_font_size;
      }
    }
  }
  
};

/**
 * @constructor
 */
FontPool::FontPool(Draw* ctx)
: m_ft_lib(nullptr)
, m_draw_ctx(ctx)
, m_display_port(nullptr)
, m_max_glyph_texture_size(0)
, m_display_port_scale(0)
{
  av_assert(m_draw_ctx);
  
  FT_Init_FreeType((FT_Library*)&m_ft_lib);
    
  { // 载入内置字体
    uint count = sizeof(native_fonts_) / sizeof(Native_font_data_);
    
    for (int i = 0 ; i < count; i++) {
      WeakBuffer data((cchar*)native_fonts_[i].data, native_fonts_[i].count);
      register_font(data, i == 1 ? "icon" : String::empty);
    }
    
    if ( m_familys.has("DejaVu Serif") ) {
      // 这个内置字体必须载入成功,否则退出程序
      // 把载入的一个内置字体做为默认备用字体,当没有任何字体可用时候,使用这个内置字体
      m_spare_family = m_familys["DejaVu Serif"];
    } else {
      av_fatal("Unable to initialize font");
    }
  }
  
  {
    // 载入系统字体
    const Array<SimpleFontFamily>& arr = get_sys_font_family();
    
    for (auto i = arr.begin(), e = arr.end(); i != e; i++) {
      
      const SimpleFontFamily& sffd = i.value();
      
      for (int i = 0; i < sffd.fonts.length(); i++) {
        const SimpleFont& sfd = sffd.fonts[i];
        
        _inl_pool(this)->register_font(sffd.family, sfd.name,
                                       sfd.style_flags, sfd.num_glyphs,
                                       i, sfd.height,
                                       sfd.max_advance, sfd.ascender,
                                       sfd.descender, sfd.underline_position,
                                       sfd.underline_thickness, sffd.path, Buffer());
        // end for
      }
    }
  }
  
  inl_set_default_fonts(this); // 设置默认字体列表
}

/**
 * @destructor
 */
FontPool::~FontPool() {
  
  for ( auto& i : m_familys ) {
    Release(i.value()); // delete
  }
  for ( auto& i : m_fonts ) {
    Release(i.value()); // delete
  }
  for ( auto& i : m_tables ) {
    Release(i.value()); // delete
  }
  
  m_familys.clear();
  m_fonts.clear();
  m_tables.clear();
  m_blend_fonts.clear();
  m_default_fonts.clear();
  
  FT_Done_FreeType((FT_Library)m_ft_lib); m_ft_lib = nullptr;
  
  if ( m_display_port ) {
    m_display_port->av_off(change, &Inl::display_port_change_handle, _inl_pool(this));
  }
}

/**
 * @func set_default_fonts # 尝试设置默认字体
 * @arg first {const Array<String>*}  # 第一字体列表
 * @arg ... {const Array<String>*} # 第2/3/4..字体列表
 */
void FontPool::set_default_fonts(const Array<String>* first, ...) {
  
  m_default_fonts.clear();
  Map<String, bool> has;
  
  auto end = m_blend_fonts.end();
  
  for (int i = 0; i < first->length(); i++) {
    auto j = m_blend_fonts.find(first->item(i));
    if (j != end) {
      has.set(j.value()->name(), true);
      m_default_fonts.push(j.value()); break;
    }
  }
  
  va_list arg;
  va_start(arg, first);
  
  const Array<String>* ls = va_arg(arg, const Array<String>*);
  
  while (ls) {
    for (int i = 0; i < ls->length(); i++) {
      auto j = m_blend_fonts.find(ls->item(i));
      if (j != end) {
        if ( ! has.has(j.value()->name()) ) {
          has.set(j.value()->name(), true);
          m_default_fonts.push(j.value());
        }
        break;
      }
    }
    ls = va_arg(arg, const Array<String>*);
  }
  
  va_end(arg);
  
  if ( /*! has.has(m_spare_family->name())*/ !m_default_fonts.length() ) {
    m_default_fonts.push(m_spare_family);
  }
}

/**
 * @func set_default_fonts # 在当前字体库找到字体名称,设置才能生效
 * @arg fonts {const Array<String>&} # 要设置的默认字体的名称
 */
void FontPool::set_default_fonts(const Array<String>& fonts) {
  
  m_default_fonts.clear();
  Map<String, bool> has;
  
  auto end = m_blend_fonts.end();
  
  for (int i = 0; i < fonts.length(); i++) {
    auto j = m_blend_fonts.find(fonts[i].trim());
    if (j != end) {
      if ( ! has.has(j.value()->name()) ) {
        has.set(j.value()->name(), true);
        m_default_fonts.push(j.value());
      }
    }
  }
  
  if ( /*! has.has(m_spare_family->name())*/ !m_default_fonts.length() ) {
    m_default_fonts.push(m_spare_family);
  }
}

/**
 * @func default_font_names
 */
Array<String> FontPool::default_font_names() const {
  Array<String> rev;
  for (int i = 0; i < m_default_fonts.length(); i++)
    rev.push(m_default_fonts[i]->name());
  return rev;
}

Array<String> FontPool::font_names(cString& family_name) const {
  FontFamily* ff = const_cast<FontPool*>(this)->get_font_family(family_name);
  return ff ? ff->font_names() : Array<String>();
}

FontFamily* FontPool::get_font_family(cString& family_name) {
  auto i = m_familys.find(family_name);
  return i == m_familys.end() ? NULL : i.value();
}

/**
 * @func get_font # 通过名称获得一个字体对像
 * @arg name {cString&} # 字体名称或家族名称
 * @arg [style = fs_regular] {Font::TextStyle}
 * @ret {Font*}
 */
Font* FontPool::get_font(cString& name, TextStyle style) {
  auto i = m_blend_fonts.find(name);
  return i == m_blend_fonts.end() ? NULL : i.value()->font(style);
}

/**
 * @func get_group # 通过字体名称列表获取字型集合
 * @arg id {cFFID} # cFFID
 * @arg [style = fs_regular] {Font::TextStyle} # 使用的字体家族才生效
 */
FontGlyphTable* FontPool::get_table(cFFID ffid, TextStyle style) {
  
  uint code = ffid->code() + (uint)style;
  
  auto i = m_tables.find(code);
  if ( !i.is_null() ) {
    return i.value();
  }
  
  FontGlyphTable* table = new FontGlyphTable();
  _inl_table(table)->initializ(ffid, style, this);
  
  m_tables.set(code, table);
  
  return table;
}

/**
 * @func get_table # 获取默认字型集合
 * @arg [style = fs_regular] {Font::TextStyle}
 */
FontGlyphTable* FontPool::get_table(TextStyle style) {
  return get_table(get_font_familys_id(String::empty), style);
}

/**
 * @func register_font # 通过Buffer数据注册字体
 * @arg buff {cBuffer&} # 字体数据
 * @arg [family_alias = String::empty] {cString&} # 给所属家族添加一个别名
 */
void FontPool::register_font(cBuffer& buff, cString& family_alias) {
  
  const FT_Byte* data = (const FT_Byte*)*buff;
   
  FT_Face face;
  FT_Error err = FT_New_Memory_Face((FT_Library)m_ft_lib, data, buff.length(), 0, &face);
  
  if (err) {
    WARN("Unable to load font, Freetype2 error code: %d", err);
  } else {
    
    FT_Long num_faces = face->num_faces;
    String  family_name = face->family_name;
    int     face_index = 0;
    
    while (1) {
      
      if (face->charmap &&
          face->charmap->encoding == FT_ENCODING_UNICODE && // 必须要有unicode编码表
          FT_IS_SCALABLE(face)                              // 必须为矢量字体
      ) {
        // 以64 pem 为标准
        float ratio = face->units_per_EM / 4096.0 /*( 64 * 64 = 4096 )*/;
        int height = face->height / ratio;
        int max_advance_width = face->max_advance_width / ratio;
        int ascender = face->ascender / ratio;
        int descender = -face->descender / ratio;
        int underline_position	= face->underline_position;
        int underline_thickness = face->underline_thickness;
        
        _inl_pool(this)->register_font(family_name,
                                       FT_Get_Postscript_Name(face),
                                       parse_style_flags(face->style_name, face->style_flags),
                                       (uint)face->num_glyphs,
                                       face_index, height,
                                       max_advance_width, ascender,
                                       descender, underline_position,
                                       underline_thickness, String::empty, buff);
      }
      face_index++;
      
      FT_Done_Face(face);
      
      if (face_index < num_faces) {
        err = FT_New_Memory_Face((FT_Library)m_ft_lib, data, buff.length(), face_index, &face);
        if (err) {
          WARN("Unable to load font, Freetype2 error code: %d", err); break;
        }
      } else {
        break;
      }
    }
    
    // set family_alias
    set_family_alias(family_name, family_alias);
  }
}

/**
 * @func register_font_file  # 注册本地字体文件
 * @arg path {cString&} # 字体文件的本地路径
 * @arg [family_alias = String::empty] {cString&} # 给所属家族添加一个别名
 */
void FontPool::register_font_file(cString& path, cString& family_alias) {
  
  if (!m_paths.has(path) ) { //
    
    Handle<SimpleFontFamily> sffd = inl_read_font_file(path, (FT_Library)m_ft_lib);
    
    if ( !sffd.is_null() ) {
      
      for (int i = 0; i < sffd->fonts.length(); i++) {
        
        SimpleFont& sfd = sffd->fonts[i];
        
        _inl_pool(this)->register_font(sffd->family,
                                       sfd.name,
                                       sfd.style_flags,
                                       sfd.num_glyphs,
                                       i,
                                       sfd.height,
                                       sfd.max_advance,
                                       sfd.ascender,
                                       sfd.descender,
                                       sfd.underline_position,
                                       sfd.underline_thickness, sffd->path, Buffer());
      }
      // set family_alias
      set_family_alias(sffd->family, family_alias);
    }
  }
}

/**
 * @func set_family_alias
 */
void FontPool::set_family_alias(cString& family, cString& alias) {
  if ( ! alias.is_empty() ) {
    
    auto i = m_blend_fonts.find(family);
    
    if (i != m_blend_fonts.end() && !m_blend_fonts.has(alias)) {
      m_blend_fonts[alias] = i.value(); // 设置一个别名
    }
  }
}

/**
 * @func clear # 释放池中不用的字体数据
 * @arg [full = false] {bool} # 全面清理资源尽可能最大程度清理
 */
void FontPool::clear(bool full) {
  
  for ( auto& i : m_tables ) {
    _inl_table(i.value())->clear_table();
  }
  
  for ( auto& i : m_fonts ) {
    _inl_font(i.value())->clear(full);
  }
}

/**
 * @func bind_display_port
 */
void FontPool::bind_display_port(DisplayPort* display_port) {
  av_assert( ! m_display_port );
  display_port->av_on(change, &Inl::display_port_change_handle, _inl_pool(this));
  m_display_port = display_port;
}

/**
 * @func get_glyph_texture_level 通过字体尺寸获取纹理等级,与纹理大小font_size
 */
TexureLevel FontPool::get_glyph_texture_level(float& font_size_out) {
  if (font_size_out > m_max_glyph_texture_size) {
    return FontGlyph::LEVEL_NONE;
  }
  uint index = ceilf(font_size_out);
  
  FontGlyphTextureLevel leval = font_glyph_texture_levels_idx[index];
  
  font_size_out = leval.max_font_size;
  
  return leval.level;
}

/**
 * @func get_glyph_texture_level # 根据字体尺寸获取纹理等级
 */
float FontPool::get_glyph_texture_size(TexureLevel leval) {
  av_assert( leval < FontGlyph::LEVEL_NONE );
  
  const float glyph_texture_levels_size[13] = {
    10, 12, 14, 16, 18, 20, 25, 32, 64, 128, 256, 512, 0
  };
  return glyph_texture_levels_size[leval];
}

/**
 * @func default_font_familys_id
 */
static cFFID default_font_familys_id() {
  static FontFamilysID* id = nullptr; // default group i
  if ( ! id ) {
    id = new FontFamilysID();
    _inl_ff_id(id)->initializ(Array<String>());
  }
  return id;
}

/**
 * @func get_font_familys_id
 */
cFFID FontPool::get_font_familys_id(const Array<String> fonts) {
  
  static Map<uint, FontFamilysID*> ffids; // global groups
  
  if ( fonts.length() ) {
    FontFamilysID id;
    _inl_ff_id(&id)->initializ(fonts);
    
    auto it = ffids.find(id.code());
    if (it != ffids.end()) {
      return it.value();
    } else {
      FontFamilysID* id_p = new FontFamilysID(move( id ));
      ffids.set ( id_p->code(), id_p );
      return id_p;
    }
  } else {
    return default_font_familys_id();
  }
}

/**
 * @func get_font_familys_id
 */
cFFID FontPool::get_font_familys_id(cString fonts) {
  if ( fonts.is_empty() ) {
    return default_font_familys_id();
  } else {
    Array<String> ls = fonts.split(',');
    Array<String> ls2;
    
    for (int i = 0, len = ls.length(); i < len; i++) {
      String name = ls[i].trim();
      if ( ! name.is_empty() ) {
        ls2.push(name);
      }
    }
    return get_font_familys_id(ls2);
  }
}

av_gui_end
