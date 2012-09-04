//===========================================================================

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#include <v8.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <node.h>
#include <node_version.h>
#include <node_buffer.h>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include "workers/disk_io_counters_worker.h"
#include "workers/network_io_counters_worker.h"
#include "workers/virtual_memory_worker.h"
#include "workers/swap_memory_worker.h"
#include "workers/cpu_worker.h"
#include "workers/pid_list_worker.h"
#include "workers/pid_exists_worker.h"
#include "workers/process_worker.h"

#include "psutil_lib_osx.h"

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

// using namespace v8;
using namespace node;

static v8::Handle<Value> VException(const char *msg)
{
  HandleScope scope;
  return ThrowException(Exception::Error(String::New(msg)));
}

Persistent<FunctionTemplate> PSUtilLib::constructor_template;

PSUtilLib::PSUtilLib() : ObjectWrap()
{
}

void PSUtilLib::Initialize(v8::Handle<v8::Object> target)
{
  // Grab the scope of the call from Node
  HandleScope scope;

  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("PSUtilLib"));

  // Set up the available methods
  t->InstanceTemplate()->SetInternalFieldCount(1);

  // Set up the iostat command
  NODE_SET_PROTOTYPE_METHOD(t, "disk_io_counters", PSUtilLib::DiskIOCounters);
  NODE_SET_PROTOTYPE_METHOD(t, "network_io_counters", PSUtilLib::NetworkIOCounters);
  NODE_SET_PROTOTYPE_METHOD(t, "virtual_memory", PSUtilLib::VirtualMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "swap_memory", PSUtilLib::SwapMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "cpu_times", PSUtilLib::CPUPercent);
  NODE_SET_PROTOTYPE_METHOD(t, "pid_list", PSUtilLib::PidList);
  NODE_SET_PROTOTYPE_METHOD(t, "pid_exists", PSUtilLib::PidExists);
  NODE_SET_PROTOTYPE_METHOD(t, "process_info", PSUtilLib::ProcessInfo);

  // Set the name of the class
  target->ForceSet(String::NewSymbol("PSUtilLib"), constructor_template->GetFunction());
}

Handle<Value> PSUtilLib::NetworkIOCounters(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 2 && args[0]->IsBoolean() == false && args[1]->IsFunction() == false) return VException("function requires [boolean, function] or [function] 1");
  if(args.Length() == 1 && args[0]->IsFunction() == false) return VException("function requires [boolean, function] or [function] 2");
  // Get the callback
  Local<Function> callback;
  // If we have a single parameter
  callback = args.Length() == 1 ? Local<Function>::Cast(args[0]) : Local<Function>::Cast(args[1]);
  // Create a worker object and map the information
  NetworkIOCountersWorker *worker = new NetworkIOCountersWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Get the value of results being returned
  worker->prNic = args.Length() == 2 ? args[1]->ToBoolean()->BooleanValue() : false;
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

Handle<Value> PSUtilLib::DiskIOCounters(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 2 && args[0]->IsBoolean() == false && args[1]->IsFunction() == false) return VException("function requires [boolean, function] or [function]");
  if(args.Length() == 1 && args[0]->IsFunction() == false) return VException("function requires [boolean, function] or [function]");
  // Get the callback
  Local<Function> callback;
  // If we have a single parameter
  callback = args.Length() == 1 ? Local<Function>::Cast(args[0]) : Local<Function>::Cast(args[1]);
  // Create a worker object and map the information
  DiskIOCountersWorker *worker = new DiskIOCountersWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Get the value of results being returned
  worker->prDisk = args.Length() == 2 ? args[1]->ToBoolean()->BooleanValue() : false;
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

Handle<Value> PSUtilLib::VirtualMemory(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 1 && args[0]->IsFunction() == false) return VException("function requires [function]");
  // Get the callback
  Local<Function> callback;
  // If we have a single parameter
  callback = args.Length() == 1 ? Local<Function>::Cast(args[0]) : Local<Function>::Cast(args[1]);
  // Create a worker object and map the information
  VirtualMemoryWorker *worker = new VirtualMemoryWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

Handle<Value> PSUtilLib::SwapMemory(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 1 && args[0]->IsFunction() == false) return VException("function requires [function]");
  // Get the callback
  Local<Function> callback;
  // If we have a single parameter
  callback = args.Length() == 1 ? Local<Function>::Cast(args[0]) : Local<Function>::Cast(args[1]);
  // Create a worker object and map the information
  SwapMemoryWorker *worker = new SwapMemoryWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}


Handle<Value> PSUtilLib::CPUPercent(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 2 && !args[0]->IsBoolean() && !args[1]->IsFunction()) return VException("function requires [boolean, function] or [function]");
  // Get the callback
  Local<Function> callback = Local<Function>::Cast(args[1]);
  // Create a worker object and map the information
  CPUWorker *worker = new CPUWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Set parameters
  worker->perCPU = args[0]->ToBoolean()->BooleanValue();
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

Handle<Value> PSUtilLib::PidList(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 1 && !args[0]->IsFunction()) return VException("function requires [function]");
  // Get the callback
  Local<Function> callback = Local<Function>::Cast(args[0]);
  // Create a worker object and map the information
  PidListWorker *worker = new PidListWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

Handle<Value> PSUtilLib::PidExists(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 2 && !args[0]->IsNumber() && !args[1]->IsFunction()) return VException("function requires [number, function]");
  // Get the callback
  Local<Function> callback = Local<Function>::Cast(args[1]);
  // Create a worker object and map the information
  PidExistsWorker *worker = new PidExistsWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  // Set parameters
  worker->pid = args[0]->ToNumber()->Value();
  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

Handle<Value> PSUtilLib::ProcessInfo(const Arguments& args) {
  HandleScope scope;

  // Legal modes
  if(args.Length() == 4 && !args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsObject() && !args[3]->IsFunction()) return VException("function requires [number, number, object, function]");

  // Get the callback
  Local<Function> callback = Local<Function>::Cast(args[3]);

  // Create a worker object and map the information
  ProcessWorker *worker = new ProcessWorker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);

  // Set all data for the worker
  worker->pid = args[0]->ToNumber()->Value();
  worker->operation = args[1]->ToUint32()->Value();
  worker->parameters = Persistent<Object>::New(args[2]->ToObject());

  // Trigger the work
  uv_queue_work(uv_default_loop(), &worker->request, PSUtilLib::Process, PSUtilLib::After);
  // Return the handle to the instance
  return Undefined();
}

void PSUtilLib::Process(uv_work_t* work_req) {
  // Grab the worker
  Worker *worker = static_cast<Worker*>(work_req->data);
  // Execute the worker code
  worker->execute();
}

void PSUtilLib::After(uv_work_t* work_req) {
  // Grab the scope of the call from Node
  v8::HandleScope scope;

  // Get the worker reference
  Worker *worker = static_cast<Worker*>(work_req->data);

  // If we have an error
  if(worker->error) {
    v8::Local<v8::Value> err = v8::Exception::Error(v8::String::New(worker->error_message));
    v8::Local<v8::Value> args[2] = { err, v8::Local<v8::Value>::New(v8::Null()) };
    // Execute the error
    v8::TryCatch try_catch;
    // Call the callback
    worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
    // If we have an exception handle it as a fatalexception
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  } else {
    // Map the data
    v8::Handle<v8::Value> result = worker->map();
    // Set up the callback with a null first
    v8::Handle<v8::Value> args[2] = { v8::Local<v8::Value>::New(v8::Null()), result };
    // Wrap the callback function call in a TryCatch so that we can call
    // node's FatalException afterwards. This makes it possible to catch
    // the exception from JavaScript land using the
    // process.on('uncaughtException') event.
    v8::TryCatch try_catch;
    // Call the callback
    worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
    // If we have an exception handle it as a fatalexception
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }

  // Clean up the memory
  worker->callback.Dispose();
  delete worker;
}

// Create a new instance of BSON and passing it the existing context
Handle<Value> PSUtilLib::New(const Arguments &args)
{
  HandleScope scope;

  PSUtilLib *psUtil = new PSUtilLib();
  psUtil->Wrap(args.This());
  return args.This();
}

// Exporting function
extern "C" void init(Handle<Object> target)
{
  HandleScope scope;
  PSUtilLib::Initialize(target);
}
