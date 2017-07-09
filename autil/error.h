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

#ifndef __avocado_error__
#define __avocado_error__

/**
 * @ns avocado
 */

#include "util.h"
#include "error-code.h"

#if av_error_support

#define av_throw(code, ...) throw avocado::Error(code, __VA_ARGS__)
#define av_assert_err(cond, ...) if(!(cond)) throw avocado::Error(__VA_ARGS__)
#define av_def_err throw(avocado::Error)

#define av_ignore_err(block) try block catch (const avocado::Error& err) {    \
  av_debug("%s,%s", "The exception is ignored", err.message().c());     \
}((void)0)

av_ns(avocado)

/**
 * @class Error
 */
class Error: public Object {
public:
  
  Error();
  Error(int code, cString& msg);
  Error(int code, cchar*, ...);
  Error(cString& msg);
  Error(cchar*, ...);
  Error(const Error& err);
  
  virtual ~Error();
  
  Error& operator=(const Error& e);
  
  /**
   * @func message
   */
  virtual cString& message() const throw();

  /**
   * @func code
   */
  virtual int code() const throw();

  /**
   * @func set_code
   */
  void set_code(int value);

  /**
   * @func set_message
   */
  void set_message(cString& value);

private:
  
  int     m_code;
  String* m_message;
};

typedef const Error cError;

av_end

#else

#define av_throw avocado::fatal()
#define av_assert_err(cond, ...) if(!(cond)) avocado::fatal()
#define av_err
#define av_ignore_err(block) block ((void) 0)

#endif
#endif
