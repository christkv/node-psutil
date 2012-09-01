var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch swap memory information'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.swap_memory(function(err, result) {
    test.equal('number', typeof result.total);
    test.equal('number', typeof result.used);
    test.equal('number', typeof result.free);
    test.equal('number', typeof result.percent);
    test.equal('number', typeof result.sin);
    test.equal('number', typeof result.sout);
    test.done();
  });
}