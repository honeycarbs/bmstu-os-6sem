#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  FILE *f1 = fopen("out.txt", "w");
  FILE *f2 = fopen("out.txt", "w");

  for (char letr = 'a'; letr < '{'; letr++) {
    letr % 2 ? fprintf(f1, "%c", letr) : fprintf(f2, "%c", letr);
  }

  fclose(f2);
  fclose(f1);

  return 0;
}