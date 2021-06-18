/**
 * Test for HitSig function hit_sigIntersect
 * 
 * Cases checked:
 *      * Comparison with a null HitSig
 *          Expected result: a null HitSig
 *      * A HitSig which starts at the rightmost index of the other HitSig
 *          Expected result: a HitSig containing only that index
 *      * A HitSig which starts at the rightmost index of the other HitSig plus one 
 *          Expected result: a null HitSig
 *      * A HitSig which starts at the leftmost index of the other HitSig
 *          Expected result: a HitSig containing only that index
 *      * A HitSig which starts at the leftmost index of the other HitSig minus one 
 *          Expected result: a null HitSig
 *      * With itself:
 *          Expected result: a HitSig with same begin and stride and the exact end index which is divisible by the stride
 *      * A standard HitSig
 *          Expected result: a HitSig containing the common indexes and as stride the lcm of the strides of the parameters
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig = hit_sig(0, 50, 4);
    HitSig resItself = hit_sig(sig.begin, sig.end-(sig.end%sig.stride), sig.stride);
    HitSig sigNull = HIT_SIG_NULL;

    HitSig sigLimRightIn = hit_sigIndex(sig.end-(sig.end%sig.stride));
    HitSig resLimRightIn = hit_sig(sigLimRightIn.begin, sigLimRightIn.end, sig.stride);

    HitSig sigLimRightOut = hit_sig(sig.end-sig.end%sig.stride+1, sig.end-sig.end%sig.stride+100, 1);

    HitSig sigLimLeftIn = hit_sigIndex(sig.begin);
    HitSig resLimLeftIn = hit_sig(sigLimLeftIn.begin, sigLimLeftIn.end, sig.stride);

    HitSig sigLimLeftOut = hit_sigIndex(sig.begin-1);

    HitSig sig2 = hit_sig(21, 40, 3);
    HitSig resStd = hit_sig(24, 36, 12);

    HitSig intersect;
    int ok = 1;

    printf("Check hit_sigIntersect: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tHitSig NULL: " );
    hit_sigCmp(hit_sigIntersect(sig, sigNull), HIT_SIG_NULL) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.end limit in: " );
    intersect = hit_sigIntersect(sig, sigLimRightIn);
    //printHitSig(intersect);
    hit_sigCmp(intersect, resLimRightIn) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.end limit out: " );
    intersect = hit_sigIntersect(sig, sigLimRightOut);
    hit_sigCmp(intersect, HIT_SIG_NULL) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.begin limit in: " );
    intersect = hit_sigIntersect(sig, sigLimLeftIn);
    hit_sigCmp(intersect, resLimLeftIn) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.begin limit out: " );
    intersect = hit_sigIntersect(sig, sigLimLeftOut);
    hit_sigCmp(intersect, HIT_SIG_NULL) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tWith itself: " );
    intersect = hit_sigIntersect(sig, sig);
    hit_sigCmp(intersect, resItself) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: " );
    intersect = hit_sigIntersect(sig, sig2);
    hit_sigCmp(intersect, resStd) ? printf("OK\n") : printf("ERROR\n");

#else

    // HitSig NULL
    if(! hit_sigCmp(hit_sigIntersect(sig, sigNull), HIT_SIG_NULL)) ok = 0;

    // sig.end limit in
    intersect = hit_sigIntersect(sig, sigLimRightIn);
    if(! hit_sigCmp(intersect, resLimRightIn)) ok = 0;

    // sig.end limit out
    intersect = hit_sigIntersect(sig, sigLimRightOut);
    if(! hit_sigCmp(intersect, HIT_SIG_NULL)) ok = 0;

    // sig.begin limit in
    intersect = hit_sigIntersect(sig, sigLimLeftIn);
    if(! hit_sigCmp(intersect, resLimLeftIn)) ok = 0;

    // sig.begin limit out
    intersect = hit_sigIntersect(sig, sigLimLeftOut);
    if(! hit_sigCmp(intersect, HIT_SIG_NULL)) ok = 0;

    // With itself
    intersect = hit_sigIntersect(sig, sig);
    if(! hit_sigCmp(intersect, resItself)) ok = 0;

    // Standard case
    intersect = hit_sigIntersect(sig, sig2);
    if(! hit_sigCmp(intersect, resStd)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}