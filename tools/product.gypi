{
  'variables': {
    'OS%': 'ios',
  },
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG', 'av_memory_trace_mark=0', ],
        'cflags': [ '-g', '-O0' ],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',
          'ONLY_ACTIVE_ARCH': 'YES',      # Build Active Architecture Only
        },
      },
      'Release': {
        'cflags': [ '-O3', '-ffunction-sections', '-fdata-sections' ],
        'ldflags': [ '-s' ],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '3',  # -O3
          'GCC_STRICT_ALIASING': 'YES',
          'ONLY_ACTIVE_ARCH': 'NO',
        },
      }
    },
    'cflags!': ['-Werror'],
    'conditions': [
      ['OS=="ios"', {
        'link_settings': { 
          'libraries': [ '$(SDKROOT)/System/Library/Frameworks/Foundation.framework' ],
        },
        'xcode_settings': {
          'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS)',
          'ALWAYS_SEARCH_USER_PATHS': 'NO',
          'GCC_TREAT_WARNINGS_AS_ERRORS': 'NO',
          'SDKROOT': 'iphoneos',
          'TARGETED_DEVICE_FAMILY': '1,2',
          'IPHONEOS_DEPLOYMENT_TARGET': '8.0',
          'USE_HEADERMAP': 'NO',
          'ARCHS': [ '$(ARCHS_STANDARD)' ],   # 'ARCHS': [ '$(ARCHS_STANDARD_32_BIT)' ],
          'SKIP_INSTALL': 'YES',
          'DEBUG_INFORMATION_FORMAT': 'dwarf', # dwarf-with-dsym
          'ENABLE_BITCODE': 'YES',
          'CLANG_ENABLE_OBJC_ARC': 'YES',
        },
      }],
      ['OS=="mac"', {
        'link_settings': { 
          'libraries': [ '$(SDKROOT)/System/Library/Frameworks/Foundation.framework' ],
        },
        'xcode_settings': {
          'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS)',
          'ALWAYS_SEARCH_USER_PATHS': 'NO',
          'GCC_TREAT_WARNINGS_AS_ERRORS': 'NO',
          'SDKROOT': 'macosx',
          'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
          'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                    # (Equivalent to -fPIC)
          'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
          'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
          'PREBINDING': 'NO',                       # No -Wl,-prebind
          'MACOSX_DEPLOYMENT_TARGET': '10.7',       # -mmacosx-version-min=10.7
          'USE_HEADERMAP': 'NO',
          'AECHS': ['$(ARCHS_STANDARD)'],           # 'ARCHS': 'x86_64',
          'SKIP_INSTALL': 'YES',
          'DEBUG_INFORMATION_FORMAT': 'dwarf',      # dwarf-with-dsym
          'ENABLE_BITCODE': 'YES',
          'CLANG_ENABLE_OBJC_ARC': 'YES',
        },
      }],
    ],
    'xcode_settings': {
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',   # -std=c++0x
      'CLANG_CXX_LIBRARY': 'libc++',            # c++11 libc support
       # 'GCC_C_LANGUAGE_STANDARD': 'c99',
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',       # -fexceptions
      'GCC_ENABLE_CPP_RTTI': 'YES',             # -frtti / -fno-rtti
    },
  },

  'targets': [{
    'target_name': 'avocado',
    'type': 'none',
    'conditions': [
      ['OS=="android"', { 
        'link_settings': { 'libraries': [
          '-llog', 
          '-latomic',
          '-landroid',
          '-lz',
          '-lGLESv3', '-lEGL', '-lOpenSLES', '-lmediandk',
          'android/libavocado.so',
          'android/libv8.so',
          'android/libffmpeg.so',
        ]},
      }],
      ['OS=="ios"', {
        'link_settings': { 'libraries': [ 
          'ios/libavocado.a',
          'ios/libv8.a',
          'ios/libffmpeg.a',
          '$(SDKROOT)/System/Library/Frameworks/SystemConfiguration.framework',
          '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
          '$(SDKROOT)/System/Library/Frameworks/AudioToolbox.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework',
          '$(SDKROOT)/System/Library/Frameworks/VideoToolbox.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreMedia.framework',
          '$(SDKROOT)/System/Library/Frameworks/OpenGLES.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
          '$(SDKROOT)/System/Library/Frameworks/UIKit.framework',
          '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
          '$(SDKROOT)/usr/lib/libiconv.tbd',
          '$(SDKROOT)/usr/lib/libbz2.tbd',
          '$(SDKROOT)/usr/lib/libz.tbd',
          '$(SDKROOT)/usr/lib/libsqlite3.tbd',
        ]},
      }],
      ['OS=="mac"', { 'libraries': [ 
          'ios/libavocado.a',
          'ios/libv8.a',
          'ios/libffmpeg.a',
          # TODO
        ]
      }],
    ],
    'direct_dependent_settings': {
      'include_dirs': [ 'include' ],
      'mac_bundle_resources': [
        #'cacert.pem',
      ],
    },
  }],

  'conditions': [
    ['OS=="ios"', {
      'xcode_settings': {
        'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS)',
        'SDKROOT': 'iphoneos',
      },
    }, 
    'OS=="mac"', {
      'xcode_settings': {
        'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS)',
        'SDKROOT': 'macosx',
      },
    }],
  ],
}
