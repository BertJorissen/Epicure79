/**
 * Test for HitTile functions hit_tileTile2Array and hit_tileArray2Tile
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile;
    hit_tileDomain(&tile, int, 3, 10, 10, 10);
    int localTile[10], localArray[10];  // local and global coordinates from Tile coordinates
    int globalTile[10], globalArray[10];// local and global coordinates from Array coordinates
    HitSig sig = hit_tileDimSig(tile, 0);

    // Tile to Array
    for(int i=sig.begin, j=0; i<hit_tileDimCard(tile,0); i++, j++){
        localTile[j] = i;
        globalTile[j] = hit_tileTile2Array(tile, 0, i);
    }

    // Array to Tile
    for(int i=sig.begin, j=0; i<=sig.end; i+=sig.stride, j++){
        globalArray[j] = i;
        localArray[j] = hit_tileArray2Tile(tile, 0, i);
    }


    printf("Check hit_tileTile2Array and hit_tileArray2Tile: ");
    if(arrayCmp(localTile, localArray, 10, 10) && arrayCmp(globalTile, globalArray, 10, 10)) printf("OK\n");
    else printf("ERROR\n");

    return 0;
}