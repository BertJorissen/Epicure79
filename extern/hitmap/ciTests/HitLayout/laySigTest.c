/**
 * #nprocs: 1
 * 
 * Test for HitLayout Signature based functions:
 *  * hit_layCard
 *  * hit_layDimCard
 *  * hit_layDimBegin
 *  * hit_layDimEnd
 *  * hit_layDimStride
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main(int argc, char *argv[]) {

    hit_comInit( &argc, &argv );
    
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(-1, 20, 2);   // card = 11
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo = hit_topology(plug_topPlain);
    HitLayout lay = hit_layout(plug_layBlocks, topo, sh);

    printf("Check hit_layCard: ");
    hit_layCard(lay) == 10*11 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_layDimCard: ");
    hit_layDimCard(lay, 0) == 10 && hit_layDimCard(lay, 1) == 11 && hit_layDimCard(lay, 2) == 0 ?
        printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_layDimSig: ");
    hit_sigCmp(sig0, hit_layDimSig(lay, 0)) && hit_sigCmp(sig1, hit_layDimSig(lay, 1)) ?
        printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_layDimBegin: ");
    hit_layDimBegin(lay, 0) == sig0.begin && hit_layDimBegin(lay, 1) == sig1.begin ?
        printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_layDimEnd: ");
    hit_layDimEnd(lay, 0) == sig0.end && hit_layDimEnd(lay, 1) == sig1.end ?
        printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_layDimStride: ");
    hit_layDimStride(lay, 0) == sig0.stride && hit_layDimStride(lay, 1) == sig1.stride ?
        printf("OK\n") : printf("ERROR\n");


    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}