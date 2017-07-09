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
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIeBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

var _util        = binding('_util');
var EventNoticer = binding('_event').EventNoticer;
var currentTimezone = new Date().getTimezoneOffset() / -60; // 当前时区
var default_throw = Function.prototype.throw;
var config  = null;
var main_lib  = null;
var onuncaught_exception  = null;
var onexit = null;

//
// util
// ======
//
function obj_constructor () { }

function clone_object (new_obj, obj) {
  var names = Object.getOwnPropertyNames(obj);
  for (var i = 0, len = names.length; i < len; i++) {
    var name = names[i];
    var property = Object.getOwnPropertyDescriptor(obj, name);
    if (property.writable) {
      new_obj[name] = clone(property.value);
    }
    else {
      // Object.defineProperty(new_obj, name, property);
    }
  }
  return new_obj;
}

function clone (obj) {
  if (obj && typeof obj == 'object') {
    var new_obj = null, i;
    
    switch (obj.constructor) {
      case Object:
        new_obj = { };
        for(i in obj) {
          new_obj[i] = clone(obj[i]);
        }
        return new_obj;
      case Array:
        new_obj = [ ];
        for (i = 0; i < obj.length; i++) {
          new_obj[i] = clone(obj[i]);
        }
        return new_obj;
      case Date:
        return new Date(obj.valueOf());
      default:
        obj_constructor.prototype = obj.constructor.prototype;
        new_obj = new obj_constructor();
        return clone_object(new_obj, obj);
    }
  }
  return obj;
}

function ext_class(obj, extd) {
  if (extd.__proto__ && extd.__proto__ !== Object.prototype) {
    ext_class(obj, extd.__proto__);
  }
  var names = Object.getOwnPropertyNames(extd);
  for (var i of names) {
    if (i != 'constructor') {
      var desc = Object.getOwnPropertyDescriptor(extd, i);
      desc.enumerable = false;
      Object.defineProperty(obj, i, desc);
    }
  }
}

// index of
function index_of(str, str1) {
  var index = str.indexOf(str1);
  return index > -1 ? index : 1e9;
}

/**
 * 解析字符串为时间
 * <pre><code>
 * var i = '2008-02-13 01:12:13';
 * var date = Date.parse_date(i); //返回的新时间
 * </code></pre>
 * @func parse_date(str[,format[,timezone]])
 * @arg str {String}        要解析的字符串
 * @arg [format] {String}   date format   default yyyyMMddhhmmssfff
 * @arg [timezone] {Number} 要解析的时间所在时区,默认为当前时区
 * @ret {Date}              返回新时间
 */
function parse_date(date, format, timezone) {

  date = date.replace(/[^0-9]/gm, '');
  format = format || 'yyyyMMddhhmmssfff';

  var l = date.length;
  var val;
  
  if (timezone === undefined)
    timezone = currentTimezone;

  return new Date(
    date.substr(index_of(format, 'yyyy'), 4) || 0,
    (date.substr(index_of(format, 'MM'), 2) || 1) - 1,
    date.substr(index_of(format, 'dd'), 2) || 1,
    (date.substr(index_of(format, 'hh'), 2) || 0) - currentTimezone + timezone,
    date.substr(index_of(format, 'mm'), 2) || 0,
    date.substr(index_of(format, 'ss'), 2) || 0,
    date.substr(index_of(format, 'fff'), 3) || 0
  );
}

 /**
  * 格式化时间戳(单位:毫秒)
  * <pre><code>
  * var x = 10002100;
  * var format = 'dd hh:mm:ss';
  * var str = Date.format_time_span(x, format); // str = '0 2:46:42'
  * var format = 'dd hh:mm:ss';
  * var str = Date.format_time_span(x, format); // str = '0天2时46分42秒'
  * format = 'hh时mm分ss秒';
  * str = Date.format_time_span(x, format); // str = '2时46分42秒'
  * format = 'mm分ss秒';
  * str = Date.format_time_span(x, format); // str = '166分42秒'
  * </code></pre>
  * @func format_time_span(ts[,format])
  * @arg ts {Number} 要格式化的时间戳
  * @arg [format]  {String} 要格式化的时间戳格式
  * @ret {String} 返回的格式化后的时间戳
  */
function format_time_span(x, format) {
  
  format = format || 'dd hh:mm:ss';

  var data = [];
  var items = [
    [1, 1000, /fff/g],
    [1000, 60, /ss/g],
    [60, 60, /mm/g],
    [60, 24, /hh/g],
    [24, 1, /dd/g]
  ];
  
  var start = false;

  for (var i = 0; i < 5; i++) {
    var item = items[i];
    var reg = item[2];

    if (format.match(reg)) {
      start = true;
    }
    else if (start) {
      break;
    }
    x = x / item[0];
    data.push([x % item[1], x]);
  }

  if (!start) {
    return format;
  }

  data.last(0).reverse();
  data.forEach(function (item, index) {
    format =
      format.replace(items[index][2], Math.floor(item[0]).to_fixed_before(2));
  });
  return format;
};

/**
 * @func array_delete_value(arr,val) 移除指定值元素
 * @arg arr {Array}
 * @arg val {Object}
 */
function array_delete_value (arr, val) {
  var i = arr.indexOf(val);
  if (i != -1) {
    arr.splice(i, 1);
  }
};

/**
 * 查询数组元素指定属性名称的值是否与val相等,如果查询不匹配返回-1
 * @func array_inl_index_of(arr,name,val)
 * @arg arr {Array} 
 * @arg name {String}  数组元素的属性名
 * @arg val {Object}   需要查询的值
 * @ret {Number}     返回数组索引值
 */
function array_inl_index_of(arr, name, val, from_index) {
  var len = arr.length;
  if (!len) {
    return -1;
  }
  var i = from_index ? ((from_index % len) + len) % len: 0;
  for ( ;i < len; i++) {
    if (arr[i][name] === val) {
      return i;
    }
  }
  return -1;
};

/**
 * 从后面开始查询数组元素指定属性名称的值是否与val相等,如果查询不匹配返回-1
 * @func array_inl_last_index_of(arr,name,val)
 * @arg arr {Array}
 * @arg name {String} 数组元素的属性名
 * @arg val {Object}  需要查询的值
 * @ret {Number}  返回数组索引值
 */
function array_inl_last_index_of(arr, name, val, from_index) {
  var len = arr.length;
  if (!len) {
    return -1;
  }
  var i = from_index ? ((from_index % len) + len) % len: 0;
  for ( ;i > -1; i--) {
    if (arr[i][name] === val) {
      return i;
    }
  }
  return -1;
};

/**
  * 给定日期格式返回日期字符串
  * <pre><code>
  * var date = new Date();
  * var format = 'yyyy-MM-dd hh:mm:ss.fff 星期D';
  * var dateStr = date.toString(format); // dateStr的值为 '2008-12-10 10：32：23 星期六'
  * format = 'yyyy-MM-dd hh:mm:ss 日期D D2 D3';
  * dateStr = date.toString(format); // dateStr的值为 '2008-12-10 10：32：23 星期六 Saturday 6'
  * format = 'yyyy/MM/dd hh:mm:ss';
  * dateStr = date.toString(format); // dateStr的值为 '2008/12/10 10：32：23'
  * format = 'yyyy/MM/dd';
  * dateStr = date.toString(format); // dateStr的值为 '2008/12/10'
  * format = 'yyyy-MM-dd hh';
  * dateStr = date.toString(format); // dateStr的值为 '2008-12-10 10'
  * </code></pre>
  * @func date_to_string(date[,foramt])
  * @arg date {Date}
  * @arg [format] {String} 要转换的字符串格式
  * @ret {String} 返回格式化后的时间字符串
  */
function date_to_string(date, format, time_zone) {
  if (format) {
    var i = [
      ['日', 'Sunday', 0],
      ['一', 'Monday', 1],
      ['二', 'Tuesday', 2],
      ['三', 'Wednesday', 3],
      ['四', 'Thursday', 4],
      ['五', 'Friday', 5],
      ['六', 'Saturday', 6]
    ][date.getDay()];
    
    var d = new Date(date.valueOf());
    
    if (typeof time_zone == 'number') {
      var cur_time_zone = d.getTimezoneOffset() / -60;
      var offset = time_zone - cur_time_zone;
      d.setHours(d.getHours() + offset);
    }
    
    return format.replace('yyyy', d.getFullYear())
      .replace('MM', (d.getMonth() + 1).toFixedBefore(2))
      .replace('dd', d.getDate().toFixedBefore(2))
      .replace('hh', d.getHours().toFixedBefore(2))
      .replace('HH', d.getHours().toFixedBefore(2))
      .replace('mm', d.getMinutes().toFixedBefore(2))
      .replace('ss', d.getSeconds().toFixedBefore(2))
      .replace('fff', d.getMilliseconds().toFixedBefore(3))
      .replace('D2', i[1])
      .replace('D3', i[2])
      .replace('D', i[0]);
  } else {
    return date.toString(format, time_zone);
  }
}

exports.__proto__ = _util;

var util = exports;

export {
  cb                : util.cb,
  noop              : util.noop,
  is_array          : Array.isArray,
  to_array          : Array.toArray,
  format_string     : String.format,
  parse_date        : parse_date,
  format_time_span  : format_time_span,
  timezone          : currentTimezone,
  date_to_string    : date_to_string,
  array_delete_value: array_delete_value,
  array_inl_index_of: array_inl_index_of,
  array_inl_last_index_of: array_inl_last_index_of,
  
  /**
   * @func is_default_throw
   */
  is_default_throw: function(func) {
    return default_throw === func.throw;
  },
  
  /**
   * @fun get(name[,self]) # get object value by name
   * @arg name {String} 
   * @arg [self] {Object}
   * @ret {Object}
   */
  get: function (name, self) {
    var names = name.split('.');
    var item;
    self = self || global;

    while ( (item = names.shift()) ) {
      self = self[item];
      if (!self)
        return self;
    }
    return self;
  },

 /**
  * @fun set(name,value[,self]) # Setting object value by name
  * @arg name {String} 
  * @arg value {Object} 
  * @arg [self] {Object}
  * @ret {Object}
  */
  set: function (name, value, self) {
    self = self || global;
    var item = null;
    var names = name.split('.');
    name = names.pop();
    
    while ( (item = names.shift()) ){
      self = self[item] || (self[item] = {});
    }
    self[name] = value;
    return self;
  },

  /**
   * @fun def(name[,self]) # Delete object value by name
   * @arg name {String} 
   * @arg [self] {Object}
   */
  del: function (name, self) {
    var names = name.split('.');
    name = names.pop();
    self = util.get(names.join('.'), self || global);
    if (self)
      delete self[name];
  },
  
  /**
   * @fun random # 创建随机数字
   * @arg [start] {Number} # 开始位置
   * @arg [end] {Number}   # 结束位置
   * @ret {Number}
   */
  random: function (start, end) {
    var r = Math.random();
    start = start || 0;
    end = end || 1E8;
    return Math.floor(start + r * (end - start + 1));
  },
  
  /**
  * @fun fix_random # 固定随机值,指定几率返回常数
  * @arg args.. {Number} # 输入百分比
  * @ret {Number}
  */
  fix_random: function () {
    var total = 0;
    var argus = [];
    var i = 0;
    
    var len = arguments.length;
    for (; (i < len); i++) {
      var e = arguments[i];
      total += e;
      argus.push(total);
    }

    var r = util.random(0, total - 1);
    for (i = 0; (i < len); i++) {
      if (r < argus[i])
        return i;
    }
  },

  /**
   * @fun clone # 克隆一个Object对像
   * @arg obj {Object} # 要复制的Object对像
   * @arg {Object}
   */
  clone: clone,

  /**
   * @fun wrap
   * @ret {Object}
   */
  wrap: function (o) {
    return { __proto__: o };
  },
  
	/**
		* @fun filter # object filter
		* @arg obj {Object}  
		* @arg exp {Object}  #   filter exp
		* @arg non {Boolean} #   take non
		* @ret {Object}
		*/
	filter: function (obj, exp, non) {
		var rev = { };
		var isfn = (typeof exp == 'function');
    
    if (isfn || non) {
        for (var key in obj) {
        	var value = obj[key];
        	var b = isfn ? exp(key, value) : (exp.indexOf(key) != -1);
        	if (non ? !b : b)
        		rev[key] = value;
        }
    } else {
      exp.forEach(function (item) {
        item = String(item);
        if (item in obj)
          rev[item] = obj[item];
      });
    }
		return rev;
	},
  
  /**
   * @fun values # 获取对像属性值列表
   * @arg {Object} obj
   * @ret {Array}
   */
  values: function (obj) {
    var result = [];
    for (var i in obj)
      result.push(obj[i]);
    return result;
  },
  
  /**
   * @fun keys # 获取对像属性键列表
   * @arg obj {Object} 
   * @ret {Array}
   */
  keys: function (obj) {
    var result = [];
    for (var i in obj)
      result.push(i);
    return result;
  },
  
  /**
   * @fun update # update object property value
   * @arg obj {Object}      #        need to be updated for as
   * @arg extd {Object}    #         update object
   * @arg {Object}
   */
  update: function (obj, extd) {
    for (var key in extd) {
      if (key in obj) {
        obj[key] = util.default(extd[key], obj[key]);
      }
    }
    return obj;
  },
  
  /**
   * @fun default
   * @arg value   {Object} 
   * @arg default {Object} 
   * @reg {Object}
   */
  'default': function (value, default_) {
    if (value === undefined || value === null) {
      return default_;
    } else if (
      default_ === undefined || default_ === null ||
      value.constructor === default_.constructor ||
      value instanceof default_.constructor) 
    {
      return value;
    } else {
      return default_;
    }
  },
  
  /**
   * @fun ext_class #  EXT class prototype objects
   */
  ext_class: function (cls, extd) {
    var proto = cls.prototype;
    if (extd instanceof Function) {
      extd = extd.prototype;
    }
    ext_class(proto, extd);
  },
  
  /**
   * @fun equals_class  # Whether this type of sub-types
   * @arg baseclass {class}
   * @arg subclass {class}
   */
  equals_class: function (baseclass, subclass) {
    if (!baseclass || !subclass) return false;
    if (baseclass === subclass) return true;
    
    var prototype = baseclass.prototype;
    var obj = subclass.prototype.__proto__;
    
    while (obj) {
      if (prototype === obj)
        return true;
      obj = obj.__proto__;
    }
    return false;
  },
  
  /**
   * @fun assert
   */
  assert: function (is) {
    if (is)
      return;
    var args = util.to_array(arguments).slice(1);
    if (args.length)
      throw new Error(util.format_string.apply(null, args));
    else
      throw new Error('assert');
  },
  
  /**
   * @fun finally # create none cb fun
   * @arg finally_fun {Function}
   * @ret {Function}
   */
  finally: function (finally_fun) {
    return function () { }.catch(function () { }).finally(finally_fun);
  },
  
  /**
   * @fun err_to
   * @arg err {Error}
   * @ret {Object}
   */
  err_to: function (err) {
    if ( err ) {
      if ( typeof err == 'string' ) {
        return { message: err || 'unknown error', code: 0, name: '', description: '' };
      } else if ( typeof err == 'number' ) {
        return { message: 'unknown error', code: err, name: '', description: '' };
      } else {
        var r = util.wrap(err);
        r.code = r.code || 0;
        r.name = r.name || '';
        r.description = r.description || '';
        r.message = r.message || 'unknown error';
        return r;
      } 
    } else {
      return { message: 'unknown error', code: 0, name: '', description: '' };
    }
  },

  /**
   * @get config # 获取配置
   */ 
  get config() {
    if (!config || main_lib !== util.libs.main_lib) { // 更改main_lib 配置会发生变化
      main_lib = util.libs.main_lib; // 使用主要lib配置
      config = { };
      if (main_lib) {
        try {
          config = __req(':' + main_lib.name + '/config.keys');
        } catch(err) { }
      }
    }
    return config;
  },

  // onuncaught_exception event
  get onuncaught_exception() {
    if ( !onuncaught_exception ) {
      onuncaught_exception = new EventNoticer('uncaught_exception');
      __lib.onclear.on(function () {
        onuncaught_exception = null;
        util._uncaught_exception = null;
      });
      util._uncaught_exception = function(err) {
        return onuncaught_exception.trigger(err);
      };
    }
    return onuncaught_exception;
  },

  // exit event
  get onexit() {
    if ( !onexit ) {
      onexit = new EventNoticer('before_exit');
      __lib.onclear.on(function (evt) {
        onexit = null;
        util._exit = null;
      });
      util._exit = function() {
        return onexit.trigger();
      };
    }
    return onexit;
  }
  
  // @end
};
