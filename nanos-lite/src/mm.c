#include <memory.h>
#include <stdio.h>
#include <proc.h>

static void *pf = NULL;
extern PCB *current;

void* new_page(size_t nr_page) {
  void * first = pf;
  pf = (void *)ROUNDUP(pf + nr_page * PGSIZE, PGSIZE);
  return first;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  int nr_page = n / PGSIZE;
  if(nr_page * PGSIZE < n)
    nr_page ++;
  void * buf = new_page(nr_page);
  memset(buf, 0, nr_page * PGSIZE);
  return buf;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  if(brk > current->max_brk) {
    int n = (brk - current->max_brk) / PGSIZE;
    if(brk > current->max_brk + n * PGSIZE) n ++;
    for(int i = 0; i < n; i ++, current->max_brk += PGSIZE) {
      void * pa = new_page(1);
      map(&current->as, (void*)current->max_brk, pa, 1);
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
