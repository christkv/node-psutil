var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib,
  fs = require('fs');

var PSUtil = function PSUtil() {
  this._lib = new PSUtilLib();
}

PSUtil.prototype.disk_io_counters = function disk_io_counters(perDisk, callback) {
  if(process.platform === 'linux') {
    _disk_io_counters(perDisk, callback);
  } else {
	  if(typeof perDisk == 'function') {
	    this._lib.disk_io_counters(perDisk);
	  } else {
	    this._lib.disk_io_counters(perDisk, callback);
	  }
  }
}

var _disk_io_counters = function _disk_io_counters(perDisk, callback) {
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
          var values = lines[i].trim().split(/ +/);
          partitionNames.push(values[3]);
        }

        var lines = diskstats.trim().split(/\n/);
        for(var i = 0; i < lines.length; i++) {
          var values = lines[i].trim().split(/ +/);

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

exports.PSUtil = PSUtil;