var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve io counters of the system'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.disk_partitions(function(err, partitions) {
    test.equal(null, err)
    test.ok(Array.isArray(partitions));
    test.done();
  });
}