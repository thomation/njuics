#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  // TODO: fix the order of these members to match trap.S
  union 
  {
    void *pdir; // pdir = gpr[0]
    uintptr_t gpr[32];
  };
  uintptr_t mcause;
  uintptr_t mstatus;
  uintptr_t mepc;
  uintptr_t np;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10] // a0
#define GPR3 gpr[11] // a1
#define GPR4 gpr[12] // a2
#define GPRx gpr[10] // a0
#define ARG1 gpr[10]
#define GPSP gpr[2] // sp

#endif
