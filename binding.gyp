{
  'variables': {
    'library%': 'static_library',
  },
  'targets': [
    {
      'target_name': 'psutil_lib_osx',
      'sources': [ 
        'lib/psutil_lib_osx.cc',
        'lib/psutil_lib_osx.h',
        'lib/workers/worker.h',
        'lib/workers/virtual_memory_worker.h',
        'lib/workers/cpu_worker.h',
        'lib/workers/disk_io_counters_worker.h',
        'lib/workers/disk_usage_worker.h',
        'lib/workers/network_io_counters_worker.h',
        'lib/workers/pid_exists_worker.h',
        'lib/workers/pid_list_worker.h',
        'lib/workers/process_worker.h',
        'lib/workers/swap_memory_worker.h'
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
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