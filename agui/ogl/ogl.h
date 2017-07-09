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
 
#ifndef __avocado__gui__ogl__ogl__
#define __avocado__gui__ogl__ogl__

#include "../draw.h"

#if av_ios
# include <OpenGLES/ES3/gl.h>
# include <OpenGLES/ES3/glext.h>
#
#elif av_android
# define GL_GLEXT_PROTOTYPES
# include <GLES3/gl3.h>
# include <GLES3/gl3ext.h>
#
#elif av_osx
#
#elif av_linux
# define GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
# include <GL/glext.h>
#
#elif av_win
#
#else
# error "The operating system does not support"
#endif

/**
 * @ns gui
 */

av_gui_begin

struct GLShaders;
class  GLDraw;
class  GLES2Draw;
class  Font;
class  FontGlyph;

namespace shader {
  struct NativeGLSL {
    const char* name;
    const unsigned char* source_vp;
    const unsigned char* source_fp;
    unsigned long source_vp_len;
    unsigned long source_fp_len;
    int shader;
    const char* shader_uniforms;
    const char* shader_uniform_blocks;
    const char* shader_attributes;
  };
}

/**
 * @class Shader # opengl 着色器程序
 * @bases Object
 */
class GLShader {
public:
  
  /**
   * @func uniform # 通过名称返回着色器统一值句柄
   * @arg name {cchar*}
   * @ret {GLint}
   */
  inline GLint uniform(cchar* name) const {
    return glGetUniformLocation(m_handle, name);
  }
  
  /**
   * @func attribute # 通过名称返回着色器属性句柄
   * @arg name {cchar*}
   * @ret {GLint}
   * @const
   */
  inline GLint attribute(cchar* name) const {
    return glGetAttribLocation(m_handle, name);
  }
  
  /**
   * #func handle # GL句柄值
   * @ret {GLuint}
   * @const
   */
  inline GLuint handle() const { return m_handle; }
  
  /**
   * @func use # 使用着色器程序
   */
  void use() const;
  
  /**
   * @func is_query
   */
  inline bool is_query() const { return m_is_query; }
  
private:
  
  /**
   * @func handle
   * @arg value {GLuint}
   */
  inline void handle(GLuint value) { m_handle = value; }
  
private:
#pragma pack(push, 4)
  GLuint  m_handle;
  int     m_is_query;
#pragma pack(pop)
  
  friend class GLDraw;
  friend class GLES2Draw;
};

/**
 * @class GLDraw
 */
class GLDraw: public Draw {
public:
  
  /**
   * @constructor
   */
  GLDraw(const Map<String, int>& option);
  
  /**
   * @destructor
   */
  virtual ~GLDraw();
  
  /**
   * 初始化上下文
   */
  virtual void initializ();
  virtual void refresh_status_for_buffer();
  virtual void refresh_status_for_root_matrix(Mat4& root, Mat4& query_root);
  virtual void begin_render();
  virtual void commit_render();
  virtual void begin_screen_occlusion_query();
  virtual void end_screen_occlusion_query();
  virtual void set_anisotropic(bool value);
  virtual void set_mipmap(bool value);
  virtual bool load_texture(Texture* tex, const Array<PixelData>& data);
  virtual bool load_yuv_texture(TextureYUV* yuv_tex, cPixelData& data);
  virtual void delete_texture(uint id);
  virtual void use_texture(uint id, Repeat repeat, uint slot);
  virtual void use_texture(uint id, uint slot);
  virtual void delete_buffer(uint id);
  virtual void refresh_status_for_font_pool(FontPool* pool);
  virtual bool set_font_glyph_vertex_data(Font* font, FontGlyph* glyph);
  virtual bool set_font_glyph_texture_data(Font* font, FontGlyph* glyph, int level);
  virtual void clear_screen(Color color);
  /**
   * @overwrite draw
   */
  virtual void draw(Root* v);
  virtual void draw(Video* v);
  virtual void draw(Image* v);
  virtual void draw(Shadow* v);
  virtual void draw(Box* v);
  virtual void draw(TextNode* v);
  virtual void draw(Label* v);
  virtual void draw(Text* v);
  virtual void draw(Sprite* v);
  virtual void draw(Scroll* v);
  virtual void draw(Clip* v);
  virtual void draw(Input* v);
  virtual void draw(Textarea* v);
  
  /**
   * @func get_ogl_texture_pixel_format 获取当前环境对应的OpenGL纹理像素格式,如果返回0表示不支持纹理格式
   */
  virtual GLint get_ogl_texture_pixel_format(PixelData::Format pixel_format) = 0;
  
  /**
   * @func compile_shader       # 编译着色器程序
   * @arg code {cData&}         #     代码
   * @arg shader_type {GLenum}  #     程序类型
   * @ret {GLuint}
   */
  GLuint compile_shader(cString& name, cBuffer& code, GLenum shader_type);
  
  /**
   * @func compile_link_shader # 编译着色器程序
   * @arg vertex {cBuffer&}                 #             顶点程序代码
   * @arg fragment {cData&}                 #             片段程序代码
   * @arg attrs {const Array<String>&}      #             要编号的属性列表
   * @ret {GLuint}
   */
  GLuint compile_link_shader(cString& name, cBuffer& vertex,
                             cBuffer& fragment,
                             const Array<String>& attrs = Array<String>());
  /**
   * @func shaders_vector # 当前上下文着色器列表
   * @ret {const Array<GLShader>&}
   */
  inline const Array<GLShader>& shaders_vector() const {
    return m_shaders_vector;
  }
  
  virtual bool is_support_query() { return true; }
  virtual bool is_support_vao() { return true; }
  virtual bool is_support_instanced() { return true; }
  virtual bool is_support_multisampled() { return true; }
  virtual bool is_support_compressed_ETC1() { return true; }
  virtual bool is_support_packed_depth_stencil() { return true; }
  virtual void gl_main_render_buffer_storage();
  
  /**
   * @func initializ_ogl_buffers
   */
  virtual void initializ_ogl_buffers();
    
  /**
   * @func initializ_ogl_status
   */
  void initializ_ogl_status();
  
protected:
  
  bool              m_begin_screen_occlusion_query_status; // 屏幕遮挡test状态
  GLuint            m_SCREEN_RANGE_OCCLUSION_QUERY_HANDLE; // 屏幕遮挡查询对像句柄
  Array<GLShader>   m_shaders_vector; // 着色器程序表
  void*             m_shaders; // 着色器程序表
  const GLShader*   m_cur_use_shader;
  GLuint            m_render_buffer;
  GLuint            m_frame_buffer;
  GLuint            m_msaa_render_buffer;
  GLuint            m_msaa_frame_buffer;
  GLuint            m_depth_buffer;
  GLuint            m_stencil_buffer;
  GLuint            m_stencil_ref_value;
  GLuint            m_root_stencil_ref_value;
  
  av_def_inl_cls(Inl);
  av_def_inl_cls(Inl2);
  
  friend const GLShaders* glshaders();
  friend class GLShader;
  friend class Texture;
  friend class IOSGLDrawCore;
};

/**
 * @func glshaders
 */
av_inline const GLShaders* glshaders() {
  return static_cast<GLShaders*>(static_cast<GLDraw*>(draw_ctx())->m_shaders);
}

av_gui_end

#endif
