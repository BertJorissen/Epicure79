/**
 * Test for HitShape function hit_shapeTransform
 * 
 * Cases checked:
 *      * Transfoming a null HitShape
 *          Expected result: a null HitShape
 *      * Specifying a bigger index of dimension
 *          Expected result: a HitShape equal to itself
 *      * Specifying a smaller index of dimension
 *          Expected result: a HitShape equal to itself
 * 
 *      * Positive offset, HIT_SHAPE_BEGIN
 *          Expected result: a HitShape equal but in the specified dimension, where its begin index
 *              is incremented in offset*stride positions
 *      * Negative offset, HIT_SHAPE_BEGIN
 *          Expected result: a HitShape equal but in the specified dimension, where its begin index
 *              is decremented in offset*stride positions
 * 
 *      * Positive offset, HIT_SHAPE_END
 *          Expected result: a HitShape equal but in the specified dimension, where its end index
 *              is incremented in offset*stride positions
 *      * Negative offset, HIT_SHAPE_END
 *          Expected result: a HitShape equal but in the specified dimension, where its end index
 *              is decremented in offset*stride positions
 *      * Negative offset, HIT_SHAPE_END, raw coordinates, all dimensions
 *          Expected result: a HitShape with the end index of all dimensions decremented in offset positions
 * 
 *      * Positive offset, HIT_SHAPE_MOVE
 *          Expected result: a HitShape equal but in the specified dimension, where its begin and end indexes
 *              are incremented in offset*stride positions
 *      * Negative offset, HIT_SHAPE_MOVE
 *          Expected result: a HitShape equal but in the specified dimension, where its begin and end indexes
 *              are decremented in offset*stride positions
 * 
 *      * Positive offset, HIT_SHAPE_STRETCH
 *          Expected result: a HitShape equal but in the specified dimension, where its begin and end indexes
 *              are decremented and incremented, respectively, in offset*stride positions
 *      * Negative offset, HIT_SHAPE_STRETCH
 *          Expected result: a HitShape equal but in the specified dimension, where its begin and end indexes
 *              are incremented and decremented, respectively, in offset*stride positions
 * 
 *      * Positive offset, HIT_SHAPE_FIRST
 *          Expected result: a HitShape equal but in the specified dimension, where its end has been modified so that the
 *              cardinality matches the offset
 *      * Positive offset, HIT_SHAPE_FIRST, raw coordinates
 *          Expected result: a HitShape equal but in the specified dimension, where its end has been modified so that the
 *              cardinality matches the greatest integer near offset/stride
 * 
 *      * Positive offset, HIT_SHAPE_LAST
 *          Expected result: a HitShape equal but in the specified dimension, where its begin has been modified so that the
 *              cardinality matches the offset
 *      * Positive offset, HIT_SHAPE_LAST, raw coordinates
 *          Expected result: a HitShape equal but in the specified dimension, where its begin has been modified so that the
 *              cardinality matches the greatest integer near offset/stride
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include <math.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(2, 8, 2);
    HitShape sh = hit_shape(3, sig0, sig1, sig0);
    HitShape shNull = HIT_SHAPE_NULL;
    HitShape res;
    int ok = 1; //, b=1;

    printf("Check hit_shapeDimTransform: ");
    
#ifdef DEBUG

    printf("\n");
    //
    printf("\tHitShape NULL: ");
    res = hit_shapeTransform(shNull, 1, HIT_SHAPE_BEGIN, 5);
    hit_shapeCmp(res, shNull) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBigger index of dimension: ");
    res = hit_shapeTransform(sh, 3, HIT_SHAPE_BEGIN, 5);
    hit_shapeCmp(res, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tSmaller index of dimension: ");
    res = hit_shapeTransform(sh, -2, HIT_SHAPE_BEGIN, 5);
    hit_shapeCmp(res, res) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_BEGIN: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_BEGIN, 5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin + 5*hit_shapeSig(sh, 2).stride ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_BEGIN: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_BEGIN, -5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin - 5*hit_shapeSig(sh, 2).stride ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_END: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_END, 5);
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end + 5*hit_shapeSig(sh, 2).stride ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_END: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_END, -5);
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end - 5*hit_shapeSig(sh, 2).stride ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_END, all dimensions, raw coordinates: ");
    res = hit_shapeTransform(sh, HIT_SHAPE_ALLDIMS, HIT_SHAPE_RAW | HIT_SHAPE_END, -5);
    for(int i = 0; i<hit_sshapeDims(res); i++){
        if( hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end - 5){
            b = 0;
            break;
        }
    }
    b == 1 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_MOVE: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_MOVE, 5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin + 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end + 5*hit_shapeSig(sh, 2).stride
    ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_MOVE: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_MOVE, -5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin - 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end - 5*hit_shapeSig(sh, 2).stride
    ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_STRETCH: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_STRETCH, 5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin - 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end + 5*hit_shapeSig(sh, 2).stride
    ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNegative offset HIT_SHAPE_STRETCH: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_STRETCH, -5);
    hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin + 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end - 5*hit_shapeSig(sh, 2).stride
    ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_FIRST: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_FIRST, 5);
    (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin &&
    hit_sigCard(hit_shapeSig(res, 2)) == 5 ) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_FIRST, raw coordinates: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_FIRST, 5);
    (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin &&
    hit_sigCard(hit_shapeSig(res, 2)) == ceil(5 / hit_shapeSig(res, 2).stride)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_LAST: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_LAST, 5);
    (hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end &&
    hit_sigCard(hit_shapeSig(res, 2)) == 5 ) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tPositive offset HIT_SHAPE_LAST, raw coordinates: ");
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_LAST, 5);
    (hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end &&
    hit_sigCard(hit_shapeSig(res, 2)) == ceil(5 / hit_shapeSig(res, 2).stride)) ? printf("OK\n") : printf("ERROR\n");
    

#else

    // HitShape NULL
    res = hit_shapeTransform(shNull, 1, HIT_SHAPE_BEGIN, 5);
    if (! hit_shapeCmp(res, shNull)) ok = 0;

    // Bigger index of dimension
    res = hit_shapeTransform(sh, 3, HIT_SHAPE_BEGIN, 5);
    if (! hit_shapeCmp(res, sh)) ok = 0;

    // Smaller index of dimension
    res = hit_shapeTransform(sh, -2, HIT_SHAPE_BEGIN, 5);
    if (! hit_shapeCmp(res, sh)) ok = 0;

    // Positive offset HIT_SHAPE_BEGIN
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_BEGIN, 5);
    if (hit_shapeSig(res, 2).begin != hit_shapeSig(sh, 2).begin + 5*hit_shapeSig(sh, 2).stride) ok = 0;

    // Negative offset HIT_SHAPE_BEGIN
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_BEGIN, -5);
    if (hit_shapeSig(res, 2).begin != hit_shapeSig(sh, 2).begin - 5*hit_shapeSig(sh, 2).stride) ok = 0;

    // Positive offset HIT_SHAPE_END
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_END, 5);
    if (hit_shapeSig(res, 2).end != hit_shapeSig(sh, 2).end + 5*hit_shapeSig(sh, 2).stride) ok = 0;

    // Negative offset HIT_SHAPE_END
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_END, -5);
    if (hit_shapeSig(res, 2).end != hit_shapeSig(sh, 2).end - 5*hit_shapeSig(sh, 2).stride) ok = 0;

    // Negative offset HIT_SHAPE_END, all dimensions, raw coordinates
    res = hit_shapeTransform(sh, HIT_SHAPE_ALLDIMS, HIT_SHAPE_RAW | HIT_SHAPE_END, -5);
    for(int i = 0; i<hit_sshapeDims(res); i++){
        if( hit_shapeSig(res, i).end != hit_shapeSig(sh, i).end - 5){
            ok = 0;
            break;
        }
    }

    // Positive offset HIT_SHAPE_MOVE
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_MOVE, 5);
    if (! (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin + 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end + 5*hit_shapeSig(sh, 2).stride)) ok = 0;

    // Negative offset HIT_SHAPE_MOVE
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_MOVE, -5);
    if (! (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin - 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end - 5*hit_shapeSig(sh, 2).stride)) ok = 0;
    
    // Positive offset HIT_SHAPE_STRETCH
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_STRETCH, 5);
    if(! (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin - 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end + 5*hit_shapeSig(sh, 2).stride)) ok = 0;

    // Negative offset HIT_SHAPE_STRETCH
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_STRETCH, -5);
    if(! (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin + 5*hit_shapeSig(sh, 2).stride &&
    hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end - 5*hit_shapeSig(sh, 2).stride)) ok = 0;

    // Positive offset HIT_SHAPE_FIRST
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_FIRST, 5);
    if (! (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin &&
    hit_sigCard(hit_shapeSig(res, 2)) == 5 )) ok = 0;

    // Positive offset HIT_SHAPE_FIRST, raw coordinates
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_FIRST, 5);
    if (! (hit_shapeSig(res, 2).begin == hit_shapeSig(sh, 2).begin &&
    hit_sigCard(hit_shapeSig(res, 2)) == ceil(5 / hit_shapeSig(res, 2).stride))) ok = 0;

    // Positive offset HIT_SHAPE_LAST
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_LAST, 5);
    if (! (hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end &&
    hit_sigCard(hit_shapeSig(res, 2)) == 5 )) ok = 0;

    // Positive offset HIT_SHAPE_LAST, raw coordinates
    res = hit_shapeTransform(sh, 2, HIT_SHAPE_LAST, 5);
    if (! (hit_shapeSig(res, 2).end == hit_shapeSig(sh, 2).end &&
    hit_sigCard(hit_shapeSig(res, 2)) == ceil(5 / hit_shapeSig(res, 2).stride))) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}
