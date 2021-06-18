/**
 * Test for HitShape basic (HitSig based) constructor hit_shape and function hit_sShapeAccess
 * 
 * Cases checked:
 *      * One-dimensional HitShape
 *      * Two-dimensional HitShape
 *      * Three-dimensional HitShape
 *      * Four-dimensional HitShape
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(0, 9, 1);
    HitSig sig2 = hit_sigIndex(10);
    HitShape sh;
    HitSigShape sigsh;
    HitSig array[4] = {sig0, sig1, sig1, sig2};
    int ok = 1;

    printf("Check hit_shape and hit_sshapeAccess: ");
    
#ifdef DEBUG

    printf("\n");
    //
    printf("\t1D: ");
    sh = hit_shape(1, sig0);
    sigsh = hit_sShapeAccess(sh);
    hit_sigCmp(sig0, sigsh.sig[0]) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\t2D: ");
    sh = hit_shape(2, sig0, sig1);
    sigsh = hit_sShapeAccess(sh);
    hit_sigCmp(sig0, sigsh.sig[0]) && hit_sigCmp(sig1, sigsh.sig[1]) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\t3D: ");
    sh = hit_shape(3, sig0, sig1, sig1);
    sigsh = hit_sShapeAccess(sh);
    hit_sigCmp(sig0, sigsh.sig[0]) && hit_sigCmp(sig1, sigsh.sig[1]) && hit_sigCmp(sig1, sigsh.sig[2]) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\t4D: ");
    sh = hit_shape(4, sig0, sig1, sig1, sig2);
    sigsh = hit_sShapeAccess(sh);

    for(int i=0; i<sigsh.numDims; i++) {
        if(! hit_sigCmp(array[i], sigsh.sig[i])) {
            printf("ERROR\n");
            break;
        }
    }
    printf("OK\n");

#else

    // 1D
    sh = hit_shape(1, sig0);
    sigsh = hit_sShapeAccess(sh);
    if (! hit_sigCmp(sig0, sigsh.sig[0])) ok = 0;
    
    // 2D
    sh = hit_shape(2, sig0, sig1);
    sigsh = hit_sShapeAccess(sh);
    if (! (hit_sigCmp(sig0, sigsh.sig[0]) && hit_sigCmp(sig1, sigsh.sig[1]) )) ok = 0;
    
    // 3D
    sh = hit_shape(3, sig0, sig1, sig1);
    sigsh = hit_sShapeAccess(sh);
    if (! (hit_sigCmp(sig0, sigsh.sig[0]) && hit_sigCmp(sig1, sigsh.sig[1]) && hit_sigCmp(sig1, sigsh.sig[2]) )) ok = 0;
    
    // 4D
    sh = hit_shape(4, sig0, sig1, sig1, sig2);
    sigsh = hit_sShapeAccess(sh);

    for(int i=0; i<sigsh.numDims; i++) {
        if(! hit_sigCmp(array[i], sigsh.sig[i])) {
            ok = 0;
            break;
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}