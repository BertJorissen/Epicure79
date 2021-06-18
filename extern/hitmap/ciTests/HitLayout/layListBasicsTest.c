/**
 * #nprocs: 4 
 * 
 * Test for HitLayoutList functions hit_lay_elements, hit_layGroupTopo, hit_lgr_group,
 * hit_lgr_groupLeader and hit_lgr_groupNProcs
 * 
 * Layout-topology pairs checked:
 *      * 1D array with independentLB layout and 2D topology
 * 
 * Cases checked:
 *      * hit_lgr_group
 *          The cases of specifying a bigger or smaller group index are not contemplated
 *          because the result would be random since it is accesing an out of bounds position
 *          in an array
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
    HitShape sh = hit_shape(1, sig0);
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, two = 2;
    hit_tileFill(&okGather, &two);
    double weights[10] = {100, 100, 100, 1, 2, 3, 4, 5, 6, 7};
    int nElements;
    int * elements;
    HitTopology topo = hit_topology(plug_topArray2D);
    HitLayout lay = hit_layout(plug_layIndependentLB, topo, sh, weights);
    
    // hit_lay_elements
    if(hit_Rank == 0){
        printf("Check hit_lay_elements: ");
    }

    hit_lay_elements(lay, hit_layGroup(lay), &elements, &nElements);
    int cont = 0;
    for(int i=0; i<hit_sigCard(sig0); i++){
        if (hit_layHasElem(lay, i)){
            if (elements[cont] != i){ ok = 0; break;}
            cont++;
        }
    }

    if (cont != nElements) ok = 0;
    // printf("Proc[%d] OK=%d\n", hit_Rank, ok);
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    
    // hit_lgr_group, hit_lgr_groupLeader and hit_lgr_groupNProcs
    ok = 1;
    if(hit_Rank == 0){
        printf("Check group functions: ");
    }
    hit_comBarrier(lay);

    HitGroup group = hit_lgr_group(lay, 2);
    ok = ( group.leader == hit_lgr_groupLeader(lay, 2) && group.numProcs == hit_lgr_groupNProcs(lay, 2) );
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);


    // hit_layGroupTopo
    HitTopology t = hit_layGroupTopo(lay, 2);
    if(hit_Rank == 0) printf("Check hit_layGroupTopo: ");

    ok = (t.numDims == 1 && t.card[0] == hit_lgr_groupNProcs(lay, 2));

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);


    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}