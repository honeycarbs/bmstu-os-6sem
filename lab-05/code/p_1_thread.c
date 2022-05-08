#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

void *thread_routine(void *fd) {
  int flag = 1;
  char c;

  FILE *fs = fdopen(*((int *)fd), "r");
  char buf[20];
  setvbuf(fs, buf, _IOFBF, 20);

  while (flag == 1) {
    flag = fscanf(fs, "%c", &c);
    if (flag == 1) {
      fprintf(stdout, "%c", c);
    }
  }
}

int main(void) {
  int fd = open("alphabet.txt", O_RDONLY);

  FILE *fs = fdopen(fd, "r");
  char buf[20];
  setvbuf(fs, buf, _IOFBF, 20);

  pthread_t thr_worker;

  pthread_create(&thr_worker, NULL, thread_routine, &fd);

  int flag = 1;
  char c;
  while (flag == 1) {
    flag = fscanf(fs, "%c", &c);
    if (flag == 1) {
      fprintf(stdout, "%c", c);
    }
  }
  pthread_join(thr_worker, NULL);
  return 0;
}