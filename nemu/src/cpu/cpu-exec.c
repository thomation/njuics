/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10

#define LOG_SIZE 128
#define IRINGBUF_SIZE 16
typedef struct _logbuf {
    char str[LOG_SIZE];
} logbuf;
typedef struct _iringbuf {
    logbuf contents[IRINGBUF_SIZE];
    int next_index;
    int count;
    int error_index;
} iringbuf;

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;
static iringbuf g_iringbuf;
void device_update();
extern bool check_wp();
extern char* find_symbol(uint32_t addr);

void append_log_to_iringbuf(const char * log) {
    if(nemu_state.state == NEMU_ABORT || nemu_state.halt_ret != 0) {
      g_iringbuf.error_index = g_iringbuf.next_index;
    }
    strncpy(g_iringbuf.contents[g_iringbuf.next_index].str, log, LOG_SIZE);
    g_iringbuf.count ++;
    g_iringbuf.next_index ++;
    if(g_iringbuf.next_index >= IRINGBUF_SIZE)
        g_iringbuf.next_index = 0;
}
static void pring_one_iringbuf_log(int index) {
#ifdef CONFIG_ITRACE_COND
    bool is_error = index == g_iringbuf.error_index;
    char * s = g_iringbuf.contents[index].str;
  if (ITRACE_COND) { log_write(is_error ? "--->%s\n" :"%s\n", s);}
#endif
  IFDEF(CONFIG_ITRACE, puts(s));
}
static void print_iringbuf_log() {
    if(g_iringbuf.count > IRINGBUF_SIZE) {
        for(int i = g_iringbuf.next_index; i < IRINGBUF_SIZE; i ++) {
            pring_one_iringbuf_log(i);
        }
    }
    for(int i = 0; i < g_iringbuf.next_index; i ++) {
        pring_one_iringbuf_log(i);
    }
}
static int call_deep = 0;
static void print_call_deep() {
  for(int i = 0; i < call_deep; i ++) 
    log_write("----");
}
void print_call(vaddr_t addr, vaddr_t pc) {
    char * name = find_symbol(addr);
    if(name != NULL && ITRACE_COND) {
        print_call_deep();
        log_write("call %s@%x\n", name, pc);
        call_deep ++;
    }
}
static void trace_func(Decode *_this) {
  return;
#ifdef CONFIG_ITRACE_COND
  char * code = _this->logbuf + 24;
  bool is_jal = code[0] == 'j' && code[1] == 'a' && code[2] == 'l';
  bool is_jalr = code[0] == 'j' && code[1] == 'a' && code[2] == 'l' && code[3] == 'r';
  bool is_ret = code[0] == 'r' && code[1] == 'e' && code[2] == 't';
  if(is_jalr) {
    char reg[5];
    sscanf(code + 5, "%s", reg);
    // Log("reg:%s", reg);
    bool success;
    word_t v = isa_reg_str2val(reg, &success);
    if(success)
      print_call(v, _this->pc);
  } else if(is_jal) {
    vaddr_t addr;
    // Log("code:%s, addr:%s",code,  code + 4);
    sscanf(code + 4, "%x", &addr);
    print_call(addr, _this->pc);

  } else if(is_ret && ITRACE_COND) {
      print_call_deep();
      log_write("ret @%x\n", _this->pc);
      call_deep --;
  }
#endif
}

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
  append_log_to_iringbuf(_this->logbuf);
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
  trace_func(_this);
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
#endif
}

static void execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    if(check_wp()) {
      nemu_state.state = NEMU_STOP;
    }
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
    // check interupt
    word_t intr = isa_query_intr();
    if (intr != INTR_EMPTY) {
      cpu.pc = isa_raise_intr(intr, cpu.pc);
    }
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);
  g_iringbuf.count = g_iringbuf.next_index = g_iringbuf.error_index = 0;
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
        if(nemu_state.state == NEMU_ABORT || nemu_state.halt_ret != 0) {
          Log("%s Print iringbuf here.\n", ANSI_FMT("ERROR!", ANSI_FG_RED) );
          print_iringbuf_log();
        }
      // fall through
    case NEMU_QUIT:
      statistic();
  }
}
