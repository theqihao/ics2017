#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int value;
  char str[32];

} WP;

WP* new_wp();
void free_wp(WP *wp);

int add_wp(char* args);
int show_wp();
int del_wp(char* args);

int check_wp();

#endif
