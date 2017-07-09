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

#include "autil/sys.h"
#include "./av8.h"

/**
 * @ns avocado::av8
 */

av8_begin

class NativeSys {
public:

  static void time(FunctionCall args) {
    av8_worker(args);
    av8_return( uint64(sys::time() / 1000) );
  }
  static void time_monotonic(FunctionCall args) {
    av8_worker(args);
    av8_return( uint64(sys::time_monotonic() / 1000) );
  }
  static void name(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::name() );
  }
  static void info(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::info() );
  }
  static void version(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::version() );
  }
  static void brand(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::brand() );
  }
  static void subsystem(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::subsystem() );
  }
  static void language(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::language() );
  }
  static void is_wifi(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::is_wifi() );
  }
  static void is_mobile(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::is_mobile() );
  }
  static void network_status(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::network_status() );
  }
  static void is_ac_power(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::is_ac_power() );
  }
  static void is_battery(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::is_battery() );
  }
  static void battery_level(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::battery_level() );
  }
  static void memory(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::memory() );
  }
  static void used_memory(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::used_memory() );
  }
  static void available_memory(FunctionCall args) {
    av8_worker(args);
    av8_return( sys::available_memory() );
  }
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_set_field(GNUC, av_gnuc);
    av8_set_field(CLANG, av_clang);
    av8_set_field(MSC, av_msc);
    av8_set_field(ARCH_X86, av_arch_x86);
    av8_set_field(ARCH_ARM, av_arch_arm);
    av8_set_field(ARCH_MIPS, av_arch_mips);
    av8_set_field(ARCH_MIPS64, av_arch_mips64);
    av8_set_field(ARCH_IA32, av_arch_ia32);
    av8_set_field(ARCH_X64, av_arch_x64);
    av8_set_field(ARCH_ARM64, av_arch_arm64);
    av8_set_field(ARCH_ARMV7, av_arch_armv7);
    av8_set_field(ARCH_32BIT, av_arch_32bit);
    av8_set_field(ARCH_64BIT, av_arch_64bit);
    av8_set_field(APPLE, av_apple);
    av8_set_field(POSIX, av_posix);
    av8_set_field(UNIX, av_unix);
    av8_set_field(LINUX, av_linux);
    av8_set_field(BSD, av_bsd);
    av8_set_field(CYGWIN, av_cygwin);
    av8_set_field(NACL, av_nacl);
    av8_set_field(IOS, av_ios);
    av8_set_field(OSX, av_osx);
    av8_set_field(ANDROID, av_android);
    av8_set_field(WIN, av_win);
    av8_set_field(QNX, av_qnx);
    av8_set_method(time, time);
    av8_set_method(time_monotonic, time_monotonic);
    av8_set_method(name, name);
    av8_set_method(info, info);
    av8_set_method(version, version);
    av8_set_method(brand, brand);
    av8_set_method(subsystem, subsystem);
    av8_set_method(language, language);
    av8_set_method(is_wifi, is_wifi);
    av8_set_method(is_mobile, is_mobile);
    av8_set_method(network_status, network_status);
    av8_set_method(is_ac_power, is_ac_power);
    av8_set_method(is_battery, is_battery);
    av8_set_method(battery_level, battery_level);
    av8_set_method(memory, memory);
    av8_set_method(used_memory, used_memory);
    av8_set_method(available_memory, available_memory);
  }
};

av8_reg_module(_sys, NativeSys)
av8_end
