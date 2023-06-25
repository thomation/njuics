/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
struct sreg {
  uint32_t idx;
  char * name;
};
struct sreg sregs[] = {
  {0x300, "mstatus"},
  {0x305, "mtvec"},
  {0x340, "mscratch"},
  {0x341, "mepc"},
  {0x342, "mcause"},
};

static int regs_count() {
  return sizeof(regs) / sizeof(const char*);
}
static int sregs_count() {
  return sizeof(sregs) / sizeof(struct sreg);
}
void isa_reg_display() {
  printf("General Registers ========================\n");
  printf("PC: %x\n", cpu.pc);
  int len = regs_count();
  for(int i = 0; i < len; i ++ ) {
    if(i % 8 == 0)
      printf("\n");
    printf("%s: 0x%08x, \t", regs[i], cpu.gpr[i]);
  }
  printf("\nCSR ========================\n");
  len = sregs_count();
  for(int i = 0; i < len; i ++ ) {
    printf("%s: 0x%08x, \t", sregs[i].name, cpu.sr[sregs[i].idx]);
  }
  printf("\n========================\n");
}

word_t isa_reg_str2val(const char *s, bool *success) {
  if(strcmp(s, "pc") == 0) {
    *success = true;
    return cpu.pc;
  }
  int len = regs_count();
  for(int i = 0; i < len; i ++) {
    if(strcmp(s, regs[i]) == 0) {
      *success = true;
      return cpu.gpr[i];
    }
  }
  len = sregs_count();
  for(int i = 0; i < len; i ++) {
    if(strcmp(s, sregs[i].name) == 0) {
      *success = true;
      return cpu.sr[sregs[i].idx];
    }
  }
  *success = false;
  return 0;
}
