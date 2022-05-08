#include <fcntl.h>
#include <unistd.h>

int main() {
  int fd1 = open("alphabet.txt", O_RDONLY);
  int fd2 = open("alphabet.txt", O_RDONLY);

  char c;

  while (read(fd1, &c, 1) == 1 && read(fd2, &c, 1) == 1) {
    write(1, &c, 1);
    write(1, &c, 1);
  }

  return 0;
}