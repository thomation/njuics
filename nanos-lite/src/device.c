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
  const char * str = buf;
  for(int i = 0; i < len; i ++) {
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
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
