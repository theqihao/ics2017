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
  int fd = fs_open("/dev/fb", 0, 0);
  //file_table[fd].disk_offset = fb;
  file_table[fd].size = _screen.width * _screen.height;
  printf("init_fb_size = %d\n", file_table[fd].size);
  fs_close(fd);
}

int fs_open(const char *pathname, int flags, int mode) {
  int i = 0;
  int flag = 0;
  for (i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    assert(0);
    return -1;
  } else {
    file_table[i].open_offset = 0;
    return i;
  }
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  //assert(file_table[fd].open_offset + len <= file_table[fd].size);
  Finfo *fi = &file_table[fd];
  int write_num = fi->size - fi->open_offset;
  write_num = (len > write_num) ? write_num : len;
  switch (fd) {
    case FD_STDIN: 
    case FD_STDOUT:
    case FD_STDERR:
        return 0;
    case FD_DISPINFO: 
        dispinfo_read(buf, fi->open_offset, write_num);
        fs_lseek(fd, fi->open_offset+write_num, SEEK_SET);
        return write_num;
    case FD_EVENTS:
        return events_read(buf, len);
    default:
    //printf("read addr = %08X, len = %d\n", file_table[fd].disk_offset+file_table[fd].open_offset, len);
        ramdisk_read(buf, file_table[fd].disk_offset+file_table[fd].open_offset, write_num);
        fs_lseek(fd, file_table[fd].open_offset+write_num, SEEK_SET);
        return write_num;
  }
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  //assert(file_table[fd].open_offset + len <= file_table[fd].size);
  Finfo *fi = &file_table[fd];
  int write_num = fi->size - fi->open_offset;
  write_num = (len > write_num) ? write_num : len;
  int i = 0;
  switch (fd) {
    case FD_STDIN: return 0;
    case FD_STDOUT:
    case FD_STDERR:
      // call _putc()
      for (i = 0; i < len; i++) {
        _putc((char) *((char*)(buf+i)));
      }
      return len;
    case FD_FB:
      // write to frame buffer
      fb_write(buf, file_table[fd].open_offset, len);
      fs_lseek(fd, file_table[fd].open_offset+len, SEEK_SET);
      return len;

    default:
      // write to ramdisk
      ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, write_num);
      fs_lseek(fd, file_table[fd].open_offset+write_num, SEEK_SET);
      return write_num;
  }

}

off_t fs_lseek(int fd, off_t offset, int whence) {
  off_t newpos;
  switch (whence) {
    case SEEK_SET: newpos = offset; break;
    case SEEK_CUR: newpos = file_table[fd].open_offset + offset; break;
    case SEEK_END: newpos = file_table[fd].size + offset; break;
    default: return -1;
  }
  file_table[fd].open_offset = newpos;
  return newpos;

/*
  if (fd == 3) {
    file_table[fd].open_offset = newpos;
    return newpos;
  }

  if (0 <= newpos && newpos <= file_table[fd].size) {
    file_table[fd].open_offset = newpos;
    return newpos;
  } else {
    return -1;
  }
  */
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}

int fs_close(int fd) {
  return 0;
}
