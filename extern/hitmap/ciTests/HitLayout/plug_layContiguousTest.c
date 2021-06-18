/**
 * #nprocs: 4 
 * 
 * Test for HitLayout list constructor layContiguous
 * 
 * Cases checked:
 *      * hit_lgr_card, hit_lgr_cardPred, hit_lgr_cardSucc and hit_lgr_cardTotal
 *          Expected result: the total cardinality should be equal to the cardinality of the
 *              shape of the layout. The sum of the local, previous and succ cardinalities
 *              should also be equal to the total cardinality         
 *      * hit_layLoad
 *      * hit_layPredLoad
 *      * hit_laySuccLoad
 *      * hit_lgr_numGroups
 *          Expected result: the number of groups is equal to the number of processes
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

    HitSig sig0 = hit_sigStd(4);
    HitSig sig1 = hit_sig(-1, 3, 2);
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo;
    HitLayout lay;
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, two = 2, ok2;
    hit_tileFill(&okGather, &two);
    double weights[12] = {1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   
    topo = hit_topology(plug_topPlain);
    lay = hit_layout(plug_layContiguous, topo, sh, weights);
    
    if(hit_Rank == 0){
        printf("Check hit_layout_plug_layContiguous: ");
    }

    int card = hit_lgr_card(lay);
    int predcard = hit_lgr_cardPred(lay);
    int succcard = hit_lgr_cardSucc(lay);
    int totalcard = hit_lgr_cardTotal(lay);

    // Cardinality functions
#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tCardinality functions: ");

#endif

    ok2 = ( totalcard == hit_sigCard(sig0) * hit_sigCard(sig1) );
    ok2 = ok2 && (card + predcard + succcard == hit_sigCard(sig0) * hit_sigCard(sig1));
    ok = ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    
#endif



    hit_comBarrier(lay);
    // hit_layLoad
#ifdef DEBUG

    if(hit_Rank == 0) printf("\thit_layLoad: ");

#endif

    double load = 0;
    for(int i=predcard; i<predcard+card; i++) load += weights[i];
    ok2 = (load == hit_layLoad(lay));
    ok = ok2 && ok;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    
#endif

    hit_comBarrier(lay);
    // hit_layPredLoad
#ifdef DEBUG

    if(hit_Rank == 0) printf("\thit_layPredLoad: ");

#endif

    load = 0;
    for(int i=0; i<predcard; i++) load += weights[i];
    ok2 = (load == hit_layPredLoad(lay, 0));
    ok = ok2 && ok;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    
#endif

    hit_comBarrier(lay);

    // hit_laySuccLoad
#ifdef DEBUG

    if(hit_Rank == 0) printf("\thit_laySuccLoad: ");

#endif

    load = 0;
    for(int i=predcard+card; i<predcard+card+succcard; i++) load += weights[i];
    ok2 = (load == hit_laySuccLoad(lay, 0));
    ok = ok2 && ok;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);

#endif

    // hit_lgr_numGroups
#ifdef DEBUG

    if(hit_Rank == 0) printf("\thit_lgr_numGroups: ");

#endif

    ok2 = (hit_lgr_numGroups(lay) == hit_NProcs);
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