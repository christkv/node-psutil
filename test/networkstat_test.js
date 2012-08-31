var PSUtil = require('../lib/psutil').PSUtil;

console.dir();

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve network counters of the system'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.network_io_counters(function(err, result) {
    console.log("========================================= results 1")
    console.dir(err)
    console.dir(result)

    psUtil.network_io_counters(true, function(err, result) {
      console.log("========================================= results 2")
      console.dir(err)
      console.dir(result)
      test.done();
    })
  })

  // test.done();
}