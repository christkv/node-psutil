#ifdef __APPLE__
#elif defined __linux__
  #include <unistd.h>
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "sysconf_worker.h"

SysconfWorker::SysconfWorker() {
}

SysconfWorker::~SysconfWorker() {
}

#ifdef __linux__
void SysconfWorker::execute()
{
  long value;
  uint32_t selected;

  if(this->name == __SC_CLK_TCK) {
    selected = _SC_CLK_TCK;
  } else {
    selected = this->name;
  }

  value = sysconf(selected);
  if(value == -1) {
    this->error = true;
    this->error_message = (char *)"value for configuration key not found";
    return;
  }

  this->result = value;
}

Handle<Value> SysconfWorker::map()
{
  return Number::New(this->result);
}
#else
void SysconfWorker::execute() {}
Handle<Value> SysconfWorker::map() { return Undefined(); }
#endif
