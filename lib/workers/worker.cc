#include "worker.h"
#include <v8.h>

Worker::Worker() {
}

Worker::~Worker() { 
}

void Worker::execute() {
  return;
}

v8::Handle<v8::Value> Worker::map() {
  return v8::Undefined();
}