/**
 * Test for HitShape cardinality and dimension functions:
 * hit_shapeDimsSet, hit_shapeSigCard and hit_shapeCard
 * 
 * hit_shapeDimsSet is deprecated and no longer used in any Hitmap example.
 * The set of a bigger number of dimensions than the original one of the HitShape may cause
 * a segmentation fault error when invoking other functions to that HitShape. In this
 * test those cases are not contemplated. 
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"

#define println(a) printf("%d\n", a)


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sig(0, 9, 2);
    HitShape sh;

    printf("Check hit_shapeDimsSet: ");
    sh = hit_shape2(sig0, sig1);
    hit_shapeDimsSet(sh, 1);
    hit_sshapeDims(sh) == 1 ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_shapeSigCard: ");
    hit_shapeSigCard(sh, 0) == hit_sigCard(sig0) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("Check hit_shapeCard: ");
    hit_shapeDimsSet(sh, 2);
    hit_shapeCard(sh) == (hit_sigCard(sig0)*hit_sigCard(sig1)) ? printf("OK\n") : printf("ERROR\n");

    return 0;
}