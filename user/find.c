#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

#define MAXARGS 10

char *fmtname(char *path);
void find(char *path, char *fileName, char *argv[], int argc);
void execFind(char *argv[], int argc, char *file);

int
main(int argc, char *argv[])
{

  if(argc < 3){
    fprintf(2, "Usage: find <path> <fileName>\n");
    exit(1);
  }

  find(argv[1], argv[2], argv, argc);
  return 0;
}

char *
fmtname(char *path) {
  static char buf[DIRSIZ+1];
  char *p;
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
  ;
  p++;

  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

void
execFind(char *argv[], int argc, char *file) {
  char *processArgs[MAXARGS];
  int k = 0;
  for (int i = 4; i < argc; i++) {
    processArgs[k++] = argv[i];
  }
  processArgs[k++] = file;
  processArgs[k++] = 0;

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    if (exec(argv[4], processArgs) < 0) {
      printf("exec returned -1\n");
      exit(1);
    }
  }

  int xstatus = 0;
  if (wait(&xstatus) != pid) {
    printf("wait failed\n");
  }
  if (xstatus != 0) {
    exit(xstatus);
  }
}

void
find(char *path, char *fileName, char *argv[], int argc) {
  char buf[512];
  char *p;
  int fd; 
  struct stat st;
  struct dirent de;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open path %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot fstat %s\n", path);
    close(fd);
    return;
  }


  switch(st.type) {
    case T_FILE:
      if (strcmp(fmtname(path), fileName) == 0) {
        if (argc > 3) {
          execFind(argv, argc, path);
        } else {
          printf("%s\n", path);
        }
      }
    break;
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        return;
      }

      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';

      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
          continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
          continue;
        }

        strcpy(p, de.name);
        find(buf, fileName, argv, argc);
      }
    break;
  }
  close(fd);
}
