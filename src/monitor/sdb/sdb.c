#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/*watch point related*/
//extern WP;
WP * new_wp(word_t va,int v);
void free_wp(int n);
/*watch point related*/

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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

static int cmd_help(char *args);
static int cmd_z(char * args){
    printf("zzy new cmd\n");
    return 0;
}

static int cmd_si(char * args){
    cpu_exec(10);
    return 0;
}

static int cmd_info(char * args){
    isa_reg_display();
    return 0;
}

static int cmd_x(char * args){
    //printf("mem[]=%d\n",pmem[0]);
    word_t var = strtoul(args,NULL,16);
    word_t temp_info = vaddr_read(var,4);
    printf("temp_info=%x\n",temp_info);
    return 0;
}
static int cmd_p(char * args){
    return 0;
}

static int cmd_w(char * args){
    //stdlib.h
    word_t var = strtoul(args,NULL,16);
    printf("var=%x\n",var);
    int value = vaddr_read(var,4);
    printf("value=%d\n",value);
    WP * temp = new_wp(var,value);
    int number = temp->NO;
    printf("watch point number=%d\n",number);
    return 0;
}

static int cmd_d(char * args){
    int n = atoi(args);
    free_wp(n);
    
    printf("watch point number=%d\n",n);
    return 0;
}
static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "z", "zzy first cmd",cmd_z},
  { "si", "si cmd " ,cmd_si},
  { "info", "info cmd", cmd_info},
  { "p","p cmd",cmd_p},
  { "x","x--memory info",cmd_x},
  {"w","add watch point",cmd_w},
  {"d","delete watch point",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    printf("enter sdb_mainloop() is_batch_mode\n");
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
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

void init_sdb() {
  /* Compile the regular expressions. */
  //printf("now in init_sdb()\n");
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
