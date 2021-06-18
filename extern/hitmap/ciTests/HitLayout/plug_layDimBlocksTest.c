/**
 * #nprocs: 1 4 5
 * 
 * Test for HitLayout constructor hit_layout_plug_dimBlocks
 * 
 * Cases checked:
 *      * An index of dimension smaller than 0
 *          Expected result: all processes receive the whole layout shape
 *      * An index of dimension bigger than the number of dimensions of the layout
 *          Expected result: all processes receive the whole layout shape
 *      * An index of dimension inside the bounds
 *          Expected result: in that dimension the shape is divided evenly across the processes,
 *              and in the rest of dimensions they receive the whole HitSig
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
    HitSig sig1 = hit_sig(-1, 20, 2);   // card = 11
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo;
    HitLayout lay;
    HitSig sigDim0, sigDim1;
    HitTile_int okLocal, okGather, localTile, neighTile;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, i, ok2 = 1, zero = 0;

    topo = hit_topology(plug_topArray2DComplete);
    lay = hit_layout(plug_layDimBlocks, topo, sh, 0);
    
    if(hit_layImLeader(lay)){
        printf("Check hit_layout_plug_layDimBlocks: ");
    }

    hit_comBarrier(lay);
    
    hit_tileDomainShapeAlloc(&localTile, HIT_INT, hit_layShape(lay));
    hit_tileDomainShapeAlloc(&neighTile, HIT_INT, hit_layShape(lay));
    hit_tileFill(&neighTile, &zero);
    sigDim0 = hit_shapeSig(hit_layShape(lay),0);
    sigDim1 = hit_shapeSig(hit_layShape(lay),1);
    hit_tileForDimDomain(localTile, 0, i) hit_tileElemAt(localTile, 2, i,0) = hit_sigTileToArray(sigDim0,i);
    
    // Dimension 1 must be equal to full layout dimension 1
    ok2 = hit_sigCmp(sigDim1, sig1);

    HitCom com;
    switch (hit_NProcs){
        case 4:
            if (hit_Rank % 2 == 0){
                com = hit_comSendRecv(lay, hit_layNeighbor(lay,1,1), &localTile, hit_layNeighbor(lay,1,1), &neighTile, HIT_INT);
            }
            else{
                com = hit_comSendRecv(lay, hit_layNeighbor(lay,1,-1), &localTile, hit_layNeighbor(lay,1,-1), &neighTile, HIT_INT);
            }
            break;
        
        default:
            hit_layWrapNeighbors(&lay);
            com = hit_comSendRecv(lay, hit_layNeighbor(lay,0,1), &localTile, hit_layNeighbor(lay,0,-1), &neighTile, HIT_INT);
            break;
    }
    
    hit_comDoSendRecv(com);
    hit_comBarrier(lay);
    
    switch (hit_NProcs){ 
        case 4: case 1:
            hit_tileForDimDomain(localTile, 0, i) {
                    if (hit_tileElemAt(localTile, 2, i, 0) != hit_tileElemAt(neighTile, 2, i, 0)) {
                        ok2 = 0;
                        break;
                    }
            }
            break;
        
        default:
            hit_tileForDimDomain(localTile, 0, i) {
                    if (hit_tileElemAt(localTile, 2, i, 0) == hit_tileElemAt(neighTile, 2, i, 0)) {
                        ok2 = 0;
                        break;
                    }
            }
            break;
    }
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    hit_comBarrier(lay);

    // Smaller index of dimension

#ifdef DEBUG

    if (hit_layImLeader(lay)){
        printf("\tSmaller dimension index: ");
    }

#endif

    hit_layFree(lay);
    lay = hit_layout(plug_layDimBlocks, topo, sh, -1);
    HitShape localSh = hit_layShape(lay);
    ok2 = hit_shapeCmp(localSh, sh);
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    hit_comBarrier(lay);

    // Bigger index of dimension

#ifdef DEBUG

    if (hit_layImLeader(lay)){
        printf("\tBigger dimension index: ");
    }

#endif

    hit_layFree(lay);
    lay = hit_layout(plug_layDimBlocks, topo, sh, 2);
    localSh = hit_layShape(lay);
    ok2 = hit_shapeCmp(localSh, sh);
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
