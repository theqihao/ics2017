#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *arg) {
  if (arg == NULL) {
    cpu_exec(1);
  } else {
    int n = atoi(arg);
    cpu_exec(n);
  }
  return 0;
}

static int cmd_info(char *arg) {
  if (arg[0] == 'r') {
    printf("eip : %X\n", cpu.eip);
    printf("flags : %X\n", cpu.flags);
    printf("CF: %d ZF: %d SF: %d IF: %d OF: %d\n", cpu.CF, cpu.ZF, cpu.SF, cpu.IF, cpu.OF);
    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    printf("width\t|eax       |ecx       |edx       |ebx       |esp       |ebp       |esi       |edi       |\n");
    //printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    int i;
    printf("32\t|");
    for (i = 0; i < 8; i++) {
      printf("0x%08X|", cpu.gpr[i]._32);
    }
    printf("\n");
    //printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    printf("16\t|");
    for (i = 0; i < 8; i++) {
      printf("0x%04X    |", cpu.gpr[i]._16);
    }
    printf("\n");
    //printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    printf("h_8|l_8\t|");
    for (i = 0; i < 8; i++) {
      printf("0x%02X |0x%02X|", cpu.gpr[i]._8[1], cpu.gpr[i]._8[0]);
    }
    printf("\n");
    //printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
  } else if (arg[0] == 'w') {
    show_wp();
  } else {
    printf("Usage: info r : registers, info w : watchpoint\n");
  }
  return 0;
}

static int cmd_x(char *arg) {
  char a[10] = {'\0'};
  int i = 0;
  int j = 0;
  int num = 0;
  while (arg[i] == ' ') {
    i++;
  }
  for (; i < strlen(arg); i++) {
    if (arg[i] == ' ') {
      num = atoi(a);
      while (arg[i] != ' ') {
        i++;
      }
      break;
    } else {
      a[j++] = arg[i];
    }
  }
  //printf("%d %s\n", n, &(arg[i]));
  //uint32_t addr = atoi(arg[i], 16);
  bool sucess = -1;
  char* p = &arg[i];
  uint32_t addr = expr(p, &sucess);
  if (sucess == false) {
    printf("expr hander error\n");
    return 0;
  }
  //printf("%d %08X\n",  n, addr);
  for (i = 0; i < num; i++) {
    printf("0x%08X\t", vaddr_read(addr+4*i, 0));
    if ((i+1) % 4 == 0) printf("\n");
  }
  printf("\n");
  return 0;
}

static int cmd_p(char *args) {
  printf("%s\n", args);
  bool sucess = -1;
  uint32_t ans =  expr(args, &sucess);
  if (sucess == false) {
    printf("expr hander error\n");
    return 0;
  }
  printf("%d\n", ans);
  printf("%08X\n", ans);
  return 0;
}

static int cmd_w(char *args) {
  return add_wp(args);
}

static int cmd_d(char *args) {
  del_wp(args);
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "h", " Display informations about all supported commands", cmd_help },
  { "c", " Continue the execution of the program", cmd_c },
  { "q", " Exit NEMU", cmd_q },
  { "si", "si [n] : exe n times, default is 1", cmd_si },
  { "i", " i r : registers, i w : watchpoint", cmd_info},
  { "x", " x n expr : show the continue 4*n size memory begin with expr", cmd_x },
  { "p", " p expr : show the ans of expr", cmd_p },
  { "w", " w expr : add  watchpoint", cmd_w },
  { "d", " d no : add  watchpoint", cmd_d },
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
