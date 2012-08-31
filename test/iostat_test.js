var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve io counters of the system'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.disk_io_counters(function(err, result) {
    test.equal('number', typeof result.read_count);
    test.equal('number', typeof result.write_count);
    test.equal('number', typeof result.read_bytes);
    test.equal('number', typeof result.write_bytes);
    test.equal('number', typeof result.read_time);
    test.equal('number', typeof result.write_time);

    psUtil.disk_io_counters(true, function(err, results) {
      var result = results[Object.keys(results)[0]];
      test.equal('number', typeof result.read_count);
      test.equal('number', typeof result.write_count);
      test.equal('number', typeof result.read_bytes);
      test.equal('number', typeof result.write_bytes);
      test.equal('number', typeof result.read_time);
      test.equal('number', typeof result.write_time);
      test.done();
    });
  });
}