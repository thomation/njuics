#include <fs.h>

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};
#define FILE_TABLE_SIZE sizeof(file_table) / sizeof(Finfo)
void init_fs() {
  // TODO: initialize the size of /dev/fb
  for(int i = 3; i < FILE_TABLE_SIZE; i++) {
    Finfo *e = &file_table[i];
    e->open_offset = 0;
    e->read = ramdisk_read;
    e->write = ramdisk_write;
  }
}
int fs_open(const char *pathname, int flags, int mode) {
  for(int i = 0; i < FILE_TABLE_SIZE; i ++) {
    if(strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  return -1;
}
size_t fs_read(int fd, void *buf, size_t len) {
  Finfo * e = &file_table[fd];
  size_t ret = e->read(buf, e->disk_offset + e->open_offset, len);
  if(ret > 0)
    e->open_offset += ret;
  return ret;
}
size_t fs_write(int fd, const void *buf, size_t len) {
  Finfo * e = &file_table[fd];
  size_t ret = e->write(buf, e->disk_offset + e->open_offset, len);
  if(ret > 0)
    e->open_offset += ret;
  return ret;
}
size_t fs_lseek(int fd, size_t offset, int whence) {
  Finfo * e = &file_table[fd];
  switch(whence) {
    case SEEK_SET:
      e->open_offset = offset;
      break;
    case SEEK_CUR:
      e->open_offset += offset;
      break;
    case SEEK_END:
      e->open_offset = e->size + offset;
      break;
  }
  return e->open_offset;
}
int fs_close(int fd) {
  return 0;
}