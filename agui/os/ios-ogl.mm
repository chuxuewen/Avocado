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

#include "ios-ogl.h"
#include "../app.h"
#include "../ogl/ogles2.h"
#include "../display-port.h"
#include <OpenGLES/ES2/glext.h>

av_gui_begin

IOSGLDrawCore::IOSGLDrawCore(GLDraw* host, EAGLContext* ctx): m_host(host), m_context(ctx) {
  if ( ![EAGLContext setCurrentContext:ctx] ) {
    av_fatal("Failed to set current OpenGL context");
  }
  ctx.multiThreaded = YES;
}

IOSGLDrawCore::~IOSGLDrawCore() {
  [EAGLContext setCurrentContext:nullptr];
}

void IOSGLDrawCore::gl_main_render_buffer_storage() {
  // Create the color renderbuffer and call the rendering context to allocate the storage
  // on our Core Animation layer.
  // The width, height, and format of the renderbuffer storage are derived from the bounds
  // and properties of the CAEAGLLayer object
  // at the moment the renderbufferStorage:fromDrawable: method is called.
  [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:m_layer];
}

void IOSGLDrawCore::commit_render() {
  
  glBindVertexArray(0); // clear vao
  
  if ( m_host->multisample() > 1 ) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_host->m_msaa_frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_host->m_frame_buffer);
    
    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT, };
    
    if ( m_host->library() == DRAW_LIBRARY_GLES2 ) {
      glResolveMultisampleFramebufferAPPLE();
      glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER, 3, attachments);
    } else {
      glBlitFramebuffer(0, 0, m_host->screen_width(), m_host->screen_height(),
                        0, 0, m_host->screen_width(), m_host->screen_height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 3, attachments);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_host->m_frame_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_host->m_frame_buffer);
  } else {
    GLenum attachments[] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT, };
    
    if ( m_host->library() == DRAW_LIBRARY_GLES2 ) {
      glDiscardFramebufferEXT(GL_FRAMEBUFFER, 2, attachments);
    } else {
      glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
    }
  }
  
  // Assuming you allocated a color renderbuffer to point at a Core Animation layer,
  // you present its contents by making it the current renderbuffer
  // and calling the presentRenderbuffer: method on your rendering context.
  [m_context presentRenderbuffer:GL_FRAMEBUFFER];
}

/**
 * @func get_ogl_texture_pixel_format 获取当前环境对应的OpenGL纹理像素格式,如果返回0表示不支持纹理格式
 */
GLint IOSGLDrawCore::get_ogl_texture_pixel_format(PixelData::Format pixel_format) {
  switch (pixel_format) {
    case PixelData::RGBA4444:
    case PixelData::RGBX4444:
    case PixelData::RGBA5551:
    case PixelData::RGBA8888:
    case PixelData::RGBX8888: return GL_RGBA;
    case PixelData::RGB565:
    case PixelData::RGB888: return GL_RGB;
    case PixelData::ALPHA8: return GL_ALPHA;
    case PixelData::LUMINANCE8: return GL_LUMINANCE;
    case PixelData::LUMINANCE_ALPHA88: return GL_LUMINANCE_ALPHA;
      // compressd texture
    case PixelData::PVRTCI_2BPP_RGB: return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    case PixelData::PVRTCI_2BPP_RGBA:
    case PixelData::PVRTCII_2BPP: return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case PixelData::PVRTCI_4BPP_RGB: return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    case PixelData::PVRTCI_4BPP_RGBA:
    case PixelData::PVRTCII_4BPP: return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    case PixelData::ETC1:
    case PixelData::ETC2_RGB: return GL_COMPRESSED_RGB8_ETC2;
    case PixelData::ETC2_RGB_A1:
    case PixelData::ETC2_RGBA: return GL_COMPRESSED_RGBA8_ETC2_EAC;
    default: return 0;
  }
}

void IOSGLDrawCore::set_layer(CAEAGLLayer* layer) {
  m_layer = layer;
  m_layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                [NSNumber numberWithBool: NO],
                                kEAGLDrawablePropertyRetainedBacking,
                                kEAGLColorFormatRGBA8,
                                kEAGLDrawablePropertyColorFormat, nil];
  m_layer.opaque = YES;
  m_host->set_best_display_scale(UIScreen.mainScreen.scale);
  refresh_screen_size();
}

void IOSGLDrawCore::refresh_screen_size() {
  m_host->set_screen_size(m_layer.frame.size.width * UIScreen.mainScreen.scale,
                          m_layer.frame.size.height * UIScreen.mainScreen.scale);
}

/**
 * @func default_atom_px
 */
float DisplayPort::default_atom_px() {
  return 1.0 / UIScreen.mainScreen.scale;
}

av_gui_end
