#include <am.h>
#include <nemu.h>
#include <klib.h>

static uint32_t boot_time[2];
void __am_timer_init() {
  boot_time[1] = inl(RTC_ADDR + 4); 
  boot_time[0] = inl(RTC_ADDR);
  printf("boot time:%d, %d\n", boot_time[0], boot_time[1]);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t new_time[2];
  // Call offset==4 first which is the implement of timer.c in device folder. I don't know why it is write like this.
  new_time[1] = inl(RTC_ADDR + 4);
  new_time[0] = inl(RTC_ADDR);
  uint64_t s = new_time[1] - boot_time[1];
  uptime->us = (s << 32)  + (new_time[0] - boot_time[0] + 500);
  // printf("time:%d, %d\n", new_time[0], new_time[1]);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
