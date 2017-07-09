{
  'includes': [
    'autil/util.gypi',
    'agui/gui.gypi',
    'av8/v8.gypi',
  ],
  'conditions': [
  	['OS != "ios" or project == "xcode"', {
  		'includes': [ 
        'test/test.gypi',
        'tools/tools.gypi',
      ],
  	}]
  ],
  'targets': [{
    'target_name': 'avocado',
    'product_name': 'avocado',
    'type': 'none',
    'conditions': [
      ['OS=="android" or OS=="shared_library"', {
        'type': 'shared_library',
        'ldflags': [ '-shared' ],
        'link_settings': { 
          'libraries': [ '-shared' ],
        },
      }]
    ],
    'dependencies': [ 'avocado-util', 'avocado-gui', 'avocado-v8', ],
    'direct_dependent_settings': {
      'include_dirs': [ '.', 'depe/v8/include' ],
      'mac_bundle_resources': [
        'autil/cacert.pem',
        'libs/gui',
        'libs/util',
        'libs/xml',
      ],
    },
  }]
}
