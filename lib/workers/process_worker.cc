#include <v8.h>
#include <node.h>

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
#endif

#include "process_worker.h"

using namespace std;

ProcessWorker::ProcessWorker() {}
ProcessWorker::~ProcessWorker() {
  #ifdef __APPLE__
    parameters.Dispose();
  #endif
}

#ifdef __APPLE__
void ProcessWorker::process_cpu_times()
{
  struct proc_taskinfo pti;

  if(!_proc_pidinfo(this->pid, PROC_PIDTASKINFO, &pti, sizeof(pti))) {
    return;
  }

  // Allocate the data structure for storing the values
  this->float_data = (float*)malloc(sizeof(float) * 2);
  this->float_data[0] = (float)(pti.pti_total_user / 1000000000.0);
  this->float_data[1] = (float)(pti.pti_total_system / 1000000000.0);
}

void ProcessWorker::process_exe()
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

  // Make a copy of the data
  char *copy = (char *)malloc(sizeof(char) * PATH_MAX);
  strncpy(copy, (char *)&buf, PATH_MAX);

  // Return the handle
  this->char_data = copy;
}

void ProcessWorker::process_name()
{
  struct kinfo_proc kp;

  if(get_kinfo_proc(this->pid, &kp) == -1) {
    this->error = true;
    this->error_message = (char *)"Unable to get the process's information or no such process.";
    return;
  }

  // Make a copy of the data
  char *copy = (char *)malloc(sizeof(char) * 12);
  strncpy(copy, kp.kp_proc.p_comm, 12);

  // Return the handle
  this->char_data = copy;
}

void ProcessWorker::process_ppid()
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

int ProcessWorker::get_kinfo_proc(pid_t pid, struct kinfo_proc *kp)
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
int ProcessWorker::pid_exists(long pid)
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

/*
 * A thin wrapper around proc_pidinfo()
 */
int ProcessWorker::_proc_pidinfo(long pid, int flavor, void *pti, int size)
{
  int ret = proc_pidinfo((int)pid, flavor, 0, pti, size);
  if(ret == 0) {
    if(!pid_exists(this->pid)) {
      this->error = true;
      this->error_message = (char *)"Unable to get the process's information or no such process.";
      return 0;
    } else {
      this->error = true;
      this->error_message = (char *)"Access denied to the process.";
      return 0;
    }
  }
  else if(ret != size) {
    this->error = true;
    this->error_message = (char *)"Access denied to the process.";
    return 0;
  }
  else {
    return 1;
  }
}

void ProcessWorker::execute()
{
  if(this->operation == PSUTIL_NAME) {
    this->process_name();
  } else if(this->operation == PSUTIL_PPID) {
    this->process_ppid();
  } else if(this->operation == PSUTIL_EXE) {
    this->process_exe();
  } else if(this->operation == PSUTIL_CPU_TIMES) {
    this->process_cpu_times();
  } else {
    this->error = true;
    this->error_message = (char *)"Operation is not supported.";
  }
}

v8::Handle<v8::Value> ProcessWorker::map()
{
  v8::Handle<v8::Value> result;

  if(this->error == true) {
    return Undefined();
  }

  if(this->operation == PSUTIL_NAME) {
    result = String::New((char*)this->char_data);
    free(this->char_data);
  } else if(this->operation == PSUTIL_PPID) {
    result = Number::New(this->long_data);
  } else if(this->operation == PSUTIL_EXE) {
    result = String::New((char*)this->char_data);
    free(this->char_data);
  } else if(this->operation == PSUTIL_CPU_TIMES) {
    Local<Array> values = Array::New(2);
    values->Set(0, Number::New(this->float_data[0]));
    values->Set(1, Number::New(this->float_data[1]));
    result = values;
    free(float_data);
  } else {
    result = Undefined();
  }
  // return the result
  return result;
}

#else
void ProcessWorker::execute() {}
v8::Handle<v8::Value> ProcessWorker::map() { return v8::Undefined(); }
#endif
