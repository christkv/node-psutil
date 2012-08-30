var PSUtilLib = require('../build/Release/psutil_lib_osx').PSUtilLib;

var PSUtil = function PSUtil() {
  this._lib = new PSUtilLib();
}

PSUtil.prototype.iostat = function iostat(callback) {
  return this._lib.iostat(callback);
}

exports.PSUtil = PSUtil;