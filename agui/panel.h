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

#ifndef __avocado__gui__panel__
#define __avocado__gui__panel__

#include "div.h"

av_gui_begin

/**
 * @class Panel
 */
class Panel: public Div {
public:
  av_def_gui_view(PANEL, Panel, panel)
  
  Panel();
    
  /**
   * @func allow_leave get
   */
  inline bool allow_leave() const { return m_allow_leave; }
  
  /**
   * @func set_allow_leave set
   */
  inline void set_allow_leave(bool value) { m_allow_leave = value; }
  
  /**
   * @func switch_time get
   */
  inline uint64 switch_time() const { return m_switch_time; }
  
  /**
   * @func set_switch_time set
   */
  inline void set_switch_time(uint64 value) { m_switch_time = value; }
  
  /**
   * @func enable_switch get
   */
  inline bool enable_switch() const { return m_enable_switch; }
  
  /**
   * @func set_enable_switch set
   */
  inline void set_enable_switch(bool value) { m_enable_switch = value; }
  
  /**
   * @func first_button
   */
  Button* first_button();
  
  /**
   * @func is_activity
   */
  bool is_activity() const;
  
  /**
   * @func parent_panel
   */
  Panel* parent_panel();
  
private:
  
  bool    m_allow_leave;
  uint64  m_switch_time;
  bool    m_enable_switch;
  
  av_def_inl_cls(Inl);
  
};

av_gui_end

#endif
