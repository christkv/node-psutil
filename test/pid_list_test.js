var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch pid list'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.get_pid_list(function(err, result) {
    test.equal(null, err);
    test.ok(Array.isArray(result));
    test.done();
  });
}