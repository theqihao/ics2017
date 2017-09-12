#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  
  int fd = fs_open(filename, 0, 0);
  int len = fs_filesz(fd);
  fs_read(fd, DEFAULT_ENTRY, len);
  printf("load file name : %s, fd = %d, len = %d\n", filename, fd, len);
	//ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
