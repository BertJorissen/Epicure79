/**
 * Test for HitShape functions hit_shapeDimExpand and hit_shapeExpand
 * 
 * Both functions are deprecated and have been replaced by hit_shapeTransform
 * in the Hitmap examples
 * 
 * Cases checked (hit_shapeDimExpand):
 *      * Expanding a null HitShape
 *          Expected result: a null HitShape
 *      * Specifying a number of dimension smaller than the index of the first dimension
 *          Expected result: a HitShape equal to itself
 *      * Specifying a number of dimension bigger than the index of the last dimension
 *          Expected result: a HitShape equal to itself
 *      * Positive offset in the starting border of the dimension:
 *          Expected result: a HitShape equal but in the specified dimension, where its begin index
 *              is decremented in offset positions
 *      * Negative offset in the starting border of the dimension:
 *          Expected result: a HitShape equal but in the specified dimension, where its begin index
 *              is incremented in offset positions
 *      * Positive offset in the ending border of the dimension:
 *          Expected result: a HitShape equal but in the specified dimension, where its end index
 *              is decremented in offset positions
 *      * Negative offset in the ending border of the dimension:
 *          Expected result: a HitShape equal but in the specified dimension, where its end index
 *              is incremented in offset positions
 * 
 * Cases checked (hit_shapeExpand):
 *      * Expanding a null HitShape
 *          Expected result: a null HitShape
 *      * Specifying a negative number of dimensions
 *          Expected result: a HitShape equal to itself
 *      * Positive offset, many dimensions
 *          Expected result: a HitShape with the begin index and the end index of the specified dimensions
 *              decremented and incremented respectively by the offset
 *      * Negative offset, many dimensions
 *          Expected result: a HitShape with the begin and the end of the specified dimensions
 *              incremented and decremented respectively by the offset
 *      * Positive offset, bigger number of dimensions
 *          Expected result: the operation is applyied to all the dimensions of the HitShape
 *      * Negative offset, bigger number of dimensions
 *          Expected result: the operation is applyied to all the dimensions of the HitShape
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(2, 8, 2);
    HitShape sh = hit_shape(3, sig0, sig1, sig0);
    HitShape shNull = HIT_SHAPE_NULL;
    HitShape res;
    int b = 1, ok = 1;

    printf("Check hit_shapeDimExpand: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tHitShape NULL: ");
    res = hit_shapeDimExpand(shNull, 1, HIT_SHAPE_BEGIN, 5);
    hit_shapeCmp(res, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tSmaller index of dimension: ");
    res = hit_shapeDimExpand(sh, -1, HIT_SHAPE_BEGIN, 5);
    hit_shapeCmp(res, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBigger index of dimension: ");
    res = hit_shapeDimExpand(sh, 3, HIT_SHAPE_BEGIN, 5);
    hit_shapeCmp(res, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_BEGIN: ");
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_BEGIN, 5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin - 5 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_BEGIN: ");
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_BEGIN, -5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin + 5 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_END: ");
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_END, 5);
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end + 5 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_END: ");
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_END, -5);
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end - 5 ? printf("OK\n") : printf("ERROR\n");

#else

    // HitShape NULL
    res = hit_shapeDimExpand(shNull, 1, HIT_SHAPE_BEGIN, 5);
    if (! hit_shapeCmp(res, shNull)) ok = 0;

    // Smaller index of dimension
    res = hit_shapeDimExpand(sh, -1, HIT_SHAPE_BEGIN, 5);
    if (! hit_shapeCmp(res, sh)) ok = 0;

    // Bigger index of dimension
    res = hit_shapeDimExpand(sh, 3, HIT_SHAPE_BEGIN, 5);
    if (! hit_shapeCmp(res, sh)) ok = 0;
    
    // Positive offset HIT_SHAPE_BEGIN
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_BEGIN, 5);
    if (hit_shapeSig(res, 2).begin != hit_shapeSig(sh, 2).begin - 5) ok = 0;

    // Negative offset HIT_SHAPE_BEGIN
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_BEGIN, -5);
    if (hit_shapeSig(res, 2).begin != hit_shapeSig(sh, 2).begin + 5) ok = 0;

    // Positive offset HIT_SHAPE_END
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_END, 5);
    if (hit_shapeSig(res, 2).end != hit_shapeSig(sh, 2).end + 5) ok = 0;

    // Negative offset HIT_SHAPE_END: ");
    res = hit_shapeDimExpand(sh, 2, HIT_SHAPE_END, -5);
    if (hit_shapeSig(res, 2).end != hit_shapeSig(sh, 2).end - 5) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    printf("Check hit_shapeExpand: ");
    ok = 1;

#ifdef DEBUG

    printf("\n");
    //
    printf("\tHitShape NULL: ");
    res = hit_shapeExpand(shNull, 1, 5);
    hit_shapeCmp(res, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tSmaller number of dimensions: ");
    res = hit_shapeExpand(sh, -1, 5);
    hit_shapeCmp(res, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tMany dimensions, positive offset: ");
    res = hit_shapeExpand(sh, 2, 5);
    for(int i = 0; i<2 && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin - 5 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end + 5)
            b = 0;
    }
    b == 1 ? printf("OK\n") : printf("ERROR\n");
    //
    b = 1;
    printf("\tMany dimensions, negative offset: ");
    res = hit_shapeExpand(sh, 2, -10);
    for(int i = 0; i<2 && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin + 10 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end - 10)
            b = 0;
    }
    b == 1 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBigger number of dimensions, positive offset: ");
    b = 1;
    res = hit_shapeExpand(sh, 4, 5);
    for(int i = 0; i<hit_sshapeDims(res) && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin - 5 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end + 5)
            b = 0;
    }
    b == 1 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBigger number of dimensions, negative offset: ");
    res = hit_shapeExpand(sh, 4, -10);
    for(int i = 0; i<hit_sshapeDims(res) && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin + 10 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end - 10)
            b = 0;
    }
    b == 1 ? printf("OK\n") : printf("ERROR\n");
    
#else

    // HitShape NULL
    res = hit_shapeExpand(shNull, 1, 5);
    if (! hit_shapeCmp(res, shNull)) ok = 0;

    //Smaller number of dimensions
    res = hit_shapeExpand(sh, -1, 5);
    if (! hit_shapeCmp(res, sh)) ok = 0;

    // Many dimensions, positive offset
    res = hit_shapeExpand(sh, 2, 5);
    for(int i = 0; i<2 && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin - 5 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end + 5){
                ok = 0;
                break;
            }
    }

    // Many dimensions, negative offset
    res = hit_shapeExpand(sh, 2, -10);
    for(int i = 0; i<2 && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin + 10 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end - 10){
                ok = 0;
                break;
            }
    }

    // Bigger number of dimensions, positive offset
    res = hit_shapeExpand(sh, 4, 5);
    for(int i = 0; i<hit_sshapeDims(res) && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin - 5 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end + 5){
                ok = 0;
                break;
            }
    }

    // Bigger number of dimensions, negative offset
    res = hit_shapeExpand(sh, 4, -10);
    for(int i = 0; i<hit_sshapeDims(res) && b==1; i++){
        if( hit_shapeSig(res, i).begin != hit_shapeSig(sh, i).begin + 10 &&
            hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end - 10){
                ok = 0;
                break;
            }
    }

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}