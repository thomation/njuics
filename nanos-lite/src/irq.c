#include <common.h>
#include "syscall.h"

extern void do_syscall(Context *c); 
extern Context* schedule(Context *prev);
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:
      printf("Hello yield\n");
      return schedule(c);;
    case EVENT_SYSCALL:
      do_syscall(c);
      break;
    case EVENT_IRQ_TIMER:
      printf("Timer intr\n");
      return schedule(c);
    default:
      panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
