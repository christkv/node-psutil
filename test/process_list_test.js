var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch process list'] = function(test) {
  var psUtil = new PSUtil();
  // Get the process list
  psUtil.process_list(function(err, processes) {
    processes[20].name(function(err, name) {
      test.equal(null, err);
      test.ok(typeof name == 'string');

      processes[10].ppid(function(err, ppid) {
        test.equal(null, err);
        test.ok(typeof ppid == 'number');

        processes[20].exe(function(err, exe) {
          test.equal(null, err);
          test.ok(typeof exe == 'string');
          // console.log("========================================================= 4")
          // console.dir(err)
          // console.dir(exe)
          test.done();
        });
      });
    });
  });
}