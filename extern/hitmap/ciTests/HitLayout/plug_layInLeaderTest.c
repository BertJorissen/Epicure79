/** 
 * Test for HitLayout constructor hit_layout_plug_layInLeader
 * 
 * Cases checked:
 *      * hit_layShape()
 *          Expected result: only the leader processor receives the full shape
 *      * hit_layNumActives()
 *          Expected result: only one processor active
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main(int argc, char *argv[]) {

    hit_comInit( &argc, &argv );
    setbuf(stdout, NULL);
    hit_tileNewType(int);
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(-1, 20, 2);
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo = hit_topology(plug_topArray2D);
    HitLayout lay = hit_layout(plug_layInLeader, topo, sh);
    HitTile_int okGather, okLocal;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, a=-1;

    if(hit_Rank == 0) printf("Check hit_layout_plug_layInLeader: ");
    if(hit_layNumActives(lay) != 1) ok = 0;

    HitShape localSh = hit_layShape(lay);
    if(hit_layImLeader(lay)) ok = hit_shapeCmp(localSh, sh);
    else ok = (ok && hit_shapeCmp(localSh, HIT_SHAPE_NULL));

    hit_tileFill(&okLocal, &ok);
    hit_tileFill(&okGather, &a);
    printOKRoot(&okLocal, &okGather, lay);
    

    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}