var Syslog = require('./node-syslog');
var logger1 = new Syslog("node-syslog-test1", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0);
var logger2 = new Syslog("node-syslog-test2", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0);

logger1.log(Syslog.LOG_INFO, "test info log from logger1");
logger1.log(Syslog.LOG_ERR, "test error log from logger1");
logger2.log(Syslog.LOG_WARNING, "test warn log from logger2");
// this will be the last message appearing
logger1.log(Syslog.LOG_DEBUG, "[Last] test debug message from logger: " + new Date());

// mask anything below info level
logger1.setMask(Syslog.LOG_INFO, true);
logger1.log(Syslog.LOG_DEBUG, "This test message from logger1 will never appear in logs");

// logging in background
Syslog.setAsync(true);
logger1.log(Syslog.LOG_ERR, "This test message from logger1 may never appear in logs");
process.exit(1);
