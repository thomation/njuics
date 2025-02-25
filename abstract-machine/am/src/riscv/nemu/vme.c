#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);
  printf("vme_init: ptr %p\n", kas.ptr);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    printf("map start %p, end %p\n", segments[i].start, segments[i].end);
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }
  printf("vme_init map finished\n");
  // uint32_t * dir = (uint32_t*)kas.ptr;
  // for(int i = 0; i < PGSIZE / sizeof(uint32_t); i ++) {
  //   if(i % 16 == 0)
  //     printf("\n");
  //   printf("%p:%x,", dir + i, *(dir + i));
  // }
  set_satp(kas.ptr);
  printf("vme_init set satp finished\n");
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // if(prot == 1)
  //   printf("map from va %p to pa %p dir %p\n", va, pa, as->ptr);
  uintptr_t vpn1 = (((uintptr_t) va) >> 22) & 0x3ff;
  uintptr_t vpn0 = (((uintptr_t) va) >> 12) & 0x3ff;
  uintptr_t pnn = (((uintptr_t) pa) >> 12) & 0xfffff;
  uintptr_t *dir = (uintptr_t *)as->ptr;
  uintptr_t mode = 0x3ff;
  if(!dir[vpn1]) {
    uintptr_t np = (uintptr_t)pgalloc_usr(PGSIZE);
    dir[vpn1] = np | mode;
  }
  uintptr_t * table = (uintptr_t *)(dir[vpn1] &~mode);
  table[vpn0] = pnn << 10 | mode;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *context = (Context *)(kstack.end - sizeof(Context));
  context->mstatus = 0xff; // enable intr
  context->mcause = 0;
  context->mepc = (uintptr_t)entry;
  context->pdir = as->ptr;
  context->np = 0;
  return context;
}
