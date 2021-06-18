/** 
 * #nprocs: 1
 * 
 * Test for HitWeights constructors
 * 
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include "ciTests.h"


int main(int argc, char *argv[]) {

    hit_comInit( &argc, &argv );
    setbuf(stdout, NULL);
    int ok, size = 8;
    float weights[8] = {1,2,3,4,5,6,7,8};

    printf("Check hitWeightsEmpty: ");
    HitWeights w = hitWeightsEmpty(size);
    ok = (w.num_procs == size);
    for (int i=0; i<size; i++){
        if (w.ratios[i] != 0){ ok = 0; break;}
    }

    ok ? printf("OK\n") : printf("ERROR\n");

    //
    printf("Check hitWeights: ");
    w = hitWeights(size, weights);
    ok = (w.num_procs == size);
    for (int i=0; i<size; i++){
        if (w.ratios[i] != i+1){ ok = 0; break;}
    }

    ok ? printf("OK\n") : printf("ERROR\n");





    hit_comFinalize();

    return 0;
}