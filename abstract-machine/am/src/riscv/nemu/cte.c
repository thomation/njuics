#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    // printf("am_irq_handle:%d\n", c->mcause);
    if(c->mcause == -1)
      ev.event = EVENT_YIELD;
    else if(c->mcause >= 0 && c->mcause < 20)
      ev.event = EVENT_SYSCALL;
    else
      ev.event = EVENT_ERROR;

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *context = (Context *)(kstack.end - sizeof(Context));
  context->mstatus = 0;
  context->mcause = 0;
  context->mepc = (uintptr_t)entry;
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
