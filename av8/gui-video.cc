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

#include "event-1.h"
#include "gui.h"
#include "agui/video.h"
#include "gui-player.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

/**
 * @class WrapVideo
 */
class WrapVideo: public BasicWrapView {
public:
  
  template<class T>
  void add_event_listener_(const GUIEventName& name, cString& func, int id) {
    self<Video>()->on(name, [this, func]( GUIEvent& evt ) {
      v8::HandleScope scope(worker()->isolate());
      // arg event
      Wrap<GUIEvent>* ev = Wrap<GUIEvent>::wrap(&evt);
      
      ev->set_data(EventDataCast::entity<T>());
      
      Local<v8::Value> args[2] = { ev->local(), worker()->New(true) };
      // call js trigger func
      call( _worker->NewStringAscii(func), 2, args );
    }, id);
  }
  
  /**
   * @func overwrite
   */
  virtual bool add_event_listener(cString& name, cUcs2String& func, int id) {
    
    if ( BasicWrapView::add_event_listener(name, func, id) ) {
      return true;
    }
    auto i = GUI_EVENT_PLAYER_TABLE.find(name);
    if ( i == GUI_EVENT_PLAYER_TABLE.end() ) {
      return false;
    }
    
    if ( i.value() == GUI_EVENT_PLAYER_WAIT_BUFFER ) { // Float
      add_event_listener_<Float>(i.value(), func, id);
    } else if ( i.value() == GUI_EVENT_PLAYER_ERROR ) { // Error
      add_event_listener_<Error>(i.value(), func, id);
    } else if ( i.value() == GUI_EVENT_PLAYER_SEEK ) { // Uint64
      add_event_listener_<Uint64>(i.value(), func, id);
    } else { // object
      add_event_listener_<Object>(i.value(), func, id);
    }
    
    return true;
  }
  
  virtual bool remove_event_listener(cString& name, int id) {
    
    if ( BasicWrapView::remove_event_listener(name, id) ) {
      return true;
    }
    auto i = GUI_EVENT_PLAYER_TABLE.find(name);
    if ( i == GUI_EVENT_PLAYER_TABLE.end() ) {
      return false;
    }
    // off event listener
    self<Video>()->off(i.value(), id);
    
    return true;
  }
  
  static void constructor(FunctionCall args) {
    av8_external(args);
    av8_check_gui_app();
    New<WrapVideo>(args, new Video());
  }
  
  static void auto_play(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->auto_play() );
  }
  
  static void set_auto_play(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Video);
    self->set_auto_play( value->ToBoolean()->IsTrue() );
  }
  
  static void source_status(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->source_status() );
  }
  
  static void status(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->status() );
  }
  
  static void mute(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->mute() );
  }
  
  static void set_mute(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Video);
    self->set_mute( value->ToBoolean()->IsTrue() );
  }
  
  static void volume(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->mute() );
  }
  
  static void set_volume(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err("* @set volume {uint} 0-100");
    }
    av8_self(Video);
    self->set_volume( value->ToNumber()->Value() );
  }
  
  static void time(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->time() / 1000.0 );
  }
  
  static void duration(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->duration() / 1000.0 );
  }
  
  static void audio_track_index(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->audio_track_index() );
  }
  
  static void audio_track_count(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->audio_track_count() );
  }
  
  static void select_audio_track(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func select_audio_track(index)\n"
        "* @arg index {uint} audio track index\n"
      );
    }
    av8_self(Video);
    self->select_audio_track( args[0]->ToUint32()->Value() );
  }
  
  static void audio_track(FunctionCall args) {
    av8_worker(args);
    av8_self(Video);
    if (args.Length() < 1 || ! args[0]->IsUint32() ) {
      av8_return( track_to_v8value(self->audio_track(), worker) );
    } else {
      av8_return( track_to_v8value(self->audio_track(args[0]->ToUint32()->Value()), worker) );
    }
  }
  
  static void video_track(FunctionCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( track_to_v8value(self->video_track(), worker) );
  }
  
  static void start(FunctionCall args) {
    av8_worker(args);
    av8_self(Video);
    self->start();
  }
  
  static void seek(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsNumber() ) {
      av8_throw_err(
        "* @func seek(time)\n"
        "* @arg time {uint} ms\n"
        "* @ret {bool} success\n"
      );
    }
    av8_self(Video);
    av8_return( self->seek( args[0]->ToNumber()->Value() * 1000.0 ));
  }
  
  static void pause(FunctionCall args) {
    av8_worker(args);
    av8_self(Video);
    self->pause();
  }
  
  static void resume(FunctionCall args) {
    av8_worker(args);
    av8_self(Video);
    self->resume();
  }
  
  static void stop(FunctionCall args) {
    av8_worker(args);
    av8_self(Video);
    self->stop();
  }
  
  static void disable_wait_buffer(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->disable_wait_buffer() );
  }
  
  static void set_disable_wait_buffer(Local<v8::String> name,
                                      Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(Video);
    self->disable_wait_buffer( value->ToBoolean()->IsTrue() );
  }
  
  static void video_width(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->video_width() );
  }
  
  static void video_height(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(Video);
    av8_return( self->video_height() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_binding_class(Video, constructor, {
      //
      av8_set_static_field(MEDIA_TYPE_AUDIO, MEDIA_TYPE_AUDIO);
      av8_set_static_field(MEDIA_TYPE_VIDEO, MEDIA_TYPE_VIDEO);
      av8_set_static_field(PLAYER_STATUS_STOP, PLAYER_STATUS_STOP);
      av8_set_static_field(PLAYER_STATUS_START, PLAYER_STATUS_START);
      av8_set_static_field(PLAYER_STATUS_PLAYING, PLAYER_STATUS_PLAYING);
      av8_set_static_field(PLAYER_STATUS_PAUSED, PLAYER_STATUS_PAUSED);
      av8_set_static_field(SOURCE_STATUS_UNINITIALIZED, MULTIMEDIA_SOURCE_STATUS_UNINITIALIZED);
      av8_set_static_field(SOURCE_STATUS_READYING, MULTIMEDIA_SOURCE_STATUS_READYING);
      av8_set_static_field(SOURCE_STATUS_READY, MULTIMEDIA_SOURCE_STATUS_READY);
      av8_set_static_field(SOURCE_STATUS_WAIT, MULTIMEDIA_SOURCE_STATUS_WAIT);
      av8_set_static_field(SOURCE_STATUS_FAULT, MULTIMEDIA_SOURCE_STATUS_FAULT);
      av8_set_static_field(SOURCE_STATUS_EOFF, MULTIMEDIA_SOURCE_STATUS_EOFF);
      av8_set_static_field(VIDEO_COLOR_FORMAT_INVALID, VIDEO_COLOR_FORMAT_INVALID);
      av8_set_static_field(VIDEO_COLOR_FORMAT_YUV420P, VIDEO_COLOR_FORMAT_YUV420P);
      av8_set_static_field(VIDEO_COLOR_FORMAT_YUV420SP, VIDEO_COLOR_FORMAT_YUV420SP);
      av8_set_static_field(VIDEO_COLOR_FORMAT_YUV411P, VIDEO_COLOR_FORMAT_YUV411P);
      av8_set_static_field(VIDEO_COLOR_FORMAT_YUV411SP, VIDEO_COLOR_FORMAT_YUV411SP);
      //
      av8_set_cls_property(auto_play, auto_play, set_auto_play);
      av8_set_cls_property(source_status, source_status);
      av8_set_cls_property(status, status);
      av8_set_cls_property(mute, mute, set_mute);
      av8_set_cls_property(volume, volume, set_volume);
      av8_set_cls_property(time, time);
      av8_set_cls_property(duration, duration);
      av8_set_cls_property(audio_track_index, audio_track_index);
      av8_set_cls_property(audio_track_count, audio_track_count);
      av8_set_cls_property(disable_wait_buffer, disable_wait_buffer, set_disable_wait_buffer);
      av8_set_cls_property(video_width, video_width);
      av8_set_cls_property(video_height, video_height);
      av8_set_cls_method(select_audio_track, select_audio_track);
      av8_set_cls_method(audio_track, audio_track);
      av8_set_cls_method(video_track, video_track);
      av8_set_cls_method(start, start);
      av8_set_cls_method(seek, seek);
      av8_set_cls_method(pause, pause);
      av8_set_cls_method(resume, resume);
      av8_set_cls_method(stop, stop);
    }, Image);
    worker->native_constructors()->set_alias(av8_typeid(Video), View::VIDEO);
  }
};

void binding_video(Local<v8::Object> exports, Worker* worker) {
  WrapVideo::binding(exports, worker);
}

av8_nsd
