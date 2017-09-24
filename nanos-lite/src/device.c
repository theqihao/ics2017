#include "common.h"
#include "fs.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

#define KEYDOWN_MASK 0x8000

size_t events_read(void *buf, size_t len) {
	
	int real_len = 0;
	char temp[32];
	int flag = 0;
	int offset = 0;
	while (real_len < len) {
		flag = 0;
		int keycode = _read_key();
		if (keycode == _KEY_NONE) {
			keycode = _uptime();
			flag = 1;
		} else {
			if ((keycode & KEYDOWN_MASK) == 0) {
				flag = 2;
			} else {
				keycode = keycode & ~KEYDOWN_MASK;
				flag = 3;

				if (keycode == _KEY_F12) current_game = !current_game;
			}
		}
		memset(temp, '\0', sizeof(temp));
		int temp_len = 0;
		switch (flag) {
			case 1: sprintf(temp, "t %d\n", keycode); temp_len = strlen(temp); break;
			case 2: sprintf(temp, "ku %s\n", keyname[keycode]); temp_len = strlen(temp); break;
			case 3: sprintf(temp, "kd %s\n", keyname[keycode]); temp_len = strlen(temp); break;
			default: printf("no such keycode = %d \n", keycode);
		}

		if (real_len+temp_len <= len) {
			real_len += temp_len;
			sprintf(buf+offset, "%s", temp);
			offset += temp_len;
		} else {
			break;
		}
	}
	//printf("events_read = %s", buf);
	//memcpy(buf+real_len, "\0", 1);
  return real_len;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
	memcpy(buf, dispinfo+offset, len);
	//memcpy(buf+len, "\0", 1);
	// /buf[len] = (void *)'\0';
}

void fb_write(const void *buf, off_t offset, size_t len) {
	
	int width = _screen.width;
	len /= 4;
	offset /= 4;
	int y = (offset) / width;
	int x = (offset) % width;
	_draw_rect((uint32_t *)(buf), x, y, len, 1);
	
	/*
	int width = _screen.width;
	len /= 4;
	offset /= 4;
	int y = (offset) / width;
	int x = (offset) % width;
	//printf("offset = %d, len = %d, x = %d, y = %d\n", offset, len, x, y);
	int used = 0;
	while (len > 0) {
		int cur_row = width - x;
		cur_row = (cur_row > len) ? len : cur_row;
		_draw_rect((uint32_t *)(buf+used*4), x, y, cur_row, 1);
		used += cur_row;
		offset += cur_row;
		y = (offset) / width;
		x = (offset) % width;
		len -= cur_row;
		//printf("offset = %d, len = %d, x = %d, y = %d\n", offset, len, x, y);
	}
	*/
	//int h = len / (_screen.width-x);
	//_draw_rect((uint32_t *)buf, y, x, len, 2);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  //int fd = fs_open("/proc/dispinfo", 0, 0);
  //int len = fs_filesz(fd);
  // memcpy(dispinfo, 0, len);
  //fs_read(fd, (void*)dispinfo, len);
  //memcpy(dispinfo, 0, len);
  //sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", _screen.width, _screen.height);
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n\0", _screen.width, _screen.height);
  //fs_close(fd);
  printf("dispinfo:\n%s\n", dispinfo);
}
