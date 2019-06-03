# logwatchdog_daemon
## Advanced Unix Programming HW5

Detect Login, Logout, makes log file. Print with syslog

![Preview](https://github.com/BaeJuneHyuck/logwatchdog_daemon/blob/master/snapshot/snap2.png?raw=true)

1.	Run and check with ps 

![Preview](https://github.com/BaeJuneHyuck/logwatchdog_daemon/blob/master/snapshot/snap3.png?raw=true)

2.  In CSE(Temp file) , can check login , logout information
Used Strings to print out byte file


![Preview](https://github.com/BaeJuneHyuck/logwatchdog_daemon/blob/master/snapshot/snap4.png?raw=true)

3.	Tracking with tail -f command

![Preview](https://github.com/BaeJuneHyuck/logwatchdog_daemon/blob/master/snapshot/snap5.png?raw=true)

4.	New login

![Preview](https://github.com/BaeJuneHyuck/logwatchdog_daemon/blob/master/snapshot/snap6.png?raw=true)

5.	New login data reflected in output of “tail -f”

![Preview](https://github.com/BaeJuneHyuck/logwatchdog_daemon/blob/master/snapshot/snap7.png?raw=true)

6.	New login data reflected in temp file “cse”
