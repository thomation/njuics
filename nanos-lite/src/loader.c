#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern uint8_t ramdisk_start;
static void load_segment(size_t offset, uint8_t * vaddr, size_t file_size, size_t mem_size) {
    memcpy(vaddr, &ramdisk_start + offset, file_size);
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

  Elf_Ehdr elf_header;
  fs_read(fd, &elf_header, sizeof(Elf_Ehdr));
  if(elf_header.e_ident[EI_MAG0] == ELFMAG0 &&
          elf_header.e_ident[EI_MAG1] == ELFMAG1 &&
          elf_header.e_ident[EI_MAG2] == ELFMAG2 &&
          elf_header.e_ident[EI_MAG3] == ELFMAG3) {
    uintptr_t start = (uintptr_t)&ramdisk_start;
    Log("start:%p, entry:%x\n", start, elf_header.e_entry);
    Elf_Phdr elf_program_header;
    for(int i = 0; i < elf_header.e_phnum; i ++) {
      fs_lseek(fd, elf_header.e_phoff + i * elf_header.e_phentsize, SEEK_SET);
      fs_read(fd, &elf_program_header, elf_header.e_phentsize);
      if(elf_program_header.p_type == PT_LOAD) {
        load_segment(elf_program_header.p_offset, (uint8_t*)elf_program_header.p_vaddr, elf_program_header.p_filesz, elf_program_header.p_memsz);
      }
    }
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

