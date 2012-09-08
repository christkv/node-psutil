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
  #include <node.h>
  #include <sys/sysctl.h>
  #include <unistd.h>
  #include <math.h>
  #include <errno.h>
#elif defined __linux__
  //#include <devstat.h>      /* get io counters */
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "swap_memory_worker.h"

SwapMemoryWorker::SwapMemoryWorker() {}
SwapMemoryWorker::~SwapMemoryWorker() {}

#ifdef __APPLE__
void SwapMemoryWorker::execute()
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

v8::Handle<v8::Value> SwapMemoryWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Object::New();
  // Map the structure to the final object
  resultsObject->Set(v8::String::New("total"), v8::Number::New(this->results->total));
  resultsObject->Set(v8::String::New("used"), v8::Number::New(this->results->used));
  resultsObject->Set(v8::String::New("free"), v8::Number::New(this->results->free));
  resultsObject->Set(v8::String::New("percent"), v8::Number::New(this->results->percent));
  resultsObject->Set(v8::String::New("sin"), v8::Number::New(this->results->sin));
  resultsObject->Set(v8::String::New("sout"), v8::Number::New(this->results->sout));
  // Cleanup memory
  delete this->results;
  // Return final object
  return resultsObject;
}
#else
void SwapMemoryWorker::execute() {}
v8::Handle<v8::Value> SwapMemoryWorker::map() { return v8::Undefined(); }
#endif
