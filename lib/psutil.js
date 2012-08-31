var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib;

var PSUtil = function PSUtil() {
  this._lib = new PSUtilLib();
}

PSUtil.prototype.disk_io_counters = function disk_io_counters(perDisk, callback) {
  if(typeof perDisk == 'function') {
    this._lib.disk_io_counters(perDisk);
  } else {
    this._lib.disk_io_counters(perDisk, callback);
  }
}

exports.PSUtil = PSUtil;