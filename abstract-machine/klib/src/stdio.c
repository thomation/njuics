#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define MAX_INT_STR_LEN 32
int uint_to_str(unsigned int n, char * str)
{
  char tmp[MAX_INT_STR_LEN];
  int i = 0;
  while(n > 0)
  {
    tmp[i ++] = n % 10 + '0';
    n /= 10;
  }
  int j = 0;
  while(i > 0)
  {
    str[j++] = tmp[--i];
  }
  str[j] = '\0';
  return j;
}
int int_to_str(int n, char * str)
{
  if(n > 0)
    return uint_to_str(n, str);
  char tmp[MAX_INT_STR_LEN];
  int i = 0;
  while(n != 0)
  {
    tmp[i++] = - (n % 10) + '0';
    n /= 10;
  }
  str[0] = '-';
  int j = 1;
  while(i > 0)
  {
    str[j++] = tmp[--i];
  }
  str[j] = '\0';
  return j;
}
#define PUTCHARTO(out, c) \
  do { \
    if(out == NULL) { \
      putch(c); \
    } else { \
      *out ++ = (c); \
    } \
  } while(0) 

int vsprintf(char* out, const char *fmt, va_list ap) {
  int count = 0;
  while(*fmt) {
    if(*fmt == '%') {
      fmt ++;
      switch(*fmt ++) {
        case 'd':{
          char tmp[32];
          int d = va_arg(ap, int);
          int len = int_to_str(d, tmp);
          count += len;
          for(int i = 0; i < len; i ++)
            PUTCHARTO(out, tmp[i]);
        }
        break;
        case 's': {
          char *s = va_arg(ap, char*);
          int len = strlen(s);
          count += len;
          for(int i = 0; i < len; i ++)
            PUTCHARTO(out, s[i]);
        }
        break;
        default:
          panic("Unsupported");
      }
    }
    else{
      PUTCHARTO(out, *fmt ++);
      count ++;
    }
  }
  return count;
}
int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(NULL, fmt, ap);
  va_end(ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
