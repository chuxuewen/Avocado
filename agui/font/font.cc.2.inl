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

/**
 * @func parse_style_flag
 */
static uint parse_style_flags(cString& style_name, FT_ULong style_flags) {
  
  static String Light("light");
  static String Regular("regular");
  static String Medium("medium");
  static String Thin("thin");
  static String Bold("bold");
  static String Italic("italic");
  
  uint flags = 0;
  
  String str = style_name.to_lower_case();
  int len = str.length();
  
  str = str.replace(Light, String::empty);
  if ( str.length() != len ) {
    flags |= FSF_LIGHT;
    len = str.length();
  }
  
  str = str.replace(Thin, String::empty);
  if ( str.length() != len ) {
    flags |= (FSF_LIGHT | FSF_OTHER);
    len = str.length();
  }
  
  str = str.replace(Regular, String::empty);
  if ( str.length() != len ) {
    flags |= FSF_REGULAR;
    len = str.length();
  }
  
  str = str.replace(Medium, String::empty);
  if ( str.length() != len ) {
    flags |= (FSF_REGULAR | FSF_OTHER);
    len = str.length();
  }
  
  str = str.replace(Bold, String::empty);
  if ( str.length() != len ) {
    flags |= FSF_BOLD;
    len = str.length();
  }
  
  str = str.replace(Italic, String::empty);
  if ( str.length() != len ) {
    flags |= FSF_ITALIC;
    len = str.length();
  }
  
  if (!str.trim().is_empty()) { // 还有其它
    flags |= FSF_OTHER;
  }
  
  return flags;
}

/**
 * @func read_font_file
 */
Handle<FontPool::SimpleFontFamily> FontPool::read_font_file(cString& path) {
  FT_Library lib;
  FT_Init_FreeType(&lib);
  
  ScopeClear clear([&lib]() { FT_Done_FreeType(lib); });
  
  return inl_read_font_file(path, lib);
}

/**
 * @func read_font_file
 */
Handle<FontPool::SimpleFontFamily> inl_read_font_file(cString& path, FT_Library lib) {
  FT_Face face;
  FT_Error err = FT_New_Face(lib, Path::restore_c(path), 0, &face);
  
  if (err) {
    WARN("Unable to load font file \"%s\", Freetype2 error code: %d", *path, err);
  } else {
    
    FT_Long num_faces = face->num_faces;
    
    Handle<FontPool::SimpleFontFamily> sff = new FontPool::SimpleFontFamily();
    
    sff->path = path;
    sff->family = face->family_name;
    
    int face_index = 0;
    
    while (1) {
      
      if (face->charmap &&
          face->charmap->encoding == FT_ENCODING_UNICODE && // 必须要有unicode编码表
          FT_IS_SCALABLE(face)                              // 必须为矢量字体
      ) {
        
        FT_Set_Char_Size(face, 0, 64 * 64, 72, 72);
      
        // 以64 pt 为标准
        float ratio = face->units_per_EM / 4096.0 /*( 64 * 64 = 4096 )*/;
        int height = face->height / ratio;
        int max_advance_width = face->max_advance_width / ratio;
        int ascender = face->ascender / ratio;
        int descender = -face->descender / ratio;
        int underline_position	= face->underline_position;
        int underline_thickness = face->underline_thickness;
        
        sff->fonts.push({
          FT_Get_Postscript_Name(face),
          face->style_name,
          parse_style_flags(face->style_name, face->style_flags),
          (uint)face->num_glyphs,
          height,
          max_advance_width,
          ascender,
          descender,
          underline_position,
          underline_thickness
        });
      }
      
      face_index++;
      
      FT_Done_Face(face);
      
      if (face_index < num_faces) {
        err = FT_New_Face(lib, Path::restore_c(path), face_index, &face);
        if (err) {
          WARN("Unable to load font file \"%s\", Freetype2 error code: %d", *path, err); break;
        }
      } else {
        if (sff->fonts.length())
          return sff;
        else
          break;
      }
    }
  }
  return Handle<FontPool::SimpleFontFamily>();
}
