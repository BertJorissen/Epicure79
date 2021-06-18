/**
 * Test for HitShape function hit_shapeArrayToTile
 * 
 * Cases checked:
 *      * Selecting from a null HitShape
 *          Expected result: a null HitShape
 *      * A null HitShape as second parameter
 *          Expected result: a null HitShape
 *      * A HitShape with different number of dimensions
 *          Expected result: a null HitShape
 *      * An incompatible HitShape (strides are not divisible)
 *          Expected result: a null HitShape
 *      * Standard case
 *          Expected result: a HitShape with the indexes of the second parameter in the
 *              reference system of the first parameter in tile coordinates
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(22, 30, 2);
    HitSig sig2 = hit_sig(1, 20, 4);
    HitSig sig3 = hit_sig(22, 35, 3);
    HitShape sh0 = hit_shape(2, sig1, sig1);
    HitShape sh1 = hit_shape(2, sig2, sig2);
    HitShape shIncomp = hit_shape(2, sig0, sig3);
    HitShape diffDims = hit_shape(3, sig0, sig1, sig2);
    HitShape arrToTile, res;
    HitShape shapeNull = HIT_SHAPE_NULL;
    int ok = 1;

    HitSig sigres = hit_sig(-10, -1, 2);
    res = hit_shape(2, sigres, sigres);


    printf("Check hit_shapeArrayToTile: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape as first parameter: ");
    arrToTile = hit_shapeArrayToTile(shapeNull, sh0);
    hit_shapeCmp(shapeNull, arrToTile) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNull HitShape as second parameter: ");
    arrToTile = hit_shapeArrayToTile(sh0, shapeNull);
    hit_shapeCmp(shapeNull, arrToTile) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHitShape with different dimensions: ");
    arrToTile = hit_shapeArrayToTile(sh0, diffDims);
    hit_shapeCmp(shapeNull, arrToTile) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tIncompatible HitShape: ");
    arrToTile = hit_shapeArrayToTile(sh0, shIncomp);
    hit_shapeCmp(shapeNull, arrToTile) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: ");
    arrToTile = hit_shapeArrayToTile(sh0, sh1);
    hit_shapeCmp(res, arrToTile) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape as first parameter
    arrToTile = hit_shapeArrayToTile(shapeNull, sh0);
    if (! hit_shapeCmp(shapeNull, arrToTile)) ok = 0;

    // Null HitShape as second parameter
    arrToTile = hit_shapeArrayToTile(sh0, shapeNull);
    if (! hit_shapeCmp(shapeNull, arrToTile)) ok = 0;

    // HitShape with different dimensions
    arrToTile = hit_shapeArrayToTile(sh0, diffDims);
    if (! hit_shapeCmp(shapeNull, arrToTile)) ok = 0;

    // Incompatible HitShape
    arrToTile = hit_shapeArrayToTile(sh0, shIncomp);
    if (! hit_shapeCmp(shapeNull, arrToTile)) ok = 0;

    // Standard case
    arrToTile = hit_shapeArrayToTile(sh0, sh1);
    if (! hit_shapeCmp(res, arrToTile)) ok = 0;


    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}