#include "nemu.h"
#define MYDEBUG 0

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, 
  
  TK_INT,
  TK_REG,

  //TK_LP,
  TK_RP,

  TK_EQ,
  TK_NEQ,
  TK_AND,
  TK_OR,

  TK_LP,
  
  TK_PLUS,
  TK_MIN,
  TK_MUL,
  TK_DIV,

  TK_NOT,
  TK_NEG,
  TK_V,
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
 // {"\\-", TK_NEG},         // neg
  {"([0-9]+)|(0x[0-9a-fA-F]+)", TK_INT}, // int
  {"\\%[a-z]{3}", TK_REG},             // register
  {"\\(", TK_LP},         
  {"\\)", TK_RP}, 
  {"\\+", TK_PLUS},         // plus
  {"\\-", TK_MIN},         // min
  {"\\*", TK_MUL},         // mul
  {"\\/", TK_DIV},         // div
  {"\\=\\=", TK_EQ},        // equal
  {"\\!\\=", TK_NEQ},       // not equal
  {"\\&\\&", TK_AND},       // and
  {"\\|\\|", TK_OR},        // or 
  {"\\!", TK_NOT},        // not 
  //{"\\*", TK_V},          // value  

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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

Token tokens[32];
int nr_token;

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
        if (MYDEBUG)
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token].str));
        switch (rules[i].token_type) {
          case TK_NOTYPE:  break;
          //case TK_NEG : tokens[nr_token].type = TK_NEG; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_INT: tokens[nr_token].type = TK_INT; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_REG: tokens[nr_token].type = TK_REG; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_LP: tokens[nr_token].type = TK_LP; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_RP: tokens[nr_token].type = TK_RP; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_PLUS: tokens[nr_token].type = TK_PLUS; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_MIN: tokens[nr_token].type = TK_MIN; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_MUL: tokens[nr_token].type = TK_MUL; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_DIV: tokens[nr_token].type = TK_DIV; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_EQ: tokens[nr_token].type = TK_EQ; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_NEQ: tokens[nr_token].type = TK_NEQ; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_AND: tokens[nr_token].type = TK_AND; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_OR: tokens[nr_token].type = TK_OR; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          case TK_NOT: tokens[nr_token].type = TK_NOT; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          //case TK_V: tokens[nr_token].type = TK_V; memcpy(tokens[nr_token].str, substr_start, substr_len); nr_token++; break;
          default: assert(0);
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

void print_tokens(int p, int q) {
  if (MYDEBUG == 0) return ;
  printf("p, q =  %d %d\n", p, q);
  int i = 0;
  for (i = p; i<= q; i++) {
    printf("%s ", tokens[i].str);
  }
  printf("\n");
}

uint32_t get_num(char* a) {
  if (a[0] == '%') {
    if (strcmp(a, "%eax") == 0) return cpu.eax;
    else if (strcmp(a, "%ecx") == 0) return cpu.ecx;
    else if (strcmp(a, "%edx") == 0) return cpu.edx;
    else if (strcmp(a, "%ebx") == 0) return cpu.ebx;
    else if (strcmp(a, "%esp") == 0) return cpu.esp;
    else if (strcmp(a, "%ebp") == 0) return cpu.ebp;
    else if (strcmp(a, "%esi") == 0) return cpu.esi;
    else if (strcmp(a, "%edi") == 0) return cpu.edi;
    else if (strcmp(a, "%eip") == 0) return cpu.eip;
    else assert(0);
  } else if (a[0] == '0' && a[1] == 'x') {
    int n = 0;
    int i = 2;
    int t = 0;
    while (a[i] != '\0') {
      if (a[i] == ' ') {
        i++;
        continue;
      }
      if ('0' <= a[i] && a[i] <= '9') {
        t = a[i] - '0';
      } else if ('a' <= a[i] && a[i] <= 'f') {
        t = a[i] - 'a' + 10;
      } else if ('A' <= a[i] && a[i] <= 'F') {
        t = a[i] - 'A' + 10;
      }
      n = 16 * n + t;
      i++;
    }
    return n;
  } else {
    return (uint32_t) atoi(a);
  }
}

bool check_parentheses(p, q) {
  if (tokens[p].type != TK_LP || tokens[q].type != TK_RP) return false;
  if (MYDEBUG) printf("begin check_parentheses\n");
  print_tokens(p, q);
  int a[32];
  int left[32];
  
  memset(a, 0, sizeof(a));
  memset(left, 0, sizeof(left));
  int i = 0;
  int l = 0;
  for (i = p; i <= q; i++) {
    if (tokens[i].type == TK_LP) {
      left[++l] = i;
    } else if (tokens[i].type == TK_RP) {
      a[i] = left[l];
      a[left[l]] = i;
      l--;
    } 
  }

  if (a[p] == q && a[q] == p) {
    //printf("parenthese\n");
    return true;
  } else {
    return false;
  }
}

uint32_t get_op_pos(uint32_t p, uint32_t q) {
  print_tokens(p, q);
  int pos = -1;
  int kuohao = 0;
  int i = 0;
  for (i = p; i <= q; i++) {
    if (strcmp(tokens[i].str, "(") == 0) {
      kuohao++;
    } else if (strcmp(tokens[i].str, ")") == 0) {
      kuohao--;
    }
   // printf("luohao = %d\n", kuohao);
    if (kuohao != 0) continue;

    if (TK_EQ <= tokens[i].type && tokens[i].type <= TK_V) {
      if (pos == -1) {
        pos = i;
      } else {
        if ((tokens[pos].type == TK_NEG || tokens[pos].type == TK_NOT || tokens[pos].type == TK_V) && \
            (TK_EQ <= tokens[i].type && tokens[i].type <= TK_DIV)) {
          pos = i;
        } else if ((TK_MUL <= tokens[pos].type && tokens[pos].type <= TK_DIV) && \
                   (TK_EQ <= tokens[i].type && tokens[i].type <= TK_MIN)) {
          pos = i;
        } else if ((TK_PLUS <= tokens[pos].type && tokens[pos].type <= TK_MIN) && \
                   (TK_EQ <= tokens[i].type && tokens[i].type <= TK_OR)) {
          pos = i;
        }
      }
    }
  }
  if (MYDEBUG) printf("pos = %d\n", pos);
  if (pos < 0) assert(0);
  return pos;
}

uint32_t eval(uint32_t p, uint32_t q) {
  if (MYDEBUG) printf("eval\n");
  print_tokens(p, q);
  if (p > q) {
    /* Bad expression */
    assert(0);
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if (MYDEBUG) printf("get_num = %d\n", get_num(tokens[p].str));
    return get_num(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */

    return eval(p + 1, q - 1);
  }
  else {
    if (MYDEBUG) printf("begin get op\n");
    uint32_t op = get_op_pos(p, q);
    int op_type = tokens[op].type;
    if (MYDEBUG) printf("pos = %d %1X\n", op, op_type);
    if (op_type == TK_NOT || op_type == TK_NEG || op_type == TK_V) {
      uint32_t val2 = eval(op + 1, q);
      
      switch (op_type) {
        case TK_NOT: return (!val2);
        case TK_NEG: return (-val2);
        case TK_V:   return vaddr_read(val2, 0);
        default: assert(0);
      }
    }

    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);
    switch (op_type) {
      case TK_PLUS: return val1 + val2;
      case TK_MIN: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: if (val2 == 0) assert(0);
                return val1 / val2;
      case TK_EQ: return (val1 == val2);
      case TK_NEQ: return (val1 != val2);
      case TK_AND: return (val1 && val2);
      case TK_OR: return (val1 || val2);
      default: assert(0);
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // for the overload and single op //  TK_INT TK_REG,
  int i = 0;
  for (i = 0; i < nr_token; i ++) {
    if (tokens[i].type == TK_MIN && (i == 0 || ((TK_EQ <= tokens[i-1].type) && (tokens[i-1].type <= TK_V)))) {
      tokens[i].type = TK_NEG;
    } else if (tokens[i].type == TK_MUL && (i == 0 || ((TK_EQ <= tokens[i-1].type) && (tokens[i-1].type <= TK_V)))) {
      tokens[i].type = TK_V;
    }
  }
  /* TODO: Insert codes to evaluate the expression. */
  uint32_t ans = eval(0, nr_token-1);
  *success = true;
  return ans;
}
