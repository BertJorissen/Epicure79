/**
 * Test for HitTile functions
 *  hit_tileDimSig: get the HitSig of a HitTile in a dimension
 *  hit_tileDimBegin: get the begin index of that HitSig
 *  hit_tileDimEnd: get the end index of that HitSig
 *  hit_tileDimStride: get the stride of that HitSig
 *  hit_tileDimLast: get the last end index of that HitSig in tile coordinates
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {

    hit_tileNewType(int);
    HitTile_int tile;
    HitSig sig0 = hit_sigStd(4);
    HitSig sig1 = hit_sig(1,10,2);
    HitSig sig2;
    HitShape sh = hit_shape(2,sig0, sig1);
    hit_tileDomainShape(&tile, int, sh);

    sig2 = hit_tileDimSig(tile,1);
    printf("Check hit_tileDimSig: ");
    hit_sigCmp(sig0, hit_tileDimSig(tile,0)) && hit_sigCmp(sig1, sig2) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDimBegin: ");
    hit_tileDimBegin(tile, 1) == 1 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDimEnd: ");
    hit_tileDimEnd(tile, 1) == 10 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDimStride: ");
    hit_tileDimStride(tile, 1) == 2 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_tileDimLast: ");
    hit_tileDimLast(tile, 1) == 4 ? printf("OK\n") : printf("ERROR%d\n",hit_tileDimLast(tile, 1));

    return 0;
}