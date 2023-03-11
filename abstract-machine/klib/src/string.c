#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while(*s++ != '\0')
    len ++;
  return len;
}

char *strcpy(char *dst, const char *src) {
  char * p = dst;
  while((*p++ = *src++) != '\0')
    ;
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char * p = dst;
  int i = 0;
  while(i++ < n && (*p++ = *src++) != '\0')
    ;
  while(i ++ < n)
    *p ++ = '\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  char * p = dst;
  while(*p++ != '\0')
    ;
  while((*p++ = *src++) != '\0')
    ;
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  for(; *s1 != '\0' && *s2 != '\0'; s1++, s2++) {
    if(*s1 < *s2)
      return -1;
    if(*s1 > *s2)
      return 1;
  }
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for(int i = 0; i < n && *s1 != '\0' && *s2 != '\0'; s1++, s2++, i++) {
    if(*s1 < *s2)
      return -1;
    if(*s1 > *s2)
      return 1;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  unsigned char * p = s;
  for(int i = 0; i < n; i ++)
    *p ++ = c;
  return s;
}
void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented, need malloc and free");
}

void *memcpy(void *out, const void *in, size_t n) {
  unsigned char * d = out;
  const unsigned char * s = in;
  for(int i = 0; i < n; i ++)
    *d++ = *s ++;
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char * c1 = s1;
  const unsigned char * c2 = s2;
  for(int i = 0; i < n; i ++) {
    int r = (int)(*c1 - *c2);
    if(r != 0)
      return r;
  }
  return 0;
}

#endif
