#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static struct timeval start_tv = {0, 0};
static int events_fd = -1;

extern int open(const char *path, int flags, ...);

uint32_t NDL_GetTicks() {
  struct timeval tv;
  struct timezone tz;
  int ret = gettimeofday(&tv, &tz);
  if(ret < 0) {
      printf("Error on get time of day \n");
      return 0;
  }
  return (tv.tv_sec - start_tv.tv_sec) * 1000 + (tv.tv_usec - start_tv.tv_usec);
}

int NDL_PollEvent(char *buf, int len) {
  if(events_fd < 0)
    return 0;
  return read(events_fd, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  printf("NDL_OpenCanvas w=%d, h=%d evn=%s\n", *w, *h, getenv("NWM_APP"));
  screen_w = *w; screen_h = *h;
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  printf("NDL_DrawRect x = %d, y = %d, w = %d, h =%d \n", x, y, w, h);
  int fd = open("/dev/fb", 0, 0);
  if(fd < 0) {
    printf("Cannot open /dev/fb\n");
    return;
  }
  for(int i = 0; i < h; i ++) {
    int start = (i + y) * screen_w + x;
    lseek(fd, start * 4, SEEK_SET);
    write(fd, pixels + i * w, w * 4);
  }
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  struct timezone tz;
  int ret = gettimeofday(&start_tv, &tz);
  if(ret < 0) {
    printf("Error on get time of day.\n");
    return -1;
  }
  events_fd = open("/dev/events", 0, 0);
  if(events_fd < 0) {
    printf("Error on open /dev/events\n");
    return -1;
  }
  printf("fd of /dev/events is %d\n", events_fd);
  return 0;
}

void NDL_Quit() {
  if(events_fd > 0)
    close(events_fd);
}
