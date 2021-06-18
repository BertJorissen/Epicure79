/**
 * Test for HitShape function hit_shapeIntersect
 * 
 * Cases checked:
 *      * Comparison with a null HitShape
 *          Expected result: a null HitShape
 *      * With itself
 *          Expected result: a HitShape equal to itself
 *      * A HitShape equal in all dimensions but in the 1st, where it starts
 *          at the leftmost index of the other HitSig
 *          Expected result: a HitShape equal in all dimensions, but the 1st contains only one index
 *      * A HitShape equal in all dimensions but in the 1st, where it starts
 *          at the leftmost index of the other HitSig plus one
 *          Expected result: a null HitShape
 *      * A HitShape with different number of dimensions
 *          Expected result: a null HitShape
 *      * A Standard HitShape
 *          Expected result: a HitShape containing the common indexes to the parameters in all dimensions
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(2, 9, 2);
    HitSig sig2 = hit_sig(-1, 10, 4);

    HitShape shNull = HIT_SHAPE_NULL;
    HitShape sh = hit_shape(2, sig0, sig0);
    HitShape shOtrasDim = hit_shape(1, hit_sigIndex(0));
    HitShape shLimRightIn = hit_shape(2, hit_sig(9, 12, 1), sig0);
    HitShape shLimRightOut = hit_shape(2, hit_sig(10, 12, 1), sig0);
    HitShape shStd = hit_shape(2, sig1, sig2);
    HitShape res, intersect;
    int ok = 1;

    printf("Check hit_shapeIntersect: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tHitShape NULL: ");
    intersect = hit_shapeIntersect(sh, shNull);
    hit_shapeCmp(intersect, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tWith itself: ");
    intersect = hit_shapeIntersect(sh, sh);
    hit_shapeCmp(intersect, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tRight limit in dimension 0 in: ");
    intersect = hit_shapeIntersect(sh, shLimRightIn);
    res = hit_shape(2, hit_sigIndex(9), sig0);
    hit_shapeCmp(intersect, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tRight limit in dimension 0 out: ");
    intersect = hit_shapeIntersect(sh, shLimRightOut);
    hit_shapeCmp(intersect, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHitShape with different number of dimensions: ");
    intersect = hit_shapeIntersect(sh, shOtrasDim);
    hit_shapeCmp(intersect, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: ");
    intersect = hit_shapeIntersect(sh, shStd);
    res = hit_shape(2, hit_sig(2, 8, 2), hit_sig(3, 7, 4));
    hit_shapeCmp(intersect, res) ? printf("OK\n") : printf("ERROR\n");

#else

    // HitShape NULL
    intersect = hit_shapeIntersect(sh, shNull);
    if (! hit_shapeCmp(intersect, shNull)) ok = 0;

    // With itself
    intersect = hit_shapeIntersect(sh, sh);
    if (! hit_shapeCmp(intersect, sh)) ok = 0;

    // Right limit in dimension 0 in
    intersect = hit_shapeIntersect(sh, shLimRightIn);
    res = hit_shape(2, hit_sigIndex(9), sig0);
    if (! hit_shapeCmp(intersect, res)) ok = 0;

    // Right limit in dimension 0 out
    intersect = hit_shapeIntersect(sh, shLimRightOut);
    if (! hit_shapeCmp(intersect, shNull)) ok = 0;

    // HitShape with different number of dimensions
    intersect = hit_shapeIntersect(sh, shOtrasDim);
    if (! hit_shapeCmp(intersect, shNull)) ok = 0;

    // Standard case
    intersect = hit_shapeIntersect(sh, shStd);
    res = hit_shape(2, hit_sig(2, 8, 2), hit_sig(3, 7, 4));
    if (! hit_shapeCmp(intersect, res)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}