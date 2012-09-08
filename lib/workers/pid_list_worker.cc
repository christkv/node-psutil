#include <v8.h>
#include <vector>

#ifdef __APPLE__

#include <node.h>
#include <sys/sysctl.h>
#include <mach/mach_vm.h>
#include <errno.h>

#elif defined __linux__
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "pid_list_worker.h"

using namespace std;

PidListWorker::PidListWorker() {}
PidListWorker::~PidListWorker() {}

#ifdef __APPLE__
// bool perCPU;
vector<uint64_t> results;

/*
* Returns a list of all BSD processes on the system.  This routine
* allocates the list and puts it in *procList and a count of the
* number of entries in *procCount.  You are responsible for freeing
* this list (use "free" from System framework).
* On success, the function returns 0.
* On error, the function returns a BSD errno value.
*/
int get_proc_list(kinfo_proc **procList, size_t *procCount)
{
  /* Declaring mib as const requires use of a cast since the
   * sysctl prototype doesn't include the const modifier. */
  static const int mib3[3] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
  size_t           size, size2;
  void            *ptr;
  int              err, lim = 8;  /* some limit */

  assert( procList != NULL);
  assert(*procList == NULL);
  assert(procCount != NULL);

  *procCount = 0;

  /* We start by calling sysctl with ptr == NULL and size == 0.
   * That will succeed, and set size to the appropriate length.
   * We then allocate a buffer of at least that size and call
   * sysctl with that buffer.  If that succeeds, we're done.
   * If that call fails with ENOMEM, we throw the buffer away
   * and try again.
   * Note that the loop calls sysctl with NULL again.  This is
   * is necessary because the ENOMEM failure case sets size to
   * the amount of data returned, not the amount of data that
   * could have been returned.
   */
  while (lim-- > 0) {
      size = 0;
      if (sysctl((int *)mib3, 3, NULL, &size, NULL, 0) == -1) {
          return errno;
      }

      size2 = size + (size >> 3);  /* add some */
      if (size2 > size) {
          ptr = malloc(size2);
          if (ptr == NULL) {
              ptr = malloc(size);
          } else {
              size = size2;
          }
      }
      else {
          ptr = malloc(size);
      }
      if (ptr == NULL) {
          return ENOMEM;
      }

      if (sysctl((int *)mib3, 3, ptr, &size, NULL, 0) == -1) {
          err = errno;
          free(ptr);
          if (err != ENOMEM) {
              return err;
          }

      } else {
          *procList = (kinfo_proc *)ptr;
          *procCount = size / sizeof(kinfo_proc);
          return 0;
      }
  }
  return ENOMEM;
}

void PidListWorker::execute()
{
  kinfo_proc *proclist = NULL;
  kinfo_proc *orig_address = NULL;
  size_t num_processes;
  size_t idx;

  if(get_proc_list(&proclist, &num_processes) != 0) {
    this->error = true;
    this->error_message = (char *)"failed to retrieve process list.";
    return;
  }

  if(num_processes > 0) {
    // save the address of proclist so we can free it later
    orig_address = proclist;
    for(idx=0; idx < num_processes; idx++) {
      this->results.push_back(proclist->kp_proc.p_pid);
      proclist++;
    }

    free(orig_address);
  }
}

v8::Handle<v8::Value> PidListWorker::map()
{
  // Process all the values
  vector<uint64_t>::const_iterator i;
  // HandleScope scope;
  v8::Local<v8::Array> resultsObject = v8::Array::New(this->results.size());
  // Just an array pointer
  int index = 0;
  // Go over all the items in the vector
  for(i = this->results.begin(); i != this->results.end(); i++) {
    // Reference the diskCounters
    uint64_t value = *i;
    // Add to the result object
    resultsObject->Set(index++, v8::Number::New(value));
  }
  // Return the result
  return resultsObject;
}
#else
void PidListWorker::execute() {}
v8::Handle<v8::Value> PidListWorker::map() { return v8::Undefined(); }
#endif
