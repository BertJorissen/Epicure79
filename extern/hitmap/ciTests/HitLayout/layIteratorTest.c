/**
 * #nprocs: 1
 * 
 * Test for HitLayout iterators hit_layForDimDomain and hit_tileForDimDomainArray
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main(int argc, char *argv[]) {
    hit_comInit( &argc, &argv );
    setbuf(stdout, NULL);
    hit_tileNewType(int);
    HitTile_int t;
    HitSig sig = hit_sig(0, 19, 2);    // card = 10
    HitShape sh = hit_shape(1, sig);
    hit_tileDomainShape(&t, int, sh);
    int i, j, ok;
    int tile[10], tileIt[10];
    HitTopology topo = hit_topology(plug_topPlain);
    HitLayout lay = hit_layout(plug_layBlocks, topo, sh);

    printf("Check hit_layForDimDomain: ");
    for(i=sig.begin, j=0; i<sig.end && j<10; i+=sig.stride, j++) tile[j] = i;

    j = 0;
    ok = 1;
    hit_layForDimDomain(lay, 0, i){
        if(j>=10) {ok = 0; break;}
        tileIt[j] = i;
        j++;
    }
   
    if(ok){
        arrayCmp(tile, tileIt, 10, 10) ? printf("OK\n") : printf("ERROR\n");
    }
    else printf("ERROR\n");



    
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}