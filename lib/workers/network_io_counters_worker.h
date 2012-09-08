#ifndef NETWORK_IO_COUNTERS_WORKER_H_
#define NETWORK_IO_COUNTERS_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace std;

// Data struct
struct NetworkCounters {
  char* nic_name;
  int64_t bytes_sent;
  int64_t bytes_recv;
  int64_t packets_sent;
  int64_t packets_recv;
  int64_t errin;
  int64_t errout;
  int64_t dropin;
  int64_t dropout;
};

class NetworkIOCountersWorker : public Worker {
  public:
    NetworkIOCountersWorker();
    ~NetworkIOCountersWorker();

    bool prNic;
    vector<NetworkCounters*> results;

    void execute();
    v8::Handle<v8::Value> map();
};

#endif  // NETWORK_IO_COUNTERS_WORKER_H_
