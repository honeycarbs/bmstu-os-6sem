## Лабораторная №1, процессы - демоны

### COMPILE & RUN
```bash
gcc main.c -lpthread
sudo ./a.out
```

### OTHER
Получаем pid
```bash
cat /var/run/daemon.pid
```
Смотрим лог
```
cat /var/log/syslog | grep a.out
```
Проверяем чтобы демон был демоном. В консоль:
```
ps -ajx | head -1 && ps -ajx | grep $DAEMON_PID$
```
PID == PGID == SID, TTY = "?", TPGID = Ssl

### THREAD SIGNAL TRIGGERS

1. Триггер SIGHUB: ```sudo kill -1 $DAEMON_PID$```  
2. Триггер SIGTERM: ```sudo kill $DAEMON_PID$```
### QnA
