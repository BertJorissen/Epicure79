/**
 * Test for HitShape function hit_shapeSubset
 * 
 * Cases checked:
 *      * Selecting from a null HitShape
 *          Expected result: a null HitShape
 *      * A null HitShape as second parameter
 *          Expected result: a null HitShape
 *      * A HitShape with different number of dimensions
 *          Expected result: a null HitShape
 *      * Standard case
 *          Expected result: a HitShape which is the selection of the indexes
 *              of the first parameter indicated by the second parameter
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(22, 30, 2);
    HitSig sig2 = hit_sig(1, 4, 2);
    HitShape sh0 = hit_shape(2, sig1, sig1);
    HitShape sh1 = hit_shape(2, sig0, sig1);
    HitShape diffDims = hit_shape(3, sig0, sig1, sig2);
    HitShape subset, res;
    HitShape shapeNull = HIT_SHAPE_NULL;
    int ok = 1;

    HitSig sigres1 = hit_sigBlend(hit_shapeSig(sh0, 0), hit_shapeSig(sh1, 0));
    HitSig sigres2 = hit_sigBlend(hit_shapeSig(sh0, 1), hit_shapeSig(sh1, 1));
    res = hit_shape(2, sigres1, sigres2);


    printf("Check hit_shapeSubset: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape as first parameter: ");
    subset = hit_shapeSubset(shapeNull, sh0);
    hit_shapeCmp(shapeNull, subset) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNull HitShape as second parameter: ");
    subset = hit_shapeSubset(sh0, shapeNull);
    hit_shapeCmp(shapeNull, subset) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHitShape with different dimensions: ");
    subset = hit_shapeSubset(sh0, diffDims);
    hit_shapeCmp(shapeNull, subset) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: ");
    subset = hit_shapeSubset(sh0, sh1);
    hit_shapeCmp(res, subset) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape as first parameter
    subset = hit_shapeSubset(shapeNull, sh0);
    if (! hit_shapeCmp(shapeNull, subset)) ok = 0;

    // Null HitShape as second parameter
    subset = hit_shapeSubset(sh0, shapeNull);
    if (! hit_shapeCmp(shapeNull, subset)) ok = 0;

    // Standard case
    subset = hit_shapeSubset(sh0, sh1);
    if (! hit_shapeCmp(res, subset)) ok = 0;

    // HitShape with different dimensions
    subset = hit_shapeSubset(sh0, diffDims);
    if (! hit_shapeCmp(shapeNull, subset)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}