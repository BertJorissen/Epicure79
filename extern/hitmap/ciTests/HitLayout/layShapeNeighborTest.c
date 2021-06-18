/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout functions hit_layShapeNeighbor and hit_layShapeOther
 * (unwrapped)
 * 
 * Layout-topology pairs checked:
 *      * 2D matrix with blocks layout and Plain topology
 *      
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
    HitShape shNeigh;
    HitTopology topo;
    HitLayout lay;
    HitTile_int okLocal, okGather, myShape, neighShape;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    hit_tileDomainAlloc(&myShape, int, 1, 3);
    hit_tileDomainAlloc(&neighShape, int, 1, 3);
    int ok = 1, two = 2, zero = 0;
    hit_tileFill(&okGather, &two);
    hit_tileFill(&neighShape, &zero);
    
    HitRanks neighRanks;
   
    topo = hit_topology(plug_topPlain);
    lay = hit_layout(plug_layBlocks, topo, sh);
    hit_layUnwrapNeighbors(&lay);
    
    // hit_layShapeNeighbor
    if(hit_Rank == 0){
        printf("Check hit_layShapeNeighbor: ");
    }

    hit_tileElemAt(myShape, 1, 0) = hit_shapeSig(hit_layShape(lay), 0).begin;
    hit_tileElemAt(myShape, 1, 1) = hit_shapeSig(hit_layShape(lay), 0).end;
    hit_tileElemAt(myShape, 1, 2) = hit_shapeSig(hit_layShape(lay), 0).stride;

    // send my shape to next process, receive from previous
    HitCom com = hit_comSendRecv(lay, hit_layNeighbor(lay,0,+1), &myShape, hit_layNeighbor(lay,0,-1), &neighShape, HIT_INT);
    hit_comDoSendRecv(com);
    hit_comBarrier(lay);

    shNeigh = hit_layShapeNeighbor(lay, 0, -1);
    HitSig sigg = hit_shapeSig(shNeigh,0);
    if(hit_shapeDims(shNeigh) != -1)     // If the process has no neighbor the result is a null HitShape
        ok = ( sigg.begin == hit_tileElemAt(neighShape, 1, 0) && sigg.end == hit_tileElemAt(neighShape, 1, 1) &&
            sigg.stride == hit_tileElemAt(neighShape, 1, 2) );


    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    // hit_layShapeOther
    if(hit_Rank == 0){
        printf("Check hit_layShapeOther: ");
    }
    hit_tileFill(&okGather, &two);

    // no sendrecv because it's the same as before
    
    neighRanks = hit_ranks(1, -1);
    shNeigh = hit_layShapeOther(lay, neighRanks);
    sigg = hit_shapeSig(shNeigh,0);
    if(hit_shapeDims(shNeigh) != -1)     // If the process has no neighbor the result is a null HitShape
        ok = ( sigg.begin == hit_tileElemAt(neighShape, 1, 0) && sigg.end == hit_tileElemAt(neighShape, 1, 1) &&
            sigg.stride == hit_tileElemAt(neighShape, 1, 2) );


    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_tileFree(neighShape);
    hit_tileFree(myShape);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}