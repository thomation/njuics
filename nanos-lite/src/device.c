#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

struct timevalue {
  uint32_t s;
  uint32_t ms;
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  yield();
  const char * str = buf;
  for(int i = 0; i < len; i ++) {
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  yield();
  char * str = buf;
  bool has_kbd  = io_read(AM_INPUT_CONFIG).present;
  if (has_kbd) {
      AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
      if (ev.keycode == AM_KEY_NONE) return 0;
      // printf("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
      str[0] = 'k';
      str[1] = ev.keydown? 'd' : 'u';
      str[2] = ' ';
      strcpy(str + 3, keyname[ev.keycode]);
      int len = strlen(str);
      str[len + 1] = '\n';
      str[len + 2] = '\0';
      return len + 3;
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  char * str = buf;
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  sprintf(str, "WIDTH=%d,HEIGHT=%d", cfg.width, cfg.height);
  return strlen(str);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  yield();
  // Just draw one line
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  // printf("cfg:(%d, %d), draw len(%d), offset(%d)\n", cfg.width, cfg.height, len, offset);
  int h = offset / 4 / cfg.width;
  int w = offset / 4 - h * cfg.width;
  assert(w + len / 4 <= cfg.width);
  io_write(AM_GPU_FBDRAW, w, h, (uint32_t*)buf, len / 4, 1, false);
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  return len;
}
int gettimeofday(void * tv, void * tz) {
  struct timevalue *_tv = (struct timevalue*)tv;
  uint32_t t = io_read(AM_TIMER_UPTIME).us / 1000;
  _tv->s = t / 1000;
  _tv->ms = t % 1000;
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
