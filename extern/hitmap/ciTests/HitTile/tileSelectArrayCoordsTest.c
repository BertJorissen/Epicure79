/**
 * Test for HitTile functions hit_tileSelectArrayCoords and hit_tileSelectArrayCoordsNoBoundary
 * 
 * Cases checked:
 *      * A null HitShape
 *          Expected result: a null HitTile
 * 
 *      * A HitSig with begin index out of bounds of that dimension
 *          Expected result (arrayCoords): a null HitTile
 *          Expected result (arrayCoordsNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 *      * A HitSig with begin index not in that dimension in array coords
 *          Expected result (arrayCoords): a null HitTile
 *          Expected result (arrayCoordsNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 * 
 *      * A HitSig with end index out of bounds of that dimension
 *          Expected result (arrayCoords): a null HitTile
 *          Expected result (arrayCoordsNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 *      * A HitSig with end index not in that dimension in array coords
 *          Expected result (arrayCoords): a null HitTile
 *          Expected result (arrayCoordsNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 * 
 *      * A HitSig with stride not divisible to the stride of that dimension
 *          Expected result (arrayCoords): a null HitTile
 *          Expected result (arrayCoordsNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 *      * A HitSig with no stride
 *          Expected result (arrayCoords): a null HitTile
 *          Expected result (arrayCoordsNoBoundary): this test can't be done in this function because
 *              it will do a division by 0
 * 
 *      * A HitSig with the end smaller than the begin
 *          Expected result (tileSelect): a null HitTile
 *          Expected result (tileSelectNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 * 
 *      * Select the whole HitShape
 *          Expected result: a HitTile with the same HitShape as the original
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile, tileChild;
    HitSig sig = hit_sig(1, 20, 2);
    HitSig sig2 = hit_sig(1, 19, 2);
    HitSig sigBeginOut = hit_sig(-1, 5, 2);
    HitSig sigBeginNotIn = hit_sig(2, 5, 2);
    HitSig sigEndOut = hit_sig(1, 21, 2);
    HitSig sigEndNotIn = hit_sig(1, 20, 2);
    HitSig sigInv = hit_sig(5, 1, 2);
    HitSig sigStride = hit_sig(1, 5, 3);
    HitSig sigNoStride = hit_sig(1, 5, 0);
    HitShape sh = hit_shape(2, sig, sig);
    HitShape shBeginOut = hit_shape(2, sig2, sigBeginOut);
    HitShape shBeginNotIn = hit_shape(2, sig2, sigBeginNotIn);
    HitShape shEndOut = hit_shape(2, sig2, sigEndOut);
    HitShape shEndNotIn = hit_shape(2, sig2, sigEndNotIn);
    HitShape shInverted = hit_shape(1, sigInv);
    HitShape shStride = hit_shape(2, sigStride, sig2);
    HitShape shNoStride = hit_shape(2, sigNoStride, sig2);
    HitShape res;
    int ok = 1;

    hit_tileDomainShapeAlloc(&tile, int, sh);
    
    printf("Check hit_tileSelectArrayCoords: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, HIT_SHAPE_NULL);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBegin out of bounds: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shBeginOut);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBegin not in the domain: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shBeginNotIn);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd out of bounds: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shEndOut);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd not in the domain: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shEndNotIn);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd smaller than begin: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shInverted);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNo stride: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shNoStride);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStride not divisible: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, shStride);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHIT_SHAPE_WHOLE: ");
    hit_tileSelectArrayCoords(&tileChild, &tile, HIT_SHAPE_WHOLE);
    hit_shapeCmp(sh, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape
    hit_tileSelectArrayCoords(&tileChild, &tile, HIT_SHAPE_NULL);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // Begin out of bounds
    hit_tileSelectArrayCoords(&tileChild, &tile, shBeginOut);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // Begin not in the domain
    hit_tileSelectArrayCoords(&tileChild, &tile, shBeginNotIn);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // End out of bounds
    hit_tileSelectArrayCoords(&tileChild, &tile, shEndOut);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // End not in the domain
    hit_tileSelectArrayCoords(&tileChild, &tile, shEndNotIn);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // End smaller than begin
    hit_tileSelectArrayCoords(&tileChild, &tile, shInverted);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // No stride
    hit_tileSelectArrayCoords(&tileChild, &tile, shNoStride);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // Stride not divisible
    hit_tileSelectArrayCoords(&tileChild, &tile, shStride);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // HIT_SHAPE_WHOLE
    hit_tileSelectArrayCoords(&tileChild, &tile, HIT_SHAPE_WHOLE);
    if(! hit_shapeCmp(sh, hit_tileShape(tileChild))) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    ok = 1;
    printf("Check hit_tileSelectArrayCoordsNoBoundary: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, HIT_SHAPE_NULL);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBegin out of bounds: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shBeginOut);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shBeginOut));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBegin not in the domain: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shBeginNotIn);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shBeginNotIn));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n"); 
    //
    printf("\tEnd out of bounds: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shEndOut);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shEndOut));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd not in the domain: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shEndNotIn);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shEndNotIn));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd smaller than begin: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shInverted);
    res = hit_shape(2, sigInv, sig);
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStride not divisible: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shStride);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shStride));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHIT_SHAPE_WHOLE: ");
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, HIT_SHAPE_WHOLE);
    res = hit_shape(2, sig, sig);
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, HIT_SHAPE_NULL);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // Begin out of bounds
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shBeginOut);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shBeginOut));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // Begin not in the domain
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shBeginNotIn);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shBeginNotIn));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0; 
    
    // End out of bounds
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shEndOut);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shEndOut));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // End not in the domain
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shEndNotIn);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shEndNotIn));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // End smaller than begin
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shInverted);
    res = hit_shape(2, sigInv, sig);
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // Stride not divisible
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, shStride);
    res = hit_shapeTileToArray(sh, hit_shapeArrayToTile(sh, shStride));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // HIT_SHAPE_WHOLE
    hit_tileSelectArrayCoordsNoBoundary(&tileChild, &tile, HIT_SHAPE_WHOLE);
    res = hit_shape(2, sig, sig);
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}