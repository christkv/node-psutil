var PSUtil = require('../lib/psutil').PSUtil,
 Process = require('../lib/process').Process,
 fs = require('fs');

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch process list'] = function(test) {
  var psUtil = new PSUtil();
  // Get the process list
  psUtil.process_list(function(err, processes) {
    processes[20].name(function(err, name) {
      test.equal(null, err);
      test.ok(typeof name == 'string');

      processes[10].ppid(function(err, ppid) {
        test.equal(null, err);
        test.ok(typeof ppid == 'number');

        processes[20].exe(function(err, exe) {
          processes[21].cpu_times(function(err, times) {
            test.equal(null, err);
            test.ok(Array.isArray(times));
            test.done();
          });
        });
      });
    });
  });
}

exports['Should correctly fetch process name in linux'] = function(test) {
  var psUtil = new PSUtil();
  var _process = new Process(0, psUtil._lib);
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = Process._readFile;

  // Setup Overrides
  process.platform = 'linux'
  Process._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/process_stat.txt', encoding, callback);
  }

  _process.name(function(err, name) {
    test.equal(null, err);
    test.equal("gvfsd", name);

    // Reset platform changes and finish test
    process.platform = platform;
    Process._readFile = readFileFunction;
    test.done();
  });
}

exports['Should correctly fetch process ppid in linux'] = function(test) {
  var psUtil = new PSUtil();
  var _process = new Process(0, psUtil._lib);
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = Process._readFile;

  // Setup Overrides
  process.platform = 'linux'
  Process._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/process_status.txt', encoding, callback);
  }

  _process.ppid(function(err, ppid) {
    test.equal(null, err);
    test.equal(1, ppid);

    // Reset platform changes and finish test
    process.platform = platform;
    Process._readFile = readFileFunction;
    test.done();
  });
}

exports['Should correctly fetch process exe in linux'] = function(test) {
  var psUtil = new PSUtil();
  var _process = new Process(0, psUtil._lib);
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = Process._readLink;

  // Setup Overrides
  process.platform = 'linux'
  Process._readLink = function(path, callback) {
    callback(null, "/usr/bin/do");
  }

  _process.exe(function(err, ppid) {
    test.equal(null, err);
    test.equal("/usr/bin/do", ppid);

    // Reset platform changes and finish test
    process.platform = platform;
    Process._readLink = readFileFunction;
    test.done();
  });
}

exports['Should correctly fetch process cpu_times in linux'] = function(test) {
  var psUtil = new PSUtil();
  var _process = new Process(0, psUtil._lib);
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = Process._readFile;
  var lib_function = psUtil._lib.sys_conf;

  // Setup Overrides
  process.platform = 'linux'
  Process._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/process_stat.txt', encoding, callback);
  }
  psUtil._lib.sys_conf = function(code, callback) {
    callback(null, 100);
  }

  _process.cpu_times(function(err, cpu_times) {
    test.equal(null, err);
    test.ok(Array.isArray(cpu_times));
    test.ok(2, cpu_times.length);

    // Reset platform changes and finish test
    process.platform = platform;
    Process._readFile = readFileFunction;
    psUtil._lib.sys_conf = lib_function;
    test.done();
  });
}
