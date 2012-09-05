var PSUtil = require('../lib/psutil').PSUtil,
  fs = require('fs');

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
    test.equal('number', typeof result.free);
    test.equal('number', typeof result.available);
    test.equal('number', typeof result.used);
    test.equal('number', typeof result.percent);
    test.done();
  });
}

exports['Should correctly fetch virtual memory information on linux'] = function(test) {
  var psUtil = new PSUtil();
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = PSUtil._readFile;
  var lib_function = psUtil._lib.virtual_memory;

  // Setup Overrides
  process.platform = 'linux'
  PSUtil._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/meminfo.txt', encoding, callback);
  }
  psUtil._lib.virtual_memory = function(callback) {
    callback(null, { total: 1042206720,
        free: 96088064,
        buffer: 9764864,
        shared: 0,
        swap_total: 1071640576,
        swap_free: 960806912 });
  }

  psUtil.virtual_memory(function(err, result) {
    test.equal('number', typeof result.total);
    test.equal('number', typeof result.active);
    test.equal('number', typeof result.inactive);
    test.equal('number', typeof result.free);
    test.equal('number', typeof result.available);
    test.equal('number', typeof result.used);
    test.equal('number', typeof result.percent);
    // Reset platform changes and finish test
    process.platform = platform;
    PSUtil._readFile = readFileFunction;
    psUtil._lib.virtual_memory = lib_function;
    test.done();
  });
}