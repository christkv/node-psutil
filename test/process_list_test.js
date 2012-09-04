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
    console.log("=========================================================")
    console.dir(err)
    console.dir(processes)
    // test.equal(null, err);
    // test.ok(Array.isArray(result));
    test.done();
  });
}