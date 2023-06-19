#include <stdint.h>
#include <stdio.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  int test = 10;
  printf("dummy main addr %p\n", main);
  printf("dummy test addr %p, value %d\n", &test, test);
  return _syscall_(SYS_yield, 0, 0, 0);
}
