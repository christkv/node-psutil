var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib,
  fs = require('fs'),
  format = require('util').format;

const NAME = 0;
const PPID = 1;
const EXE = 2;
const IO_COUNTERS = 3;
const CPU_TIMES = 4;

var Process = function Process(pid, lib) {
  this._lib = lib;
  this._pid = pid;
  this._name = null;
  this._ppid = null;
  this._exe = null;
  this._cpu_times = null;
}

Process.prototype.toJSON = function() {
  return {
      pid: this._pid
    , name: this._name
    , ppid: this._ppid
    , exe: this._exe
    , cpu_times: this._cpu_times
  }
}

Process.prototype.name = function name(callback) {
  _processField(this, this._pid, '_name', NAME, callback);
}

Process.prototype.ppid = function ppid(callback) {
  _processField(this, this._pid, '_ppid', PPID, callback);
}

Process.prototype.exe = function exe(callback) {
  _processField(this, this._pid, '_exe', EXE, callback);
}

Process.prototype.cpu_times = function cpu_times(callback) {
  _processField(this, this._pid, '_cpu_times', CPU_TIMES, callback);
}

var _processField = function _processField(self, pid, field_name, param_name, callback) {
  if(self[field_name]) return callback(null, self[field_name]);

  if(process.platform == 'linux') {
    _processFieldLinux(self, pid, field_name, param_name, callback);
  } else {
    // Retrieve the process name
    self._lib.process_info(pid, param_name, {}, function(err, name) {
      if(err) return callback(err);
      // Save the name
      self[field_name] = name;
      callback(null, name);
    });
  }
}

var _processFieldLinux = function _processFieldLinux(self, pid, field_name, param_name, callback) {
  if(param_name == NAME) {
    _setNameLinux(self, pid, field_name, callback);
  } else if(param_name == PPID) {
    _setPPIDLinux(self, pid, field_name, callback);
  } else if(param_name == EXE) {
    _setExeLinux(self, pid, field_name, callback);
  } else if(param_name == CPU_TIMES) {
    _setCPUTimesLinux(self, pid, field_name, callback);
  }
}

/******************************************************************************
 * Linux functions where no native api is available
 *****************************************************************************/
var _setNameLinux = function _setNameLinux(self, pid, field_name, callback) {
  Process._readFile(format('/proc/%s/stat', self._pid), 'ascii', function(err, data) {
    if(err) return callback(new Error("process doest not exist or access denied"));
    var name = data.split(/ +/)[1].replace(/\(/, '').replace(/\)/, '');
    self[field_name] = name;
    callback(null, name);
  });
}

var _setExeLinux = function _setExeLinux(self, pid, field_name, callback) {
  Process._readLink(format('/proc/%s/exe', self._pid), function(err, data) {
    if(err) return callback(new Error("process doest not exist or access denied"));
    self[field_name] = data;
    callback(null, data);
  });
}

var _setPPIDLinux = function _setPPIDLinux(self, pid, field_name, callback) {
  Process._readFile(format('/proc/%s/status', self._pid), 'ascii', function(err, data) {
    if(err) return callback(new Error("process doest not exist or access denied"));
    var lines = data.split(/\n/);
    var ppid = 0;
    for(var i = 0; i < lines.length; i++) {
      if(lines[i].match(/^PPid:/)) {
        ppid = parseInt(lines[i].split(/\W+/)[1].trim(), 10);
        break;
      }
    }
    self[field_name] = ppid;
    callback(null, ppid);
  });
}

var _setCPUTimesLinux = function _setCPUTimesLinux(self, pid, field_name, callback) {
  self._lib.sys_conf(0, function(err, clock_ticks) {
    Process._readFile(format('/proc/%s/stat', self._pid), 'ascii', function(err, data) {
      if(err) return callback(new Error("process doest not exist or access denied"));
      data = data.substr(data.indexOf(')') + 1).trim();
      var values = data.split(/\W+/);
      var utime = parseFloat(values[11]) / clock_ticks;
      var stime = parseFloat(values[12]) / clock_ticks;
      callback(null, [utime, stime]);
    });
  });
}

/******************************************************************************
 * Utility helper, helps for cross os testing
 *****************************************************************************/
Process._readFile = function _readFile(path, encoding, callback) {
  fs.readFile(path, encoding, callback);
}

Process._readLink = function _readFile(path, callback) {
  fs.readlink(path, callback);
}

Process._readDirectory = function _readDirectory(path, callback) {
  fs.readdir(path, callback);
}




exports.Process = Process;