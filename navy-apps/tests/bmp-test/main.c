#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

int main() {
  printf("Start bmp test\n");
  NDL_Init(0);
  printf("bmp test NDL inited\n");
  // int w, h;
  // void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  // assert(bmp);
  // NDL_OpenCanvas(&w, &h);
  // NDL_DrawRect(bmp, 0, 0, w, h);
  // free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
