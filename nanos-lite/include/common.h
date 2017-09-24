#ifndef __COMMON_H__
#define __COMMON_H__

#include <am.h>
#include <klib.h>
#include "debug.h"


typedef char bool;
#define true 1
#define false 0

extern bool current_game;
size_t get_ramdisk_size();
void ramdisk_read(void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);

#endif
