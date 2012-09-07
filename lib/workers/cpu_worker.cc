#include "cpu_worker.h"

#ifdef __APPLE__
#include <sys/proc_info.h>
#include <netinet/tcp_fsm.h>
#include <arpa/inet.h>
#include <net/if_dl.h>
#include <mach/host_info.h>
#include <sys/sysctl.h>
#include <math.h>
#elif defined __linux__
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

using namespace std;
using namespace v8;

CPUWorker::CPUWorker() {
}

CPUWorker::~CPUWorker() {
}

#ifdef __APPLE__
void CPUWorker::execute()
{
  // If we are collecting the information by cpu
  if(!this->perCPU) {
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    kern_return_t error;
    host_cpu_load_info_data_t r_load;

    error = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&r_load, &count);
    if(error != KERN_SUCCESS) {
      // Contains the message
      char message[256];
      // Prepare the error message
      sprintf(message, "host_statistics() failed: %s", mach_error_string(error));
      // Prepare error
      this->error = true;
      this->error_message = message;
      return;
    }

    // Create a struct to store the disk information
    CPUStatistics *cpuStatistics = new CPUStatistics();
    cpuStatistics->user = (double)r_load.cpu_ticks[CPU_STATE_USER];
    cpuStatistics->nice = (double)r_load.cpu_ticks[CPU_STATE_NICE];
    cpuStatistics->system = (double)r_load.cpu_ticks[CPU_STATE_SYSTEM];
    cpuStatistics->idle = (double)r_load.cpu_ticks[CPU_STATE_IDLE];
    // Add to vector
    this->results.push_back(cpuStatistics);
  } else {
    natural_t cpu_count, i;
    processor_info_array_t info_array;
    mach_msg_type_number_t info_count;
    kern_return_t error;
    processor_cpu_load_info_data_t* cpu_load_info = NULL;
    int ret;

    error = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpu_count, &info_array, &info_count);
    if(error != KERN_SUCCESS) {
      // Contains the message
      char message[256];
      // Prepare the error message
      sprintf(message, "Error in host_processor_info(): %s", mach_error_string(error));
      // Prepare error
      this->error = true;
      this->error_message = message;
      return;
    }

    cpu_load_info = (processor_cpu_load_info_data_t*) info_array;

    for(i = 0; i < cpu_count; i++) {
      // Create a struct to store the disk information
      CPUStatistics *cpuStatistics = new CPUStatistics();
      cpuStatistics->user = (double)cpu_load_info[i].cpu_ticks[CPU_STATE_USER];
      cpuStatistics->nice = (double)cpu_load_info[i].cpu_ticks[CPU_STATE_NICE];
      cpuStatistics->system = (double)cpu_load_info[i].cpu_ticks[CPU_STATE_SYSTEM];
      cpuStatistics->idle = (double)cpu_load_info[i].cpu_ticks[CPU_STATE_IDLE];
      // Add to vector
      this->results.push_back(cpuStatistics);
    }

    ret = vm_deallocate(mach_task_self(), (vm_address_t)info_array, info_count * sizeof(int));
    if(ret != KERN_SUCCESS) {
      this->error = true;
      this->error_message = (char *)"vm_deallocate() failed";
    }
  }
}

v8::Handle<v8::Value> CPUWorker::map()
{
  // If not pr disk accumulate all the data
  if(!this->perCPU) {
    vector<CPUStatistics*>::const_iterator i;
    // HandleScope scope;
    v8::Local<v8::Object> resultsObject = v8::Object::New();

    // All accumulators
    double user, nice, system, idle;

    for(i = this->results.begin(); i != this->results.end(); i++) {
      // Reference the diskCounters
      CPUStatistics *cpuStatistics = *i;
      user += (double)cpuStatistics->user;
      nice += (double)cpuStatistics->nice;
      system += (double)cpuStatistics->system;
      idle += (double)cpuStatistics->idle;

      // Clean up memory
      delete cpuStatistics;
    }

    // Set values
    resultsObject->Set(v8::String::New("user"), v8::Number::New(round(user)));
    resultsObject->Set(v8::String::New("nice"), v8::Number::New(round(nice)));
    resultsObject->Set(v8::String::New("system"), v8::Number::New(round(system)));
    resultsObject->Set(v8::String::New("idle"), v8::Number::New(round(idle)));
    // Return final object
    return resultsObject;
  } else {
    vector<CPUStatistics*>::const_iterator i;
    // HandleScope scope;
    v8::Local<v8::Array> resultsObject = v8::Array::New(this->results.size());
    // Just an array pointer
    int index = 0;

    for(i = this->results.begin(); i != this->results.end(); i++) {
      // Reference the diskCounters
      CPUStatistics *cpuStatistics = *i;
      // DiskObject
      v8::Local<v8::Object> result = v8::Object::New();
      result->Set(v8::String::New("user"), v8::Number::New(round(cpuStatistics->user)));
      result->Set(v8::String::New("nice"), v8::Number::New(round(cpuStatistics->nice)));
      result->Set(v8::String::New("system"), v8::Number::New(round(cpuStatistics->system)));
      result->Set(v8::String::New("idle"), v8::Number::New(round(cpuStatistics->idle)));

      // Add to the result object
      resultsObject->Set(index++, result);
      // Clean up memory
      delete cpuStatistics;
    }
    // Return final object
    return resultsObject;
  }
}
#else
void CPUWorker::execute() {}
v8::Handle<v8::Value> CPUWorker::map() { return v8::Undefined(); }
#endif
