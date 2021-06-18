/**
 * Test for HitTile functions hit_tileCard, hit_tileDimCard and hit_tileDims
 * 
 * This test does not check extreme cases as accessing a dimension bigger than the
 * number of dimensions of the HitTile or working with a null HitTile, because the
 * results might not be consistent.
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"

int main() {

    hit_tileNewType(double);
    HitTile_double tile;
    hit_tileDomain(&tile, double, 3, 10, 10, 10);
    int ok = 1;
    
    printf("Check hit_tileCard: ");
    hit_tileCard(tile) == 10*10*10 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDims: ");
    hit_tileDims(tile) == 3 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDimCard: ");
    for(int dim=0; dim<hit_tileDims(tile); dim++){
        if(hit_tileDimCard(tile, dim) != 10){
            ok = 0;
            break;
        }
    }
    ok ? printf("OK\n") : printf("ERROR\n");


    return 0;
}