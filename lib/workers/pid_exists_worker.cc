#include <v8.h>
#include <vector>
#include "pid_exists_worker.h"
#include <errno.h>

using namespace std;

PidExistsWorker::PidExistsWorker() {}
PidExistsWorker::~PidExistsWorker() {}

void PidExistsWorker::execute()
{
  this->exists = pid_exists(this->pid) == 1 ? true : false;
}

v8::Handle<v8::Value> PidExistsWorker::map()
{
  return v8::Boolean::New(this->exists);
}

/*
 * Return 1 if PID exists in the current process list, else 0.
 */
int PidExistsWorker::pid_exists(long pid)
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
