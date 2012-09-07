#ifndef VIRTUAL_MEMORY_WORKER_H_
#define VIRTUAL_MEMORY_WORKER_H_

#include <v8.h>
#include <node.h>

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

// using namespace v8;
// using namespace node;
// using namespace std;

// #ifdef __APPLE__
// // Data struct
// struct VirtualMemory {
//   uint64_t total;
//   uint64_t available;
//   double percent;
//   uint64_t used;
//   uint64_t free;
//   uint64_t active;
//   uint64_t inactive;
//   uint64_t wired;
// };

// // Contains the information about the worker to be processes in the work queue
// class VirtualMemoryWorker : public Worker {
//   public:
//     VirtualMemoryWorker();
//     ~VirtualMemoryWorker();

//     VirtualMemory *results;

//     void inline execute()
//     {
//       int      mib[2];
//       uint64_t total;
//       size_t   len = sizeof(total);
//       vm_statistics_data_t vm;
//       int pagesize = getpagesize();

//       // physical mem
//       mib[0] = CTL_HW;
//       mib[1] = HW_MEMSIZE;

//       // Execute sys call
//       if(sysctl(mib, 2, &total, &len, NULL, 0)) {
//         if(errno != 0) {
//           this->error = true;
//           this->error_message = strerror(errno);
//           return;
//         } else {
//           this->error = true;
//           this->error_message = (char *)"sysctl(HW_MEMSIZE) failed";
//           return;
//         }
//       }

//       kern_return_t ret;
//       mach_msg_type_number_t count = sizeof(vm) / sizeof(integer_t);
//       mach_port_t mport = mach_host_self();

//       ret = host_statistics(mport, HOST_VM_INFO, (host_info_t)&vm, &count);
//       if(ret != KERN_SUCCESS) {
//         // Contains the message
//         char message[256];
//         // Prepare the error message
//         sprintf(message, "host_statistics() failed: %s", mach_error_string(ret));
//         // Prepare error
//         this->error = true;
//         this->error_message = message;
//         return;
//       }

//       // Create result object
//       this->results = new VirtualMemory();
//       this->results->total = total;
//       this->results->active = vm.active_count * pagesize;
//       this->results->inactive = vm.inactive_count * pagesize;
//       this->results->wired = vm.wire_count * pagesize;
//       this->results->free = vm.free_count * pagesize;
//       this->results->available = this->results->inactive + this->results->free;
//       this->results->used = this->results->active + this->results->inactive + this->results->wired;
//       this->results->percent = ((double)(this->results->total - this->results->available) / this->results->total) * 100;
//       this->results->percent = ceilf(this->results->percent * 100.0) / 100.0;
//     }

//     v8::Handle<v8::Value> inline map()
//     {
//       // HandleScope scope;
//       v8::Local<v8::Object> resultsObject = v8::Object::New();
//       // Map the structure to the final object
//       resultsObject->Set(v8::String::New("total"), v8::Number::New(this->results->total));
//       resultsObject->Set(v8::String::New("active"), v8::Number::New(this->results->active));
//       resultsObject->Set(v8::String::New("inactive"), v8::Number::New(this->results->inactive));
//       resultsObject->Set(v8::String::New("wired"), v8::Number::New(this->results->wired));
//       resultsObject->Set(v8::String::New("free"), v8::Number::New(this->results->free));
//       resultsObject->Set(v8::String::New("available"), v8::Number::New(this->results->available));
//       resultsObject->Set(v8::String::New("used"), v8::Number::New(this->results->used));
//       resultsObject->Set(v8::String::New("percent"), v8::Number::New(this->results->percent));

//       // Cleanup memory
//       delete this->results;

//       // Return final object
//       return resultsObject;
//     }
// };
// #elif defined __linux__
class Worker;

// Data struct
struct VirtualMemory {
  uint64_t total;
  uint64_t free;
  uint64_t buffer;
  uint64_t shared;
  uint64_t swap_total;
  uint64_t swap_free;
};

// Contains the information about the worker to be processes in the work queue
class VirtualMemoryWorker : public Worker {
  public:
    VirtualMemoryWorker();
    ~VirtualMemoryWorker();

    // // libuv's request struct.
    // uv_work_t request;
    // // Callback
    // v8::Persistent<v8::Function> callback;
    // // Was there an error
    // bool error;
    // // The error message
    // char *error_message;

    VirtualMemory *results;

    void execute();
    v8::Handle<v8::Value> map();
};
// #else
// // Contains the information about the worker to be processes in the work queue
// class VirtualMemoryWorker : public Worker {
//   public:
//     VirtualMemoryWorker();
//     ~VirtualMemoryWorker();

//     void inline execute()
//     {
//     }

//     v8::Handle<v8::Value> inline map()
//     {
//       // HandleScope scope;
//       v8::Local<v8::Object> resultsObject = v8::Object::New();
//       // Return final object
//       return resultsObject;
//     }
// };
// #endif

#endif  // VIRTUAL_MEMORY_WORKER_H_
