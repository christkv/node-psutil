#ifndef SWAP_MEMORY_WORKER_H_
#define SWAP_MEMORY_WORKER_H_

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
struct SwapMemory {
  uint64_t total;
  uint64_t used;
  uint64_t free;
  double percent;
  uint64_t sin;
  uint64_t sout;
};

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class SwapMemoryWorker : public Worker {
  public:
    SwapMemoryWorker() {}
    ~SwapMemoryWorker() {}

    SwapMemory *results;

    void inline execute()
    {
      int mib[2];
      size_t size;
      struct xsw_usage totals;
      vm_statistics_data_t vm;
      int pagesize = getpagesize();

      mib[0] = CTL_VM;
      mib[1] = VM_SWAPUSAGE;
      size = sizeof(totals);
      if(sysctl(mib, 2, &totals, &size, NULL, 0) == -1) {
        if(errno != 0) {
          this->error = true;
          this->error_message = strerror(errno);
          return;
        } else {
          this->error = true;
          this->error_message = (char *)"sysctl(VM_SWAPUSAGE) failed";
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
      this->results = new SwapMemory();
      this->results->total = totals.xsu_total;
      this->results->used = totals.xsu_used;
      this->results->free = totals.xsu_avail;
      this->results->sin = (unsigned long long)vm.pageins * pagesize;
      this->results->sout = (unsigned long long)vm.pageouts * pagesize;
      this->results->percent = ((double)(this->results->used) / this->results->total) * 100;
      this->results->percent = ceilf(this->results->percent * 100.0) / 100.0;
    }

    Local<Object> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      // Map the structure to the final object
      resultsObject->Set(String::New("total"), Number::New(this->results->total));
      resultsObject->Set(String::New("used"), Number::New(this->results->used));
      resultsObject->Set(String::New("free"), Number::New(this->results->free));
      resultsObject->Set(String::New("percent"), Number::New(this->results->percent));
      resultsObject->Set(String::New("sin"), Number::New(this->results->sin));
      resultsObject->Set(String::New("sout"), Number::New(this->results->sout));

      // Cleanup memory
      delete this->results;

      // Return final object
      return resultsObject;
    }
};
#else
// Contains the information about the worker to be processes in the work queue
class SwapMemoryWorker : public Worker {
  public:
    SwapMemoryWorker() {}
    ~SwapMemoryWorker() {}

    void inline execute()
    {
    }

    Local<Object> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      // Return final object
      return resultsObject;
    }
};
#endif

#endif  // SWAP_MEMORY_WORKER_H_
