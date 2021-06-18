/**
 * Test for HitTile function hit_tileElemAtArrayCoords
 * 
 * Cases checked:
 *      * 1D HitTile
 *      * 2D HitTile
 *      * 3D HitTile
 *      * 4D HitTile
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile1D, tile2D, tile3D, tile4D;
    HitSig sig = hit_sig(1, 20, 2);     // card = 10
    int ok = 1, i, j;

    hit_tileDomainShapeAlloc(&tile1D, int, hit_shape(1, sig));
    hit_tileDomainShapeAlloc(&tile2D, int, hit_shape(2, sig, sig));
    hit_tileDomainShapeAlloc(&tile3D, int, hit_shape(3, sig, sig, sig));
    hit_tileDomainShapeAlloc(&tile4D, int, hit_shape(4, sig, sig, sig, sig));
    
    printf("Check hit_tileElemAtArrayCoords: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\t1D: ");
    for(i=1; i<20; i+=2) hit_tileElemAtArrayCoords(tile1D, 1, i) = i;

    for(i=1; i<20; i+=2){
        if(tile1D.data[hit_sigArrayToTile(sig, i)] != i) ok = 0;   // qstride = 1
    }

    ok ? printf("OK\n") : printf("ERROR\n");

    //
    printf("\t2D: ");
    ok = 1;
    for(i=1; i<20; i+=2)
        for(j=1; j<20; j+=2) hit_tileElemAtArrayCoords(tile2D, 2, i, j) = j+i*10;
    

    for(i=1; i<20 && ok; i+=2){
        for(j=1; j<20; j+=2){
            if(tile2D.data[hit_sigArrayToTile(sig, i)*tile2D.origAcumCard[1] + hit_sigArrayToTile(sig, j)] != j+i*10) {ok = 0; break;}
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");

    //
    printf("\t3D: ");
    ok = 1;
    for(i=1; i<20; i+=2)
        for(j=1; j<20; j+=2) hit_tileElemAtArrayCoords(tile3D, 3, i, j, 2) = j+i*10;
    

    for(i=1; i<20 && ok; i+=2){
        for(j=1; j<20; j+=2){
            if(tile3D.data[hit_sigArrayToTile(sig, i)*tile3D.origAcumCard[1] + hit_sigArrayToTile(sig, j)*tile3D.origAcumCard[2] +
                hit_sigArrayToTile(sig, 2)] != j+i*10) {
                ok = 0; break;
            }
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");

    //
    printf("\t4D: ");
    ok = 1;
    for(i=1; i<20; i+=2)
        for(j=1; j<20; j+=2) hit_tileElemAtArrayCoords(tile4D, 4, i, j, 3, 2) = j+i*10;
    

    for(i=1; i<20 && ok; i+=2){
        for(j=1; j<20; j+=2){

            if(tile4D.data[hit_sigArrayToTile(sig, i)*tile4D.origAcumCard[1] + hit_sigArrayToTile(sig, j)*tile4D.origAcumCard[2] + 
                hit_sigArrayToTile(sig, 3)*tile4D.origAcumCard[3] + hit_sigArrayToTile(sig, 2)] != j+i*10) {
                ok = 0; break;
            }
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");
   
#else

    // 1D
        for(i=1; i<20; i+=2) hit_tileElemAtArrayCoords(tile1D, 1, i) = i;

        for(i=1; i<20; i+=2){
            if(tile1D.data[hit_sigArrayToTile(sig, i)] != i) ok = 0;   // qstride = 1
        }

    // 2D
        for(i=1; i<20; i+=2)
            for(j=1; j<20; j+=2) hit_tileElemAtArrayCoords(tile2D, 2, i, j) = j+i*10;
        

        for(i=1; i<20 && ok; i+=2){
            for(j=1; j<20; j+=2){
                if(tile2D.data[hit_sigArrayToTile(sig, i)*tile2D.origAcumCard[1] + hit_sigArrayToTile(sig, j)] != j+i*10) {ok = 0; break;}
            }
        }

    // 3D
        for(i=1; i<20; i+=2)
            for(j=1; j<20; j+=2) hit_tileElemAtArrayCoords(tile3D, 3, i, j, 2) = j+i*10;
        

        for(i=1; i<20 && ok; i+=2){
            for(j=1; j<20; j+=2){
                if(tile3D.data[hit_sigArrayToTile(sig, i)*tile3D.origAcumCard[1] + hit_sigArrayToTile(sig, j)*tile3D.origAcumCard[2] +
                    hit_sigArrayToTile(sig, 2)] != j+i*10) {
                    ok = 0; break;
                }
            }
        }

    // 4D
        for(i=1; i<20; i+=2)
            for(j=1; j<20; j+=2) hit_tileElemAtArrayCoords(tile4D, 4, i, j, 3, 2) = j+i*10;
    

        for(i=1; i<20 && ok; i+=2){
            for(j=1; j<20; j+=2){

                if(tile4D.data[hit_sigArrayToTile(sig, i)*tile4D.origAcumCard[1] + hit_sigArrayToTile(sig, j)*tile4D.origAcumCard[2] + 
                hit_sigArrayToTile(sig, 3)*tile4D.origAcumCard[3] + hit_sigArrayToTile(sig, 2)] != j+i*10) {
                    ok = 0; break;
                }
            }
        }

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}