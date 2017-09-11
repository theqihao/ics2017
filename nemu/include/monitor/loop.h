#ifndef __LOOP_H__
#define __LOOP_H__

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "common.h"

#define MAX_LOOP_TIME 0x0000fff
#define MAX_LOOP_STEP 0x0000ffff

typedef struct {
	uint32_t times;
	uint32_t eip;
	struct timeval tv;
} LOOP;

void init_loops();
int cmp(const void *x, const void *y);
bool new_eip(uint32_t cur_eip);

#endif