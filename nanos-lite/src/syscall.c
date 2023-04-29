#include <common.h>
#include "syscall.h"
#include <fs.h>

int sys_exit();
int sys_yield();
int sys_open(const char * path, int flags, int mode);
int sys_read(int fd, void * buf, size_t len);
int sys_write(int fd, void * buf, size_t len);
int sys_close(int fd);
int sys_lseek(int fd, int offset, int whence);
int sys_brk(void * addr);
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  printf("System Call: type:%d, a1:%p, a2:%p, a3:%p\n", a[0], a[1], a[2], a[3]);
  switch (a[0]) {
    case SYS_exit:
      c->GPRx = sys_exit();
      break;
    case SYS_yield:
      c->GPRx = sys_yield();
      break;
    case SYS_open:
      c->GPRx = sys_open((const char *)a[1], a[2], a[3]);
      break;
    case SYS_read:
      c->GPRx = sys_read(a[1], (void*)a[2], a[3]);
      break;
    case SYS_write:
      c->GPRx = sys_write(a[1], (void*)a[2], a[3]);
      break;
    case SYS_close:
      c->GPRx = sys_close(a[1]);
      break;
    case SYS_lseek:
      c->GPRx = sys_lseek(a[1], a[2], a[3]);
      break;
    case SYS_brk:
      c->GPRx = sys_brk((void*)a[1]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

int sys_exit()
{
  halt(0);
  return 0;
}
int sys_yield()
{
  yield();
  return 0;
}
int sys_open(const char * path, int flags, int mode) {
  return fs_open(path, flags, mode);
}
int sys_read(int fd, void * buf, size_t len) {
  return fs_read(fd, buf, len);
}
int sys_write(int fd, void* buf, size_t len) {
  uint8_t *str = buf;
  if(fd == 1) {
    for(int i = 0; i < len; i ++) {
      putch(str[i]);
    }
    return len;
  }
  return fs_write(fd, buf, len);
}
int sys_close(int fd) {
  return fs_close(fd);
}
int sys_lseek(int fd, int offset, int whence) {
  return fs_lseek(fd, offset, whence);
}
int sys_brk(void * addr) {
  return 0;
}
