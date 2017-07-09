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

#include "./av8.h"
#include "autil/jsa.h"

/**
 * @ns avocado::av8
 */

av8_begin

/**
 * @class NativeJsa
 */
class NativeJsa {
  
  static void transform(FunctionCall args, bool jsax) {
    av8_worker(args);
    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
      av8_throw_err("Bad argument");
    }
    Ucs2String rv;
    Ucs2String in = worker->to_string_ucs2(args[0]);
    String path = worker->to_string_utf8(args[1]);
    av8_try_catch({
      if (jsax) {
        rv = Jsa::transform_jsax(in, path);
      } else {
        rv = Jsa::transform_jsa(in, path);
      }
    }, Error);
  
    av8_return( rv );
  }
  
  static void transform_jsax(FunctionCall args) {
    transform(args, true);
  }
  
  static void transform_jsa(FunctionCall args) {
    transform(args, false);
  }
  
public:
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_set_method(transform_jsax, transform_jsax);
    av8_set_method(transform_jsa, transform_jsa);
  }
};

av8_reg_module(_jsa, NativeJsa)
av8_end
