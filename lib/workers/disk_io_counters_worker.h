#ifndef DISK_IO_COUNTERS_WORKER_H_
#define DISK_IO_COUNTERS_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace std;

// Data struct
struct DiskCounters {
  char* disk_name;
  int64_t reads;
  int64_t writes;
  int64_t read_bytes;
  int64_t write_bytes;
  int64_t read_time;
  int64_t write_time;
};

class DiskIOCountersWorker : public Worker {
  public:
    DiskIOCountersWorker();
    ~DiskIOCountersWorker();

    bool prDisk;
    vector<DiskCounters*> results;

    void execute();
    v8::Handle<v8::Value> map();
};

#endif  // DISK_IO_COUNTERS_WORKER_H_
