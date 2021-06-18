/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout functions hit_layNeighbor1, hit_layNeighbor2,
 * hit_layNeighbor3 and hit_layNeighborN (unwrapped)
 * 
 * Layout-topology pairs checked:
 *      * 2D matrix with blocks layout and 2D topology
 * 
 * Cases checked (layNeighbor):
 *      * Valid dimension index
 *          Expected result: if the neighbor index is out of bounds, the result is -1
 *      * Invalid indeces are checked in file layNeighborWrappedTest.c
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
    HitTopology topo;
    HitLayout lay;
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, ok2 = 1, n, m, o, two = 2;
    hit_tileFill(&okGather, &two);
    
    HitRanks neighRanks;
   
    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layBlocks, topo, sh);
    hit_layUnwrapNeighbors(&lay);
    HitRanks myRanks = hit_laySelfRanks(lay);
    
    // hit_layNeighbor1
    if(hit_Rank == 0){
        printf("Check hit_layNeighbor1: ");
    }

    hit_comBarrier(lay);
    neighRanks = hit_layNeighbor1(lay, 1);
    if (hit_layImActive(lay)){
        m = myRanks.rank[0]+1;
        n = myRanks.rank[1];
        if (m >= hit_layDimNumActives(lay, 0)) {
            m = -1;
            n = -1;
        }
    }
    else{
        m = myRanks.rank[0];
        n = myRanks.rank[1];
    }
    
    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n);
    ok = ok && ok2;
  
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);


    // hit_layNeighbor2
    hit_tileFill(&okGather, &two);
    if(hit_Rank == 0){
        printf("Check hit_layNeighbor2: ");
    }

    hit_comBarrier(lay);
    neighRanks = hit_layNeighbor2(lay, 1, 1);
    if (hit_layImActive(lay)){
        m = myRanks.rank[0]+1;
        n = myRanks.rank[1]+1;
        if (m >= hit_layDimNumActives(lay, 0) || n >= hit_layDimNumActives(lay, 1)) {
            m = -1;
            n = -1;
        }
    }
    else{
        m = myRanks.rank[0];
        n = myRanks.rank[1];
    }
    
    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n);
    ok = ok && ok2;
  
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);


    // hit_layNeighbor3
    hit_layFree(lay);
    hit_topFree(topo);
    sh = hit_shape(3, sig0, sig1, sig1);
    topo = hit_topology(plug_topArray3D);
    lay = hit_layout(plug_layBlocks, topo, sh);
    hit_layUnwrapNeighbors(&lay);
    hit_tileFill(&okGather, &two);
    myRanks = hit_laySelfRanks(lay);

    if(hit_Rank == 0){
        printf("Check hit_layNeighbor3: ");
    }

    hit_comBarrier(lay);
    neighRanks = hit_layNeighbor3(lay, 1, 1, 1);
    if (hit_layImActive(lay)){
        m = myRanks.rank[0]+1;
        n = myRanks.rank[1]+1;
        o = myRanks.rank[2]+1;
        if (m >= hit_layDimNumActives(lay, 0) || n >= hit_layDimNumActives(lay, 1) || o >= hit_layDimNumActives(lay, 2)){
            m = -1;
            n = -1;
            o = -1;
        } 
    }
    else{
        m = myRanks.rank[0];
        n = myRanks.rank[1];
        o = myRanks.rank[2];
    }
    
    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n && neighRanks.rank[2] == o);
    ok = ok && ok2;

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);


    // hit_layNeighborN
    hit_tileFill(&okGather, &two);
    HitRanks shifts = hit_ranks(3, 1, 1, 1);

    if(hit_Rank == 0){
        printf("Check hit_layNeighborN: ");
    }

    hit_comBarrier(lay);
    neighRanks = hit_layNeighborN(lay, shifts);
    if (hit_layImActive(lay)){
        m = myRanks.rank[0]+1;
        n = myRanks.rank[1]+1;
        o = myRanks.rank[2]+1;
        if (m >= hit_layDimNumActives(lay, 0) || n >= hit_layDimNumActives(lay, 1) || o >= hit_layDimNumActives(lay, 2)){
            m = -1;
            n = -1;
            o = -1;
        } 
    }
    else{
        m = myRanks.rank[0];
        n = myRanks.rank[1];
        o = myRanks.rank[2];
    }
    
    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n && neighRanks.rank[2] == o);
    ok = ok && ok2;

    //if(hit_Rank == 0) printf("actives: (%d, %d, %d)\n", hit_layDimNumActives(lay, 0),hit_layDimNumActives(lay, 1),hit_layDimNumActives(lay, 2));
    //hit_comBarrier(lay);
    //printf("Proc[%d] local ranks (%d,%d,%d)\n", hit_Rank, myRanks.rank[0], myRanks.rank[1], myRanks.rank[2]);
    //printf("Proc[%d] neigh ranks (%d,%d,%d)\n", hit_Rank, neighRanks.rank[0], neighRanks.rank[1], neighRanks.rank[2]);
    //printf("Proc[%d] calculated ranks (%d,%d,%d)\n", hit_Rank, m, n, o);

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);



    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}