#include <proc.h>
#include <elf.h>
#include <fs.h>
#include <memory.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define LOAD_BUF_SIZE 512
static uint8_t buf[LOAD_BUF_SIZE];
static void do_load_buf(int fd, size_t source_offset, uint8_t* dest_addr, size_t len) {
  fs_lseek(fd, source_offset, SEEK_SET);
  fs_read(fd, buf, len);
  memcpy(dest_addr, buf, len);
}
static void load_segment(int fd, size_t offset, uint8_t * vaddr, size_t file_size, size_t mem_size) {
    for(int i = 0; i < file_size / LOAD_BUF_SIZE; i ++) {
      do_load_buf(fd, offset + i * LOAD_BUF_SIZE, vaddr + i * LOAD_BUF_SIZE, LOAD_BUF_SIZE);
    }
    size_t rest = file_size % LOAD_BUF_SIZE;
    if(rest > 0) {
      do_load_buf(fd, offset + file_size - rest, vaddr + file_size - rest, rest);
    }
    memset(vaddr + file_size, 0, mem_size - file_size);
}
static uintptr_t loader(PCB *pcb, const char *filename) {
  if(filename == NULL) {
    Log("No file name!\n");
    return 0;
  }
  Log("Start load %s\n", filename);
  int fd = fs_open(filename, 0, 0);
  if(fd < 0) {
    Log("Cannot open file:%s\n", filename);
    return 0;
  }
  Log("fd = %d of file:%s\n", fd, filename);

  Elf_Ehdr elf_header;
  fs_read(fd, &elf_header, sizeof(Elf_Ehdr));
  if(elf_header.e_ident[EI_MAG0] == ELFMAG0 &&
          elf_header.e_ident[EI_MAG1] == ELFMAG1 &&
          elf_header.e_ident[EI_MAG2] == ELFMAG2 &&
          elf_header.e_ident[EI_MAG3] == ELFMAG3) {
    Log("entry:%x\n",  elf_header.e_entry);
    Elf_Phdr elf_program_header;
    for(int i = 0; i < elf_header.e_phnum; i ++) {
      fs_lseek(fd, elf_header.e_phoff + i * elf_header.e_phentsize, SEEK_SET);
      fs_read(fd, &elf_program_header, elf_header.e_phentsize);
      if(elf_program_header.p_type == PT_LOAD) {
        load_segment(fd, elf_program_header.p_offset, (uint8_t*)elf_program_header.p_vaddr, elf_program_header.p_filesz, elf_program_header.p_memsz);
      }
    }
    fs_close(fd);
    return elf_header.e_entry;
  } else {
    Log("Invalid elf file\n");
    return 0;
  }
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
void debug_string(uintptr_t addr) {
  char * s = (char *)addr;
  printf("%s, %p\n", s, addr);
}
void debug_param(uintptr_t top) {
  printf("debug param begin >>>>>>>>>>>>>>>\n");
  uintptr_t *top2 = (uintptr_t *)top;
  printf("top2 from %p\n", top2);
  uintptr_t argc = *((uintptr_t *)top2 ++);
  printf("argc = %d\n", argc);
  for(int i = 0; i < argc; i ++, top2 ++) {
    uintptr_t v = *((uintptr_t*)top2);
    printf("argv[%d]=%p, top2:%p\n", i, v, top2);
    debug_string(v);
  }
  for(int i = 0; *top2 != 0; i++, top2 ++) {
    uintptr_t v = *((uintptr_t*)top2);
    printf("envp[%d]=%p, top2:%p\n", i, v, top2);
    debug_string(v);
  }
  printf("debug param end <<<<<<<<<<<<<<<<\n");
}
uintptr_t pargv[10];
uintptr_t penvp[10];
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  // Assign stack and copy argv first to avoid argv, which may be assigned in code space,  is destroyed by loading code.
  char *top = new_page(8);
  top -=8;
  int argc = 0;
  for(argc = 0; argv[argc] != NULL ; argc ++) {
    top --;
    char * const s = argv[argc];
    int len = strlen(s) + 1;
    top -= len;
    strncpy(top, s, len);
    top[len] = '\0';
    pargv[argc] = (uintptr_t)top;
    printf("context_uload:%s, %s, %s, %p\n", argv[argc], top, (char*)pargv[argc], pargv[argc]);
  }
  int envc = 0;
  for(envc = 0; envp[envc] != NULL; envc ++) {
    top --;
    char * const s = envp[envc];
    int len = strlen(s) + 1;
    top -= len;
    strncpy(top, s, len);
    top[len] = '\0';
    penvp[envc] = (uintptr_t)top;
    printf("context_uload:%s, %s, %s, %p\n", argv[envc], top, (char*)penvp[envc], penvp[envc]);
  }
  uintptr_t * top2 = (uintptr_t*)top;
  // printf("top:%p, top2:%p\n", top, top2);
  *--top2 = 0;
  for(int i = envc - 1; i >= 0; i --){
    --top2;
    // printf("penvp:%p, top2=%p\n", penvp[i], top2);
    * top2 = penvp[i];
  }
  for(int i = argc - 1; i >= 0; i --) {
    --top2;
    // printf("pargv:%p, top2=%p\n", pargv[i], top2);
    * top2 = pargv[i];
  }
  * --top2 = argc;
  uintptr_t entry = loader(pcb, filename);
  Area area;
  area.start = pcb->stack;
  area.end = pcb->stack + STACK_SIZE;
  pcb->cp = ucontext(NULL, area, (void*)entry);
  pcb->cp->GPRx = (uintptr_t)top2;
  printf("context_uload stack:%p, heap:%p\n", pcb->cp->GPRx, heap.end);
  debug_param(pcb->cp->GPRx);
}

