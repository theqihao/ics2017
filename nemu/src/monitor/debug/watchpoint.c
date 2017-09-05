#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

int add_wp(char* args) {
  WP* p = new_wp();
  printf("%d \n", p->NO);
  memcpy(p->str, args, strlen(args));
  bool sucess = -1;
  p->value = expr(p->str, &sucess);
  if (sucess == true) {
    return 0;
  } else {
    return -1;
  }
}

int show_wp() {
	if (head == NULL) {
		printf("empty\n");
		return 0;
	}
  printf("NO\tvalue\texpr\n");
  WP* p = head->next;
  while (p != NULL) {
    printf("%d\t%d\t%s\n", p->NO, p->value, p->str);
    p = p->next;
  }
  return 0;
}

int del_wp(char* args) {
	if (head == NULL) {
		printf("empty\n");
		return 0;
	}
  int n = atoi(args);
  WP* p = head->next;
  WP* q = head;
  while (p != NULL && p->NO != n) {
    q = p;
    p = p->next;
  }
  if (p == NULL) {
    assert(0);
  } else if (p->next == NULL) {
    q->next = NULL;
    free_wp(p);
  } else {
    q->next = p->next;
    free_wp(p);
  }
  return 0;
}


// new_wp()从free_链表中返回一个空闲的监视点结构
WP* new_wp() {
	WP* h = head;
	if (free_ == NULL)  assert(0);
	if (h == NULL) {
		head = free_;
		free_ = free_->next;
		head->next = free_;
		free_ = free_->next;
		h = head->next;
		h->next = NULL;
	} else {
		while (h->next != NULL) {
			h = h->next;
		}
		h->next = free_;
		free_ = free_->next;
		h = h->next;
		h->next = NULL;
	}
	return h;
}

// free_wp()将wp归还到free_链表
void free_wp(WP *wp) {
	WP* p = free_;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = wp;
	p->next->next = NULL;
}

int check_wp() {
	if (head == NULL) {
		return 0;
	}
	WP* p = head->next;
	int flag = 0;
	while (p != NULL) {
	  bool sucess = -1;
	  int ans =  expr(p->str, &sucess);
	  if (ans != p->value) {
	    printf("%08X --> %08X\t%s\n", p->value, ans, p->str);
	    p->value = ans;
	    flag = 1;
	  }
	  p = p->next;
	}
	return flag;
}

