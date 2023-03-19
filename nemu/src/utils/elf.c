
#include <common.h>
#include <elf.h>
#define SYMBOL_NAME_LEN 16
#define SYMBOL_LIST_SIZE 32
#define STRING_TABLE_COUNT 2
typedef char symbol_name [SYMBOL_NAME_LEN];
struct symbol {
  Elf32_Word string_index;
  symbol_name name;
  Elf32_Addr addr;
};
struct symbol symbol_list[SYMBOL_LIST_SIZE];
static int symbol_size = 0;
static Elf32_Shdr symbol_table_header;
static Elf32_Shdr string_table_headers[STRING_TABLE_COUNT];
static int string_table_size = 0;

static void find_symbol_string(FILE *fp, int index, symbol_name name) {
  for(int tab = 0; tab < string_table_size; tab ++ ) {
    int size = string_table_headers[tab].sh_size;
    if(size <= index)
      continue;
    int offset = string_table_headers[tab].sh_offset;
    fseek(fp, offset + index, SEEK_SET);
    for(int i = 0; i < SYMBOL_NAME_LEN; i ++) {
      char c = fgetc(fp);
      // Log("%c of %d", c, index);
      name[i] = c;
      if(c == '\0') {
        name[SYMBOL_NAME_LEN - 1] = '\0';
        return;
      }
    }
  }
  name[SYMBOL_NAME_LEN - 1] = '\0';
}
static void parse_symbol_header(FILE *fp) {
  int size = symbol_table_header.sh_size / sizeof(Elf32_Sym);
  Log("There are %d symbols", size);
  int offset = symbol_table_header.sh_offset;
  fseek(fp, offset, SEEK_SET);
  Elf32_Sym sym;
  for(int i = 0; i < size; i ++) {
    int ret = fread(&sym, sizeof(Elf32_Sym), 1, fp);
    if(ret != 1) {
      Log("Error on parse symbol");
      continue;
    }
    if(ELF32_ST_TYPE(sym.st_info) == STT_FUNC) {
      Assert(symbol_size < SYMBOL_LIST_SIZE, "Symbol overflow");
      struct symbol * ps = &symbol_list[symbol_size ++];
      ps->addr = sym.st_value;
      ps->string_index = sym.st_name;
    }
  }
}
static void parse_symbole_name(FILE *fp) {
  for(int i = 0; i < symbol_size; i ++) {
      struct symbol * ps = &symbol_list[i];
      find_symbol_string(fp, ps->string_index, ps->name);
      // Log("symbol %d, sindex:%d, name:%s, addr:%x", i, ps->string_index, ps->name, ps->addr);
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
        symbol_table_header = section_header;
      else if(section_header.sh_type == SHT_STRTAB)
        string_table_headers[string_table_size ++] = section_header;
    }
    // print_string_tab(fp);
    parse_symbol_header(fp);
    parse_symbole_name(fp);
    fclose(fp);
  }
}
char* find_symbol(uint32_t addr) {
  for(int i = 0; i < symbol_size; i ++) {
    struct symbol *ps = &symbol_list[i];
    if(ps->addr == addr) {
      return ps->name;
    }
  }
  return NULL;
}