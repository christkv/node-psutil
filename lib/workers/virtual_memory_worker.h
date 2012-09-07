#ifndef VIRTUAL_MEMORY_WORKER_H_
#define VIRTUAL_MEMORY_WORKER_H_

#include <v8.h>
#include "worker.h"

// Data struct
struct VirtualMemory {
  uint64_t total;
  uint64_t available;
  uint64_t used;
  double percent;
  uint64_t free;
  uint64_t buffer;
  uint64_t shared;
  uint64_t swap_total;
  uint64_t swap_free;
  uint64_t active;
  uint64_t inactive;
  uint64_t wired;
};

// Contains the information about the worker to be processes in the work queue
class VirtualMemoryWorker : public Worker {
  public:
    VirtualMemoryWorker();
    ~VirtualMemoryWorker();

    VirtualMemory *results;

    void execute();
    v8::Handle<v8::Value> map();
};

#endif  // VIRTUAL_MEMORY_WORKER_H_
