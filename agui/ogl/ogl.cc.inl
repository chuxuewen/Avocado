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

#define gl_  glshaders()
#define es2_ reinterpret_cast<const ES2GLShaders*>(glshaders())

#define SIZEOF(T) sizeof(T) / sizeof(float)
#define av_ctx_data(view, T)       static_cast<CtxDataWrap<T>*>(view->m_ctx_data)->value()
#define av_ctx_data_float_p(view)  static_cast<CtxDataWrap<CtxData>*>(view->m_ctx_data)->float_p()

#define new_ctx_data2(v) if ( !v->m_ctx_data ) {  \
v->m_ctx_data = new CtxDataWrap<Box_CtxData>(); \
mark_value |= (View::M_SHAPE | View::M_BORDER_RADIUS | View::M_TRANSFORM);  \
}

template<typename T>
class CtxDataWrap: public Object {
public:
  inline CtxDataWrap() {
    memset(&m_value, 0, sizeof(T)); m_value.opacity = 1;
  }
  inline float* float_p() { return m_value.view_matrix.value(); }
  inline T* value() { return &m_value; }
private:
  T m_value;
};

struct CtxData {
  Mat   view_matrix;
  float opacity;
  Vec2  vertex_ac[2];
};

struct Box_CtxData: CtxData {
  float border_width[4];      // left/top/right/bottom
  float radius_size[4];       // left-top/top-right/right-bottom/bottom-left
  FloatColor border_color[4];
  FloatColor background_color;
};

struct Image_CtxData: Box_CtxData { };

struct Shadow_CtxData: Box_CtxData {
  float       offset_x;
  float       offset_y;
  float       size;
  FloatColor  color;
};

struct Sprite_CtxData: CtxData {
  Vec2 texture_start;
  Vec2 texture_size;
  Vec2 texture_ratio;
};

/**
 * @class GLDraw::Inl2
 */
class GLDraw::Inl2: public GLDraw {
public:
#define _inl2_gl(self) static_cast<GLDraw::Inl2*>(self)
  
  /**
   * @func new_ctx_data
   */
  void new_ctx_data(Box* v) {
    if ( !v->m_ctx_data ) {
      v->m_ctx_data = new CtxDataWrap<Box_CtxData>();
      v->mark_value |= (Box::M_SHAPE | Box::M_BORDER_RADIUS);
    }
  }
  
  void new_ctx_data(Shadow* v) {
    if ( !v->m_ctx_data ) {
      v->m_ctx_data = new CtxDataWrap<Shadow_CtxData>();
    }
  }
  
  void new_ctx_data(Sprite* v) {
    if ( !v->m_ctx_data ) {
      v->m_ctx_data = new CtxDataWrap<Sprite_CtxData>();
    }
  }
  
  void new_ctx_data(Image* v) {
    if ( !v->m_ctx_data ) {
      v->m_ctx_data = new CtxDataWrap<Image_CtxData>();
    }
  }
  
  void solve(Box* v) {
    
    uint mark_value = v->mark_value;
    
    if ( mark_value & Box::M_BACKGROUND_COLOR ) { // 背景颜色
      
      if ( v->m_background_color.a() ) {
        new_ctx_data2(v); // 初始gl数据
        Box_CtxData* data = av_ctx_data(v, Box_CtxData);
        data->background_color = v->m_background_color.to_float_color();
        //av_debug("color2,%d,%f", v->m_background_color.a(),data->background_color.a());
      }
    }
    
    if ( mark_value & Box::M_BORDER ) { // 边框
      
      v->m_is_draw_border = (v->m_border_left.width != 0 || v->m_border_right.width  != 0 ||
                             v->m_border_top.width  != 0 || v->m_border_bottom.width != 0);
      if ( v->m_is_draw_border ) {
        new_ctx_data2(v); // 初始gl数据
        Box_CtxData* data =  av_ctx_data(v, Box_CtxData);
        data->border_width[0] = v->m_border_left.width;
        data->border_width[1] = v->m_border_top.width;
        data->border_width[2] = v->m_border_right.width;
        data->border_width[3] = v->m_border_bottom.width;
        data->border_color[0] = v->m_border_left.color.to_float_color();
        data->border_color[1] = v->m_border_top.color.to_float_color();
        data->border_color[2] = v->m_border_right.color.to_float_color();
        data->border_color[3] = v->m_border_bottom.color.to_float_color();
      }
      mark_value |= Box::M_BORDER_RADIUS; // 边框会影响圆角
    }
    
    
    if ( v->m_ctx_data ) { // m_ctx_data = nullptr 表示这个视图没有绘制任务,所以不需要设置绘制数据
      
      Box_CtxData* data = av_ctx_data(v, Box_CtxData);
      
      // 形状变化包括 width、height、border、margin,设置顶点数据
      if ( mark_value & Box::M_SHAPE ) {
        data->vertex_ac[0].x( -v->m_origin.x() );
        data->vertex_ac[0].y( -v->m_origin.y() );
        data->vertex_ac[1].x( v->m_final_width - v->m_origin.x() );
        data->vertex_ac[1].y( v->m_final_height - v->m_origin.y() );
        mark_value |= Box::M_BORDER_RADIUS; // 会影响圆角
      }
      
      if ( mark_value & Box::M_BORDER_RADIUS ) { // 圆角标记
        
        float w = (v->m_final_width + v->m_border_left.width + v->m_border_right.width) / 2;
        float h = (v->m_final_height + v->m_border_top.width + v->m_border_bottom.width) / 2;
        float max = av_min(w, h);
        data->radius_size[0] = av_min(v->m_border_radius_left_top, max);
        data->radius_size[1] = av_min(v->m_border_radius_right_top, max);
        data->radius_size[2] = av_min(v->m_border_radius_right_bottom, max);
        data->radius_size[3] = av_min(v->m_border_radius_left_bottom, max);
        
        v->m_is_draw_border_radius = (data->radius_size[0] != 0 || data->radius_size[1] != 0 ||
                                      data->radius_size[2] != 0 || data->radius_size[3] != 0
                                      );
      }
      
      if ( mark_value & View::M_TRANSFORM ) {
        data->view_matrix = v->m_final_matrix;
        data->opacity = v->m_final_opacity; // opacity
      }
      else {
        if ( mark_value & Box::M_OPACITY ) {
          data->opacity = v->m_final_opacity; // opacity
        }
        if ( mark_value & Box::M_TRANSFORM_AND_OPACITY_CTX_DATA ) {
          data->view_matrix = v->m_final_matrix;
          data->opacity = v->m_final_opacity; // opacity
        }
      }
    }
  }
  
  /**
   * @func draw_scroll_bar
   */
  void draw_scroll_bar(Box* v1, BasicScroll* v) {
    
    if ( (v->m_h_scrollbar || v->m_v_scrollbar) && v->m_scrollbar_color.a() ) {
      
      const uint size = SIZEOF(Box_CtxData);
      
      float scrollbar_width = v->scrollbar_width();
      float scrollbar_margin = v->scrollbar_margin();
      
      Box_CtxData data;
      memset(&data, 0, size);
      data.view_matrix = v1->m_final_matrix;
      data.opacity = v1->m_final_opacity * v->m_scrollbar_opacity;
      data.radius_size[0] = scrollbar_width / 2.0;
      data.radius_size[1] = scrollbar_width / 2.0;
      data.radius_size[2] = scrollbar_width / 2.0;
      data.radius_size[3] = scrollbar_width / 2.0;
      data.background_color = v->m_scrollbar_color.to_float_color();
      
      gl_->box_color_radius.use();
      
      if ( v->m_h_scrollbar ) { // 绘制水平滚动条
        data.vertex_ac[0].x( v->m_h_scrollbar_position[0] - v1->m_origin.x() );
        data.vertex_ac[0].y( v1->m_final_height - v1->m_origin.y() - scrollbar_width - scrollbar_margin);
        data.vertex_ac[1].x( data.vertex_ac[0].x() + v->m_h_scrollbar_position[1] );
        data.vertex_ac[1].y( data.vertex_ac[0].y() + scrollbar_width );
        
        glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, data.view_matrix.value());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
      }
      
      if ( v->m_v_scrollbar ) { // 绘制垂直滚动条
        data.vertex_ac[0].x( v1->m_final_width - v1->m_origin.x() - scrollbar_width - scrollbar_margin );
        data.vertex_ac[0].y( v->m_v_scrollbar_position[0] - v1->m_origin.y() );
        data.vertex_ac[1].x( data.vertex_ac[0].x() + scrollbar_width );
        data.vertex_ac[1].y( data.vertex_ac[0].y() + v->m_v_scrollbar_position[1] );
        
        glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, data.view_matrix.value());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
      }
    }
  }
  
  /**
   * @func draw_box
   */
  void draw_box(Box* v) {
    if ( v->m_ctx_data /* 没有数据无需绘制 */ ) {
      
      const uint size = SIZEOF(Box_CtxData);
      
      if ( v->m_is_draw_border_radius ) { // 圆角
        
        if ( v->m_background_color.a() ) { // 绘制背景
          gl_->box_color_radius.use();
          
          glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
          glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
        }
        
        if ( v->m_is_draw_border ) { // 绘制边框
          
          gl_->box_border_radius.use();
          glUniform1fv(gl_->box_border_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
          
          if ( m_library == DRAW_LIBRARY_GLES2 ) {
            if ( is_support_instanced() ) {
              glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
            } else {
              for (int i = 0; i < 4; i++) {
                glVertexAttrib1f(es2_->es2_box_border_radius_in_instance_id, i);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 84);
              }
            }
          } else {
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
          }
        }
      } else {
        
        if ( v->m_background_color.a() ) { // 绘制背景
          gl_->box_color.use();
          
          glUniform1fv(gl_->box_color_uniform_draw_data, size, av_ctx_data_float_p(v));
          glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        
        if ( v->m_is_draw_border ) { // 绘制边框
          gl_->box_border.use();
          glUniform1fv(gl_->box_border_uniform_draw_data, size, av_ctx_data_float_p(v));
          
          if ( m_library == DRAW_LIBRARY_GLES2 ) {
            if ( is_support_instanced() ) {
              glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
            } else {
              for (int i = 0; i < 4; i++) {
                glVertexAttrib1f(es2_->es2_box_border_in_instance_id, i);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
              }
            }
          } else {
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
          }
          //
        }
      }
    } // end draw
  }
  
  /**
   * @func draw_text_background
   */
  void draw_text_background(View* v, TextFont::Data& data,
                            Color color, uint begin, uint end, Vec2 offset) {
    
    gl_->text_box_color.use();
    
    glUniform1fv(gl_->text_box_color_uniform_transf_opacity, 7, &v->m_final_matrix[0]);
    glUniform4f(gl_->text_box_color_uniform_background_color,
                color.r() / 255.0, color.g() / 255.0, color.b() / 255.0, color.a() / 255.0 );
    glUniform2f( gl_->text_box_color_uniform_origin, v->m_origin.x(), v->m_origin.y() );
    
    while ( begin < end ) {
      TextFont::Cell& cell = data.cells[begin];
      
      if ( cell.chars.length() ) {
        float y = cell.baseline - data.text_hori_bearing + offset.y();
        float offset_start = cell.offset_start + offset.x();
        float* offset_table = &cell.offset[0];
        
        if ( cell.reverse ) {
          glUniform4f(gl_->text_box_color_uniform_vertex_ac,
                      offset_start - offset_table[cell.chars.length()], y,
                      offset_start - offset_table[0], y + data.text_height);
        } else {
          glUniform4f(gl_->text_box_color_uniform_vertex_ac,
                      offset_start + offset_table[0], y,
                      offset_start + offset_table[cell.chars.length()], y + data.text_height);
        }
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 绘图背景
      }
      begin++;
    }
  }
  
  /**
   * @func draw_vector_text
   */
  void draw_vector_text(View* v, TextFont* f, TextFont::Data& data, Color color, Vec2 offset) {
    
    FontGlyphTable* table = font_pool()->get_table(f->m_text_family.value, f->m_text_style.value);
    
    glBindVertexArray(0);
    gl_->text_vertex.use();
    glUniform1fv(gl_->text_vertex_uniform_transf_opacity, 7, &v->m_final_matrix[0]);
    glUniform1f(gl_->text_vertex_uniform_text_size, f->m_text_size.value);
    glUniform4f(gl_->text_vertex_uniform_color,
                color.r() / 255.0, color.g() / 255.0, color.b() / 255.0, color.a() / 255.0 );
    glEnableVertexAttribArray(gl_->text_vertex_in_vertex);
    
    for (int i = data.cell_draw_begin, e = data.cell_draw_end; i < e; i++) {
      
      TextFont::Cell& cell = data.cells[i];
      uint count = cell.chars.length();
      if ( count ) {
        const uint16* chars = &cell.chars[0];
        float offset_start = cell.offset_start + offset.x();
        float* offset_table = &cell.offset[0];
        
        glUniform1f(gl_->text_vertex_uniform_hori_baseline, cell.baseline + offset.y());
        
        for (int j = 0; j < count; j++) {
          FontGlyph* glyph = table->use_vector_glyph(chars[j]);
          /* 水平偏移 */
          float offset_x = offset_start + (cell.reverse ? -offset_table[j + 1] : offset_table[j]);
          
          glUniform1f(gl_->text_vertex_uniform_offset_x, offset_x);
          glBindBuffer(GL_ARRAY_BUFFER, glyph->vertex_data());
          glVertexAttribPointer(gl_->text_vertex_in_vertex, 2, GL_SHORT, GL_FALSE, 0, 0);
          glDrawArrays(GL_TRIANGLES, 0, glyph->vertex_count()); // 绘图
        }
      }
    }
  }
  
  /**
   * @func draw_texture_text
   */
  void draw_texture_text(View* v, TextFont* f, TextFont::Data& data, Color color, Vec2 offset) {
    
    FontGlyphTable* table = font_pool()->get_table(f->m_text_family.value, f->m_text_style.value);
    TexureLevel level = data.texture_level;
    
    gl_->text_texture.use();
    
    glUniform1fv(gl_->text_texture_uniform_transf_opacity, 7, &v->m_final_matrix[0]);
    glUniform1f(gl_->text_texture_uniform_texture_scale, data.texture_scale);
    glUniform4f(gl_->text_texture_uniform_color,
                color.r() / 255.0, color.g() / 255.0, color.b() / 255.0, color.a() / 255.0 );
    glActiveTexture(GL_TEXTURE7);
    
    for (int i = data.cell_draw_begin, e = data.cell_draw_end; i < e; i++) {
      
      TextFont::Cell& cell = data.cells[i];
      uint count = cell.chars.length();
      if ( count ) {
        const uint16* chars = &cell.chars[0];
        float offset_start = cell.offset_start + offset.x();
        float* offset_table = &cell.offset[0];
        
        glUniform1f(gl_->text_texture_uniform_hori_baseline, cell.baseline + offset.y() );
        
        for (int j = 0; j < count; j++) {
          
          uint16 unicode = chars[j];
          FontGlyph* glyph = table->use_texture_glyph(unicode, level); /* 使用纹理 */
          FontGlyph::TexSize s = glyph->texture_size(level);
          glUniform4f(gl_->text_texture_uniform_tex_size, s.width, s.height, s.left, s.top );
          
          /* 水平偏移 */
          float offset_x = offset_start + (cell.reverse ? -offset_table[j + 1] : offset_table[j]);
          
          glUniform1f(gl_->text_texture_uniform_offset_x, offset_x);
          glBindTexture(GL_TEXTURE_2D, glyph->texture_id(level));
          glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // 绘制文字纹理
        }
      }
    }
  }
  
  /**
   * @func draw_text
   */
  void draw_text(View* v, TextFont* f, TextFont::Data& data, Color color, Vec2 offset) {
    if ( data.texture_level == FontGlyph::LEVEL_NONE ) { //  没有纹理等级,使用矢量顶点
      draw_vector_text(v, f, data, color, offset);
    } else {
      draw_texture_text(v, f, data, color, offset);
    }
  }
  
  /**
   * @func draw_input_cursor
   */
  void draw_input_cursor(Input* v, int linenum, Vec2 offset) {
    
    gl_->text_box_color.use();
    
    float y = v->m_rows[linenum].baseline - v->m_data.text_hori_bearing + offset.y();
    float x = v->cursor_x_ + offset.x();
    
    Color color = v->m_text_color.value;
    
    glUniform1fv(gl_->text_box_color_uniform_transf_opacity, 7, &v->m_final_matrix[0]);
    glUniform4f(gl_->text_box_color_uniform_background_color,
                color.r() / 255.0, color.g() / 255.0, color.b() / 255.0, color.a() / 255.0 );
    
    glUniform2f(gl_->text_box_color_uniform_origin, v->m_origin.x(), v->m_origin.y());
    glUniform4f(gl_->text_box_color_uniform_vertex_ac, x - 1, y, x + 1, y + v->m_data.text_height);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 绘图背景
  }
  
  /**
   * @func draw_begin_clip
   */
  void draw_begin_clip(Box* v) {
    
    const uint size = SIZEOF(Box_CtxData);
    
    if ( v->m_is_draw_border ) { // 绘制边框
      
      if ( v->m_is_draw_border_radius ) {
        gl_->box_border_radius.use();
        glUniform1fv(gl_->box_border_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_radius_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_STRIP, 0, 84);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
        }
      } else {
        gl_->box_border.use();
        glUniform1fv(gl_->box_border_uniform_draw_data, size, av_ctx_data_float_p(v));
        
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
        }
      }
    }
    
    // build stencil test value and background color draw
    
    if ( m_stencil_ref_value == m_root_stencil_ref_value ) {
      glEnable(GL_STENCIL_TEST); // 启用模板测试
      glStencilFunc(GL_ALWAYS, m_stencil_ref_value + 1, 0xFF); // 总是通过模板测试
      glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); // Test成功将参考值设置为模板值
    } else {
      glStencilOp(GL_KEEP, GL_INCR, GL_INCR); // Test成功增加模板值
    }
    
    if ( v->m_is_draw_border_radius ) { // 圆角
      gl_->box_color_radius.use();
      glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
      glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
    } else {
      gl_->box_color.use();
      glUniform1fv(gl_->box_color_uniform_draw_data, size, av_ctx_data_float_p(v));
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    glStencilFunc(GL_LEQUAL, ++m_stencil_ref_value, 0xFF); // 设置新参考值
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  }
  
  /**
   * @func draw_end_clip
   */
  void draw_end_clip(Box* v) {
    
    const uint size = SIZEOF(Box_CtxData);
    
    if ( m_root_stencil_ref_value == m_stencil_ref_value - 1 ) {
      m_root_stencil_ref_value = m_stencil_ref_value;
      glDisable(GL_STENCIL_TEST); // 禁止模板测试
    } else {
      // 恢复原模板值
      glStencilFunc(GL_LEQUAL, --m_stencil_ref_value, 0xFF); // 新参考值
      
      glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); // Test成功设置模板为参考值
      glBlendFunc(GL_ZERO, GL_ONE); // 禁止颜色输出
      
      if ( v->m_is_draw_border_radius ) { // 圆角
        gl_->box_color_radius.use();
        glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
      } else {
        gl_->box_color.use();
        glUniform1fv(gl_->box_color_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
      
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 打开颜色输出
    }
  }
  
  /**
   * @func draw_input
   */
  void draw_input(Input* v) {
    draw_begin_clip(v);
    
    Vec2 offset = v->input_text_offset();
    uint begin = v->m_data.cell_draw_begin;
    uint end = v->m_data.cell_draw_end;
    
    if ( begin != end ) {
      
      Color color = v->m_text_background_color.value;
      if ( color.a() ) {
        _inl2_gl(this)->draw_text_background(v, v->m_data, color, begin, end, offset);
      }
      
      // draw marked background
      begin = v->marked_cell_begin_; end = v->marked_cell_end_;
      if ( begin != end ) {
        _inl2_gl(this)->draw_text_background(v, v->m_data, v->marked_color_, begin, end, offset);
      }
      
      color = v->length() ? v->m_text_color.value : v->placeholder_color_;
      
      _inl2_gl(this)->draw_text(v, v, v->m_data, color, offset);
      
      // draw cursor
      if ( v->editing_ && v->cursor_twinkle_status_ ) {
        _inl2_gl(this)->draw_input_cursor(v, v->cursor_linenum_, offset);
      }
      
    } else {
      // draw cursor
      if ( v->editing_ && v->cursor_twinkle_status_ ) {
        _inl2_gl(this)->draw_input_cursor(v, v->cursor_linenum_, offset);
      }
    }
    draw_end_clip(v);
  }
  
};

void GLDraw::clear_screen(Color color) {
  glClearColor(color.r() / 255, color.g() / 255, color.b() / 255, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLDraw::draw(Box* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->solve(v);
  }
  if ( v->m_visible_draw ) {
    _inl2_gl(this)->draw_box(v);
  }
  
  v->visit();
}

void GLDraw::draw(Image* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
  }
  
  if ( v->m_visible_draw ) {
    
    const uint size = SIZEOF(Image_CtxData);
    
    if ( v->m_is_draw_border_radius ) { // 绘制圆角
      
      if ( v->m_texture->use() ||
          (v->m_background_image && v->m_background_image->use()) ) {
        gl_->box_image_radius.use(); // 使用圆角矩形纹理着色器
        glUniform1fv(gl_->box_image_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
      }
      else {
        if ( v->m_background_color.a() ) { // 绘制背景
          gl_->box_color_radius.use();
          glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
          glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
        }
      }
      
      if ( v->m_is_draw_border ) { // 绘制边框
        gl_->box_border_radius.use();
        glUniform1fv(gl_->box_border_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_radius_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_STRIP, 0, 84);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
        }
      }
    }
    else {
      
      if ( v->m_texture->use() ||
          (v->m_background_image && v->m_background_image->use()) ) {
        gl_->box_image.use(); // 使用矩形纹理着色器
        glUniform1fv(gl_->box_image_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
      else {
        if ( v->m_background_color.a() ) { // 绘制背景
          gl_->box_color.use();
          glUniform1fv(gl_->box_color_uniform_draw_data, size, av_ctx_data_float_p(v));
          glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
      }
      
      if ( v->m_is_draw_border ) { // 绘制边框
        gl_->box_border.use();
        glUniform1fv(gl_->box_border_uniform_draw_data, size, av_ctx_data_float_p(v));
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
        }
      }
    }
  } // end draw
  
  v->visit();
  
}

void GLDraw::draw(Video* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
  }
  
  if ( v->m_visible_draw ) {
    
    TextureYUV* tex = (TextureYUV*)v->m_texture;
    const uint size = SIZEOF(Image_CtxData);
    
    // Video忽略圆角的绘制
    
    if (v->m_status != PLAYER_STATUS_STOP &&
        v->m_status != PLAYER_STATUS_START && tex->use() ) {
      
      if ( tex->format() == PixelData::YUV420P ) {
        gl_->box_yuv420p_image.use();
        glUniform1fv(gl_->box_yuv420p_image_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
      else if (tex->format() == PixelData::YUV420SP) {
        gl_->box_yuv420sp_image.use();
        glUniform1fv(gl_->box_yuv420sp_image_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      } else {
        // TODO 咱不支持
      }
    }
    else {
      if ( v->m_background_image && v->m_background_image->use() ) { // 绘制背景图像
        gl_->box_image.use(); // 使用矩形纹理着色器
        glUniform1fv(gl_->box_image_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
      else if ( v->m_background_color.a() ) { // 绘制背景
        gl_->box_color.use();
        glUniform1fv(gl_->box_color_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
    }
    
    if ( v->m_is_draw_border ) { // 绘制边框
      gl_->box_border.use();
      glUniform1fv(gl_->box_border_uniform_draw_data, size, av_ctx_data_float_p(v));
      if ( m_library == DRAW_LIBRARY_GLES2 ) {
        if ( is_support_instanced() ) {
          glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
        } else {
          for (int i = 0; i < 4; i++) {
            glVertexAttrib1f(es2_->es2_box_border_in_instance_id, i);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
          }
        }
      } else {
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
      }
    }
  } // end draw
  
  v->visit();
}

void GLDraw::draw(Shadow* v) {
  
  uint mark_value = v->mark_value;
  if ( mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
    
    if ( mark_value & View::M_BOX_SHADOW ) {  // 阴影
      v->m_is_draw_shadow_box =
      v->m_shadow.offset_x != 0 || v->m_shadow.offset_y != 0 || v->m_shadow.size != 0;
      if ( v->m_is_draw_shadow_box ) { //  是否要显示阴影
        Shadow_CtxData* data = av_ctx_data(v, Shadow_CtxData);
        data->offset_x = v->m_shadow.offset_x;
        data->offset_y = v->m_shadow.offset_y;
        data->size = v->m_shadow.size;
        data->color = v->m_shadow.color.to_float_color();
      }
    }
  }
  
  if (v->m_visible_draw) {
    
    const uint size = SIZEOF(Shadow_CtxData);
    
    if ( v->m_is_draw_border_radius ) { // 圆角
      
      if ( v->m_background_color.a() ) { // 绘制背景
        gl_->box_color_radius.use();
        glUniform1fv(gl_->box_color_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 84);
      }
      
      if ( v->m_is_draw_border ) { // 绘制边框
        gl_->box_border_radius.use();
        glUniform1fv(gl_->box_border_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_radius_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_STRIP, 0, 84);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
        }
      }
      
      if ( v->m_is_draw_shadow_box ) {
        // draw shadow
      }
      
    } else {
      if ( v->m_background_color.a() ) { // 绘制背景
        gl_->box_color.use();
        glUniform1fv(gl_->box_color_uniform_draw_data, size, av_ctx_data_float_p(v));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
      
      if ( v->m_is_draw_border ) { // 绘制边框
        gl_->box_border.use();
        glUniform1fv(gl_->box_border_uniform_draw_data, size, av_ctx_data_float_p(v));
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
        }
      }
      
      if ( v->m_is_draw_shadow_box ) {
        // draw shadow
      }
    }
  } // end draw
  
  v->visit();
}

void GLDraw::draw(Sprite* v) {
  
  uint mark_value = v->mark_value;
  if ( mark_value ) {
    
    _inl2_gl(this)->new_ctx_data(v);
    
    Sprite_CtxData* data = av_ctx_data(v, Sprite_CtxData);
    
    if (mark_value & View::M_SHAPE) {
      
      data->vertex_ac[0].x( -v->m_origin.x() );
      data->vertex_ac[0].y( -v->m_origin.y() );
      data->vertex_ac[1].x( v->m_size.width() - v->m_origin.x() );
      data->vertex_ac[1].y( v->m_size.height() - v->m_origin.y() );
    }
    
    if ( mark_value & View::M_TEXTURE ) {
      v->m_texture->load();
      data->texture_start = v->m_start;
      data->texture_size.x(v->m_texture->width());
      data->texture_size.y(v->m_texture->height());
      data->texture_ratio = v->m_ratio;
    }
    
    if ( mark_value & View::M_TRANSFORM ) {
      data->view_matrix = v->m_final_matrix;
      data->opacity = v->m_final_opacity; // opacity
    }
    else {
      if ( mark_value & View::M_OPACITY ) {
        data->opacity = v->m_final_opacity; // opacity
      }
      if ( mark_value & View::M_TRANSFORM_AND_OPACITY_CTX_DATA ) {
        data->view_matrix = v->m_final_matrix;
        data->opacity = v->m_final_opacity; // opacity
      }
    }
  }
  
  if ( v->m_visible_draw ) { // 为false时不需要绘制
    if ( v->m_texture->use(v->m_repeat) ) {
      gl_->sprite.use();
      glUniform1fv(gl_->sprite_uniform_draw_data, SIZEOF(Sprite_CtxData), av_ctx_data_float_p(v));
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
  }
  
  v->visit();
}

void GLDraw::draw(TextNode* v) {
  
  if ( v->m_visible_draw ) {
    
    uint begin = v->m_data.cell_draw_begin;
    uint end = v->m_data.cell_draw_end;
    
    if ( begin != end ) {
      Vec2 offset = Vec2(-v->m_offset_start.x(), 0);
      Color color = v->m_text_background_color.value;
      if ( color.a() ) {
        _inl2_gl(this)->draw_text_background(v, v->m_data, color, begin, end, offset);
      }
      _inl2_gl(this)->draw_text(v, v, v->m_data, v->m_text_color.value, offset);
    }
  }
}

void GLDraw::draw(Label* v) {
  
  if ( v->m_visible_draw ) {
    
    uint begin = v->m_data.cell_draw_begin;
    uint end = v->m_data.cell_draw_end;
    
    if ( begin != end ) {
      Color color = v->m_text_background_color.value;
      if ( color.a() ) {
        _inl2_gl(this)->draw_text_background(v, v->m_data, color, begin, end, Vec2());
      }
      _inl2_gl(this)->draw_text(v, v, v->m_data, v->m_text_color.value, Vec2());
    }
  }
}

void GLDraw::draw(Text* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->solve(v);
  }
  
  if ( v->m_visible_draw ) {
    _inl2_gl(this)->draw_box(v);
    
    uint begin = v->m_data.cell_draw_begin;
    uint end = v->m_data.cell_draw_end;
    
    if ( begin != end ) {
      Color color = v->m_text_background_color.value;
      if ( color.a() ) {
        _inl2_gl(this)->draw_text_background(v, v->m_data, color, begin, end, Vec2());
      }
      _inl2_gl(this)->draw_text(v, v, v->m_data, v->m_text_color.value, Vec2());
    }
  }
}

void GLDraw::draw(Root* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->solve(v);
  }
  
  clear_screen(v->m_background_color);
  
  if ( v->m_visible_draw ) {
    
    const uint size = SIZEOF(Box_CtxData);
    
    if ( v->m_is_draw_border_radius ) { // 圆角
      if ( v->m_is_draw_border ) { // 绘制边框
        gl_->box_border_radius.use();
        glUniform1fv(gl_->box_border_radius_uniform_draw_data, size, av_ctx_data_float_p(v));
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_radius_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_STRIP, 0, 84);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 84, 4);
        }
      }
    } else {
      
      if ( v->m_is_draw_border ) { // 绘制边框
        gl_->box_border.use();
        glUniform1fv(gl_->box_border_uniform_draw_data, size, av_ctx_data_float_p(v));
        if ( m_library == DRAW_LIBRARY_GLES2 ) {
          if ( is_support_instanced() ) {
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
          } else {
            for (int i = 0; i < 4; i++) {
              glVertexAttrib1f(es2_->es2_box_border_in_instance_id, i);
              glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
          }
        } else {
          glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
        }
      }
    }
  } // end draw
  
  v->visit();
}

void GLDraw::draw(Scroll* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
  }
  
  uint inherit_mark = v->mark_value & View::M_INHERIT;
  
  if ( v->mark_value & Scroll::M_SCROLL ) {
    inherit_mark |= View::M_MATRIX;
  }
  
  if ( v->m_visible_draw ) {
    
    _inl2_gl(this)->draw_begin_clip(v);
    
    // 限制子视图绘图区域
    display_port()->push_draw_region(v->get_screen_region());
    v->visit( inherit_mark );
    display_port()->pop_draw_region(); // 弹出
    
    _inl2_gl(this)->draw_end_clip(v);
    _inl2_gl(this)->draw_scroll_bar(v, v); // 绘制scrollbar
    
  } else {// end draw
    v->visit( inherit_mark );
  }
}

void GLDraw::draw(Clip* v) {
  
  if ( v->mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
  }
  
  if ( v->m_visible_draw ) {
    
    _inl2_gl(this)->draw_begin_clip(v);
    
    // 限制子视图绘图区域
    display_port()->push_draw_region(v->get_screen_region());
    v->visit( v->mark_value & View::M_INHERIT );
    display_port()->pop_draw_region(); // 弹出
    
    _inl2_gl(this)->draw_end_clip(v);
    
  } else { // end draw
    v->visit( v->mark_value & View::M_INHERIT );
  }
}

void GLDraw::draw(Input* v) {
  
  uint mark_value = v->mark_value;
  
  if ( mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
  }
  
  if ( v->m_visible_draw ) {
    _inl2_gl(this)->draw_input(v);
  }
}

void GLDraw::draw(Textarea* v) {
  
  uint mark_value = v->mark_value;
  
  if ( mark_value ) {
    _inl2_gl(this)->new_ctx_data(v);
    _inl2_gl(this)->solve(v);
  }
  
  if ( v->m_visible_draw ) {
    _inl2_gl(this)->draw_input(v);
    _inl2_gl(this)->draw_scroll_bar(v, v); // 绘制scrollbar
  }
}
