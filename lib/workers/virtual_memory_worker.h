#ifndef VIRTUAL_MEMORY_WORKER_H_
#define VIRTUAL_MEMORY_WORKER_H_

#include <v8.h>

#ifdef __APPLE__
  #include <mach/mach.h>
  #include <mach/task.h>
  #include <mach/mach_init.h>
  #include <mach/host_info.h>
  #include <mach/mach_host.h>
  #include <mach/mach_traps.h>
  #include <mach/mach_vm.h>
  #include <mach/shared_region.h>
  #include <sys/sysctl.h>
  #include <math.h>
#elif defined __linux__
  #include <sys/sysinfo.h>
  #include <string.h>
  #include <errno.h>
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "worker.h"

using namespace node;
using namespace std;

// Data struct
struct VirtualMemory {
  uint64_t total;
  uint64_t available;
  uint64_t used;
  double percent;
  uint64_t free;
  uint64_t buffer;
  uint64_t shared;
  uint64_t swap_total;
  uint64_t swap_free;
  uint64_t active;
  uint64_t inactive;
  uint64_t wired;
};

// Contains the information about the worker to be processes in the work queue
class VirtualMemoryWorker : public Worker {
  public:
    VirtualMemoryWorker();
    ~VirtualMemoryWorker();

    VirtualMemory *results;

    void execute();
    Handle<Value> map();
};

#endif  // VIRTUAL_MEMORY_WORKER_H_
