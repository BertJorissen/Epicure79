/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout functions hit_layMinShape and hit_layDimMinShape
 * 
 * Layout-topology pairs checked:
 *      * A layout type with no minimum elements and 2D topology
 *          Expected result: the minElements is defined by the layout type and the topology. Normally
 *              it is calculated for each dimension as floor( card(dimension)/activeProcs in dimension )
 *      * MinBlocks layout and 2D topology
 *          Expected result (minShape): the min shape has the local shape's begin and stride and the
 *              end adjusted to have the cardinality equal to the minElems set in the constructor
 *          Expected result (dimMinShape): the modification above is applyied only in the dimension
 *              specified, the other dimensions remain the same 
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
    HitShape localSh, minSh0, minSh1;
    HitTopology topo;
    HitLayout lay;
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, two = 2, ok2 = 1;
    int minElements = 5, minsize0, minsize1;
    topo = hit_topology(plug_topArray2D);
    hit_tileFill(&okGather, &two);
    

    ///// hit_layMinShape
    // Blocks Layout
    lay = hit_layout(plug_layBlocks, topo, sh);
    int * min = lay.minSize;

    if(hit_Rank == 0){
        printf("Check hit_layMinShape: ");
    }

#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tBlocks Layout with 2D Topology: ");  

#endif

    hit_comBarrier(lay);

    // Checking minsizes first
    minsize0 = hit_sigCard(sig0)/hit_layDimNumActives(lay, 0);
    minsize1 = hit_sigCard(sig1)/hit_layDimNumActives(lay, 1);

    if(hit_Rank == 0){
        if(minsize0 != min[0] || minsize1 != min[1]){
            printf("\nError in lay.minsize\n");
            ok2 = 0;
        }
    }

    localSh = hit_layShape(lay);
    minSh0 = hit_layMinShape(lay);

    if (hit_layImActive(lay)){
        for (int i=0; i<hit_shapeDims(localSh); i++){
           hit_shapeSig(localSh, i).end = hit_shapeSig(localSh, i).begin + (min[i]-1)*hit_shapeSig(localSh, i).stride;
        }
    
    
        ok2 = ok2 && hit_shapeCmp(localSh, minSh0);
    }

    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // MinBlocks Layout
    ok2 = 1;
    hit_layFree(lay);
    lay = hit_layout(plug_layMinBlocks, topo, sh, minElements);
    min = lay.minSize;

#ifdef DEBUG

    if(hit_Rank == 0) printf("\tMinBlocks Layout with 2D Topology: ");  
    
#endif

    hit_comBarrier(lay);

    // Checking minsizes first
    // why minsize != 5 (restricted minimum size) ?
    minsize0 = hit_sigCard(sig0)/hit_layDimNumActives(lay, 0);
    minsize1 = hit_sigCard(sig1)/hit_layDimNumActives(lay, 1);
    if(hit_Rank == 0){
        if(minsize0 != min[0] || minsize1 != min[1]){
            printf("\nError in lay.minsize %d %d\n", minsize1, min[1]);
            ok2 = 0;
        }
    }

    hit_comBarrier(lay);

    localSh = hit_layShape(lay);
    minSh0 = hit_layMinShape(lay);

    if (hit_layImActive(lay)){
        for (int i=0; i<hit_shapeDims(localSh); i++){
           hit_shapeSig(localSh, i).end = hit_shapeSig(localSh, i).begin + (min[i]-1)*hit_shapeSig(localSh, i).stride;
        }
    
    
        ok2 = ok2 && hit_shapeCmp(localSh, minSh0);
    }

    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#else

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    ///// hit_layDimMinShape
    // Blocks Layout
    ok = 1;
    ok2 = 1;
    hit_layFree(lay);
    lay = hit_layout(plug_layBlocks, topo, sh);
    min = lay.minSize;
    
    if(hit_Rank == 0){
        printf("Check hit_layDimMinShape: ");
    }

#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tBlocks Layout with 2D Topology: ");  

#endif

    hit_comBarrier(lay);
    localSh = hit_layShape(lay);
    minSh0 = hit_layDimMinShape(lay, 0);
    minSh1 = hit_layDimMinShape(lay, 1);

    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 0).end = hit_shapeSig(localSh, 0).begin + (min[0]-1)*hit_shapeSig(localSh, 0).stride;
    
        ok2 = hit_shapeCmp(localSh, minSh0);
    }

    localSh = hit_layShape(lay);
    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 1).end = hit_shapeSig(localSh, 1).begin + (min[1]-1)*hit_shapeSig(localSh, 1).stride;
    
        ok2 = ok2 && hit_shapeCmp(localSh, minSh1);
    }

    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // MinBlocks Layout
    ok2 = 1;
    hit_layFree(lay);
    lay = hit_layout(plug_layMinBlocks, topo, sh, minElements);
    min = lay.minSize;
    
#ifdef DEBUG

    if(hit_Rank == 0) printf("\tMinBlocks Layout with 2D Topology: ");  
    
#endif

    hit_comBarrier(lay);
    localSh = hit_layShape(lay);
    minSh0 = hit_layDimMinShape(lay,0);
    minSh1 = hit_layDimMinShape(lay, 1);

    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 0).end = hit_shapeSig(localSh, 0).begin + (min[0]-1)*hit_shapeSig(localSh, 0).stride;    
    
        ok2 = hit_shapeCmp(localSh, minSh0);
    }

    localSh = hit_layShape(lay);
    if (hit_layImActive(lay)){
        hit_shapeSig(localSh, 1).end = hit_shapeSig(localSh, 1).begin + (min[1]-1)*hit_shapeSig(localSh, 1).stride;
    
        ok2 = ok2 && hit_shapeCmp(localSh, minSh1);
    }

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