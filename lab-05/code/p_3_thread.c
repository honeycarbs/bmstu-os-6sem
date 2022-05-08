#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct stat statbuf;

void *thread_routine() {
  FILE *f2 = fopen("out.txt", "a");
  stat("out.txt", &statbuf);
  printf("open for fs2: inode  = %ld, buffsize = %ld blocksize= %ld\n",
         (long int)statbuf.st_ino, (long int)statbuf.st_size,
         (long int)statbuf.st_blksize);

  for (char letr = 'a'; letr < '{'; letr += 2) fprintf(f2, "%c", letr);
  fclose(f2);
  stat("out.txt", &statbuf);
  printf("close for fs2: inode  = %ld, buffsize = %ld blocksize= %ld\n",
         (long int)statbuf.st_ino, (long int)statbuf.st_size,
         (long int)statbuf.st_blksize);
}

int main() {
  FILE *f1 = fopen("out.txt", "a");
  stat("out.txt", &statbuf);
  printf("open for fs1: inode  = %ld, buffsize = %ld blocksize= %ld\n",
         (long int)statbuf.st_ino, (long int)statbuf.st_size,
         (long int)statbuf.st_blksize);

  pthread_t thr_worker;
  pthread_create(&thr_worker, NULL, thread_routine, f1);
  pthread_join(thr_worker, NULL);

  for (char letr = 'a'; letr < '{'; letr += 2) fprintf(f1, "%c", letr);

  fclose(f1);
  stat("out.txt", &statbuf);
  printf("close for fs2: inode  = %ld, buffsize = %ld blocksize= %ld\n",
         (long int)statbuf.st_ino, (long int)statbuf.st_size,
         (long int)statbuf.st_blksize);

  return 0;
}