{ 
  'targets': [
    {
      'target_name': 'test',
      'type': 'executable',
      'include_dirs': [ '../out' ],
      'dependencies': [
        'avocado',
        'depe/tess2/tess2.gyp:tess2',
        'depe/freetype2/freetype2.gyp:ft2',
        'depe/curl/curl.gyp:curl',
        'depe/openssl/openssl.gyp:openssl',
        'depe/v8/src/v8.gyp:v8',
        'depe/ffmpeg/ffmpeg.gyp:ffmpeg',
        'depe/uv/uv.gyp:libuv',
        'depe/http_parser/http_parser.gyp:http_parser',
      ],
      'mac_bundle': 1,
      'mac_bundle_resources': [
        'res',
        'test_js',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '$(SRCROOT)/test/test.plist',
        'OTHER_LDFLAGS': '-all_load',
      },
      'sources': [
        'Storyboard.storyboard',
        'test.plist',
        'test.cc',
        'test-fs.cc',
        'test-gui.cc',
        'test-freetype.cc',
        'test-json.cc',
        'test-string.cc',
        'test-list.cc',
        'test-map.cc',
        'test-event.cc',
        'test-zlib.cc',
        'test-curl.cc',
        'test-curl2.cc',
        'test-http.cc',
        'test-https.cc',
        'test-thread.cc',
        'test-v8-bind.cc',
        'test-v8-bind-gui.cc',
        'test-ffmpeg.cc',
        'test-float.cc',
        'test-uv.cc',
        'test-net.cc',
        'test-fs-async.cc',
        'test-ssl.cc',
        'test-http2.cc',
        'test-net-ssl.cc',
        'test-http-cookie.cc',
        'test-localstorage.cc',
        'test-buffer.cc',
        'test-js.cc',
      ],
      'conditions': [
        ['OS == "ios"', {
          'sources': [ 'test-ios-run-loop.mm', ]
        }]
      ],
    }, {
      'target_name': 'avocado-demo',
      'type': 'executable',
      'dependencies': [ 'avocado' ],
      'mac_bundle': 1,
      'mac_bundle_resources': [
        '../demo',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': '$(SRCROOT)/test/test.plist',
        'OTHER_LDFLAGS': '-all_load',
      },
      'sources': [
        'test.plist',
        'Storyboard.storyboard',
      ],
      'conditions': [
        ['OS == "ios" or OS == "mac"', {
          'sources': [ 'demo.mm', ]
        }, {
          'sources': [ 'demo.cc', ]
        }]
      ],
    },
  ]
}
