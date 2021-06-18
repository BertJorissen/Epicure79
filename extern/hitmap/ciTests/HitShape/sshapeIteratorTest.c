/**
 * Test for HitShape iterators hit_shapeIterator and hit_shapeIteratorTileCoord
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig = hit_sig(0, 19, 2);    // card = 10
    HitShape sh = hit_shape(1, sig);
    int i, j, ok;
    int tile[10], tileIt[10], array[10], arrayIt[10];

    printf("Check hit_shapeIteratorTileCoord: ");
    for(i=0; i<10; i++) tile[i] = i;

    j = 0;
    ok = 1;
    hit_shapeIteratorTileCoord(i, sh, 0){
        if(j>=10) {ok = 0; break;}
        tileIt[j] = i;
        j++;
    }
   
    if(ok){
        arrayCmp(tile, tileIt, 10, 10) ? printf("OK\n") : printf("ERROR\n");
    }
    else printf("ERROR\n");

    //
    printf("Check hit_shapeIterator: ");
    for(i=sig.begin, j=0; i<sig.end && j<10; i+=sig.stride, j++) array[j] = i;
    j = 0;
    ok = 1;
    hit_shapeIterator(i, sh, 0){
        if(j>=10) {ok = 0; break;}
        arrayIt[j] = i;
        j++;
    }

    if(ok){
        arrayCmp(array, arrayIt, 10, 10) ? printf("OK\n") : printf("ERROR\n");
    }
    else printf("ERROR\n");
    

    return 0;
}