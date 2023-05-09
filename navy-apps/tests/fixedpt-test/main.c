#include <fixedptc.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    fixedpt a = fixedpt_rconst(1.2);
    assert(a == 307);
    fixedpt ac = fixedpt_ceil(a);
    printf("ac: %d ?= %lld\n", ac, fixedpt_fromint(2));
    assert(ac == (fixedpt)fixedpt_fromint(2));
    fixedpt af = fixedpt_floor(a);
    printf("af: %d ?= %lld\n", af, fixedpt_fromint(1));
    assert(af == fixedpt_fromint(1));

    // fixedpt b = fixedpt_fromint(10);
    // int c = 0;
    // fixedpt d = fixedpt_rconst(5.6);
    // printf("The d is %d\n", d);
    // assert(d == 1434);
    // if (b > fixedpt_rconst(7.9))
    // {
    //     c = fixedpt_toint(fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), fixedpt_rconst(2.3)));
    // }
    // printf("The result is %d\n", c);
    printf("PASS!!!\n");
    return 0;
}