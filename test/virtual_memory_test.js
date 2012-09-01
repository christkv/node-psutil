var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch virtual memory information'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.virtual_memory(function(err, result) {
    test.equal('number', typeof result.total);
    test.equal('number', typeof result.active);
    test.equal('number', typeof result.inactive);
    test.equal('number', typeof result.wired);
    test.equal('number', typeof result.free);
    test.equal('number', typeof result.available);
    test.equal('number', typeof result.used);
    test.equal('number', typeof result.percent);
    test.done();
  });
}