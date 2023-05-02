#include <stdio.h>
#include <stdint.h>
#include <NDL.h>

int main() {
    printf("Start timer test\n");
    NDL_Init(0);
    uint32_t last_ms = 0;
    for(;;) {
        uint32_t ms = NDL_GetTicks();
        if(ms - last_ms > 500) {
            printf("Get Tickets:%u, last:%u\n", ms, last_ms);
            last_ms = ms;
        }
    }
    NDL_Quit();
}