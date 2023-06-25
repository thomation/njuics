#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

extern void __am_get_cur_as(Context *c);
void __am_switch(Context *c); 

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    // printf("am_irq_handle:%d\n", c->mcause);
    if(c->mcause == -1)
      ev.event = EVENT_YIELD;
    else if(c->mcause >= 0 && c->mcause < 20)
      ev.event = EVENT_SYSCALL;
    else if(c->mcause == 0x80000007)
      ev.event = EVENT_IRQ_TIMER;
    else
      ev.event = EVENT_ERROR;

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  printf("Cte init\n");
  asm volatile("csrrw x0, mscratch, x0; csrw mtvec, %0" : : "r"(__am_asm_trap));
  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *context = (Context *)(kstack.end - sizeof(Context));
  context->mstatus = 0xff; // enable intr
  context->mcause = 0;
  context->mepc = (uintptr_t)entry;
  context->ARG1 = (uintptr_t)arg;
  context->pdir = NULL;
  printf("kcontext: mepc:%p\n", context->mepc);
  return context;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
