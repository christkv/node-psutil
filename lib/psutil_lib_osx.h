//===========================================================================

#ifndef PSUTIL_LIB_OSX_H_
#define PSUTIL_LIB_OSX_H_

//===========================================================================

#define USE_MISALIGNED_MEMORY_ACCESS 1

#include <node.h>
#include <node_object_wrap.h>
#include <v8.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOBlockStorageDriver.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/IOBSD.h>

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

    // Virtual execute function
    void virtual execute();
    // Map to output object
    Local<Object> virtual map();
};

// Contains the information about the worker to be processes in the work queue
class IoStatWorker : public Worker {
  public:
    IoStatWorker() {}
    ~IoStatWorker() {}

    bool prDisk;
    // Contains the disk list
    io_iterator_t disk_list;

    void inline execute()
    {
      CFDictionaryRef parent_dict;
      CFDictionaryRef props_dict;
      CFDictionaryRef stats_dict;
      io_registry_entry_t parent;
      io_registry_entry_t disk;

      /* Get list of disks */
      if(IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOMediaClass), &disk_list) != kIOReturnSuccess)
      {
        this->error = true;
        this->error_message = "Unable to get the list of disks.";
        return;
      }

      /* Iterate over disks */
      while ((disk = IOIteratorNext(disk_list)) != 0)
      {
        if(IORegistryEntryGetParentEntry(disk, kIOServicePlane, &parent) != kIOReturnSuccess) {
          this->error = true;
          this->error_message = "Unable to get the disk's parent.";
          // Release resources
          IOObjectRelease(disk);
          return;
        }

        if(IOObjectConformsTo(parent, "IOBlockStorageDriver")) {
          if(IORegistryEntryCreateCFProperties(disk, (CFMutableDictionaryRef *) &parent_dict, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess)
          {
              this->error = true;
              this->error_message = "Unable to get the parent's properties.";
              // Release resources
              IOObjectRelease(disk);
              IOObjectRelease(parent);
              return;
          }

          if (IORegistryEntryCreateCFProperties(parent, (CFMutableDictionaryRef *) &props_dict, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess)
          {
              this->error = true;
              this->error_message = "Unable to get the disk properties.";
              // Release resources
              CFRelease(props_dict);
              IOObjectRelease(disk);
              IOObjectRelease(parent);
          }

          // Maximum size of the disk name
          const int kMaxDiskNameSize = 64;
          // Get reference to the name of the disk
          CFStringRef disk_name_ref = (CFStringRef)CFDictionaryGetValue(parent_dict, CFSTR(kIOBSDNameKey));
          // Allocated a char array for the name
          char disk_name[kMaxDiskNameSize];
          // Load the name into the disk_array
          CFStringGetCString(disk_name_ref, disk_name, kMaxDiskNameSize, CFStringGetSystemEncoding());
          // Extract the statistics for the drive
          stats_dict = (CFDictionaryRef)CFDictionaryGetValue(props_dict, CFSTR(kIOBlockStorageDriverStatisticsKey));

          // If get no statistics error out
          if(stats_dict == NULL) {
            this->error = true;
            this->error_message = "Unable to get disk stats.";
            return;
          }

        }
      }
    }

    Local<Object> inline map()
    {
      return Object::New();
    }
};

//===========================================================================

#endif  // PSUTIL_LIB_OSX_H_

//===========================================================================
