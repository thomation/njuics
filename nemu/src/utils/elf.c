
#include <common.h>
#include <elf.h>
void parse_symbol_header(FILE *fp, Elf32_Shdr header) {

}
void parse_string_header(FILE *fp, Elf32_Shdr header) {
  int size = header.sh_size;
  int offset = header.sh_offset;
  fseek(fp, offset, SEEK_SET);
  char str[128] = {'a', 'b', 'c', '\0'};
  int j = 0;
  for(int i = 0; i < size; i ++) {
    char c = fgetc(fp);
    // Log("%c", c);
    str[j ++] = c;
    if(c == '\0') {
      j = 0;
      Log("str:%s", str);
    }
  }
}
void init_elf(const char *elf_file) {
  if (elf_file != NULL) {
    FILE *fp = fopen(elf_file, "r");
    Assert(fp, "Can not open '%s'", elf_file);
    Log("Elf is from %s", elf_file);
    Elf32_Ehdr elf_header;
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
      Elf32_Shdr section_header;
      ret = fread(&section_header, elf_header.e_shentsize, 1, fp);
      if(ret != 1) {
        Log("Read section %d error\n", i);
        return;
      }
      if(section_header.sh_type == SHT_SYMTAB)
        parse_symbol_header(fp, section_header);
      else if(section_header.sh_type == SHT_STRTAB)
        parse_string_header(fp, section_header);
    }
    fclose(fp);
  }
}