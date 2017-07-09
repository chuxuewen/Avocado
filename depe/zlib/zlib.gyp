{
  'variables': {
    'use_system_zlib%': 0
  },
  'targets': [
    {
      'target_name': 'zlib',
      'product_name': 'avocado-zlib',
      'type': '<(library)',
      'cflags': [ '-ansi' ],
      'direct_dependent_settings': {
        'include_dirs': [ 'contrib/minizip' ],
      },
      'include_dirs': [  ],
      'sources': [
        'contrib/minizip/ioapi.c',
        'contrib/minizip/zip.c',
        'contrib/minizip/unzip.c',
      ],
      'conditions': [
        ['OS in "mac ios"', {
          'link_settings': { 'libraries': [ '$(SDKROOT)/usr/lib/libz.tbd' ] },
        }, 'OS == "android" or use_system_zlib==1', {
          'link_settings': { 'libraries': [ '-lz' ] },
        }, {
          'include_dirs': [ '.' ],
          'direct_dependent_settings': {
            'include_dirs': [ '.' ],
          },
          'sources':[
            'adler32.c',
            'compress.c',
            'crc32.c',
            'deflate.c',
            'gzclose.c',
            'gzlib.c',
            'gzread.c',
            'gzwrite.c',
            'infback.c',
            'inffast.c',
            'inflate.c',
            'inftrees.c',
            'trees.c',
            'uncompr.c',
            'zutil.c',
          ],
        }],
        [ 'OS=="win"', {
          'sources': [ 'contrib/minizip/iowin32.c' ]
        },{
          'cflags!': [ '-ansi' ],
        }],
        [ 'OS in "mac ios"', {
          'xcode_settings': {
            'GCC_C_LANGUAGE_STANDARD': 'ansi',
          },
        }],
        [ 'OS in "android mac ios"', {
          'defines': [ 'USE_FILE32API', ],
        }],
      ]
    }
  ]
}
