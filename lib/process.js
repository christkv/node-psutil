var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib,
  fs = require('fs');

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
  this._cpu_times = [];
  // Process library function
  this.library_function = process.platform === 'linux' ? function(){} : this._lib.process_info;
}

Process.prototype.name = function name(callback) {
  _processField(this, this._pid, '_name', NAME, this.library_function, callback);
}

Process.prototype.ppid = function ppid(callback) {
  _processField(this, this._pid, '_ppid', PPID, this.library_function, callback);
}

Process.prototype.exe = function exe(callback) {
  _processField(this, this._pid, '_exe', EXE, this.library_function, callback);
}

Process.prototype.cpu_times = function cpu_times(callback) {
  _processField(this, this._pid, '_cpu_times', CPU_TIMES, this.library_function, callback);
}

var _processField = function _processField(self, pid, field_name, param_name, field_method, callback) {
  var self = this;
  if(self[field_name]) return callback(null, self[field_name]);
  // Retrieve the process name
  field_method(pid, param_name, {}, function(err, name) {
    if(err) return callback(err);
    // Save the name
    self[field_name] = name;
    callback(null, name);
  });
}

// Process.prototype.ppid = function ppid(callback) {
//   var self = this;
//   if(self._name) return callback(null, self._name);
//   // Method to call depending on the platform
//   var library_function = null;
//   if(process.platform === 'linux') {
//     // library_function =
//   } else {
//     library_function = self._lib.process_info;
//   }
//   // Retrieve the process name
//   library_function(self.pid, NAME, {}, function(err, name) {
//     if(err) return callback(err);
//     // Save the name
//     self._name = name;
//     callback(null, name);
//   });
// }

exports.Process = Process;