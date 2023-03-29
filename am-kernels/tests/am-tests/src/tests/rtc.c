#include <amtest.h>
#define Start_Timer() Begin_Time = uptime_ms()
#define Stop_Timer()  End_Time   = uptime_ms()

static uint32_t uptime_ms() { return io_read(AM_TIMER_UPTIME).us / 1000; }
static uint32_t Begin_Time;
static uint32_t End_Time;
void rtc_test() {
  AM_TIMER_RTC_T rtc;
  int sec = 1;
  Start_Timer();
  while (1) {
    while(io_read(AM_TIMER_UPTIME).us / 1000000 < sec) ;
    rtc = io_read(AM_TIMER_RTC);
    printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    if (sec == 1) {
      printf("%d second).\n", sec);
    } else {
      printf("%d seconds).\n", sec);
    }
    sec ++;
    Stop_Timer();
    int delta = End_Time - Begin_Time;
    printf("delta:%d\n", delta);
  }
}
