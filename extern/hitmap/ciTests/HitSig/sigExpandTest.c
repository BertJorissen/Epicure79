/**
 * Test for HitSig function hit_sigExpand
 * 
 * Cases checked:
 *      * Standard case
 *          Expected result: a HitSig whose begin and end indexes are the sum
 *              of those of the parameters, and its stride is the product
 *     * Standard case inverted
 *          Expected result: the same as the previous case, since this operation is conmutative
 * 
 * This test does not check a null HitSig as parameter. Since HIT_SIG_NULL is a HitSig itself, with
 * a begin, end and stride value, the result could be a "real" HitSig
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig = hit_sig(0, 30, 4);
    HitSig sig2 = hit_sig(5, 30, 2);
    HitSig intersect, res;
    int begin, end, stride, ok = 1;

    printf("Check hit_sigExpand: ");   
    
#ifdef DEBUG

    printf("\n");
    //
    printf("\tStandard case: ");
    intersect = hit_sigExpand(sig, sig2);
    begin = sig2.begin + sig.begin;
    end = sig2.end + sig.end;
    stride = sig.stride * sig2.stride;
    res = hit_sig(begin, end, stride);
    hit_sigCmp(intersect, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case inverted: ");
    intersect = hit_sigExpand(sig2, sig);
    hit_sigCmp(intersect, res) ? printf("OK\n") : printf("ERROR\n");

#else

    // Standard case
    intersect = hit_sigExpand(sig, sig2);
    begin = sig2.begin + sig.begin;
    end = sig2.end + sig.end;
    stride = sig.stride * sig2.stride;
    res = hit_sig(begin, end, stride);
    if(! hit_sigCmp(intersect, res)) ok = 0;

    // Standard case inverted
    intersect = hit_sigExpand(sig2, sig);
    if(! hit_sigCmp(intersect, res)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");
    
#endif

    return 0;
}