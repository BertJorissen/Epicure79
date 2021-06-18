/**
 * Test for HitTile function hit_tileRoot
 * 
 * Cases checked:
 *      * A HitTile with no parent
 *          Expected result: a HitTile equal to itself
 *      * A HitTile with one parent
 *          Expected result: a HitTile equal to its parent
 *      * A HitTile with more than one ancestor
 *          Expected result: a HitTile equal to its further ancestor
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile, tileChild, tileChild2;
    HitTile * root;
    HitSig sig = hit_sig(1, 20, 2);
    HitSig sigChild = hit_sig(1, 10, 2);
    HitSig sigChild2 = hit_sig(1, 5, 2);
    HitShape sh = hit_shape(1, sig);
    HitShape shChild = hit_shape(1, sigChild);
    HitShape shChild2 = hit_shape(1, sigChild2);
    int ok = 1;

    hit_tileDomainShapeAlloc(&tile, int, sh);
    hit_tileSelect(&tileChild, &tile, shChild);
    hit_tileSelect(&tileChild2, &tileChild, shChild2);
    
    printf("Check hit_tileRoot: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\tA HitTile with no parent: ");
    root = hit_tileRoot(&tile);
    hit_shapeCmp(root->shape, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitTile with one parent: ");
    root = hit_tileRoot(&tileChild);
    hit_shapeCmp(root->shape, sh) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tA HitTile with more than one parent: ");
    root = hit_tileRoot(&tileChild2);
    hit_shapeCmp(root->shape, sh) ? printf("OK\n") : printf("ERROR\n");

#else

    // A HitTile with no parent
    root = hit_tileRoot(&tile);
    if(! hit_shapeCmp(root->shape, sh)) ok = 0;
    
    // A HitTile with one parent
    root = hit_tileRoot(&tileChild);
    if(! hit_shapeCmp(root->shape, sh)) ok = 0;
    
    // A HitTile with more than one parent
    root = hit_tileRoot(&tileChild2);
    if(! hit_shapeCmp(root->shape, sh)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}