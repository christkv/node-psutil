#ifndef PID_EXISTS_WORKER_H_
#define PID_EXISTS_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__

#include <sys/sysctl.h>
#include <mach/mach_vm.h>

#elif defined __linux__
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "worker.h"

using namespace v8;
using namespace node;
using namespace std;

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class PidExistsWorker : public Worker {
  public:
    PidExistsWorker() {}
    ~PidExistsWorker() {}

    bool exists;
    uint64_t pid;

    void inline execute()
    {
      this->exists = pid_exists(this->pid) == 1 ? true : false;
    }

    Handle<Value> inline map()
    {
      return v8::Boolean::New(this->exists);
    }

    /*
     * Return 1 if PID exists in the current process list, else 0.
     */
    int inline pid_exists(long pid)
    {
      int kill_ret;

      // save some time if it's an invalid PID
      if (pid < 0) {
        return 0;
      }

      // if kill returns success of permission denied we know it's a valid PID
      kill_ret = kill(pid , 0);
      if( (0 == kill_ret) || (EPERM == errno) ) {
        return 1;
      }

      // otherwise return 0 for PID not found
      return 0;
    }
};

#else
// Contains the information about the worker to be processes in the work queue
class PidExistsWorker : public Worker {
  public:
    PidExistsWorker() {}
    ~PidExistsWorker() {}

    bool exists;
    uint64_t pid;

    void inline execute()
    {
    }

    Handle<Value> inline map()
    {
      return v8::Boolean::New(false);
    }
};
#endif

#endif  // PID_EXISTS_WORKER_H_
