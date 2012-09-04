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
  //#include <devstat.h>      /* get io counters */
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "worker.h"

// using namespace v8;
using namespace node;
using namespace std;

// Data struct
struct VirtualMemory {
  uint64_t total;
  uint64_t available;
  double percent;
  uint64_t used;
  uint64_t free;
  uint64_t active;
  uint64_t inactive;
  uint64_t wired;
};

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class VirtualMemoryWorker : public Worker {
  public:
    VirtualMemoryWorker() {}
    ~VirtualMemoryWorker() {}

    VirtualMemory *results;

    void inline execute()
    {
      int      mib[2];
      uint64_t total;
      size_t   len = sizeof(total);
      vm_statistics_data_t vm;
      int pagesize = getpagesize();

      // physical mem
      mib[0] = CTL_HW;
      mib[1] = HW_MEMSIZE;

      // Execute sys call
      if(sysctl(mib, 2, &total, &len, NULL, 0)) {
        if(errno != 0) {
          this->error = true;
          this->error_message = strerror(errno);
          return;
        } else {
          this->error = true;
          this->error_message = (char *)"sysctl(HW_MEMSIZE) failed";
          return;
        }
      }

      kern_return_t ret;
      mach_msg_type_number_t count = sizeof(vm) / sizeof(integer_t);
      mach_port_t mport = mach_host_self();

      ret = host_statistics(mport, HOST_VM_INFO, (host_info_t)&vm, &count);
      if(ret != KERN_SUCCESS) {
        // Contains the message
        char message[256];
        // Prepare the error message
        sprintf(message, "host_statistics() failed: %s", mach_error_string(ret));
        // Prepare error
        this->error = true;
        this->error_message = message;
        return;
      }

      // Create result object
      this->results = new VirtualMemory();
      this->results->total = total;
      this->results->active = vm.active_count * pagesize;
      this->results->inactive = vm.inactive_count * pagesize;
      this->results->wired = vm.wire_count * pagesize;
      this->results->free = vm.free_count * pagesize;
      this->results->available = this->results->inactive + this->results->free;
      this->results->used = this->results->active + this->results->inactive + this->results->wired;
      this->results->percent = ((double)(this->results->total - this->results->available) / this->results->total) * 100;
      this->results->percent = ceilf(this->results->percent * 100.0) / 100.0;
    }

    Handle<Value> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      // Map the structure to the final object
      resultsObject->Set(String::New("total"), Number::New(this->results->total));
      resultsObject->Set(String::New("active"), Number::New(this->results->active));
      resultsObject->Set(String::New("inactive"), Number::New(this->results->inactive));
      resultsObject->Set(String::New("wired"), Number::New(this->results->wired));
      resultsObject->Set(String::New("free"), Number::New(this->results->free));
      resultsObject->Set(String::New("available"), Number::New(this->results->available));
      resultsObject->Set(String::New("used"), Number::New(this->results->used));
      resultsObject->Set(String::New("percent"), Number::New(this->results->percent));

      // Cleanup memory
      delete this->results;

      // Return final object
      return resultsObject;
    }
};
#else
// Contains the information about the worker to be processes in the work queue
class VirtualMemoryWorker : public Worker {
  public:
    VirtualMemoryWorker() {}
    ~VirtualMemoryWorker() {}

    void inline execute()
    {
    }

    Handle<Value> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      // Return final object
      return resultsObject;
    }
};
#endif

#endif  // VIRTUAL_MEMORY_WORKER_H_
