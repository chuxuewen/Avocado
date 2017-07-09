/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, louis.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __avocado__gui__audio_player__
#define __avocado__gui__audio_player__

#include "player.h"
#include "pcm-player.h"
#include "media-codec.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

class AudioPlayer;

/**
 * @class AudioPlayer
 */
class AudioPlayer: public Responder<Event<>, GUIEventName>, public MultimediaSource::Delegate {
  av_hidden_all_copy(AudioPlayer);
public:
  
  typedef MultimediaSource::TrackInfo TrackInfo;
  typedef MediaCodec::OutputBuffer    OutputBuffer;
  
  AudioPlayer(cString& uri = String::empty);
  
  /**
   * @destructor
   */
  virtual ~AudioPlayer();
  
  /**
   * @overwrite
   */
  virtual void multimedia_source_ready(MultimediaSource* src);
  virtual void multimedia_source_wait_buffer(MultimediaSource* src, float process);
  virtual void multimedia_source_eof(MultimediaSource* src);
  virtual void multimedia_source_error(MultimediaSource* src, cError& err);
  
  /**
   * @func src get src
   */
  String src() const;
  
  /**
   * @func set_src set src
   */
  void set_src(cString& value);
  
  /**
   * @func auto_play
   */
  inline bool auto_play() const { return m_auto_play; }
  
  /**
   * @func set_auto_play setting auto play
   */
  inline void set_auto_play(bool value) { m_auto_play = value; }
  /**
   * @func source_status
   * */
  MultimediaSourceStatus source_status() const;
  
  /**
   * @func status getting play status
   */
  inline PlayerStatus status() const { return m_status; }
  
  /**
   * @func mute getting mute status
   * */
  inline bool mute() const { return m_mute; }
  
  /**
   * @func set_mute setting mute status
   * */
  void set_mute(bool value);
  
  /**
   * @func volume
   */
  inline uint volume() { return m_volume; }
  
  /**
   * @func set_volume
   */
  void set_volume(uint value);
  
  /**
   * @func time
   * */
  inline uint64 time() const { return m_time; }
  
  /**
   * @func duration
   * */
  inline uint64 duration() const { return m_duration; }
  
  /**
   * @func audio_track_count
   */
  uint track_count() const;
  
  /**
   * @func audio_track
   */
  uint track_index() const;
  
  /**
   * @func audio_track
   */
  const TrackInfo* track() const;
  
  /**
   * @func audio_track
   */
  const TrackInfo* track(uint index) const;
  
  /**
   * @func select_audio_track
   * */
  void select_track(uint index);
  
  /**
   * @func start play
   */
  void start();
  
  /**
   * @func seek to target time
   */
  bool seek(uint64 timeUs);
  
  /**
   * @func pause play
   * */
  void pause();
  
  /**
   * @func resume play
   * */
  void resume();
  
  /**
   * @func stop play
   * */
  void stop();
  
  /**
   * @func disable_wait_buffer
   */
  void disable_wait_buffer(bool value);
  
  /**
   * @func disable_wait_buffer
   */
  inline bool disable_wait_buffer() const { return m_disable_wait_buffer; }
  
private:
  
  MultimediaSource* m_source;
  PCMPlayer*    m_pcm;
  MediaCodec*   m_audio;
  LoopProxy*    m_loop;
  PlayerStatus  m_status;
  OutputBuffer  m_audio_buffer;
  uint64  m_duration, m_time;
  uint64  m_uninterrupted_play_start_time;
  uint64  m_uninterrupted_play_start_systime;
  uint64  m_prev_presentation_time;
  Mutex   m_audio_loop_mutex, m_mutex;
  uint    m_post_id;
  uint    m_volume;
  bool    m_mute;
  bool    m_auto_play;
  bool    m_disable_wait_buffer;
  bool    m_waiting_buffer;
  
  av_def_inl_cls(Inl);
};

av_gui_end
#endif
