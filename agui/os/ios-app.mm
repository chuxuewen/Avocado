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

#import <UIKit/UIKit.h>
#import "autil/thread.h"
#import "ios-ogl.h"
#import "../app.h"
#import "../display-port.h"
#import "../app-1.h"
#import "../event.h"
#import "ios-text-input.h"
#import <OpenGLES/ES2/glext.h>
#import "ios-app.h"

using namespace avocado;
using namespace avocado::gui;

@class ApplicationDelegate;
static ApplicationDelegate* app_g = nil;
static IOSGLDrawCore* draw_core_g = nil;
static NSString* app_delegate_name_g = @"ApplicationDelegate";

// ------------------------------ OGLView ------------------------------

@interface OGLView : UIView;
@property (assign, nonatomic) GUIApplication::Inl* app;
@property (assign, nonatomic) UIWindow* window;
@end

@implementation OGLView

+ (Class)layerClass {
  return [CAEAGLLayer class];
}

- (BOOL)isMultipleTouchEnabled {
  return YES;
}

- (BOOL)isUserInteractionEnabled {
  return YES;
}

- (List<GUITouch>)to_guitouch:(NSSet<UITouch*>*)touches {
  NSEnumerator* enumerator = [touches objectEnumerator];
  List<GUITouch> rv; // (uint(touches.count));
  
  Vec2 size = _app->display_port()->size();
  
  float scale_x = size.width() / _window.frame.size.width;
  float scale_y = size.height() / _window.frame.size.height;
  
  for (UITouch* touch in enumerator) {
    ::CGPoint point = [touch locationInView:touch.view];
    CGFloat force = touch.force;
    // CGFloat angle = touch.altitudeAngle;
    // CGFloat max_force = touch.maximumPossibleForce;
    rv.push({
      uint((size_t)touch % Uint::max), 0, 0,
      float(point.x * scale_x), float(point.y * scale_y), float(force), false, nullptr,
    });
  }
  
  return rv;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
  _app->emitter()->receive_touch_start( [self to_guitouch:touches] );
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
  _app->emitter()->receive_touch_move( [self to_guitouch:touches] );
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event{
  _app->emitter()->receive_touch_end( [self to_guitouch:touches] );
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
  _app->emitter()->receive_touch_cancel( [self to_guitouch:touches] );
}

@end

// ------------------------------ RootViewController ------------------------------

@interface RootViewController: UIViewController;
@end

@implementation RootViewController

- (BOOL)shouldAutorotate {
  return YES;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
  
  NSArray* allOrientation = [NSArray arrayWithObjects:
                      @"UIInterfaceOrientationPortrait",
                      @"UIInterfaceOrientationPortraitUpsideDown",
                      @"UIInterfaceOrientationLandscapeRight",
                      @"UIInterfaceOrientationLandscapeLeft",
                      nil];
  NSString* cur = [allOrientation objectAtIndex:interfaceOrientation - 1];
  NSDictionary* dic = [[NSBundle mainBundle] infoDictionary];
  NSArray* ls = [dic objectForKey:@"UISupportedInterfaceOrientations"];
  
  for (NSString* s in ls) {
    if ([cur compare:s] == NSOrderedSame) {
      return YES;
    }
  }
  return NO;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {

  UIView* view = [self view];
  NSArray* subviews = [view subviews];
  
  if ([subviews count]) {
  
    UIView* glview = [subviews objectAtIndex:0];
    [glview setFrame:[view bounds]];
    
    Inl_GUIApplication(GUIApplication::app())->post(Callback([](SimpleEvent& d) {
      draw_core_g->refresh_screen_size();
    }));
  }
}

@end

// ------------------------------ ApplicationDelegate ------------------------------

@interface ApplicationDelegate() {
  Mutex      _top_thread_lock;
  Condition  _top_thread_cond;
  Callback   _render_cb;
}
@property (strong, nonatomic) OGLView* glview;
@property (strong, nonatomic) IOSTextInput* input;
@property (assign, nonatomic) RunLoop* loop;
@property (strong, nonatomic) CADisplayLink* display_link;
@end

@implementation ApplicationDelegate

static void render_loop_cb(SimpleEvent& evt, Object* ctx) {
  { //
    std::lock_guard<Mutex> ul(app_g->_top_thread_lock);
    app_g->_top_thread_cond.notify_one();
  }
  _inl_app(app_g.app)->onRender();
}

- (void)render_loop:(CADisplayLink*)displayLink {
  std::unique_lock<Mutex> ul(_top_thread_lock);
  _inl_app(self.app)->post(_render_cb);
  _top_thread_cond.wait(ul);  // 调用onRender之前一直阻塞这个主线程
}

- (BOOL)application:(UIApplication*)app didFinishLaunchingWithOptions:(NSDictionary*)options {
  av_assert(!app_g); app_g = self;
  
  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  self.window.backgroundColor = [UIColor blackColor];
  self.window.rootViewController = [[RootViewController alloc] init];
  
  [self.window makeKeyAndVisible];
  //[app setStatusBarHidden:YES withAnimation:UIStatusBarAnimationSlide];
  
  UIView* view = [[_window subviews] objectAtIndex:0];
  self.glview = [[OGLView alloc] initWithFrame:[view bounds]];
  self.glview.contentScaleFactor = UIScreen.mainScreen.scale;
  
  _app = Inl_GUIApplication(GUIApplication::app());
  self.loop = self.app->loop(); av_assert(self.loop);
  self.glview.app = _inl_app(self.app);
  self.glview.window = self.window;
  
  self.input = [[IOSTextInput alloc] initWithApplication:self.app];
  
  [view addSubview:self.glview];
  [view addSubview:self.input];
  
  _render_cb = Callback(render_loop_cb);
  
  self.display_link = [CADisplayLink displayLinkWithTarget:self
                                                  selector:@selector(render_loop:)];
  _inl_app(self.app)->post(Callback([self](SimpleEvent& d) {
    draw_core_g->set_layer((CAEAGLLayer*)self.glview.layer);
    _inl_app(self.app)->onLoad();
    [self.display_link addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
  }));
  
  return YES;
}

- (void)applicationWillResignActive:(UIApplication*) application {
  _inl_app(_app)->onPause();
}

- (void)applicationDidBecomeActive:(UIApplication*) application {
  _inl_app(_app)->onResume();
}

- (void)applicationDidEnterBackground:(UIApplication*) application {
  _inl_app(_app)->onBackground();
}

- (void)applicationWillEnterForeground:(UIApplication*) application {
  _inl_app(_app)->onForeground();
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication*) application {
  _inl_app(_app)->onMemorywarning();
}

- (void)applicationWillTerminate:(UIApplication*)application {
  [self.display_link removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
  _inl_app(_app)->onUnload();
}

+ (void)set_start_application_delegate:(NSString*)name {
  app_delegate_name_g = name;
}

@end

// ------------------------------ GUIApplication::Inl ------------------------------

typedef GUIApplication::Inl AppInl;

void AppInl::initializ(const Map<String, int>& option) {
  av_assert(!draw_core_g);
  
  EAGLContext* ctx = [EAGLContext alloc];
  
  if ( [ctx initWithAPI:kEAGLRenderingAPIOpenGLES3] ) {
    draw_core_g = (new IOSGLDraw<GLDraw>(ctx, DRAW_LIBRARY_GLES3, option))->core();
  } else if ( [ctx initWithAPI:kEAGLRenderingAPIOpenGLES2] ) {
    draw_core_g = (new IOSGLDraw<GLES2Draw>(ctx, DRAW_LIBRARY_GLES2, option))->core();
  } else {
    av_fatal("Unable to initialize OGL device does not support OpenGLES");
  }
  m_draw_ctx = draw_core_g->host();
}

void GUIApplication::Inl::terminate() {
  // android / ios 应用启动后都没有提供正常关闭的方法
  // 系统会自动清理长时间休眠的程序
}

/**
 * @func input_keyboard_open
 */
void AppInl::input_keyboard_open(KeyboardOptions options) {
  dispatch_async(dispatch_get_main_queue(), ^{
    [app_g.input becomeFirstResponder];
    [app_g.input input_keyboard_type:options.type];
    [app_g.input input_keyboard_return_type:options.return_type];
    if ( options.is_clear ) {
      [app_g.input clear];
    }
  });
}

/**
 * @func input_keyboard_can_back_space
 */
void AppInl::input_keyboard_can_back_space(bool can_back_space, bool can_delete) {
  dispatch_async(dispatch_get_main_queue(), ^{
    [app_g.input input_keyboard_can_back_space:can_back_space];
  });
}

/**
 * @func input_keyboard_close
 */
void AppInl::input_keyboard_close() {
  dispatch_async(dispatch_get_main_queue(), ^{
    [app_g.input resignFirstResponder];
  });
}

extern "C" {
  int main(int argc, char* argv[]) {
    GUIApplication::Inl::start_up_gui_thread(argc, argv);
    
    @autoreleasepool {
      UIApplicationMain(0, nil, nil, app_delegate_name_g);
    }
    return 0;
  }
}
