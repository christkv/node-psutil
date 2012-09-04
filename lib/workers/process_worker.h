#ifndef PROCESS_WORKER_H_
#define PROCESS_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <utmpx.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <libproc.h>
#include <sys/proc_info.h>
#include <netinet/tcp_fsm.h>
#include <arpa/inet.h>
#include <net/if_dl.h>
#include <pwd.h>

#include <mach/mach.h>
#include <mach/task.h>
#include <mach/mach_init.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/mach_traps.h>
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

const uint32_t NAME = 0;
const uint32_t PPID = 1;
const uint32_t EXE = 2;

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class ProcessWorker : public Worker {
  public:
    ProcessWorker() {}
    ~ProcessWorker() {
      parameters.Dispose();
    }

    uint64_t pid;
    uint32_t operation;
    Persistent<Object> parameters;
    // Data pointer
    char* char_data;
    long long_data;

    void inline execute()
    {
      if(this->operation == NAME) {
        this->process_name();
      } else if(this->operation == PPID) {
        this->process_ppid();
      } else if(this->operation == EXE) {
        this->process_exe();
      }
    }

    Handle<Value> inline map()
    {
      Handle<Value> result;

      if(this->error == true) {
        return Undefined();
      }

      if(this->operation == NAME) {
        result = String::New((char*)this->char_data);
      } else if(this->operation == PPID) {
        result = Number::New(this->long_data);
      } else if(this->operation == EXE) {
        result = String::New((char*)this->char_data);
      } else {
        result = Undefined();
      }
      // return the result
      return result;
    }

  protected:
    void inline process_exe()
    {
      char buf[PATH_MAX];
      int ret;

      ret = proc_pidpath(this->pid, &buf, sizeof(buf));
      if(ret == 0) {
        if(!pid_exists(this->pid)) {
          this->error = true;
          this->error_message = (char *)"Unable to get the process's information or no such process.";
          return;
        } else {
          this->error = true;
          this->error_message = (char *)"Access denied to the process.";
          return;
        }
      }
      // Return the handle
      this->char_data = (char *)&buf;
    }

    void inline process_name()
    {
      struct kinfo_proc kp;

      if(get_kinfo_proc(this->pid, &kp) == -1) {
        this->error = true;
        this->error_message = (char *)"Unable to get the process's information or no such process.";
        return;
      }
      // Return the handle
      this->char_data = kp.kp_proc.p_comm;
    }

    void inline process_ppid()
    {
      struct kinfo_proc kp;

      if(get_kinfo_proc(pid, &kp) == -1) {
        this->error = true;
        this->error_message = (char *)"Unable to get the process's information or no such process.";
        return;
      }
      // Return the handle
      this->long_data = (long)kp.kp_eproc.e_ppid;
    }

    int inline get_kinfo_proc(pid_t pid, struct kinfo_proc *kp)
    {
      int mib[4];
      size_t len;
      mib[0] = CTL_KERN;
      mib[1] = KERN_PROC;
      mib[2] = KERN_PROC_PID;
      mib[3] = pid;

      // fetch the info with sysctl()
      len = sizeof(struct kinfo_proc);
      // now read the data from sysctl
      if(sysctl(mib, 4, kp, &len, NULL, 0) == -1) {
        return -1;
      }

      /*
       * sysctl succeeds but len is zero, happens when process has gone away
       */
      if(len == 0) {
        return -1;
      }
      return 0;
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
class ProcessWorker : public Worker {
  public:
    ProcessWorker() {}
    ~ProcessWorker() {
      parameters.Dispose();
    }

    uint64_t pid;
    uint32_t operation;
    Persistent<Object> parameters;

    void inline execute()
    {
    }

    Handle<Value> inline map()
    {
      return Array::New();
    }
};
#endif

#endif  // PROCESS_WORKER_H_
