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

#include "autil/version.h"

#if av_ios
static cString system_fonts_dir = "/System/Library/Fonts";
#elif av_android
static cString system_fonts_dir = "/system/fonts";
#endif

static Array<FontPool::SimpleFontFamily>* simple_font_family_info = nullptr;

/**
 * @func read_simple_font_family_with_cache
 */
static bool read_simple_font_family_with_cache() {
  
  String path = Path::temp(".simple_font_family_info");
  
  if ( FileHelper::exists_sync(path) ) { // 这个文件存在
    String data = FileHelper::read_file_sync(path);
    Array<String> ls = data.split('\n');
    
    if ( ls.length() != 2 )
      return false;
    
    String check_code = ls[0]; // 第一行代码为文件校验码
    String json_str = ls[1];
    
    if ( check_code == hash(json_str) ) { // 验证文件是否被改动或损坏
      JSON json = JSON::parse(json_str);
      String sys_id = hash(sys::info()); // 系统标识
      String lib_version = AVOCADO_VERSION;
      
      if (sys_id == json["sys_id"].to_cstring() && // 操作系统与是否升级
          lib_version == json["library_version"].to_cstring() // lib版本是否变化
      ) {
        JSON familys = json["font_familys"];
        
        for ( int i = 0, len = familys.length(); i < len; i++ ) {
          JSON& item = familys[i];
          JSON& fonts = item["fonts"]; // fonts
          
          FontPool::SimpleFontFamily sffd = {
            item["path"].to_string(), // path
            item["family"].to_string(), // family
          };

          //LOG("family:%s, %s", item["family"].to_cstring(), item["path"].to_cstring());
          
          for ( int j = 0, o = fonts.length(); j < o; j++ ) {
            JSON& font = fonts[j];
            sffd.fonts.push({
              font[0].to_string(),  // name
              font[1].to_string(),  // style
              font[2].to_uint(),    // style_flag
              font[3].to_uint(),    // num_glyphs
              font[4].to_int(),     // height
              font[5].to_int(),     // max_advance
              font[6].to_int(),     // ascender
              font[7].to_int(),     // descender
              font[8].to_int(),     // underline_position
              font[9].to_int(),     // underline_thickness
            });

            //LOG("       %s", *JSON::stringify(font));
          }
          simple_font_family_info->push( move(sffd) );
        }
        return true; // ok
      }
    }
  }
  return false;
}

/**
 * @func get_sys_font_family
 */
const Array<FontPool::SimpleFontFamily>& FontPool::get_sys_font_family() {
  
  if ( ! simple_font_family_info ) {
    
    simple_font_family_info = new Array<SimpleFontFamily>();
    
    // 先读取缓存文件,如果找不到缓存文件遍历字体文件夹
    if ( ! read_simple_font_family_with_cache() ) {
    
      String path = Path::temp(".simple_font_family_info");
      String sys_id = hash( sys::info() ); // 系统标识
      
      FT_Library ft_lib;
      FT_Init_FreeType( &ft_lib );
      
      ScopeClear clear([&ft_lib]() { FT_Done_FreeType( ft_lib ); });
      
      JSON font_familys = JSON::array();
      
      FileHelper::each(system_fonts_dir, Callback([&](SimpleEvent& d) {
        
        Dirent* ent = static_cast<Dirent*>(d.data);
        
        if ( ent->type == FILE_FILE ) {
          Handle<SimpleFontFamily> sffd = inl_read_font_file(ent->pathname, ft_lib);
          
          if ( ! sffd.is_null() ) {
            JSON item = JSON::object();
            item["path"] = sffd->path;
            item["family"] = sffd->family;
            JSON fonts = JSON::array();
            
            for ( int i = 0; i < sffd->fonts.length(); i++ ) {
              SimpleFont& sfd = sffd->fonts[i];
              JSON font = JSON::array();
              font[0] = sfd.name;
              font[1] = sfd.style;
              font[2] = sfd.style_flags;
              font[3] = sfd.num_glyphs;
              font[4] = sfd.height;
              font[5] = sfd.max_advance;
              font[6] = sfd.ascender;
              font[7] = sfd.descender;
              font[8] = sfd.underline_position;
              font[9] = sfd.underline_thickness;
              fonts[i] = font;
            }
            item[ "fonts" ] = fonts;
            font_familys[ font_familys.length() ] = item;
            simple_font_family_info->push( move(**sffd) );
          }
        }
        d.return_value = 1;
      }));
      
      JSON json = JSON::object();
      json["sys_id"] = sys_id;
      json["library_version"] = AVOCADO_VERSION;
      json["font_familys"] = font_familys;
      String json_str = JSON::stringify( json );
      // LOG(json_str);
      
      String data = String::format( "%s\n%s", *hash(json_str), *json_str );
      FileHelper::write_file_sync(path, data); // 写入文件
    }
  }
  
  return *simple_font_family_info;
}

/**
 * @func inl_set_default_fonts
 */
void inl_set_default_fonts(FontPool* pool) {
  
  Array<String> first;    // 第一个可能的默认的字体列表
  Array<String> second;   // 第二默认字体
  Array<String> third;    // 第三默认字体
  Array<String> four;
  
#if av_ios
  first.push(".SFUIText-Regular");  // ios 9 苹方UI英文字体
  first.push("Helvetica");          // 向下兼容英文字体,没有上面的会使用这个
  second.push("PingFang HK");       // ios 9 苹方中文字体
  second.push("HeitiFallback");     // 向下兼容中文字体
  second.push("Heiti TC");          // 向下兼容中文字体
  third.push("Thonburi");           // 向下兼容英文字体
  four.push(".HeitiFallback");      // ios 9 系统的隐藏字体
#elif av_android
  first.push("Roboto");
  first.push("Droid Sans");
  first.push("Droid Sans Mono");
  second.push("Droid Sans Fallback");
#endif
  
  pool->set_default_fonts(&first, &second, &third, &four, NULL);
}
