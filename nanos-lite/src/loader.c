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
    Elf_Phdr elf_program_header;

    for(int i = 0; i < elf_header.e_phnum; i ++) {
      ramdisk_read(&elf_program_header, elf_header.e_phoff + i * elf_header.e_phentsize,elf_header.e_phentsize);
      if(elf_program_header.p_type == PT_LOAD && elf_program_header.p_flags == (PF_R|PF_X)) {
        uintptr_t addr = elf_program_header.p_vaddr;
        return start + (elf_header.e_entry - addr);
      }
    }
    printf("No program header of exec\n");
    return 0;
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

