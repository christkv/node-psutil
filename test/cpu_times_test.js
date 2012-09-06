var PSUtil = require('../lib/psutil').PSUtil,
  fs = require('fs');

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch cpu times'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.cpu_times(false, function(err, result) {
    test.equal(null, err);
    test.equal('number', typeof result.user);

    psUtil.cpu_times(true, function(err, result) {
      test.equal(null, err);
      test.equal('number', typeof result[0].user);
      test.done();
    });
  });
}

exports['Should correctly fetch cpu times on linux'] = function(test) {
  var psUtil = new PSUtil();
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = PSUtil._readFile;
  var lib_function = psUtil._lib.sys_conf;

  // Setup Overrides
  process.platform = 'linux'
  PSUtil._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/stat.txt', encoding, callback);
  }
  psUtil._lib.sys_conf = function(code, callback) {
    callback(null, 100);
  }

  psUtil.cpu_times(false, function(err, result) {
    test.equal(null, err);
    test.equal('number', typeof result.user);
    test.equal('number', typeof result.nice);
    test.equal('number', typeof result.system);
    test.equal('number', typeof result.idle);
    test.equal('number', typeof result.iowait);
    test.equal('number', typeof result.irq);
    test.equal('number', typeof result.softirq);

    // Reset platform changes and finish test
    psUtil.cpu_times(true, function(err, result) {
      result = result[0];
      test.equal(null, err);
      test.equal('number', typeof result.user);
      test.equal('number', typeof result.nice);
      test.equal('number', typeof result.system);
      test.equal('number', typeof result.idle);
      test.equal('number', typeof result.iowait);
      test.equal('number', typeof result.irq);
      test.equal('number', typeof result.softirq);

      process.platform = platform;
      PSUtil._readFile = readFileFunction;
      psUtil._lib.sys_conf = lib_function;
      test.done();
    });
  });
}