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

#ifndef __avocado__gui__ios_ogl__
#define __avocado__gui__ios_ogl__

#include "autil/util.h"

#if av_ios

#include <UIKit/UIKit.h>
#include <OpenGLES/EAGL.h>
#include "../ogl/ogles2.h"

av_gui_begin

class IOSGLDrawCore {
public:
  IOSGLDrawCore(GLDraw* host, EAGLContext* ctx);
  ~IOSGLDrawCore();
  virtual void commit_render();
  virtual GLint get_ogl_texture_pixel_format(PixelData::Format pixel_format);
  virtual void gl_main_render_buffer_storage();
  void set_layer(CAEAGLLayer* layer);
  void refresh_screen_size();
  inline GLDraw* host() { return m_host; }
private:
  CAEAGLLayer* m_layer;
  EAGLContext* m_context;
  GLDraw*      m_host;
};

/**
 * @class IOSGLDraw
 */
template<class Basic> class IOSGLDraw: public Basic {
public:
  IOSGLDraw(EAGLContext* ctx,
            DrawLibrary library,
            const Map<String, int>& option): Basic(option), core_(this, ctx) {
    this->m_library = library;
    this->initializ();
  }
  
  virtual void commit_render() {
    core_.commit_render();
  }
  virtual GLint get_ogl_texture_pixel_format(PixelData::Format pixel_format) {
    return core_.get_ogl_texture_pixel_format(pixel_format);
  }
  virtual void gl_main_render_buffer_storage() {
    core_.gl_main_render_buffer_storage();
  }
  
  IOSGLDrawCore* core() { return &core_; }
  
private:
  IOSGLDrawCore core_;
};


av_gui_end

#endif
#endif
