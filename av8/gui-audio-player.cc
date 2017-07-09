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
#include "gui-player.h"
#include "agui/audio-player.h"

/**
 * @ns avocado::av8::gui
 */

av8_ns(gui)

Local<v8::Value> track_to_v8value(const TrackInfo* track, Worker* worker) {
  if ( ! track ) {
    return worker->Null();
  }

  /**
   * @object TrackInfo
   * type {meun MediaType}
   * mime {String}
   * codec_id {int}
   * codec_tag {uint}
   * format {int}
   * profile {int}
   * level {int}
   * width {uint}
   * height {uint}
   * language {String}
   * bitrate {uint}
   * sample_rate {uint}
   * channel_count {uint64}
   * channel_layout {uint}
   * frame_interval {uint} ms
   * @end
   */
  Local<v8::Object> obj = worker->NewObject().As<v8::Object>();
  obj->Set(worker->NewStringAscii("type"), worker->New(track->type));
  obj->Set(worker->NewStringAscii("mime"), worker->New(track->mime));
  obj->Set(worker->NewStringAscii("codec_id"), worker->New(track->codec_id));
  obj->Set(worker->NewStringAscii("codec_tag"), worker->New(track->codec_tag));
  obj->Set(worker->NewStringAscii("format"), worker->New(track->format));
  obj->Set(worker->NewStringAscii("profile"), worker->New(track->profile));
  obj->Set(worker->NewStringAscii("level"), worker->New(track->level));
  obj->Set(worker->NewStringAscii("width"), worker->New(track->width));
  obj->Set(worker->NewStringAscii("height"), worker->New(track->height));
  obj->Set(worker->NewStringAscii("language"), worker->New(track->language));
  obj->Set(worker->NewStringAscii("bitrate"), worker->New(track->bitrate));
  obj->Set(worker->NewStringAscii("sample_rate"), worker->New(track->sample_rate));
  obj->Set(worker->NewStringAscii("channel_count"), worker->New(track->channel_count));
  obj->Set(worker->NewStringAscii("channel_layout"), worker->New(track->channel_layout));
  obj->Set(worker->NewStringAscii("frame_interval"), worker->New(track->frame_interval / 1000.0));
  return obj.As<v8::Value>();
}

/**
 * @class WrapAudioPlayer
 */
class WrapAudioPlayer: public WrapBase {
public:
  
  template<class T>
  void add_event_listener_(const GUIEventName& name, String& func, int id) {
    self<AudioPlayer>()->on(name, [this, func]( Event<>& evt ) {
      v8::HandleScope scope(worker()->isolate());
      // arg event
      Wrap<Event<>>* ev = Wrap<Event<>>::wrap(&evt);
      
      ev->set_data(EventDataCast::entity<T>()); // set data cast func
      
      v8::Local<v8::Value> args[2] = { ev->local(), worker()->New(true) };
      // call js trigger func
      call( _worker->NewStringAscii(func), 2, args );  
    }, id);
  }
  
  /**
   * @func overwrite
   */
  virtual bool add_event_listener(cString& name, String& func, int id) {
    
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
    
    auto i = GUI_EVENT_PLAYER_TABLE.find(name);
    if ( i != GUI_EVENT_PLAYER_TABLE.end() ) {
      // off event listener
      self<AudioPlayer>()->off(i.value(), id);
      return true;
    }
    return true;
  }
  
  /**
   * @constructor([src])
   * @arg [src] {String}
   */
  static void constructor(FunctionCall args) {
    av8_worker(args);
    if ( args.Length() > 0 && args[0]->IsString() ) {
      New<WrapAudioPlayer>(args, new AudioPlayer(worker->to_string_utf8(args[0])));
    } else {
      New<WrapAudioPlayer>(args, new AudioPlayer());
    }
  }
  
  /**
   * @get auto_play {bool}
   */
  static void auto_play(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->auto_play() );
  }
  
  /**
   * @set auto_play {bool}
   */
  static void set_auto_play(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->set_auto_play( value->ToBoolean()->IsTrue() );
  }
  
  /**
   * @get source_status {enum MultimediaSourceStatus}
   */
  static void source_status(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->source_status() );
  }
  
  /**
   * @get status {enum PlayerStatus}
   */
  static void status(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->status() );
  }
  
  /**
   * @get mute {bool}
   */
  static void mute(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->mute() );
  }
  
  /**
   * @set mute {bool}
   */
  static void set_mute(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->set_mute( value->ToBoolean()->IsTrue() );
  }
  
  /**
   * @get volume {uint} 0-100
   */
  static void volume(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->mute() );
  }
  
  /**
   * @set volume {uint} 0-100
   */
  static void set_volume(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    if ( !value->IsNumber() ) {
      av8_throw_err("* @set volume {uint} 0-100");
    }
    av8_self(AudioPlayer);
    self->set_volume( value->ToNumber()->Value() );
  }
  
  /**
   * @get src {String}
   */
  static void src(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->src() );
  }
  
  /**
   * @set src {String}
   */
  static void set_src(Local<v8::String> name, Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->set_src( worker->to_string_utf8(value) );
  }
  
  /**
   * @get time {uint64} ms
   */
  static void time(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->time() / 1000.0 );
  }
  
  /**
   * @get duration {uint64} ms
   */
  static void duration(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->duration() / 1000.0 );
  }
  
  /**
   * @get track_index {uint}
   */
  static void track_index(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->track_index() );
  }
  
  /**
   * @get track_count {uint}
   */
  static void track_count(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->track_count() );
  }
  
  /**
   * @func select_track(index)
   * @arg index {uint} audio track index
   */
  static void select_track(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsUint32() ) {
      av8_throw_err(
        "* @func select_track(index)\n"
        "* @arg index {uint} audio track index\n"
      );
    }
    av8_self(AudioPlayer);
    self->select_track( args[0]->ToUint32()->Value() );
  }
  
  /**
   * @func track([index])
   * @arg [track=curent_track] {uint} default use current track index
   * @ret {object TrackInfo}
   */
  static void track(FunctionCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    if (args.Length() < 1 || !args[0]->IsUint32() ) {
      av8_return( track_to_v8value(self->track(), worker) );
    } else {
      av8_return( track_to_v8value(self->track(args[0]->ToUint32()->Value()), worker) );
    }
  }
  
  /**
   * @func start()
   */
  static void start(FunctionCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->start();
  }
  
  /**
   * @func seek(time)
   * @arg time {uint} ms
   * @ret {bool} success
   */
  static void seek(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1 || ! args[0]->IsNumber() ) {
      av8_throw_err(
        "* @func seek(time)\n"
        "* @arg time {uint} ms\n"
        "* @ret {bool} success\n"
      );
    }
    av8_self(AudioPlayer);
    av8_return( self->seek( args[0]->ToNumber()->Value() * 1000.0 ));
  }
  
  /**
   * @func pause()
   */
  static void pause(FunctionCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->pause();
  }
  
  /**
   * @func resume()
   */
  static void resume(FunctionCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->resume();
  }
  
  /**
   * @func stop()
   */
  static void stop(FunctionCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->stop();
  }
  
  /**
   * @get disable_wait_buffer {bool}
   */
  static void disable_wait_buffer(Local<v8::String> name, PropertyCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    av8_return( self->disable_wait_buffer() );
  }
  
  /**
   * @set disable_wait_buffer {bool}
   */
  static void set_disable_wait_buffer(Local<v8::String> name,
                                      Local<v8::Value> value, PropertySetCall args) {
    av8_worker(args);
    av8_self(AudioPlayer);
    self->disable_wait_buffer( value->ToBoolean()->IsTrue() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    
    av8_set_field(MEDIA_TYPE_AUDIO, MEDIA_TYPE_AUDIO);
    av8_set_field(MEDIA_TYPE_VIDEO, MEDIA_TYPE_VIDEO);
    av8_set_field(PLAYER_STATUS_STOP, PLAYER_STATUS_STOP);
    av8_set_field(PLAYER_STATUS_START, PLAYER_STATUS_START);
    av8_set_field(PLAYER_STATUS_PLAYING, PLAYER_STATUS_PLAYING);
    av8_set_field(PLAYER_STATUS_PAUSED, PLAYER_STATUS_PAUSED);
    av8_set_field(SOURCE_STATUS_UNINITIALIZED, MULTIMEDIA_SOURCE_STATUS_UNINITIALIZED);
    av8_set_field(SOURCE_STATUS_READYING, MULTIMEDIA_SOURCE_STATUS_READYING);
    av8_set_field(SOURCE_STATUS_READY, MULTIMEDIA_SOURCE_STATUS_READY);
    av8_set_field(SOURCE_STATUS_WAIT, MULTIMEDIA_SOURCE_STATUS_WAIT);
    av8_set_field(SOURCE_STATUS_FAULT, MULTIMEDIA_SOURCE_STATUS_FAULT);
    av8_set_field(SOURCE_STATUS_EOFF, MULTIMEDIA_SOURCE_STATUS_EOFF);
    av8_set_field(CH_INVALID, CH_INVALID);
    av8_set_field(CH_FRONT_LEFT, CH_FRONT_LEFT);
    av8_set_field(CH_FRONT_RIGHT, CH_FRONT_RIGHT);
    av8_set_field(CH_FRONT_CENTER, CH_FRONT_CENTER);
    av8_set_field(CH_LOW_FREQUENCY, CH_LOW_FREQUENCY);
    av8_set_field(CH_BACK_LEFT, CH_BACK_LEFT);
    av8_set_field(CH_BACK_RIGHT, CH_BACK_RIGHT);
    av8_set_field(CH_FRONT_LEFT_OF_CENTER, CH_FRONT_LEFT_OF_CENTER);
    av8_set_field(CH_FRONT_RIGHT_OF_CENTER, CH_FRONT_RIGHT_OF_CENTER);
    av8_set_field(CH_BACK_CENTER, CH_BACK_CENTER);
    av8_set_field(CH_SIDE_LEFT, CH_SIDE_LEFT);
    av8_set_field(CH_SIDE_RIGHT, CH_SIDE_RIGHT);
    av8_set_field(CH_TOP_CENTER, CH_TOP_CENTER);
    av8_set_field(CH_TOP_FRONT_LEFT, CH_TOP_FRONT_LEFT);
    av8_set_field(CH_TOP_FRONT_CENTER, CH_TOP_FRONT_CENTER);
    av8_set_field(CH_TOP_FRONT_RIGHT, CH_TOP_FRONT_RIGHT);
    av8_set_field(CH_TOP_BACK_LEFT, CH_TOP_BACK_LEFT);
    av8_set_field(CH_TOP_BACK_CENTER, CH_TOP_BACK_CENTER);
    av8_set_field(CH_TOP_BACK_RIGHT, CH_TOP_BACK_RIGHT);
    
    av8_binding_class(AudioPlayer, constructor, {
      av8_set_cls_property(auto_play, auto_play, set_auto_play);
      av8_set_cls_property(source_status, source_status);
      av8_set_cls_property(status, status);
      av8_set_cls_property(mute, mute, set_mute);
      av8_set_cls_property(volume, volume, set_volume);
      av8_set_cls_property(src, src, set_src);
      av8_set_cls_property(time, time);
      av8_set_cls_property(duration, duration);
      av8_set_cls_property(track_index, track_index);
      av8_set_cls_property(track_count, track_count);
      av8_set_cls_property(disable_wait_buffer, disable_wait_buffer, set_disable_wait_buffer);
      av8_set_cls_method(select_track, select_track);
      av8_set_cls_method(track, track);
      av8_set_cls_method(start, start);
      av8_set_cls_method(seek, seek);
      av8_set_cls_method(pause, pause);
      av8_set_cls_method(resume, resume);
      av8_set_cls_method(stop, stop);
    }, nullptr);
  }
};

av8_reg_module(_audio, WrapAudioPlayer);
av8_nsd
