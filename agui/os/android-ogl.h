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

#ifndef __avocado__gui__android_ogl__
#define __avocado__gui__android_ogl__

#include "autil/util.h"

#if av_android

#include "../ogl/ogles2.h"
#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

av_gui_begin

class AndroidGLDrawCore {
public:
  AndroidGLDrawCore(EGLContext context);
  virtual ~AndroidGLDrawCore();
  void set_window(ANativeWindow* window);
  void change_size();
  void terminate();
  virtual GLDraw* draw_ctx() = 0;
  virtual void set_best_display_scale_core(float value) = 0;
  static EGLDisplay egl_display();
  static EGLConfig egl_config();
protected:
  ANativeWindow* m_window;
  EGLDisplay m_display;
  EGLContext m_context;
  EGLSurface m_surface;
};

/**
 * @class AndroidGLDraw
 */
class AndroidGLDraw: public GLDraw, public AndroidGLDrawCore {
 public:
  inline AndroidGLDraw(EGLContext ctx): AndroidGLDrawCore(ctx) { set_version(GLES3); }
  virtual GLDraw* draw_ctx() { return this; }
  virtual void set_best_display_scale_core(float value) { set_best_display_scale(value); }
  virtual void commit_render();
  virtual GLint get_ogl_texture_pixel_format(PixelData::Format pixel_format);
};

/**
 * @class AndroidGLES2Draw
 */
class AndroidGLES2Draw: public GLES2Draw, public AndroidGLDrawCore {
public:
  inline AndroidGLES2Draw(EGLContext ctx): AndroidGLDrawCore(ctx) { set_version(GLES2); }
  virtual GLDraw* draw_ctx() { return this; }
  virtual void set_best_display_scale_core(float value) { set_best_display_scale(value); }
  virtual void commit_render();
  virtual GLint get_ogl_texture_pixel_format(PixelData::Format pixel_format);
};

av_gui_end

#endif
#endif