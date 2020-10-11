[![Build status](https://ci.appveyor.com/api/projects/status/gtk1anffnukikuvf?svg=true)](https://ci.appveyor.com/project/Defvyb/log-parser)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=fusion_core_log_parser&metric=alert_status)](https://sonarcloud.io/dashboard?id=fusion_core_log_parser)

Log parsing test app, uses rapidjson for parsing and writing

```
Usage:  
-p=/path/to/logs/ path to logs, default is current dir 
-n=1 files count, default is 1 
-t=1 threads count, default is 1 
-c json checking enabled(it can extremely slow entire process) 
-h this help

example:
log_parser -p=/home/user/data/ -n=10 -t=10 -c - parse 10 files file1.log, file2.log...file10.log with 10 threads and check json for validity

log_parser - parse 1 file1.log with 1 thread without validity checking
```
L