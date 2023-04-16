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
#include <common.h>
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
#ifdef CONFIG_ETRACE_COND
  if (ETRACE_COND)
    log_write("isa_raise_intr@%x, cause:%d\n", epc, NO);
#endif
  cpu.mepc = epc;
  cpu.mcause= NO; 
  return cpu.mtvec;
}

word_t isa_query_intr() {
  word_t addr = cpu.mepc;
  addr += 4;
#ifdef CONFIG_ETRACE_COND
  if (ETRACE_COND)
    log_write("isa_query_intr@%x, next:%x\n", cpu.pc, addr);
#endif
  return addr;
}
