/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout constructor hit_layout_plug_layMinBlocks
 * 
 * This test is made to work when executed with the number of processes above. It may
 * give incorrect results when working with other number of processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"

hit_tileNewType(int);

int main(int argc, char *argv[]) {
    hit_comInit( &argc, &argv );
    setbuf(stdout, NULL);

    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(-1, 20, 2);
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo;
    HitLayout lay;
    HitSig sigDim0, sigDim1;
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1;
    int minElements = 5;

   
    topo = hit_topology(plug_topArray2DComplete);
    lay = hit_layout(plug_layMinBlocks, topo, sh, minElements);
    
    if(hit_layImLeader(lay)){
        printf("Check hit_layout_plug_layMinBlocks: ");  
    }
    hit_comBarrier(lay);
    sigDim0 = hit_shapeSig(hit_layShape(lay), 0);
    sigDim1 = hit_shapeSig(hit_layShape(lay), 1);
    if (hit_layImActive(lay))
        ok = (hit_sigCard(sigDim0) >= minElements) && (hit_sigCard(sigDim1) >= minElements);
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}