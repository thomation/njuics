#include <am.h>
#include <nemu.h>

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
  int i;
  uint32_t w = screen_width();
  uint32_t h = screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h / 2; i ++) fb[i] = 0x00ff0000;
  for (i = w * h / 2; i < w * h; i ++) fb[i] = 0x0000ff00;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = screen_width(), .height = screen_height(),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
