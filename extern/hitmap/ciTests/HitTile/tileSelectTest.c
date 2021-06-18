/**
 * Test for HitTile functions hit_tileSelect and hit_tileSelectNoBoundary
 * 
 * Cases checked:
 *      * A null HitShape
 *          Expected result: a null HitTile
 *      * A HitSig with begin < 0
 *          Expected result (tileSelect): a null HitTile
 *          Expected result (tileSelectNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 *      * A HitSig with end > cardinality
 *          Expected result (tileSelect): a null HitTile
 *          Expected result (tileSelectNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 *      * A HitSig with the end smaller than the begin
 *          Expected result (tileSelect): a null HitTile
 *          Expected result (tileSelectNoBoundary): the selection is performed normally. In that
 *              dimension, the result is the same as applying the function hit_sigBlend
 *      * A HitSig with no stride
 *          Expected result (tileSelect): a null HitTile
 *          Expected result (tileSelectNoBoundary): this test can't be done in this function because
 *              it will do a division by 0
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
    HitSig sigBegin = hit_sig(-1, 5, 1);
    HitSig sigEnd = hit_sig(0, 11, 1);
    HitSig sigTile = hit_sigStd(10);
    HitSig sigInv = hit_sig(5, 0, 1);
    HitSig sigNoStride = hit_sig(0, 5, 0);
    HitShape sh = hit_shape(2, sig, sig);
    HitShape shBegin = hit_shape(2, sigTile, sigBegin);
    HitShape shEnd = hit_shape(2, sigTile, sigEnd);  //Should it be hit_sig(0, 10, 1) ¿? (==cardinality)
    HitShape shInverted = hit_shape(1, sigInv);
    HitShape shNoStride = hit_shape(2, sigNoStride, sigTile);
    HitShape shStd = hit_shape(2, sig2, sig2);
    HitShape res;
    int ok = 1;

    hit_tileDomainShapeAlloc(&tile, int, sh);
    
    printf("Check hit_tileSelect: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape: ");
    hit_tileSelect(&tileChild, &tile, HIT_SHAPE_NULL);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBegin smaller than 0: ");
    hit_tileSelect(&tileChild, &tile, shBegin);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd bigger than cardinality: ");
    hit_tileSelect(&tileChild, &tile, shEnd);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd smaller than begin: ");
    hit_tileSelect(&tileChild, &tile, shInverted);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNo stride: ");
    hit_tileSelect(&tileChild, &tile, shNoStride);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHIT_SHAPE_WHOLE: ");
    hit_tileSelect(&tileChild, &tile, HIT_SHAPE_WHOLE);
    hit_shapeCmp(sh, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape
    hit_tileSelect(&tileChild, &tile, HIT_SHAPE_NULL);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // Begin smaller than 0
    hit_tileSelect(&tileChild, &tile, shBegin);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // End bigger than cardinality
    hit_tileSelect(&tileChild, &tile, shEnd);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // End smaller than begin
    hit_tileSelect(&tileChild, &tile, shInverted);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // No stride
    hit_tileSelect(&tileChild, &tile, shNoStride);
    if(! hit_tileIsNull(tileChild)) ok = 0;

    // HIT_SHAPE_WHOLE
    hit_tileSelect(&tileChild, &tile, HIT_SHAPE_WHOLE);
    if(! hit_shapeCmp(sh, hit_tileShape(tileChild))) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    ok = 1;
    printf("Check hit_tileSelectNoBoundary: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tNull HitShape: ");
    hit_tileSelectNoBoundary(&tileChild, &tile, HIT_SHAPE_NULL);
    hit_tileIsNull(tileChild) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tBegin smaller than 0: ");
    hit_tileSelectNoBoundary(&tileChild, &tile, shBegin);
    res = hit_shape(2, sig2, hit_sigBlend(sig, sigBegin));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd bigger than cardinality: ");
    hit_tileSelectNoBoundary(&tileChild, &tile, shEnd);
    res = hit_shape(2, sig2, hit_sigBlend(sig, sigEnd));
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd smaller than begin: ");
    hit_tileSelectNoBoundary(&tileChild, &tile, shInverted);
    res = hit_shape(2, hit_sigBlend(sig, sigInv), sig);
    hit_shapeCmp(res, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tHIT_SHAPE_WHOLE: ");
    hit_tileSelectNoBoundary(&tileChild, &tile, HIT_SHAPE_WHOLE);
    hit_shapeCmp(sh, hit_tileShape(tileChild)) ? printf("OK\n") : printf("ERROR\n");

#else

    // Null HitShape
    hit_tileSelectNoBoundary(&tileChild, &tile, HIT_SHAPE_NULL);
    if(! hit_tileIsNull(tileChild)) ok = 0;
    
    // Begin smaller than 0
    hit_tileSelectNoBoundary(&tileChild, &tile, shBegin);
    res = hit_shape(2, sig2, hit_sigBlend(sig, sigBegin));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // End bigger than cardinality
    hit_tileSelectNoBoundary(&tileChild, &tile, shEnd);
    res = hit_shape(2, sig2, hit_sigBlend(sig, sigEnd));
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;
    
    // End smaller than begin
    hit_tileSelectNoBoundary(&tileChild, &tile, shInverted);
    res = hit_shape(2, hit_sigBlend(sig, sigInv), sig);
    if(! hit_shapeCmp(res, hit_tileShape(tileChild))) ok = 0;

    // HIT_SHAPE_WHOLE
    hit_tileSelectNoBoundary(&tileChild, &tile, HIT_SHAPE_WHOLE);
    if(! hit_shapeCmp(sh, hit_tileShape(tileChild))) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}