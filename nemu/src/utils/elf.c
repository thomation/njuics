
#include <common.h>
#include <elf.h>

static Elf32_Ehdr elf_header;
static Elf32_Shdr section_header;
void init_elf(const char *elf_file) {
  if (elf_file != NULL) {
    FILE *fp = fopen(elf_file, "r");
    Assert(fp, "Can not open '%s'", elf_file);
    Log("Elf is from %s", elf_file);
    int ret = fread(&elf_header, sizeof(Elf32_Ehdr), 1, fp);
    if(ret != 1) {
      Log("Read Elf error\n");
      return;
    }
    Assert(elf_header.e_ident[EI_MAG0] == ELFMAG0 &&
           elf_header.e_ident[EI_MAG1] == ELFMAG1 &&
           elf_header.e_ident[EI_MAG2] == ELFMAG2 &&
           elf_header.e_ident[EI_MAG3] == ELFMAG3, "Not a elf file!\n");
    fseek(fp, elf_header.e_shoff, SEEK_SET);
    for(int i = 0; i < elf_header.e_shnum; i ++) {
      ret = fread(&section_header, elf_header.e_shentsize, 1, fp);
      if(ret != 1) {
        Log("Read section %d error\n", i);
        return;
      }
      if(section_header.sh_type == SHT_SYMTAB)
        break;
    }
    fclose(fp);
  }
}