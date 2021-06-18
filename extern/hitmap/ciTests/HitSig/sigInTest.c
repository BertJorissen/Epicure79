/**
 * Test for HitSig function hit_sigIn
 * 
 * Cases checked:
 *      * HitSig's begin index
 *          Expected result: true
 *      * HitSig's begin index minus one
 *          Expected result: false
 *      * HitSig's last index
 *          Expected result: true
 *      * HitSig's last index plus one
 *          Expected result: false
 *      * Middle index calculated with stride
 *          Expected result: true
 *      * Middle index calculated with stride minus one
 *          Expected result: false
 *      * Middle index calculated with stride plus one
 *          Expected result: false
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>


int main() {
    HitSig sig = hit_sig(0, 50, 4);
    int ok = 1;

    printf("Check hit_sigIn: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tsig.begin: " );
    hit_sigIn(sig, sig.begin) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.begin out: " );
    hit_sigIn(sig, sig.begin-1) ? printf("ERROR\n") : printf("OK\n");
    //
    printf("\tsig last index: " );
    hit_sigIn(sig, sig.end-(sig.end%sig.stride)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig last index out: " );
    hit_sigIn(sig, sig.end-(sig.end%sig.stride)+1) ? printf("ERROR\n") : printf("OK\n");
    // 
    printf("\tsig.stride: " );
    hit_sigIn(sig, sig.begin+sig.stride) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.stride left limit: " );
    hit_sigIn(sig, sig.begin+sig.stride-1) ? printf("ERROR\n") : printf("OK\n");
    //
    printf("\tsig.stride right limit: " );
    hit_sigIn(sig, sig.begin+sig.stride+1) ? printf("ERROR\n") : printf("OK\n");

#else

    // sig.begin
    if(! hit_sigIn(sig, sig.begin)) ok = 0;

    // sig.begin out
    if(hit_sigIn(sig, sig.begin-1)) ok = 0;

    // sig last index
    if(! hit_sigIn(sig, sig.end-(sig.end%sig.stride))) ok = 0;

    // sig last index out
    if(hit_sigIn(sig, sig.end-(sig.end%sig.stride)+1)) ok = 0;

    // sig.stride
    if(! hit_sigIn(sig, sig.begin+sig.stride)) ok = 0;

    // sig.stride left limit
    if(hit_sigIn(sig, sig.begin+sig.stride-1)) ok = 0;

    // sig.stride right limit
    if(hit_sigIn(sig, sig.begin+sig.stride+1)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}