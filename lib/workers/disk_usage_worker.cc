#include <v8.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include "disk_usage_worker.h"
#include <errno.h>
#include <math.h>

#ifdef __linux__
  #include <stdlib.h>
#endif

using namespace std;

DiskUsageWorker::DiskUsageWorker() {}
DiskUsageWorker::~DiskUsageWorker() {
  free(this->path);
}

void DiskUsageWorker::execute()
{
  int num;
  struct statvfs stat;

  num = statvfs(this->path, &stat);
  if(num == -1) {
    this->error = true;
    this->error_message = strerror(errno);
    return;
  }

  // Set up the result
  this->result = new DiskUsage();
  this->result->total = (stat.f_blocks * stat.f_frsize);
  this->result->free = (stat.f_bavail * stat.f_frsize);
  this->result->used = (stat.f_blocks - stat.f_bfree) * stat.f_frsize;
  this->result->percent = ((double)this->result->used/(double)this->result->total);
  this->result->percent = round((this->result->percent * 100.0) * 10) / 10;
}

v8::Handle<v8::Value> DiskUsageWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Object::New();
  resultsObject->Set(v8::String::New("total"), v8::Number::New(this->result->total));
  resultsObject->Set(v8::String::New("free"), v8::Number::New(this->result->free));
  resultsObject->Set(v8::String::New("used"), v8::Number::New(this->result->used));
  resultsObject->Set(v8::String::New("percent"), v8::Number::New(this->result->percent));
  // Free memory
  delete this->result;
  // Return final object
  return resultsObject;
}
