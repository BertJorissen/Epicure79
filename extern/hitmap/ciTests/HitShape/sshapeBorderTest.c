/**
 * Test for HitShape function hit_shapeBorder
 * 
 * Cases checked:
 *      * Specifying a dimension index smaller than 0
 *          Expected result: a HitShape equal to itself
 *      * Specifying a dimension index equal to the number of dimensions
 *          Expected result: a HitShape equal to itself
 *      * Dimension 0, starting border, no offset
 *          Expected result: a HitShape equal in all dimensions but in the first one,
 *              where it has only one index: the smaller, and the same offset of that dimension
 *      * Dimension 0, starting border, positive offset
 *          Expected result: a HitShape equal in all dimensions but in the first one,
 *              where it has only one index: the smaller minus the offset, and the same offset of that dimension
 *      * Dimension 0, starting border, negative offset
 *          Expected result: a HitShape equal in all dimensions but in the first one,
 *              where it has only one index: the smaller plus the offset, and the same offset of that dimension
 *      * Dimension 1, ending border, positive offset
 *          Expected result: a HitShape equal in all dimensions but in the last one,
 *              where it has only one index: the bigger plus the offset, and the same offset of that dimension
 *      * Dimension 0, ending border, negative offset
 *          Expected result: a HitShape equal in all dimensions but in the last one,
 *              where it has only one index: the bigger minus the offset, and the same offset of that dimension
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(2, 8, 2);
    HitSig sig;
    HitShape sh = hit_shape(2, sig0, sig1);
    HitShape border, res;
    int ok = 1;

    printf("Check hit_shapeBorder: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tSmall index of dimension: ");
    border = hit_shapeBorder(sh, -1, HIT_SHAPE_BEGIN, 0);
    hit_shapeCmp(border, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBig index of dimension: ");
    border = hit_shapeBorder(sh, 2, HIT_SHAPE_BEGIN, 0);
    hit_shapeCmp(border, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNo offset, HIT_SHAPE_BEGIN: ");
    border = hit_shapeBorder(sh, 0, HIT_SHAPE_BEGIN, 0);
    sig = hit_sig(sig0.begin, sig0.begin, sig0.stride);
    res = hit_shape(2, sig, sig1);
    hit_shapeCmp(border, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset, HIT_SHAPE_BEGIN: ");
    border = hit_shapeBorder(sh, 0, HIT_SHAPE_BEGIN, 1);
    sig = hit_sig(sig0.begin-1, sig0.begin-1, sig0.stride);
    res = hit_shape(2, sig, sig1);
    hit_shapeCmp(border, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset, HIT_SHAPE_BEGIN: ");
    border = hit_shapeBorder(sh, 0, HIT_SHAPE_BEGIN, -1);
    sig = hit_sig(sig0.begin+1, sig0.begin+1, sig0.stride);
    res = hit_shape(2, sig, sig1);
    hit_shapeCmp(border, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset, HIT_SHAPE_END: ");
    border = hit_shapeBorder(sh, 1, HIT_SHAPE_END, 1);
    sig = hit_sig(getLastIndex(sig1)+1, getLastIndex(sig1)+1, sig1.stride);
    res = hit_shape(2, sig0, sig);
    hit_shapeCmp(border, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset, HIT_SHAPE_END: ");
    border = hit_shapeBorder(sh, 1, HIT_SHAPE_END, -1);
    sig = hit_sig(getLastIndex(sig1)-1, getLastIndex(sig1)-1, sig1.stride);
    res = hit_shape(2, sig0, sig);
    hit_shapeCmp(border, res) ? printf("OK\n") : printf("ERROR\n");

#else

    // Small index of dimension
    border = hit_shapeBorder(sh, -1, HIT_SHAPE_BEGIN, 0);
    if (! hit_shapeCmp(border, sh)) ok = 0;

    // Big index of dimension
    border = hit_shapeBorder(sh, 2, HIT_SHAPE_BEGIN, 0);
    if (! hit_shapeCmp(border, sh)) ok = 0;

    // No offset, HIT_SHAPE_BEGIN
    border = hit_shapeBorder(sh, 0, HIT_SHAPE_BEGIN, 0);
    sig = hit_sig(sig0.begin, sig0.begin, sig0.stride);
    res = hit_shape(2, sig, sig1);
    if (! hit_shapeCmp(border, res)) ok = 0;

    // Positive offset, HIT_SHAPE_BEGIN
    border = hit_shapeBorder(sh, 0, HIT_SHAPE_BEGIN, 1);
    sig = hit_sig(sig0.begin-1, sig0.begin-1, sig0.stride);
    res = hit_shape(2, sig, sig1);
    if (! hit_shapeCmp(border, res)) ok = 0;

    // Negative offset, HIT_SHAPE_BEGIN
    border = hit_shapeBorder(sh, 0, HIT_SHAPE_BEGIN, -1);
    sig = hit_sig(sig0.begin+1, sig0.begin+1, sig0.stride);
    res = hit_shape(2, sig, sig1);
    if (! hit_shapeCmp(border, res)) ok = 0;

    // Positive offset, HIT_SHAPE_END
    border = hit_shapeBorder(sh, 1, HIT_SHAPE_END, 1);
    sig = hit_sig(getLastIndex(sig1)+1, getLastIndex(sig1)+1, sig1.stride);
    res = hit_shape(2, sig0, sig);
    if (! hit_shapeCmp(border, res)) ok = 0;

    // Negative offset, HIT_SHAPE_END
    border = hit_shapeBorder(sh, 1, HIT_SHAPE_END, -1);
    sig = hit_sig(getLastIndex(sig1)-1, getLastIndex(sig1)-1, sig1.stride);
    res = hit_shape(2, sig0, sig);
    if (! hit_shapeCmp(border, res)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}