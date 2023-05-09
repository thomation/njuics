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
    fixedpt c = fixedpt_rconst(7.9);
    fixedpt d = fixedpt_rconst(5.6);
    fixedpt cd = fixedpt_mul(c, d);
    // They must be same?
    printf("c*d: %d ?= %d\n", cd, fixedpt_rconst(7.9 * 5.6));
    fixedpt ci = fixedpt_muli(c, 2);
    printf("c*i: %d ?= %d\n", ci, fixedpt_rconst(7.9 * 2));
    // assert(cd == fixedpt_rconst(7.9 * 5.6));
    fixedpt c_d = fixedpt_div(c, d);
    printf("c/b: %d ?= %d\n", c_d, fixedpt_rconst(7.9 / 5.6));
    fixedpt c_i = fixedpt_divi(c, 2);
    printf("c/i: %d ?= %d\n", c_i,fixedpt_rconst(7.9 / 2));
    fixedpt e = fixedpt_rconst(-3.8);
    fixedpt e2 = fixedpt_abs(e);
    printf("abs e=%d: %d ?= %d\n", e, e2, fixedpt_rconst(3.8));
    fixedpt ef = fixedpt_floor(e);
    printf("ef: %d ?= %d\n", ef, fixedpt_rconst(-4));
    fixedpt ec = fixedpt_ceil(e);
    printf("ec: %d ?= %d\n", ec, fixedpt_rconst(-3));

    printf("PASS!!!\n");
    return 0;
}