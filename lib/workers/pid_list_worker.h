#ifndef PID_LIST_WORKER_H_
#define PID_LIST_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace std;

// Contains the information about the worker to be processes in the work queue
class PidListWorker : public Worker {
  public:
    PidListWorker();
    ~PidListWorker();

    // bool perCPU;
    vector<uint64_t> results;

    void execute();
    v8::Handle<v8::Value> map();
};
#endif
