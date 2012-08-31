#ifndef NETWORK_IO_COUNTERS_WORKER_H_
#define NETWORK_IO_COUNTERS_WORKER_H_

#include <v8.h>
#include <vector>

#ifdef __APPLE__
#include <sys/proc_info.h>
#include <netinet/tcp_fsm.h>
#include <arpa/inet.h>
#include <net/if_dl.h>
#include <mach/host_info.h>
#include <sys/sysctl.h>
#elif defined __linux__
  //#include <devstat.h>      /* get io counters */
#elif defined _WIN32 || defined _WIN64
#else
#error "unknown platform"
#endif

#include "worker.h"

// using namespace v8;
using namespace node;
using namespace std;

// Data struct
struct NetworkCounters {
  char* nic_name;
  int64_t bytes_sent;
  int64_t bytes_recv;
  int64_t packets_sent;
  int64_t packets_recv;
  int64_t errin;
  int64_t errout;
  int64_t dropin;
  int64_t dropout;
};

#ifdef __APPLE__
// Contains the information about the worker to be processes in the work queue
class NetworkIOCountersWorker : public Worker {
  public:
    NetworkIOCountersWorker() {}
    ~NetworkIOCountersWorker() {}

    bool prNic;
    vector<NetworkCounters*> results;

    void inline execute()
    {
      char *buf = NULL, *lim, *next;
      struct if_msghdr *ifm;
      int mib[6];
      size_t len;

      mib[0] = CTL_NET;          // networking subsystem
      mib[1] = PF_ROUTE;         // type of information
      mib[2] = 0;                // protocol (IPPROTO_xxx)
      mib[3] = 0;                // address family
      mib[4] = NET_RT_IFLIST2;   // operation
      mib[5] = 0;

      if(sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
        this->error = true;
        // Convert the error number into a message
        this->error_message = strerror(errno);
        return;
      }

      buf = (char *)malloc(len * sizeof(char));
      if(buf == NULL) {
        this->error = true;
        this->error_message = (char *)"could not allocate memory";
        return;
      }

      if(sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
        this->error = true;
        this->error_message = strerror(errno);
        return;
      }

      lim = buf + len;

      for(next = buf; next < lim; ) {
        ifm = (struct if_msghdr *)next;
        next += ifm->ifm_msglen;

        if(ifm->ifm_type == RTM_IFINFO2) {
          struct if_msghdr2 *if2m = (struct if_msghdr2 *)ifm;
          struct sockaddr_dl *sdl = (struct sockaddr_dl *)(if2m + 1);
          char *ifc_name = (char*)malloc(32 * sizeof(char));

          strncpy(ifc_name, sdl->sdl_data, sdl->sdl_nlen);
          ifc_name[sdl->sdl_nlen] = 0;

          // Create a network counter object and push on list of objects
          NetworkCounters *networkCounters = new NetworkCounters();
          networkCounters->nic_name = ifc_name;
          networkCounters->bytes_sent = if2m->ifm_data.ifi_obytes;
          networkCounters->bytes_recv = if2m->ifm_data.ifi_ibytes;
          networkCounters->packets_sent = if2m->ifm_data.ifi_opackets;
          networkCounters->packets_recv = if2m->ifm_data.ifi_ipackets;
          networkCounters->errin = if2m->ifm_data.ifi_ierrors;
          networkCounters->errout = if2m->ifm_data.ifi_oerrors;
          networkCounters->dropin = if2m->ifm_data.ifi_iqdrops;
          networkCounters->dropout = 0;
          // Add to vector
          this->results.push_back(networkCounters);
        } else {
          continue;
        }
      }

      free(buf);
    }

    Local<Object> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();

      // If not pr disk accumulate all the data
      if(!prNic) {
        vector<NetworkCounters*>::const_iterator i;

        // All accumulators
        int64_t bytes_sent, bytes_recv, packets_sent, packets_recv, errin, errout, dropin, dropout;

        for(i = this->results.begin(); i != this->results.end(); i++) {
          // Reference the diskCounters
          NetworkCounters *counters = *i;
          bytes_sent += counters->bytes_sent;
          bytes_recv += counters->bytes_recv;
          packets_sent += counters->packets_sent;
          packets_recv += counters->packets_recv;
          errin += counters->errin;
          errout += counters->errout;
          dropin += counters->dropin;
          dropout += counters->dropout;

          // Clean up memory
          free(counters->nic_name);
          delete counters;
        }

        // Set values
        resultsObject->Set(String::New("bytes_sent"), Number::New(bytes_sent));
        resultsObject->Set(String::New("bytes_recv"), Number::New(bytes_recv));
        resultsObject->Set(String::New("packets_sent"), Number::New(packets_sent));
        resultsObject->Set(String::New("packets_recv"), Number::New(packets_recv));
        resultsObject->Set(String::New("errin"), Number::New(errin));
        resultsObject->Set(String::New("errout"), Number::New(errout));
        resultsObject->Set(String::New("dropin"), Number::New(dropin));
        resultsObject->Set(String::New("dropout"), Number::New(dropout));
      } else {
        vector<NetworkCounters*>::const_iterator i;

        for(i = this->results.begin(); i != this->results.end(); i++) {
          // Reference the diskCounters
          NetworkCounters *counters = *i;
          // DiskObject
          Local<Object> nic = Object::New();
          nic->Set(String::New("bytes_sent"), Number::New(counters->bytes_sent));
          nic->Set(String::New("bytes_recv"), Number::New(counters->bytes_recv));
          nic->Set(String::New("packets_sent"), Number::New(counters->packets_sent));
          nic->Set(String::New("packets_recv"), Number::New(counters->packets_recv));
          nic->Set(String::New("errin"), Number::New(counters->errin));
          nic->Set(String::New("errout"), Number::New(counters->errout));
          nic->Set(String::New("dropin"), Number::New(counters->dropin));
          nic->Set(String::New("dropout"), Number::New(counters->dropout));

          // Add to the result object
          resultsObject->Set(String::New(counters->nic_name, strlen(counters->nic_name)), nic);
          // Clean up memory
          free(counters->nic_name);
          delete counters;
        }
      }
      // Return final object
      return resultsObject;
    }
};

#else
// Contains the information about the worker to be processes in the work queue
class NetworkIOCountersWorker : public Worker {
  public:
    NetworkIOCountersWorker() {}
    ~NetworkIOCountersWorker() {}

    bool prNic;
    vector<DiskCounters*> results;

    void inline execute()
    {
    }

    Local<Object> inline map()
    {
      // HandleScope scope;
      Local<Object> resultsObject = Object::New();
      // Return final object
      return resultsObject;
    }
};
#endif

#endif  // NETWORK_IO_COUNTERS_WORKER_H_
