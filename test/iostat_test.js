var PSUtil = require('../lib/psutil').PSUtil;

console.dir();

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve iostats of the system'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.iostat(function(err, result) {
    console.log("========================================= results 1")
    console.dir(err)
    console.dir(result)

    psUtil.iostat(true, function(err, result) {
      console.log("========================================= results 2")
      console.dir(err)
      console.dir(result)
      test.done();
    })
  })

  // test.done();
}