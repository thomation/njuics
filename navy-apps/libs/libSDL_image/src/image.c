#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  printf("IMG_Load file:%s\n", filename);
  FILE * f = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  size_t size = f->_offset;
  printf("IMG_Load size:%u\n", size);
  uint8_t * buf = (uint8_t*)malloc(sizeof(uint8_t) * size);
  fseek(f, 0, SEEK_SET);
  size_t len = fread(buf, 1, size, f);
  printf("IMG_load read len:%u\n", len);
  assert(size == len);
  SDL_Surface * surface = STBIMG_LoadFromMemory(buf, len);
  printf("IMG_load surface %p\n", surface);
  free(buf);
  fclose(f);
  return surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
