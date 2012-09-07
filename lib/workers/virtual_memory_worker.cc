#include "virtual_memory_worker.h"

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
  #include <unistd.h>
  #include <errno.h>
#elif defined __linux__
  #include <sys/sysinfo.h>
  #include <string.h>
  #include <errno.h>
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

VirtualMemoryWorker::VirtualMemoryWorker() {
}

VirtualMemoryWorker::~VirtualMemoryWorker() {
}

#ifdef __APPLE__
void VirtualMemoryWorker::execute()
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

v8::Handle<v8::Value> VirtualMemoryWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Object::New();
  // Map the structure to the final object
  resultsObject->Set(v8::String::New("total"), v8::Number::New(this->results->total));
  resultsObject->Set(v8::String::New("active"), v8::Number::New(this->results->active));
  resultsObject->Set(v8::String::New("inactive"), v8::Number::New(this->results->inactive));
  resultsObject->Set(v8::String::New("wired"), v8::Number::New(this->results->wired));
  resultsObject->Set(v8::String::New("free"), v8::Number::New(this->results->free));
  resultsObject->Set(v8::String::New("available"), v8::Number::New(this->results->available));
  resultsObject->Set(v8::String::New("used"), v8::Number::New(this->results->used));
  resultsObject->Set(v8::String::New("percent"), v8::Number::New(this->results->percent));

  // Cleanup memory
  delete this->results;

  // Return final object
  return resultsObject;
}

#elif defined __linux__
void VirtualMemoryWorker::execute()
{
  struct sysinfo info;

  if(sysinfo(&info) != 0) {
    this->error = true;
    this->error_message = strerror(errno);
    return;
  }

  this->results = new VirtualMemory();
  this->results->total = (unsigned long long)info.totalram * info.mem_unit;
  this->results->free = (unsigned long long)info.freeram   * info.mem_unit;
  this->results->buffer = (unsigned long long)info.bufferram * info.mem_unit;
  this->results->shared = (unsigned long long)info.sharedram * info.mem_unit;
  this->results->swap_total = (unsigned long long)info.totalswap * info.mem_unit;
  this->results->swap_free = (unsigned long long)info.freeswap  * info.mem_unit;
}

v8::Handle<v8::Value> VirtualMemoryWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Object::New();
  // Map the structure to the final object
  resultsObject->Set(v8::String::New("total"), v8::Number::New(this->results->total));
  resultsObject->Set(v8::String::New("free"), v8::Number::New(this->results->free));
  resultsObject->Set(v8::String::New("buffer"), v8::Number::New(this->results->buffer));
  resultsObject->Set(v8::String::New("shared"), v8::Number::New(this->results->shared));
  resultsObject->Set(v8::String::New("swap_total"), v8::Number::New(this->results->swap_total));
  resultsObject->Set(v8::String::New("swap_free"), v8::Number::New(this->results->swap_free));

  // Cleanup memory
  delete this->results;
  // Return final object
  return resultsObject;
}
#else
#endif
