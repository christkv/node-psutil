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

#include "psutil_osx.h"

using namespace v8;
using namespace node;

static Handle<Value> VException(const char *msg)
{
  HandleScope scope;
  return ThrowException(Exception::Error(String::New(msg)));
}

Persistent<FunctionTemplate> PSUtil::constructor_template;

PSUtil::PSUtil() : ObjectWrap()
{
}

void PSUtil::Initialize(v8::Handle<v8::Object> target)
{
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("PSUtil"));

  // Instance methods
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "calculateObjectSize", CalculateObjectSize);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "serialize", BSONSerialize);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "serializeWithBufferAndIndex", SerializeWithBufferAndIndex);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "deserialize", BSONDeserialize);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "deserializeStream", BSONDeserializeStream);

  target->ForceSet(String::NewSymbol("PSUtil"), constructor_template->GetFunction());
}

// Create a new instance of BSON and passing it the existing context
Handle<Value> PSUtil::New(const Arguments &args)
{
  HandleScope scope;

  // // Check that we have an array
  // if(args.Length() == 1 && args[0]->IsArray())
  // {
  //   // Cast the array to a local reference
  //   Local<Array> array = Local<Array>::Cast(args[0]);

  //   if(array->Length() > 0)
  //   {
  //     // Create a bson object instance and return it
  //     BSON *bson = new BSON();

  //     uint32_t foundClassesMask = 0;

  //     // Iterate over all entries to save the instantiate funtions
  //     for(uint32_t i = 0; i < array->Length(); i++)
  //     {
  //       // Let's get a reference to the function
  //       Local<Function> func = Local<Function>::Cast(array->Get(i));
  //       Local<String> functionName = func->GetName()->ToString();

  //       // Save the functions making them persistant handles (they don't get collected)
  //       if(functionName->StrictEquals(bson->longString))
  //       {
  //         bson->longConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 1;
  //       }
  //       else if(functionName->StrictEquals(bson->objectIDString))
  //       {
  //         bson->objectIDConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 2;
  //       }
  //       else if(functionName->StrictEquals(bson->binaryString))
  //       {
  //         bson->binaryConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 4;
  //       }
  //       else if(functionName->StrictEquals(bson->codeString))
  //       {
  //         bson->codeConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 8;
  //       }
  //       else if(functionName->StrictEquals(bson->dbrefString))
  //       {
  //         bson->dbrefConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 0x10;
  //       }
  //       else if(functionName->StrictEquals(bson->symbolString))
  //       {
  //         bson->symbolConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 0x20;
  //       }
  //       else if(functionName->StrictEquals(bson->doubleString))
  //       {
  //         bson->doubleConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 0x40;
  //       }
  //       else if(functionName->StrictEquals(bson->timestampString))
  //       {
  //         bson->timestampConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 0x80;
  //       }
  //       else if(functionName->StrictEquals(bson->minKeyString))
  //       {
  //         bson->minKeyConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 0x100;
  //       }
  //       else if(functionName->StrictEquals(bson->maxKeyString))
  //       {
  //         bson->maxKeyConstructor = Persistent<Function>::New(func);
  //         foundClassesMask |= 0x200;
  //       }
  //     }

  //     // Check if we have the right number of constructors otherwise throw an error
  //     if(foundClassesMask != 0x3ff)
  //     {
  //       delete bson;
  //       return VException("Missing function constructor for either [Long/ObjectID/Binary/Code/DbRef/Symbol/Double/Timestamp/MinKey/MaxKey]");
  //     }
  //     else
  //     {
  //       bson->Wrap(args.This());
  //       return args.This();
  //     }
  //   }
  //   else
  //   {
  //     return VException("No types passed in");
  //   }
  // }
  // else
  // {
  //   return VException("Argument passed in must be an array of types");
  // }
}

// Exporting function
extern "C" void init(Handle<Object> target)
{
  HandleScope scope;
  PSUtil::Initialize(target);
}

// NODE_MODULE(bson, BSON::Initialize);
// NODE_MODULE(l, Long::Initialize);
