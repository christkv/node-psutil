#ifdef __APPLE__
  #include <CoreFoundation/CoreFoundation.h>
  #include <IOKit/IOKitLib.h>
  #include <IOKit/storage/IOBlockStorageDriver.h>
  #include <IOKit/storage/IOMedia.h>
  #include <IOKit/IOBSD.h>
#elif defined __linux__
  //#include <devstat.h>      /* get io counters */
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "disk_io_counters_worker.h"

DiskIOCountersWorker::DiskIOCountersWorker() {}
DiskIOCountersWorker::~DiskIOCountersWorker() {}

#ifdef __APPLE__
void DiskIOCountersWorker::execute()
{
  CFDictionaryRef parent_dict;
  CFDictionaryRef props_dict;
  CFDictionaryRef stats_dict;
  io_iterator_t disk_list;
  io_registry_entry_t parent;
  io_registry_entry_t disk;

  /* Get list of disks */
  if(IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOMediaClass), &disk_list) != kIOReturnSuccess)
  {
    this->error = true;
    this->error_message = (char *)"Unable to get the list of disks.";
    return;
  }

  /* Iterate over disks */
  while ((disk = IOIteratorNext(disk_list)) != 0)
  {
    if(IORegistryEntryGetParentEntry(disk, kIOServicePlane, &parent) != kIOReturnSuccess) {
      this->error = true;
      this->error_message = (char *)"Unable to get the disk's parent.";
      // Release resources
      IOObjectRelease(disk);
      return;
    }

    if(IOObjectConformsTo(parent, "IOBlockStorageDriver")) {
      if(IORegistryEntryCreateCFProperties(disk, (CFMutableDictionaryRef *) &parent_dict, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess)
      {
          this->error = true;
          this->error_message = (char *)"Unable to get the parent's properties.";
          // Release resources
          IOObjectRelease(disk);
          IOObjectRelease(parent);
          return;
      }

      if (IORegistryEntryCreateCFProperties(parent, (CFMutableDictionaryRef *) &props_dict, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess)
      {
          this->error = true;
          this->error_message = (char *)"Unable to get the disk properties.";
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
      char *disk_name = (char*)malloc(kMaxDiskNameSize * sizeof(char));
      // Load the name into the disk_array
      CFStringGetCString(disk_name_ref, disk_name, kMaxDiskNameSize, CFStringGetSystemEncoding());
      // Extract the statistics for the drive
      stats_dict = (CFDictionaryRef)CFDictionaryGetValue(props_dict, CFSTR(kIOBlockStorageDriverStatisticsKey));

      // If get no statistics error out
      if(stats_dict == NULL) {
        this->error = true;
        this->error_message = (char *)"Unable to get disk stats.";
        return;
      }

      CFNumberRef number;
      int64_t reads, writes, read_bytes, write_bytes, read_time, write_time = 0;

      /* Get disk reads/writes */
      if((number = (CFNumberRef)CFDictionaryGetValue(stats_dict, CFSTR(kIOBlockStorageDriverStatisticsReadsKey))))
      {
        CFNumberGetValue(number, kCFNumberSInt64Type, &reads);
      }

      if((number = (CFNumberRef)CFDictionaryGetValue(stats_dict, CFSTR(kIOBlockStorageDriverStatisticsWritesKey))))
      {
        CFNumberGetValue(number, kCFNumberSInt64Type, &writes);
      }

      /* Get disk bytes read/written */
      if ((number = (CFNumberRef)CFDictionaryGetValue(stats_dict, CFSTR(kIOBlockStorageDriverStatisticsBytesReadKey))))
      {
        CFNumberGetValue(number, kCFNumberSInt64Type, &read_bytes);
      }

      if ((number = (CFNumberRef)CFDictionaryGetValue(stats_dict, CFSTR(kIOBlockStorageDriverStatisticsBytesWrittenKey))))
      {
        CFNumberGetValue(number, kCFNumberSInt64Type, &write_bytes);
      }

      /* Get disk time spent reading/writing (nanoseconds) */
      if((number = (CFNumberRef)CFDictionaryGetValue(stats_dict, CFSTR( ))))
      {
        CFNumberGetValue(number, kCFNumberSInt64Type, &read_time);
      }

      if((number = (CFNumberRef)CFDictionaryGetValue(stats_dict, CFSTR(kIOBlockStorageDriverStatisticsTotalWriteTimeKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &write_time);
      }

      // Create a struct to store the disk information
      DiskCounters *diskCounter = new DiskCounters();
      diskCounter->disk_name = disk_name;
      diskCounter->reads = reads;
      diskCounter->writes = writes;
      diskCounter->read_bytes = read_bytes;
      diskCounter->write_bytes = write_bytes;
      diskCounter->read_time = read_time / 1000;
      diskCounter->write_time = write_time / 1000;
      // Add to vector
      this->results.push_back(diskCounter);
    }
  }
}

v8::Handle<v8::Value> DiskIOCountersWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Object::New();

  // If not pr disk accumulate all the data
  if(!prDisk) {
    vector<DiskCounters*>::const_iterator i;

    // All accumulators
    int64_t reads, writes, read_bytes, write_bytes, read_time, write_time;

    for(i = this->results.begin(); i != this->results.end(); i++) {
      // Reference the diskCounters
      DiskCounters *diskCounters = *i;
      reads += diskCounters->reads;
      writes += diskCounters->writes;
      read_bytes += diskCounters->read_bytes;
      write_bytes += diskCounters->write_bytes;
      read_time += diskCounters->read_time;
      write_time += diskCounters->write_time;

      // Clean up memory
      free(diskCounters->disk_name);
      delete diskCounters;
    }

    // Set values
    resultsObject->Set(v8::String::New("read_count"), v8::Number::New(reads));
    resultsObject->Set(v8::String::New("write_count"), v8::Number::New(writes));
    resultsObject->Set(v8::String::New("read_bytes"), v8::Number::New(read_bytes));
    resultsObject->Set(v8::String::New("write_bytes"), v8::Number::New(write_bytes));
    resultsObject->Set(v8::String::New("read_time"), v8::Number::New(read_time));
    resultsObject->Set(v8::String::New("write_time"), v8::Number::New(write_time));
  } else {
    vector<DiskCounters*>::const_iterator i;

    for(i = this->results.begin(); i != this->results.end(); i++) {
      // Reference the diskCounters
      DiskCounters *diskCounters = *i;
      // DiskObject
      v8::Local<v8::Object> diskObject = v8::Object::New();
      diskObject->Set(v8::String::New("read_count"), v8::Number::New(diskCounters->reads));
      diskObject->Set(v8::String::New("write_count"), v8::Number::New(diskCounters->writes));
      diskObject->Set(v8::String::New("read_bytes"), v8::Number::New(diskCounters->read_bytes));
      diskObject->Set(v8::String::New("write_bytes"), v8::Number::New(diskCounters->write_bytes));
      diskObject->Set(v8::String::New("read_time"), v8::Number::New(diskCounters->read_time));
      diskObject->Set(v8::String::New("write_time"), v8::Number::New(diskCounters->write_time));

      // Add to the result object
      resultsObject->Set(v8::String::New(diskCounters->disk_name, strlen(diskCounters->disk_name)), diskObject);
      // Clean up memory
      free(diskCounters->disk_name);
      delete diskCounters;
    }
  }
  // Return final object
  return resultsObject;
}
#else
  void DiskIOCountersWorker::execute() {}
  v8::Handle<v8::Value> DiskIOCountersWorker::map() { return v8::Undefined(); }
#endif
