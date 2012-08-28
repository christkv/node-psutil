//===========================================================================

#ifndef PSUTIL_OSX_H_
#define PSUTIL_OSX_H_

//===========================================================================

#define USE_MISALIGNED_MEMORY_ACCESS 1

#include <node.h>
#include <node_object_wrap.h>
#include <v8.h>

using namespace v8;
using namespace node;

class PSUtil : public ObjectWrap {
public:
  PSUtil();
  ~PSUtil() {}

  static void Initialize(Handle<Object> target);

  // Constructor used for creating new BSON objects from C++
  static Persistent<FunctionTemplate> constructor_template;

private:
  static Handle<Value> New(const Arguments &args);
};

//===========================================================================

#endif  // PSUTIL_OSX_H_

//===========================================================================
