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
#include "autil/http.h"
#include "draw.h"
#include "root.h"
#include "display-port.h"
#include "font.h"
#include "app-1.h"
#include "action.h"
#include "css.h"

int (*gui_main)(int, char**) = nullptr;

av_gui_begin

GUIApplication* GUIApplication::m_shared = nullptr;

void GUIApplication::Inl::post(Callback exec) {
  m_proxy->post(exec);
}

void GUIApplication::Inl::onLoad() {
  m_proxy->post(Callback([&](SimpleEvent& d) {
    m_is_load = true;
    av_trigger(load);
  }));
}

void GUIApplication::Inl::onRender() {
  m_display_port->render();
}

void GUIApplication::Inl::onPause() {
  m_proxy->post(Callback([&](SimpleEvent& d) { av_trigger(pause); }));
}

void GUIApplication::Inl::onResume() {
  m_proxy->post(Callback([&](SimpleEvent& d) { av_trigger(resume); }));
}

void GUIApplication::Inl::onBackground() {
  m_proxy->post(Callback([&](SimpleEvent& d) { av_trigger(background); }));
}

void GUIApplication::Inl::onForeground() {
  m_proxy->post(Callback([&](SimpleEvent& d) { av_trigger(foreground); }));
}

void GUIApplication::Inl::onMemorywarning() {
  m_proxy->post(Callback([&](SimpleEvent& d) {
    clear();
    av_trigger(memorywarning);
  }));
}

void GUIApplication::Inl::onUnload() {
  m_proxy->post(Callback([&](SimpleEvent& d) { av_trigger(unload); terminate(); }));
}

/**
 * @func set_root
 */
void GUIApplication::Inl::set_root(Root* value) av_def_err {
  av_assert_err(!m_root, "Root view already exists");
  m_root = value;
  m_root->retain();   // strong ref
  set_first_responder(value);
}

/**
 * @func set_first_responder
 */
void GUIApplication::Inl::set_first_responder(GUIResponder* responder) {
  if ( m_first_responder != responder ) {
    if ( m_first_responder ) {
      m_first_responder->release();
    }
    m_first_responder = responder;
    m_first_responder->retain(); // strong ref
    m_emitter->make_text_input(dynamic_cast<TextInput*>(responder));
  }
}

static Mutex      top_thread_lock_;
static Condition  top_thread_cond_;

void GUIApplication::Inl::start_up_gui_thread(int argc, char* argv[]) {
  
  // 创建一个新子UI线程.这里主线程不能阻塞,这个函数必须由main入口调用
  Thread::fork([argc, argv](Thread& t) {
    
    av_assert( gui_main );
    
    int (*main)(int, char**) = gui_main;
    gui_main = nullptr;
    
    int rc = main(argc, argv); // 运行这个自定gui入口函数
    
    exit(rc); // if sub thread end then exit
  }, "gui_main");
  
  { // 在调用GUIApplication::run()之前一直阻塞这个主线程
    std::unique_lock<Mutex> ul(top_thread_lock_);
    do {
      top_thread_cond_.wait(ul); // 等待线程调用run
    } while(!m_shared || !m_shared->m_is_run);
  }
}

/**
 * @func run
 */
void GUIApplication::run() av_def_err {
  av_assert_err(!m_is_run, "GUI program has been running");
  
  m_is_run = true;
  
  auto loop = RunLoop::current(); // 当前运行的消息队列
  if (loop) {
    m_proxy = loop->keep_alive(); // 保持
    
    { // 主UI线程继续运行
      std::lock_guard<Mutex> ul(top_thread_lock_);
      top_thread_cond_.notify_one();
    }
  } 
  else {
    RunLoop loop;
    loop.post(Callback([&](SimpleEvent& d) {
      m_proxy = loop.keep_alive(); // 保持
      
      { // 主线程继续运行
        std::lock_guard<Mutex> ul(top_thread_lock_);
        top_thread_cond_.notify_one();
      }
    }));
    loop.run_loop(-1);
  }
}

GUIApplication::GUIApplication(const Map<String, int>& option) av_def_err
: av_init_event(load)
, av_init_event(unload)
, av_init_event(background)
, av_init_event(foreground)
, av_init_event(pause)
, av_init_event(resume)
, av_init_event(memorywarning)
, m_is_run(false)
, m_is_load(false)
, m_proxy(nullptr)
, m_draw_ctx(nullptr)
, m_first_responder(nullptr)
, m_display_port(nullptr)
, m_root(nullptr)
, m_default_text_background_color({ TextArrtsType::VALUE, Color(0, 0, 0, 0) })
, m_default_text_color({ TextArrtsType::VALUE, Color(0, 0, 0) })
, m_default_text_size({ TextArrtsType::VALUE, 16 })
, m_default_text_style({ TextArrtsType::VALUE, TextStyle::REGULAR })
, m_default_text_family({ TextArrtsType::VALUE, FontPool::get_font_familys_id("") })
, m_default_text_shadow({ TextArrtsType::VALUE, { 0, 0, 0, Color(0, 0, 0) } })
, m_default_text_line_height({ TextArrtsType::VALUE, { true, 0 } })
, m_default_text_decoration({ TextArrtsType::VALUE, TextDecoration::NONE })
, m_default_text_overflow({ TextArrtsType::VALUE, TextOverflow::NORMAL })
, m_default_text_white_space({ TextArrtsType::VALUE, TextWhiteSpace::NORMAL })
, m_emitter(nullptr)
, m_action_center(nullptr)
{
  av_assert_err(!m_shared, "At the same time can only run a GUIApplication entity");
  m_shared = this;
  
  HttpHelper::initializ(); // 初始http
  
  Inl_GUIApplication(this)->initializ(option);
  m_display_port = NewRetain<DisplayPort>(this); // strong ref
  m_draw_ctx->font_pool()->bind_display_port(m_display_port);
  m_emitter = new GUIEventEmitter(this);
  m_action_center = new ActionCenter();
}

GUIApplication::~GUIApplication() {
  if ( m_proxy ) {
    Release(m_proxy); m_proxy = nullptr;
  }
  if (m_root) {
    m_root->release(); m_root = nullptr;
  }
  if ( m_first_responder ) {
    m_first_responder->release();
    m_first_responder = nullptr;
  }
  Release(m_draw_ctx);            m_draw_ctx = nullptr;
  Release(m_emitter);             m_emitter = nullptr;
  Release(m_action_center);       m_action_center = nullptr;
  Release(m_display_port);        m_display_port = nullptr;
  m_shared = nullptr;
}

/**
 * @func clear
 */
void GUIApplication::clear(bool full) {
  m_draw_ctx->clear(full); // 清理不需要使用的对像
}

/**
 * @func terminate
 */
void GUIApplication::terminate() {
  if (m_is_run) {
    Inl_GUIApplication(this)->terminate();
    Release(m_proxy);
    m_proxy = nullptr;
    m_is_run = false;
  }
}

/**
 * @func loop # GUI运行队列
 */
RunLoop* GUIApplication::loop() const {
  return m_proxy ? m_proxy->host() : nullptr;
}

void GUIApplication::set_default_text_background_color(ColorValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_background_color = value;
  }
}
void GUIApplication::set_default_text_color(ColorValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_color = value;
  }
}
void GUIApplication::set_default_text_size(TextSizeValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_size = value;
  }
}
void GUIApplication::set_default_text_style(TextStyleValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_style = value;
  }
}
void GUIApplication::set_default_text_family(TextFamilyValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_family = value;
  }
}
void GUIApplication::set_default_text_shadow(TextShadowValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_shadow = value;
  }
}
void GUIApplication::set_default_text_line_height(TextLineHeightValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_line_height = value;
  }
}
void GUIApplication::set_default_text_decoration(TextDecorationValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_decoration = value;
  }
}
void GUIApplication::set_default_text_overflow(TextOverflowValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_overflow = value;
  }
}
void GUIApplication::set_default_text_white_space(TextWhiteSpaceValue value) {
  if ( value.type == TextArrtsType::VALUE ) {
    m_default_text_white_space = value;
  }
}

av_gui_end
