#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
char * argv[10];
char * envp[10];

char * parse_string(uintptr_t addr) {
  char * s = (char *)addr;
  return s;
}
int parse_param(uintptr_t top) {
  uintptr_t *top2 = (uintptr_t *)top;
  // printf("top2 from %p\n", top2);
  int argc = *((int *)top2 ++);
  // printf("argc = %d\n", argc);
  int i = 0;
  for(i = 0; i < argc; i ++, top2 ++) {
    uintptr_t v = *((uintptr_t*)top2);
    // printf("argv[%d]=%p, top2:%p\n", i, v, top2);
    argv[i] = parse_string(v);
  }
  argv[i] = NULL;
  for(i = 0; *top2 != 0; i++, top2 ++) {
    uintptr_t v = *((uintptr_t*)top2);
    // printf("envp[%d]=%p, top2:%p\n", i, v, top2);
    envp[i] = parse_string(v);
  }
  envp[i] = NULL;
  return argc;
}
void call_main(uintptr_t *args) {
  int argc = parse_param((uintptr_t)args);
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
