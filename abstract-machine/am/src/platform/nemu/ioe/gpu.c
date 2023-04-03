#include <am.h>
#include <nemu.h>
#include <klib.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define SIZE_ADDR VGACTL_ADDR

static uint32_t screen_width() {
  uint32_t size = inl(SIZE_ADDR);
  int w = size >> 16 & 0xffff;
  return w;
}

static uint32_t screen_height() {
  uint32_t size = inl(SIZE_ADDR);
  int h = size & 0xffff;
  return h;
}
void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = screen_width(), .height = screen_height(),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int w = ctl->w, h = ctl->h;
  if (w == 0 || h == 0) {
    if (ctl->sync) {
      outl(SYNC_ADDR, 1);
    }
    return;
  }
  int x = ctl->x, y = ctl->y;
  uint32_t * p = ctl->pixels;
  uint32_t size_w = screen_width();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for(int j = 0; j < h; j ++) {
    for(int i = 0; i < w; i ++) {
      int pi = j * w + i;
      int si = (j + y) * size_w + i + x;
      fb[si] = p[pi];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
