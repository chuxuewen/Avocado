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

#ifndef __avocado__android__avocado__
#define __avocado__android__avocado__

#include "autil/util.h"

#if av_android

#include "autil/string.h"
#include "autil/os/android-jni.h"

av_ns(avocado)

/**
 * @class Avocado
*/
class Avocado: public Object {
public:
	Avocado();
	virtual ~Avocado();
	void keep_screen(bool value);
  float display_scale();
	String package_code_path();
	String files_dir_path();
	String cache_dir_path();
  String default_typeface();
  String version();
  String brand();
	String subsystem();
	int network_status();
	bool is_ac_power();
	bool is_battery();
	float battery_level();
	String language();
	uint64 available_memory();
	uint64 memory();
	uint64 used_memory();
  //
	static Avocado* instance();
private:
	jclass    _clazz;
  jmethodID _keep_screen;
  jmethodID _display_scale;
	jmethodID _package_code_path;
	jmethodID _files_dir_path;
	jmethodID _cache_dir_path;
  jmethodID _default_typeface;
  jmethodID _version;
  jmethodID _brand;
	jmethodID _subsystem;
	jmethodID _network_status;
	jmethodID _is_ac_power;
	jmethodID _is_battery;
	jmethodID _battery_level;
	jmethodID _language;
	jmethodID _available_memory;
	jmethodID _memory;
	jmethodID _used_memory;
};

av_end

#endif
#endif