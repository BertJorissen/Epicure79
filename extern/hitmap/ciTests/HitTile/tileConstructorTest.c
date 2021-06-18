/**
 * Test for HitTile constructors hit_tileSingle (pointing to a variable), hit_tileDomain and
 * hit_tileDomainAlloc (specifying dimensions) and functions hit_tileAlloc, hit_tileFree and hit_tileIsNull
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tileSingle, tileDomain;
    HitTile_int tileNull = HIT_TILE_NULL_STATIC;
    int var = 5, ok = 1, null = 1;
    hit_tileSingle(&tileSingle, var, int);
    hit_tileDomain(&tileDomain, int, 2, 4, 4);

    printf("Check hit_tileSingle: ");
    if (hit_tileIsNull(tileSingle)) null = 0;
    hit_tileCard(tileSingle) == 1 && hit_tileElemAt(tileSingle, 1, 0) == var && tileSingle.memStatus == HIT_MS_NOT_OWNER ?
        printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDomain: ");
    if (hit_tileIsNull(tileDomain)) null = 0;
    hit_tileCard(tileDomain) == 16 && tileDomain.memStatus == HIT_MS_NOMEM ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDomainAlloc: ");
    hit_tileDomainAlloc(&tileDomain, int, 2, 4, 4);
    if (hit_tileIsNull(tileDomain)) null = 0;
    hit_tileCard(tileDomain) == 16 && tileDomain.memStatus == HIT_MS_OWNER ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileAlloc and hit_tileFree: ");
    hit_tileDomain(&tileDomain, int, 2, 4, 4);
    hit_tileAlloc(&tileDomain);
    if (hit_tileIsNull(tileDomain)) null = 0;
    if(tileDomain.memStatus != HIT_MS_OWNER) ok = 0;
    hit_tileFree(tileDomain);
    if (hit_tileIsNull(tileDomain)) null = 0;
    if(tileDomain.memStatus != HIT_MS_NOMEM) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

    //
    printf("Check hit_tileIsNull: ");
    if(! hit_tileIsNull(tileNull)) null = 0;
    null ? printf("OK\n") : printf("ERROR\n");


    return 0;
}