{
  'includes': [
    'autil/util.gypi',
  ],
  'conditions': [
  	['OS != "ios" or project == "xcode"', {
  		'includes': [ 
        'tools/tools.gypi',
      ],
  	}]
  ],
}
