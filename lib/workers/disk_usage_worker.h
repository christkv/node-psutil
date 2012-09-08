#ifndef DISK_USAGE_WORKER_H_
#define DISK_USAGE_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace std;

// Data struct
struct DiskUsage {
  uint64_t total;
  uint64_t used;
  uint64_t free;
  double percent;
};

class DiskUsageWorker : public Worker {
  public:
    DiskUsageWorker();
    ~DiskUsageWorker();

    char* path;
    DiskUsage* result;

    void execute();
    v8::Handle<v8::Value> map();
};
#endif  // DISK_USAGE_WORKER_H_
