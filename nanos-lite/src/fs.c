#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb

}

int fs_open(const char *pathname, int flags, int mode) {
  //printf("fs_open\n");
  int i = 0;
  int flag = 0;
  for (i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    //assert(0);
    //printf("no such file\n");
    return -1;
  } else {
    //printf("find file\n");
    file_table[i].open_offset = file_table[i].disk_offset;
    return i;
  }
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  //assert(file_table[fd].open_offset + len <= file_table[fd].disk_offset+file_table[fd].size);
  ramdisk_read(buf, file_table[fd].open_offset, len);
  fs_lseek(fd, file_table[fd].open_offset+len-file_table[fd].disk_offset, SEEK_SET);
  return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  int i = 0;
  //assert(file_table[fd].open_offset + len <= file_table[fd].disk_offset+file_table[fd].size);
  switch (fd) {
    case FD_STDOUT:
    case FD_STDERR:
      // call _putc()
      for (i = 0; i < len; i++) {
        _putc((char) *((char*)(buf+i)));
      }
      break;

    case FD_FB:
      // write to frame buffer
      break;

    default:
      // write to ramdisk
      ramdisk_write(buf, file_table[fd].open_offset, len);
      break;
  }
  fs_lseek(fd, file_table[fd].open_offset-file_table[fd].disk_offset+len, SEEK_SET);
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  off_t newpos;
  switch (whence) {
    case SEEK_SET: newpos = file_table[fd].disk_offset + offset; break;
    case SEEK_CUR: newpos = file_table[fd].open_offset + offset; break;
    case SEEK_END: newpos = /*file_table[fd].disk_offset + */file_table[fd].size+offset; break;
    default: return -1;
  }
  if (0 <= newpos && newpos <= file_table[fd].disk_offset+file_table[fd].size) {
    file_table[fd].open_offset = newpos;
    return newpos;
  } else {
    return -1;
  }
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}

int fs_close(int fd) {
  return 0;
}
