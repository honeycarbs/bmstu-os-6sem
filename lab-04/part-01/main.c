#include <stdio.h>
#include <stdlib.h>   // atoi
#include <string.h>  // snprintf
#include <unistd.h> // readlink


#define BUF_SIZE 0x1000  // 16cc  4096
#define PATH_SIZE 40

#include "helpers.h"

int read_cmdline(FILE *dest, int pid) {
  char path[PATH_SIZE];
  char buf[BUF_SIZE];
  int len;

  snprintf(path, PATH_SIZE, "/proc/%d/cmdline", pid);

  FILE *fd = fopen(path, "r");
  if (!fd) {
    fprintf(dest, "ERROR: cant open: cmdline.\n");
    return -1;
  }
  len = fread(buf, 1, BUF_SIZE, fd);
  buf[len] = 0;

  fprintf(dest, "%-30s %-30s\n", "CMD LINE:", buf);

  fclose(fd);
  return 0;
}

int read_cwd(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/cwd", pid);

  char buf[BUF_SIZE];

  int len = readlink(path, buf, BUF_SIZE);
  if (!len) {
    fprintf(dest, "cant read cwd");
    return -1;
  }
  buf[len] = 0;

  fprintf(dest, "%-30s %-30s", "CURRENT PROCESS DIRECTORY:", buf);
  return 0;
}

int read_environ(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/environ", pid);

  fprintf(dest, "\n--------------------------------------------- ENVIRON --------------------------------------------------\n");

  char buf[BUF_SIZE];  // буффер
  int len;

  FILE *fd = fopen(path, "r");
  if (!fd) {
    fprintf(dest, "Невозможно открыть environ");
    return -1;
  }

  char *tmpbuf[BUF_SIZE];

  while ((len = fread(buf, 1, BUF_SIZE, fd)) > 0) {
    for (int i = 0; i < len; i++)
      if (buf[i] == 0) buf[i] = 10;  // 10 - код символа конца строки
    buf[len] = 0;
  }
  // printf("%s\n", buf);

  int j = 0;
  char sep[3] ="\n";
  char *istr;
  istr = strtok(buf, sep);
  while (istr != NULL) {
    fprintf(dest, "%s%s",istr , ENVIRON_TEMPLATE[j++]);
    istr = strtok(NULL, sep);
  }
  fclose(fd);

  fprintf(dest,
          "--------------------------------------------------------------------"
          "------------------\n\n");

  return 0;
  }

int read_exe(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/exe", pid);

  char buf[BUF_SIZE];

  int len = readlink(path, buf, BUF_SIZE);
  if (!len) {
    fprintf(dest, "cant read exe\n");
    return -1;
  }
  buf[len] = 0;

  fprintf(dest, "%-30s%-30s\n\n", "SYMBOLIC LINK FROM EXE:", buf);

  return 0;
}

int read_fd(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/fd", pid);

  fprintf(dest,"--------------------FD INFORMATION (every file opened from this process)--------------------\n");

  dopath(path, 0, dest);

  fprintf(dest,
          "--------------------------------------------------------------------"
          "------------------\n\n");
  return 0;
}

// символическая ссылка - указатель на корень файловой системы
int read_root(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/root", pid);

  char buf[BUF_SIZE];

  int len = readlink(path, buf, BUF_SIZE);
  if (!len) {
    fprintf(dest, "cant read root.\n");
    return -1;
  }

  buf[len] = 0;
  fprintf(dest, "%-30s %-30s\n", "SYMBOLIC LINK FOR FILESYSTEM ROOT:", buf);

  return 0;
}

int read_stat(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/stat", pid);

  fprintf(dest,
          "\n-------------------------------------- FILE STAT INFORMATION (process status) --------------------------------------\n");

  char bufer[BUF_SIZE];
  int n = 0;

  FILE *fd = fopen(path, "r");
  int len = fread(bufer, 1, BUF_SIZE, fd);

  char *p = strtok(bufer, " ");

  while (n <= 52) {
    fprintf(dest, STAT_TEMPLATE[n], p);
    n++;
    p = strtok(NULL, " ");
    bufer[len] = 0;
  }

  fclose(fd);
  return 0;
}

// статистика ввода вывода
int read_io(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/io", pid);

  fprintf(dest,
          "\n---------------------------- INPUT/OUTPUT STATISTICS FROM IO ----------------------------\n");
  char buf[BUF_SIZE];
  int len;
  // список выделенных участков памяти, используемых процессом MAPS
  FILE *fd = fopen(path, "r");
  if (!fd) {
    fprintf(dest, "ERROR: cant open: io.\n");
    return -1;
  }
  while ((len = fread(buf, 1, BUF_SIZE, fd)) > 0) {
    for (int i = 0; i < len; i++) {
      if (buf[i] == 0) {
        buf[i] = 10;  // код символа конца строки
      }
    }
    buf[len] = 0;
    fprintf(dest, "%s\n", buf);
  }

  fclose(fd);
  return 0;
}

// список выделенных участков памяти, используемых процессом MAPS
int read_maps(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/maps", pid);

  fprintf(dest,
          "\n---------------------------- MAPS (ALLOCATED MEMORY) ----------------------------\n");

  char buf[BUF_SIZE];
  int len;

  FILE *fd = fopen(path, "r");
  if (!fd) {
    fprintf(dest, "cant open maps.\n");
    return -1;
  }

  while ((len = fread(buf, 1, BUF_SIZE, fd)) > 0) {
    buf[len] = 0;
    fprintf(dest, "%s\n", buf);
  }

  fprintf(dest,
          "--------------------------------------------------------------------------------------\n\n");
  fclose(fd);
  return 0;
}

int read_comm(FILE *dest, int pid) {
  char path[PATH_SIZE];
  char buf[BUF_SIZE];
  int len;

  snprintf(path, PATH_SIZE, "/proc/%d/comm", pid);

  FILE *fd = fopen(path, "r");
  if (!fd) {
    fprintf(dest, "ERROR: cant open: comm.\n");
    return -1;
  }
  len = fread(buf, 1, BUF_SIZE, fd);
  buf[len] = 0;

  fprintf(dest, "%-30s %-30s\n", "COMM:", buf);

  fclose(fd);
  return 0;
}

int read_task(FILE *dest, int pid) {
  char path[PATH_SIZE];
  snprintf(path, PATH_SIZE, "/proc/%d/task", pid);

  fprintf(dest,
          "-------------------- TASK FILE INFORMATION --------------------\n");

  dopath(path, 0, dest);

  fprintf(dest,
          "--------------------------------------------------------------------"
          "------------------\n\n");

  return 0;
}

int main(int argc, char *argv[]) {
  int pid = atoi(argv[1]);
  // printf("%d", pid);
  FILE *out = fopen("out.txt", "w");

  if (!out) {
    printf("cant open output file.");
    return -1;
  }
  
  read_cmdline(out, pid);
  read_cwd(out, pid);
  read_environ(out, pid);
  read_exe(out, pid);
  read_fd(out, pid);
  read_root(out, pid);
  read_stat(out, pid);
  read_io(out, pid);
  read_maps(out, pid);
  read_comm(out, pid);
  read_task(out, pid);

  fclose(out);
  return 0;
}