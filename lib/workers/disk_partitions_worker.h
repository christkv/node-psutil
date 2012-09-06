#ifndef DISK_PARTITIONS_WORKER_H_
#define DISK_PARTITIONS_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__
  #include <assert.h>
  #include <errno.h>
  #include <stdbool.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <utmpx.h>
  #include <sys/sysctl.h>
  #include <sys/vmmeter.h>
  #include <libproc.h>
  #include <sys/proc_info.h>
  #include <netinet/tcp_fsm.h>
  #include <arpa/inet.h>
  #include <net/if_dl.h>
  #include <pwd.h>

  #include <mach/mach.h>
  #include <mach/task.h>
  #include <mach/mach_init.h>
  #include <mach/host_info.h>
  #include <mach/mach_host.h>
  #include <mach/mach_traps.h>
  #include <mach/mach_vm.h>

  #include <CoreFoundation/CoreFoundation.h>
  #include <IOKit/IOKitLib.h>
  #include <IOKit/storage/IOBlockStorageDriver.h>
  #include <IOKit/storage/IOMedia.h>
  #include <IOKit/IOBSD.h>
#elif defined __linux__
  //#include <devstat.h>      /* get io counters */
  #include <errno.h>
  #include <stdlib.h>
  #include <mntent.h>
  #include <utmp.h>
  #include <sched.h>
  #include <sys/syscall.h>
  #include <sys/sysinfo.h>
  #include <linux/unistd.h>
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "worker.h"

// using namespace v8;
using namespace node;
using namespace std;

// Data struct
struct DiskPartition {
  char* device;
  char* mount_point;
  char* fstype;
  char* opts;
};

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class DiskPartitionsWorker : public Worker {
  public:
    DiskPartitionsWorker() {}
    ~DiskPartitionsWorker() {}

    bool prDisk;
    vector<DiskPartition*> results;

    void inline execute()
    {
      int num;
      int i;
      long len;
      uint64_t flags;
      char opts[400];
      struct statfs *fs = NULL;

      // get the number of mount points
      num = getfsstat(NULL, 0, MNT_NOWAIT);
      if(num == -1) {
        this->error = true;
        this->error_message = strerror(errno);
        return;
      }

      len = sizeof(*fs) * num;
      fs = (struct statfs *)malloc(len);
      if(fs == NULL) {
        this->error = true;
        this->error_message = (char *)"could not allocate memory";
        return;
      }

      num = getfsstat(fs, len, MNT_NOWAIT);
      if(num == -1) {
        this->error = true;
        this->error_message = strerror(errno);
        return;
      }

      for(i = 0; i < num; i++) {
        opts[0] = 0;
        flags = fs[i].f_flags;

        // see sys/mount.h
        if(flags & MNT_RDONLY)
          strlcat(opts, "ro", sizeof(opts));
        else
          strlcat(opts, "rw", sizeof(opts));

        if(flags & MNT_SYNCHRONOUS)
          strlcat(opts, ",sync", sizeof(opts));
        if(flags & MNT_NOEXEC)
          strlcat(opts, ",noexec", sizeof(opts));
        if(flags & MNT_NOSUID)
          strlcat(opts, ",nosuid", sizeof(opts));
        if(flags & MNT_UNION)
          strlcat(opts, ",union", sizeof(opts));
        if(flags & MNT_ASYNC)
          strlcat(opts, ",async", sizeof(opts));
        if(flags & MNT_EXPORTED)
          strlcat(opts, ",exported", sizeof(opts));
        if(flags & MNT_QUARANTINE)
          strlcat(opts, ",quarantine", sizeof(opts));
        if(flags & MNT_LOCAL)
          strlcat(opts, ",local", sizeof(opts));
        if(flags & MNT_QUOTA)
          strlcat(opts, ",quota", sizeof(opts));
        if(flags & MNT_ROOTFS)
          strlcat(opts, ",rootfs", sizeof(opts));
        if(flags & MNT_DOVOLFS)
          strlcat(opts, ",dovolfs", sizeof(opts));
        if(flags & MNT_DONTBROWSE)
          strlcat(opts, ",dontbrowse", sizeof(opts));
        if(flags & MNT_IGNORE_OWNERSHIP)
          strlcat(opts, ",ignore-ownership", sizeof(opts));
        if(flags & MNT_AUTOMOUNTED)
          strlcat(opts, ",automounted", sizeof(opts));
        if(flags & MNT_JOURNALED)
          strlcat(opts, ",journaled", sizeof(opts));
        if(flags & MNT_NOUSERXATTR)
          strlcat(opts, ",nouserxattr", sizeof(opts));
        if(flags & MNT_DEFWRITE)
          strlcat(opts, ",defwrite", sizeof(opts));
        if(flags & MNT_MULTILABEL)
          strlcat(opts, ",multilabel", sizeof(opts));
        if(flags & MNT_NOATIME)
          strlcat(opts, ",noatime", sizeof(opts));
        if(flags & MNT_UPDATE)
          strlcat(opts, ",update", sizeof(opts));
        if(flags & MNT_RELOAD)
          strlcat(opts, ",reload", sizeof(opts));
        if(flags & MNT_FORCE)
          strlcat(opts, ",force", sizeof(opts));
        if(flags & MNT_CMDFLAGS)
          strlcat(opts, ",cmdflags", sizeof(opts));

        // Create a new document
        DiskPartition *diskPartition = new DiskPartition();
        diskPartition->device = fs[i].f_mntfromname;
        diskPartition->mount_point = fs[i].f_mntonname;
        diskPartition->fstype = fs[i].f_fstypename;
        diskPartition->opts = opts;
        // Add to vector
        this->results.push_back(diskPartition);
      }

    }

    Handle<Value> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Array::New(this->results.size());
      vector<DiskPartition*>::const_iterator i;
      int index = 0;

      for(i = this->results.begin(); i != this->results.end(); i++) {
        // Reference the diskCounters
        DiskPartition *partition = *i;
        // DiskObject
        Local<Object> partitionObject = Object::New();
        partitionObject->Set(String::New("device"), String::New(partition->device));
        partitionObject->Set(String::New("mountpoint"), String::New(partition->mount_point));
        partitionObject->Set(String::New("fstype"), String::New(partition->fstype));
        partitionObject->Set(String::New("opts"), String::New(partition->opts));
        // Add to the result object
        resultsObject->Set(index++, partitionObject);
        // Clean up memory
        // free(partition->device);
        // free(partition->mount_point);
        // free(partition->fstype);
        // free(partition->opts);
        delete partition;
      }

      // Return final object
      return resultsObject;
    }
};
#elif defined __linux__
// Contains the information about the worker to be processes in the work queue
class DiskPartitionsWorker : public Worker {
  public:
    DiskPartitionsWorker() {}
    ~DiskPartitionsWorker() {}

    bool prDisk;
    vector<DiskPartition*> results;

    void inline execute()
    {
      FILE *file = NULL;
      struct mntent *entry;

      // MOUNTED constant comes from mntent.h and it's == '/etc/mtab'
      file = setmntent(MOUNTED, "r");
      if((file == 0) || (file == NULL)) {
        this->error = true;
        this->error_message = strerror(errno);
        return;
      }

      while((entry = getmntent(file))) {
        if(entry == NULL) {
          this->error = true;
          this->error_message = (char *)"getmntent() failed";
        }

        // Create a new document
        DiskPartition *diskPartition = new DiskPartition();
        diskPartition->device = entry->mnt_fsname;
        diskPartition->mount_point = entry->mnt_dir;
        diskPartition->fstype = entry->mnt_type;
        diskPartition->opts = entry->mnt_opts;
        // Add to vector
        this->results.push_back(diskPartition);
      }

      endmntent(file);
    }

    Handle<Value> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Array::New(this->results.size());
      vector<DiskPartition*>::const_iterator i;
      int index = 0;

      for(i = this->results.begin(); i != this->results.end(); i++) {
        // Reference the diskCounters
        DiskPartition *partition = *i;
        // DiskObject
        Local<Object> partitionObject = Object::New();
        partitionObject->Set(String::New("device"), String::New(partition->device));
        partitionObject->Set(String::New("mountpoint"), String::New(partition->mount_point));
        partitionObject->Set(String::New("fstype"), String::New(partition->fstype));
        partitionObject->Set(String::New("opts"), String::New(partition->opts));
        // Add to the result object
        resultsObject->Set(index++, partitionObject);
        // Clean up memory
        // free(partition->device);
        // free(partition->mount_point);
        // free(partition->fstype);
        // free(partition->opts);
        delete partition;
      }

      // Return final object
      return resultsObject;
    }
};
#else
// Contains the information about the worker to be processes in the work queue
class DiskPartitionsWorker : public Worker {
  public:
    DiskPartitionsWorker() {}
    ~DiskPartitionsWorker() {}

    void inline execute()
    {
    }

    Handle<Value> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      // Return final object
      return resultsObject;
    }
};
#endif

#endif  // DISK_PARTITIONS_WORKER_H_
