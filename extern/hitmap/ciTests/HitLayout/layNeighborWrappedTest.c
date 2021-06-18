/**
 * #nprocs: 1 4 5
 * 
 * Test for HitLayout functions hit_layDimNeighbor and hit_layNeighbor 
 * (wrapped with hit_layWrapNeighbors)
 * 
 * Layout-topology pairs checked:
 *      * 2D matrix with blocks layout and 2D topology
 * 
 * Cases checked (layNeighbor):
 *      * Valid dimension index
 *          Expected result: if the neighbor index is out of bounds, the result is -1
 *      * Dimension index smaller than 0
 *          Expected result: the value is -1
 *      * Dimension index bigger than the number of dimensions
 *          Expected result: the value is -1
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
    hit_layWrapNeighbors(&lay);
    HitRanks myRanks = hit_laySelfRanks(lay);
    
    if(hit_layImLeader(lay)){
        printf("Check hit_layDimNeighbor: ");
    }

    hit_comBarrier(lay);

    for(int i=0; i<2; i++){
        n = hit_layDimNeighbor(lay, i, 1);
        m = myRanks.rank[i]+1;
        if (m >= hit_layDimNumActives(lay, i)) m = 0;

        if (m != n) {ok = 0; break;}
    }

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    /////// 
    ok = 1;  
    if(hit_layImLeader(lay)){
        printf("Check hit_layNeighbor: ");
    }

#ifdef DEBUG

    if(hit_layImLeader(lay)){
        printf("\n\t\tSmaller index of dimension: ");
    }
    hit_comBarrier(lay);

#endif

    // Smaller index of dimension
    neighRanks = hit_layNeighbor(lay, -1, 1);
    ok = (neighRanks.rank[0] == -1 && neighRanks.rank[1] == -1);

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    if(hit_layImLeader(lay)){
        printf("\t\tBigger index of dimension: ");
    }

#endif

    // Bigger index of dimension
    neighRanks = hit_layNeighbor(lay, 3, 1);
    ok2 = (neighRanks.rank[0] == -1 && neighRanks.rank[1] == -1);
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    if(hit_layImLeader(lay)){
        printf("\t\tValid index of dimension: ");
    }

#endif

    hit_comBarrier(lay);
    neighRanks = hit_layNeighbor(lay, 0, 1);
    
    m = myRanks.rank[0]+1;
    n = myRanks.rank[1];
    if (m >= hit_layDimNumActives(lay, 0)) {
        m = 0;
    }
    
    ok2 = (m == neighRanks.rank[0] && neighRanks.rank[1] == n);
    ok = ok && ok2;

#ifdef DEBUG

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