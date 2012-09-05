var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly check the existance of a pid'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.pid_list(function(err, result) {
    test.equal(null, err);
    test.ok(Array.isArray(result));

    // Validate the existance of the pif
    psUtil.pid_exists(result[0], function(err, result) {
      test.equal(null, err);
      test.equal(true, result);

      // Validate the existance of the pif
      psUtil.pid_exists(-1, function(err, result) {
        test.equal(null, err);
        test.equal(false, result);
        test.done();
      });
    });
  });
}