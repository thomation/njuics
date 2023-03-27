#include <am.h>
#include <klib-macros.h>
#include <klib.h>

int main(const char *args) {
  const char *fmt =
    "Hello, AbstractMachine!\n"
    "mainargs = '%'.\n";

  for (const char *p = fmt; *p; p++) {
    (*p == '%') ? putstr(args) : putch(*p);
  } 
  printf("Hello world\n");
  printf("%d\n", 123);
  printf("%3d\n", 123);
  printf("%4d\n", 123);
  printf("%05d\n", 123);
  return 0;
}
