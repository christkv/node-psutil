#ifndef SYSCONF_WORKER_H_
#define SYSCONF_WORKER_H_

#include <v8.h>
#include "worker.h"

using namespace v8;

const uint32_t __SC_CLK_TCK = 0;

// Contains the information about the worker to be processes in the work queue
class SysconfWorker : public Worker {
  public:
    SysconfWorker();
    ~SysconfWorker();

    uint32_t name;
    long result;

    void execute();
    Handle<Value> map();
};
#endif
