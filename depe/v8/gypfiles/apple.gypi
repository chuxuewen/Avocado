{
  'variables': {
    'project%': '',
  },
  'target_defaults': {
    'conditions': [
      ['OS=="ios" and project=="xcode"', {
        'xcode_settings': {
          'GCC_PREPROCESSOR_DEFINITIONS[arch=armv7]': [
            'USE_SIMULATOR',
            '$(inherited)',
          ],
          'GCC_PREPROCESSOR_DEFINITIONS[arch=armv7s]': [
            'USE_SIMULATOR',
            '$(inherited)',
          ],
          'GCC_PREPROCESSOR_DEFINITIONS[arch=arm64]': [
            'USE_SIMULATOR',
            '$(inherited)',
          ],
        },
      }],
      ['OS=="ios" and (target_arch=="arm" or target_arch=="arm64")', {
        'defines': [ 'USE_SIMULATOR' ],
      }],
    ],
  },
}