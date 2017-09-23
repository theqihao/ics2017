#include "common.h"
#include "fs.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  printf("as->ptr = %08X\n", as->ptr);
  uintptr_t fd = fs_open(filename, 0, 0);
  uintptr_t len = fs_filesz(fd);
  uintptr_t vaddr = (uintptr_t)DEFAULT_ENTRY;
  for (; vaddr < (uintptr_t)(DEFAULT_ENTRY + PGROUNDUP((uintptr_t)len)); vaddr += PGSIZE) {
  	void *page = new_page();
  	_map(as, (void *)vaddr, page);
  	fs_read(fd, page, PGSIZE);
  	//printf("vaddr = %08X, page = %08X\n", vaddr, page);
  }
  printf("load file name : %s, fd = %d, len = %d\n", filename, fd, len);
  //printf("cr0 = %08X, cr3 = %08X\n", cpu.cr0, cpu.cr3);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
