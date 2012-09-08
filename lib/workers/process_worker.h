#ifndef PROCESS_WORKER_H_
#define PROCESS_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace v8;
using namespace node;
using namespace std;

const uint32_t PSUTIL_NAME = 0;
const uint32_t PSUTIL_PPID = 1;
const uint32_t PSUTIL_EXE = 2;
const uint32_t PSUTIL_IO_COUNTERS = 3;
const uint32_t PSUTIL_CPU_TIMES = 4;

class ProcessWorker : public Worker {
  public:
    ProcessWorker();
    ~ProcessWorker();

    uint64_t pid;
    uint32_t operation;
    v8::Persistent<v8::Object> parameters;
    // Data pointer
    char* char_data;
    long long_data;
    float *float_data;

    void execute();
    v8::Handle<v8::Value> map();

  #ifdef __APPLE__
    void process_cpu_times();
    void process_exe();
    void process_name();
    void process_ppid();
    int get_kinfo_proc(pid_t pid, struct kinfo_proc *kp);
    int pid_exists(long pid);
    int _proc_pidinfo(long pid, int flavor, void *pti, int size);
  #endif
};
#endif  // PROCESS_WORKER_H_
