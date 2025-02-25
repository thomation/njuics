#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>

int sys_exit();
int sys_yield();
int sys_open(const char * path, int flags, int mode);
int sys_read(int fd, void * buf, size_t len);
int sys_write(int fd, void * buf, size_t len);
int sys_close(int fd);
int sys_lseek(int fd, int offset, int whence);
extern int mm_brk(uintptr_t brk); 
int sys_brk(void * addr);
extern void naive_uload(PCB *pcb, const char *filename); 
extern void create_proc(const char *filename, char *const argv[], char *const envp[]); 
int sys_execve(const char *fname, char * const argv[], char *const envp[]);
extern int gettimeofday(void * tv, void * tz);
int sys_gettimeofday(void * tv, void * tz);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("System Call: context %p type:%d, a1:%p, a2:%p, a3:%p\n", c, a[0], a[1], a[2], a[3]);
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
    case SYS_execve:
      c->GPRx = sys_execve((const char*)a[1], (char* const*)a[2], (char* const*)a[3]);
      break;
    case SYS_gettimeofday:
      c->GPRx = sys_gettimeofday((void*)a[1], (void*)a[2]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  // printf("System Call: return:%d\n", c->GPRx);
}

int sys_exit()
{
  halt(0);
  // sys_execve("/bin/nterm", NULL, NULL);
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
  return fs_write(fd, buf, len);
}
int sys_close(int fd) {
  return fs_close(fd);
}
int sys_lseek(int fd, int offset, int whence) {
  return fs_lseek(fd, offset, whence);
}
int sys_brk(void * addr) {
  printf("sys_brk: %p\n", addr);
  return mm_brk((uintptr_t) addr);
}
int sys_execve(const char *fname, char * const argv[], char *const envp[])
{
  printf("sys_execve fname:%s, argv:%p, envp:%p\n", fname, argv, envp);
  for(int i = 0; argv[i] != NULL; i ++) {
    printf("sys_execve argv[%d]=%s\n", i, argv[i]);
  }
  for(int i = 0; envp[i] != NULL; i ++) {
    printf("sys_execve envp[%d]=%s\n", i, envp[i]);
  }
  char path[32];
  strcpy(path, fname);
  if(path[0] != '/') {
    sprintf(path, "%s/%s", envp[0], fname);
  }
  create_proc(path, argv, envp);
  return 0;
}
int sys_gettimeofday(void *tv, void *tz) {
  return gettimeofday(tv, tz);
}
