//===========================================================================

#ifndef PSUTIL_LIB_OSX_H_
#define PSUTIL_LIB_OSX_H_

//===========================================================================

#define USE_MISALIGNED_MEMORY_ACCESS 1

#include <node.h>
#include <node_object_wrap.h>
#include <v8.h>

using namespace v8;
using namespace node;

class PSUtilLib : public ObjectWrap {
  public:
    PSUtilLib();
    ~PSUtilLib() {}

    static void Initialize(Handle<Object> target);

    // Methods for data collection
    static Handle<Value> IoStat(const Arguments& args);

    // Handles the uv calls
    static void Process(uv_work_t* work_req);
    // Called after work is done
    static void After(uv_work_t* work_req);

    // Constructor used for creating new BSON objects from C++
    static Persistent<FunctionTemplate> constructor_template;

  private:
    static Handle<Value> New(const Arguments &args);
};

class Worker {
  public:
    Worker() {}
    ~Worker() {}

    // libuv's request struct.
    uv_work_t request;
    // Callback
    Persistent<Function> callback;
    // Was there an error
    bool error;
    // The error message
    std::string error_message;
};

// Contains the information about the worker to be processes in the work queue
class IoStatWorker : public Worker {
  public:
    IoStatWorker() {}
    ~IoStatWorker() {}
};

//===========================================================================

#endif  // PSUTIL_LIB_OSX_H_

//===========================================================================
