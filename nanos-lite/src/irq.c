#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);
extern _RegSet* schedule(_RegSet *prev);

/*
am.h
#define _EVENTS(_) \
  _(IRQ_TIME) _(IRQ_IODEV) \
  _(ERROR) _(PAGE_FAULT) _(BUS_ERROR) _(NUMERIC) \
  _(TRAP) _(SYSCALL)
*/

static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
  	case _EVENT_SYSCALL: //printf("receive _EVENT_SYSCALL\n"); 
  						do_syscall(r);  // break; printf("finish _EVENT_SYSCALL\n");
  						return schedule(r); 
  						break;
  	case _EVENT_TRAP:  	// printf("receive _EVENT_TRAP\n"); 
  						return schedule(r); 
  						break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
