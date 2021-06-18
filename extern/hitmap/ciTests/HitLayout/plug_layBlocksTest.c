/**
 * #nprocs: 1 4 5 18
 * 
 * Test for HitLayout constructor hit_layout_plug_layBlocks
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
    HitTile_int okLocal, okGather, myShape, neighShape;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok = 1, ok2 = 1;
    int i, zero = 0, two = 2;

    hit_tileDomainAlloc(&myShape, int, 1, 6);
    hit_tileDomainAlloc(&neighShape, int, 1, 6);
    hit_tileFill(&neighShape, &zero);
    hit_tileFill(&okGather, &two);

    topo = hit_topology(plug_topArray2D);
    lay = hit_layout(plug_layBlocks, topo, sh);

    sigDim0 = hit_shapeSig(hit_layShape(lay), 0);
    sigDim1 = hit_shapeSig(hit_layShape(lay), 1);
    hit_tileElemAt(myShape, 1, 0) = sigDim0.begin;
    hit_tileElemAt(myShape, 1, 1) = sigDim0.end;
    hit_tileElemAt(myShape, 1, 2) = sigDim0.stride;
    hit_tileElemAt(myShape, 1, 3) = sigDim1.begin;
    hit_tileElemAt(myShape, 1, 4) = sigDim1.end;
    hit_tileElemAt(myShape, 1, 5) = sigDim1.stride;
    
    
    if(hit_Rank == 0){
        printf("Check hit_layout_plug_layBlocks: ");
    }

    hit_comBarrier(lay);
    

    HitCom com;
    hit_layWrapNeighbors(&lay);
    switch (hit_NProcs) {
        case 1:
            com = hit_comSendRecv(lay, hit_layNeighbor(lay,0,1), &myShape, hit_layNeighbor(lay,0,1), &neighShape, HIT_INT);
            // printf("Proc[%d] Sendto: %d\n", hit_Rank, com.sendTo);
            // printf("Proc[%d] recvfrom: %d\n", hit_Rank, com.recvFrom);
            break;

        default:
            if (hit_Rank % 2 == 0){
                com = hit_comSendRecv(lay, hit_layNeighbor(lay,1,1), &myShape, hit_layNeighbor(lay,1,1), &neighShape, HIT_INT);
                // printf("Proc[%d] Sendto: %d\n", hit_Rank, com.sendTo);
                // printf("Proc[%d] recvfrom: %d\n", hit_Rank, com.recvFrom);
            }
            else{
                com = hit_comSendRecv(lay, hit_layNeighbor(lay,1,-1), &myShape, hit_layNeighbor(lay,1,-1), &neighShape, HIT_INT);
                // printf("Proc[%d] Sendto: %d\n", hit_Rank, com.sendTo);
                // printf("Proc[%d] recvfrom: %d\n", hit_Rank, com.recvFrom);
            }
            break;
    }

    hit_comDoSendRecv(com);
    hit_comBarrier(lay);
    
    // for(int j=0; j<hit_NProcs; j++){
    //     if (hit_Rank == j && hit_layImActive(lay)){
    //         printf("Proc [%d]:\n", hit_Rank);
    //         hit_tileForDimDomain(myShape, 0, i) printf("%d ", hit_tileElemAt(myShape, 2, i, 0));
    //         newline
    //         hit_tileForDimDomain(neighShape, 0, i) printf("%d ", hit_tileElemAt(neighShape, 2, i, 0));

    //         printf("\n");
    //     }
    //     // hit_comBarrier(lay);
    // }
    

    switch (hit_NProcs) {
        case 1:
            hit_tileForDimDomain(myShape, 0, i){
                if ( hit_tileElemAt(neighShape, 1, i) != hit_tileElemAt(myShape, 1, i) ){ ok = 0; break;}
            }
        
            break;
   
        default:
            if (hit_layImActive(lay)){
                for(i=0; i<3; i++)
                    if ( hit_tileElemAt(neighShape, 1, i) != hit_tileElemAt(myShape, 1, i) ){ ok = 0; break;}

                for(i=3; i<6; i++)
                    if ( hit_tileElemAt(neighShape, 1, i) != hit_tileElemAt(myShape, 1, i) ){ ok2 = 0; break;}

                ok = ok ^ ok2;
            }
            else{
                for(i=0; i<6; i++)
                    if ( hit_tileElemAt(neighShape, 1, i) != 0 ){ ok = 0; break;}
            }

            break;

    }
    // printf("Proc[%d] OK=%d\n", hit_Rank, ok);
    hit_tileFill(&okLocal, &ok);
    printOKRoot(&okLocal, &okGather, lay);
    hit_comBarrier(lay);

    hit_tileFree(okLocal);
    hit_tileFree(okGather);
    hit_layFree(lay);
    hit_topFree(topo);
    hit_comFinalize();

    return 0;
}