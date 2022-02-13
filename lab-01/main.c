#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // strerror
#include <errno.h> // errno
#include <pthread.h>
#include <stdio.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define SLEEP_TIME 15

sigset_t mask;

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl);
}

void daemonize(const char *cmd) {
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    // сброс маски создания файла, новый процесс наследует 
    // от процесса - предка сигнальную маску создания файлов. 
    // поскольку демон - особый процесс, ему не нужно наследовать маску
    // создания файлов. демон мог создавать файлы с любыми правами доступа.
    umask(0);
    // RLIMIT_NOFILE specifies a value one greater than 
    // the  maximum  file  descriptor  number  that  can be opened by this process.
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) { // позволяет получить максимальный номер дескриптора для того чтобы в последствии закрыть все открытые файлы
        syslog(LOG_ERR, "can\'t get file limit\n");
    }
    // стать лидером новой сессии, чтобы утратить управляющий терминал
    if ((pid = fork()) < 0) {
        syslog(LOG_ERR, "can\'t fork\n");
    } else if (pid != 0) {
        exit(0);
    } 
    setsid();
    // обеспечить невозможность утрату терминала в будущем
    // сигнал SIGHUB сообщает процессу, что процесс утратил управляющий терминал
    sa.sa_handler = SIG_IGN; // мактрос - ignore this signal
    sigemptyset(&sa.sa_mask); // empty mask set
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        syslog(LOG_ERR, "can\'t ignore SIGHUP\n");
    }
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "can\'t set directory to /\n");
    }
    // закрыть все открытые файловые дескрипторы
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024; // максимально возможное количество открытых файлов
    }
    for (i = 0; i < rl.rlim_max; i++) {
        close(i);
    }
    // присоединить файловые дескрипторы к 0, 1, 2 к /dev/null
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d, %d, %d\n", fd0, fd1, fd2);
        exit(1);
    }
}

int already_running(void) {
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0) {
        syslog(LOG_ERR, "can\'t open %s : %s\n", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "can't lock %s: %s\n", LOCKFILE, strerror(errno));
        exit(1);
    }
    // усечение размера файла необходимо по той причине, что 
    // идентификатор процесса предыдцщей копии демона, 
    // представленный в виде строки, мог иметь большую длину.
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

void reread(void) {
    FILE *fd;
    int pid;

    if ((fd = fopen("/var/run/daemon.pid", "r")) == NULL) {
        syslog(LOG_ERR, "can\'t open /var/run/daemon.pid.");
    }

    fscanf(fd, "%d", &pid);
    fclose(fd);

    syslog(LOG_INFO, "Daemon pid red from config file: %d", pid);
}

void *thr_fn(void *arg) {
    int err, signo;
    for (;;) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            syslog(LOG_ERR, "can\'t sigwait\n");
            exit(1);
        }
        switch (signo)
        {
        case SIGHUP:
            syslog(LOG_INFO, "SIGHUB occured. re-reading config file...\n");
            reread();
            break;
        case SIGTERM:
            syslog(LOG_INFO, "SIGTERM occured. exiting...\n");
            exit(0);
        default:
            syslog(LOG_INFO, "unexpected signal %d\n", signo);
            break;
        }
    }
    return (void*)0;
}

int main(int argc, char *argv[]) {
    int err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL) {
        cmd = argv[0];
    } else {
        cmd++;
    }
    daemonize(cmd);
    if(already_running()) {
        syslog(LOG_ERR, "can\'t launch daemon - already running\n");
        exit(1);
    }
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        syslog(LOG_ERR, "can\'t restore SIGHUB to default\n");
    }
    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        syslog(LOG_ERR, "SIG_BLOCK error");
    }
    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0) {
        syslog(LOG_ERR, "can\'t create new thread\n");
    } else {
        syslog(LOG_INFO, "new thread created\n");
    }
    time_t timestamp;
    struct tm *time_info;
    for (;;) {
        sleep(SLEEP_TIME);
        time(&timestamp);
        time_info = localtime(&timestamp);
        syslog(LOG_INFO, "%s", asctime(time_info));
    }
}