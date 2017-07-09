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

var fs = require('fs');
var path = require('path');
var child_process = require('child_process');
var host_os = process.platform == 'darwin' ? 'mac': process.platform;
var host_arch = arch_format(process.arch);
var opts = { };
var help_info = [];

read_argv();
def_opts('help', 0,             '-h, --help     print help info');
def_opts('v', 0,                '-v, --v        enable compile print info [{0}]');
def_opts('debug', 0,            '--debug        enable debug status [{0}]');
def_opts('os', host_os,         '--os=OS        system type ios/android/mac/linux/win [{0}]');
def_opts('arch', process.arch || 'x86',  
                                '--arch=CPU     cpu type options arm/arm64/mips/mips64/x86/x64 [{0}]');
def_opts('library', 'static',   '--library=LIB  compile output library type static/shared [{0}]');
def_opts('armv7', 1,            '--armv7        enable armv7 [{0}]');
def_opts('armv7s', 0,           '--armv7s       enable armv7s form apple iphone [{0}]');
def_opts('arm-neon', 1,         '--arm-neon     enable arm neno [{0}]');
def_opts('arm-vfp', opts.arch == 'arm64' ? 'vfpv4': 
                    (opts.arch == 'arm' ? (opts.armv7 || opts.armv7s ? 'vfpv3' : 'vfpv2'): 'none'),
                                '--arm-vfp=VAL  enable arm vfp options vfpv2/vfpv3/vfpv4/none [{0}]');
def_opts('arm-fpu', opts.arm_neon ? 'neon': opts.arm_vfp, 
                                '--arm-fpu=VAL  enable arm fpu [{0}]');
def_opts('clang', opts.os == 'mac' || opts.os == 'ios' ? 1 : 0, 
                                '--clang        enable clang compiler [{0}]');
def_opts('ffmpeg', 'auto',      '--ffmpeg  compile ffmpeg [{0}]');
def_opts('ndk-path', '',        '--ndk-path     android NDK path [{0}]');

function arch_format(arch) {
  arch = arch == 'ia32' || arch == 'i386' ? 'x86' : arch;
  arch = arch == 'x86_64' || arch == 'ia64' ? 'x64' : arch;
  return arch;
}

function touch_file(pathnames) {
  if ( !Array.isArray(pathnames)) {
    pathnames = [ pathnames ];
  }
  pathnames.forEach(function(pathname) {
    if ( !fs.existsSync(pathname) ) {
      fs.writeFileSync(pathname, '');
    }
  });
}

function syscall2(cmd) {
  var r = child_process.spawnSync('sh', ['-c', cmd]);
  if (r.status != 0) {
    if (r.stderr.length) {
      console.error(r.stderr.toString('utf8'));
    }
    if (r.stdout.length) {
      console.log(r.stdout.toString('utf8'));
    }
    process.exit(0);
  } else {
    var rv = [];
    if (r.stderr.length) {
      rv.push(r.stderr.toString('utf8'));
    }
    if (r.stdout.length) {
      rv.push(r.stdout.toString('utf8'));
    }
    return rv.join('\n');
  }
}

function syscall(cmd) {
  return syscall2(cmd).split('\n');
}

function read_argv() {
  var argv = process.argv.slice(2);
  for (var i = 0; i < argv.length; i++) {
    var item = argv[i];
    if (item.substr(0, 2) == '--') {
      var ls = item.substr(2).split('=');
      var key = ls[0].replace(/\-/mg, '_');
      var value = ls[1] || 1;
      
      if (value == 'true' || value == 'yes') {
        opts[key] = 1;
      } else if (value == 'false' || value == 'no') {
        opts[key] = 0;
      } else if (/^[0-9]+$/.test(value)) {
        opts[key] = parseInt(value);
      } else {
        opts[key] = value;
      }
    } else if (item.substr(0, 1) == '-') {
      opts[item.substr(1).replace(/\-/mg, '_')] = 1;
    }
  }
}

function format_string(val) {
  for (var i = 1, len = arguments.length; i < len; i++)
    val = val.replace(new RegExp('\\{' + (i - 1) + '\\}', 'g'), arguments[i]);
  return val;
}

function def_opts(name, defaults, info) {
  name = name.replace(/\-/mg, '_');
  if ( ! (name in opts) ) {
    opts[name] = defaults;
  }
  var default_val = opts[name] === 0 ? 'no' : opts[name] === 1 ? 'yes' : opts[name];
  help_info.push(format_string(info, default_val));
}

function configure_ffmpeg(opts, variables, configuration) {
  var os = opts.os;
  var arch = opts.arch;
  var cmd = '';
  var source = __dirname + '/../depe/ffmpeg'

  syscall(`rm -rf ${variables.output}/obj.target/depe/ffmpeg/*; \
           rm -rf ${source}/compat/strtod.d \
                  ${source}/compat/strtod.o \
                  ${source}/.config \
                  ${source}/config.fate \
                  ${source}/config.log \
                  ${source}/config.mak \
                  ${source}/doc/config.texi \
                  ${source}/doc/examples/pc-uninstalled \
                  ${source}/libavcodec/libavcodec.pc \
                  ${source}/libavdevice/libavdevice.pc \
                  ${source}/libavfilter/libavfilter.pc \
                  ${source}/libavformat/libavformat.pc \
                  ${source}/libavutil/libavutil.pc \
                  ${source}/libswresample/libswresample.pc \
  `);

  if (os == 'android') {
    cmd = `
      ./configure \
      --target-os=android \
      --arch=${arch} \
      --sysroot=${variables.sysroot} \
      --cross-prefix=${variables.cross_prefix} \
      --enable-cross-compile \
      --enable-pic \
      --enable-jni \
      --enable-mediacodec `;
    if ( arch == 'arm' ) {
      cmd += `--cc='${variables.cc} ${opts.armv7 ? '-march=armv7-a' : '-march=armv6'}' `;
    }
  } 
  else if ( os=='linux' ) {
    cmd = `./configure --target-os=linux --arch=${arch} `;
    if ( host_arch != arch ) {
      cmd += '--enable-cross-compile ';
    }
  }
  else if (os == 'ios') {
    cmd = `
      ./configure \
      --target-os=darwin \
      --arch=${arch} \
      --enable-cross-compile `;

    var cflags = '-fembed-bitcode -miphoneos-version-min=8.0 -arch'; //  -fembed-bitcode-marker
    if ( arch == 'arm' ) {
      cmd += `--cc='clang ${cflags} ${opts.armv7s ? 'armv7s' : 'armv7'}' `;
    } else if ( arch == 'arm64' ) {
      cmd += `--cc='clang ${cflags} arm64' `;
    } else if ( arch == 'x86' ) {
      cmd += `--cc='clang ${cflags} i386' `;
    } else if (arch == 'x64') {
      cmd += `--cc='clang ${cflags} x86_64' `;
    }
    if (arch == 'x86' || arch == 'x64') {
      cmd += '--sysroot=$(xcrun --sdk iphonesimulator --show-sdk-path) ';
    } else {
      cmd += '--sysroot=$(xcrun --sdk iphoneos --show-sdk-path) ';
    }
  } 
  else if (os == 'mac') {
    cmd = `
      ./configure \
      --target-os=darwin \
      --arch=${arch} `;
      
    var cflags = '-fembed-bitcode -mmacosx-version-min=10.7 -arch';
    if ( arch == 'x86' ) {
      cmd += `--cc='clang ${cflags} i386' `;
    } else if (arch == 'x64') {
      cmd += `--cc='clang ${cflags} x86_64' `;
    }
    cmd += '--sysroot=$(xcrun --sdk macosx --show-sdk-path) ';
  }

  if ( !cmd ) {
    return false;
  }

  var ff_opts = [
    // '--enable-shared',
    '--disable-programs',
    '--disable-doc',
    '--disable-avdevice',
    '--disable-swscale',
    '--disable-postproc',
    '--disable-avfilter',
    '--disable-encoders',
    '--disable-muxers',
    // '--disable-avcodec',
    // '--disable-avformat',
    // '--disable-swresample'
  ];

  if ( opts.debug ) {
    ff_opts.push('--enable-debug=2');
  } else {
    ff_opts.push('--disable-debug');
    ff_opts.push('--strip');
    // ff_opts.push('--disable-logging');
  }

  cmd += ff_opts.join(' ');

  var log = syscall2(`export PATH=${__dirname}:${variables.bin}:$PATH; \
                      cd depe/ffmpeg; \
                      make clean; \
                      find libavutil libavformat libswresample libavcodec -name *.o|xargs rm; \
                      ${cmd} \
                    `);
  console.log('FFMpeg Configuration output:\n');
  console.log(log);
  return true;
}

function configure() {

  if (opts.help || opts.h) { // print help info
    console.log('');
    console.log('Usage: ./configure [OPTION]... [VAR=VALUE]...');
    console.log('');
    console.log('Defaults for the options are specified in brackets.');
    console.log('');
    console.log('Configuration:');
    console.log('  ' + help_info.join('\n  '));
    return;
  }
  // 
  opts.arch = arch_format(opts.arch);

  var configuration = opts.debug ? 'Debug': 'Release';
  var config_gypi = {
    target_defaults: {
      default_configuration: configuration,
    },
    variables: {
      target_arch: opts.arch == 'x86' ? 'ia32' : opts.arch, // v8 target_arch
      host_arch: host_arch == 'x86' ? 'ia32' : host_arch, // v8 host_arch
      arch: opts.arch,
      host_os: host_os,
      want_separate_host_toolset: opts.arch != host_arch ? 1: 0,
      node: process.execPath,
      debug: opts.debug,
      OS: opts.os,
      os: opts.os,
      clang: opts.clang,
      library: opts.library + '_library',
      component: opts.library + '_library',
      armv7: opts.armv7,
      armv7s: opts.armv7s,
      arm_neon: opts.arm_neon,
      arm_vfp: opts.arm_vfp,
      arm_fpu: opts.arm_fpu,
      node: process.execPath,
      ffmpeg: opts.ffmpeg,
      cross_prefix: '',
      sysroot: '/',
      output: '',
      cc: 'gcc',
      cxx: 'g++',
      ld: 'g++',
      ar: 'ar',
      as: 'as',
      bin: '/bin',
      tools: __dirname,
      suffix: opts.arch,
    }
  };

  var config_mk = [
    '# Do not edit. Generated by the configure script.',
    'ARCHE=' + opts.arch,
    'OS=' + opts.os,
    'BUILDTYPE=' + configuration,
    'OUTDIR=out',
    'V=' + opts.v,
  ];

  var ENV = [ ];
  var os = opts.os;
  var arch = opts.arch;
  var suffix = arch;
  var variables = config_gypi.variables;

  if ( os == 'android' ) {
    // check android toolchain
    
    var toolchain_dir = `${__dirname}/android-toolchain/${arch}`;
    if (!fs.existsSync(toolchain_dir)) {
      if ( opts.ndk_path && fs.existsSync(opts.ndk_path) ) {
        syscall(`./tools/install-android-toolchain ${arch} ${opts.ndk_path}`); // install tool
      } else {
        console.error(`Run "./tools/install-android-toolchain ${arch} NDK-DIR" to install android toolchain !`);
        return;
      }
    }

    var tools = {
      'arm': { prefix: 'arm-linux-androideabi-', jni: 'armeabi' },
      'arm64': { prefix: 'aarch64-linux-android-', jni: 'arm64-v8a' },
      'mips': { prefix: 'mipsel-linux-android-', jni: 'mips' },
      'mips64': { prefix: 'mips64el-linux-android-', jni: 'mips64' },
      'x86': { prefix: 'i686-linux-android-', jni: 'x86' },
      'x64':  { prefix: 'x86_64-linux-android-', jni: 'x86_64' },
    };
    var tool = tools[arch];
    if (!tool) {
      console.error(`do not support android os and ${arch} cpu architectures`);
      return;
    }    
    if (arch == 'arm' && opts.armv7) {
      suffix = 'armv7';
      tool.jni = 'armeabi-v7a';
    }
    variables.cross_prefix = tool.prefix;
    variables.cc = `${tool.prefix}gcc`;
    variables.cxx = `${tool.prefix}g++`;
    variables.ld = `${tool.prefix}g++`;
    variables.ar = `${tool.prefix}ar`;
    variables.as = `${tool.prefix}as`;
    variables.bin = toolchain_dir + '/bin';
    variables.sysroot = `${toolchain_dir}/sysroot`;
    config_mk.push('ANDROID_JNI_DIR=' + tool.jni);
    config_mk.push('CXX=' + variables.cxx);
    config_mk.push('LINK=' + variables.ld);
    ENV.push('export CC=' + variables.cc);
    ENV.push('export AR=' + variables.ar);
    ENV.push('export AS=' + variables.as);
  } 
  else if ( os == 'linux' ) {
    config_mk.push('CXX=g++');
    config_mk.push('LINK=g++');
    ENV.push('export CC=gcc');
    ENV.push('export AR=ar');
    ENV.push('export AS=as');
  }
  else if (os == 'ios' || os == 'mac') {
    if ( os == 'ios' ) {
      if ( host_os != 'mac' ) {
        console.error('Only in the mac os and the installation of the Xcode environment to compile target iOS');
        return;
      }
      if ( ['arm', 'arm64', 'x86', 'x64'].indexOf(arch) == -1) {
        console.error(`do not support iOS and ${arch} cpu architectures`);
        return;
      }
    } else {
      if ( ['x86', 'x64'].indexOf(arch) == -1) {
        console.error(`do not support MacOSX and ${arch} cpu architectures`);
        return;
      }
    }

    var XCODEDIR = syscall('xcode-select --print-path')[0];

    if ( os == 'ios' ) {
      //var IPHONEOS_PLATFORM = `${XCODEDIR}/Platforms/iPhoneOS.platform`;
      //var IPHONESIMULATOR_PLATFORM = `${XCODEDIR}/Platforms/iPhoneSimulator.platform`;
      //var IPHONEOS_SYSROOT = `${IPHONEOS_PLATFORM}/Developer/SDKs/iPhoneOS.sdk`;
      //var IPHONESIMULATOR_SYSROOT = `${IPHONESIMULATOR_PLATFORM}/Developer/SDKs/iPhoneSimulator.sdk`;

      if (arch == 'x86' || arch == 'x64') {
        variables.sysroot = syscall('xcrun --sdk iphonesimulator --show-sdk-path')[0];
      } else {
        variables.sysroot = syscall('xcrun --sdk iphoneos --show-sdk-path')[0];
      }
      if (arch == 'arm') {
        suffix = opts.armv7s ? 'armv7s' : 'armv7';
      }
    } else { // mac
      var MACOSX_PLATFORM = `${XCODEDIR}/Platforms/MacOSX.platform`;
      var MACOSX_SYSROOT = `${MACOSX_PLATFORM}/Developer/SDKs/MacOSX.sdk`;
      variables.sysroot = MACOSX_SYSROOT;
    }
    
    variables.cc = 'clang';
    variables.cxx = 'clang';
    variables.ld = 'clang++';
    variables.ar = 'ar'; 
    variables.as = 'as';
    variables.bin = `${XCODEDIR}/Toolchains/XcodeDefault.xctoolchain/usr/bin`;
    config_mk.push('CXX=clang');
    config_mk.push('LINK=clang++');
    ENV.push('export CC=clang');
    ENV.push('export AR=ar');
    ENV.push('export AS=as');

  } else {
    console.error(`do not support ${os} os`);
    return;
  }

  variables.output = path.resolve(
    `${__dirname}/../out/${os}.${suffix}.${configuration}`
  );
  variables.suffix = suffix;
  ENV.push(`export PATH=${__dirname}:${variables.bin}:$$PATH`);
  ENV.push(`export SYSROOT=${variables.sysroot}`);
  config_mk.push(`SUFFIX=${suffix}`);

  if ( opts.ffmpeg == 'auto' ) { //auto
    if ( fs.existsSync(`${variables.output}/libffmpeg.a`) ) {
      opts.ffmpeg = 0;
      variables.ffmpeg = 0;
    } else {
      opts.ffmpeg = 1;
      variables.ffmpeg = 1;
    }
  }

  if ( variables.ffmpeg ) {
    if ( !configure_ffmpeg(opts, variables, configuration) ) {
      return;
    }
  }
  
  var config_gypi_str = JSON.stringify(config_gypi);
  var config_mk_str = config_mk.join('\n') + '\n';
  config_mk_str += 'ENV=\\\n';
  config_mk_str += ENV.join(';\\\n') + ';\n';

  {
    console.log('\nConfiguration output:');
    for (var i in opts) {
      console.log(' ', i, ':', opts[i]);
    }
    console.log('');
    console.log(config_gypi_str, '\n');
    console.log(config_mk_str);
  }

  if ( ! fs.existsSync('out') ) {
    fs.mkdirSync('out');
  }

  touch_file([
    'out/glsl-shader.cc', 
    'out/glsl-es2-shader.cc', 
    'out/native-core-js.cc',
  ]);
  
  fs.writeFileSync('out/config.gypi', config_gypi_str);
  fs.writeFileSync('out/config.mk', config_mk_str);
}

configure();

