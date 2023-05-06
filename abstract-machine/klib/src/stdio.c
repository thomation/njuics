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
  if(n == 0) {
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }
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
int hex_to_str(unsigned int n, char * str)
{
  char tmp[MAX_INT_STR_LEN];
  int i = 0;
  while(n > 0)
  {
    int v = n % 16;
    tmp[i ++] =  v >= 10 ? v - 10 + 'a' : v + '0';
    n /= 16;
  }
  str[0] = '0';
  str[1] = 'x';
  int j = 2;
  if(i == 0)
    str[j++] = '0';
  while(i > 0)
  {
    str[j++] = tmp[--i];
  }
  str[j] = '\0';
  return j;
}
int str_to_int(char* str) {
  int ret = 0;
  while(*str != '\0')
  {
    ret *= 10;
    ret += *str ++ - '0';
  }
  return ret; 
}
int handle_number(char *out, char*in, int len, char* prefix, int pi) {
    int min_len = len;
    char pad = ' ';
    if(pi > 0) {
      if(prefix[0] >= '1' && prefix[0] <= '9') {
        min_len = str_to_int(prefix);
      } else {
        pad = prefix[0];
        min_len = str_to_int(prefix + 1);
      }
    }
    for(int i = len; i < min_len; i ++) {
      if(out == NULL) putch(pad); else *out++ = pad;
    }
    for(int i = 0; i < len; i ++)
      if(out == NULL) putch(in[i]); else *out++ = in[i];
    return min_len > len ? min_len : len;
  }
int vsprintf(char* out, const char *fmt, va_list ap) {
  int count = 0;
  char prefix[8];
  while(*fmt) {
    if(*fmt == '%') {
      fmt ++;
      int pi = 0;
      while(*fmt >= '0' && *fmt <= '9') {
        prefix[pi ++] = *fmt ++;
      }
      prefix[pi] = '\0';
      switch(*fmt ++) {
        case 'd':
        {
          char tmp[32];
          int d = va_arg(ap, int);
          int len = int_to_str(d, tmp);
          int n = handle_number(out, tmp, len, prefix, pi);
          count += n; 
          if(out != NULL) out += n;
        }
        break;
        case 'u':
        {
          char tmp[32];
          int d = va_arg(ap, int);
          int len = uint_to_str(d, tmp);
          int n = handle_number(out, tmp, len, prefix, pi);
          count += n; 
          if(out != NULL) out += n;
        }
        break;
        // x and p are same
        case 'x':
        case 'p':
        {
          char tmp[32];
          int d = va_arg(ap, int);
          int len = hex_to_str(d, tmp);
          int n = handle_number(out, tmp, len, prefix, pi);
          count += n; 
          if(out != NULL) out += n;
        }
        break;
        case 's': {
          char *s = va_arg(ap, char*);
          int len = strlen(s);
          count += len;
          for(int i = 0; i < len; i ++)
            if(out == NULL) putch(s[i]); else *out ++ = s[i];
        }
        break;
        default:
          panic("Unsupported");
      }
    }
    else{
      if(out == NULL) putch(*fmt ++); else *out ++ = *fmt++;
      count ++;
    }
  }
  if(out != NULL) *out = '\0';
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
