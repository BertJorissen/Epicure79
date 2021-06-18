/**
 * Test for HitTile function hit_tileDimHasArrayCoord and hit_tileHasArrayCoords
 * 
 * Cases checked (hit_tileDimHasArrayCoord):
 *      * HitSig's begin index
 *          Expected result: true
 *      * HitSig's begin index minus one
 *          Expected result: false
 *      * HitSig's last index
 *          Expected result: true
 *      * HitSig's last index plus one
 *          Expected result: false
 *      * Middle index calculated with stride
 *          Expected result: true
 *      * Middle index calculated with stride minus one
 *          Expected result: false
 *      * Middle index calculated with stride plus one
 *          Expected result: false
 * 
 * Cases checked (hit_tileHasArrayCoords):
 * For every dimension (1D, 2D, 3D, 4D). In the false cases, only one dimension fails,
 * the other indeces are correct
 *      * HitSig's begin index
 *          Expected result: true
 *      * HitSig's begin index minus one
 *          Expected result: false
 *      * HitSig's last index
 *          Expected result: true
 *      * HitSig's last index plus one
 *          Expected result: false
 *      * Middle index calculated with stride
 *          Expected result: true
 *      * Middle index calculated with stride minus one
 *          Expected result: false
 *      * Middle index calculated with stride plus one
 *          Expected result: false
 */


#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>


int main() {
    HitSig sig = hit_sig(0, 50, 4);
    HitSig sig2 = hit_sig(1, 50, 4);
    int ok = 1;
    hit_tileNewType(int);
    HitTile_int tile1D, tile2D, tile3D, tile4D;
    hit_tileDomainShape(&tile1D, int, hit_shape(1, sig));
    hit_tileDomainShape(&tile2D, int, hit_shape(2, sig, sig2));
    hit_tileDomainShape(&tile3D, int, hit_shape(3, sig, sig, sig2));
    hit_tileDomainShape(&tile4D, int, hit_shape(4, sig, sig, sig, sig2));

    printf("Check hit_tileDimHasArrayCoord: ");

#ifdef DEBUG

    printf("\n");
    printf("\tsig.begin: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.begin) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.begin out: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.begin-1) ? printf("ERROR\n") : printf("OK\n");
    //
    printf("\tsig last index: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.end-(sig.end%sig.stride)) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig last index out: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.end-(sig.end%sig.stride)+1) ? printf("ERROR\n") : printf("OK\n");
    // 
    printf("\tsig.stride: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.begin+sig.stride) ? printf("OK\n") : printf("ERROR\n");
    //
    printf("\tsig.stride left limit: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.begin+sig.stride-1) ? printf("ERROR\n") : printf("OK\n");
    //
    printf("\tsig.stride right limit: " );
    hit_tileDimHasArrayCoord(tile2D, 0, sig.begin+sig.stride+1) ? printf("ERROR\n") : printf("OK\n");

#else

    // sig.begin
    if(! hit_tileDimHasArrayCoord(tile2D, 0, sig.begin)) ok = 0;

    // sig.begin out
    if(hit_tileDimHasArrayCoord(tile2D, 0, sig.begin-1)) ok = 0;

    // sig last index
    if(! hit_tileDimHasArrayCoord(tile2D, 0, sig.end-(sig.end%sig.stride))) ok = 0;

    // sig last index out
    if(hit_tileDimHasArrayCoord(tile2D, 0, sig.end-(sig.end%sig.stride)+1)) ok = 0;

    // sig.stride
    if(! hit_tileDimHasArrayCoord(tile2D, 0, sig.begin+sig.stride)) ok = 0;

    // sig.stride left limit
    if(hit_tileDimHasArrayCoord(tile2D, 0, sig.begin+sig.stride-1)) ok = 0;

    // sig.stride right limit
    if(hit_tileDimHasArrayCoord(tile2D, 0, sig.begin+sig.stride+1)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");

#endif

    printf("Check hit_tileHasArrayCoords: ");
    ok = 1;

#ifdef DEBUG

    printf("\n");
    //
    printf("1D:\n");
        printf("\tsig.begin: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.begin) ? printf("OK\n") : printf("ERROR\n");
        
        printf("\tsig.begin out: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.begin-1) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig last index: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.end-(sig.end%sig.stride)) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig last index out: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.end-(sig.end%sig.stride)+1) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.begin+sig.stride) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig.stride left limit: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.begin+sig.stride-1) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride right limit: " );
        hit_tileHasArrayCoords(tile1D, 1, sig.begin+sig.stride+1) ? printf("ERROR\n") : printf("OK\n");

    //
    printf("2D:\n");
        printf("\tsig.begin: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.begin, sig2.begin) ? printf("OK\n") : printf("ERROR\n");
        
        printf("\tsig.begin out: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.begin, sig2.begin-1) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig last index: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.end-(sig.end%sig.stride), sig2.begin) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig last index out: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.end-(sig.end%sig.stride)+1, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.begin+sig.stride, sig2.begin) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig.stride left limit: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.begin+sig.stride-1, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride right limit: " );
        hit_tileHasArrayCoords(tile2D, 2, sig.begin+sig.stride+1, sig2.begin) ? printf("ERROR\n") : printf("OK\n");

    //
    printf("3D:\n");
        printf("\tsig.begin: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin, sig2.begin) ? printf("OK\n") : printf("ERROR\n");
        
        printf("\tsig.begin out: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin, sig2.begin-1) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig last index: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.end-(sig.end%sig.stride), sig.begin, sig2.begin) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig last index out: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.end-(sig.end%sig.stride)+1, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin+sig.stride, sig2.begin) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig.stride left limit: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin+sig.stride-1, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride right limit: " );
        hit_tileHasArrayCoords(tile3D, 3, sig.begin+sig.stride+1, sig.begin, sig2.begin) ? printf("ERROR\n") : printf("OK\n");

    //
    printf("4D:\n");
        printf("\tsig.begin: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin, sig.begin, sig2.begin) ? printf("OK\n") : printf("ERROR\n");
        
        printf("\tsig.begin out: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin-1, sig.begin, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig last index: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.end-(sig.end%sig.stride), sig.begin, sig.begin, sig2.begin) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig last index out: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin, sig.end-(sig.end%sig.stride)+1, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin+sig.stride, sig.begin, sig2.begin) ? printf("OK\n") : printf("ERROR\n");

        printf("\tsig.stride left limit: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin+sig.stride-1, sig.begin, sig2.begin) ? printf("ERROR\n") : printf("OK\n");
        
        printf("\tsig.stride right limit: " );
        hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin, sig.begin, sig2.begin+sig2.stride+1) ? printf("ERROR\n") : printf("OK\n");

#else

    // 1D
        // sig.begin
        if(! hit_tileHasArrayCoords(tile1D, 1, sig.begin)) ok = 0;
        
        // sig.begin out
        if(hit_tileHasArrayCoords(tile1D, 1, sig.begin-1)) ok = 0;
        
        // sig last index
        if(! hit_tileHasArrayCoords(tile1D, 1, sig.end-(sig.end%sig.stride))) ok = 0;

        // sig last index out
        if(hit_tileHasArrayCoords(tile1D, 1, sig.end-(sig.end%sig.stride)+1)) ok = 0;
        
        // sig.stride
        if(! hit_tileHasArrayCoords(tile1D, 1, sig.begin+sig.stride)) ok = 0;

        // sig.stride left limit
        if(hit_tileHasArrayCoords(tile1D, 1, sig.begin+sig.stride-1)) ok = 0;
        
        // sig.stride right limit
        if(hit_tileHasArrayCoords(tile1D, 1, sig.begin+sig.stride+1)) ok = 0;

    // 2D
        // sig.begin
        if(! hit_tileHasArrayCoords(tile2D, 2, sig.begin, sig2.begin)) ok = 0;
        
        // sig.begin out
        if(hit_tileHasArrayCoords(tile2D, 2, sig.begin, sig2.begin-1)) ok = 0;
        
        // sig last index
        if(! hit_tileHasArrayCoords(tile2D, 2, sig.end-(sig.end%sig.stride), sig2.begin)) ok = 0;

        // sig last index out
        if(hit_tileHasArrayCoords(tile2D, 2, sig.end-(sig.end%sig.stride)+1, sig2.begin)) ok = 0;
        
        // sig.stride
        if(! hit_tileHasArrayCoords(tile2D, 2, sig.begin+sig.stride, sig2.begin)) ok = 0;

        // sig.stride left limit
        if(hit_tileHasArrayCoords(tile2D, 2, sig.begin+sig.stride-1, sig2.begin)) ok = 0;
        
        // sig.stride right limit
        if(hit_tileHasArrayCoords(tile2D, 2, sig.begin+sig.stride+1, sig2.begin)) ok = 0;

    // 3D
        // sig.begin
        if(! hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin, sig2.begin)) ok = 0;
        
        // sig.begin out
        if(hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin, sig2.begin-1)) ok = 0;
        
        // sig last index
        if(! hit_tileHasArrayCoords(tile3D, 3, sig.end-(sig.end%sig.stride), sig.begin, sig2.begin)) ok = 0;

        // sig last index out
        if(hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.end-(sig.end%sig.stride)+1, sig2.begin)) ok = 0;
        
        // sig.stride
        if(! hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin+sig.stride, sig2.begin)) ok = 0;

        // sig.stride left limit
        if(hit_tileHasArrayCoords(tile3D, 3, sig.begin, sig.begin+sig.stride-1, sig2.begin)) ok = 0;
        
        // sig.stride right limit
        if(hit_tileHasArrayCoords(tile3D, 3, sig.begin+sig.stride+1, sig.begin, sig2.begin)) ok = 0;

    // 4D
        // sig.begin
        if(! hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin, sig.begin, sig2.begin)) ok = 0;
        
        // sig.begin out
        if(hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin-1, sig.begin, sig2.begin)) ok = 0;
        
        // sig last index
        if(! hit_tileHasArrayCoords(tile4D, 4, sig.end-(sig.end%sig.stride), sig.begin, sig.begin, sig2.begin)) ok = 0;

        // sig last index out
        if(hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin, sig.end-(sig.end%sig.stride)+1, sig2.begin)) ok = 0;
        
        // sig.stride
        if(! hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin+sig.stride, sig.begin, sig2.begin)) ok = 0;

        // sig.stride left limit
        if(hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin+sig.stride-1, sig.begin, sig2.begin)) ok = 0;
        
        // sig.stride right limit
        if(hit_tileHasArrayCoords(tile4D, 4, sig.begin, sig.begin, sig.begin, sig2.begin+sig2.stride+1)) ok = 0;

    ok ? printf("OK\n") : printf("ERROR\n");


#endif

    return 0;
}