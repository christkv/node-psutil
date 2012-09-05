#ifndef CPU_WORKER_H_
#define CPU_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__
#include <sys/proc_info.h>
#include <netinet/tcp_fsm.h>
#include <arpa/inet.h>
#include <net/if_dl.h>
#include <mach/host_info.h>
#include <sys/sysctl.h>
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
struct CPUStatistics {
  double user;
  double nice;
  double system;
  double idle;
};

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class CPUWorker : public Worker {
  public:
    CPUWorker() {}
    ~CPUWorker() {}

    bool perCPU;
    vector<CPUStatistics*> results;

    void inline execute()
    {
      // struct timespec *req = new timespec;
      // struct timespec *rem = new timespec;

      // req->tv_sec=0;
      // req->tv_nsec=0;
      // rem->tv_sec=0;
      // //  I want to sleep 935 micro seconds
      // rem->tv_nsec=935000000L;
      // nanosleep(rem, req);

      // If we are collecting the information by cpu
      if(!this->perCPU) {
        mach_msg_type_number_t  count = HOST_CPU_LOAD_INFO_COUNT;
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

    Handle<Value> inline map()
    {
      // If not pr disk accumulate all the data
      if(!this->perCPU) {
        vector<CPUStatistics*>::const_iterator i;
        // HandleScope scope;
        Local<Object> resultsObject = Object::New();

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
        resultsObject->Set(String::New("user"), Number::New(round(user)));
        resultsObject->Set(String::New("nice"), Number::New(round(nice)));
        resultsObject->Set(String::New("system"), Number::New(round(system)));
        resultsObject->Set(String::New("idle"), Number::New(round(idle)));
        // Return final object
        return resultsObject;
      } else {
        vector<CPUStatistics*>::const_iterator i;
        // HandleScope scope;
        Local<Array> resultsObject = Array::New(this->results.size());
        // Just an array pointer
        int index = 0;

        for(i = this->results.begin(); i != this->results.end(); i++) {
          // Reference the diskCounters
          CPUStatistics *cpuStatistics = *i;
          // DiskObject
          Local<Object> result = Object::New();
          result->Set(String::New("user"), Number::New(round(cpuStatistics->user)));
          result->Set(String::New("nice"), Number::New(round(cpuStatistics->nice)));
          result->Set(String::New("system"), Number::New(round(cpuStatistics->system)));
          result->Set(String::New("idle"), Number::New(round(cpuStatistics->idle)));

          // Add to the result object
          resultsObject->Set(index++, result);
          // Clean up memory
          delete cpuStatistics;
        }
        // Return final object
        return resultsObject;
      }
    }
};

#else
// Contains the information about the worker to be processes in the work queue
class CPUWorker : public Worker {
  public:
    CPUWorker() {}
    ~CPUWorker() {}

    bool perCPU;

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

#endif  // CPU_WORKER_H_
