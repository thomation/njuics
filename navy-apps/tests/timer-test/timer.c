#include <sys/time.h>
#include <stdio.h>
int main() {
    printf("Start timer test\n");
    struct timeval tv;
    struct timezone tz;
    struct timeval last_tv = {0, 0};
    for(;;) {
        int ret = gettimeofday(&tv, &tz);
        if(ret < 0) {
            printf("Error on get time of day \n");
            return -1;
        }
        if(tv.tv_sec > last_tv.tv_sec || tv.tv_usec - last_tv.tv_usec > 500) {
            printf("Get time of day \n");
            last_tv = tv;
        }
    }
}