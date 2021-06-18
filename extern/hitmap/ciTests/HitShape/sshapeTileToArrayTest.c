/**
 * Test for HitShape functions hit_shapeTileToArray
 * 
 * Cases checked:
 *      * Selecting from a null HitShape
 *          Expected result: a null HitShape
 *      * A null HitShape as second parameter
 *          Expected result: a null HitShape
 *      * A HitShape with different number of dimensions
 *          Expected result: a null HitShape
 *      * Standard case
 *          Expected result: a HitShape with the indexes of the second parameter in the
 *              reference system of the first parameter in array coordinates
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
    HitShape tileToArr, res;
    int ok = 1;

    HitSig sigres1 = hit_sigBlend(hit_shapeSig(sh0, 0), hit_shapeSig(sh1, 0));
    HitSig sigres2 = hit_sigBlend(hit_shapeSig(sh0, 1), hit_shapeSig(sh1, 1));
    res = hit_shape(2, sigres1, sigres2);


    printf("Check hit_shapeTileToArray: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape as first parameter: ");
    tileToArr = hit_shapeTileToArray(HIT_SHAPE_NULL, sh0);
    hit_shapeCmp(HIT_SHAPE_NULL, tileToArr) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNull HitShape as second parameter: ");
    tileToArr = hit_shapeTileToArray(sh0, HIT_SHAPE_NULL);
    hit_shapeCmp(HIT_SHAPE_NULL, tileToArr) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHitShape with different dimensions: ");
    tileToArr = hit_shapeTileToArray(sh0, diffDims);
    hit_shapeCmp(HIT_SHAPE_NULL, tileToArr) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: ");
    tileToArr = hit_shapeTileToArray(sh0, sh1);
    hit_shapeCmp(res, tileToArr) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape as first parameter
    tileToArr = hit_shapeTileToArray(HIT_SHAPE_NULL, sh0);
    if (! hit_shapeCmp(HIT_SHAPE_NULL, tileToArr)) ok = 0;

    // Null HitShape as second parameter
    tileToArr = hit_shapeTileToArray(sh0, HIT_SHAPE_NULL);
    if (! hit_shapeCmp(HIT_SHAPE_NULL, tileToArr)) ok = 0;

    // Standard case
    tileToArr = hit_shapeTileToArray(sh0, sh1);
    if (! hit_shapeCmp(res, tileToArr)) ok = 0;

    // HitShape with different dimensions
    tileToArr = hit_shapeTileToArray(sh0, diffDims);
    if (! hit_shapeCmp(HIT_SHAPE_NULL, tileToArr)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}