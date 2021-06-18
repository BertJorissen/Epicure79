/**
 * Test for HitTile functions hit_tileCheckBoundaryTileCoords and hit_tileCheckBoundaryArrayCoords
 * 
 * Cases checked (hit_tileCheckBoundaryTileCoords):
 *      * A HitSig with begin index < 0
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with end index equal to the cardinality of the Tile in that dimension
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with the end smaller than the begin
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with no stride
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with begin 0 and end equal to the cardinality-1
 *          Expected result: HIT_NO_OUTOFBOUNDS_CHECK
 * 
 * Cases checked (hit_tileCheckBoundaryArrayCoords):
 *      * A HitSig with begin index smaller than the begin of that dimension
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with end index bigger than the end of that dimension
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with the end smaller than the begin
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with no stride
 *          Expected result: HIT_OUTOFBOUNDS_CHECK
 *      * A HitSig with begin and end indeces equal to those of the HitTile
 *          Expected result: HIT_NO_OUTOFBOUNDS_CHECK
 * 
 * This test does not check a HitShape with a bigger number of dimensions than the HitTile. We may expect
 * the result to be HIT_OUTOFBOUNDS_CHECK, but since the HitSig of a non existing dimension is (0,0,0),
 * the result may be the opposite.
 * 
 * A null HitShape has negative dimensions so it can't be checked against the positive dimensions of the HitTile,
 * therefore the result will be HIT_NO_OUTOFBOUNDS_CHECK
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile;
    HitSig sig = hit_sig(1, 20, 2);
    HitSig sigTile = hit_sig(0, 9, 1);
    HitShape sh = hit_shape(2, sig, sig);
    HitShape shTile = hit_shape(2, sigTile, sigTile);
    HitShape shBegin = hit_shape(2, sigTile, hit_sig(-1, 5, 1));
    HitShape shEnd = hit_shape(2, sigTile, hit_sig(0, 10, 1));
    HitShape shInverted = hit_shape(1, hit_sig(5, 0, 1));
    HitShape shNoStride = hit_shape(2, hit_sig(0, 5, 0), sigTile);
    int ok = 1;
    hit_tileDomainShape(&tile, int, sh);


    printf("Check hit_tileCheckBoundaryTileCoords: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tBegin smaller than 0: ");
    hit_tileCheckBoundaryTileCoords(&tile, shBegin) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd equal to cardinality: ");
    hit_tileCheckBoundaryTileCoords(&tile, shEnd) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd smaller than begin: ");
    hit_tileCheckBoundaryTileCoords(&tile, shInverted) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNo stride: ");
    hit_tileCheckBoundaryTileCoords(&tile, shNoStride) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: ");
    hit_tileCheckBoundaryTileCoords(&tile, shTile) == HIT_NO_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");

#else

    // Begin smaller than 0
    if(hit_tileCheckBoundaryTileCoords(&tile, shBegin) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    // End bigger than cardinality
    if(hit_tileCheckBoundaryTileCoords(&tile, shEnd) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    // End smaller than begin
    if(hit_tileCheckBoundaryTileCoords(&tile, shInverted) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    // No stride
    if(hit_tileCheckBoundaryTileCoords(&tile, shNoStride) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    // Standard case
    if(hit_tileCheckBoundaryTileCoords(&tile, shTile) != HIT_NO_OUTOFBOUNDS_CHECK) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    ok = 1;
    shBegin = hit_shape(2, sig, hit_sig(0, 5, 1));
    shEnd = hit_shape(2, sig, hit_sig(1, 21, 1));
    printf("Check hit_tileCheckBoundaryArrayCoords: ");
    

#ifdef DEBUG

    printf("\n");
    //
    printf("\tBegin index smaller: ");
    hit_tileCheckBoundaryArrayCoords(&tile, shBegin) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd index bigger: ");
    hit_tileCheckBoundaryArrayCoords(&tile, shEnd) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tEnd smaller than begin: ");
    hit_tileCheckBoundaryArrayCoords(&tile, shInverted) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tNo stride: ");
    hit_tileCheckBoundaryArrayCoords(&tile, shNoStride) == HIT_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tStandard case: ");
    hit_tileCheckBoundaryArrayCoords(&tile, sh) == HIT_NO_OUTOFBOUNDS_CHECK ? printf("OK\n") : printf("ERROR\n");

#else

    // Begin index smaller
    if(hit_tileCheckBoundaryArrayCoords(&tile, shBegin) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    
    // End index bigger
    if(hit_tileCheckBoundaryArrayCoords(&tile, shEnd) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    
    // End smaller than begin
    if(hit_tileCheckBoundaryArrayCoords(&tile, shInverted) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    
    // No stride
    if(hit_tileCheckBoundaryArrayCoords(&tile, shNoStride) != HIT_OUTOFBOUNDS_CHECK) ok = 0;
    
    // Standard case
    if(hit_tileCheckBoundaryArrayCoords(&tile, sh) != HIT_NO_OUTOFBOUNDS_CHECK) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}