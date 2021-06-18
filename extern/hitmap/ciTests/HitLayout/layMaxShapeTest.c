/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout functions hit_layDimMaxSize hit_layMaxShape and hit_layDimMaxShape
 * 
 * Layout-topology pairs checked:
 *      * 2D matrix with blocks layout and 2D topology
 *      * 2D matrix with blocks layout and plain topology
 * 
 * Cases checked (hit_layMaxSize):
 *      * The maxSize is defined by the layout type and the topology. Normally it is calculated
 *          for each dimension as ceil( card(dimension)/activeProcs in dimension ).
 *          When the num of dimensions of the topology is smaller than the shape's, the remaining
 *          shape dimensions' max size is equal to its cardinality
 * 
 * Cases checked (maxShape):
 *      * In all layouts, the max shape has the local shape's begin and stride and the
 *             end adjusted to have the cardinality equal to the maxElems
 * 
 * Cases checked (dimMaxShape):
 *      * In all layouts, the modification above is applyied only in the dimension specified,
 *          the other dimensions remain the same 
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
    HitShape localSh, maxSh0, maxSh1;
    HitTopology topo;
    HitLayout lay;
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, two = 2, ok2 = 1;
    int maxsize0, maxsize1;
    int * max = lay.maxSize;
    topo = hit_topology(plug_topArray2D);
    hit_tileFill(&okGather, &two);

    ///// hit_layMaxShape
    // Blocks Layout with 2D Topology
    lay = hit_layout(plug_layBlocks, topo, sh);
    
    if(hit_Rank == 0){
        printf("Check hit_layMaxShape: ");
    }

#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tBlocks Layout with 2D Topology: ");  

#endif

    hit_comBarrier(lay);
    localSh = hit_layShape(lay);
    maxSh0 = hit_layMaxShape(lay);

    if (hit_layImActive(lay)){
        for (int i=0; i<hit_shapeDims(localSh); i++){
           hit_shapeSig(localSh, i).end = hit_shapeSig(localSh, i).begin + (max[i]-1)*hit_shapeSig(localSh, i).stride;
        }
    
    
        ok2 = hit_shapeCmp(localSh, maxSh0);
    }
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // Blocks Layout with Plain Topology
    ok2 = 1;
    hit_layFree(lay);
    hit_topFree(topo);
    topo = hit_topology(plug_topPlain);
    lay = hit_layout(plug_layBlocks, topo, sh);
    
#ifdef DEBUG

    if(hit_Rank == 0) printf("\tBlocks Layout with Plain Topology: ");  

#endif

    hit_comBarrier(lay);
    localSh = hit_layShape(lay);
    maxSh0 = hit_layMaxShape(lay);

    if (hit_layImActive(lay)){
        for (int i=0; i<hit_shapeDims(localSh); i++){
           hit_shapeSig(localSh, i).end = hit_shapeSig(localSh, i).begin + (max[i]-1)*hit_shapeSig(localSh, i).stride;
        }
    
    
        ok2 = hit_shapeCmp(localSh, maxSh0);
    }
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#else

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);

#endif
    
    ///// hit_layDimMaxShape
    // Blocks Layout with 2D Topology
    ok = 1;
    ok2 = 1;
    hit_layFree(lay);
    hit_topFree(topo);
    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layBlocks, topo, sh);
    
    if(hit_Rank == 0){
        printf("Check hit_layDimMaxShape: ");
    }

#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tBlocks Layout with 2D Topology: ");  

#endif

    hit_comBarrier(lay);
    localSh = hit_layShape(lay);
    maxSh0 = hit_layDimMaxShape(lay, 0);
    maxSh1 = hit_layDimMaxShape(lay, 1);

    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 0).end = hit_shapeSig(localSh, 0).begin + (max[0]-1)*hit_shapeSig(localSh, 0).stride;
    
        ok2 = hit_shapeCmp(localSh, maxSh0);
    }

    localSh = hit_layShape(lay);
    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 1).end = hit_shapeSig(localSh, 1).begin + (max[1]-1)*hit_shapeSig(localSh, 1).stride;
    
        ok2 = ok2 && hit_shapeCmp(localSh, maxSh1);
    }
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // Blocks Layout with Plain Topology
    ok2 = 1;
    hit_layFree(lay);
    hit_topFree(topo);
    topo = hit_topology(plug_topPlain);
    lay = hit_layout(plug_layBlocks, topo, sh);
    
#ifdef DEBUG

    if(hit_Rank == 0) printf("\tBlocks Layout with Plain Topology: ");  

#endif

    hit_comBarrier(lay);
    localSh = hit_layShape(lay);
    maxSh0 = hit_layDimMaxShape(lay,0);
    maxSh1 = hit_layDimMaxShape(lay,1);

    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 0).end = hit_shapeSig(localSh, 0).begin + (max[0]-1)*hit_shapeSig(localSh, 0).stride;    
    
        ok2 = hit_shapeCmp(localSh, maxSh0);
    }

    localSh = hit_layShape(lay);
    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 1).end = hit_shapeSig(localSh, 1).begin + (max[1]-1)*hit_shapeSig(localSh, 1).stride;
    
        ok2 = ok2 && hit_shapeCmp(localSh, maxSh1);
    }

    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#else

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    ///// hit_layDimMaxSize
    // Blocks Layout with 2D Topology
    ok = 1;
    ok2 = 1;
    hit_layFree(lay);
    hit_topFree(topo);
    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layBlocks, topo, sh);
    
    maxsize0 = (int) ceil(hit_sigCard(sig0)/(double)hit_layDimNumActives(lay, 0));
    maxsize1 = (int) ceil(hit_sigCard(sig1)/(double)hit_layDimNumActives(lay, 1));

    if(hit_Rank == 0){
        printf("Check hit_layDimMaxSize: ");
    }

#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tBlocks Layout with 2D Topology: ");  

#endif

    hit_comBarrier(lay);

    if (hit_layImActive(lay)) ok2 = (hit_layDimMaxSize(lay, 0) == maxsize0 && hit_layDimMaxSize(lay, 1) == maxsize1);
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // Blocks Layout with Plain Topology
    hit_layFree(lay);
    hit_topFree(topo);
    hit_tileFill(&okGather, &two);
    topo = hit_topology(plug_topPlain);
    lay = hit_layout(plug_layBlocks, topo, sh);

    maxsize0 = (int) ceil(hit_sigCard(sig0)/(double)hit_layDimNumActives(lay, 0));
    maxsize1 = (int) ceil(hit_sigCard(sig1)/(double)hit_layDimNumActives(lay, 1));

#ifdef DEBUG

    if(hit_Rank == 0) printf("\tBlocks Layout with Plain Topology: ");

#endif

    hit_comBarrier(lay);

    ok2 = (hit_layDimMaxSize(lay, 0) == maxsize0 && hit_layDimMaxSize(lay, 1) == hit_sigCard(sig1));
    ok = ok && ok2;


#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#else

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);

#endif


    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}