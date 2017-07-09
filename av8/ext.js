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

var slice = Array.prototype.slice;

function illegal_operation() {
  throw new Error('Illegal operation');
}

/**
 * @fun ext_class #  EXT class prototype objects
 */
function extend(obj, extd) {
  for (var i in extd) {
    var desc = Object.getOwnPropertyDescriptor(extd, i);
    desc.enumerable = false;
    Object.defineProperty(obj, i, desc);
  }
}

extend(Function.prototype, {
  
  catch: function(catch_func) {
    this.throw = catch_func;
    this.catch = illegal_operation;
    return this;
  },
  
  /**
   * @fun err # 捕获回调异常
   * @arg cb {Function}
   */
  err: function(cb) {
    if (cb)
      return this.catch(cb.throw);
    return this;
  },
  
  /**
   * @fun throw # 抛出异常到给回调函数
   * @arg e {Object}
   */
  throw: function(e) {
    throw e;
  },
  
  /**
   * @fun finally
   * @arg finally_func {Object}
   */
  finally: function(finally_func) {
    var self = this;
    
    var fun = function () {
      var rev = self.apply(this, arguments);
      finally_func.apply(this, [null].concat(slice.call(arguments)));
      return rev;
    };
    
    fun.throw = function (err) {
      self.throw(err);
      finally_func(err);
    };
    
    fun.catch = function (catch_func) {
      self.catch(catch_func);
      return fun;
    };
    
    fun.err = function (cb) {
      self.err(cb);
      return fun;
    };
    
    fun.finally = illegal_operation;
    
    return fun;
  },
  
  /**
    * @func setTimeout 延迟执行函数单位毫秒
    * @arg time {Number}  要延迟时间长度单位(毫秒)
    * @arg ...args        提前传入的参数1
    */
  setTimeout: function(time, ...args) {
    return setTimeout(()=> {
      this(...args);
    }, time);
  },
  
});

extend(Array, {
  toArray: function (obj, index, end) {
    return slice.call(obj, index, end);
  },
});

extend(Array.prototype, {
  /**
   * 倒叙索引数组元素
   */
  last: function (index) {
    return this[this.length - 1 - index];
  },
});

extend(String, {
  format: function(str) {
    str = String(str);
    return str.format.apply(str, Array.toArray(arguments).splice(1));
  }
});

extend(String.prototype, {
/**
 * var str = 'xxxxxx{0}xxxxx{1}xxxx{2},xxx{0}xxxxx{2}';
 * var newStr = str.format('A', 'B', 'C');
 * @ret : xxxxxxAxxxxxBxxxxC,xxxAxxxxxB
 */
  format: function(...args) {
    for (var i = 0, val = this, len = args.length; i < len; i++)
      val = val.replace(new RegExp('\\{' + i + '\\}', 'g'), args[i]);
    return val;
  }
});

extend(Number.prototype, {
  /**
  * 转换为前后固定位数的字符串
  * @arg before {Number}  小数点前固定位数
  * @arg [after] {Number} 小数点后固定位数
  */
  toFixedBefore: function (before, after) {
    if (!isFinite(this)) {
      return String(this);
    } else {
      var num = typeof after == 'number' ? this.toFixed(after) : String(this);
      var match = num.match(/^(\d+)(\.\d+)?$/);
      var integer = match[1];
      var len = before - integer.length;
      if (len > 0)
        num = new Array(len + 1).join('0') + num;
      return num;
    }
  }
});

extend(Date.prototype, {
  /**
   * 给当前Date时间追加毫秒,改变时间值
   * @arg ms {Number}  要添追加的毫秒值
   * @ret {Date}
   */
  addms: function (ms) {
    this.setMilliseconds(this.getMilliseconds() + ms);
    return this;
  }
});
