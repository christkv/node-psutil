{
  'targets': [
    {
      'target_name': 'psutil_lib_osx',
      'sources': [ 
        'lib/psutil_lib_osx.cc',
        'lib/workers/worker.cc',
        'lib/workers/virtual_memory_worker.cc'
      ],
      'cflags!': [ '-fno-exceptions', '-export-dynamic' ],
      'cflags_cc!': [ '-fno-exceptions', '-export-dynamic'],
      'ldflags!': [ '-fPIC', '-static'],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ]
    }
  ]
}