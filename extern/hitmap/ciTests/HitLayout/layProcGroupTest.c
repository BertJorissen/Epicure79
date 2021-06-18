/**
 * #nprocs: 1 5 18 
 * 
 * Test for HitLayoutList function hit_lay_procGroup
 * 
 * Cases checked:
 *      * A processor rank bigger than the number of active processors in the layout
 *          Expected output: -1
 *      * The own rank
 *          Expected output: the same group number as returned from hit_layGroup
 * 
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
    int ok = 1, two = 2, ok2, g;
    hit_tileFill(&okGather, &two);
    double weights[10] = {100, 100, 100, 1, 2, 3, 4, 5, 6, 7};
    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layIndependentLB, topo, sh, weights);
    

    if(hit_Rank == 0){
        printf("Check hit_lay_procGroup: ");
    }

    // Non active rank
#ifdef DEBUG

    if(hit_Rank == 0) printf("\n\tNon active rank: ");

#endif

    g = hit_lay_procGroup(lay, hit_layNumActives(lay));
    ok2 = (g == -1);
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    printOKRoot(&okLocal, &okGather, lay);
    
#endif

    hit_comBarrier(lay);

    // Own rank
#ifdef DEBUG

    if(hit_Rank == 0) printf("\tOwn rank: ");

#endif

    g = hit_lay_procGroup(lay, hit_Rank);
    ok2 = (g == hit_layGroup(lay));
    ok = ok && ok2;

#ifdef DEBUG

    hit_tileFill(&okLocal, &ok2);
    
#else

    hit_tileFill(&okLocal, &ok);

#endif

    printOKRoot(&okLocal, &okGather, lay);


    // printf("Proc[%d] OK=%d\n", hit_Rank, g);

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}