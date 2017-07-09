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

#include "audio-player.h"
#include "app.h"

av_gui_begin

const GUIEventName GUI_EVENT_PLAYER_WAIT_BUFFER     ("wait_buffer", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_READY           ("ready", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_START_PLAY      ("start_play", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_ERROR           ("error", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_SOURCE_EOF      ("source_eof", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_PAUSE           ("pause", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_RESUME          ("resume", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_STOP            ("stop", GUI_EVENT_CATEGORY_DEFAULT);
const GUIEventName GUI_EVENT_PLAYER_SEEK            ("seek", GUI_EVENT_CATEGORY_DEFAULT);

const Map<String, GUIEventName> GUI_EVENT_PLAYER_TABLE([] {
  Map<String, GUIEventName> r;
  r.set(GUI_EVENT_PLAYER_WAIT_BUFFER.to_string(), GUI_EVENT_PLAYER_WAIT_BUFFER);
  r.set(GUI_EVENT_PLAYER_READY.to_string(), GUI_EVENT_PLAYER_READY);
  r.set(GUI_EVENT_PLAYER_START_PLAY.to_string(), GUI_EVENT_PLAYER_START_PLAY);
  r.set(GUI_EVENT_PLAYER_ERROR.to_string(), GUI_EVENT_PLAYER_ERROR);
  r.set(GUI_EVENT_PLAYER_SOURCE_EOF.to_string(), GUI_EVENT_PLAYER_SOURCE_EOF);
  r.set(GUI_EVENT_PLAYER_PAUSE.to_string(), GUI_EVENT_PLAYER_PAUSE);
  r.set(GUI_EVENT_PLAYER_RESUME.to_string(), GUI_EVENT_PLAYER_RESUME);
  r.set(GUI_EVENT_PLAYER_STOP.to_string(), GUI_EVENT_PLAYER_STOP);
  r.set(GUI_EVENT_PLAYER_SEEK.to_string(), GUI_EVENT_PLAYER_SEEK);
  return r;
}());

/**
 * @constructor
 */
AudioPlayer::AudioPlayer(cString& uri)
: Responder(this)
, m_source(NULL)
, m_pcm(NULL)
, m_audio(NULL)
, m_loop(NULL)
, m_status(PLAYER_STATUS_STOP)
, m_audio_buffer()
, m_time(0), m_duration(0)
, m_uninterrupted_play_start_time(0)
, m_uninterrupted_play_start_systime(0)
, m_prev_presentation_time(0)
, m_post_id(0)
, m_volume(100)
, m_mute(false)
, m_auto_play(true)
, m_disable_wait_buffer(false)
, m_waiting_buffer(false)
{
  set_src(uri);
}

typedef MultimediaSource::TrackInfo TrackInfo;

/**
 * @class Video::Inl
 */
av_def_inl_members(AudioPlayer, Inl) {
public:
  
  // set pcm ..
  bool write_audio_pcm(uint64 st) {
    bool r = m_pcm->write(WeakBuffer((char*)m_audio_buffer.data[0], m_audio_buffer.linesize[0]));
    if ( !r ) {
      av_debug("Discard, audio PCM frame, %lld", m_audio_buffer.time);
    } else {
      m_prev_presentation_time = st;
    }
    m_audio->release(m_audio_buffer);
    return r;
  }
  
  void play_audio() {
    
  loop:
    
    uint64 sys_time = sys::time_monotonic();
    
    { //
      ScopeLock scope(m_mutex);
      
      if ( m_status == PLAYER_STATUS_STOP ) { // stop
        return; // stop audio
      }
      
      if ( !m_audio_buffer.total ) {
        if ( m_status == PLAYER_STATUS_PLAYING || m_status == PLAYER_STATUS_START ) {
          m_audio_buffer = m_audio->output();
          
          if ( m_audio_buffer.total ) {
            if ( m_waiting_buffer ) {
              m_loop->post(Callback([this](SimpleEvent& evt) {
                trigger(GUI_EVENT_PLAYER_WAIT_BUFFER, Float(1.0F)); // trigger source WAIT event
              }));
              m_waiting_buffer = false;
            }
          } else {
            MultimediaSourceStatus status = m_source->status();
            if ( status == MULTIMEDIA_SOURCE_STATUS_WAIT ) { // 源..等待数据
              if ( m_waiting_buffer == false ) {
                m_loop->post(Callback([this](SimpleEvent& evt) {
                  trigger(GUI_EVENT_PLAYER_WAIT_BUFFER, Float(0.0F)); // trigger source WAIT event
                }));
                m_waiting_buffer = true;
              }
            } else if ( status == MULTIMEDIA_SOURCE_STATUS_EOFF ) {
              m_loop->post(Callback([this](SimpleEvent& evt) { stop(); })); return;
            }
          }
          //
        }
      }
      
      if ( m_audio_buffer.total ) {
        uint64 pts = m_audio_buffer.time;
        
        if (m_uninterrupted_play_start_systime &&      // 0表示还没开始
            pts &&                                        // 演示时间为0表示开始或(即时渲染如视频电话)
            sys_time - m_prev_presentation_time < 300000  // 距离上一帧超过300ms重新记时(如应用程序从休眠中恢复或数据缓冲)
        ) {
          int64 st =  (sys_time - m_uninterrupted_play_start_systime) -     // sys
                      (pts - m_uninterrupted_play_start_time); // frame
          int delay = m_audio->frame_interval();
          
          if (st >= -delay) { // 是否达到播放声音时间。输入pcm到能听到声音会有一些延时,这里设置补偿
            write_audio_pcm(sys_time);
          }
        } else {
          if ( m_status == PLAYER_STATUS_START ) {
            m_status = PLAYER_STATUS_PLAYING;
            m_loop->post(Callback([this](SimpleEvent& evt) {
              trigger(GUI_EVENT_PLAYER_START_PLAY); // trigger start_play event
            }));
          }
          m_uninterrupted_play_start_systime = sys_time;
          m_uninterrupted_play_start_time = m_audio_buffer.time;
          write_audio_pcm(sys_time);
        }
      }
      
      m_audio->advance();
    }
    
    int frame_interval = 1000.0 / 120.0 * 1000; // 120fsp
    int64 sleep_st = frame_interval - sys::time_monotonic() + sys_time;
    if ( sleep_st > 0 ) {
      Thread::sleep_cur(sleep_st);
    }
    
    goto loop;
  }
  
  void trigger(const GUIEventName& type, const Object& data = Object()) {
    Handle<Event<>> evt = New<Event<>>(data);
    Responder::trigger(type, **evt);
  }
  
  bool stop2(Lock& lock, bool is_event) {
    
    if ( m_status != PLAYER_STATUS_STOP ) {
      m_status = PLAYER_STATUS_STOP;
      m_uninterrupted_play_start_time = 0;
      m_uninterrupted_play_start_systime = 0;
      m_prev_presentation_time = 0;
      m_time = 0;
      
      if ( m_audio ) {
        m_audio->release(m_audio_buffer);
        m_audio->extractor()->set_disable(true);
        m_audio->close();
        Release(m_audio); m_audio = nullptr;
      }
      PCMPlayer::Characteristic::Release(m_pcm); m_pcm = nullptr;
      m_source->stop();
      
      lock.unlock();
      { // wait audio thread end
        ScopeLock scope(m_audio_loop_mutex);
      }
      if ( is_event ) {
        Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_STOP); // trigger stop event
      }
      lock.lock();
      
      return true;
    }
    return false;
  }
  
  void stop_and_release(Lock& lock, bool is_event) {
    
    if ( m_post_id ) {
      RunLoop::utils()->abort(m_post_id);
      gui_loop()->abort(m_post_id);
      m_post_id = 0;
    }
    
    stop2(lock, is_event);
    
    Release(m_audio); m_audio = nullptr;
    Release(m_source); m_source = nullptr;
    Release(m_loop); m_loop = nullptr;
    PCMPlayer::Characteristic::Release(m_pcm); m_pcm = nullptr;
    
    m_time = 0;
    m_duration = 0;
  }
  
  inline void stop_and_release(bool is_event) {
    Lock lock(m_mutex);
    stop_and_release(lock, is_event);
  }
  
  /**
   * @func is_active
   */
  inline bool is_active() {
    return m_status == PLAYER_STATUS_PAUSED || m_status == PLAYER_STATUS_PLAYING;
  }
  
  void start_run() {
    Lock lock(m_mutex);
    
    av_assert( m_source && m_audio && m_pcm );
    av_assert( m_source->is_active() );
    av_assert( m_status == PLAYER_STATUS_START );
    
    m_waiting_buffer = false;
    
    if ( m_audio->open() ) {
      m_source->seek(0);
      m_audio->release( m_audio_buffer );
      m_audio->flush();
      m_audio->extractor()->set_disable(false);
      m_pcm->flush();
      m_pcm->set_volume(m_volume);
      m_pcm->set_mute(m_mute);
      
      Thread::fork([this](Thread& t) {
        ScopeLock scope(m_audio_loop_mutex);
        Inl_AudioPlayer(this)->play_audio();
      }, "video_audio_play_loop");
    } else {
      stop2(lock, true);
    }
  }
  
};

void AudioPlayer::multimedia_source_ready(MultimediaSource* src) {
  av_assert( m_source == src );
  
  if ( m_audio ) {
    Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_READY); // trigger event ready
    if ( m_status == PLAYER_STATUS_START ) {
      Inl_AudioPlayer(this)->start_run();
    }
    return;
  }
  
  // 创建解码器很耗时,所以这里不在主线程创建
  
  m_post_id = RunLoop::utils()->post(Callback([=](SimpleEvent& d) {
    av_assert( m_source == src );
    
    MediaCodec* audio = MediaCodec::create(MEDIA_TYPE_AUDIO, m_source);
    PCMPlayer* pcm = m_pcm;
    
    if ( audio && !m_pcm ) {
      pcm = PCMPlayer::create(audio->channel_count(),
                              audio->extractor()->track(0).sample_rate );
    }
    
    ScopeLock scope(m_mutex);
    m_pcm = pcm;
    m_post_id = 0;
    
    if ( m_source != src ) {
      Release(audio); return;
    } else {
      m_audio = audio;
    }
    
    // 创建解码器完成后发送到主线程
    
    m_post_id = m_loop->post(Callback([&, src](SimpleEvent& d) {
      m_post_id = 0;
      av_assert( m_source == src );
      
      if (m_audio && m_pcm) {
        { //
          ScopeLock scope(m_mutex);
          m_duration  = m_source->duration();
        }
        
        Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_READY); // trigger event ready
        
        if ( m_status == PLAYER_STATUS_START ) {
          Inl_AudioPlayer(this)->start_run();
        } else {
          if (m_auto_play) {
            start();
          }
        }
      } else {
        Error e(ERR_AUDIO_NEW_CODEC_FAIL, "Unable to create video decoder");
        av_err("%s", *e.message());
        Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_ERROR, e); // trigger event error
        stop();
      }
      
    }, this));
  }, this));
}

void AudioPlayer::multimedia_source_wait_buffer(MultimediaSource* so, float process) {
  if ( m_waiting_buffer ) {/* 开始等待数据缓存不触发事件,因为在解码器队列可能还存在数据,
                            * 所以等待解码器也无法输出数据时再触发事件
                            */
    if ( process < 1.0 ) {
       // trigger event wait_buffer
      Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_WAIT_BUFFER, Float(process));
    }
  }
}

void AudioPlayer::multimedia_source_eof(MultimediaSource* so) {
  Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_SOURCE_EOF); // trigger event eof
}

void AudioPlayer::multimedia_source_error(MultimediaSource* so, cError& err) {
  Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_ERROR, err); // trigger event error
  stop();
}

/**
 * @destructor
 */
AudioPlayer::~AudioPlayer() {
  Inl_AudioPlayer(this)->stop_and_release(false);
}

/**
 * @func src # get src
 */
String AudioPlayer::src() const {
  if ( m_source ) {
    return m_source->uri().href();
  } else {
    return String::empty;
  }
}

/**
 * @func src # set src
 */
void AudioPlayer::set_src(cString& value) {
  
  if ( value.is_empty() ) {
    return;
  }
  String src = freader()->format(value);
  
  Lock lock(m_mutex);
  
  if ( m_source ) {
    if ( m_source->uri().href() == src ) {
      return;
    }
    Inl_AudioPlayer(this)->stop_and_release(lock, true);
  }
  m_source = new MultimediaSource(src);
  m_loop = RunLoop::current()->keep_alive();
  m_source->set_delegate(this);
  m_source->disable_wait_buffer(m_disable_wait_buffer);
  m_source->start();
}

/**
 * @func start play
 */
void AudioPlayer::start() {
  Lock scope(m_mutex);
  
  if ( m_status == PLAYER_STATUS_STOP && m_source ) {
    m_status = PLAYER_STATUS_START;
    m_uninterrupted_play_start_time = 0;
    m_uninterrupted_play_start_systime = 0;
    m_prev_presentation_time = 0;
    m_time = 0;
    m_source->start();
    
    if ( m_audio && m_pcm ) {
      if ( m_source->is_active() ) {
        scope.unlock();
        Inl_AudioPlayer(this)->start_run();
      }
    }
  }
}

/**
 * @func stop play
 * */
void AudioPlayer::stop() {
  Lock lock(m_mutex);
  Inl_AudioPlayer(this)->stop2(lock, true);
}

/**
 * @func source_status
 * */
MultimediaSourceStatus AudioPlayer::source_status() const {
  if ( m_source ) {
    return m_source->status();
  }
  return MULTIMEDIA_SOURCE_STATUS_UNINITIALIZED;
}

/**
 * @func seek to target time
 */
bool AudioPlayer::seek(uint64 timeUs) {
  Lock scope(m_mutex);
  
  if ( Inl_AudioPlayer(this)->is_active() && timeUs < m_duration ) {
    av_assert(m_source);
    
    if ( m_source->seek(timeUs) ) {
      m_uninterrupted_play_start_systime = 0;
      m_time = timeUs;
      m_audio->release( m_audio_buffer );
      m_audio->flush();
      m_pcm->flush();
      scope.unlock();
      Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_SEEK, Uint64(m_time)); // trigger seek event
      return true;
    }
  }
  return false;
}

/**
 * @func pause play
 * */
void AudioPlayer::pause() {
  Lock scope(m_mutex);
  
  if ( m_status == PLAYER_STATUS_PLAYING && m_duration /*没有长度信息不能暂停*/ ) {
    m_status = PLAYER_STATUS_PAUSED;
    m_uninterrupted_play_start_systime = 0;
    scope.unlock();
    Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_PAUSE); // trigger pause event
  }
}

/**
 * @func resume play
 * */
void AudioPlayer::resume() {
  Lock scope(m_mutex);
  
  if ( m_status == PLAYER_STATUS_PAUSED ) {
    m_status = PLAYER_STATUS_PLAYING;
    m_uninterrupted_play_start_systime = 0;
    scope.unlock();
    Inl_AudioPlayer(this)->trigger(GUI_EVENT_PLAYER_RESUME); // trigger resume event
  }
}

/**
 * @func set_mute setting mute status
 * */
void AudioPlayer::set_mute(bool value) {
  ScopeLock scope(m_mutex);
  
  if ( value != m_mute ) {
    m_mute = value;
    if ( m_pcm ) { // action
      m_pcm->set_mute(m_mute);
    }
  }
}

/**
 * @func set_volume
 */
void AudioPlayer::set_volume(uint value) {
  ScopeLock scope(m_mutex);
  
  value = av_min(value, 100);
  m_volume = value;
  if ( m_pcm ) {
    m_pcm->set_volume(value);
  }
}

/**
 * @func disable_wait_buffer
 */
void AudioPlayer::disable_wait_buffer(bool value) {
  ScopeLock scope(m_mutex);
  m_disable_wait_buffer = value;
  if (m_source) {
    m_source->disable_wait_buffer(value);
  }
}

/**
 * @func audio_track_count
 */
uint AudioPlayer::track_count() const {
  if ( m_audio ) {
    return m_audio->extractor()->track_count();
  }
  return 0;
}

/**
 * @func audio_track
 */
uint AudioPlayer::track_index() const {
  if ( m_audio ) {
    return m_audio->extractor()->track_index();
  }
  return 0;
}

/**
 * @func audio_track
 */
const TrackInfo* AudioPlayer::track() const {
  if ( m_audio ) {
    return &m_audio->extractor()->track();
  }
  return nullptr;
}

/**
 * @func audio_track
 */
const TrackInfo* AudioPlayer::track(uint index) const {
  if ( m_audio && index < m_audio->extractor()->track_count() ) {
    return &m_audio->extractor()->track(index);
  }
  return nullptr;
}

/**
 * @func select_audio_track
 * */
void AudioPlayer::select_track(uint index) {
  ScopeLock scope(m_mutex);
  
  if ( m_audio && index < m_audio->extractor()->track_count() ) {
    m_audio->extractor()->select_track(index);
  }
}

av_gui_end
