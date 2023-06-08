#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
// static int used_pbc_count = 0;
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
extern void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
#define EXEC_FILE "/bin/menu"
static char * const argv[] = {EXEC_FILE, NULL};
static char * const envp[] = {NULL};
void init_proc() {
  context_kload(&pcb[0], hello_fun, (void*)1);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // man execve, argv and envp must be terminated with NULL
  context_uload(&pcb[1], EXEC_FILE, argv, envp);

}
void create_proc(const char *filename, char *const argv[], char *const envp[]) {
  // if(used_pbc_count >= MAX_NR_PROC) {
  //   printf("Only support %d processes\n", MAX_NR_PROC);
  //   return;
  // }
  // Just reuse 1
  context_uload(&pcb[1], filename, argv, envp);
  switch_boot_pcb();
  yield();
}
Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;
// always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
// then return the new context
  printf("schedule from %p to %p\n", prev, current->cp);
  return current->cp;
}
