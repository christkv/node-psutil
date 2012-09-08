#ifdef __APPLE__
  #include <sys/mount.h>
  #include <errno.h>
#elif defined __linux__
  #include <errno.h>
  #include <stdlib.h>
  #include <mntent.h>
#endif

#include "disk_partitions_worker.h"

DiskPartitionsWorker::DiskPartitionsWorker() {
}

DiskPartitionsWorker::~DiskPartitionsWorker() {
}

#ifdef __APPLE__
void DiskPartitionsWorker::execute()
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

v8::Handle<v8::Value> DiskPartitionsWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Array::New(this->results.size());
  vector<DiskPartition*>::const_iterator i;
  int index = 0;

  for(i = this->results.begin(); i != this->results.end(); i++) {
    // Reference the diskCounters
    DiskPartition *partition = *i;
    // DiskObject
    v8::Local<v8::Object> partitionObject = v8::Object::New();
    partitionObject->Set(v8::String::New("device"), v8::String::New(partition->device));
    partitionObject->Set(v8::String::New("mountpoint"), v8::String::New(partition->mount_point));
    partitionObject->Set(v8::String::New("fstype"), v8::String::New(partition->fstype));
    partitionObject->Set(v8::String::New("opts"), v8::String::New(partition->opts));
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
#elif defined __linux__
char* copyString(char* string) {
  char* newString = (char *)malloc(strlen(string) * sizeof(char));
  strcpy(newString, string);
  return newString;
}

void DiskPartitionsWorker::execute()
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
    diskPartition->device = copyString(entry->mnt_fsname);
    diskPartition->mount_point = copyString(entry->mnt_dir);
    diskPartition->fstype = copyString(entry->mnt_type);
    diskPartition->opts = copyString(entry->mnt_opts);
    // Add to vector
    this->results.push_back(diskPartition);
  }

  endmntent(file);
}

v8::Handle<v8::Value> DiskPartitionsWorker::map()
{
  // HandleScope scope;
  v8::Local<v8::Object> resultsObject = v8::Array::New(this->results.size());
  vector<DiskPartition*>::const_iterator i;
  int index = 0;

  for(i = this->results.begin(); i != this->results.end(); i++) {
    // Reference the diskCounters
    DiskPartition *partition = *i;

    // DiskObject
    v8::Local<v8::Object> partitionObject = v8::Object::New();
    partitionObject->Set(v8::String::New("device"), v8::String::New(partition->device));
    partitionObject->Set(v8::String::New("mountpoint"), v8::String::New(partition->mount_point));
    partitionObject->Set(v8::String::New("fstype"), v8::String::New(partition->fstype));
    partitionObject->Set(v8::String::New("opts"), v8::String::New(partition->opts));
    // Add to the result object
    resultsObject->Set(index++, partitionObject);
    // Clean up memory
    free(partition->device);
    free(partition->mount_point);
    free(partition->fstype);
    free(partition->opts);
    delete partition;
  }

  // Return final object
  return resultsObject;
}

#else
  void DiskPartitionsWorker::execute() {}
  v8::Handle<v8::Value> DiskPartitionsWorker::map() { return v8::Undefined(); }
#endif
