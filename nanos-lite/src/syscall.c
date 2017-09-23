#include "common.h"
#include "syscall.h"
#include "fs.h"

int sys_none() {
	return 1;
}

void sys_exit() {
	_halt(0);
}


int sys_write(int fd, void *buf, size_t count) {
	//Log("write, len = %d\n", count);
	return fs_write(fd, buf, count);

/*
	if (!(fd == 1 || fd == 2)) {
		return -1;
	}
	Log("write, len = %d\n", count);
	int i = 0;
	for (i = 0; i < count; i++) {
		_putc((char) *((char*)(buf+i)));
	}
	return count;
*/
}



/*
extern char end;
static uint32_t pb = (uint32_t)&end;
int mm_brk(uint32_t new_brk);
uint32_t sys_brk(uint32_t addr) {
	if (addr == 0) {
		return pb;
	}
	*(uint32_t *)pb = addr;
	return mm_brk(addr);
}
*/
int mm_brk(uint32_t new_brk);
uint32_t sys_brk(uintptr_t addr, uintptr_t inc) {
	*(uintptr_t *)addr = *(uintptr_t *)addr + inc;
	uint32_t new_brk = *(uintptr_t *)addr;
	return mm_brk(new_brk);
}

int sys_open(char *pathname) {
	return fs_open(pathname, 0, 0);
}
 
int sys_read(int fd, void *buf, size_t len) {
	return fs_read(fd, buf, len);
}

off_t sys_lseek(int fd, off_t offset, int whence) {
	return fs_lseek(fd, offset, whence);
}

int sys_close(int fd) {
	return fs_close(fd);
}


_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
  	case SYS_none : SYSCALL_ARG1(r) = sys_none(); break;
  	case SYS_exit : sys_exit(); break;
  	case SYS_write: SYSCALL_ARG1(r) = sys_write((int)a[1], (void *)a[2], (size_t)a[3]); break;
  	case SYS_brk: SYSCALL_ARG1(r) = sys_brk((uintptr_t)a[1], (uintptr_t)a[2]); break;
  	case SYS_open: SYSCALL_ARG1(r) = sys_open((char *)a[1]) ; break;
  	case SYS_read: SYSCALL_ARG1(r) = sys_read((int)a[1], (void *)a[2], (size_t)a[3]); break;
  	case SYS_lseek: SYSCALL_ARG1(r) = sys_lseek((int)a[1], (int)a[2], (int)a[3]); break;
  	case SYS_close: SYSCALL_ARG1(r) = sys_close((int)a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
