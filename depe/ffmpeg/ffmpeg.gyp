{
  'variables': {
    'ffmpeg%': 1,
    'output%': '',
  },
  'targets': [{
    'target_name': 'ffmpeg',
    'direct_dependent_settings': {
      'include_dirs': [ '.' ],
      'defines': [ '__STDC_CONSTANT_MACROS', ],
    },
    'dependencies': [
      '../openssl/openssl.gyp:openssl',
      '../zlib/zlib.gyp:zlib'
    ],
    'sources': [
      'libavutil/avutil.h',
      'libavformat/avformat.h',
      'libswresample/swresample.h',
      'libavcodec/avcodec.h',
    ],
    'conditions': [
      ['library=="shared_library"', { 
        'type': '<(library)',
        'ldflags': [
          '-Wl,--whole-archive',
          '<(output)/libffmpeg.a',
          '-Wl,--no-whole-archive',
        ],
      }, { 
        'type': 'none',
        'link_settings': { 'libraries': [ 
          '<(output)/libffmpeg.a',
        ]},
      }],
      ['OS=="ios"', {
        'link_settings': { 'libraries': [ 
          '$(SDKROOT)/System/Library/Frameworks/AudioToolbox.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework',
          '$(SDKROOT)/System/Library/Frameworks/VideoToolbox.framework',
          '$(SDKROOT)/System/Library/Frameworks/CoreMedia.framework',
          '$(SDKROOT)/usr/lib/libiconv.tbd',
          '$(SDKROOT)/usr/lib/libbz2.tbd',
          '$(SDKROOT)/usr/lib/libz.tbd',
        ]},
      }],
    ],
    'actions': [{
      'action_name': 'ffmpeg_compile',
      'inputs': [ 'config.h' ],
      'outputs': [
        '<(output)/libffmpeg.a',
      ],
      'conditions': [
        ['ffmpeg==1', {
          'action': [ 'sh', '-c', 
            'export PATH=<(tools):<(bin):${PATH};'
            'make -j3;'
            'mkdir -p <(output);'
            'ar rc <(output)/libffmpeg.a '
            '`find libavutil libavformat libswresample libavcodec -name *.o|xargs`;'
            'ranlib <(output)/libffmpeg.a;'
          ],
        }, {
          'action': ['echo', 'skip ffmpeg compile'],
        }]
      ],
    }]
  }],
}