var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve network counters of the system'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.network_io_counters(function(err, result) {
    test.equal('number', typeof result.bytes_sent);
    test.equal('number', typeof result.bytes_recv);
    test.equal('number', typeof result.packets_sent);
    test.equal('number', typeof result.packets_recv);
    test.equal('number', typeof result.errin);
    test.equal('number', typeof result.errout);
    test.equal('number', typeof result.dropin);
    test.equal('number', typeof result.dropout);

    psUtil.network_io_counters(true, function(err, results) {
      var result = results[Object.keys(results)[0]];
      test.equal('number', typeof result.bytes_sent);
      test.equal('number', typeof result.bytes_recv);
      test.equal('number', typeof result.packets_sent);
      test.equal('number', typeof result.packets_recv);
      test.equal('number', typeof result.errin);
      test.equal('number', typeof result.errout);
      test.equal('number', typeof result.dropin);
      test.equal('number', typeof result.dropout);
      test.done();
    });
  });
}