/**
 * Test for HitShape functions hit_shapeCmp and hit_shapeFromSigShape (HitSigShape based constructor)
 * 
 * Cases checked:
 *      * Two HitShape objects pointing to the same reference
 *          Expected result: true
 *      * A null HitShape
 *          Expected result: false
 *      * Two HitShape objects pointing to different references but with the same values
 *          Expected result: true
 *      * Two HitShape objects pointing to different references with different values
 *          Expected result: false
 *      * A HitShape built using the same HitSigShape
 *          Expected result: true
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sigStd(20);
    HitShape shNull = HIT_SHAPE_NULL;
    HitShape sh = hit_shape(2, sig0, sig1);
    HitShape shDiff = hit_shape(1, sig0);
    HitShape shSameRef = sh;
    HitShape shAnotherRef = hit_shape(2, sig0, sig1);
    HitSigShape sigsh = hit_sShapeAccess(sh);
    HitShape shSameHitSig = hit_shapeFromSigShape(sigsh);
    int ok = 1;

    printf("Check hit_shapeFromSigShape and hit_shapeCmp: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tTwo objects pointing to different references with the same values: ");
    hit_shapeCmp(sh, shAnotherRef) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHitShape NULL: ");
    !hit_shapeCmp(sh, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tTwo objects pointing to the same reference: ");
    hit_shapeCmp(sh, shSameRef) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tTwo different objects: ");
    !hit_shapeCmp(sh, shDiff) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitShape built from the same HitSigShape: ");
    hit_shapeCmp(sh, shSameHitSig) ? printf("OK\n") : printf("ERROR\n");

#else

    // Two objects pointing to different references with the same values
    if (! hit_shapeCmp(sh, shAnotherRef)) ok = 0;
    // HitShape NULL
    if (hit_shapeCmp(sh, shNull)) ok = 0;
    // Two objects pointing to the same reference
    if (!  hit_shapeCmp(sh, shSameRef)) ok = 0;
    // Two different objects
    if (hit_shapeCmp(sh, shDiff)) ok = 0;
    // A HitShape built from the same HitSigShape
    if (! hit_shapeCmp(sh, shSameHitSig)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}