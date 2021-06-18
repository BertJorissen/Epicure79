/**
 * Test for HitSig functions hit_sigTileToArray and hit_sigArrayToTile.
 * These functions transform an index of a HitSig from tile (local) coordinates
 * to array (ancestors) coordinates and viceversa.
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig = hit_sig(0, 50, 4);
    int card = hit_sigCard(sig);
    int localTile[card], localArray[card];  // local and global coordinates from Tile coordinates
    int globalTile[card], globalArray[card];// local and global coordinates from Array coordinates

    // Tile to Array
    for(int i=sig.begin, j=0; i<card; i++, j++){
        localTile[j] = i;
        globalTile[j] = hit_sigTileToArray(sig, i);
    }

    // Array to Tile
    for(int i=sig.begin, j=0; i<=sig.end; i+=sig.stride, j++){
        globalArray[j] = i;
        localArray[j] = hit_sigArrayToTile(sig, i);
    }


    printf("Check hit_sigTileToArray and hit_sigArrayToTile: ");
    if(arrayCmp(localTile, localArray, card, card) && arrayCmp(globalTile, globalArray, card, card)) printf("OK\n");
    else printf("ERROR\n");


#ifdef DEBUG

    printf("Local (tile) coordinates:\n");
    printArray(localArray, card); printArray(localTile, card);
    printf("Global (array) coordinates:\n");
    printArray(globalArray, card); printArray(globalTile, card);
    
#endif

    return 0;
}