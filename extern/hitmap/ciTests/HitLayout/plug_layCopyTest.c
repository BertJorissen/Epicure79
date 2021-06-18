/** 
 * Test for HitLayout constructor hit_layout_plug_layCopy
 * 
 * Cases checked:
 *      * hit_layShape()
 *          Expected result: all processes have the same local shape, equal to the original
 *              shape used to build the layout
 *      * hit_layFullShape()
 *         Expected result: the original shape is equal to the one used to build the layout.
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
    HitTopology topo = hit_topology(plug_topArray2DComplete);
    HitLayout lay = hit_layout(plug_layCopy, topo, sh);
    HitTile_int okGather, okLocal;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok;

    if(hit_layImLeader(lay)) printf("Check hit_layout_plug_layCopy: ");

    HitShape localSh = hit_layShape(lay);
    HitShape fullSh = hit_layFullShape(lay);
    ok = hit_shapeCmp(localSh, sh) && hit_shapeCmp(fullSh, sh);
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);


    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}