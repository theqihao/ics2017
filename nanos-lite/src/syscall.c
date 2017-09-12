#include "common.h"
#include "syscall.h"

void sys_none(_RegSet *r) {
	SYSCALL_ARG1(r) = 1;
}

void sys_exit() {
	_halt(0);
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);


  switch (a[0]) {
  	case SYS_none : sys_none(r); break;
  	case SYS_exit : sys_exit(); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
