/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout functions hit_layWrapNeighborsDim and hit_layUnwrapNeighborsDim
 * 
 * Layout-topology pairs checked:
 *      * 2D matrix with blocks layout and 2D topology
 * 
 * Cases checked:
 *      * hit_layDimNeighbor
 *          Expected result: each dimension acts individually as wrapped-unwrapped
 *      * hit_layNeighbor2
 *          Expected result: if only the wrapped dimension is out of bounds it starts again
 *              from 0, but if the unwrapped dimension is out of bounds the global result is
 *              HIT_RANKS_NULL
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
    int ok = 1, ok2 = 1, n, m;
    HitRanks neighRanks;
   
    topo = hit_topology(plug_topArray2DComplete);
    lay = hit_layout(plug_layBlocks, topo, sh);
    hit_layWrapNeighborsDim(&lay, 0);
    HitRanks myRanks = hit_laySelfRanks(lay);
    

    /// hit_layWrapNeighborsDim
    if(hit_layImLeader(lay)){
        printf("Check hit_layWrapNeighborsDim: ");
    }

    hit_comBarrier(lay);

    // hit_layDimNeighbor
    for(int i=0; i<2; i++){
        n = hit_layDimNeighbor(lay, i, 1);
        m = myRanks.rank[i]+1;
        if (m >= hit_layDimNumActives(lay, i)){
            if (i != 0) m = -1;
            else m = 0;
        }

        if (m != n) {ok2 = 0; break;}
    }
    ok = ok && ok2;

#ifdef DEBUG

    if(hit_Rank == 0){
        printf("\n\thit_layDimNeighbor: ");
    }
    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

#endif

    // hit_layNeighbor2
    neighRanks = hit_layNeighbor2(lay, 1, 1);

    m = myRanks.rank[0]+1;
    n = myRanks.rank[1]+1;
    if (m >= hit_layDimNumActives(lay, 0)) m = 0;
    if (n >= hit_layDimNumActives(lay, 1)) {
        m = -1;
        n = -1;
    }

    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n);
    ok = ok && ok2;

#ifdef DEBUG

    if(hit_Rank == 0){
        printf("\thit_layNeighbor2: ");
    }
    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

#else

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

#endif

    /// hit_layUnwrapNeighborsDim
    hit_layUnwrapNeighborsDim(&lay, 0);
    if(hit_layImLeader(lay)){
        printf("Check hit_layUnwrapNeighborsDim: ");
    }

    ok = 1; ok2 = 1;

    hit_comBarrier(lay);

    // hit_layDimNeighbor
    for(int i=0; i<2; i++){
        n = hit_layDimNeighbor(lay, i, 1);
        m = myRanks.rank[i]+1;
        if (m >= hit_layDimNumActives(lay, i)) m = -1;
        if (m != n) {ok2 = 0; break;}
    }
    ok = ok && ok2;

#ifdef DEBUG

    if(hit_Rank == 0){
        printf("\n\thit_layDimNeighbor: ");
    }
    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

#endif

    // hit_layNeighbor2
    neighRanks = hit_layNeighbor2(lay, 1, 1);

    m = myRanks.rank[0]+1;
    n = myRanks.rank[1]+1;
    if (m >= hit_layDimNumActives(lay, 0) || n >= hit_layDimNumActives(lay, 1)) {
        m = -1;
        n = -1;
    }

    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n);
    ok = ok && ok2;

#ifdef DEBUG

    if(hit_Rank == 0){
        printf("\thit_layNeighbor2: ");
    }
    hit_tileFill(&okLocal, &ok2);

#else

    hit_tileFill(&okLocal, &ok);
    
#endif

    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}