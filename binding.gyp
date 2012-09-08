{
  'variables': {
    'library%': 'static_library',
  },
  'targets': [
    {
      'target_name': 'psutil_lib_osx',
      'sources': [
        'lib/psutil_lib_osx.cc',
        'lib/workers/worker.cc',
        'lib/workers/virtual_memory_worker.cc',
        'lib/workers/cpu_worker.cc',
        'lib/workers/sysconf_worker.cc',
        'lib/workers/disk_io_counters_worker.cc',
        'lib/workers/disk_partitions_worker.cc',
        'lib/workers/network_io_counters_worker.cc',
        'lib/workers/swap_memory_worker.cc',
        'lib/workers/pid_list_worker.cc',
        'lib/workers/disk_usage_worker.cc',
        'lib/workers/pid_exists_worker.cc',
        'lib/workers/process_worker.cc'
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