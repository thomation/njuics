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

#define IRQ_TIMER 0x80000007  // for riscv32
#define MIE_BIT 3
#define MIE_MASK (1 << MIE_BIT)
#define MPIE_BIT 7
#define MPIE_MASK (1 << MPIE_BIT)
#define MIE_TO_MPIE(v) ((v & ~MPIE_MASK) | ((v & MIE_MASK) << (MPIE_BIT - MIE_BIT)))
#define MPIE_TO_MIE(v) ((v & ~MIE_MASK) | ((v & MPIE_MASK) >> (MPIE_BIT - MIE_BIT)))

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  // printf("isa_raise_intr before close intr cpu.mstatus %x\n", cpu.mstatus);
  cpu.mstatus = MIE_TO_MPIE(cpu.mstatus);
  cpu.mstatus &= ~MIE_MASK;
  // printf("isa_raise_intr after close intr cpu.mstatus %x\n", cpu.mstatus);
#ifdef CONFIG_ETRACE_COND
  if (ETRACE_COND)
    log_write("isa_raise_intr@%x, cause:%x\n", epc, NO);
#endif
  cpu.mepc = epc;
  cpu.mcause= NO; 
  return cpu.mtvec;
}

word_t isa_query_intr() {
  if (cpu.INTR && cpu.mstatus & MIE_MASK) { // check INTR and mstatus.MIE
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
word_t isa_handle_mret() {
  word_t addr = cpu.mepc;
  addr += 4;
#ifdef CONFIG_ETRACE_COND
  if (ETRACE_COND)
    log_write("mret@%x, next:%x\n", cpu.pc, addr);
#endif
  // printf("isa_handle_mret before open intr cpu.mstatus %x\n", cpu.mstatus);
  cpu.mstatus = MPIE_TO_MIE(cpu.mstatus);
  cpu.mstatus |= MPIE_MASK;
  // printf("isa_handle_mret after open intr cpu.mstatus %x\n", cpu.mstatus);
  return addr;
}
