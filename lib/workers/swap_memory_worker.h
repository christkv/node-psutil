#ifndef SWAP_MEMORY_WORKER_H_
#define SWAP_MEMORY_WORKER_H_

#include <v8.h>
#include "worker.h"

// Data struct
struct SwapMemory {
  uint64_t total;
  uint64_t used;
  uint64_t free;
  double percent;
  uint64_t sin;
  uint64_t sout;
};

class SwapMemoryWorker : public Worker {
  public:
    SwapMemoryWorker();
    ~SwapMemoryWorker();

    SwapMemory *results;

    void execute();
    v8::Handle<v8::Value> map();
};

#endif  // SWAP_MEMORY_WORKER_H_
