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

#include "autil/thread.h"
#include "android/avocado.h"
#include "../app-inl.h"
#include "../display-port.h"
#include "../event.h"
#include "android-ogl.h"
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>

av_gui_begin

class AndroidApplication;
static AndroidApplication* android_app = NULL;

class AndroidApplication: public Object {
public:
  AndroidApplication(ANativeActivity* activity, void* saved_state, size_t saved_state_size)
  : m_activity(activity)
  , m_saved_state(saved_state)
  , m_saved_state_size(saved_state_size)
  , m_window(nullptr)
  , m_app( nullptr )
  , m_loop( nullptr )
  , m_ctx_cote( nullptr )
  , m_is_render(false)
  , m_queue(nullptr)
  , m_looper(nullptr)
  , m_distribute( nullptr )
  {
    av_assert(!android_app);

    android_app = this;

    m_looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    
    activity->callbacks->onDestroy                  = &AndroidApplication::onDestroy;
    activity->callbacks->onStart                    = &AndroidApplication::onStart;
    activity->callbacks->onResume                   = &AndroidApplication::onResume;
    activity->callbacks->onSaveInstanceState        = &AndroidApplication::onSaveInstanceState;
    activity->callbacks->onPause                    = &AndroidApplication::onPause;
    activity->callbacks->onStop                     = &AndroidApplication::onStop;
    activity->callbacks->onConfigurationChanged     = &AndroidApplication::onConfigurationChanged;
    activity->callbacks->onLowMemory                = &AndroidApplication::onLowMemory;
    activity->callbacks->onWindowFocusChanged       = &AndroidApplication::onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated      = &AndroidApplication::onNativeWindowCreated;
    activity->callbacks->onNativeWindowResized      = &AndroidApplication::onNativeWindowResized;
    activity->callbacks->onNativeWindowRedrawNeeded = &AndroidApplication::onNativeWindowRedrawNeeded;
    activity->callbacks->onNativeWindowDestroyed    = &AndroidApplication::onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated        = &AndroidApplication::onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed      = &AndroidApplication::onInputQueueDestroyed;
    activity->callbacks->onContentRectChanged       = &AndroidApplication::onContentRectChanged;
    activity->instance = this;
  }
  
  virtual ~AndroidApplication() { }
  
  void change_size() {
    m_app->post([this]() { m_ctx_cote->change_size(); });
  }

  void render_loop() {
    if ( m_is_render ) {
      m_loop->post([&]() { render_loop(); }, 1000.0 / 60.0 * 1000); // 60fsp
      m_app->onRender();
    }
  }

  void terminate_application() {
    std::unique_lock<Mutex> ul(m_lock);
    if (m_is_render) {
      m_app->post([&]() {
        m_is_render = false; // stop draw
        m_ctx_cote->terminate();
        m_cond.notify_one();
      });
      m_cond.wait(ul); // wait
    }
  }
  
  static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    av_assert(!android_app->m_window);
    android_app->m_window = window;

    android_app->m_app->post([window]() {
      android_app->m_ctx_cote->set_window(window);
      android_app->m_app->onLoad();
      android_app->m_is_render = true;
      android_app->render_loop(); // start render
    });
  }
  
  static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    android_app->terminate_application();
  }
  
  static void onDestroy(ANativeActivity* activity) {
    android_app->terminate_application();
    android_app->m_app->onUnload();
    { delete android_app;
      android_app = nullptr;
    }
  }
  
  static void onStart(ANativeActivity* activity) {

    if ( android_app->m_app == nullptr ) { // start gui

      GUIApplication::Inl::start_sub_gui_thread(0, nullptr); // start sub gui thread

      android_app->m_app = Inl_GUIApplication(app());
      android_app->m_loop = android_app->m_app->gui_run_loop();
      android_app->m_ctx_cote = dynamic_cast<AndroidGLDrawCore *>(draw_ctx());
      android_app->m_distribute = android_app->m_app->distribute();

      av_assert(android_app->m_ctx_cote);
      av_assert(android_app->m_activity);
      av_assert(android_app->m_app);
      av_assert(android_app->m_loop);
    }

    android_app->m_app->onForeground();
  }
  
  static void onResume(ANativeActivity* activity) {
    android_app->m_app->onResume();
  }
  
  static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    return nullptr;
  }
  
  static void onPause(ANativeActivity* activity) {
    android_app->m_app->onPause();
  }
  
  static void onStop(ANativeActivity* activity) {
    android_app->m_app->onBackground();
  }
  
  static void onConfigurationChanged(ANativeActivity* activity) {
    
  }
  
  static void onLowMemory(ANativeActivity* activity) {
    android_app->m_app->onMemorywarning();
  }
  
  static void onWindowFocusChanged(ANativeActivity* activity, int hasFocus) {
    
  }
  
  static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    android_app->change_size();
  }
  
  static void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window) {
    
  }

  static int onInputEvent_callback(int fd, int events, AInputQueue* queue) {
    AInputEvent* event = nullptr;
    GUIEventDistribute* distribute = android_app->m_distribute;

    while( AInputQueue_getEvent(queue, &event) >= 0 ) {

      if ( AInputQueue_preDispatchEvent(queue, event) == 0 ) {
        int type = AInputEvent_getType(event);
        int device = AInputEvent_getDeviceId(event);
        int source = AInputEvent_getSource(event);

        switch ( type ) {
          case AINPUT_KEYBOARD_TYPE_NONE:
            av_debug("AINPUT_KEYBOARD_TYPE_NONE");
            break;
          case AINPUT_KEYBOARD_TYPE_NON_ALPHABETIC:
          case AINPUT_KEYBOARD_TYPE_ALPHABETIC: {

            int code = AKeyEvent_getKeyCode(event);
            int repeat = AKeyEvent_getRepeatCount(event);

            switch ( AKeyEvent_getAction(event) ) {
              case AKEY_EVENT_ACTION_DOWN:
                distribute->key_down(code, repeat, device, source);
                break;
              case AKEY_EVENT_ACTION_UP:
                distribute->key_up(code, repeat, device, source);
                break;
              case AKEY_EVENT_ACTION_MULTIPLE:
                av_debug("AKEY_EVENT_ACTION_MULTIPLE");
                break;
            }
            break;
          }
        }
        AInputQueue_finishEvent(queue, event, fd);
      }
    }
    return 1;
  }

  static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    if ( queue != android_app->m_queue ) {
      if ( android_app->m_queue ) {
        AInputQueue_detachLooper(android_app->m_queue);
      }
      AInputQueue_attachLooper(queue, android_app->m_looper,
                               ALOOPER_POLL_CALLBACK,
                               (ALooper_callbackFunc) &onInputEvent_callback, queue);
      android_app->m_queue = queue;
    }
  }
  
  static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    AInputQueue_detachLooper(queue);
    android_app->m_queue = NULL;
  }
  
  static void onContentRectChanged(ANativeActivity* activity, const ARect* rect) {
    android_app->change_size();
  }

private:
  ANativeActivity* m_activity;
  void* m_saved_state;
  size_t m_saved_state_size;
  ANativeWindow* m_window;
  GUIApplication::Inl* m_app;
  ThreadRunLoop* m_loop;
  AndroidGLDrawCore* m_ctx_cote;
  bool  m_is_render;
  Mutex m_lock;
  Condition m_cond;
  AInputQueue* m_queue;
  ALooper*     m_looper;
  GUIEventDistribute* m_distribute;
};

void GUIApplication::Inl::initializ() {
  
  EGLDisplay display = AndroidGLDraw::egl_display();
  EGLConfig config = AndroidGLDraw::egl_config();

  EGLint ctx_attrs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 3,  // opengl es 3
    EGL_NONE
  };
  
  EGLContext context = NULL; // eglCreateContext(display, config, NULL, ctx_attrs);
  if ( context ) {
    m_draw_ctx = new AndroidGLDraw(context);
  } else {
    ctx_attrs[1] = 2; // opengl es 2
    context = eglCreateContext(display, config, NULL, ctx_attrs);
    av_assert(context);
    m_draw_ctx = new AndroidGLES2Draw(context);
  }
}

void GUIApplication::Inl::terminate() {
  // android / ios 应用启动后都没有提供正常关闭的方法
  // 系统会自动清理长时间休眠的程序
}

extern "C" {
  
  void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize)
  {
    if ( android_app ) {
      av_fatal("Error");
    } else {
      new AndroidApplication(activity, savedState, savedStateSize);
    }
  }
}

av_gui_end