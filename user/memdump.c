#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);
void print_uint64(uint64 value);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
print_uint64(uint64 value) {
  unsigned int top_half = (unsigned int)(value >> 32);
  unsigned int bottom_half = (unsigned int)value;
  printf("%x", bottom_half);
  printf("%x\n", top_half);
}

int
pow (int base, int exp) {
  int result = 1;
  for (int i = 0; i < exp; ++i) {
    result = result * base;
  }
  return result;
}

void
memdump(char *fmt, char *data)
{
  int curr = 0;

  for (int j = 0; j < strlen(fmt); j++) {
    switch (fmt[j]) {
      case 'i':
        printf("%d\n", *(int*)(data + curr));
        curr+=4;
      break;

      case 'p':
        print_uint64((uint64)*(char**)data);
        curr += 8;
      break;

      case 'h':
        printf("%d\n", *(short*)(data + curr));
        curr+=2;
      break;

      case 'c':
        printf("%c\n", data[curr]);
        curr++;
      break;

      case 's':
        printf("%s\n", *(char **)data);
        curr += 8;
      break;

      case 'S':
        while(data[curr]) {
          printf("%c", data[curr]);
          curr++;
        }
        printf("\n");
      break;
    }
  }
}
