#ifndef SYSCONF_WORKER_H_
#define SYSCONF_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__
#elif defined __linux__
  #include <unistd.h>
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "worker.h"

// using namespace v8;
using namespace node;
using namespace std;

#ifdef __linux__

const uint32_t __SC_CLK_TCK = 0;

// Contains the information about the worker to be processes in the work queue
class SysconfWorker : public Worker {
  public:
    SysconfWorker() {}
    ~SysconfWorker() {}

    uint32_t name;
    long result;

    void inline execute()
    {
      long value;
      uint32_t selected;

      if(this->name == __SC_CLK_TCK) {
        selected = _SC_CLK_TCK;
      } else {
        selected = this->name;
      }

      value = sysconf(selected);
      if(value == -1) {
        this->error = true;
        this->error_message = (char *)"value for configuration key not found";
        return;        
      }

      this->result = value;
    }

    Handle<Value> inline map()
    {
      return Number::New(this->result);
    }
};

#else
// Contains the information about the worker to be processes in the work queue
class SysconfWorker : public Worker {
  public:
    SysconfWorker() {}
    ~SysconfWorker() {}

    uint32_t name;
    long result;

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

#endif  // SYSCONF_WORKER_H_
