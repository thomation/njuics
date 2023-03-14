
#include <common.h>

FILE *elf_fp = NULL;
void init_elf(const char *elf_file) {
  if (elf_file != NULL) {
    FILE *fp = fopen(elf_file, "r");
    Assert(fp, "Can not open '%s'", elf_file);
    elf_fp = fp;
    Log("Elf is from %s", elf_file);
  }
}