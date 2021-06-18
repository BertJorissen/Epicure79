/**
 * Test for HitTile iterators hit_tileForDimDomain and hit_tileForDimDomainArray
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    hit_tileNewType(int);
    HitTile_int t;
    HitSig sig = hit_sig(0, 19, 2);    // card = 10
    HitShape sh = hit_shape(1, sig);
    hit_tileDomainShape(&t, int, sh);
    int i, j, ok;
    int tile[10], tileIt[10], array[10], arrayIt[10];

    printf("Check hit_tileForDimDomain: ");
    for(i=0; i<10; i++) tile[i] = i;

    j = 0;
    ok = 1;
    hit_tileForDimDomain(t, 0, i){
        if(j>=10) {ok = 0; break;}
        tileIt[j] = i;
        j++;
    }
   
    if(ok){
        arrayCmp(tile, tileIt, 10, 10) ? printf("OK\n") : printf("ERROR\n");
    }
    else printf("ERROR\n");

    //
    printf("Check hit_tileForDimDomainArray: ");
    for(i=sig.begin, j=0; i<sig.end && j<10; i+=sig.stride, j++) array[j] = i;
    j = 0;
    ok = 1;
    hit_tileForDimDomainArray(t, 0, i){
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