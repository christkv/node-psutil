var PSUtil = require('../lib/psutil').PSUtil;

exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Should correctly fetch pid list'] = function(test) {
  var psUtil = new PSUtil();
  psUtil.pid_list(function(err, result) {
    test.equal(null, err);
    test.ok(Array.isArray(result));
    test.done();
  });
}

exports['Should correctly fetch pid list on linux'] = function(test) {
  var psUtil = new PSUtil();
  var directoryData = [ 'asound',  'sysrq-trigger',  'partitions',  'diskstats',  'crypto',  'key-users',  'version_signature',  'kpageflags',  'kpagecount',  'kmsg',  'kcore',  'softirqs',  'version',  'uptime',  'stat',  'meminfo',  'loadavg',  'interrupts',  'devices',  'cpuinfo',  'consoles',  'cmdline',  'locks',  'filesystems',  'slabinfo',  'swaps',  'vmallocinfo',  'zoneinfo',  'vmstat',  'pagetypeinfo',  'buddyinfo',  'latency_stats',  'kallsyms',  'modules',  'dma',  'timer_stats',  'timer_list',  'iomem',  'ioports',  'execdomains',  'schedstat',  'sched_debug',  'mdstat',  'scsi',  'misc',  'acpi',  'fb',  'mtrr',  'irq',  'cgroups',  'sys',  'bus',  'tty',  'driver',  'fs',  'sysvipc',  'net',  'mounts',  'self',  '1',  '2',  '3',  '6',  '7',  '8',  '9',  '10',  '11',  '12',  '13',  '14',  '15',  '16',  '17',  '18',  '19',  '22',  '23',  '24',  '25',  '26',  '27',  '28',  '36',  '38',  '39',  '40',  '41',  '42',  '43',  '45',  '47',  '50',  '51',  '72',  '209',  '210',  '303',  '311',  '484',  '490',  '525',  '531',  '533',  '540',  '567',  '576',  '670',  '722',  '725',  '771',  '788',  '806',  '809',  '830',  '862',  '914',  '921',  '931',  '933',  '938',  '951',  '953',  '954',  '972',  '1184',  '1225',  '1232',  '1237',  '1241',  '1249',  '1252',  '1275',  '1350',  '1386',  '1389',  '1390',  '1400',  '1401',  '1411',  '1430',  '1449',  '1461',  '1522',  '1524',  '1526',  '1532',  '1533',  '1534',  '1536',  '1537',  '1538',  '1540',  '1543',  '1568',  '1570',  '1571',  '1576',  '1578',  '1582',  '1586',  '1592',  '1594',  '1600',  '1604',  '1608',  '1618',  '1625',  '1630',  '1632',  '1643',  '1645',  '1657',  '1658',  '1666',  '1675',  '1680',  '1685',  '1690',  '1693',  '1723',  '1726',  '1734',  '1735',  '1738',  '1750',  '1752',  '1754',  '1756',  '1800',  '1802',  '1806',  '1842',  '1858',  '1864',  '1865',  '1924',  '1956',  '1960',  '1968',  '2096',  '2325',  '2328',  '3295',  '3351',  '3433',  '3561',  '3583' ];
  // Save the existing setup
  var platform = process.platform;
  var file_function = PSUtil._readDirectory;

  // Setup Overrides
  process.platform = 'linux'
  PSUtil._readDirectory = function(path, callback) {
    callback(null, directoryData);
  }
  psUtil._lib.sys_conf = function(code, callback) {
    callback(null, 100);
  }

  psUtil.pid_list(function(err, result) {
    test.equal(null, err);
    test.ok(typeof result[0] == 'number');

    process.platform = platform;
    PSUtil._readDirectory = file_function;
    test.done();
  });
}