#include <stdint.h>
#include <stdio.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main(int argc, char ** argv) {
  int test = 0;
  while(1) {
    if(test % 10000 == 0) {
      printf("dummy main addr %p, test=%d, argv=%s\n", main, test, argv[1]);
      _syscall_(SYS_yield, 0, 0, 0);
    }
    test ++;
 }
  return _syscall_(SYS_yield, 0, 0, 0);
}
