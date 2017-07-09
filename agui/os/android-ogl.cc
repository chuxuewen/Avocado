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

#include "../app.h"
#include "android-ogl.h"
#include "android/avocado.h"

av_gui_begin

#define GL_ETC1_RGB8_OES  0x8D64

static EGLDisplay g_display = EGL_NO_DISPLAY;
static EGLConfig g_config = NULL; // match required configuration by `attribs`

AndroidGLDrawCore::AndroidGLDrawCore(EGLContext context)
: m_window(NULL) 
, m_display( egl_display() )
, m_context(context)
, m_surface(EGL_NO_SURFACE) {

}

AndroidGLDrawCore::~AndroidGLDrawCore() {
  terminate();
}

void AndroidGLDrawCore::terminate() {
  if ( m_display != EGL_NO_DISPLAY ) {
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if ( m_context != EGL_NO_CONTEXT ) {
      eglDestroyContext(m_display, m_context);
    }
    if ( m_surface != EGL_NO_SURFACE ) {
      eglDestroySurface(m_display, m_surface);
    }
    
    eglTerminate(m_display);
  }
  g_display = EGL_NO_DISPLAY;
  g_config = NULL;
  m_display = EGL_NO_DISPLAY;
  m_context = EGL_NO_CONTEXT;
  m_surface = EGL_NO_SURFACE;
  m_window = NULL;
}

EGLDisplay AndroidGLDrawCore::egl_display() {
  if ( g_display == EGL_NO_DISPLAY ) { // get display and init it
    g_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    av_assert_err(g_display != EGL_NO_DISPLAY, "Unable to eglGetDisplay");
    EGLBoolean displayState = eglInitialize(g_display, NULL, NULL);
  }
  return g_display;
}

EGLConfig AndroidGLDrawCore::egl_config() {
  if ( g_config ) 
    return g_config;
  
  EGLDisplay display = egl_display();
  
  // choose configuration
  const EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 0,
    // EGL_SAMPLE_BUFFERS, 1,
    // EGL_SAMPLES, 4,
    EGL_NONE
  };
  
  EGLint numConfigs = 0; // number of frame buffer configurations
  
  // first we get size of all configurations
  EGLBoolean chooseConfigState = eglChooseConfig(display, attribs, NULL, 0, &numConfigs);

  av_assert_err(numConfigs != 0, "We can't have EGLConfig array with zero size!");
  
  // then we create array large enough to store all configs
  ArrayBuffer<EGLConfig> supportedConfigs(numConfigs);
  
  // and load them
  chooseConfigState = eglChooseConfig(display, attribs, *supportedConfigs, numConfigs, &numConfigs);
  av_assert_err(numConfigs != 0, "Value of `numConfigs` must be positive");
  
  EGLint configIndex = 0;
  for ( ; configIndex < numConfigs; configIndex++ ) {

    EGLConfig& cfg = supportedConfigs[configIndex];

    EGLint r, g ,b, a, d;
    
    bool hasMatch =  eglGetConfigAttrib(display, cfg, EGL_RED_SIZE,   &r) && r == 8
                  && eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) && g == 8
                  && eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE,  &b) && b == 8
                  && eglGetConfigAttrib(display, cfg, EGL_ALPHA_SIZE, &a) && a == 8
                  && eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) && d == 0;
    if ( hasMatch ) {
      g_config = supportedConfigs[configIndex];
      break;
    }
  }
  
  // if we don't find anything choose first one
  if ( configIndex == numConfigs ) {
    g_config = supportedConfigs[0];
  }
  
  return g_config;
}

void AndroidGLDrawCore::set_window(ANativeWindow* window) {
  av_assert( window && ! m_window );
  m_window = window;

  EGLConfig config = egl_config();
  
  // EGLint format = 0;
  // EGLBoolean nativeVisualStatus = eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
  // ANativeWindow_setBuffersGeometry(window, 0, 0, format);

  m_surface = eglCreateWindowSurface(m_display, config, m_window, nullptr);
  av_assert_err( m_surface , "Unable to create a drawing surface");
  
  EGLBoolean isCurrent = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
  av_assert( isCurrent );
  
  draw_ctx()->initializ();

  set_best_display_scale_core(Avocado::instance()->display_scale());
  change_size();
}

void AndroidGLDrawCore::change_size() {
  EGLint width = 0;
  EGLint height = 0;
  eglQuerySurface(m_display, m_surface, EGL_WIDTH, &width);
  eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &height);
  
  float w = width;
  float h = height;
  
  draw_ctx()->update_screen_size(w, h);
}

// ---------  AndroidGLDraw ----------

void AndroidGLDraw::commit_render() {
  GLDraw::commit_render();
  eglSwapBuffers(m_display, m_surface);
}

GLint AndroidGLDraw::get_ogl_texture_pixel_format(PixelData::Format pixel_format) {
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
    // compressd
    case PixelData::ETC1:
    case PixelData::ETC2_RGB: return GL_COMPRESSED_RGB8_ETC2;
    case PixelData::ETC2_RGB_A1:
    case PixelData::ETC2_RGBA: return GL_COMPRESSED_RGBA8_ETC2_EAC;
    default: return 0;
  }
  // #define GL_COMPRESSED_RGB8_ETC2           0x9274
  // #define GL_COMPRESSED_SRGB8_ETC2          0x9275
  // #define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
  // #define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
  // #define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
  // #define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
}

// ---------  AndroidGLES2Draw ----------

void AndroidGLES2Draw::commit_render() {
  GLES2Draw::commit_render();
  eglSwapBuffers(m_display, m_surface);
}

GLint AndroidGLES2Draw::get_ogl_texture_pixel_format(PixelData::Format pixel_format) {
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
    // compressd
    case PixelData::ETC1: 
      return is_support_compressed_ETC1() ? GL_ETC1_RGB8_OES : 0;
      // return is_support_compressed_ETC1() ? GL_COMPRESSED_RGB8_ETC2 : 0;
    default: return 0;
  }
}

av_gui_end