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
  this._cpu_times = [];
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
  }
}

/******************************************************************************
 * Linux functions where no native api is available
 *****************************************************************************/
var _setNameLinux = function _setNameLinux(self, pid, field_name, callback) {
  Process._readFile(format('/proc/%s/stat', self.pid), 'ascii', function(err, data) {
    var name = data.split(/ +/)[1].replace(/\(/, '').replace(/\)/, '');
    callback(null, name);
  });
}

/******************************************************************************
 * Utility helper, helps for cross os testing
 *****************************************************************************/
Process._readFile = function _readFile(path, encoding, callback) {
  fs.readFile(path, encoding, callback);
}

Process._readDirectory = function _readDirectory(path, callback) {
  fs.readdir(path, callback);
}




exports.Process = Process;