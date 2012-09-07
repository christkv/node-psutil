#ifndef DISK_PARTITIONS_WORKER_H_
#define DISK_PARTITIONS_WORKER_H_

#include <v8.h>
#include <vector>
#include "worker.h"

using namespace std;

// Data struct
struct DiskPartition {
  char* device;
  char* mount_point;
  char* fstype;
  char* opts;
};

// Contains the information about the worker to be processes in the work queue
class DiskPartitionsWorker : public Worker {
  public:
    DiskPartitionsWorker();
    ~DiskPartitionsWorker();

    bool prDisk;
    vector<DiskPartition*> results;

    void execute();
    v8::Handle<v8::Value> map();
    char* copyString(char* string);
};

// #elif defined __linux__
// // Contains the information about the worker to be processes in the work queue
// class DiskPartitionsWorker : public Worker {
//   public:
//     DiskPartitionsWorker() {}
//     ~DiskPartitionsWorker() {}

//     bool prDisk;
//     vector<DiskPartition*> results;

//     void inline execute()
//     {
//       FILE *file = NULL;
//       struct mntent *entry;

//       // MOUNTED constant comes from mntent.h and it's == '/etc/mtab'
//       file = setmntent(MOUNTED, "r");
//       if((file == 0) || (file == NULL)) {
//         this->error = true;
//         this->error_message = strerror(errno);
//         return;
//       }

//       while((entry = getmntent(file))) {
//         if(entry == NULL) {
//           this->error = true;
//           this->error_message = (char *)"getmntent() failed";
//         }

//         // Create a new document
//         DiskPartition *diskPartition = new DiskPartition();
//         diskPartition->device = copyString(entry->mnt_fsname);
//         diskPartition->mount_point = copyString(entry->mnt_dir);
//         diskPartition->fstype = copyString(entry->mnt_type);
//         diskPartition->opts = copyString(entry->mnt_opts);
//         // Add to vector
//         this->results.push_back(diskPartition);
//       }

//       endmntent(file);
//     }

//     Handle<Value> inline map()
//     {
//       // HandleScope scope;
//       Local<Object> resultsObject = Array::New(this->results.size());
//       vector<DiskPartition*>::const_iterator i;
//       int index = 0;

//       for(i = this->results.begin(); i != this->results.end(); i++) {
//         // Reference the diskCounters
//         DiskPartition *partition = *i;

//         // DiskObject
//         Local<Object> partitionObject = Object::New();
//         partitionObject->Set(String::New("device"), String::New(partition->device));
//         partitionObject->Set(String::New("mountpoint"), String::New(partition->mount_point));
//         partitionObject->Set(String::New("fstype"), String::New(partition->fstype));
//         partitionObject->Set(String::New("opts"), String::New(partition->opts));
//         // Add to the result object
//         resultsObject->Set(index++, partitionObject);
//         // Clean up memory
//         free(partition->device);
//         free(partition->mount_point);
//         free(partition->fstype);
//         free(partition->opts);
//         delete partition;
//       }

//       // Return final object
//       return resultsObject;
//     }

//     char* copyString(char* string) {
//       char* newString = (char *)malloc(strlen(string) * sizeof(char));
//       strcpy(newString, string);
//       return newString;
//     }
// };
// #else
// // Contains the information about the worker to be processes in the work queue
// class DiskPartitionsWorker : public Worker {
//   public:
//     DiskPartitionsWorker() {}
//     ~DiskPartitionsWorker() {}

//     void inline execute()
//     {
//     }

//     Handle<Value> inline map()
//     {
//       // HandleScope scope;
//       Local<Object> resultsObject = Object::New();
//       // Return final object
//       return resultsObject;
//     }
// };

#endif  // DISK_PARTITIONS_WORKER_H_
