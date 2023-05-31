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
  printf("context_kload area:(%p, %p), cp:%p, entry:%p, of %p\n", area.start, area.end, pcb->cp, entry, pcb);
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    printf("hello fun pcb 0: %p, pcb 1:%p, stack:%p\n", pcb[0].cp, pcb[1].cp, &j);
    j ++;
    yield();
  }
}

extern void naive_uload(PCB *pcb, const char *filename); 
void init_proc() {
  context_kload(&pcb[0], hello_fun, (void*)1);
  context_kload(&pcb[1], hello_fun, (void*)2);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here

  // naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;
// always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
// then return the new context
  return current->cp;
}
