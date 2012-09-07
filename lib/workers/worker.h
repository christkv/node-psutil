#ifndef WORKER_H_
#define WORKER_H_

#include <v8.h>
#include <node.h>

using namespace node;

class Worker {
  public:
    Worker();
    virtual ~Worker();

    // libuv's request struct.
    uv_work_t request;
    // Callback
    v8::Persistent<v8::Function> callback;
    // Was there an error
    bool error;
    // The error message
    char *error_message;

    // Virtual execute function
    void virtual execute();
    // Map to output object
    v8::Handle<v8::Value> virtual map();
};

#endif  // WORKER_H_
