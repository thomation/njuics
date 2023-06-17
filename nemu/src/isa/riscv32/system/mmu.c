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
#include <memory/paddr.h>
#include <memory/vaddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  bool showlog = false;
  if(vaddr <= 0x80000000) showlog = false;
  if(showlog)
    printf("isa_mmu_trans: vaddr %x, len %d, type %d\n", vaddr, len, type);
  vaddr_t vpn1 = (vaddr >> 22) & 0x3ff;
  vaddr_t vpn0 = (vaddr >> 12) & 0x3ff;
  vaddr_t offset = vaddr &0xfff;
  uintptr_t ptr = (cpu.satp << 12);
  paddr_t * dir = (paddr_t*)ptr;
  if(showlog) {
    printf("is_mmu_trans: vpn1 %x, vpn0 %x, offset %x\n", vpn1, vpn0, offset);
    printf("is_mmu_trans: satp %x, dir %lx\n", cpu.satp, ptr);
  }
  uintptr_t pageaddr = (uintptr_t)(dir + vpn1);
  uintptr_t page = paddr_read((paddr_t)pageaddr, 4) & ~0x3ff;
  if(showlog)
    printf("isa_mmu_trans: page %lx @ %lx\n", page, pageaddr);
  uintptr_t pnnaddr = (uintptr_t)((paddr_t *)page + vpn0);
  paddr_t pnn = (paddr_read((paddr_t)pnnaddr, 4) & ~0x3ff) >> 10;
  if(showlog)
    printf("isa_mmu_trans: pnn %x @ %lx\n", pnn, pnnaddr);
  paddr_t paddr =  pnn << 12 | offset;
  if(showlog)
    printf("isa_mmu_trans: paddr %x\n", paddr);
  return paddr;
}
