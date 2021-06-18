/**
 * #nprocs: 1 4 18
 * 
 * Test for HitLayout functions hit_layout_class, hit_layTopology, hit_layNumDims,
 * hit_layOnDim, hit_layImActive, hit_layLeader and hit_layImLeader
 * 
 * Layout-topology pairs checked:
 *      * 2D array with independentLB layout and 2D topology
 *      * 2D array with copy layout and 2D topology
 *      * 2D array with dimblocks layout and plain topology
 *      * 1D array with contiguous layout and plain topology
 * 
 * Cases checked:
 *      * hit_layout_class
 *          If the layout is of type List the result is HIT_LAYOUT_LIST_CLASS, otherwise it is 
 *              HIT_LAYOUT_SIG_CLASS
 *      * hit_layOnDim (Signature Layouts)
 *          The not restricted layout's result is -1 and the restricted layout's is
 *              the dimension set in the constructor
 *      * hit_layLeader (Signature Layouts)
 *          If the process is active the result is 0, otherwise it's -1
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
    HitShape sh1D = hit_shape(1, sig0);
    HitShape sh2D = hit_shape(2, sig0, sig0);
    HitTile_int okLocal, okGather;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, two = 2, ok2, l;
    hit_tileFill(&okGather, &two);
    double weights[10] = {100, 100, 100, 1, 2, 3, 4, 5, 6, 7};

    HitTopology topoPlain = hit_topology(plug_topPlain);
    HitTopology topo2D = hit_topology(plug_topArray2D);
    HitLayout layLB = hit_layout(plug_layIndependentLB, topo2D, sh2D, weights);
    HitLayout layCont = hit_layout(plug_layContiguous, topoPlain, sh1D, weights);
    HitLayout layCopy = hit_layout(plug_layCopy, topoPlain, sh1D);
    HitLayout layBlocks = hit_layout(plug_layDimBlocks, topo2D, sh2D, 0);
    
    
    // hit_layout_class
    if(hit_Rank == 0) printf("Check hit_layout_class: ");
    ok = ( hit_layout_class(layLB) == HIT_LAYOUT_LIST_CLASS && hit_layout_class(layCont) == HIT_LAYOUT_LIST_CLASS  &&
            hit_layout_class(layCopy) == HIT_LAYOUT_SIG_CLASS && hit_layout_class(layBlocks) == HIT_LAYOUT_SIG_CLASS );


    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layBlocks);


    // hit_layTopology
    if(hit_Rank == 0) printf("Check hit_layTopology: ");
    hit_comBarrier(layBlocks);
    //// 2D topo
    HitTopology t = hit_layTopology(layBlocks);
    ok = ( t.type == topo2D.type && t.numDims == topo2D.numDims && t.card[0] == topo2D.card[0] && t.card[1] == topo2D.card[1] );

    t = hit_layTopology(layLB);
    ok2 = ( t.type == topo2D.type && t.numDims == topo2D.numDims && t.card[0] == topo2D.card[0] && t.card[1] == topo2D.card[1] );
    ok = ok && ok2;

    //// Plain topo
    t = hit_layTopology(layCont);
    ok2 = ( t.type == topoPlain.type && t.numDims == topoPlain.numDims && t.card[0] == topoPlain.card[0]);
    ok = ok && ok2;

    t = hit_layTopology(layCopy);
    ok2 = ( t.type == topoPlain.type && t.numDims == topoPlain.numDims && t.card[0] == topoPlain.card[0]);
    ok = ok && ok2;

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layCopy);


    // hit_layNumDims (Signature layouts)
    if(hit_Rank == 0) printf("Check hit_layNumDims: ");

    ok = (hit_layNumDims(layCopy) == 1) && (hit_layNumDims(layBlocks) == 2);

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layBlocks);


    // hit_layOnDim (Signature layouts)
    if(hit_Rank == 0) printf("Check hit_layOnDim: ");

    ok = (hit_layOnDim(layCopy) == -1) && (hit_layOnDim(layBlocks) == 0);

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layBlocks);


    // hit_layImActive (Signature layouts)
    if(hit_Rank == 0) printf("Check hit_layImActive: ");

    //// Blocks lay
    if (hit_Rank < hit_layNumActives(layBlocks)) l = 1;
    else l = 0;
    ok = (l == hit_layImActive(layBlocks));
    
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layBlocks);


    // hit_layImLeader (Signature layouts)
    if(hit_Rank == 0) printf("Check hit_layImLeader: ");

    if (hit_layImActive(layBlocks) && hit_Rank == 0) l = 1;
    else if (! hit_layImActive(layBlocks) && hit_Rank == hit_layNumActives(layBlocks)) l = 1;
    else l = 0;
    ok = (l == hit_layImLeader(layBlocks));
    
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layBlocks);
   

    // hit_layLeader
    if(hit_Rank == 0) printf("Check hit_layLeader: ");

    if (hit_layImActive(layBlocks)) l = 0;
    else l = -1;
    ok = (l == hit_topRankInternal(topo2D, hit_layLeader(layBlocks)));

    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, layBlocks);


    

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(layLB);
    hit_layFree(layCont);
    hit_layFree(layCopy);
    hit_layFree(layBlocks);
    hit_topFree(topoPlain);
    hit_topFree(topo2D);
    hit_comFinalize();

    return 0;
}