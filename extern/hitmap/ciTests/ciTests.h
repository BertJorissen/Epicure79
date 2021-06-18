/**
 * Auxiliar class with functions which are used in many test cases
 * in the test itself or with debug purposes.
 * 
 * @author Maria Sanchez Giron
 * @date Jun 2020
 */

#ifndef _AuxFunctions_
#define _AuxFunctions_

#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>


#define newline printf("\n");


/**
 * Pretty print a HitSig object.
 * 
 * This function prints a HitSig basic attributes: begin, end, stride and cardinality,
 * as well as all of its indexes in array coordinates.
 * 
 * @param sig The HitSig to be printed
 */
static inline void printHitSig(HitSig sig){
    printf("HitSig(begin=%d, end=%d, stride=%d, card=%d)\n", sig.begin, sig.end, sig.stride, hit_sigCard(sig));
    for(int i=sig.begin; i<=sig.end; i+=sig.stride) printf("%d ", i);
    printf("\n");
}

/**
 * Pretty print a HitShape object.
 * 
 * This function prints the num of dimensions of the HitShape and, for each dimension,
 * the begin, end, stride, cardinality, and all of its indexes in array coordinates.
 * 
 * @param sh The HitShape to be printed
 */
static inline void printHitShape(HitShape sh){
    HitSig sig;

    printf("HitShape(dims=%d)\n",hit_sshapeDims(sh));
    for(int dim=0; dim<hit_sshapeDims(sh); dim++){
        sig = hit_shapeSig(sh, dim);
        printf("Dim %d\n", dim);
        printf("\tHitSig(begin=%d, end=%d, stride=%d, card=%d)\n", sig.begin, sig.end, sig.stride, hit_sigCard(sig));
        printf("\t");
        for(int i=sig.begin; i<=sig.end; i+=sig.stride) printf("%d ", i);

        printf("\n");
    }
}

/**
 * Get the last real index (not necessarily the end index) of a HitSig in array coordinates.
 * 
 * @param sig The HitSig to get the last index from.
 * @return \e int The last index.
 */ 
static inline int getLastIndex(HitSig sig){
    return sig.end-(sig.end%sig.stride);
}

/**
 * Get the GCD (Greatest Common Divisor) of two integers.
 * 
 * @param a An integer.
 * @param b An integer.
 * @return \e int the GCD of @code a and @code b.
 */ 
static inline int gcd(int a, int b){  
    if(a == 0) return b;
    return gcd(b % a, a);  
}  

/**
 * Get the LCM (Least Common Multiple) of two integers.
 * 
 * @param a An integer.
 * @param b An integer.
 * @return \e int the GCD of @code a and @code b.
 */
static inline int lcm(int a, int b){  
    return (a*b) / gcd(a, b);  
} 

/**
 * Compare two arrays of integers. They are equal if they store the same values
 * and in the same order.
 * 
 * @param a An array of integers.
 * @param b An array of integers.
 * @param size_a The number of elements in @code a.
 * @param size_a The number of elements in @code b.
 * @retval 0 If the arrays' sizes are different or they don't have the same values
 * @retval 1 otherwise.
 */ 
static inline int arrayCmp(int a[], int b[], int size_a, int size_b){
    if(size_a != size_b) return 0;

    for(int i=0; i<size_a; i++){
        if(a[i] != b[i]) return 0;
    }

    return 1;
}

/**
 * Pretty print an array of integers.
 * 
 * @param a An array of integers.
 * @param size_a The number of elements in @code a.
 */ 
static inline void printArrayInt(int a[], int size_a){

    for(int i=0; i<size_a; i++){
        printf("%d ", a[i]);
    }
    printf("\n");

}

/**
 * Pretty print an array of doubles.
 * 
 * @param a An array of doubles.
 * @param size_a The number of elements in @code a.
 */ 
static inline void printArrayDouble(double a[], int size_a){
    for(int i=0; i<size_a; i++){
        printf("%.5lf ", a[i]);
    }
    printf("\n");

}

/**
 * Pretty print an array of floats.
 * 
 * @param a An array of floats.
 * @param size_a The number of elements in @code a.
 */ 
static inline void printArrayFloat(float a[], int size_a){
    for(int i=0; i<size_a; i++){
        printf("%.5f ", a[i]);
    }
    printf("\n");

}

/**
 * Gather the ok values of all processes and print the global result.
 * 
 * @param okLocal pointer to the local OK value.
 * @param okGather pointer to the global OK values.
 * @param lay The layout of the processes.
 */ 
static inline void printOKRoot(void * local, void * global, HitLayout lay){
    HitTile * okLocal = (HitTile *) local;
    HitTile * okGather = (HitTile *) global;
    int ok;
    //HitCom com = hit_comAlltoall(lay, okLocal, okGather, HIT_INT, 1);
    HitCom com = hit_comAllgather(lay, okLocal, okGather, HIT_INT, 1);
    hit_comDo(&com);
    hit_comBarrier(lay);
    //if(hit_layImLeader(lay)){
    if(hit_Rank == 0){
        ok = 1;
        int * a = (int *)okGather->data;
        for(int i=0; i<hit_NProcs; i++){
            if(a[i] == 0) {ok = 0; break;}
        }

        ok ? printf("OK\n") : printf("ERROR\n");
    }
}

#endif
