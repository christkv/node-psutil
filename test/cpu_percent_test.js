var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch virtual memory information'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.cpu_percent(0.1, false, function(err, result) {
    test.equal(null, err);
    test.equal('number', typeof result);

    psUtil.cpu_percent(0.1, true, function(err, result) {
      test.equal(null, err);
      test.equal('number', typeof result[0]);
      test.done();
    });
  });
}