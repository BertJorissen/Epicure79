/**
 * Test for HitShape standard (size based) constructor hit_shapeStd and functions
 * hit_shapeSig and hit_sshapeDims
 * 
 * Cases checked:
 *      * One-dimensional HitShape
 *      * Two-dimensional HitShape
 *      * Three-dimensional HitShape
 *      * Four-dimensional HitShape
 * 
 *          Expected result: in all cases, the number of dimensions returned by the hit_sshapeDims function
 *              must match the number of dimensions of each case. In addition, the HitSigs in all dimensions
 *              returned by hit_shapeSig must be equal to one created by HitSig class' standard constructor
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main() {
    HitSig sig0 = hit_sigStd(10);
    HitSig sig1 = hit_sigStd(20);
    HitSig sig2 = hit_sigStd(30);
    HitSig sig3 = hit_sigStd(40);
    HitShape sh;
    HitSig array[4] = {sig0, sig1, sig2, sig3};
    int ok = 1;

    printf("Check hit_shapeStd, hit_shapeSig and hit_sshapeDims: ");

#ifdef DEBUG

    printf("\n");
    //
    printf("\t1D: ");
    sh = hit_shapeStd(1, 10);
    if(hit_sshapeDims(sh) != 1) printf("ERROR in hit_sshapeDims\n");
    else hit_sigCmp(sig0, hit_shapeSig(sh, 0)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\t2D: ");
    sh = hit_shapeStd(2, 10, 20);
    if(hit_sshapeDims(sh) != 2) printf("ERROR in hit_sshapeDims\n");
    else hit_sigCmp(sig0, hit_shapeSig(sh, 0)) && hit_sigCmp(sig1, hit_shapeSig(sh, 1)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\t3D: ");
    sh = hit_shapeStd(3, 10, 20, 30);
    if(hit_sshapeDims(sh) != 3) printf("ERROR in hit_sshapeDims\n");
    else hit_sigCmp(sig0, hit_shapeSig(sh, 0)) && hit_sigCmp(sig1, hit_shapeSig(sh, 1))
        && hit_sigCmp(sig2, hit_shapeSig(sh, 2)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\t4D: ");
    sh = hit_shapeStd(4, 10, 20, 30, 40);

    if(hit_sshapeDims(sh) != 4) printf("ERROR in hit_sshapeDims\n");
    else{
        for(int i=0; i<hit_sshapeDims(sh); i++) {
            if(! hit_sigCmp(array[i], hit_shapeSig(sh, i))) {
                printf("ERROR\n");
                break;
            }
        }
        printf("OK\n");
    }

#else

    // 1D
    sh = hit_shapeStd(1, 10);
    if(hit_sshapeDims(sh) != 1) ok = 0;
    else if(! hit_sigCmp(sig0, hit_shapeSig(sh, 0))) ok = 0;

    // 2D
    sh = hit_shapeStd(2, 10, 20);
    if(hit_sshapeDims(sh) != 2) ok = 0;
    else if(! hit_sigCmp(sig0, hit_shapeSig(sh, 0)) || !hit_sigCmp(sig1, hit_shapeSig(sh, 1))) ok = 0;
    
    // 3D
    sh = hit_shapeStd(3, 10, 20, 30);
    if(hit_sshapeDims(sh) != 3) ok = 0;
    else if(!hit_sigCmp(sig0, hit_shapeSig(sh, 0)) || !hit_sigCmp(sig1, hit_shapeSig(sh, 1))
        || !hit_sigCmp(sig2, hit_shapeSig(sh, 2))) ok = 0;
        
    // 4D
    sh = hit_shapeStd(4, 10, 20, 30, 40);

    if(hit_sshapeDims(sh) != 4) ok = 0;
    else{
        for(int i=0; i<hit_sshapeDims(sh); i++) {
            if(! hit_sigCmp(array[i], hit_shapeSig(sh, i))) {
                ok = 0;
                break;
            }
        }
    }

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    return 0;
}