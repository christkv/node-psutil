#ifndef CPU_WORKER_H_
#define CPU_WORKER_H_

#include <v8.h>
#include <vector>

#include "worker.h"

using namespace std;

// Data struct
struct CPUStatistics {
  double user;
  double nice;
  double system;
  double idle;
};

class CPUWorker : public Worker {
  public:
    CPUWorker();
    ~CPUWorker();

    bool perCPU;
    vector<CPUStatistics*> results;

    void execute();
    v8::Handle<v8::Value> map();
};

#endif  // CPU_WORKER_H_
