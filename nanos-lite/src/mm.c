#include <memory.h>
#include <stdio.h>

static void *pf = NULL;

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
  return new_page(nr_page);
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
