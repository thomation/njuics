#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
void get_screen_size(int *w, int *h) {
  int disp_fd = open("/proc/dispinfo", 0, 0);
  assert(disp_fd > 0);
  printf("fd of /proc/dispinfo is %d\n", disp_fd);
  char buf[128];
  if(read(disp_fd, buf, 128)) {
    sscanf(buf, "WIDTH=%d,HEIGHT=%d", w, h);
  }
}
int main() {
  printf("Start bmp test\n");
  NDL_Init(0);
  printf("bmp test NDL inited\n");
  int w, h;
  get_screen_size(&w, &h);
  printf("Canvas (w=%d, h=%d)\n", w, h);
  NDL_OpenCanvas(&w, &h);
  // void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  // assert(bmp);
  // NDL_DrawRect(bmp, 0, 0, w, h);
  // free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
