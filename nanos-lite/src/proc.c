#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void context_kload(PCB * pcb, void(* entry)(void *), void * arg) {
  Area area;
  area.start = pcb->stack;
  area.end = pcb->stack + STACK_SIZE;
  pcb->cp = kcontext(area, entry, arg);
  printf("context_kload area:(%p, %p), cp:%p, entry:%p\n", area.start, area.end, pcb->cp, entry);
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  printf("hello fun\n");
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

extern void naive_uload(PCB *pcb, const char *filename); 
void init_proc() {
  context_kload(&pcb[0], hello_fun, NULL);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here

  // naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;
// always select pcb[0] as the new process
  current = &pcb[0];
  printf("schedule from %p to %p\n", prev, current->cp);
// then return the new context
  return current->cp;
}
