/**
 * Test for HitTile functions hit_tileClone and hit_tileClass
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile, tile2;
    HitSig sig = hit_sig(1, 20, 2);
    HitShape sh = hit_shape(1, sig);
    int ok = 1, a = 3, i;

    hit_tileDomainShapeAlloc(&tile, int, sh);
    hit_tileFill(&tile, &a);
    hit_tileClone(&tile2, &tile);
    
    printf("Check hit_tileClone: ");
    if(! hit_shapeCmp(sh, hit_tileShape(tile2))) ok = 0;

    hit_tileForDimDomain(tile2, 0, i){
        if(hit_tileElemAt(tile2, 1, i) != a) ok = 0;
    }

    ok ? printf("OK\n") : printf("ERROR\n");

    //
    printf("Check hit_tileClass: ");
    hit_tileClass(tile) == HIT_TILE && hit_tileClass(tile2) == HIT_TILE && hit_tileClass(HIT_TILE_NULL) == HIT_TILE ?
        printf("OK\n") : printf("ERROR\n");
        

    return 0;
}