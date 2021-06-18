/**
 * Test for HitTile functions hit_tileShapeTile2Array and hit_tileShapeArray2Tile
 * 
 * Cases checked (hit_tileShapeTile2Array):
 *      * A null HitShape
 *          Expected result: a null HitShape
 *      * A HitShape with more dimensions than the HitTile
 *          Expected result: the transformation is performed normally. In the big dimensions the result
 *              is a HitSig(0,0,1)
 *      * A HitShape with less dimensions than the HitTile
 *          Expected result: the transformation is performed normally
 * 
 * Cases checked (hit_tileShapeArray2Tile):
 *      * A null HitShape
 *          Expected result: a null HitShape
 *      * A HitShape with more dimensions than the HitTile
 *          This test can't be done in this function because it will perform a division by 0
 *      * A HitShape with less dimensions than the HitTile and a smaller (and divisible) stride
 *          Expected result: the transformation is performed normally. The result stride is the division of the
 *              big stride / the small stride
 *      * A HitShape with less dimensions than the HitTile and a bigger (and divisible) stride
 *          Expected result: the transformation is performed normally. The result stride is the division of the
 *              big stride / the small stride
 *      * An incompatible HitShape
 *          Expected result: a null HitShape
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile;
    HitSig sig = hit_sig(1,20,2);
    HitSig sig2 = hit_sig(5, 10, 1);
    HitSig sig3 = hit_sig(1, 20, 4);
    HitSig sig4 = hit_sig(1, 20, 3);
    HitShape sh = hit_shape(2, sig, sig);
    HitShape shMoreDims = hit_shape(3, sig, sig, sig2);
    HitShape shLessDims = hit_shape(1, sig2);
    HitShape shLessDims2 = hit_shape(1, sig3);
    HitShape shIncomp = hit_shape(2, sig, sig4);
    HitShape res, res2;
    hit_tileDomainShape(&tile, int, sh);
    int ok = 1;

    printf("Check hit_tileShapeTile2Array: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tA null HitShape: ");
    res = hit_tileShapeTile2Array(&tile, HIT_SHAPE_NULL);
    hit_shapeCmp(res, HIT_SHAPE_NULL) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitShape with more dims: ");
    res = hit_tileShapeTile2Array(&tile, shMoreDims);
    res2 = hit_shape(3, hit_sigBlend(sig, sig), hit_sigBlend(sig, sig), hit_sig(0,0,1));
    hit_shapeCmp(res, res2) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitShape with less dims: ");
    res = hit_tileShapeTile2Array(&tile, shLessDims);
    res2 = hit_shape(1, hit_sigBlend(sig, sig2));
    hit_shapeCmp(res, res2) ? printf("OK\n") : printf("ERROR\n");

#else

    // A null HitShape
    res = hit_tileShapeTile2Array(&tile, HIT_SHAPE_NULL);
    if(! hit_shapeCmp(res, HIT_SHAPE_NULL)) ok = 0;
    
    // A HitShape with more dims
    res = hit_tileShapeTile2Array(&tile, shMoreDims);
    res2 = hit_shape(3, hit_sigBlend(sig, sig), hit_sigBlend(sig, sig), hit_sig(0,0,1));
    if(! hit_shapeCmp(res, res2)) ok = 0;
    
    // A HitShape with less dims
    res = hit_tileShapeTile2Array(&tile, shLessDims);
    res2 = hit_shape(1, hit_sigBlend(sig, sig2));
    if(! hit_shapeCmp(res, res2)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif
    
    ok = 1;
    printf("Check hit_tileShapeArray2Tile: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tA null HitShape: ");
    res = hit_tileShapeArray2Tile(&tile, HIT_SHAPE_NULL);
    hit_shapeCmp(res, HIT_SHAPE_NULL) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitShape with less dims, small stride: ");
    res = hit_tileShapeArray2Tile(&tile, shLessDims);
    //printf("%d %d %d\n", hit_shapeSig(res, 0).begin, hit_shapeSig(res, 0).end, hit_shapeSig(res, 0).stride);
    res2 = hit_shapeArrayToTile(hit_shape(1, sig), shLessDims);
    hit_shapeCmp(res, res2) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitShape with less dims, big stride: ");
    res = hit_tileShapeArray2Tile(&tile, shLessDims2);
    res2 = hit_shapeArrayToTile(hit_shape(1, sig), shLessDims2);
    hit_shapeCmp(res, res2) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tIncompatible HitShape: ");
    res = hit_tileShapeArray2Tile(&tile, shIncomp);
    hit_shapeCmp(res, HIT_SHAPE_NULL) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape
    res = hit_tileShapeArray2Tile(&tile, HIT_SHAPE_NULL);
    if(! hit_shapeCmp(res, HIT_SHAPE_NULL)) ok = 0;
    
    // HitShape with less dims, small stride
    res = hit_tileShapeArray2Tile(&tile, shLessDims);
    res2 = hit_shapeArrayToTile(hit_shape(1, sig), shLessDims);
    if(! hit_shapeCmp(res, res2)) ok = 0;
    
    // HitShape with less dims, big stride
    res = hit_tileShapeArray2Tile(&tile, shLessDims2);
    res2 = hit_shapeArrayToTile(hit_shape(1, sig), shLessDims2);
    if(! hit_shapeCmp(res, res2)) ok = 0;
    
    // Incompatible HitShape
    res = hit_tileShapeArray2Tile(&tile, shIncomp);
    if(! hit_shapeCmp(res, HIT_SHAPE_NULL)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}