/**
 * Test for HitTile constructors hit_tileDomainShape and hit_tileDomainShapeAlloc (HitShape based)
 * and functions hit_tileShape and hit_tileShapeLocal
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile;
    HitSig sig = hit_sig(1,10,2);
    HitShape sh = hit_shape2(sig, sig);
    HitShape sh2, sh3, shLocal;
    shLocal = hit_shape(2, hit_sigStd(5), hit_sigStd(5));
    hit_tileDomainShape(&tile, int, sh);
    sh2 = hit_tileShape(tile);
    sh3 = hit_tileShapeLocal(&tile);

    printf("Check hit_tileDomainShape and hit_tileShape: ");
    hit_shapeCmp(sh, sh2) && tile.memStatus == HIT_MS_NOMEM ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDomainShapeAlloc: ");
    hit_tileDomainShapeAlloc(&tile, int, sh);
    sh2 = hit_tileShape(tile);
    hit_shapeCmp(sh, sh2) && tile.memStatus == HIT_MS_OWNER ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileShapeLocal: ");
    hit_shapeCmp(sh3, shLocal) && hit_shapeCmp(shLocal, hit_tileShapeLocal(&tile)) ? printf("OK\n") : printf("ERROR\n");

    return 0;
}