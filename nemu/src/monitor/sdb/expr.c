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
#include <memory/vaddr.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NUM,
  TK_REG,
  TK_DEREF,
  TK_MINUS,
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
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"==", TK_EQ},        // equal
  {"\\(", '('},
  {"\\)", ')'},
  {"0x[0-9|a-f]+|[0-9]+", TK_NUM},
  {"\\$\\w+", TK_REG}
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
  t->str[len] = '\0';
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
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
            add_token(rules[i].token_type);
            break;
          case TK_NUM:
          case TK_REG:
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
static word_t str_to_int(char* s, bool *success) {
    word_t num;
    if(strlen(s) >= 2 && s[0] == '0' && s[1] == 'x')
      sscanf(s, "%x", &num);
    else
      sscanf(s, "%d", &num);
    *success = true;
    return num;
}
static word_t token_value(int p, bool *success) {
    Token *t = &tokens[p];
    char * s = t->str;
    switch(t->type) {
      case TK_NUM:
        return str_to_int(s, success);
      case TK_REG:
        word_t v = isa_reg_str2val(s + 1, success);
        if(*success == false) {
          printf("Invalid reg name:%s\n", s + 1);
        }
        return v;
      default:
        printf("No support token: %d, %s\n", t->type, s);
        *success = false;
    }
    return 0;
}
bool check_parentheses(int p, int q) {
  if(!(tokens[p].type == '(' && tokens[q].type == ')'))
    return false;
  int lb = 0;
  for(int i = p + 1; i <= q - 1; i ++) {
    if(tokens[i].type == '(') lb ++;
    if(tokens[i].type == ')') lb --;
    if(lb < 0)
      return false;
  }
  return true;
}
typedef int Ops[5];
// 0 is the end of array
static Ops op_pri[] = {
  {TK_DEREF, 0},
  {'*', '/', 0},
  {'+', '-', 0},
};
static int find_op_pri(int op) {
  int len = sizeof(op_pri) / sizeof(Ops);
  for(int i = 0; i < len; i++) {
    for(int j = 0; op_pri[i][j] != 0; j ++) {
      if(op_pri[i][j] == op)
        return i;
    }
  }
  return -1;
}
static int find_op(int p, int q, bool *success) {
  int last_pri = -1;
  int last_op_index = -1;
  int bcount = 0;
  // printf("find_op: %d to %d\n", p, q);
  for(int i = p; i <= q; i ++) {
    int op = tokens[i].type;
    switch (op) {
      case '+':
      case '-':
      case '*':
      case '/':
      case TK_DEREF: 
        int pri = find_op_pri(op);
        if(pri >= last_pri && bcount == 0) {
          last_op_index = i;  
          last_pri = pri;
        }
        break;
      case '(':
        bcount ++;
        break;
      case ')':
        bcount --;
        break;
    }
  }
  *success = last_op_index >= 0 && bcount == 0;
  // printf("find_op result:%d\n", op);
  return last_op_index;
}
static int operand_count(int op) {
  if(op == TK_DEREF)
    return 1;
  return 2;
}
word_t eval(int p, int q, bool * success); 
static word_t eval_uniq(int op, int q, bool *success) {
  word_t rv = eval(op + 1, q, success);
  switch(tokens[op].type) {
    case TK_DEREF:
      printf("deref %u = ?\n", rv);
      if(!*success) {
        return 0;
      }
      return vaddr_read(rv, sizeof(word_t));
    default:
      *success = false;
      printf("Does not support:%d\n", tokens[op].type);
  }
  return 0;
}
static word_t eval_binary(int p, int op, int q, bool *success) {
    word_t lv = eval(p, op - 1, success);
    if(!*success) {
      return 0;
    }
    word_t rv = eval(op + 1, q, success);
    printf("%d %c %d = ?\n", lv, tokens[op].type, rv);
    if(!*success) {
      return 0;
    }
    switch(tokens[op].type) {
      case '+': return lv + rv; 
      case '-': return lv - rv;
      case '*': return lv * rv;
      case '/': return lv / rv;
      default:
        TODO();
    }
    *success = false;
    return 0;
}
word_t eval(int p, int q, bool * success) {
  if(p > q) {
    *success = false;
    return 0;
  } else if(p == q) {
    return token_value(p, success);
  } else if (check_parentheses(p, q)){
    return eval(p + 1, q - 1, success);
  } else {
    int op = find_op(p, q, success);
    if(!*success) {
      return 0;
    }
    int op_cout = operand_count(tokens->type);
    switch (op_cout) {
    case 1:
      return eval_uniq(op, q, success);
    case 2: 
      return eval_binary(p, op, q, success);
    default:
      printf("Does not support %d operands\n", op_cout);
      *success = false;
      break;
    }
  }
  return 0;
}

static bool is_operand(int p) {
  if(p >= nr_token)
    return false;
  Token *t = &tokens[p];
  return t->type == TK_NUM || t->type == TK_REG || t->type == '(' || t->type == ')';
}
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || !is_operand(i - 1)) ) {
      tokens[i].type = TK_DEREF;
    }
  }
  return eval(0, nr_token - 1, success);
}
