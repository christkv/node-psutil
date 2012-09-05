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
    test.equal('number', typeof result.wired);
    test.equal('number', typeof result.free);
    test.equal('number', typeof result.available);
    test.equal('number', typeof result.used);
    test.equal('number', typeof result.percent);
    test.done();
  });
}

exports['Should correctly fetch virtual memory information on linux'] = function(test) {
  var psUtil = new PSUtil();
  var platform = process.platform;
  // Override platform
  process.platform = 'linux'
  // Override the read file function
  var readFileFunction = PSUtil._readFile;
  PSUtil._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/meminfo.txt', encoding, callback);
  }

  psUtil.virtual_memory(function(err, result) {
    console.log("-------------------------------------------------------")
    console.dir(err)
    console.dir(result)
    // Reset platform changes and finish test
    process.platform = platform;
    PSUtil._readFile = readFileFunction;
    test.done();
  });
}