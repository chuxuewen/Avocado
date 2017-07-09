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

#include "avocado.h"

av_ns(avocado)

typedef JNI::MethodInfo MethodInfo;
typedef JNI::ScopeENV   ScopeENV;

static Avocado* _instance = nullptr;

Avocado* Avocado::instance() {
	if ( !_instance ) {
		_instance = new Avocado();
	}
	return _instance;
}

Avocado::Avocado() {
  ScopeENV env;
  _clazz              = JNI::find_clazz("com/avocado/Avocado");
  _keep_screen        = JNI::find_static_method(_clazz, "keep_screen", "(Z)V");
  _display_scale      = JNI::find_static_method(_clazz, "display_scale", "()F");
  _package_code_path  = JNI::find_static_method(_clazz, "package_code_path", "()Ljava/lang/String;");
  _files_dir_path     = JNI::find_static_method(_clazz, "files_dir_path", "()Ljava/lang/String;");
  _cache_dir_path     = JNI::find_static_method(_clazz, "cache_dir_path", "()Ljava/lang/String;");
  _default_typeface   = JNI::find_static_method(_clazz, "default_typeface", "()Ljava/lang/String;");
  _version            = JNI::find_static_method(_clazz, "version", "()Ljava/lang/String;");
  _brand              = JNI::find_static_method(_clazz, "brand", "()Ljava/lang/String;");
  _subsystem          = JNI::find_static_method(_clazz, "subsystem", "()Ljava/lang/String;");
  _network_status     = JNI::find_static_method(_clazz, "network_status", "()I");
  _is_ac_power        = JNI::find_static_method(_clazz, "is_ac_power", "()Z");
  _is_battery         = JNI::find_static_method(_clazz, "is_battery", "()Z");
  _battery_level      = JNI::find_static_method(_clazz, "battery_level", "()F");
  _language           = JNI::find_static_method(_clazz, "language", "()Ljava/lang/String;");
  _available_memory   = JNI::find_static_method(_clazz, "available_memory", "()J");
  _memory             = JNI::find_static_method(_clazz, "memory", "()J");
  _used_memory        = JNI::find_static_method(_clazz, "used_memory", "()J");
  _clazz = (jclass)env->NewGlobalRef(_clazz);
}

Avocado::~Avocado() {
  ScopeENV env;
  env->DeleteGlobalRef(_clazz);
}

void Avocado::keep_screen(bool value) {
  ScopeENV env;
  env->CallStaticVoidMethod(_clazz, _keep_screen, value);
}
float Avocado::display_scale() {
  ScopeENV env;
  return env->CallStaticFloatMethod(_clazz, _display_scale);
}
String Avocado::package_code_path() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _package_code_path);
  return JNI::jstring_to_string((jstring)obj, *env);
}
String Avocado::files_dir_path() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _files_dir_path);
  return JNI::jstring_to_string((jstring)obj, *env);
}
String Avocado::cache_dir_path() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _cache_dir_path);
  return JNI::jstring_to_string((jstring)obj, *env);
}
String Avocado::default_typeface() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _default_typeface);
  return JNI::jstring_to_string((jstring)obj, *env);
}
String Avocado::version() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _version);
  return JNI::jstring_to_string((jstring)obj, *env);
}
String Avocado::brand() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _brand);
  return JNI::jstring_to_string((jstring)obj, *env);
}
String Avocado::subsystem() {
  ScopeENV env;
  jobject obj = env->CallStaticObjectMethod(_clazz, _subsystem);
  return JNI::jstring_to_string((jstring)obj, *env);
}
int Avocado::network_status() {
  ScopeENV env;
  return env->CallStaticIntMethod(_clazz, _network_status);
}
bool Avocado::is_ac_power() {
  ScopeENV env;
  return env->CallStaticBooleanMethod(_clazz, _is_ac_power);
}
bool Avocado::is_battery() {
  ScopeENV env;
  return env->CallStaticBooleanMethod(_clazz, _is_battery);
}
float Avocado::battery_level() {
  ScopeENV env;
  return env->CallStaticFloatMethod(_clazz, _battery_level);
}
String Avocado::language() {
	ScopeENV env;
	return JNI::jstring_to_string((jstring)env->CallStaticObjectMethod(_clazz, _language), *env);
}
uint64 Avocado::available_memory() {
  ScopeENV env;
  return env->CallStaticLongMethod(_clazz, _available_memory);
}
uint64 Avocado::memory() {
  ScopeENV env;
  return env->CallStaticLongMethod(_clazz, _memory);
}
uint64 Avocado::used_memory() {
  ScopeENV env;
  return env->CallStaticLongMethod(_clazz, _used_memory);
}

av_end