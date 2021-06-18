/**
 * Test for HitSig function hit_sigCmp
 * 
 * Cases checked:
 *      * Two HitSig objects pointing to the same reference
 *          Expected result: true
 *      * Two HitSig objects pointing to different references but with the same values
 *          Expected result: true
 *      * Two HitSig objects pointing to different references with different values
 *          Expected result: false
 *      * Comparison with a null HitSig
 *          Expected result: false
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>


int main() {
    int numElems = 10;
    HitSig sig = hit_sigStd(numElems);
    HitSig sigAnotherRef = hit_sig(0, 9, 1);
    HitSig sigNull = HIT_SIG_NULL;
    HitSig sigSameRef = sig;
    HitSig sigDiff = hit_sigIndex(22);
    int ok = 1;

    printf("Check hit_sigCmp: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tTwo objects pointing to different references with the same values: " );
    hit_sigCmp(sig, sigAnotherRef) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHitSig NULL: " );
    hit_sigCmp(sig, sigNull) ?  printf("ERROR\n") : printf("OK\n");
    //
    printf("\tTwo objects pointing to the same reference: " );
    hit_sigCmp(sig, sigSameRef) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tTwo objects pointing to different references with different values: " );
    hit_sigCmp(sig, sigDiff) ?  printf("ERROR\n") : printf("OK\n");

#else

    // Two objects pointing to different references with the same values
    if (! hit_sigCmp(sig, sigAnotherRef)) ok = 0;
    // HitSig NULL
    if (hit_sigCmp(sig, sigNull)) ok = 0;
    // Two objects pointing to the same reference
    if (! hit_sigCmp(sig, sigSameRef)) ok = 0;
    // Two objects pointing to different references with different values
    if (hit_sigCmp(sig, sigDiff)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}