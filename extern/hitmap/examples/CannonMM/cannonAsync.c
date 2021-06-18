/*
* cannonAsync.c
* 	Hitmap example
* 	Cannon's algorithm for Matrix Multiplication
*
* 	Asynchronous communications
*
* v1.1
* (c) 2007-2010, Arturo Gonzalez-Escribano
*/

/*
 * <license>
 * 
 * Hitmap v1.3
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>

#define	max(a,b)	((a)>(b)) ? (b):(a)

hit_tileNewType( double );

#define	MATRIX_AROWS		10
#define	MATRIX_ACOLUMNS		20
#define	MATRIX_BCOLUMNS		10

HitClock	mainClock;
HitClock	productClock;

void cannonsMM( int n, int m, int p );


/* A. MAIN: INVOCATION OF CANNON'S ALGORITHM */
int main(int argc, char *argv[]){
	hit_comInit( &argc, &argv );

	cannonsMM( MATRIX_AROWS, MATRIX_ACOLUMNS, MATRIX_BCOLUMNS );

	hit_comFinalize();
	return 0;
}


/* B. FUNCTION TO COMPUTE THE PRODUCT OF TWO LOCAL SUBMATRICES */
void matrixBlockProduct( HitTile_double A, HitTile_double B, HitTile_double C ) {
	int i,j,k;
#ifdef OPTIMIZE_PRODUCT
	double *datas[3] = { C.data, A.data, B.data };
	int rowSizes[3] ={  C.origAcumCard[1],A.origAcumCard[1],B.origAcumCard[1] };
#  ifdef OPTIMIZE_LOOPS
	int loopLimits[3] = { hit_tileDimCard(C,0), hit_tileDimCard(C,1), hit_tileDimCard(A,1) };
	for (i=0; i<loopLimits[0]; i++) {
		for (j=0; j<loopLimits[1]; j++) {
			for (k=0; k<loopLimits[2]; k++) {
#   else
	for (i=0; i<hit_tileDimCard(C,0); i++) {
		for (j=0; j<hit_tileDimCard(C,1); j++) {
			for (k=0; k<hit_tileDimCard(A,1); k++) {
#   endif
#else
	for (i=0; i<hit_tileDimCard(C,0); i++) {
		for (j=0; j<hit_tileDimCard(C,1); j++) {
			for (k=0; k<hit_tileDimCard(A,1); k++) {
#endif

#ifdef OPTIMIZE_PRODUCT
					datas[0][i*rowSizes[0]+j] =
						datas[0][i*rowSizes[0]+j] + datas[1][i*rowSizes[1]+k] * datas[2][k*rowSizes[2]+j] ;
#else
					hit_tileElemAtNoStride(C,2,i,j) = 
							hit_tileElemAtNoStride(C,2,i,j) 
							+ hit_tileElemAtNoStride(A,2,i,k) * hit_tileElemAtNoStride(B,2,k,j);
#endif
			}
		}
	}
}


/* C. INITIALIZE MATRICES */
void initMatrices( int m, int p, 
					HitTile_double tileA, HitTile_double tileB, HitTile_double tileC,
					HitLayout layA, HitLayout layB ) {
	/* 1. INIT C = 0 */
	double zero=0; 
	hit_tileFill( &tileA, &zero );
	hit_tileFill( &tileB, &zero );
	hit_tileFill( &tileC, &zero );

#if defined(READ_AB) || defined(WRITE_AB)
	HitTile_double realTileA, realTileB;
	hit_tileSelectArrayCoords( &realTileA, &tileA, hit_layShape( layA ) );
	hit_tileSelectArrayCoords( &realTileB, &tileB, hit_layShape( layB ) );
#endif

#ifdef READ_AB
#ifdef READ_BIN
	hit_tileFileRead( &realTileA, "A.in", HIT_FILE_ARRAY );
	hit_tileFileRead( &realTileB, "B.in", HIT_FILE_ARRAY );
#else
	hit_tileTextFileRead( &realTileA, "A.in.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
	hit_tileTextFileRead( &realTileB, "B.in.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#else
	int i,j;

	/* 2. INIT A(i,j) = ( i*numCol + j ) % 1000 */
	hit_layForDimDomain( layA, 0, i )  
		hit_layForDimDomain( layA, 1, j )  
			hit_tileElemAtArrayCoords( tileA, 2, i,j ) = ( i * m + j ) % 1000;

	/* 3. INIT B: DIFFERENT COMPILE-TIME OPTIONS */
#ifdef B_IDENTITY
	/* 3.a. INIT B = Identity */
	hit_layForDimDomain( layB, 0, i )  
		hit_layForDimDomain( layB, 1, j )  
			hit_tileElemAtArrayCoords( tileB, 2, i,j ) = (i==j) ?  1.0 : 0.0;
#else
	/* 3.b. INIT B = Random with seed */
	srand48( 364543 );
	for (i=0; i<m; i++) {
		for (j=0; j<p; j++) {
			if ( i>=hit_layDimBegin(layB,0) && i<=hit_layDimEnd(layB,0) && j>=hit_layDimBegin(layB,1) && j<=hit_layDimEnd(layB,1) ) 
				hit_tileElemAtArrayCoords( tileB, 2, i,j ) = drand48();
			else drand48();
		}
	}
#endif
#endif
#ifdef WRITE_AB
	/* 4. WRITE A,B ON FILES */
#ifdef WRITE_BIN
	hit_tileFileWrite( &realTileA, "A.out", HIT_FILE_ARRAY );
	hit_tileFileWrite( &realTileB, "B.out", HIT_FILE_ARRAY );
#else
#ifdef WRITE_TILE_FILES
	char name[8];
	sprintf(name, "A.%d", hit_Rank );
	hit_tileTextFileWrite( &realTileA, name, HIT_FILE_TILE, HIT_FILE_DOUBLE, 14, 4 );
	sprintf(name, "B.%d", hit_Rank );
	hit_tileTextFileWrite( &realTileB, name, HIT_FILE_TILE, HIT_FILE_DOUBLE, 14, 4 );
#endif
	hit_tileTextFileWrite( &realTileA, "A.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
	hit_tileTextFileWrite( &realTileB, "B.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#endif
}


/* D. PARALLEL CANNON'S ALGORITHM FOR MATRIX MULTIPLICATION */
void cannonsMM( int n, int m, int p )  {
	/* 0. INIT CLOCKS */
	hit_clockSynchronizeAll();
	hit_clockStart( mainClock );
	hit_clockReset( productClock );

    /* 1. CREATE VIRTUAL TOPOLOGY */
    HitTopology topo = hit_topology( plug_topSquare );

	/* 2. DECLARE FULL MATRICES WITHOUT MEMORY */
	HitTile_double A, B, C;
	hit_tileDomain( &A, double, 2, n, m );
	hit_tileDomain( &B, double, 2, m, p );
	hit_tileDomain( &C, double, 2, n, p );

    /* 3. COMPUTE PARTITIONS */
#define LAYOUT_BALANCED
#ifdef LAYOUT_BALANCED
	HitLayout layA = hit_layout(plug_layBlocksBalance, topo, hit_tileShape( A ), 0, 0.8f );
    HitLayout layC = hit_layout(plug_layBlocksBalance, topo, hit_tileShape( C ), 0, 0.8f );
#else
	HitLayout layA = hit_layout(plug_layBlocks, topo, hit_tileShape( A ) );
    HitLayout layC = hit_layout(plug_layBlocks, topo, hit_tileShape( C ) );
#endif
	HitLayout layB = hit_layout(plug_layBlocks, topo, hit_tileShape( B ) );
	
	hit_layWrapNeighbors( &layA );
	hit_layWrapNeighbors( &layB );

	/* 4. CREATE AND ALLOCATE TILES */
	HitTile_double tileA, tileB, tileC, copyTileA, copyTileB;
	hit_tileSelectNoBoundary( &tileA, &A, hit_layMaxShape(layA));
	hit_tileSelectNoBoundary( &tileB, &B, hit_layMaxShape(layB) );
	hit_tileSelect( &tileC, &C,  hit_layShape(layC) );
	hit_tileAlloc( &tileA ); hit_tileAlloc( &tileB ); hit_tileAlloc( &tileC );
	hit_tileClone( &copyTileA, &tileA ); hit_tileClone( &copyTileB, &tileB );

	/* 5. INITIALIZE MATRICES */
	initMatrices( m, p, tileA, tileB, tileC, layA, layB );

	/* 6. INITIAL ALIGNMENT PHASE */
	HitCom commRow, commColumn;
	commRow = hit_comShiftDim( layA, 1, -hit_laySelfRanksDim(layA,0), &tileA, HIT_DOUBLE );
	commColumn = hit_comShiftDim( layB, 0, -hit_laySelfRanksDim(layB,1), &tileB, HIT_DOUBLE );

	hit_comDo( &commRow );
	hit_comDo( &commColumn );
	hit_comFree( commRow );
	hit_comFree( commColumn );

	/* 7. REPEATED COMM PATTERN */
	HitPattern shift = hit_pattern( HIT_PAT_UNORDERED );
	hit_comTagSet( TAG_A, TAG_B );
	hit_patternAdd( &shift, hit_comShiftDimAsync(layA, 1, 1, &tileA, &copyTileA, HIT_DOUBLE, TAG_A));
	hit_patternAdd( &shift, hit_comShiftDimAsync(layB, 0, 1, &tileB, &copyTileB, HIT_DOUBLE, TAG_B));

	/* 8. DO COMPUTATION */
	int loopIndex;
	int loopLimit = max( layA.numActives[0], layA.numActives[1] );
	for (loopIndex = 0; loopIndex < loopLimit-1; loopIndex++) {
		/* 8.1. START COMMUNICATIONS FROM TILES TO REMOTE TILE BUFFERS */
		hit_patternStartAsync( shift );

		/* 8.2. CLOCK TO MEASURE SECUENTIAL TIME OF THE PRODUCT */
		hit_clockContinue( productClock );
		matrixBlockProduct( tileA, tileB, tileC );
		hit_clockStop( productClock );

		/* 8.3. END COMMUNICATIONS, MOVE DATA FROM TILE BUFFERS */
		hit_patternEndAsync( shift );
		hit_tileUpdateToAncestor( &copyTileA );
		hit_tileUpdateToAncestor( &copyTileB );
	}
	/* 9. LAST ITERATION MULTIPLICATION: NO COMMUNICATION AFTER */
	hit_clockContinue( productClock );
	matrixBlockProduct( tileA, tileB, tileC );
	hit_clockStop( productClock );

	/* 10. CLOCK RESULTS */
	hit_clockStop( mainClock );
	hit_clockReduce( layC, mainClock );
	hit_clockReduce( layC, productClock );
	hit_clockPrintMax( mainClock );
	hit_clockPrintMax( productClock );

	/* 11. WRITE RESULT MATRIX */
#ifdef WRITE_RESULT
#ifdef WRITE_BIN
	hit_tileFileWrite( &tileC, "C.out", HIT_FILE_ARRAY );
#else
	hit_tileTextFileWrite( &tileC, "C.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#endif

	/* 12. FREE RESOURCES */
	hit_tileFree( tileA ); hit_tileFree( tileB ); hit_tileFree( tileC );
	hit_tileFree( copyTileA ); hit_tileFree( copyTileB ); 
	hit_patternFree( &shift );
	hit_layFree( layA ); hit_layFree( layB ); hit_layFree( layC );
	hit_topFree( topo );
}

