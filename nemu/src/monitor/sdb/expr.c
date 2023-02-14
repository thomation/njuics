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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"[0-9]+", TK_NUM}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static void add_token(int token_type) {
  Token *t = &tokens[nr_token ++];
  t->type = token_type;
}
static void add_token_with_str(int token_type, char* start, int len) {
  Token *t = &tokens[nr_token ++];
  t->type = token_type;
  strncpy(t->str, start, len);
}
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\", type %d at position %d with len %d: %.*s",
            i, rules[i].regex, rules[i].token_type, position, substr_len, substr_len, substr_start);

        position += substr_len;

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_EQ:
          case '+':
            add_token(rules[i].token_type);
            break;
          case TK_NUM:
            add_token_with_str(rules[i].token_type, substr_start, substr_len);
            break;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
int find_op(int p, int q, bool *success) {
  int pri = 0;
  int op = -1;
  for(int i = p; i <= q; i ++) {
    switch (tokens[i].type) {
      case '+':
      case '-':
        if(pri < 2) {
          op = i;
          pri = 2;
        }
        break;
      case '*':
      case '/':
        if(pri < 1) {
          op = i;
          pri = 1;
        }
        break;
    }
  }
  *success = op >= 0;
  return op;
}
word_t eval(int p, int q, bool * success) {
  if(p > q) {
    *success = false;
    return 0;
  } else if(p == q) {
    char * s = tokens[p].str;
    word_t num;
    sscanf(s, "%d", &num);
    *success = true;
    return num;
  } else {
    int op = find_op(p, q, success);
    if(!*success) {
      return 0;
    }
    word_t lv = eval(p, op - 1, success);
    if(!*success) {
      return 0;
    }
    word_t lr = eval(op + 1, q, success);
    if(!*success) {
      return 0;
    }
    switch(tokens[op].type) {
      case '+':
        return lv + lr; 
      default:
        TODO();
    }
  }
  return 0;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  return eval(0, nr_token - 1, success);
}
