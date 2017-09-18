#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT) - boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;


_Screen _screen = {
  .width  = 400,
  .height = 300,
};

/*
_Screen _screen = {
  .width  = 800,
  .height = 600,
};

memcpy((void *)(fb+(y+i)*_screen.height+x), (const void *)(pixels+i*w), w*4);
*/


extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int i, j;
  /*
  for (i = 0; i < _screen.width * _screen.height; i++) {
    fb[i] = i;
  }
  */
  int k = 0;
  for (i = y; i < y+h; i++) {
    for (j = x; j < x+w; j++) {
      fb[_screen.width*i+j] = pixels[k++];
    } 
  }
}

void _draw_sync() {
}

int _read_key() {
	if (inb(0x64) == 0x1) {
		return inl(0x60);
	} else {
		return _KEY_NONE;
	}
}
