#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int checkDelimiter(const char *str, const char *delim);
int checkIfNumber(const char *str);
void printSixFive(const char *fileName);

int
main(int argc, char *argv[])
{

  if(argc < 2){
    fprintf(2, "Usage: p <duration>...\n");
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
    printSixFive(argv[i]);
  }
}

void
printSixFive(const char *fileName) {
  int fd = open(fileName, O_RDONLY);
  if (fd < 0) {
      printf("open %s failed\n", fileName);
      exit(1);
  }

  char readChar;
  char buffer[100];
  int i = 0;
  char *delim = "^-\r\t\n./,";
  while(read(fd, &readChar, 1)) {
    buffer[i++] = readChar;
    buffer[i+1] = '\0';

    if (checkDelimiter(buffer, delim)) {
      buffer[i-1] = '\0';
      if (checkIfNumber(buffer)) {
        //printf("buffer = %s\n", buffer);
       int value = atoi(buffer);
       if ((value % 5 == 0) || (value % 6 == 0)) {
        printf("%d\n", value);
       }
      }
      i = 0;
    }
  }

  if (i > 1) {
    buffer[i++] = '\0';
    if (checkIfNumber(buffer)) {
      int value = atoi(buffer);
      if ((value % 5 == 0) || (value % 6 == 0)) {
        printf("%d\n", value);
      }
    }
  }
}

int
checkDelimiter(const char *str, const char *delim) {
  for (int i = 0; i < strlen(delim); i++) {
    if (strchr(str, delim[i]) != 0) {
      return 1;
    }
  }
  return 0;
}

int
checkIfNumber(const char *str) {
  if (strlen(str) < 1) {
    return 0;
  }
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] >= '0' && str[i] <= '9') {
      continue;
    } else{
      return 0;
    }
  }
  return 1;
}
