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
    test.equal(null, err);
    test.equal(2, result.length);

    psUtil.disk_partitions(true, function(err, result) {
      test.equal(null, err);
      test.equal(15, result.length);

      // Reset platform changes and finish test
      process.platform = platform;
      PSUtil._readFile = readFileFunction;
      psUtil._lib.disk_partitions = lib_function;
      test.done();
    });
  });
}

var disk_partitions_data = [ { device: '/dev/sda1',
    mountpoint: '/',
    fstype: 'ext4',
    opts: 'rw,errors=remount-ro' },
  { device: 'proc',
    mountpoint: '/proc',
    fstype: 'proc',
    opts: 'rw,noexec,nosuid,nodev' },
  { device: 'sysfs',
    mountpoint: '/sys',
    fstype: 'sysfs',
    opts: 'rw,noexec,nosuid,nodev' },
  { device: 'none',
    mountpoint: '/sys/fs/fuse/connections%',
    fstype: 'fusectl',
    opts: 'rw' },
  { device: 'none',
    mountpoint: '/sys/kernel/debug',
    fstype: 'debugfs',
    opts: 'rw' },
  { device: 'none',
    mountpoint: '/sys/kernel/security',
    fstype: 'securityfs',
    opts: 'rw' },
  { device: 'udev',
    mountpoint: '/dev',
    fstype: 'devtmpfs',
    opts: 'rw,mode=0755' },
  { device: 'devpts',
    mountpoint: '/dev/pts',
    fstype: 'devpts',
    opts: 'rw,noexec,nosuid,gid=5,mode=0620' },
  { device: 'tmpfs',
    mountpoint: '/run',
    fstype: 'tmpfs',
    opts: 'rw,noexec,nosuid,size=10%,mode=0755' },
  { device: 'none',
    mountpoint: '/run/lock',
    fstype: 'tmpfs',
    opts: 'rw,noexec,nosuid,nodev,size=5242880' },
  { device: 'none',
    mountpoint: '/run/shm',
    fstype: 'tmpfs',
    opts: 'rw,nosuid,nodev' },
  { device: 'none',
    mountpoint: '/media/psf',
    fstype: 'prl_fs',
    opts: 'rw,nosuid,nodev,sync,noatime,share,_netdev' },
  { device: 'binfmt_misc',
    mountpoint: '/proc/sys/fs/binfmt_misc%',
    fstype: 'binfmt_misc',
    opts: 'rw,noexec,nosuid,nodev' },
  { device: 'gvfs-fuse-daemon',
    mountpoint: '/home/parallels/.gvfs',
    fstype: 'fuse.gvfs-fuse-daemon',
    opts: 'rw,nosuid,nodev,user=parallels' },
  { device: '/dev/sr0',
    mountpoint: '/media/Parallels Tools',
    fstype: 'iso9660',
    opts: 'ro,nosuid,nodev,uid=1000,gid=1000,iocharset=utf8,mode=0400,dmode=0500,uhelper=udisks' } ];