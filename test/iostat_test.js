var PSUtil = require('../build/Release/psutil_osx').PSUtil;

console.dir();

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve iostats of the system'] = function(test) {
  var psUtil = new PSUtil();
  console.dir(psUtil)
  test.done();
}