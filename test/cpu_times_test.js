var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch cpu times'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.cpu_times(false, function(err, result) {
    test.equal(null, err);
    test.equal('number', typeof result.user);

    psUtil.cpu_times(true, function(err, result) {
      test.equal(null, err);
      test.equal('number', typeof result[0].user);
      test.done();
    });
  });
}