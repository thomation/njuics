#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
int main() {
  printf("Start bmp test\n");
  NDL_Init(0);
  printf("bmp test NDL inited\n");
  int w = 0;
  int h = 0;
  NDL_OpenCanvas(&w, &h);
  printf("Canvas (w=%d, h=%d)\n", w, h);
  printf("Start load bmp\n");
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  printf("Bmp is loaded\n");
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
