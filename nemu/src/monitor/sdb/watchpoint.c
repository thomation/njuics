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
  word_t snapshot;
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
  bool suc;
  word_t v = expr(e, &suc);
  Assert(suc, "Invalid expr:%s\n", e);
  WP wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  strncpy(wp->expr, e, 64);
  wp->snapshot = v;
  return wp;
}
void wp_info(WP wp)
{
  Assert(wp, "Empty wp");
  printf("wp no: %d, expr:%s, snapsot:%u\n", wp->NO, wp->expr, wp->snapshot);
}
void wp_list_info()
{
  WP wp = head;
  while (wp)
  {
    wp_info(wp);
    wp = wp->next;
  }
}
static WP find_wp(int n)
{
  WP wp = head;
  while (wp)
  {
    if (wp->NO == n)
      return wp;
    wp = wp->next;
  }
  return NULL;
}
static void remove_wp(WP wp)
{
  if (head == wp)
  {
    head = wp->next;
  }
  else
  {
    WP pre = head;
    while (pre && pre->next != wp)
    {
      pre = pre->next;
    }
    pre->next = wp->next;
  }
  // Insert into free
  wp->next = free_;
  free_ = wp;
}
void free_wp(int n)
{
  WP wp = find_wp(n);
  if (!wp)
  {
    printf("Cannot find watchpoint of number:%d\n", n);
    return;
  }
  wp_info(wp);
  remove_wp(wp);
}
bool check_wp()
{
  WP wp = head;
  while (wp)
  {
    bool suc;
    word_t v = expr(wp->expr, &suc);
    if(!suc || v != wp->snapshot)
    {
      printf("Catch watchpoint new value:%u\n", v);
      wp_info(wp);
      return true;
    }
    wp = wp->next;
  }
  return false;
}
