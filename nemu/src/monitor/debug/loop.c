#include "monitor/loop.h"

LOOP loops[MAX_LOOP_STEP];

uint32_t cur_num = 0;

void init_loops() {
	uint32_t i = 0;
	for (i = 0; i < MAX_LOOP_STEP; i++) {
		loops[i].times = 0;
	}
	cur_num = 0;
}

int cmp(const void *x, const void *y) {
  LOOP* a = (LOOP *) x;
  LOOP* b = (LOOP *) y;
 	if (a->tv.tv_sec != b->tv.tv_sec) {
		return a->tv.tv_sec < b->tv.tv_sec;
	} else if (a->tv.tv_usec != b->tv.tv_usec) {
		return a->tv.tv_usec < b->tv.tv_usec;
	} else {
		return a->times < b->times;
	}
}

/*
int cmp(const void *x, const void *y) {
  LOOP* a = (LOOP *) x;
  LOOP* b = (LOOP *) y;
  if (a->times != b->times) {
	return a->times < b->times;
  } else if (a->tv.tv_sec != b->tv.tv_sec) {
		return a->tv.tv_sec < b->tv.tv_sec;
	} else  {
		return a->tv.tv_usec < b->tv.tv_usec;
	}
}
*/

/* (a->tv.tv_usec != b->tv.tv_usec) */
bool new_eip(uint32_t cur_eip) {
	uint32_t i = 0;
	uint32_t flag = 0;
	for (i = 0; i < cur_num; i++) {
		if (loops[i].eip == cur_eip) {
			flag = 1;
			loops[i].times = loops[i].times + 1;
			gettimeofday(&(loops[i].tv),NULL);
			break;
		}
	}
	if (flag == 0) {
		if (cur_num >= MAX_LOOP_STEP) {
			loops[MAX_LOOP_STEP-1].eip = cur_eip;
			loops[MAX_LOOP_STEP-1].times = 1;
			gettimeofday(&(loops[MAX_LOOP_STEP-1].tv),NULL);
		} else {
			loops[cur_num].eip = cur_eip;
			loops[cur_num].times = 1;
			gettimeofday(&(loops[cur_num].tv),NULL);
			cur_num++;
		}
	}
	qsort(loops, cur_num, sizeof(LOOP), cmp);
	int times = 0;
	for (i = 0; i 	< cur_num; i++) {
		if (loops[i].times > times) {
			times = loops[i].times;
		}
	}
	//printf("times = %d, flag = %d, num = %d, eip = %0X, times = %d\n",times, flag, cur_num, loops[0].eip, loops[0].times);
	if (loops[0].times > MAX_LOOP_TIME) {
	return false;
	} else {
	return true;
	}
}
