/**
 * Test for HitSig function hit_sigBlend
 * 
 * Cases checked:
 *      * Standard case
 *          Expected result: a HitSig which is the selection of the indexes
 *              of the first parameter indicated by the second parameter
 * 
 * This test does not check a null HitSig as parameter. Since HIT_SIG_NULL is a HitSig itself, with
 * a begin, end and stride values, the result could be a "real" HitSig
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig = hit_sig(0, 50, 4);
    HitSig sig2 = hit_sig(21, 40, 3);
    HitSig intersect, res;
    int begin, end, stride, ok = 1;

    printf("Check hit_sigBlend: ");

#ifdef DEBUG

    printf("\n");
    printf("\tStandard case: ");
    intersect = hit_sigBlend(sig, sig2);
    begin = sig2.begin*sig.stride + sig.begin;
    end = sig2.end*sig.stride + sig.begin;
    stride = sig.stride * sig2.stride;
    res = hit_sig(begin, end, stride);
    hit_sigCmp(intersect, res) ? printf("OK\n") : printf("ERROR\n");

#else

    // Standard case
    intersect = hit_sigBlend(sig, sig2);
    begin = sig2.begin*sig.stride + sig.begin;
    end = sig2.end*sig.stride + sig.begin;
    stride = sig.stride * sig2.stride;
    res = hit_sig(begin, end, stride);
    if(! hit_sigCmp(intersect, res)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}