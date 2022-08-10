#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  word_t var;   //to be scaned address
  int old_value;

} WP;

#endif
