var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve disk usage'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.disk_usage('/', function(err, disk_usage) {
    test.equal(null, err);
    test.ok(typeof disk_usage.total == 'number');
    test.ok(typeof disk_usage.used == 'number');
    test.ok(typeof disk_usage.free == 'number');
    test.ok(typeof disk_usage.percent == 'number');
    test.done();
  });
}