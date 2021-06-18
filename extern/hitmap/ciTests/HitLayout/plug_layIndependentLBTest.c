/**
 * #nprocs: 4 
 * 
 * Test for HitLayoutList constructor layIndependentLB
 * 
 * Cases checked:
 *      * hit_lgr_cardTotal
 *          Expected result: the total cardinality should be equal to the cardinality of the
 *              dimension 0 of the layout
 *      * hit_layHasElem and hit_lgr_elementGroup
 *          Expected result: each group has at least one element. In the cases where layHasElem
 *              returns true, the result of hit_lgr_elementGroup should be the group of the processor
 *      * hit_lgr_elementGroup, hit_lgr_group, hit_lgr_leader and hit_lgr_nProcs
 *          Expected result: for each element, the result of lgr_leader and lgr_nProcs must be
 *              the leader index and number of procs of the group that has the element
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
    HitSig sig1 = hit_sig(-1, 3, 2);
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo;
    HitLayout lay;
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, two = 2, ok2;
    hit_tileFill(&okGather, &two);
    double weights[10] = {100, 100, 100, 1, 2, 3, 4, 5, 6, 7};
    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layIndependentLB, topo, sh, weights);
    
    if(hit_Rank == 0){
        printf("Check hit_layout_plug_layIndependentLB: ");
    }

    int totalcard = hit_lgr_cardTotal(lay);

    // hit_lgr_cardTotal 
#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\thit_lgr_cardTotal: ");

#endif

    ok2 = ( totalcard == hit_sigCard(sig0));
    ok = ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    
#endif

    hit_comBarrier(lay);

    // hit_layHasElem
#ifdef DEBUG

    if(hit_Rank == 0) printf("\thit_layHasElem: ");

#endif

    ok2 = 0;
    for(int i=0; i<hit_sigCard(sig0); i++){
        if (hit_layHasElem(lay, i)){
            ok2 = 1;
            if(hit_lgr_elementGroup(lay, i) != hit_layGroup(lay)){
                ok2 = 0; break;
            }
        }
    }
    ok = ok2 && ok;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // hit_lgr_elementGroup, hit_lgr_group, hit_lgr_leader and hit_lgr_nProcs, 
#ifdef DEBUG

    if(hit_Rank == 0) printf("\thit_lgr_elementGroup, leader and nProcs: ");

#endif

    ok2 = 1;
    HitGroup group;
    for(int i=0; i<hit_sigCard(sig0); i++){
        group = hit_lgr_group(lay,hit_lgr_elementGroup(lay, i));
        ok2 = (hit_lgr_leader(lay, i) == group.leader && hit_lgr_nProcs(lay, i) == group.numProcs);
        if(! ok2) break;
    }


    ok = ok2 && ok;
    // printf("Proc[%d] OK=%d\n", hit_Rank, ok2);

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);

#else

    hit_tileFill(&okLocal, &ok);

#endif

    printOKRoot(&okLocal, &okGather, lay);

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}