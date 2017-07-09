{
  'variables': {
    'arch%': 'x86',
    ############################# v8 set ##################
    'target_arch%': 'ia32',           # set v8's target architecture
    'host_arch%': 'ia32',             # set v8's host architecture
    'host_os%': 'mac',
    'library%': 'static_library',     # allow override to 'shared_library' for DLL/.so builds
    'component%': '<(library)',       # NB. these names match with what V8 expects
    'uv_library%': '<(library)',
    'asan': 0,
    'werror': '',                     # Turn off -Werror in V8 build.
    'msvs_multi_core_compile': '0',   # we do enable multicore compiles, but not using the V8 way
    'want_separate_host_toolset': 0,  # V8 should not build target and host
    'v8_enable_gdbjit': 0,
    'v8_enable_i18n_support': 0,
    'v8_no_strict_aliasing': 1,
    'v8_use_snapshot': 'false',       # Enable the snapshot feature, for fast context creation.
    'v8_optimized_debug': 0,          # Default to -O0 for debug builds.
    'v8_enable_disassembler': 0,      # Enable disassembler
    'v8_use_external_startup_data': 0, # Don't bake anything extra into the snapshot.
    'v8_postmortem_support': 'false',
    
    # Default ARM variable settings.
    'arm_version%': 'default',
    'arm_float_abi%': 'default',
    'arm_thumb': 'default',
    'armv7%': 1,
    'armv7s%': 0,  # armv7s form apple iphone
    'arm_neon%': 1,
    'arm_vfp%': 'vfpv3',
    'arm_fpu%': 'neon',
    'arm64%': 0,
    
    # Default MIPS variable settings.
    'mips_arch_variant%': 'r2',
    # Possible values fp32, fp64, fpxx.
    # fp32 - 32 32-bit FPU registers are available, doubles are placed in
    #        register pairs.
    # fp64 - 32 64-bit FPU registers are available.
    # fpxx - compatibility mode, it chooses fp32 or fp64 depending on runtime
    #        detection
    'mips_fpu_mode%': 'fp32',
    #
    ##########################################################
    'byteorder%': 'little',
    'clang%': 0,
    'is_clang': '<(clang)',
    'python%': 'python',
    'node%': 'node',
    'project%': '',
    'ffmpeg%': 1,
    'gcc_version%': 0,
    'llvm_version%': 0,
    'xcode_version%': 0,
    'MSVS_VERSION%': 0,
    'gas_version%': 0,
    'cplusplus11%': 1,
    'cplusplus_exceptions%': 1,
    'cplusplus_rtti%': 1,
    'openssl_fips%': '',
    'openssl_no_asm%': 0,
    'sysroot%': '/',
    'SDKROOT%': '<(sysroot)',
    'cc%': 'gcc', 
    'cxx%': 'g++', 
    'ar%': 'ar', 
    'bin%': '/bin', 
    'tools%': 'tools',
    'output%': '',
    'suffix%': '<(arch)',

    # conditions
    'conditions': [
      ['arch=="arm64"', { 'arm64': 1 }],
    ]
  },
  
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
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
      ['OS=="android"', {
        'conditions': [
          ['arch=="arm" and armv7==1', { 'cflags': [ '-march=armv7-a' ] }],
          ['arch=="arm" and armv7==0', { 'cflags': [ '-march=armv6' ] }],
          ['arch=="arm" and arm_vfp!="none"', {
            'cflags': [ '-mfpu=<(arm_vfp)', '-mfloat-abi=softfp' ],
          }],
        ],
        'ldflags': [ '-shared' ],
      }],
      ['OS=="linux"', {
        'defines': [ '__STDC_LIMIT_MACROS' ],
      }],
      ['OS=="ios"', {
        'cflags': [ 
          '-miphoneos-version-min=8.0', 
          '-isysroot <(sysroot)', 
          #'-fembed-bitcode-marker',
          '-fembed-bitcode',
        ],
        'ldflags': [ '-miphoneos-version-min=8.0' ],
        'link_settings': { 
          'libraries!': [ '-lm' ],
          'libraries': [ '<(SDKROOT)/System/Library/Frameworks/Foundation.framework' ],
        },
        'conditions': [
          ['arch=="arm" and armv7s==0', { 
            'cflags': [ '-arch armv6',  ],
            'ldflags': [ '-arch armv7' ],
          }],
          ['arch=="arm" and armv7s==1', { 
            'cflags': [ '-arch armv7s' ], 
            'ldflags': [ '-arch armv7s' ],
          }],
          ['arch=="arm64"',             { 
            'cflags': [ '-arch arm64' ],
            'ldflags': [ '-arch arm64' ],
          }],
          ['arch=="x86"',              { 
            'cflags': [ '-arch i386' ],
            'ldflags': [ '-arch i386' ],
          }],
          ['arch=="x64"',               { 
            'cflags': [ '-arch x86_64' ],
            'ldflags': [ '-arch x86_64' ],
          }],
          ['cplusplus11==1', { 'cflags_cc': [ '-stdlib=libc++' ] }],
        ],
        'xcode_settings': {
          'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS).<(suffix)',
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
          'conditions': [
            # use physics device
            ['arch=="arm" and armv7s==0', { 'VALID_ARCHS': [ 'armv7' ] }],
            ['arch=="arm" and armv7s==1', { 'VALID_ARCHS': [ 'armv7s' ] }],
            ['arch=="arm64"', { 'VALID_ARCHS': [ 'arm64' ] }],
            # use simulator test
            ['arch=="x86"', { 'VALID_ARCHS': [ 'i386' ], 'SDKROOT':'iphonesimulator', }],
            ['arch=="x64"', { 'VALID_ARCHS': [ 'x86_64' ], 'SDKROOT': 'iphonesimulator', }],
          ],
        },
      }],
      ['OS=="mac"', {
        'cflags': [ 
          '-mmacosx-version-min=8.0',
          '-isysroot <(sysroot)', 
          #'-fembed-bitcode-marker', 
          '-fembed-bitcode',
        ],
        'ldflags!': [ '-pthread', '-s' ],
        'conditions': [
          ['arch=="x86"', { 
            'cflags': [ '-arch i386' ], 
            'ldflags': [ '-arch i386' ],
          }],
          ['arch=="x64"',  { 
            'cflags': [ '-arch x86_64' ], 
            'ldflags': [ '-arch x86_64' ],
          }],
          ['cplusplus11==1', { 'cflags_cc': [ '-stdlib=libc++' ] }],
        ],
        'link_settings': { 
          'libraries!': [ '-lm' ],
          'libraries': [ '<(SDKROOT)/System/Library/Frameworks/Foundation.framework' ],
        },
        'xcode_settings': {
          'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS).<(suffix)',
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
          'conditions': [
            ['arch=="x86"', { 'VALID_ARCHS': [ 'i386' ] }],
            ['arch=="x64"', { 'VALID_ARCHS': [ 'x86_64' ] }],
          ],
        },
      }],
      ['cplusplus11==1', {
        'xcode_settings': {
          'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',    # -std=c++0x
          'CLANG_CXX_LIBRARY': 'libc++',             # c++11 libc support
           # 'GCC_C_LANGUAGE_STANDARD': 'c99',
        },
        'cflags_cc': [ '-std=c++0x' ], 
      }],
      ['cplusplus_exceptions==1', {
        'xcode_settings': {
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',   # -fexceptions
        },
        'cflags_cc': [ '-fexceptions' ], 
      },{
        'xcode_settings': {
          'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',   # -fno-exceptions
        },
        'cflags_cc': [ '-fno-exceptions' ], 
      }],
      ['cplusplus_rtti==1', {
        'xcode_settings': {
          'GCC_ENABLE_CPP_RTTI': 'YES',              # -frtti / -fno-rtti
        },
        'cflags_cc': [ '-frtti', ], 
      }, {
        'xcode_settings': {
          'GCC_ENABLE_CPP_RTTI': 'NO',              # -frtti / -fno-rtti
        },
        'cflags_cc': [ '-fno-rtti', ], 
      }],
    ],
  },
  
  'conditions': [
    ['OS=="ios"', {
      'xcode_settings': {
        'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS).<(suffix)',
        'SDKROOT': 'iphoneos',
      },
    }, 
    'OS=="mac"', {
      'xcode_settings': {
        'SYMROOT': '<(DEPTH)/out/xcodebuild/<(OS).<(suffix)',
        'SDKROOT': 'macosx',
      },
    }],
  ],
}
