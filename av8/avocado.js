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

var util = binding('_util');
var parse_keys = binding('_keys').parse;
var _jsa = binding('_jsa');
var _http = binding('_http');
var _fs = binding('_fs');
var { read, read_sync, exists, ls } = _fs.reader;
var _path = binding('_path');
var EventNoticer = binding('_event').EventNoticer;
var compile = util.run_script;
var cwd = _path.cwd; // func
var is_win = binding('_sys').WIN;
var debug = util.debug; // debug status
var options = { };      // start options
var _libs = null; // libs
var _libs_export = null;
var join = Array.prototype.join;
var slice = Array.prototype.slice;
var cur_time = new Date().valueOf();
var absolute_import_obj = { };

var print_err = function (err) {
  if (err instanceof Error) {
    console.error(err.message);
  } else {
    console.error(err);
  }
};

/**
 * Empty function
 */
function noop() { }

/**
 * @fun extend # Extended attribute from obj to extd
 * @arg obj   {Object} 
 * @arg extd  {Object}
 * @ret       {Object}
 */
function extend(obj, extd) {
  for (var name in extd) {
    obj[name] = extd[name];
  }
  return obj;
}

/**
 * @fun err # create error object
 * @arg e {Object}
 * @ret {Error}
 */
function new_err(e) {
  if (! (e instanceof Error)) {
    if (typeof e == 'object') {
      e = extend(new Error(e.message || 'Unknown error'), e);
    } else {
      e = new Error(e);
    }
  }
  return e;
}

/**
 * @fun cb # return default callback
 * @ret {Function}
 */
function new_cb(cb) {
  return cb || function () { };
}

/**
 * @fun throw # 抛出异常
 * @arg err {Object}
 * @arg [cb] {Function} # 异步回调
 */
function throw_err(e, cb) {
  new_cb(cb).throw(new_err(e));
}

/** 
 * format part 
 */
function format_part_path(path, retain_up) {
  var ls = path.split('/');
  var rev = [];
  var up = 0;
  for (var i = ls.length - 1; i > -1; i--) {
    var v = ls[i];
    if (v && v != '.') {
      if (v == '..') // set up
        up++;
      else if (up === 0) // no up item
        rev.push(v);
      else // un up
        up--;
    }
  }
  path = rev.reverse().join('/');
  
  return (retain_up ? new Array(up + 1).join('../') + path : path);
}

/**
 * return format path
 */
function format() {
  for (var i = 0, ls = []; i < arguments.length; i++) {
    var item = arguments[i];
    if (item)
      ls.push(is_win ? item.replace(/\\/g, '/') : item);
  }
  
  var path = ls.join('/');
  var prefix = '';
  var reg = /^((\/)|([a-z]{2,}:\/\/[^\/]+)|:([^\/]+)|((file|zip):\/\/\/))/i;
  if (is_win) {
    reg = /^((\/)|([a-z]{2,}:\/\/[^\/]+)|:([^\/]+)|((file|zip):\/\/\/([a-z]:(?![^\/]))?)|([a-z]:(?![^\/])))/i;
  }
  // Find absolute path
  var mat = path.match(reg);
  
  if (mat) {
    if (mat[2]) { // local absolute path
      if (is_win) {
        prefix = cwd().substr(0, 11);
      } else {
        prefix = 'file:///';
      }
    } else {
      if (mat[4]) { // lib://xxx/
        prefix = 'lib://' + mat[4] + '/';
        path = path.substr(mat[0].length);
      } else {
        if (mat[5]) { // local file protocol
          if (mat[7]) // windows path and volume label
            prefix = mat[5] + '/';
          else // unix path
            prefix = mat[5];
        } else { // network protocol and windows local file
          prefix = mat[0] + '/';
        }
        if (prefix >= path.length) //
          return prefix;
        path = path.substr(prefix.length);
      }
    }
  } else { // Relative path, no network protocol
    var _cwd = cwd();
    if (is_win) { // windows lable name
      // file:///c:/
      prefix = _cwd.substr(0, 11);
    } else {
      prefix = 'file:///';
    }
    path = _cwd.substr(prefix.length) + '/' + path;
  }

  path = format_part_path (path);
  return prefix + path;
}

/**
 * @func is_absolute # 是否为绝对路径
 */
function is_absolute(path) {
  return /^([\/\\]|[a-z]{2,}:\/\/[^\/]+|:([^\/]+)|(file|zip):\/\/\/|[a-z]:[\/\\])/i.test(path);
}

/**
 * @func is_local # 是否为本地路径
 */
function is_local(path) {
  return /^([\/\\]|(file|zip):\/\/\/|[a-z]:[\/\\])/i.test(path);
}

function is_local_zip(path) {
  return /^zip:\/\/\//i.test(path);
}

function is_network(path) {
  return /^(https?):\/\/[^\/]+/i.test(path);
}

function is_lib_path(path) {
  return /^(lib:\/\/|:)/.test(path);
}

// add url args
function set_url_args(path, arg) {
  if (/^(https?):\/\//i.test(path)) {
    var args = [];
    var url_arg = arg || options.url_arg;
    if ( url_arg ) {
      return path + (path.indexOf('?') == -1 ? '?' : '&') + url_arg;
    }
  }
  return path;
}

// @private
function new_event_noticer(self, name) {
  return new EventNoticer(name, self);
}

/**
 * @func read_text
 */
function read_text(path, cb) {
  return read(path, function(buff) { cb(buff.to_string('utf8')); }.catch(cb.throw));
}

/**
 * @func read_text_sync
 */
function read_text_sync(path) {
  return read_sync(path).to_string('utf8');
}

/**
 * @class __bind data
 */
class __bind {
  constructor(func, once) {
    this.once = once;
    this.exec = func;
  }
}

// ====================== Lib =====================

function Lib_install_with_path(self, dirname, is_lib, cb) {
  // 读取lib.keys文件
  var config = null;
  var path = set_url_args(dirname + '/lib.keys', self.m_version_code);
  
  var read_versions_ok = function(data) {
    data = JSON.parse(data);
    if(self.m_build) {
      self.m_versions = Object.create(data.versions);
      self.m_lib_files = Object.create(data.lib_files || null);  // .lib 中包含的文件列表
    } else {
      self.m_versions = Object.create(data.versions);
    }
    self.m_install = true;
    cb && cb(); // ok
  }.catch(new_cb(cb).throw);
  
  var read_lib_keys_ok = function(data) {
    config        = self.m_config = parse_keys(data);
    config.src    = config.src || '';
    self.m_src    = format(self.m_path, config.src);
    if (config.name != self.m_name) {
      return throw_err('Lib name must be ' +
                       `consistent with the folder name, ${self.m_name} != ${config.name}`, cb);
    }
    
    if (self.m_build) {
      path = set_url_args(dirname + '/versions.json', self.m_version_code);
    } else {
      if (typeof config.versions != 'string' || !/\.json$/i.test(config.versions)) {
        self.m_install = true;
        return cb && cb(); // ok
      }
      // 下载lib内部文件版本信息,这个文件可由工具生成,versions为文件路径
      path = set_url_args(dirname + '/' + config.versions, self.m_version_code);
    }
    
    cb ? read_text(path, read_versions_ok) : read_versions_ok(read_text_sync(path));
  }.catch(new_cb(cb).throw);
  
  cb ? read_text(path, read_lib_keys_ok) : read_lib_keys_ok(read_text_sync(path));
}

function Lib_install_remote(self, cb) {
  // 如果本地不存在相应版本的文件,下载远程.lib文件到本地.
  // 远程.avlib文件必须存在否则抛出异常,并且不使用备选2方式
  var path = _path.temp(`${self.m_name}.avlib.${self.m_version_code}`);
  
  if (_fs.exists_sync(path)) { // 文件存在,无需下载
    // 设置一个本地zip文件读取协议路径,使用这种路径可直接读取zip内部文件
    self.m_lib_path = `zip:///${path.substr(8)}@`;  // file:///
    Lib_install_with_path(self, self.m_lib_path, true, cb);
  } else {
    var url = set_url_args(`${self.m_path}/${self.m_name}.avlib`, self.m_version_code);
    var tmp = path + '.~';
    // TODO 文件比较大时需要断点续传下载
    // TODO 还应该使用读取数据流方式,实时回调通知下载进度
    if (cb) {
      _http.download(url, tmp, function() { // 下载成功
        _fs.mv_sync(tmp, path);
        self.m_lib_path = `zip:///${path.substr(8)}@`; // file:///
        Lib_install_with_path(self, self.m_lib_path, true, cb);
      }.catch(cb.throw));
    } else {
      _http.download_sync(url, tmp);
      _fs.mv_sync(tmp, path);
      self.m_lib_path = `zip:///${path.substr(8)}@`; // file:///
      Lib_install_with_path(self, self.m_lib_path, true);
    }
  }
}

function Lib_install_old(self, cb) {
  var old = self.m_old;
  var install_ok = function() {
    // 保存旧文件版本信息
    old.src = self.m_src; 
    old.versions = self.m_versions;
    old.lib_files = self.m_lib_files;
    cb && cb();
  }.catch(new_cb(cb).throw);
  
  old.lib_path = '';
  var is_lib = false;
  
  if ( ! is_local_zip(old.path) ) {
    is_lib = _fs.exists_sync(`${old.path}/${self.m_name}.avlib`);
    if (is_lib) {
      old.lib_path = `zip:///${old.path.substr(8)}/${self.m_name}.avlib@`;
    }
  }
  cb ? Lib_install_with_path(self, old.path, is_lib, install_ok) : 
        install_ok(Lib_install_with_path(self, old.path, is_lib));
}

// install
function Lib_install2(self, cb) {
  if (self.m_build) { // lib明确声明为已build状态
    if (self.m_local) { // 本地lib
      //
      if ( self.m_old ) { // 先载入本地旧包,然后载入远程origin包
        if (cb) {
          Lib_install_old(self, function() {
            Lib_install_remote(self, cb);
          }.catch(cb.throw));
        } else {
          Lib_install_old(self); // 读取旧lib中的信息也许有些离散文件还可以继续使用,降低网络消耗
          Lib_install_remote(self);
        }
        
      } else {
        var is_lib = false;
        /*
         * build的lib有两种格式
         * 1.avlib根目录存在.avlib压缩文件,文件中包含全部文件版本信息与一部分源码文件以及资源文件.
         * 2.avlib根目录不存在.avlib压缩文件,相比build前只多出文件版本信息,适用于android/ios安装包中存在.
         */
         /* 文件读取器不能读取zip内的.avlib文件. 
          * 比如无法android资源包中的.avlib文件 
          * 所以这里的lib不能存在.avlib格式文件只能为普通文件
          */
        if ( ! is_local_zip(self.m_path) ) { // 非zip内路径
          is_lib = _fs.exists_sync(`${self.m_path}/${self.m_name}.avlib`);
          if (is_lib) { // 文件存在
            // 设置一个本地zip文件读取协议路径
            // zip:///temp/test.avlib@
            self.m_lib_path = `zip:///${self.m_path.substr(8)}/${self.m_name}.avlib@`;
          }
        }
        Lib_install_with_path(self, self.m_path, is_lib, cb);
      }
    } else { //  单纯的远程.avlib, 远程包一定都是.avlib
      Lib_install_remote(self, cb);
    }
  } else {
    Lib_install_with_path(self, self.m_path, false, cb);
  }
}

// install
function Lib_install(self, cb) {
  if ( self.m_install ) { // 这里可以不使用锁定,因为释放代码只会加载一次
    return cb && cb();
  }
  if (cb) { // async
    Lib_install2(self, function () {
      if (_libs.m_oninstall) {
        _libs.m_oninstall.trigger(self); // 安装事件
      }
      cb();
    }.catch(cb.throw));
  } else {
    Lib_install2(self);
    if (_libs.m_oninstall) {
      _libs.m_oninstall.trigger(self); // 安装事件
    }
  }
}

// 获取当前lib内路径
function Lib_get_path(self, path) {
  var ver = self.m_versions[path];
  var _p;
  if ( self.m_lib_files[path] ) { // 使用.avlib
    _p = self.m_lib_path + '/' + path;
  } else { // 普通文件
    _p = self.m_src + '/' + path;
  }
  if (self.m_old) { // 读取本地旧文件
    var old = self.m_old;
    if ( old.versions[path] == ver ) { // 版本相同,可以使用旧文件路径
      if ( old.lib_files[path] ) {
        _p = old.lib_path + '/' + path;
      } else {
        _p = old.src + '/' + path;
      }
    }
  }
  return set_url_args(_p, ver);
}

function Lib_get_path2(self, dir, name) {
  var mat = name.match(/^(lib:\/\/|:)([^\/\.]+)(\/.*)?$/);
  if (mat) {
    var lib = _libs_export.get_lib( mat[2] );
    if (lib) {
      Lib_install(lib);
      return Lib_get_path(lib, mat[3] ? format_part_path(mat[3]) : '');
    }
  } else if ( is_absolute(name) ) {
    var o = _libs_export.get_relative_path(name);
    if (o) {
      return Lib_get_path(o.lib, o.name);
    }
  } else { // lib path
    return Lib_get_path(self, format_part_path(dir + '/' + name));
  }
  return format(name);
}

function get_path(path) {
  var mat = path.match(/^(lib:\/\/|:)([^\/\.]+)(\/.*)?$/);
  if (mat) {
    var lib = _libs_export.get_lib( mat[2] );
    if (lib) {
      Lib_install(lib);
      return Lib_get_path(lib, mat[3] ? format_part_path(mat[3]) : '');
    }
  } else if (is_absolute(path)) {
    var o = _libs_export.get_relative_path(path);
    if (o) {
      return Lib_get_path(o.lib, o.path);
    }
  }
  return format(path);
}

function __export(exports, extd) {
  for (var name of Object.getOwnPropertyNames(extd)) {
    Object.defineProperty(exports, name, Object.getOwnPropertyDescriptor(extd, name));
  }
}

function Lib_run_sandbox(self, exports, path) {
  var index = path.lastIndexOf('/');
  var dir = ( index == -1 ? '' : path.substr(0, index) );
  var sandbox = {
    global        : global,
    exports       : exports,
set __export(value) { __export(sandbox.exports, value) },
    __bind        : __bind,
    __filename    : path,
    __dirname     : dir,
    __lib         : self,
    __lib_path    : self.m_path,
    __lib_src     : self.m_src,
    __lib_name    : self.m_name,
    __lib_config  : self.m_config,
    __req         : function(path) { return inl_import(self, dir, path) },
    __path        : function(name) { return Lib_get_path2(self, dir, name) },
    $             : function(name) { return Lib_get_path2(self, dir, name) },
  };
  sandbox.sandbox = sandbox;
  return sandbox;
}

function set_property(obj, name, desc) {
  if (desc.type == 1) {// 属性访问
    Object.defineProperty(obj, name, desc.property);
  } else {
    obj[name] = desc.value;
  }
}

//返回成员信息,0为普通字段,1为属性访问器,2为函数,3为不存在
function get_own_p_desc(obj, name) {
  var type = 3;
  var value = null;
  var property = Object.getOwnPropertyDescriptor(obj, name);
  if (property) {
    if (property.writable) {
      value = property.value;
      type = typeof value == 'function' ? 2 : 0;
    } else {
      type = 1;  //属性访问器
    }
  }
  return { property: property, value: value, type: type, };
}

function Lib_get_exports(self, exports, sandbox) {
  var new_exports = sandbox.exports;
  if (new_exports !== exports) {
    for (var i in exports) {    // 扩展最终要输出
      if (!(i in new_exports))  // 没有找到才扩展
        set_property(new_exports, i, get_own_p_desc(exports, i));
    }
    // 扩展原始输出,在出现交叉导入的情况时原始输出会继续得到使用.
    // 所以扩展原始输出.
    exports.__proto__ = new_exports;
  }
  return new_exports;
}

function Lib_import2(self, path, suffix) {
  var _path = Lib_get_path(self, path);
  var self_exports = self.m_exports, exports = null;
  
  switch (suffix) {
    case '.js':
    case '.jsx': {
      self_exports[path] = exports = { };
      var code = read_text_sync(_path);
      if ( ! self.m_build) {
        if (suffix == '.js') { // jas
          code = _jsa.transform_jsa(code, _path);
        } else { // .jsax
          code = _jsa.transform_jsax(code, _path);
        }
      }
      var sandbox = Lib_run_sandbox(self, exports, path);
      compile(code, _path, sandbox);
      self_exports[path] = exports = Lib_get_exports(self, exports, sandbox);
      break;
    }
    case '.keys':  exports = parse_keys(read_text_sync(_path)); break;
    case '.json':  exports = JSON.parse(read_text_sync(_path)); break;
    default     :  exports = read_sync(_path); // read buffer
  }
  return exports;
}

/** 
 * 如果没有path, import main file 
 */
function Lib_import(self, path, suffix) {
  Lib_install(self);
  
  if (path) {
    path += suffix;
  } else { // 没有path
    path = self.m_config.main;
    if (path) {
      var mat = path.match(/[^\/]+?(\.[a-zA-Z]+)?$/);
      suffix = mat ? mat[1] : '';
    } else { // 没有main
      return { };
    }
  }
  
  path = format_part_path(path, true);
  var exports = self.m_exports[path];
  if (exports) return exports;
  
  suffix = suffix.toLowerCase();
  
  // .js,.jsx,.keys,.json
  exports = Lib_import2(self, path, suffix);
  
  // import事件
  if (_libs.m_onimport) {
    _libs.m_onimport.trigger({ lib: self, path: path, suffix: suffix, exports: exports });
  }
  return exports;
}

/**
 * @func Lib_replace_local 必需要install之前调用,安装后无法实现替换
 */
function Lib_replace_local(self, path, build, version_code) {
  if ( Lib_is_can_replace_local(self, path) ) {
    if ( build && version_code != self.m_version_code ) {
      if (self.m_build) {
        self.m_old = { path: path, version_code: self.m_version_code };
      }
      self.m_build = true;
      self.m_path = path;
      self.m_version_code = version_code;
    }
  }
}

/**
 * @func Lib_is_can_replace_local 是否可以替换
 */
function Lib_is_can_replace_local(self, path) {
  if ( !debug && !self.m_install && self.m_local ) {
    if (path == self.m_origin && path != self.m_path) {
      return true;
    }
  }
  return false;
}

// -------------------------- LibsCore private func --------------------------

// 注册更多相同名称的lib都没关系,最终都只使用最开始注册的lib
function LibsCore_register_lib_path(self, path) {
  // check lib path validity
  if ( /^(lib:|:)/i.test(path) ) {
    console.error(`Invalid lib path "${path}"`);
  } else {
    var path2 = format(path);
    if ( !self.m_libs_register[path2] ) {
      var mat = path2.match(/^(.+\/)?([a-z_$][a-z0-9_$]*)$/i);
      if ( mat ) {
        /* lib载入的状态,-1忽略/0未加载/1加载中/2加载完成/3异常 */
        self.m_libs_register[path2] = { 
          status: 0, path: path2, name: mat[2],
        };
        self.m_is_ready = false; /* 设置未准备状态 */
      } else {
        throw new Error(`Invalid lib path "${path}"`);
      }
    }
  }
}

function LibsCore_unregister_lib_path(self, path) {
  path = format(path)
  var register = self.m_libs_register[path];
  if (register) {
    register.status = -1;
    delete self.m_libs_register[path];
  }
}

function assert_origin(self, path) {
  var register = self.m_libs_register[format(path)];
  if ( !register ) {
    throw new Error('unregister "' + path + '"');
  }
  if (register.status != 0 || register.status != 1 ) { 
    throw new Error('It cannot be modified now origin "' + path + '"');
  }
  return register;
}

function LibsCore_set_origin(self, path, origin) {
  assert_origin(self, path).origin = origin || '';
}

function LibsCore_disable_origin(self, path, disable) {
  assert_origin(self, path).disable_origin = !!disable;
}

function LibsCore_depe_lib(self, path, depe) {
  for ( var i in depe ) { // 添加依赖路径
    if ( is_absolute(i) ) {
      LibsCore_register_lib_path(self, i);
    } else {
      LibsCore_register_lib_path(self, path + '/' + i);
    }
  }
}

function LibsCore_new_lib(self, path, name, 
                          is_build, version_code, origin) {
  var lib = self.m_libs[name];
  if (lib) { // Lib 实例已创建(尝试更新)
    Lib_replace_local(lib, path, is_build, version_code);
  } else {
    lib = new Lib(path, name, is_build, version_code, String(origin)); // 创建一个lib
    self.m_libs[name] = lib;
    
    if ( lib.m_origin ) { // reg origin lib and check update
      LibsCore_register_lib_path(self, lib.m_origin);
    }
  }
}

//
// 通过一个在lib父亲目录下libs.key的描述列表文件创建lib对像
// 如果这些lib在一个http服务器，通过这个libs.key文件能避免下载所有的lib.keys
// 因为libs.key中包含所有这个目录中lib的简单描述,这个文件一般会由开发工具创建
//
function LibsCore_parse_new_libs_keys(self, libs_keys, code) {
  if ( self.m_libs_keys_flags[libs_keys] == 2 ) return;
  self.m_libs_keys_flags[libs_keys] = 2;
  
  var keys = parse_keys(code);
  var dirname = format(libs_keys, '..')
  
  for ( var name in keys ) {
    if ( name[0] != '@' ) { // 忽略: @
      var value = keys[name]; 
      var path  = dirname + '/' + name; // lib path
      var is_build = false;       // is lib build, 是否为build过的代码
      var version_code  = '';     // lib version code
      var origin = '';
      
      if (typeof value == 'object') {
         // Build Lib 会自动创建这个文件
        if (value.path) {
          path = format(is_absolute(value.path) ? value.path : dirname + '/' + value.path);
        }
        version_code = String(value.version_code || '');
        // 指定一个最终build的版本代码也可视目标lib为build过后的代码
        is_build = '_build' in value ? !!value._build : !!version_code;
        origin = value.origin || '';
      }
      else if (value) { // 把单纯的值当成lib路径
        path = format(is_absolute(value) ? value : dirname + '/' + value);
      }
      
      var register = self.m_libs_register[path];
      if ( register ) {
        register.status = 2;
        origin = register.disable_origin ? '' : register.origin || origin;
      } else {
        self.m_libs_register[path] = { status: 2, path: path, name: name };
      }
      
      LibsCore_depe_lib(self, path, keys.depe);
      LibsCore_new_lib(self, path, name, is_build, version_code, origin);
    }
  }
}

function LibsCore_parse_new_lib_keys(self, register, code) {
  var keys = parse_keys(code);
  var version_code = String(keys.version_code || '');
  var is_build = '_build' in keys ? !!keys._build : !!version_code;
  var origin = register.disable_origin ? '' : register.origin || keys.origin || '';

  register.status = 2; // 完成
  
  LibsCore_depe_lib(self, register.path, keys.depe);
  LibsCore_new_lib(self, register.path, keys.name, is_build, version_code, origin);
}

function LibsCore_load_lib_keys(self, register, async) {
  
  var lib = self.m_libs[register.name];
  
  if ( lib && !Lib_is_can_replace_local(lib, register.path) ) { // lib 已创建并且无法替换
    console.warn('ignore', register.path);
    register.status = -1; // 忽略
    LibsCore_load_lib_keys_after(self, null, register.path);    
  } else {
    // 没有此lib实例,尝试读取lib.keys文件
    // 文件必须强制加载不使用缓存
    var lib_keys = set_url_args(register.path + '/lib.keys', '_no_cache');
    if ( async ) {
      read_text(lib_keys, function(code) {
        var err;
        try {
          LibsCore_parse_new_lib_keys(self, register, code);
        } catch (e) { 
          err = e;
        }
        LibsCore_load_lib_keys_after(self, err, register.path);
        if ( ! err) { // 继续依赖项目
          LibsCore_import_before(self, true);
        }
      }.catch(function(err) {
        LibsCore_load_lib_keys_after(self, err, register.path);
      }));
    } else {
      try {
        LibsCore_parse_new_lib_keys(self, register, read_text_sync(lib_keys));
      } catch(err) {
        LibsCore_load_lib_keys_after(self, err, register.path); throw err;
      }
      LibsCore_load_lib_keys_after(self, null, register.path);
      LibsCore_import_before(self, false); // 继续依赖项目
    }
  }
}

function LibsCore_load_lib_keys_after(self, err, path) {
  var async_cb = self.m_async_cb;
  
  if (err) {
    var register = self.m_libs_register[path];
    if (register) {
      register.status = 3; // 设置为异常
      self.m_async_cb = [];
      for ( let cb of async_cb ) {
        cb.throw(err); // 抛出异常
      }
      return;
    }
    // 如果路径已经被删除忽略这个异常
  }
  
  if ( self.m_is_ready || !async_cb.length ) {
    return;
  }
  
  for ( var i in self.m_libs_register ) {
    var status = self.m_libs_register[i].status;
    if (status != 2 && status != -1) { // 不为2/-1表示还没完成
      return;
    }
  }
  
  self.m_is_ready = true;
  self.m_async_cb = [ ];
  for ( let cb of async_cb ) {
    cb();
  }
}

// 准备工作，实例化已注册过的所有lib,并读取libs.keys描述文件
// import befory ready
function LibsCore_import_before(self, async, cb) {
  
  if ( self.m_is_ready ) {
    return cb && cb();
  }
  
  if ( self.m_async_cb.length && !async ) { // 正在异步载入中,完成前禁止再使用同步
    throw new Error('Now can not be loaded synchronously because an ' +
                    'asynchronous loading operation is being carried out.');
  }
  if (async && cb) {
    self.m_async_cb.push(cb);
  }
  var is_load = false;
  
  for ( var i in self.m_libs_register ) {
    let register = self.m_libs_register[i];
    
    if ( register.status == 0 ) {
      is_load = true;
      register.status = 1; // 设置成加载中状态
      // 先尝试读取lib父目录是否有libs.keys文件
      var libs_keys = format(register.path, '../libs.keys');
      var flags = self.m_libs_keys_flags[libs_keys]; // 1.载入中,2.完成
      
      if ( flags ) { // libs.keys文件尝试加载中或已完成
        if ( flags == 2 ) { // 已经载入完成libs.keys
          LibsCore_load_lib_keys(self, register, async);
        } // else 正在载入libs.keys文件现在必须等待完成
      } else {
        // load libs.keys
        // libs.keys文件必须强制加载不使用缓存
        var keys_path_no_cache = set_url_args(libs_keys, '_no_cache');
        
        self.m_libs_keys_flags[libs_keys] = 1; // 载入中libs.keys
        
        if (async) { // 使用异步
          read_text(keys_path_no_cache, function(code) {
            try { 
              LibsCore_parse_new_libs_keys(self, libs_keys, code);
            } catch (err) { 
              print_err(err);
            }
            self.m_libs_keys_flags[libs_keys] = 2;
            LibsCore_load_lib_keys(self, register, true);
            
          }.catch(function(err) {
            print_err(err);
            console.log(`Ignore load ${libs_keys}`);
            self.m_libs_keys_flags[libs_keys] = 2;
            LibsCore_load_lib_keys(self, register, true);
          }));
        } else { // 使用同步
          try {
            LibsCore_parse_new_libs_keys(self, libs_keys, read_text_sync(keys_path_no_cache));
          } 
          catch (err) { 
            print_err(err);
            console.log(`Ignore load ${libs_keys}`);
          }
          self.m_libs_keys_flags[libs_keys] = 2;
          LibsCore_load_lib_keys(self, register, false);
        }
      }
    } else if ( register.status == 3 ) { // err
      throw_err(`Load lib fail "${register.path}"`, cb); return;
    } // end if ( register.status == 0 ||
  } // end for (var i in this.m_libs_register)
  
  if ( !is_load) { // 没有任何载入
    // self.m_is_ready = true;
    if (async && cb) {
      self.m_async_cb.pop(); cb();
    }
  }
}

// -------------------------- LibsCore private func END --------------------------

/**
 * lib class
 */
class Lib {
  //
  m_config: null
  m_origin: ''
  m_name: ''
  m_path: ''
  m_src: ''
  m_build: false      /*  build 状态 */
  m_lib_path: ''     /* .avlib 文件本地路径 zip:///temp/test.avlib@ */
  m_lib_files: null  /* .avlib 中的文件列表,没有.avlib文件这个属性为null */
  m_versions: null    /* .avlib 文件以外资源文件的版本信息 */
  m_version_code: 0
  m_install: false
  m_exports: null
  m_local: false
  m_old: null
  //
  get config() { return this.m_config }
  get origin() { return this.m_origin }
  get name() { return this.m_name }   // lib 名称
  get path() { return this.m_path }   // lib 路径
  get src() { return this.m_src }
  get build() { return this.m_build }                 // build
  get versions() { return this.m_versions }           // 资源版本
  get version_code() { return this.m_version_code }   // 版本代码
  get exports() { return this.m_exports }
  get onclear() {
    if (!this.m_onclear) {
      this.m_onclear = new_event_noticer(this, 'clear');
    }
    return this.m_onclear;
  }
  
  /**
   * @constructor
   */
  constructor(path, name, build, version_code, origin) {
    var dirname = path.match(/([^\/]+)$/)[1];
    if (dirname != name) {
      throw new Error(`Lib name must be consistent with the folder name, ${dirname} != ${name}`);
    }
    this.m_path = path;               // lib 路径
    this.m_name = name;               // lib 名称
    this.m_build = build;             // build 标志
    this.m_versions = { };
    this.m_lib_files = { };
    this.m_version_code = version_code;
    this.m_exports = { };
    this.m_local = is_local(path);
    this.m_origin = is_network(origin) ? origin : '';
    // zip:///applications/test.apk@/
  }
  
  /**
   * 获取路径
   */
  get_path(path) {
    Lib_install(this);
    return Lib_get_path2(this, '', path);
  }
  
  /**
   * 清理当前lib中所有的exports引用,应用程序下次调用import时会重新从文件加载代码.
   * 调用这个函数有一定的内存清理功能,如果当前没有任何其它lib或代码使用这个lib,
   * javascript垃圾收集器就会开始工作了.
   * 
   * 调用这个函数会触发LibsCore.onclear事件.
   */
  clear() {
    if (this.m_onclear) {
      this.m_onclear.trigger(this); // onclear
      this.m_onclear = null;
    }
    if (_libs.m_onclear) {
      _libs.m_onclear.trigger(this); // onclear
    }
    this.m_exports = { }; // 断开所有输出引用
  }
}

/**
 * class LibsCore
 */
class LibsCore {
  m_libs_keys_flags: null
  m_libs_register: null   // all register path
  m_libs: null            // 当前加载的libs
  m_async_cb: null        // 
  m_is_ready: true        // 是否已准备
  m_oninstall: null       // 安装一个lib时候触发
  m_onclear: null         // 清理一个lib时候触发
  m_onimport: null        // import文件时候触发
  m_main: ''              // main run file path
  m_main_lib: null        // main run lib
  
  constructor(main) {
    this.m_libs_keys_flags = { };
    this.m_libs_register = { };   // all register path
    this.m_libs = { };            // 当前加载的libs
    this.m_async_cb = [ ];        // 
    this.m_main = main;           // main run file path

    // 添加默认libs
    [_path.resources()/*, _path.resources('libs')*/].forEach((libs)=>{
      if ( exists(libs) ) { 
        ls(libs).forEach((dirent)=>{
          if ( dirent.type == _fs.FILE_DIR ) {
            if ( exists(dirent.pathname + '/lib.keys') ) {
              LibsCore_register_lib_path(this, dirent.pathname);
            }
          }
        });
      }
      this.m_libs_keys_flags[libs + '/libs.keys'] = 2;
    });
  }
}

/**
 * class LibsCoreExport
 */
class LibsCoreExport {
  
  /**
   * @get main_lib
   */
  get main_lib() {
    if (!_libs.m_main_lib || _libs.m_main) {
      var o = this.get_relative_path(format(_libs.m_main)); // 查找内部路径
      if (o) {
        _libs.m_main_lib = o.lib; // 设置主要lib
      }
    }
    return _libs.m_main_lib;
  }
  
  /**
   * @event oninstall
   */
  get oninstall() {
    if (!_libs.m_oninstall) {
      _libs.m_oninstall = new_event_noticer(this, 'install');
    }
    return _libs.m_oninstall;
  }
  
  /**
   * @event onclear
   */
  get onclear() {
    if (!_libs.m_onclear) {
      _libs.m_onclear = new_event_noticer(this, 'clear');
    }
    return _libs.m_onclear;
  }
  
  /**
   * @event onimport
   */
  get onimport() {
    if (!_libs.m_onimport) {
      _libs.m_onimport = new_event_noticer(this, 'import');
    }
    return _libs.m_onimport;
  }
  
  /**
   * @get main main run file path
   */
  get main() {
    return _libs.m_main;
  }
  
  /**
   * @get libs 获取libs 名称列表
   */
  get names() {
    var rev = [];
    for (var i in _libs.m_libs) {
      rev.push(i);
    }
    return rev;
  }
  
  /**
   * @constructor(main)
   * @arg main {String}
   */
  constructor(main) {
    if (_libs) {
      throw new Error('Error.');
    }
    _libs = new LibsCore(main);
  }
  
  /**
   * 是否有这个lib
   */
  has_lib(name) {
    return name in _libs.m_libs;
  }
  
  /**
   * 通过名称获取lib实体
   */
  get_lib(name) {
    return _libs.m_libs[name];
  }
  
  /**
   * @fun get_relative_path 通过绝对路径获取lib内部相对路径,没有找到返回null
   * @arg path {String} 绝对路径
   */
  get_relative_path(path) {
    for (var i in _libs.m_libs) {
      var lib = _libs.m_libs[i];
      if ( !path.indexOf(lib.path) ) { // 可能匹配
        Lib_install(lib);
        if ( !path.indexOf(lib.src) ) {
          return { lib: lib, path: path.substr(lib.src.length + 1) };
        }
      }
    }
    return null;
  }
  
  /**
   * @func register_lib
   */
  register_lib(path, origin) {
    LibsCore_register_lib_path(_libs, path);
  }
  
  /**
   * @func unregister_lib
   */
  unregister_lib(path) {
    LibsCore_unregister_lib_path(_libs, path);
  }

  /**
   * @func set_origin(path,origin)
   */
  set_origin(path, origin) {
    LibsCore_set_origin(_libs, path, origin);
  }

  /**
   * @func disable_origin(path)
   */
  disable_origin(path, disable) {
    LibsCore_disable_origin(_libs, path, disable);
  }

  /**
   * @func load_lib libs
   */
  load_lib(libs, cb) {
    if ( typeof cb != 'function' ) {
      throw Error('Bad argument.');
    }
    if ( !Array.isArray(libs) ) {
      libs = [ libs ];
    }

    var len = libs.length;
    var count = 0;
    var e = null;
    var self = this;
    
    var callback = function() {
      if ( !e) {
        cb((++count) / len); // err, process
      }
    }.catch(function(err) {
      if ( !e) {
        e = err; cb.throw(e);
      }
    });
    
    LibsCore_import_before(_libs, true, function() {
      for (var i = 0; i < len; i++) {
        var lib = self.get_lib(libs[i]);
        if ( ! lib) {
          return throw_err(`Lib "${libs[i]}" does not exist`, cb);
        }
        Lib_install(lib, callback);
      }
    }.catch(cb.throw));
  }
  
}

// import absolute path file
function inl_import_absolute(path) {
  var o = _libs_export.get_relative_path(path);
  if (o) { // 重新import
    return inl_import(o.lib, '', o.name);
  }
  // 文件不在lib内部,这是一个外部文件
  // 是否载入过这个文件
  if (absolute_import_obj[path]) {
    return absolute_import_obj[path];
  }
  
  // 外部导入的文件都做为js执行
  
  var code = read_text_sync(set_url_args(path));
  var exports = { };
  absolute_import_obj[path] = exports;
  
  var sandbox = {
    global        : global,
    exports       : exports,
    set 
    __export(value) { __export(sandbox.exports, value) },
    __bind        : __bind,
    __dirname     : '',
    __filename    : path,
    __req         : function(path) { return inl_import(null, '', path) },
    __path        : get_path,
    $             : get_path,
  };
  sandbox.sandbox = sandbox;
  compile( _jsa.transform_jsa(code, path), path, sandbox );
  absolute_import_obj[path] = sandbox.exports;
  return sandbox.exports;
}

// inl_import
// import 'teide/util.js';
// import 'teide/util.js';
// 返回在上下文中声明的代码
/**
 * @fun inl_import
 * @arg lib {Object}   #    lib 对像
 * @arg dir {String}   #    lib 内相对目录
 * @arg name {String}  #    lib 内相对路径
 */
function inl_import(lib, dir, name) {
  if ( !is_lib_path(name) ) {
    if ( is_absolute(name) ) { // 绝对路径
      return inl_import_absolute(format(name));
    }
  }
  var mat = name.match(/^(?:(lib:\/\/|:)([^\/\.]+)(?:\/(?!$)|$))?(.*?([^\/]+?))?(\.[a-zA-Z]+)?$/);
  
  // import 'lib://util/util.js'
  // ["lib://util/gui/util.js", "lib", "avocado", "gui/util", "util", ".js"]
  if ( ! mat) {
    throw new Error('import param error, ' + name);
  }
  
  var libname = mat[2];
  var lib_inl_path = mat[3];
  var suffix = mat[5] || '.js';  // 只 import lib 没有suffix 或使用 .js
  
  LibsCore_import_before(_libs, false); //先准备libs
  
  if (libname) { // 导入一个lib的名称,如果lib不存在会抛出异常
    lib = _libs_export.get_lib(libname);
    if ( ! lib) { // 这是错误的, import ':test/xx'; 这个 test lib 必须存在
      throw throw_err('import error, "lib://' + libname + '" lib not register');
    }
  } 
  else if (lib) {
    lib_inl_path = dir + '/' + lib_inl_path; // 包内路径
  } 
  else { // 格式化路径,给inl_import_absolute
    return inl_import_absolute(format(dir, name));
  }
  
  return Lib_import(lib, lib_inl_path, suffix);
}

// inl require
function inl_require(path) {
  return inl_import(null, '', path);
}

// util
extend(util, {
  noop: noop, // func
  ext: extend,  // func
  extend: extend, // func
  err: new_err, // func
  cb: new_cb,   // func
  throw: throw_err, // func
  format: format,   // func
  is_absolute: is_absolute, // func
  read_text: read_text, // func
  read_text_sync: read_text_sync, // func
  get_path: get_path, // func
  reg_lib: function (path) { _libs_export.register_lib(path) }, // func
  load_lib: function(libs, cb) { _libs_export.load_lib(libs, cb) }, // func
});

util.__defineGetter__('libs', function() { return _libs_export });
util.__defineGetter__('debug', function() { return debug });
util.__defineGetter__('options', function() { return options });

/**
 * # start run
 * # parse args ready run
 */
(function() {
  
  var main = '';
  var args = util.args;  // start args

  for (var i = 0; i < args.length; i++) {
    var item = args[i];
    if (i != 0 || item != 'avocado') {
      var mat = item.match(/^--([^=]+)(?:=(.*))?$/);
      if (mat) {
        options[mat[1]] = mat[2] || '';
      } else {
        main = String(item).trim();
      }
    }
  }

  if ( !('url_arg' in options) ) {
    options.url_arg = '';
  }

  if ( 'no_cache' in options ) {
    if (options.url_arg) {
      options.url_arg += '&_no_cache';
    } else {
      options.url_arg = '_no_cache';
    }
  }
  
  // start
  if (main) {
    
    // check main path validity
    if ( /^(lib:|:)/.test(main) ) {
      throw new Error(`Invalid boot path "${main}"`);
    }
    
    _libs_export = new LibsCoreExport(main); // libs
    
    if ( /^.+?\.js$/i.test(main) ) { // js
      inl_require(main);
    } else {
      var mat = main.match( /^(.+\/)?([a-z_$][a-z0-9_$]*)$/i );
      if ( ! mat) { // lib
        throw new Error(`Could not start, invalid lib path "${main}"`);
      }

      var lib = mat[2];
      _libs_export.register_lib(main); // 添加lib

      if (debug) { // sync
        inl_require(':' + lib); // 载入lib
      } else {
        _libs_export.load_lib(lib, function(proc) {
          if (proc == 1) {
            inl_require(':' + lib);
          }
        }.catch(function(err) {
          util.fatal(err.message);
        }));
      }
    }
  }

})();
