node-psutil
===========

The start of a port of the pyhton library psutil.

Usage
=====

    var PSUtil = require('psutil').PSUtil;

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

