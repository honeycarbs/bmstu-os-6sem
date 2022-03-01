#include <dirent.h>  //opendir()/readdir()/closedir()
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

//путь	//глубина рекурсии
static int dopath(const char *pathname, int depth) {
  struct stat statbuf;
  struct dirent *dirp;
  DIR *dp;

  //'.' - указатель на сам каталог
  //'..' - указатель на родительский каталог
  if (depth != 0 && (strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0))
    return 0;

  //Вызов lstat() идентичен stat(), но в случае, если pathname является
  //символьной ссылкой, то возвращается информация о самой ссылке, а не о файле,
  //на который она указывает; возвращает информацию об указанном файле
  if (lstat(pathname, &statbuf) < 0) {
    switch (errno) {
      case EBADF:
        printf("Not a valid open file descriptor.");
        break;
      case ENOENT:
        printf(
            "A component of pathname does not exist or "
            "is a dangling symbolic link.");
        break;
      case ENOTDIR:
        printf("A component of the path prefix of pathname is not a directory.");
        break;
      case ELOOP:
        printf("Too many symbolic links encountered while traversing the path.");
        break;
      case EFAULT:
        printf("Bad address. ");
        break;
      case EACCES:
        printf(
            "Search  permission  is  denied for one of the directories in "
            "the path prefix of pathname.");
        break;
      case ENOMEM:
        printf("Out of memory (i.e., kernel memory).");
        break;
      case ENAMETOOLONG:
        printf("pathname is too long.");
        break;
      case EOVERFLOW:
        printf(
            "pathname or fd refers to a file whose  size,  inode  number,  or"
            "number  of  blocks  cannot  be represented in s.");
        break;
    }
    printf("Can't lstat.");
    return -1;
  }

  for (int i = 0; i < depth; ++i) printf("              |");

  // s_isdir проверяет, является ли данный файл каталогом
  if (S_ISDIR(statbuf.st_mode) == 0) {
    printf("%s %ld\n", pathname, statbuf.st_ino);
    return 0;
  }

  /* каталог */
  printf("%s %ld\n", pathname, statbuf.st_ino);
  if ((dp = opendir(pathname)) == NULL) {
    printf("couldn't open directory '%s'\n", pathname);
    return 1;
  }

  // chdir() изменяет текущий рабочий каталог вызывающего процесса на каталог,
  // указанный в path; PATH — переменная окружения, представляющая собой набор
  // каталогов, в которых расположены исполняемые файлы; нужен для коротких имен
  chdir(pathname);

  // для каждого элемента каталога
  // Функция readdir() возвращает указатель на следующую запись каталога в
  // структуре dirent, прочитанную из потока каталога. Каталог указан в dir.
  // Функция возвращает NULL по достижении последней записи или если была
  // обнаружена ошибка.
  while ((dirp = readdir(dp)) != NULL) dopath(dirp->d_name, depth + 1);
  chdir("..");

  //закрывает поток каталога и освобождает ресурсы, выделенные ему.
  //Она возвращает 0 в случае успеха и -1 при наличии ошибки.
  closedir(dp);
}

int main(int argc, char *argv[]) {
  int ret;  ///код возврата

  if (argc != 2)            
    ret = dopath(".", 0);  
  else
    ret = dopath(argv[1], 0); 

  return ret;
}