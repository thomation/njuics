
#include <common.h>
#include <elf.h>
#define SYMBOL_NAME_LEN 16
#define SYMBOL_NAME_LIST_SIZE 32
typedef char symbol_name [SYMBOL_NAME_LEN];
symbol_name g_symbol_name_list[SYMBOL_NAME_LIST_SIZE];
static int symbol_name_index = 0;
void parse_symbol_header(FILE *fp, Elf32_Shdr header) {
  int size = header.sh_size / sizeof(Elf32_Sym);
  Log("There are %d symbols", size);
  int offset = header.sh_offset;
  fseek(fp, offset, SEEK_SET);
  Elf32_Sym sym;
  for(int i = 0; i < size; i ++) {
    int ret = fread(&sym, sizeof(Elf32_Sym), 1, fp);
    if(ret != 1) {
      Log("Error on parse symbol");
      continue;
    }
    // if(ELF32_ST_TYPE(sym.st_info) == STT_FUNC)
      Log("symbol:%d, size:%d, type:%d\n", sym.st_name, sym.st_size, ELF32_ST_TYPE(sym.st_info));
  }
}
void parse_string_header(FILE *fp, Elf32_Shdr header) {
  int size = header.sh_size;
  int offset = header.sh_offset;
  fseek(fp, offset, SEEK_SET);
  int j = 0;
  for(int i = 0; i < size; i ++) {
    char c = fgetc(fp);
    // Log("%c", c);
    Assert(symbol_name_index < SYMBOL_NAME_LIST_SIZE, "Symbol overflow");
    if(j < SYMBOL_NAME_LEN)
      g_symbol_name_list[symbol_name_index][j ++] = c;
    if(c == '\0') {
      j = 0;
      g_symbol_name_list[symbol_name_index][SYMBOL_NAME_LEN - 1] = 0;
      Log("str:%s", g_symbol_name_list[symbol_name_index]);
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