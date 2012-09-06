var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib,
// var PSUtilLib = require('../build/Release/obj.target/psutil_lib_osx').PSUtilLib,
  Process = require('./process').Process,
  fs = require('fs');

var PSUtil = function PSUtil() {
  this._lib = new PSUtilLib();
}

PSUtil.prototype.disk_io_counters = function disk_io_counters(perDisk, callback) {
  if(process.platform === 'linux') {
    _disk_io_counters_linux(perDisk, callback);
  } else {
	  if(typeof perDisk == 'function') {
	    this._lib.disk_io_counters(perDisk);
	  } else {
	    this._lib.disk_io_counters(perDisk, callback);
	  }
  }
}

PSUtil.prototype.network_io_counters = function network_io_counters(perNic, callback) {
  if(process.platform === 'linux') {
    _network_io_counters_linux(perNic, callback);
  } else {
    if(typeof perNic == 'function') {
      this._lib.network_io_counters(perNic);
    } else {
      this._lib.network_io_counters(perNic, callback);
    }
  }
}

PSUtil.prototype.virtual_memory = function virtual_memory(callback) {
  if(process.platform === 'linux') {
    _virtual_memory_linux(this, callback);
  } else {
    this._lib.virtual_memory(callback);
  }
}

PSUtil.prototype.swap_memory = function swap_memory(callback) {
  if(process.platform === 'linux') {
    _swap_memory_linux(this, callback);
  } else {
    this._lib.swap_memory(callback);
  }
}

PSUtil.prototype.cpu_percent = function cpu_percent(interval, perCpu, callback) {
  var self = this;

  if(typeof interval == 'function') {
    callback = interval;
    interval = 0.1;
    perCpu = false;
  } else if(typeof perCpu == 'function') {
    callback = perCpu;
    perCpu = false;
  }

  // Retrieve the cpu times
  self.cpu_times(perCpu, function(err, cpu_times1) {
    if(err) callback(err);
    if(!Array.isArray(cpu_times1)) cpu_times1 = [cpu_times1];

    // Wait for the programmed interval
    setTimeout(function() {
      self.cpu_times(perCpu, function(err, cpu_times2) {
        if(err) callback(err);
        if(!Array.isArray(cpu_times2)) cpu_times2 = [cpu_times2];
        // Final results
        var results = [];
        // Process all the times
        for(var i = 0; i < cpu_times2.length; i++) {
          var t1 = cpu_times1[i];
          var t2 = cpu_times2[i];
          // Sum off all the data
          var t1_all = 0;
          var t2_all = 0;
          // Sum up all the values
          for(var k in t1) t1_all += t1[k];
          for(var k in t2) t2_all += t2[k];
          // Calculate busy values
          var t1_busy = t1_all - t1.idle;
          var t2_busy = t2_all - t2.idle;

          if(t2_busy <= t1_busy) {
            results.push(0);
            continue;
          }

          var busy_delta = t2_busy - t1_busy;
          var all_delta = t2_all - t1_all;
          var busy_perc = roundNumber((busy_delta / all_delta) * 100, 2);
          results.push(busy_perc);
        }

        // Return the values
        callback(null, !perCpu ? results[0] : results);
      });
    }, (interval * 1000));
  });
}

PSUtil.prototype.cpu_times = function cpu_times(perCpu, callback) {
  var self = this;

  if(typeof interval == 'function') {
    callback = interval;
    interval = 0.1;
    perCpu = false;
  } else if(typeof perCpu == 'function') {
    callback = perCpu;
    perCpu = false;
  }

  // Method to call depending on the platform
  var library_function = null;
  if(process.platform === 'linux') {
    _cpu_times_linux(self, perCpu, callback);
  } else {
    self._lib.cpu_times(perCpu, callback);
  }
}

PSUtil.prototype.pid_list = function pid_list(callback) {
  var self = this;
  // Method to call depending on the platform
  var library_function = null;
  if(process.platform === 'linux') {
    library_function = _pid_list_linux;
  } else {
    library_function = self._lib.pid_list;
  }

  // Retrieve the cpu times
  library_function(function(err, pids) {
    // Cache the pids
    self._pids = pids;
    // Perform the callback
    callback(null, pids);
  });
}

PSUtil.prototype.pid_exists = function pid_exists(pid, callback) {
  this._lib.pid_exists(pid, callback);
}

PSUtil.prototype.process_list = function process_list(callback) {
  var self = this;
  // Fetch the pid list
  this.pid_list(function(err, pids) {
    if(err) return callback(err, null);
    // Create a list of pids
    var processes = new Array(pids.length);
    for(var i = 0; i < pids.length; i++) {
      processes[i] = new Process(pids[i], self._lib);
    }
    // Return list of processes
    callback(null, processes);
  });
}

PSUtil.prototype.disk_partitions = function disk_partitions(all, callback) {
  var self = this;
  if(typeof all == 'function') {
    callback = all;
    all = false;
  }

  // For darwin
  if(process.platform == 'darwin') {
    // Retrieve the disk partitions
    self._lib.disk_partitions(function(err, partitions) {
      if(err) return callback(err);
      if(!all) {
        var finalResults = [];
        var totalNumber = partitions.length;
        // Validate all entries and ensure only disk based ones are shown
        for(var i = 0; i < partitions.length; i++) {
          // Execute exists on each partition
          var existsFunction = function(_partition) {
            return function() {
              fs.exists(_partition.device, function(exists) {
                totalNumber = totalNumber - 1;
                if(exists) finalResults.push(_partition);
                if(totalNumber == 0) callback(null, finalResults);
              });
            }
          };
          // Exists function
          existsFunction(partitions[i])();
        }
      } else {
        callback(err, partitions);
      }
    });
  } else if(process.platform == 'linux') {
    _disk_partitions_linux(self, all, callback);
  }
}

PSUtil.prototype.disk_usage = function disk_usage(path, callback) {
  this._lib.disk_usage(path, callback);
}

/******************************************************************************
 * Utility helper, helps for cross os testing
 *****************************************************************************/
PSUtil._readFile = function _readFile(path, encoding, callback) {
  fs.readFile(path, encoding, callback);
}

PSUtil._readDirectory = function _readDirectory(path, callback) {
  fs.readdir(path, callback);
}

/******************************************************************************
 * Linux functions where no native api is available
 *****************************************************************************/
var _disk_partitions_linux = function _disk_partitions_linux(self, all, callback) {
  // Read the file directory
  PSUtil._readFile('/proc/filesystems', 'ascii', function(err, filesystems) {
    var physicalDevices = [];
    // Go over all the lines
    var lines = filesystems.split(/\n/);
    for(var i = 0; i < lines.length; i++) {
      if(lines[i].match(/^nodev/) == null) physicalDevices.push(lines[i].trim());
    }

    self._lib.disk_partitions(function(err, partitions) {
      var returnList = [];

      for(var j = 0; j < partitions.length; j++) {
        var partition = partitions[j];
        if(!all && physicalDevices.indexOf(partition.fstype) == -1) continue;
        returnList.push(partition);
      }
      // Return the list of partitions
      callback(null, returnList);
    });
  });
}

var _pid_list_linux = function _pid_list_linux(callback) {
  // Read the file directory
  PSUtil._readDirectory('/proc', function(err, items) {
    if(err) return callback(err);
    var pids = [];
    // Filter out all the pids
    for(var i = 0; i < items.length; i++) {
      var parsedNumber = parseInt(items[i], 10);
      if(!isNaN(parsedNumber)) pids.push(parsedNumber);
    }
    // Return values
    callback(null, pids);
  });
}

var _cpu_times_linux = function _cpu_times_linux(self, perCpu, callback) {
  self._lib.sys_conf(0, function(err, clock_ticks) {
    PSUtil._readFile('/proc/stat', 'ascii', function(err, stat) {
      if(err) return callback(err);
      // Split all the lines
      var lines = stat.split(/\n/);

      if(!perCpu) {
        var values = lines[0].split(/\W+/).slice(1);
        // Process the times to get the right adjusted value
        values = values.map(function(value) { return parseFloat(value, 10) / clock_ticks });
        // Return the values
        callback(null, {
          user:values[0], nice:values[1], system:values[2], idle:values[3],
          iowait:values[4], irq:values[5],softirq:values[6]
        });
      } else {
        var cpuTimes = [];

        for(var i = 1; i < lines.length; i++) {
          if(lines[i].match(/^cpu/) == null) break;
          var values = lines[0].split(/\W+/).slice(1);
          // Process the times to get the right adjusted value
          values = values.map(function(value) { return parseFloat(value, 10) / clock_ticks });
          cpuTimes.push({
              user:values[0], nice:values[1], system:values[2], idle:values[3],
              iowait:values[4], irq:values[5],softirq:values[6]
            });
        }
        // Return the data
        callback(null, cpuTimes);
      }
    });
  });
}

var _swap_memory_linux = function _swap_memory_linux(self, callback) {
  PSUtil._readFile('/proc/vmstat', 'ascii', function(err, meminfo) {
    self._lib.virtual_memory(function(_err, _data) {
      if(_err) return callback(_err);
      // Return the data
      var total = _data.swap_total;
      var free = _data.swap_free;
      var used = total - free;
      var percent = roundNumber((used/total) * 100.0, 1);
      var sin = 0, sout = 0;

      // Split up the files
      var lines = meminfo.split(/\n/);
      for(var i = 0; i < lines.length; i++) {
        var line = lines[i];
        if(line.match(/^pswpin/)) {
          sin = parseInt(line.split(/\W+/)[1] * 4 * 1024, 10);
        } else if(line.match(/^pswpout/)) {
          sout = parseInt(line.split(/\W+/)[1] * 4 * 1024, 10);
        }
      }
      // Perform the result callback
      callback(null, {total: total, used: used,
            free: free, percent:percent,
            sin: sin, sout: sout
          });
    });
  });
}

var _virtual_memory_linux = function _virtual_memory_linux(self, callback) {
  PSUtil._readFile('/proc/meminfo', 'ascii', function(err, meminfo) {
    self._lib.virtual_memory(function(_err, _data) {
      if(_err) return callback(_err);
      // Return the data
      var total = _data.total;
      var free = _data.free;
      var buffers = _data.buffer;
      var shared = _data.shared;
      var cached = 0, active = 0, inactive = 0;

      // Split up the files
      var lines = meminfo.split(/\n/);
      for(var i = 0; i < lines.length; i++) {
        var line = lines[i];
        if(line.match(/^Cached:/)) {
          cached = parseInt(line.split(/\W+/)[1] * 1024, 10);
        } else if(line.match(/^Active:/)) {
          active = parseInt(line.split(/\W+/)[1] * 1024, 10);
        } else if(line.match(/^Inactive:/)) {
          inactive = parseInt(line.split(/\W+/)[1] * 1024, 10);
        }
      }

      // Calculate missing files
      var available = free + buffers + cached;
      var used = total - free;
      var percent = roundNumber(((total - available)/total) * 100.0, 1);
      // Perform the result callback
      callback(null, {total: total, available: available,
            percent:percent, used: used,
            free: free, active:active, inactive:inactive,
            buffers:buffers, cached:cached
          });
    });
  });
}

var _network_io_counters_linux = function _network_io_counters_linux(perNic, callback) {
  if(typeof perNic == 'function') {
    callback = perNic;
    perNic = false;
  }

  // Read the settings from the operation system
  fs.readFile('/proc/net/dev', 'ascii', function(err, counters) {
    if(err) return callback(err);
    // Set up result
    var results = perNic ? {} : { bytes_recv:0, packets_recv:0,
        errin:0, dropin:0, bytes_sent:0, packets_sent:0,
        errout:0, dropout:0 };

    try {
      // Get all the lines
      var lines = counters.trim().split(/\n/).slice(2);
      // Parse all the data
      for(var i = 0; i < lines.length; i++) {
        var line = lines[i];
        var index = line.indexOf(":");
        if(index == -1) return callback(new Error("failed to parse data"), null);
        var values = line.substr(index + 1).trim().split(/\W+/);
        var nicName = line.substring(0, index).trim();

        // Unpack values
        var bytes_recv = parseInt(values[0], 10);
        var packets_recv = parseInt(values[1], 10);
        var errin = parseInt(values[2], 10);
        var dropin = parseInt(values[2], 10);
        var bytes_sent = parseInt(values[8], 10);
        var packets_sent = parseInt(values[9], 10);
        var errout = parseInt(values[10], 10);
        var dropout = parseInt(values[11], 10);

        // Sum up or do pr nic
        if(perNic) {
          results[nicName] = { bytes_recv:bytes_recv, packets_recv:packets_recv,
            errin:errin, dropin:dropin, bytes_sent:bytes_sent, packets_sent:packets_sent,
            errout:errout, dropout:dropout };
        } else {
          results.bytes_recv += bytes_recv;
          results.packets_recv += packets_recv;
          results.errin += errin;
          results.dropin += dropin;
          results.bytes_sent += bytes_sent;
          results.packets_sent += packets_sent;
          results.errout += errout;
          results.dropout += dropout;
        }
      }
    } catch(err) {
      if(err) return callback(err);
    }

    callback(null, results);
  });
}

var _disk_io_counters_linux = function _disk_io_counters_linux(perDisk, callback) {
  if(typeof perDisk == 'function') {
    callback = perDisk;
    perDisk = false;
  }

  // Read the settings from the operation system
  fs.readFile('/proc/partitions', 'ascii', function(err, partitions) {
    if(err) return callback(err);

    fs.readFile('/proc/diskstats', 'ascii', function(err, diskstats) {
      if(err) return callback(err);
      var partitionNames = [];
      var results = !perDisk ? {read_count: 0, write_count: 0, read_bytes: 0, write_bytes: 0, read_time: 0, write_time: 0} : {};

      try {
        var lines = partitions.trim().split(/\n/).slice(2);
        for(var i = 0; i < lines.length; i++) {
          var values = lines[i].trim().split(/\W+/);
          partitionNames.push(values[3]);
        }

        var lines = diskstats.trim().split(/\n/);
        for(var i = 0; i < lines.length; i++) {
          var values = lines[i].trim().split(/\W+/);

          if(!perDisk && partitionNames.indexOf(values[2]) != -1) {
            results.read_count = results.read_count + parseInt(values[3], 10);
            results.write_count = results.write_count + parseInt(values[7], 10);
            results.read_bytes = results.read_bytes + parseInt(values[5], 10);
            results.write_bytes = results.write_bytes + parseInt(values[9], 10);
            results.read_time = results.read_time + parseInt(values[6], 10);
            results.write_time = results.write_time + parseInt(values[10], 10);
          } else if(perDisk && partitionNames.indexOf(values[2]) != -1) {
            results[values[2]] = {
              read_count: parseInt(values[3], 10),
              write_count: parseInt(values[7], 10),
              read_bytes: parseInt(values[5], 10),
              write_bytes: parseInt(values[9], 10),
              read_time: parseInt(values[6], 10),
              write_time: parseInt(values[10], 10)
            }
          }
        }
      } catch(err) {
        if(err) return callback(err);
      }

      callback(null, results);
    });
  });
}

/******************************************************************************
 * Utility functions
 *****************************************************************************/
var roundNumber = function roundNumber(num, dec) {
  return Math.round(num*Math.pow(10,dec))/Math.pow(10,dec);
}

exports.PSUtil = PSUtil;