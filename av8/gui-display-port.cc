/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, Louis.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

#include "gui.h"
#include "agui/display-port.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

static cString change("change");
static cString beforerender("beforerender");
static cString render("render");

class WrapDisplayPort: public WrapBase {
public: typedef DisplayPort Type;
  
  /**
   * @func bind_event
   */
  bool add_event_listener(cString& name, cString& func, int id) {
    if ( name == change ) {
      self<Type>()->av8_bind_common_native_event(change);
    }
    //else if ( name == beforerender ) {
    //  self<Type>()->av8_bind_common_native_event(beforerender);
    //}
    else if ( name == render ) {
      self<Type>()->av8_bind_common_native_event(render);
    } else {
      return false;
    }
    return true;
  }
  
  /**
   * @func unbind_event
   */
  bool remove_event_listener(cString& name, int id) {
    if ( name == change ) {
      self<Type>()->av8_unbind_native_event(change);
    }
    //else if ( name == beforerender ) {
    //  self<Type>()->av8_unbind_native_event(beforerender);
    //}
    else if ( name == render ) {
      self<Type>()->av8_unbind_native_event(render);
    } else {
      return false;
    }
    return true;
  }
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_worker(args);
    av8_throw_err("Forbidden access");
  }

  /**
   * @func lock_size([width[,height]])
   * @arg [width=0] {float}
   * @arg [height=0] {float}
   *
   * width与height都设置为0时自动设置一个最舒适的默认显示尺寸
   *
   * 设置锁定视口为一个固定的逻辑尺寸,这个值改变时会触发change事件
   *
   * 如果width设置为零表示不锁定宽度,系统会自动根据height值设置一个同等比例的宽度
   * 如果设置为非零表示锁定宽度,不管display_port_size怎么变化对于编程者来说,这个值永远保持不变
   *
   * 如果height设置为零表示不锁定,系统会自动根据width值设置一个同等比例的高度
   * 如果设置为非零表示锁定高度,不管display_port_size怎么变化对于编程者来说,这个值永远保持不变
   *
   */
  static void lock_size(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() < 1 || !args[0]->IsNumber() ) {
      av8_throw_err(
        "* @func lock_size([width[,height]])"
        "* @arg [width=0] {float}"
        "* @arg [height=0] {float}"
      );
    }
    av8_self(DisplayPort);
    
    if (args.Length() > 1 && args[1]->IsNumber()) {
      self->lock_size( args[0]->ToNumber()->Value(), args[1]->ToNumber()->Value() );
    } else {
      self->lock_size( args[0]->ToNumber()->Value() );
    }
  }

  /**
   * @func next_frame(cb)
   * @arg cb {Function}
   */
  static void next_frame(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || !args[0]->IsFunction()) {
      av8_throw_err(
        "* @func next_frame(cb)\n"
        "* @arg cb {Function}\n"
      );
    }
    av8_self(DisplayPort);
    
    PersistentFunc cb(worker->isolate(), args[0].As<v8::Function>());
    
    self->next_frame(Callback([cb, worker](SimpleEvent& evt) {
      worker->call(cb);
    }));
  }
  
  /**
   * @get width {float} 
   */
  static void width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->size().width() );
  }
  
  /**
   * @get height {float} 
   */
  static void height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->size().height() );
  }
  
  /**
   * @get phy_width {float} 
   */
  static void phy_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->phy_size().width() );
  }
  
  /**
   * @get phy_height {float} 
   */
  static void phy_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->phy_size().height() );
  }
  
  /**
   * @get best_scale {float} 
   */
  static void best_scale(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->best_scale() );
  }
  
  /**
   * @get scale {float} 
   */
  static void scale(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->scale() );
  }
  
  /**
   * @get scale_width {float} 
   */
  static void scale_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->scale_width() );
  }
  
  /**
   * @get scale_height {float} 
   */
  static void scale_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->scale_height() );
  }
  
  /**
   * @get root_matrix {Mat4} 
   */
  static void root_matrix(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( worker->gui_value_program()->New(self->root_matrix()) );
  }
  
  /**
   * @get atom_px {float} 
   */
  static void atom_px(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(DisplayPort);
    av8_return( self->atom_px() );
  }
  
  /**
   * @static default_atom_px {float} 
   */
  static void default_atom_px(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_return( DisplayPort::default_atom_px() );
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(DisplayPort, constructor, {
      av8_set_property(default_atom_px, default_atom_px);
      av8_set_cls_method(lock_size, lock_size);
      av8_set_cls_method(next_frame, next_frame);
      av8_set_cls_property(width, width);
      av8_set_cls_property(height, height);
      av8_set_cls_property(phy_width, phy_width);
      av8_set_cls_property(phy_height, phy_height);
      av8_set_cls_property(best_scale, best_scale);
      av8_set_cls_property(scale, scale);
      av8_set_cls_property(scale_width, scale_width);
      av8_set_cls_property(scale_height, scale_height);
      av8_set_cls_property(root_matrix, root_matrix);
      av8_set_cls_property(atom_px, atom_px);
    }, NULL);
  }
};

void binding_display(Local<v8::Object> exports, Worker* worker) {
  WrapDisplayPort::binding(exports, worker);
}

av8_nsd
