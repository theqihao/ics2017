#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);

/*
am.h
#define _EVENTS(_) \
  _(IRQ_TIME) _(IRQ_IODEV) \
  _(ERROR) _(PAGE_FAULT) _(BUS_ERROR) _(NUMERIC) \
  _(TRAP) _(SYSCALL)
*/

static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
  	case _EVENT_SYSCALL: do_syscall(r); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
