/**
 * Test for HitTile functions hit_tileUpdateFromAncestor, hit_tileUpdateToAncestor
 * and hit_tileFill
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile, tileChild;
    HitSig sig = hit_sig(1, 20, 2);
    HitSig sigChild = hit_sig(3, 10, 2);
    HitShape sh = hit_shape(2, sig, sig);
    HitShape shChild = hit_shape(2, sigChild, sigChild);
    int a = 2, b = 3, zero = 0;
    int i, j, ok;

    hit_tileDomainShapeAlloc(&tile, int, sh);
    hit_tileFill(&tile, &a);
    hit_tileSelect(&tileChild, &tile, shChild);
    hit_tileAlloc(&tileChild);
    hit_tileFill(&tileChild, &zero);


    printf("Check hit_tileFill: ");
    ok = 1;
    hit_tileForDimDomain(tile, 0, i){
        hit_tileForDimDomain(tile, 1, j){
            if(hit_tileElemAt(tile, 2, i, j) != a) {
                ok = 0; break;
            } 
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");
    //

    printf("Check hit_tileUpdateFromAncestor: ");
    hit_tileUpdateFromAncestor(&tileChild);
    ok = 1;
    hit_tileForDimDomain(tileChild, 0, i){
        hit_tileForDimDomain(tileChild, 1, j){
            if(hit_tileElemAt(tileChild, 2, i, j) != a) {
                ok = 0; break;
            }
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileUpdateToAncestor: ");
    hit_tileFill(&tileChild, &b);
    hit_tileUpdateToAncestor(&tileChild);
    ok = 1;
    hit_tileForDimDomain(tile, 0, i){
        hit_tileForDimDomain(tile, 1, j){
            if(hit_tileHasArrayCoords(tileChild, 2, hit_sigTileToArray(sig, i), hit_sigTileToArray(sig, j))){
                if(hit_tileElemAt(tile, 2, i, j) != b) {
                    ok = 0; break;
                }
            }
            else{
                if(hit_tileElemAt(tile, 2, i, j) != a) {
                    ok = 0; break;
                } 
            }
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");


    return 0;
}