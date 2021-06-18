/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout functions hit_laySelfRanks, hit_laySelfRanksDim
 * and hit_laySelfRank 
 * 
 * Layout-topology pairs checked:
 *      * 2D matrix with cyclic layout and 2D Array topology
 *          (hit_laySelfRanks)
 *      * 2D matrix with blocks layout and 2D Array topology
 *          (hit_laySelfRanksDim)
 *      * 2D matrix with blocks layout and Plain topology
 *          (hit_laySelfRank)
 * 
 * Cases checked (hit_laySelfRanks, hit_laySelfRanksDim):
 *      * The ranks must be asigned to each process incrementally from 0 to n
 *      * The ranks of a dimension non existent in the topology is 0
 * 
 * Cases checked (hit_laySelfRank):
 *      * If the process is active in the topology, the result of the function is
 *          equal to hit_Rank. Otherwise it is HIT_RANK_NULL
 *      
 * This test is made to work when executed with the number of processes above. It may
 * give incorrect results when working with other number of processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include <math.h>
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
    int ok = 1, two = 2, m, n, o, p;
    hit_tileFill(&okGather, &two);
    HitRanks myRanks;
   
    /// hit_laySelfRanks
    // 2D Topology
    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layCyclic, topo, sh,0);
    if(hit_Rank == 0){
        printf("Check hit_laySelfRanks: ");
    }

    myRanks = hit_laySelfRanks(lay);
    int lendim1 = hit_layDimNumActives(lay, 1);
    m = hit_Rank / lendim1;
    n = hit_Rank % lendim1;

    ok = (m == myRanks.rank[0] && n == myRanks.rank[1] && myRanks.rank[2] == -1 && myRanks.rank[3] == -1);
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    /// hit_laySelfRanksDim
    // 2D Topology
    ok = 1;
    if(hit_Rank == 0){
        printf("Check hit_laySelfRanksDim: ");
    }

    lendim1 = hit_layDimNumActives(lay, 1);
    m = hit_Rank / lendim1;
    n = hit_Rank % lendim1;
    o = -1;
    p = -1;

    ok = (m == hit_laySelfRanksDim(lay, 0) && n == hit_laySelfRanksDim(lay, 1) && 
            o == hit_laySelfRanksDim(lay, 2) && p == hit_laySelfRanksDim(lay, 3));

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);


    /// hit_laySelfRank
    // Plain Topology
    hit_layFree(lay);
    hit_topFree(topo);
    topo = hit_topology(plug_topPlain);
    lay = hit_layout(plug_layBlocks, topo, sh);
    if(hit_Rank == 0){
        printf("Check hit_laySelfRank: "); 
    }
    m = hit_laySelfRank(lay);

    if(hit_layImActive(lay)) ok = (m == hit_Rank);
    else ok = (m == HIT_RANK_NULL);

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);
    

    //if(hit_Rank == 0) printf("actives: (%d, %d, %d)\n", hit_layDimNumActives(lay, 0),hit_layDimNumActives(lay, 1),hit_layDimNumActives(lay, 2));
    //hit_comBarrier(lay);
    //printf("Proc[%d] local ranks (%d,%d,%d)\n", hit_Rank, myRanks.rank[0], myRanks.rank[1], myRanks.rank[2]);
    //printf("Proc[%d] rank (%d)\n", hit_Rank, rank);
    //printf("Proc[%d] neigh ranks (%d,%d,%d)\n", hit_Rank, neighRanks.rank[0], neighRanks.rank[1], neighRanks.rank[2]);
    //printf("Proc[%d] calculated ranks (%d,%d,%d)\n", hit_Rank, m, n, o);

    //printf("Proc[%d] OK=%d\n", hit_Rank, ok);

    
    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}