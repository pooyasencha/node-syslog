# Node-Syslog

v2.0

This is an node module (add-on) to work with system log daemon on unix systems.
Module is tested with node.js v0.8.1 and metalog, syslog-ng 3.1.1.

Read Wiki "setMask" page for more information about how to use "setMask" functionality.


Current version is compatible to node 0.8.x and higher versions. For older nodejs versions please use node-syslog v1.0.3

Node-syslog doesn't support Darwin OS and MS Windows, but should work fine with this OSes.

## Authors

*   Nazar Kulyk
*   Jeremy Childs
*   Pooya Karimian

## Installation

### npm

      npm install node-syslog

### manual

      git clone
      node-gyp configure build

## Usage

Here is an example of how to use `node-syslog`:

     var Syslog = require('node-syslog');
     
     var logger = new Syslog("node-syslog", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0, false);
     logger.log(Syslog.LOG_INFO, "Node Syslog Module output " + new Date());
     
Check your /var/log/messages (syslog, syslog-ng), or /var/log/everything/current (metalog) file for any test entry.

By default, .log calls are blocking, but you can use `setAsync` method to make it log in the background. **Note** that you may lose some log messages if node.js process exits (by calling process.exit or killed) before all the messages are logged.
	
	var Syslog = require('node-syslog');
	Syslog.setAsync(true);
	
	var logger = new Syslog("node-syslog", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0, false);
	
Using `.setMask(level, upTo=true/false)`, you can set a mask to only log messages of certain log level or all priorities above and including it.

    var Syslog = require('node-syslog');
	
	var logger = new Syslog("node-syslog", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0, false);
	logger.setMask(Syslog.LOG_INFO, true);
    logger.log(Syslog.LOG_INFO, "This log will appear");
    logger.log(Syslog.LOG_DEBUG, "This log will not appear");

For more information about how to use this module check `test.js`.
