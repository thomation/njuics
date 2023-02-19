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

#include "sdb.h"

#define NR_WP 32

struct watchpoint
{
  int NO;
  char expr[64];
  struct watchpoint *next;
};

static struct watchpoint wp_pool[NR_WP] = {};
static WP head = NULL, free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP new_wp(char *e)
{
  Assert(free_, "Only support %d watchpoints", NR_WP);
  WP wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  strncpy(wp->expr, e, 64);
  return wp;
}
void wp_info(WP wp)
{
  Assert(wp, "Empty wp");
  printf("wp no: %d, expr:%s\n", wp->NO, wp->expr);
}
void wp_list_info()
{
  WP wp = head;
  while(wp)
  {
    wp_info(wp);
    wp = wp->next;
  }
}
void free_wp(WP wp)
{
  TODO();
}
