#ifndef DISK_PARTITIONS_WORKER_H_
#define DISK_PARTITIONS_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace std;

// Data struct
struct DiskPartition {
  char* device;
  char* mount_point;
  char* fstype;
  char* opts;
};

// Contains the information about the worker to be processes in the work queue
class DiskPartitionsWorker : public Worker {
  public:
    DiskPartitionsWorker();
    ~DiskPartitionsWorker();

    bool prDisk;
    vector<DiskPartition*> results;

    void execute();
    v8::Handle<v8::Value> map();
};
#endif  // DISK_PARTITIONS_WORKER_H_
