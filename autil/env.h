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

#ifndef __avocado__env__
#define __avocado__env__

#define av_cpp __cplusplus

#ifdef __EXCEPTIONS
# define av_error_support 1
#else
# define av_error_support 0
#endif

#ifndef NULL
# define NULL 0
#endif

#define av_gnuc          0
#define av_clang         0
#define av_msc           0
#define av_arch_x86      0
#define av_arch_arm      0
#define av_arch_mips     0
#define av_arch_mips64   0
#define av_arch_ia32     0
#define av_arch_x64      0
#define av_arch_arm64    0
#define av_arch_armv7    0
#define av_arch_32bit    0
#define av_arch_64bit    0
#define av_apple         0
#define av_posix         0
#define av_unix          0
#define av_linux         0
#define av_bsd           0
#define av_cygwin        0
#define av_nacl          0
#define av_ios           0
#define av_osx           0
#define av_android       0
#define av_win           0
#define av_qnx           0

#if defined(__GNUC__)
# undef av_gnuc
# define av_gnuc       1
#endif

#if defined(__clang__)
# undef av_clang
# define av_clang       1
#endif

#if defined(_MSC_VER)
# undef av_msc
# define av_msc       1
#endif

#if defined(_M_X64) || defined(__x86_64__)
# undef av_arch_x86
# define av_arch_x86        1
# if defined(__native_client__)
#   undef av_arch_ia32
#   define av_arch_ia32     1
#   undef av_arch_32bit
#   define av_arch_32bit    1
# else // else __native_client__
#   undef av_arch_x64
#   define av_arch_x64      1
#   undef av_arch_64bit
#   define av_arch_64bit    1
# endif  // __native_client__

#elif defined(_M_IX86) || defined(__i386__)
# undef av_arch_x86
# define av_arch_x86        1
# undef av_arch_ia32
# define av_arch_ia32       1
# undef av_arch_32bit
# define av_arch_32bit      1

#elif defined(__arm64__) || defined(__AARCH64EL__)
# undef av_arch_arm
# define av_arch_arm        1
# undef av_arch_arm64
# define av_arch_arm64      1
# undef av_arch_64bit
# define av_arch_64bit      1

#elif defined(__ARMEL__)
# undef av_arch_arm
# define av_arch_arm        1
# undef av_arch_32bit
# define av_arch_32bit      1

#elif defined(__mips64)
# undef av_arch_mips
# define av_arch_mips       1
# undef av_arch_mips64
# define av_arch_mips64     1
# undef av_arch_64bit
# define av_arch_64bit      1

#elif defined(__MIPSEL__)
# undef av_arch_mips
# define av_arch_mips       1
# undef av_arch_32bit
# define av_arch_32bit      1

#else
# error Host architecture was not detected as supported by avocado
#endif

#if defined(__ARM_ARCH_7A__) || \
defined(__ARM_ARCH_7R__) || \
defined(__ARM_ARCH_7__)
# undef av_arch_armv7
# define av_arch_armv7  1
#endif

#if defined(__sun)
# undef av_bsd
# define av_bsd        1
# undef av_unix
# define av_unix       1
#endif

#if defined(__OpenBSD__) || \
defined(__NetBSD__)   || \
defined(__FreeBSD__)  || \
defined(__DragonFly__)
# undef av_posix
# define av_posix      1
# undef av_bsd
# define av_bsd        1
# undef av_unix
# define av_unix       1
#endif

#if defined(__APPLE__)
# undef av_apple
# define av_apple      1
# undef av_posix
# define av_posix      1
# undef av_bsd
# define av_bsd        1
# undef av_unix
# define av_unix       1
#endif

#if defined(__CYGWIN__)
# undef av_cygwin
# define av_cygwin     1
# undef av_posix
# define av_posix      1
#endif

#if defined(__unix__)
# undef av_unix
# define av_unix       1
#endif

#if defined(__linux__)
# undef av_linux
# define av_linux      1
# undef av_posix
# define av_posix      1
# undef av_unix
# define av_unix       1
#endif

#if defined(__native_client__)
# undef av_nacl
# define av_nacl       1
# undef av_posix
# define av_posix      1
#endif

#ifdef __APPLE__
# include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
# undef av_ios
# define av_ios        1
#endif

#if TARGET_OS_MAC
# undef av_osx
# define av_osx        1
#endif

#define av_mac  				av_osx

#if defined(__ANDROID__)
# undef av_android
# define av_android    1
# undef av_posix
# define av_posix      1
# undef av_linux
# define av_linux      1
# undef av_unix
# define av_unix       1
#endif

#if defined(_WINDOWS)
# undef av_win
# define av_win        1
#endif

#if defined(__QNXNTO__)
# undef av_posix
# define av_posix 1
# undef av_qnx
# define av_qnx 1
#endif

#endif
