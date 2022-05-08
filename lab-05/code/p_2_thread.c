#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mux;

void *thread_routine(void *arg) {
  int fd = *((int *)arg);

  int flag = 1;
  char c;

  pthread_mutex_lock(&mux);
  while (flag == 1) {
    flag = read(fd, &c, 1);
    if (flag == 1) write(1, &c, 1);
  }
  pthread_mutex_unlock(&mux);
}

int main() {
  int fd1 = open("alphabet.txt", O_RDONLY);
  int fd2 = open("alphabet.txt", O_RDONLY);

  pthread_t thr_worker;

  if (pthread_mutex_init(&mux, NULL) != 0) {
    printf("can't pthread_mutex_init.\n");
    return 1;
  }

  pthread_create(&thr_worker, NULL, thread_routine, &fd1);

  int flag = 1;
  char c;

  pthread_mutex_lock(&mux);
  while (flag == 1) {
    flag = read(fd2, &c, 1);
    if (flag == 1) write(1, &c, 1);
  }
  pthread_mutex_unlock(&mux);
  
  pthread_join(thr_worker, NULL);

  return 0;
}