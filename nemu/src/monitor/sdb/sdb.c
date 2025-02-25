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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_help(char *args);
static int cmd_c(char *args);
static int cmd_q(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_si(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct
{
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"info", "Print programe state:(r)egister or (w)atch", cmd_info},
    {"x", "Print N * 4 bytes memory value.", cmd_x},
    {"si", "Run [N] Steps", cmd_si},
    {"p", "Compute expression", cmd_p},
    {"w", "Add watchpoint at expression", cmd_w},
    {"d", "Delete the [N]th watchpoint", cmd_d},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  NEMUTRAP(cpu.pc, 0);
  return -1;
}

static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if (!arg)
  {
    printf("Usage: info -r or -w\n");
  }
  else if (strcmp(arg, "-r") == 0)
  {
    isa_reg_display();
  }
  else if (strcmp(arg, "-w") == 0)
  {
    wp_list_info();
  }
  else
  {
    printf("Arg %s is not supported\n", arg);
  }
  return 0;
}
static int cmd_x(char *args)
{
  char *sn = strtok(NULL, " ");
  char *saddr = strtok(NULL, " ");
  if (sn == NULL || saddr == NULL)
  {
    printf("Usage: x n addr\n");
    return 0;
  }
  int n = 0;
  if (!sscanf(sn, "%d", &n))
  {
    printf("%s is not a number!\n", sn);
    return 0;
  }
  bool suc;
  vaddr_t addr = expr(saddr, &suc);
  if (!suc)
  {
    printf("Invalid address:%s\n", saddr);
    return 0;
  }

  printf("%d values from %#x is:\n", n, addr);
  for (int i = 0; i < n; i++)
  {
    uint8_t *host_addr = guest_to_host(addr + i * 4);
    printf("%#x|", *(uint32_t *)host_addr);
  }
  printf("\n");
  return 0;
}
int cmd_si(char *args)
{
  int n = 1;
  char *sn = strtok(NULL, " ");

  if (sn != NULL && !sscanf(sn, "%d", &n))
  {
    printf("%s is not a number!\n", sn);
    return 0;
  }
  cpu_exec(n);
  return 0;
}
int cmd_p(char *args)
{
  bool suc;
  word_t v = expr(args, &suc);
  if (suc)
  {
    printf("%u:=%s\n", v, args);
  }
  else
  {
    printf("?:=%s\n", args);
  }
  return 0;
}
int cmd_w(char *args)
{
  char *e = strtok(NULL, " ");
  if (!e)
  {
    printf("Usage: w [expr]\n");
    return 0;
  }
  bool suc;
  expr(e, &suc);
  if (!suc)
  {
    printf("Invalid expr:%s\n", e);
    return 0;
  }
  WP wp = new_wp(e);
  wp_info(wp);
  return 0;
}
int cmd_d(char *args)
{
  char *sn = strtok(NULL, " ");
  int n;
  if (sn != NULL && !sscanf(sn, "%d", &n))
  {
    printf("Usage: d [N]\n");
    return 0;
  }
  free_wp(n);
  return 0;
}
void sdb_set_batch_mode()
{
  is_batch_mode = true;
}

void sdb_mainloop()
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb()
{
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
