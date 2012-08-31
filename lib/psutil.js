var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib;

var PSUtil = function PSUtil() {
  this._lib = new PSUtilLib();
}

PSUtil.prototype.iostat = function iostat(perDisk, callback) {
  if(typeof perDisk == 'function') {
    this._lib.iostat(perDisk);
  } else {
    this._lib.iostat(perDisk, callback);
  }
}

exports.PSUtil = PSUtil;