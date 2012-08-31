var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib,
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

/******************************************************************************
 * Linux functions where no native api is available
 *****************************************************************************/
var _network_io_counters_linux = function _network_io_counters_linux(perNic, callback) {
  if(typeof perNic == 'function') {
    callback = perNic;
    perNic = false;
  }

  // Read the settings from the operation system
  fs.readFile('/proc/net/dev', 'ascii', function(err, counters) {
    if(err) return callback(err);

        // bytes_recv = int(fields[0])
        // packets_recv = int(fields[1])
        // errin = int(fields[2])
        // dropin = int(fields[2])
        // bytes_sent = int(fields[8])
        // packets_sent = int(fields[9])
        // errout = int(fields[10])
        // dropout = int(fields[11])
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
        var values = line.substr(index + 1).trim().split(/ +/);
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