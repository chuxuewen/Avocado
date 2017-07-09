/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright © 2015-2016, louis.chu
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

#include "draw.h"
#include "font.h"
#include "texture.h"

av_gui_begin

static char empty_[4] = { 0, 0, 0, 0 };
static cPixelData empty_pixel_data(WeakBuffer(empty_, 4), 1, 1, PixelData::RGBA8888);

/**
 * @class TextureEmpty
 */
class TextureEmpty: public Texture {
public:
  virtual ~TextureEmpty() {
    Texture::unload();
  }
  virtual void load() {
    if ( !load_data(empty_pixel_data) ) {
      av_fatal("Load temp texture error");
    }
  }
  virtual void unload() { /* empty noop */ }
};

void Draw::set_screen_size(uint width, uint height) {
  if (m_screen_width != width || m_screen_height != height) {
    m_screen_width = width;
    m_screen_height = height;
    refresh_status_for_buffer();
    av_trigger(screen_size_change);
  }
}

Draw* Draw::m_draw_ctx = nullptr; // 当前GL上下文

/**
 * @constructor
 */
Draw::Draw(const Map<String, int>& option)
: av_init_event(screen_size_change)
, m_anisotropic(false)
, m_mipmap(false)
, m_multisample(0)
, m_empty_texture( NewRetain<TextureEmpty>() )
, m_font_pool(nullptr)
, m_tex_pool(nullptr)
, m_best_display_scale(1)
, m_screen_width(0)
, m_screen_height(0)
, m_library(DRAW_LIBRARY_INVALID)
{
  av_assert_err(!m_draw_ctx, "At the same time can only run a GLDraw entity");
  m_draw_ctx = this;
  
  memset(m_cur_bind_textures, 0, sizeof(Texture*) * 8);
  
  if (option.has("anisotropic")) m_anisotropic = option.get("anisotropic");
  if (option.has("mipmap")) m_mipmap = option.get("mipmap");
  if (option.has("multisample")) m_multisample = av_max(option.get("multisample"), 0);
  
  m_font_pool = new FontPool(this); // 初始字体池
  m_tex_pool = new TexturePool(this); // 初始文件纹理池
}

Draw::~Draw() {
  
  Release(m_empty_texture); m_empty_texture = nullptr;
  Release(m_font_pool); m_font_pool = nullptr;
  Release(m_tex_pool); m_tex_pool = nullptr;
  m_draw_ctx = nullptr;
}

/**
 * @func cur_texture
 */
Texture* Draw::current_texture(uint slot) {
  return m_cur_bind_textures[slot];
}

/**
 * @func clear
 */
void Draw::clear(bool full) {
  m_tex_pool->clear(full);
  m_font_pool->clear(full);
}

/**
 * @func support_max_texture_font_size
 */
uint Draw::support_max_texture_font_size() {
  return 512;
}

av_gui_end
