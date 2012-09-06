var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch cpu usage percentages'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.cpu_percent(0.1, false, function(err, result) {
    test.equal(null, err);
    test.equal('number', typeof result);

    psUtil.cpu_percent(0.1, true, function(err, result) {
      test.equal(null, err);
      test.equal('number', typeof result[0]);
      test.done();
    });
  });
}

exports['Should correctly fetch cpu usage percentages on linux'] = function(test) {
  var psUtil = new PSUtil();
  // Save the existing setup
  var platform = process.platform;
  var lib_function = psUtil._lib.cpu_times;

  // Setup Overrides
  process.platform = 'linux'
  psUtil.cpu_times = function(perCpu, callback) {
    if(!perCpu) {
      callback(null, { user: 21371137, nice: 0, system: 10084998, idle: 177731694 });
    } else {
      callback(null, [
        { user: 6510936, nice: 0, system: 5011204, idle: 40775388 },
        { user: 3728987, nice: 0, system: 1011353, idle: 47556441 },
        { user: 7249526, nice: 0, system: 3078746, idle: 41968513 },
        { user: 3881692, nice: 0, system: 983696, idle: 47431390 } ]);
    }
  }

  psUtil.cpu_percent(0.1, false, function(err, result) {
    test.equal(null, err);
    test.equal(0, result);

    // Reset platform changes and finish test
    psUtil.cpu_percent(0.1, true, function(err, result) {
      test.equal(null, err);
      test.deepEqual([0, 0, 0, 0], result);

      process.platform = platform;
      psUtil.cpu_times = lib_function;
      test.done();
    });
  });
}