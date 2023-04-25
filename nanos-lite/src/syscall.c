#include <common.h>
#include "syscall.h"
int sys_yield();
int sys_exit();
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();
    case SYS_exit:
      c->GPRx = sys_exit();
    break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
int sys_yield()
{
  yield();
  return 0;
}
int sys_exit()
{
  halt(0);
  return 0;
}
