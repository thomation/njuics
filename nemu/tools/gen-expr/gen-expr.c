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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char *buf_cur;
static int token_num;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static int choose(int n) {
  int r = rand();
  return r % n; 
}
static void gen_num() {
  int r = rand() % 32768;
  if (r < 0) r = -r;
  char tmp[32];
  sprintf(tmp, "%d", r);
  int len = strlen(tmp);
  strcpy(buf_cur, tmp);
  buf_cur += len;
  token_num ++;
}
static void gen(char c) {
  *buf_cur ++ = c;
  token_num ++;
}
static void gen_rand_op() {
  switch(choose(4)) {
    case 0: gen('+'); break;
    case 1: gen('-'); break;
    case 2: gen('*'); break;
    default: gen('/'); break;
  }
}
static void gen_rand_expr() {
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_cur = buf;
    token_num = 0;
    gen_rand_expr();
    *buf_cur = '\0';
    if(token_num > 32)
      continue;
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if(fscanf(fp, "%d", &result))
      printf("%u=%s\n", result, buf);
    pclose(fp);
  }
  return 0;
}
