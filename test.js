var Syslog = require('./node-syslog');
var logger1 = new Syslog("node-syslog-test1", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0);
var logger2 = new Syslog("node-syslog-test2", Syslog.LOG_PID | Syslog.LOG_ODELAY, Syslog.LOG_LOCAL0);

logger1.log(Syslog.LOG_INFO, "news info log test1");
logger1.log(Syslog.LOG_ERR, "news log error test1");
logger2.log(Syslog.LOG_WARNING, "news warn log test2");
logger1.log(Syslog.LOG_DEBUG, "Last log message test1 as debug: " + new Date());

// logging in background
Syslog.setAsync(true);
logger1.log(Syslog.LOG_ERR, "This message may never appear in logs");
process.exit(1);
