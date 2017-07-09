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

#include "sys.h"
#include "string.h"
#include <iostream>

#if av_unix
#include <sys/utsname.h>
#include <unistd.h>
#endif

#if av_apple
#include <mach/mach_time.h>
#include <mach/mach.h>
#include <mach/clock.h>

#define clock_gettime clock_gettime2

/* mach clock port */
extern mach_port_t clock_port;

/* timing struct for osx */
static struct TimingMach {
  mach_timebase_info_data_t timebase;
  clock_serv_t cclock;
} timing_mach_g;

av_init_block(timing_mach_init) {
  if ( mach_timebase_info(&timing_mach_g.timebase) == 0 ) {
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &timing_mach_g.cclock);
  }
}

int clock_gettime2(clockid_t id, struct timespec *tspec) {
  mach_timespec_t mts;
  int retval = 0;
  if (id == CLOCK_REALTIME) {
    retval = clock_get_time(timing_mach_g.cclock, &mts);
    if (retval != 0) return retval;
    tspec->tv_sec = mts.tv_sec;
    tspec->tv_nsec = mts.tv_nsec;
  } else if (id == CLOCK_MONOTONIC) {
    retval = clock_get_time(clock_port, &mts);
    if (retval != 0) return retval;
    tspec->tv_sec = mts.tv_sec;
    tspec->tv_nsec = mts.tv_nsec;
  } else {
    /* only CLOCK_MONOTOIC and CLOCK_REALTIME clocks supported */
    return -1;
  }
  return 0;
}

#endif

av_ns(avocado)
av_ns(sys)

String name() {
#if av_nacl
  static String _name("Nacl"); return _name;
#elif av_ios
  static String _name("iOS"); return _name;
#elif av_osx
  static String _name("MacOSX"); return _name;
#elif av_android
  static String _name("Android"); return _name;
#elif av_win
  static String _name("Windows"); return _name;
#elif av_qnx
  static String _name("Qnx"); return _name;
#elif av_linux
  static String _name("Linux"); return _name;
#else
# error no Support
#endif
}

#if av_unix

static String _info;

static void initializ() {
  if (_info.is_empty()) {
    static struct utsname _uts;
    static char _hostname[256];
    gethostname(_hostname, 255);
    uname(&_uts);
    _info = String::format("host: %s\nsys: %s\nmachine: %s\nnodename: %s\nversion: %s\nrelease: %s",
                           _hostname,
                           _uts.sysname,
                           _uts.machine,
                           _uts.nodename, _uts.version, _uts.release);
    //  getlogin(), getuid(), getgid(),
  }
}

String info() {
  initializ(); return _info;
}

#endif

bool is_wifi() {
  return network_status() == 2;
}

bool is_mobile() {
  return network_status() >= 3;
}

int64 time_second() {
  return ::time(nullptr);
}

int64 time() {
  timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  int64_t r = now.tv_sec * 1000000LL + now.tv_nsec / 1000LL;
  return r;
}

int64 time_monotonic() {
  timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  int64_t r = now.tv_sec * 1000000LL + now.tv_nsec / 1000LL;
  return r;
}

av_end av_end
