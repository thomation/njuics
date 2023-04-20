#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern uint8_t ramdisk_start;
extern size_t ramdisk_read(void *buf, size_t offset, size_t len); 
static uintptr_t loader(PCB *pcb, const char *filename) {
  if(filename != NULL)
    Log("Start load %s\n", filename);
  else
    Log("Start load default \n");
  Elf_Ehdr elf_header;
  ramdisk_read(&elf_header, 0, sizeof(Elf_Ehdr));
  if(elf_header.e_ident[EI_MAG0] == ELFMAG0 &&
          elf_header.e_ident[EI_MAG1] == ELFMAG1 &&
          elf_header.e_ident[EI_MAG2] == ELFMAG2 &&
          elf_header.e_ident[EI_MAG3] == ELFMAG3) {
    uintptr_t start = (uintptr_t)&ramdisk_start;
    Log("start:%p, entry:%x\n", start, elf_header.e_entry);
    return start + (elf_header.e_entry - 0x830000b4);
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

