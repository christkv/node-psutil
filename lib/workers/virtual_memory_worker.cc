#include "virtual_memory_worker.h"

VirtualMemoryWorker::VirtualMemoryWorker() { 
}

VirtualMemoryWorker::~VirtualMemoryWorker() {
}

void VirtualMemoryWorker::execute()
{
  struct sysinfo info;

  if(sysinfo(&info) != 0) {
    this->error = true;
    this->error_message = strerror(errno);
    return;
  }

  this->results = new VirtualMemory();
  this->results->total = (unsigned long long)info.totalram * info.mem_unit;
  this->results->free = (unsigned long long)info.freeram   * info.mem_unit;
  this->results->buffer = (unsigned long long)info.bufferram * info.mem_unit;
  this->results->shared = (unsigned long long)info.sharedram * info.mem_unit;
  this->results->swap_total = (unsigned long long)info.totalswap * info.mem_unit;
  this->results->swap_free = (unsigned long long)info.freeswap  * info.mem_unit;
}

v8::Handle<v8::Value> VirtualMemoryWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Object::New();
  // Map the structure to the final object
  resultsObject->Set(v8::String::New("total"), v8::Number::New(this->results->total));
  resultsObject->Set(v8::String::New("free"), v8::Number::New(this->results->free));
  resultsObject->Set(v8::String::New("buffer"), v8::Number::New(this->results->buffer));
  resultsObject->Set(v8::String::New("shared"), v8::Number::New(this->results->shared));
  resultsObject->Set(v8::String::New("swap_total"), v8::Number::New(this->results->swap_total));
  resultsObject->Set(v8::String::New("swap_free"), v8::Number::New(this->results->swap_free));

  // Cleanup memory
  delete this->results;
  // Return final object
  return resultsObject;
}
