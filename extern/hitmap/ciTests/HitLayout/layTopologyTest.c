/** 
 * #nprocs: 1 4 5 18
 * 
 * Test for Hitmap different topologies.
 * 
 * Topologies checked:
 *  * plug_topPlain
 *  * plug_topPlainPower2
 *  * plug_topSquare
 *  * plug_topArray2DComplete
 * 
 * Functions checked for each topology: hit_layNumActives, hit_layDimNumActives
 * and hit_layActivesTopology
 * 
 * This test is made to work when executed with the number of processes above. It may
 * give incorrect results when working with other number of processers.
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"

hit_tileNewType(int);

int main(int argc, char *argv[]) {

    hit_comInit( &argc, &argv );
    setbuf(stdout, NULL);
    
    HitSig sig0 = hit_sigStd(20);
    HitSig sig1 = hit_sig(-1, 20, 2);
    HitShape sh = hit_shape(2, sig0, sig1);
    HitTopology topo = hit_topology(plug_topPlain);
    HitTopology topoActives;
    HitLayout lay = hit_layout(plug_layDimBlocks, topo, sh, 0);
    HitTile_int okGather, okLocal;
    hit_tileDomainAlloc(&okGather, int, 1, hit_NProcs);
    hit_tileDomainAlloc(&okLocal, int, 1, 1);
    int ok, actives0, actives1;
    
    // Plain Topology
        // hit_layNumActives
        if(hit_Rank == 0){
            printf("Check hit_topology_plug_topPlain: \n");
            printf("\thit_layNumActives: ");
        }

        hit_comBarrier(lay);
        ok = (hit_layNumActives(lay) == hit_NProcs);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);

        // hit_layDimActives
        if(hit_Rank == 0){
            printf("\thit_layDimActives: ");  
        }

        hit_comBarrier(lay);
        ok = (hit_layDimNumActives(lay,0) == hit_NProcs && hit_layDimNumActives(lay,1) == hit_sigCard(sig1) && hit_layDimNumActives(lay,2) == 0);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);
        
        // hit_layActivesTopology
        if(hit_Rank == 0){
            printf("\thit_layActivesTopology: ");  
        }
        hit_comBarrier(lay);
        topoActives = hit_layActivesTopology(lay);
        
        ok = (topoActives.active && topoActives.card[0] == hit_NProcs && topoActives.numDims == 1 &&
                topoActives.self.rank[0] == topo.self.rank[0]);
        
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        
        hit_comBarrier(lay);
        hit_layFree(lay);
        if (topoActives.active) hit_topFree(topoActives);
        hit_topFree(topo);


    // Plain power of 2 Topology
        topo = hit_topology(plug_topPlainPower2);
        lay = hit_layout(plug_layDimBlocks, topo, sh, 0);

        // hit_layNumActives
        if(hit_Rank == 0){
            printf("Check hit_topology_plug_topPlainPower2: \n");
            printf("\thit_layNumActives: ");
        }
        
        switch(hit_NProcs){
            case 1:
                actives0 = 1; break;
            case 4: case 5:
                actives0 = 4; break;
            case 18:
                actives0 = 16; break;   // if log2(nprocs) > sigcard => actives = sigcard
            default:
                actives0 = 1; break;
        }

        hit_comBarrier(lay);
        ok = (hit_layNumActives(lay) == actives0);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);
        //
        
        if(hit_Rank == 0){
            printf("\thit_layDimActives: ");  
        }
        hit_comBarrier(lay);
        ok = (hit_layDimNumActives(lay,0) == actives0 && hit_layDimNumActives(lay,1) == hit_sigCard(sig1) && hit_layDimNumActives(lay,2) == 0);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);

        // hit_layActivesTopology
        if(hit_Rank == 0){
            printf("\thit_layActivesTopology: ");  
        }
        hit_comBarrier(lay);
        topoActives = hit_layActivesTopology(lay);
        
        ok = (topoActives.active && topoActives.card[0] == actives0 && topoActives.numDims == 1 &&
                topoActives.self.rank[0] == topo.self.rank[0]);
        
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        
        hit_comBarrier(lay);
        hit_layFree(lay);
        if (topoActives.active) hit_topFree(topoActives);
        hit_topFree(topo);


    // Square Topology
        topo = hit_topology(plug_topSquare);
        lay = hit_layout(plug_layDimBlocks, topo, sh, 0);

        if(hit_Rank == 0){
            printf("Check hit_topology_plug_topSquare: \n");
            printf("\thit_layNumActives: ");
        }
        
        switch(hit_NProcs){
            case 1:
                actives0 = 1; actives1 = 1; break;
            case 4: case 5:
                actives0 = 2; actives1 = 2; break;
            case 18:
                actives0 = 4; actives1 = 4; break;
            default:
                actives0 = 1; actives1 = 1; break;
        }

        hit_comBarrier(lay);
        ok = (hit_layNumActives(lay) == actives0*actives1);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);
        //
        
        if(hit_Rank == 0){
            printf("\thit_layDimActives: ");  
        }
        hit_comBarrier(lay);
        ok = (hit_layDimNumActives(lay,0) == actives0 && hit_layDimNumActives(lay,1) == actives1 && hit_layDimNumActives(lay,2) == 0);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);

        // hit_layActivesTopology
        if(hit_Rank == 0){
            printf("\thit_layActivesTopology: ");  
        }
        hit_comBarrier(lay);
        topoActives = hit_layActivesTopology(lay);
        
        if (topoActives.active)
            ok = (topoActives.card[0] == actives0*actives1 && topoActives.numDims == 1 &&
                topoActives.self.rank[0] == (lay.pTopology)[0]->selfRank);

        else    // Inactive procs => topoActives = HIT_TOPOLOGY_NULL
            ok = (topoActives.card[0] == 0 && topoActives.numDims == 0 &&
                topoActives.self.rank[0] == HIT_RANK_NULL_STATIC);
        
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        
        hit_comBarrier(lay);
        hit_layFree(lay);
        if (topoActives.active) hit_topFree(topoActives);
        hit_topFree(topo);


    // Array 2D Complete Topology
        topo = hit_topology(plug_topArray2DComplete);
        lay = hit_layout(plug_layDimBlocks, topo, sh, 0);

        // hit_layNumActives
        if(hit_Rank == 0){
            printf("Check hit_topology_plug_topArray2DComplete: \n");
            printf("\thit_layNumActives: ");
        }
        
        switch(hit_NProcs){
            case 1:
                actives0 = 1; actives1 = 1; break;
            case 4:
                actives0 = 2; actives1 = 2; break;
            case 5:
                actives0 = 5; actives1 = 1; break;
            case 18:
                actives0 = 6; actives1 = 3; break;
            default:
                actives0 = 1; actives1 = 1; break;
        }

        hit_comBarrier(lay);
        ok = (hit_layNumActives(lay) == actives0*actives1);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);
        
        // hit_layDimActives
        if(hit_Rank == 0){
            printf("\thit_layDimActives: ");  
        }
        hit_comBarrier(lay);
        ok = (hit_layDimNumActives(lay,0) == actives0 && hit_layDimNumActives(lay,1) == actives1 && hit_layDimNumActives(lay,2) == 0);
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        hit_comBarrier(lay);

        // hit_layActivesTopology
        if(hit_Rank == 0){
            printf("\thit_layActivesTopology: ");  
        }
        hit_comBarrier(lay);
        topoActives = hit_layActivesTopology(lay);
        
        if (topoActives.active)
            ok = (topoActives.card[0] == actives0*actives1 && topoActives.numDims == 1 &&
                topoActives.self.rank[0] == (lay.pTopology)[0]->selfRank);

        else    // Inactive procs => topoActives = HIT_TOPOLOGY_NULL
            ok = (topoActives.card[0] == 0 && topoActives.numDims == 0 &&
                topoActives.self.rank[0] == HIT_RANK_NULL_STATIC);
        
        hit_tileFill(&okLocal, &ok);
        printOKRoot(&okLocal, &okGather, lay);
        
        hit_comBarrier(lay);
        hit_layFree(lay);
        if (topoActives.active) hit_topFree(topoActives);
        hit_topFree(topo);
    

    hit_comFinalize();

    return 0;
}