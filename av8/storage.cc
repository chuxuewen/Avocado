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

#include "av8.h"
#include "autil/localstorage.h"

/**
 * @ns avocado::av8
 */

av8_begin

/**
 * @class NativeStorage
 */
class NativeStorage {
  
  /**
   * @func get(key)
   * @arg key {String}
   * @ret {String}
   */
  static void get(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func get(key)\n"
        "* @arg key {String}\n"
        "* @ret {String}\n"
      );
    }
    av8_return( localstorage_get( worker->to_string_utf8(args[0])) );
  }
  
  /**
   * @func set(key, value)
   * @arg key {String}
   * @arg value {String}
   */
  static void set(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 2) {
      av8_throw_err(
        "* @func get(key)\n"
        "* @arg key {String}\n"
        "* @arg value {String}\n"
      );
    }
    localstorage_set( worker->to_string_utf8(args[0]), worker->to_string_utf8(args[1]) );
  }
  
  /**
   * @func del(key)
   * @arg key {String}
   */
  static void del(FunctionCall args) {
    av8_worker(args);
    if (args.Length() < 1) {
      av8_throw_err(
        "* @func del(key)\n"
        "* @arg key {String}\n"
      );
    }
    localstorage_delete( worker->to_string_utf8(args[0]) );
  }

  static void clear(FunctionCall args) {
    localstorage_clear();
  }
  
public:
  
  static void binding(Local<v8::Object> exports, Worker* worker) {
    av8_set_method(get, get);
    av8_set_method(set, set);
    av8_set_method(del, del);
    av8_set_method(clear, clear);
  }
};

av8_reg_module(_storage, NativeStorage);
av8_end
