#ifndef DISK_USAGE_WORKER_H_
#define DISK_USAGE_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__
  #include <sys/statvfs.h>
  #include <sys/types.h>
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
struct DiskUsage {
  uint64_t total;
  uint64_t used;
  uint64_t free;
  double percent;
};

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class DiskUsageWorker : public Worker {
  public:
    DiskUsageWorker() {}
    ~DiskUsageWorker() {
      free(this->path);
    }

    char* path;
    DiskUsage* result;

    void inline execute()
    {
      int num;
      struct statvfs stat;

      num = statvfs(this->path, &stat);
      if(num == -1) {
        this->error = true;
        this->error_message = strerror(errno);
        return;
      }

      // Set up the result
      this->result = new DiskUsage();
      this->result->total = (stat.f_blocks * stat.f_frsize);
      this->result->free = (stat.f_bavail * stat.f_frsize);
      this->result->used = (stat.f_blocks - stat.f_bfree) * stat.f_frsize;
      this->result->percent = ((double)this->result->used/(double)this->result->total);
      this->result->percent = round((this->result->percent * 100.0) * 10) / 10;
    }

    Handle<Value> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      resultsObject->Set(String::New("total"), Number::New(this->result->total));
      resultsObject->Set(String::New("free"), Number::New(this->result->free));
      resultsObject->Set(String::New("used"), Number::New(this->result->used));
      resultsObject->Set(String::New("percent"), Number::New(this->result->percent));
      // Free memory
      delete this->result;
      // Return final object
      return resultsObject;
    }
};
#else
// Contains the information about the worker to be processes in the work queue
class DiskUsageWorker : public Worker {
  public:
    DiskUsageWorker() {}
    ~DiskUsageWorker() {}

    char* path;
    DiskUsage result;

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

#endif  // DISK_USAGE_WORKER_H_
