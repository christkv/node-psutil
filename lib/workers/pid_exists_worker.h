#ifndef PID_EXISTS_WORKER_H_
#define PID_EXISTS_WORKER_H_

#include <v8.h>
#include "worker.h"

class PidExistsWorker : public Worker {
  public:
    PidExistsWorker();
    ~PidExistsWorker();

    bool exists;
    uint64_t pid;

    void execute();
    v8::Handle<v8::Value> map();
    int pid_exists(long pid);
};
#endif  // PID_EXISTS_WORKER_H_
