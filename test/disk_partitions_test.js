var PSUtil = require('../lib/psutil').PSUtil,
  fs = require('fs');

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly retrieve io counters of the system'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.disk_partitions(function(err, partitions) {
    test.equal(null, err)
    test.ok(Array.isArray(partitions));
    test.done();
  });
}

exports['Should correctly retrieve io counters of the system on linux'] = function(test) {
  var psUtil = new PSUtil();
  // Save the existing setup
  var platform = process.platform;
  var readFileFunction = PSUtil._readFile;
  var lib_function = psUtil._lib.disk_partitions;

  // Setup Overrides
  process.platform = 'linux'
  PSUtil._readFile = function(path, encoding, callback) {
    fs.readFile('./test/linux/filesystems.txt', encoding, callback);
  }
  psUtil._lib.disk_partitions = function(callback) {
    callback(null, disk_partitions_data);
  }

  psUtil.disk_partitions(function(err, result) {
    console.log("-------------------------------------------------------")
    console.dir(err)
    console.dir(result)
    // test.equal('number', typeof result.total);
    // test.equal('number', typeof result.active);
    // test.equal('number', typeof result.inactive);
    // test.equal('number', typeof result.free);
    // test.equal('number', typeof result.available);
    // test.equal('number', typeof result.used);
    // test.equal('number', typeof result.percent);
    // Reset platform changes and finish test
    process.platform = platform;
    PSUtil._readFile = readFileFunction;
    psUtil._lib.disk_partitions = lib_function;
    test.done();
  });
}

var disk_partitions_data = [ { device: '/dev/sr0',
    mountpoint: 'media/Parallels Tools',
    fstype: 'dia/Parallels Tools',
    opts: 'arallels Tools' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 'edia/Parallels Tools',
    opts: 'Parallels Tools' },
  { device: '/dev/sr0',
    mountpoint: 'r0',
    fstype: 'edia/Parallels Tools',
    opts: 'arallels Tools' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 's',
    opts: '9660' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 'ls Tools',
    opts: '' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 'Tools',
    opts: 'o9660' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 'media/Parallels Tools',
    opts: 'allels Tools' },
  { device: '/dev/sr0',
    mountpoint: '0',
    fstype: 'Parallels Tools',
    opts: 'ls Tools' },
  { device: '/dev/sr0',
    mountpoint: 'r0',
    fstype: 'edia/Parallels Tools',
    opts: 'arallels Tools' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: '/Parallels Tools',
    opts: 'lels Tools' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 'a/Parallels Tools',
    opts: 'llels Tools' },
  { device: '/dev/sr0',
    mountpoint: 'sr0',
    fstype: 'Parallels Tools',
    opts: 'ls Tools' },
  { device: '/dev/sr0',
    mountpoint: 'dia/Parallels Tools',
    fstype: 'o9660',
    opts: 'uid,nodev,uid=1000,gid=1000,iocharset=utf8,mode=0400,dmode=0500,uhelper=udisks' },
  { device: '/dev/sr0',
    mountpoint: 'arallels Tools',
    fstype: '660',
    opts: 'd=1000,gid=1000,iocharset=utf8,mode=0400,dmode=0500,uhelper=udisks' },
  { device: '/dev/sr0',
    mountpoint: '/media/Parallels Tools',
    fstype: 'iso9660',
    opts: 'ro,nosuid,nodev,uid=1000,gid=1000,iocharset=utf8,mode=0400,dmode=0500,uhelper=udisks' } ];