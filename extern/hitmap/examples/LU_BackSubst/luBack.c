/* 
* Hitmap implementation of equation system solver AX = B
* 	using LU factorization and Back substitution
*
* v0.1: (c) 2010, Carlos de Blas Carton
* v0.9: (c) 2012, Arturo Gonzalez-Escribano
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
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>
#include <hitmap.h>


/*
 * DECLARE USE OF TILES OF THE PROPER BASETYPE: double
 */
hit_tileNewType(double);

/*
 * PROTOTYPES
 */
void randomMat(HitBlockTile selection, int n1, int n2, int seed, double min, double max);
void print(HitBlockTile selection, const char * fileName );
void factorizeMat(HitLayout, HitBlockTile);
void solveMat(HitLayout, HitBlockTile, HitBlockTile);

/*
 * HELPER: PRINT USAGE AND EXIT
 */
void print_help_exit() {
	if ( hit_Rank == 0 ) fprintf(stderr, " \nUsage: lu <matrixSize> <bloqSize>\n\n");
	hit_comFinalize();
	exit( EXIT_FAILURE );
}


/*
 * MAIN (Timed stages)
 */
int main(int argc, char** argv) {
	HitClock processTime;
	HitClock initTime;

	// 1. INITIALIZE HITMAP COMMUNICATIONS
	hit_comInit(&argc,&argv);  

	// 2. READ PARAMETERS
	if ( argc < 2 ) print_help_exit();
	int n, blockSize;
	n = atoi(argv[1]);
	blockSize = atoi(argv[2]);
	if ( n < 1 || blockSize > n ) print_help_exit();

	// 3. VIRTUAL TOPOLOGY
	HitTopology topo;
	topo = hit_topology(plug_topArray2DComplete); 

	// 4. CREATION OF BLOCK TILE HIERARCHY FOR LOCAL BLOCKS
	// 4.1. DECLARE THE FULL DOMAINS
	HitTile_double matrix, vector;
	hit_tileDomain( &matrix, double, 2, n, n );
	hit_tileDomain( &vector, double, 2, 1, n ); 

	// 4.2. DECLARE THE MULTIBLOCK TILES
	int blockSizes[2] = { blockSize, blockSize };
	int blockSizesB[2] = { 1, blockSize };
	HitBlockTile matrixTiles, vectorTiles;
	hit_blockTileNew( &matrixTiles, &matrix, blockSizes );
	hit_blockTileNew( &vectorTiles, &vector, blockSizesB );

	// 4.3. COMPUTE LAYOUTS
	HitLayout vectorLayout = hit_layout(plug_layCyclic, topo, hit_tileShape( vectorTiles ), HIT_LAYOUT_NODIM ); 
	HitLayout matrixLayout = hit_layout(plug_layCyclic, topo, hit_tileShape( matrixTiles ), HIT_LAYOUT_NODIM ); 
	hit_comAllowDims( matrixLayout );


	// 5. ONLY ACTIVE PROCESSES CONTINUE WITH THE COMPUTATION
	// OPTIONAL: WARN ABOUT INACTIVE PROCESSORS IN THE LAYOUT
	//hit_layInactiveWarning( matrixLayout );
	if ( hit_layImActive( matrixLayout ) ) {

		// 5.1. DECLARE AND ALLOCATE LAYOUT SELECTIONS
		HitBlockTile matrixSelection, vectorSelection;
		hit_blockTileSelect( &matrixSelection, &matrixTiles, hit_layShape( matrixLayout ) );
		hit_blockTileSelect( &vectorSelection, &vectorTiles, hit_layShape( vectorLayout ) );

		hit_blockTileAlloc( &matrixSelection, double );
		hit_blockTileAlloc( &vectorSelection, double );

		// 5.2. INITIALIZE MATRICES WITH RANDOM VALUES
		hit_clockStart( initTime );
		randomMat( matrixSelection, n, n,  0xCAFE, -50.0, 50.0 );
		// ONLY PROCESSORS IN THE FIRST ROW HAVE PIECES OF THE VECTOR B
		if ( hit_laySelfRanksDim( matrixLayout, 0 ) == 0 ) 
			randomMat( vectorSelection, 1, n,  0xBECA, -50.0, 50.0 );
		hit_clockStop( initTime );

#ifdef WRITE_DATA
		print( matrixSelection, "A.dtxt" );
		if ( hit_laySelfRanksDim( matrixLayout, 0 ) == 0 ) print( vectorSelection, "B.dtxt" );
#else
		hit_clockReduce( matrixLayout, initTime );
		hit_clockPrintMax( initTime );
#endif

		// 5.3. FACTORIZE MATRIX LU = A
		hit_clockStart( processTime );
		factorizeMat( matrixLayout, matrixSelection );

		// 5.4. SOLVE SYSTEM  AX = B 
		solveMat( matrixLayout, matrixSelection, vectorSelection );
		hit_clockStop( processTime );

#ifdef WRITE_DATA
		print( matrixSelection, "LU.dtxt" );
		if ( hit_laySelfRanksDim( matrixLayout, 0 ) == 0 ) print( vectorSelection, "X.dtxt" );
#else
		hit_clockReduce( matrixLayout, processTime );
		hit_clockPrintMax( processTime );
#endif
		
		// 5.5. FREE DATA STRUCTURES
		hit_blockTileFree( matrixSelection );
		hit_blockTileFree( vectorSelection );

	} /* END hit_layImActive */

	// 6. FREE Hitmap RESOURCES
	hit_layFree( matrixLayout );
	hit_layFree( vectorLayout );
	hit_topFree( topo );

	// 7. FINALIZE Hitmap USAGE
	hit_comFinalize();
	return 0;
}


/*
 * MACROS FOR RENAMING DATA ACCESS METHODS
 * (USED DURING DEBUG, COULD BE FORWARD-SUBSTITUTED)
 */
#define matrixBlockAt(m,i,j)  hit_blockTileElemAtNS( m, double, 2, i, j )
#define matrixBufferAt(m,i,j)  hit_blockTileBufferAtNS( m, double, 2, i, j )
#define matrixBlockAtArrayCoords(m,i,j) hit_blockTileElemAtArrayCoords( m, double, 2, i, j )

HitCom comBroadcastDiagonalBlock;
HitCom comReduceVectorBlock; 

/*
 * RANDOM DATA INITIALIZATION OF A SELECTED BLOCK TILE
 */
void randomMat(HitBlockTile selection, int n1, int n2, int seed, double min, double max ) {
	int i;

	/* 1. CHECK PRECONDITION OF THE RANGE OF RANDOM VALUES */
	if (min>=max) return;

	/* 2. INITIALIZE RANDOM SEQUENCE WITH SEED */
	srand48((long)seed);

	/* 3. TRAVERSE THE WHOLE ARRAY COORDINATES SPACE, INITIALIZING ONLY THE TILE POSITIONS */
	int indBlock[2] = { 0, 0 };
	int ind[2] = { 0, 0 };

	for (ind[0] = 0; ind[0]<n1; ind[0]++ ) {
		// COMPUTE THE INDEX OF THE ROW OF BLOCKS, IF I'M NOT THE OWNER, SKIP THE WHOLE ROW
		indBlock[0] = ind[0] / selection.childSize[0];
		if ( ! hit_sigIn( hit_tileDimSig( selection, 0 ), indBlock[0] ) ) {
			for (i=0; i<n2; i++) drand48();
			continue;
		}

		// FOR THE BLOCKS OF THE COLUMNS
		int numBlocks = (int)ceil( (double)n2 / selection.childSize[1] );
		for (indBlock[1] = 0; indBlock[1] < numBlocks; indBlock[1]++ ) {
			// GET THE SIZE WITHOUT PADDING
			int size;
			if ( indBlock[1] != numBlocks-1 ) size = selection.childSize[1];
			else size = ( n2 % selection.childSize[1] == 0 ) ? selection.childSize[1] : n2 % selection.childSize[1];

			// SKIP IF I'M NOT THE OWNER OF THE BLOCK
			if ( ! hit_sigIn( hit_tileDimSig( selection, 1 ), indBlock[1] ) ) {
				for (i=0; i < size; i++) drand48();
				continue;
			}

			// GET BLOCK 
			HitTile_double block = matrixBlockAtArrayCoords( selection, indBlock[0], indBlock[1] );

			// FILL MY BLOCK (TAKE CARE WITH THE PADDING IN THE LAST BLOCK)
			int localInd = ind[0] % selection.childSize[0];
			for (ind[1] = 0; ind[1] < size; ind[1]++)
				hit_tileElemAtNoStride( block, 2, localInd, ind[1] ) = drand48()*(max-min)+min;
			
		}

	}
}


/*
 * PRINT ALL THE BLOCK OF A BLOCK-TILE IN A STREAM
 */
void print( HitBlockTile selection, const char * fileName ) {
	int ind[2];
	hit_tileForDimDomain ( selection, 0, ind[0] ) {
		hit_tileForDimDomain ( selection, 1, ind[1] ) {
			HitTile_double block = matrixBlockAt( selection, ind[0], ind[1] );
			hit_tileTextFileWrite( &block, fileName, HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 16, 6 );
		}
	}
}


/*
 * SEQUENTIAL MATRIX-MATRIX, MATRIX-VECTOR OPERATIONS
 */

/* 
 * SOLVES AX=B'  /  A MxM matrix, X and B MxN matrices
 * A is upper triangular
 * B' is B transposed
 * Solution in B
 * Requires m,n>=0	
 */
void strsmuv(HitTile_double a, HitTile_double b) {
	int j,k,i;
	double temp;

 	int n = hit_tileDimCard( b, 0 );
 	int m = hit_tileDimCard( b, 1 );
	if (m==0 || n==0) return;

	for(k=m-1;k>=0;k--) {
		for(i=m-1;i>k;i--) {
			if((temp=-hit_tileGetNoStride(a,2,k,i))!=0.0) {
				for (j=0;j<n;j++) {
					hit_tileElemAtNoStride(b,2,j,k) += temp * hit_tileGetNoStride(b,2,j,i);
				}
			}
		}

		temp=hit_tileGetNoStride(a,2,k,k);

		for(j=0;j<n;j++) {
			hit_tileSetNoStride(b,2,hit_tileGetNoStride(b,2,j,k)/temp,j,k);
		}
	}
}

/*
 * SOLVES AX=B' /  A MxM matrix, X and B MxN matrices
 * B' is B transposed (NxM)
 * A is lower triangular with 1's in the diagonal
 * Solution in B
 * Requires m,n>=0	
 */
void strsmlv(HitTile_double a, HitTile_double b) {
	int n = hit_tileDimCard( b, 0 ); 
	int m = hit_tileDimCard( b, 1 );

	int j,k,i;
	double temp;

	if (m==0 || n==0) return;

	for(k=0;k<m;k++) {
		for(i=0;i<k;i++) {
			if((temp=-hit_tileGetNoStride(a,2,k,i))!=0.0) {
				for (j=0;j<n;j++) {
					hit_tileElemAtNoStride(b,2,j,k) += temp * hit_tileGetNoStride(b,2,j,i);
				}
			}
		}
	}
}

/*
 * SOLVES XA=B /  A NxN matrix, X and B MxN matrices
 * A is lower triangular with 1's in the diagonal
 * Solution in B
 * Requires m,n>=0	
 */
void strsml2( HitTile_double a, HitTile_double b ) {
	int j,k,i;
	double temp;

	int m = hit_tileDimCard( b, 0 ); 
	int n = hit_tileDimCard( b, 1 ); 
	if (m==0 || n==0) return;

	for(k=0;k<n;k++) {
		for(j=0;j<m;j++) {
			if ( hit_tileGetNoStride(b,2,j,k) != 0.0 ) {
				hit_tileElemAtNoStride(b,2,j,k) = hit_tileGetNoStride(b,2,j,k)/hit_tileGetNoStride(a,2,k,k);
				temp=-hit_tileGetNoStride(b,2,j,k);
				for(i=k+1;i<n;i++) 
					hit_tileElemAtNoStride(b,2,j,i) += temp * hit_tileGetNoStride(a,2,k,i);
			}
		}
	}
}

/*
 * SOLVES C=C-AB /  A MxK matrix, B KxN matrix and C MxN matrix
 * Requires m,n,k>=0		
 */
void dgemm ( HitTile_double a, HitTile_double b, HitTile_double c ) {
	double temp;
	int i,j,l;

	int m = hit_tileDimCard( a, 0 );
	int k = hit_tileDimCard( a, 1 );
	int n = hit_tileDimCard( b, 1 );

	if (n==0 || m==0 || k==0) return;

	for(i=0;i<m;i++) {
		for(l=0;l<k;l++) {
			if((temp=-hit_tileGetNoStride(a,2,i,l))!=0.0) {
				for(j=0;j<n;j++) {
					hit_tileElemAtNoStride(c,2,i,j) += temp * hit_tileGetNoStride(b,2,l,j);
				}
			}
		}
	}
}

/*
 * SOLVES C'=C'-AB' /  A MxK matrix, B KxN matrix and C MxN matrix
 * B' is B transposed
 * Requires m,n,k>=0
 */
void dgemmv ( HitTile_double a, HitTile_double b, HitTile_double c ) {
	int m = hit_tileDimCard( a, 0 );
	int k = hit_tileDimCard( a, 1 );
	int n = hit_tileDimCard( b, 0 );

	double temp;
	int i,j,l;

	if (n==0 || m==0 || k==0) return;

	for(i=0;i<m;i++) {
		for(l=0;l<k;l++) {
			if((temp=-hit_tileGetNoStride(a,2,i,l))!=0.0) {
				for(j=0;j<n;j++) {
					hit_tileElemAtNoStride(c,2,j,i) += temp * hit_tileGetNoStride(b,2,j,l);
				}
			}
		}
	}
}


/* 
 * FACTORIZES MATRIX A = L*U 
 */ 
void factorizeMat( HitLayout layout, HitBlockTile mat ) {
	// stage: INDEX TO ADVANCE IN THE DIAGONAL OF BLOCKS
	// i,j: INDEXES FOR ROWS/COLUMNS OF BLOCKS
	// ii,jj: INDEXES FOR ROWS/COLUMNS OF ELEMENTS INSIDE A BLOCK	
	// k: INDEX TO ADVANCE IN THE DIAGONAL INSIDE A BLOCK
	// block: TEMPORAL BLOCK EXTRACTED FROM BLOCK TILES
	int stage, i, ii, j, jj, k;
	HitTile_double block;

	// GENERIC COMMUNICATIONS (SAME TYPE, ONLY SENDER AND BUFFERS ARE UPDATED)
	HitTile_double bufferBlock;
	hit_tileDomainAlloc( &bufferBlock, double, 2, hit_blockTileDimBlockSize( mat, 0 ), hit_blockTileDimBlockSize( mat, 1 ) );
	comBroadcastDiagonalBlock = hit_comBroadcastDimSelect( layout, 0, HIT_RANK_NULL, &bufferBlock, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, HIT_DOUBLE ); 

	// BUFFERS FOR ONE ROW AND ONE COLUMN OF THE MATRIX
	// FOR COMMUNICATIONS ACROSS A DIMENSION 
	// (LAYOUT PROPERTY: ALL PROCS. IN THE SAME DIMENSION INDEX HAVE THE SAME BLOCK SIZES)
	HitBlockTile bufferStageCol, bufferStageRow;
	hit_blockTileSelect( &bufferStageCol, &mat, hit_shape( 2, HIT_SIG_WHOLE, hit_sigIndex(0) ) );
	hit_blockTileSelect( &bufferStageRow, &mat, hit_shape( 2, hit_sigIndex(0), HIT_SIG_WHOLE ) );
	hit_blockTileAlloc( &bufferStageCol, double );
	hit_blockTileAlloc( &bufferStageRow, double );

	// COUNT THE NUMBER OF ASSIGNED BLOCKS ALREADY PROCESSED
	int processedBlocks[2] = { 0, 0 };

	// LOOP ACROSS THE DIAGONAL
	HitBlockTile *domain = hit_blockTileRoot( &mat );
	int numStages = hit_min( hit_tileDimCard( *domain, 0 ), hit_tileDimCard( *domain, 1 ) );
	for( stage=0; stage<numStages; stage++ ) {

		int stageRow = processedBlocks[0];
		int stageColumn = processedBlocks[1];
		if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) processedBlocks[0]++; 
		if ( hit_tileDimHasArrayCoord( mat, 1, stage ) ) processedBlocks[1]++; 

		// CREATE STAGE COMMUNICATIONS FOR THE UNPROCESSED BLOCKS IN THE ROW/COLUMN
		HitCom combloqfil, combloqfilrcv; 
		if( processedBlocks[1] < hit_tileDimCard( mat, 1 ) ) {
			combloqfilrcv = hit_comBroadcastDimSelect( layout, 0, hit_layDimOwner( layout, 0, stage ), &bufferStageRow, hit_shape2(hit_sigIndex(0), hit_sig( processedBlocks[1], hit_tileDimCard(mat,1)-1,1)), HIT_COM_TILECOORDS, HIT_DOUBLE ); 

			if ( stageRow < hit_tileDimCard( mat, 0 ) ) 
				combloqfil = hit_comBroadcastDimSelect( layout, 0, HIT_COM_MYSELF, &mat, hit_shape2( hit_sigIndex(stageRow), hit_sig(processedBlocks[1],hit_tileDimCard(mat,1)-1,1)), HIT_COM_TILECOORDS, HIT_DOUBLE ); 
		}

		HitCom combloqcol, combloqcolrcv;
		if( processedBlocks[0] < hit_tileDimCard(mat,0) ) {
			combloqcolrcv = hit_comBroadcastDimSelect( layout, 1, hit_layDimOwner( layout, 1, stage ), &bufferStageCol, hit_shape2( hit_sig( processedBlocks[0], hit_tileDimCard(mat,0)-1,1), hit_sigIndex(0)), HIT_COM_TILECOORDS, HIT_DOUBLE ); 

			if ( stageColumn < hit_tileDimCard(mat,1) ) 
				combloqcol = hit_comBroadcastDimSelect( layout, 1, HIT_COM_MYSELF, &mat, hit_shape2( hit_sig( processedBlocks[0], hit_tileDimCard(mat,0)-1,1), hit_sigIndex(stageColumn)), HIT_COM_TILECOORDS, HIT_DOUBLE ); 
		}

		// 
		// A. PROCESS WITH DIAGONAL BLOCK
		//
		if ( hit_tileHasArrayCoords( mat, 2, stage, stage ) ) {
			block = matrixBlockAtArrayCoords(mat, stage, stage);
			int numBlockRows = hit_tileDimCard( block, 0 );

			// k IS THE INDEX OF THE INSIDE-BLOCK DIAGONAL ELEMENT WE ARE UPDATING
			for( k=0; k < numBlockRows-1; k++ ) {
				double diagonalElement = hit_tileGetNoStride( block, 2, k, k );
				if ( diagonalElement == 0.0 ) fprintf(stderr,"Warning: 0.0 in position %d,%d\n",
							hit_tileTile2Array( block, 0, k ), hit_tileTile2Array( block, 1, k ) );

				// LAST ELEMENT DOES NOT NEED TO BE PROCESSED
				if ( k == numBlockRows-1 ) break;

				// L MULTIPLIER: UPDATE THE REST OF THIS COLUMN
				for( ii=k+1; ii < numBlockRows; ii++ ) {
					// AVOID A TRIVIAL 0.0/x DIVISION WITH A TEST
					double elem = hit_tileGetNoStride(block,2,ii,k);
					if ( elem != 0.0 ) hit_tileSetNoStride( block, 2, elem/diagonalElement, ii, k );
				}

				// SEND THE UPDATED COLUMN TO OTHER PROCESSES WITH THIS ROW OF BLOCKS
				HitCom comColBellowDiagonal = hit_comBroadcastDimSelect( layout, 1, 
						HIT_COM_MYSELF,
						&block, 
						hit_shape2( hit_sig( k+1, numBlockRows-1, 1 ), hit_sigIndex(k) ), 
						HIT_COM_TILECOORDS, HIT_DOUBLE ); 
				hit_comDo( &comColBellowDiagonal ); 
				hit_comFree( comColBellowDiagonal ); 

				// UPDATE TRAILING ROWS
				for( ii = k+1; ii < numBlockRows; ii++ ) {
					double elem = - hit_tileGetNoStride( block, 2, ii, k );
					if( elem !=0.0 ) {
						// UPDATE IN MY DIAGONAL BLOCK
						int numBlockColumns = hit_tileDimCard( block, 1 );
						for( jj=k+1; jj < numBlockColumns; jj++ ) {
							hit_tileElemAtNoStride( block, 2, ii, jj ) += elem * hit_tileGetNoStride(block,2,k,jj);
						}

						// UPDATE OTHER BLOCKS IN THE SAME BLOCKS ROW
						for( j = processedBlocks[1]; j < hit_tileDimCard(mat,1); j++ ) {
							HitTile_double updateBlock = matrixBlockAt( mat, stageRow, j );
							for( jj=0; jj < hit_tileDimCard( updateBlock, 1 ); jj++ ) {
								hit_tileElemAtNoStride( updateBlock, 2, ii, jj ) += elem * hit_tileGetNoStride(updateBlock,2,k,jj);
							}
						}
					}
				}
			}

			// SEND THE UPDATED BLOCK TO OTHER PROCESSES IN THE COLUMN
			hit_comUpdateDimSendTo( comBroadcastDiagonalBlock, 0, hit_laySelfRanksDim( layout, 0 ) );
			hit_comUpdateSendTile( comBroadcastDiagonalBlock, matrixBlockAt( mat, stageRow, stageColumn ) ); 
			hit_comDo( &comBroadcastDiagonalBlock ); 

			// UPDATE BLOCKS BELOW THE DIAGONAL BLOCK
			for( j = stageRow+1; j < hit_tileDimCard( mat, 0 ); j++ ) 
				strsml2( block, matrixBlockAt( mat, j, stageColumn ) );

			// SEND THE UPDATED BLOCKS TO OTHER PROCESSES IN THE SAME ROW
			if( stageColumn+1 < hit_tileDimCard( mat, 1 ) ) hit_comDo( &combloqfil ); 
			// SEND THE UPDATED BLOCKS OF MY ROW TO OTHER PROCESSES IN THE SAME COLUMN
			if( stageRow+1 < hit_tileDimCard( mat, 0 ) ) hit_comDo( &combloqcol ); 

			// UPDATE THE REST OF TRAILING BLOCKS (RIGHT/BELLOW THE DIAGONAL BLOCK)
			for( i = stageRow+1; i < hit_tileDimCard( mat, 0 ); i++) 
				for( j = stageColumn+1; j < hit_tileDimCard( mat, 1 ); j++ ) 
					dgemm( matrixBlockAt( mat, i, stageColumn ), matrixBlockAt( mat, stageRow, j ), matrixBlockAt( mat, i, j ) );

		} // END A. PROCESS WITH DIAGONAL BLOCK

		//
		// B. PROCESSES IN THE SAME COLUMN AS THE DIAGONAL
		//
		else if ( hit_tileDimHasArrayCoord( mat, 1, stage ) ) {
			// RECEIVE THE UPDATED DIAGONAL BLOCK
			hit_comUpdateDimSendTo( comBroadcastDiagonalBlock, 0, hit_layDimOwner( layout, 0, stage ) ); 
			hit_comUpdateSendTile( comBroadcastDiagonalBlock, bufferBlock ); 
			hit_comDo( &comBroadcastDiagonalBlock );   

			// UPDATE BLOCKS IN THE DIAGONAL COLUMN
			for( i = processedBlocks[0]; i < hit_tileDimCard( mat, 0 ); i++ ) {
				strsml2( bufferBlock, matrixBlockAt( mat, i, stageColumn ) );
			}

			// RECEIVE THE UPDATED ROW OF BLOCKS
			if( processedBlocks[1] < hit_tileDimCard( mat, 1 )) {
				hit_comDo( &combloqfilrcv ); 
			}

			// SEND MY UPDATED COLUMN TO OTHER PROCESSES IN THE SAME ROW
			if( processedBlocks[0] < hit_tileDimCard( mat, 0 )) {
				hit_comDo( &combloqcol ); 
			}

			// UPDATE TRAILING BLOCKS (BELLOW/RIGHT THE DIAGONAL)
			for( i = processedBlocks[0]; i < hit_tileDimCard( mat, 0 ); i++ ) {
				block = matrixBlockAt( mat, i, stageColumn );
				for( j = processedBlocks[1]; j < hit_tileDimCard( mat, 1 ); j++) {
					dgemm( block, matrixBlockAt( bufferStageRow, 0, j ), matrixBlockAt( mat, i, j ) );	  
				}
			}
		}
		// END B. PROCESSES IN THE SAME COLUMN AS THE DIAGONAL

		//
		// C. PROCESSES IN THE SAME ROW AS THE DIAGONAL
		//
		else if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) {
			block = matrixBlockAt( bufferStageCol, stageRow, 0 ); 
			int numRowsBlock = hit_tileDimCard( block, 0 );

			for( k=0; k < numRowsBlock-1; k++ ){
				// RECEIVE AN UPDATED COLUMN OF THE MATRIX FOR THE PROCESS WITH THE DIAGONAL BLOCK
				int owner = hit_layDimOwner( layout, 1, stage );
				HitCom comColBellowDiagonal = hit_comBroadcastDimSelect( layout, 1, owner, &block, hit_shape(2, hit_sig( k+1, numRowsBlock-1, 1), hit_sigIndex(0) ), HIT_COM_TILECOORDS, HIT_DOUBLE ); 
				hit_comDo( &comColBellowDiagonal ); 
				hit_comFree( comColBellowDiagonal ); 
			
				// UPDATE TRAILING BLCOKS IN THE ROW
				for( ii = k+1; ii < numRowsBlock; ii++ ) {
					double elem = - hit_tileGetNoStride( block, 2, ii, 0 );
					if( elem != 0.0 ) {
						for( j = processedBlocks[1]; j < hit_tileDimCard( mat, 1 ); j++ ) {
							HitTile_double updateBlock = matrixBlockAt( mat, stageRow, j );
							for( jj=0; jj < hit_tileDimCard( updateBlock, 1 ); jj++ ) {
								hit_tileElemAtNoStride( updateBlock, 2, ii, jj ) += elem * hit_tileGetNoStride( updateBlock, 2, k, jj );
							}
						}
					}
				}
			}

			// SEND THE UPDATED ROW TO OTHER PROCESSES IN THE COLUMN
			if( processedBlocks[1] < hit_tileDimCard(mat,1) ) hit_comDo( &combloqfil ); 

			// RECEIVE UPDATED COLUMN BLOCKS FROM THE PROCESS BELLOW THE DIAGONAL
			if( processedBlocks[0] < hit_tileDimCard(mat,0) ) hit_comDo( &combloqcolrcv ); 
	
			// UPDATE TRAILLING BLOCKS	
			for( i = processedBlocks[0]; i < hit_tileDimCard( mat, 0 ); i++ ) {
				block = matrixBlockAt( bufferStageCol, i, 0 );
				for( j = processedBlocks[1]; j < hit_tileDimCard( mat, 1 ); j++) {
					dgemm( block, matrixBlockAt( mat, stageRow, j ), matrixBlockAt( mat, i, j ) );	  
				}
			}
		}
		// END C. PROCESSES IN THE SAME ROW AS THE DIAGONAL


		// 
		// D. PROCESSES WITH NO BLOCKS IN THE DIAGONAL 
		//
		else {
			// RECEIVE ROW OF UPDATED BLOCKS FROM THE PROCESS OF MY COLUMN IN THE DIAGONAL ROW
			if( processedBlocks[1] < hit_tileDimCard(mat,1) ) hit_comDo( &combloqfilrcv ); 

			// RECEIVE COLUMN OF UPDATED BLOCKS FROM THE PROCESS OF MY ROW IN THE DIAGONAL COLUMN
			if( processedBlocks[0] < hit_tileDimCard(mat,0) ) hit_comDo( &combloqcolrcv ); 
		
			// UPDATE TRAILING MATRIX
			for( i = processedBlocks[0]; i < hit_tileDimCard( mat, 0 ); i++ ) {
				block = matrixBufferAt( bufferStageCol, i, 0 );
				for( j = processedBlocks[1]; j < hit_tileDimCard( mat, 1 ); j++) {
					dgemm( block, matrixBlockAt( bufferStageRow, 0, j ), matrixBlockAt( mat, i, j ) );	  
				}
			}
		} // END D. PROCESSES WITH NO BLOCKS IN THE DIAGONAL


		// ALL PROCESSES CHECK TO FREE GENERIC COMM. RESOURCES FOR THIS STAGE
		if( processedBlocks[1] < hit_tileDimCard( mat, 1 ) ) {
			hit_comFree( combloqfilrcv ); 
			if ( stageRow < hit_tileDimCard( mat, 0 ) ) hit_comFree( combloqfil ); 
		}
		if( processedBlocks[0] < hit_tileDimCard( mat, 0 ) ) {
			hit_comFree( combloqcolrcv ); 
			if ( stageColumn < hit_tileDimCard( mat, 1 ) ) hit_comFree(combloqcol); 
		}
	}

	// FREE Hitmap RESOURCES
	hit_comFree( comBroadcastDiagonalBlock );
	hit_tileFree( bufferBlock );
	hit_blockTileFree( bufferStageCol ); 
	hit_blockTileFree( bufferStageRow ); 
}


/*
 * SOLVER: EQUATION SYSTEM Ax=B
 */
void solveMat( HitLayout layout, HitBlockTile mat, HitBlockTile matb ) {
	// GENERIC COMMUNICATIONS (SAME TYPE, ONLY BUFFERS AND SENDER IDs ARE UPDATED)
	HitOp suma;
	hit_comOp(hit_comOpSumDouble,suma); 

	HitTile_double bufferVectorBlock;
	hit_tileDomainAlloc( &bufferVectorBlock, double, 2, 1, hit_blockTileDimBlockSize( matb, 1 ) );

	if ( hit_laySelfRanksDim( layout, 0 ) == 0 ) comReduceVectorBlock = hit_comReduceDimSelect( layout, 1, HIT_RANKS_NULL, &bufferVectorBlock, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, &bufferVectorBlock, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, HIT_DOUBLE, suma ); 

	// SKIP INACTIVE PROCESSES IN THE LAYOUT
	if( ! hit_layImActive( layout ) ) return;

	HitBlockTile bufferStageRow;
	hit_blockTileSelect( &bufferStageRow, &mat, hit_shape( 2, hit_sigIndex(0), HIT_SIG_WHOLE ) );
	hit_blockTileAlloc( &bufferStageRow, double );

	int j, jj;
	HitTile_double blockA, blockB;
	HitCom combloqfil,combloqfilrcv; 

	int processedBlocks[2] = { 0, 0 };

	//
	// PHASE 1. SOLVE Ly=b
	//
	int stage;
	HitBlockTile *domain = hit_blockTileRoot( &mat );
	int numStages = hit_min( hit_tileDimCard( *domain, 0 ), hit_tileDimCard( *domain, 1 ) );
	for( stage=0; stage < numStages; stage++ ) {
		int stageRow = processedBlocks[0];
		int stageColumn = processedBlocks[1];
		if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) processedBlocks[0]++; 
		if ( hit_tileDimHasArrayCoord( mat, 1, stage ) ) processedBlocks[1]++; 

		// RECEPTION OF DATA TO UPDATE VECTOR BLOCKS (ONLY FIRST ROW PROCESSES)
		if ( hit_tileDimHasArrayCoord( matb, 0, 0 ) ) 
			combloqfilrcv = hit_comRecvSelect( layout, hit_ranks2( hit_layDimOwner( layout, 0, stage ), hit_laySelfRanksDim( layout, 1 ) ), &bufferStageRow, hit_shape2( hit_sigIndex(0), hit_sig( 0, processedBlocks[1]-1, 1 ) ), HIT_COM_TILECOORDS, HIT_DOUBLE); 

		//
		// PROCESS WITH THE BLOCK OF THE VECTOR INDICATED BY stage
		//
		if ( hit_tileHasArrayCoords( matb, 2, 0, stage ) ) {
			blockB = matrixBlockAt( matb, 0, stageColumn );

			// BLOCKS OF THE MATRIX IN THIS PROCESS
			if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) { 
				for( j = stageColumn-1; j >= 0; j-- ) {
					dgemmv( matrixBlockAt(mat,stageRow,j), matrixBlockAt(matb,0,j), blockB );
				}
				blockA = matrixBlockAt( mat, stageRow, stageColumn ); 
			}
			// BLOCKS OF THE MATRIX IN OTHER PROCESS
			else {
				// RECEPTION OF ROW
				hit_comDo( &combloqfilrcv ); 

				for( j = stageColumn-1; j >= 0; j-- ) {
					dgemmv( matrixBlockAt(bufferStageRow,0,j), matrixBlockAt(matb,0,j), blockB );
				}
				blockA = matrixBlockAt( bufferStageRow, 0, stageColumn );
			}

			// REDUCTION OF PARTIAL RESULTS
			for( jj=0; jj < hit_tileDimCard( blockB, 1 ); jj++ ) 
				hit_tileElemAtNoStride( bufferVectorBlock, 1, jj ) = hit_tileElemAtNoStride( blockB, 2, 0, jj );

			hit_comUpdateDimSendTo( comReduceVectorBlock, 1, hit_layDimOwner( layout, 1, stage ) ); 
			hit_comUpdateRecvTile( comReduceVectorBlock, blockB ); 
			hit_comDo( &comReduceVectorBlock ); 

			strsmlv( blockA, blockB );
		}


		//
		// PROCESSES WITH OTHER BLOCKS OF THE VECTOR
		//
		else if ( hit_tileDimHasArrayCoord( matb, 0, 0 ) ) {
			for( j=0; j < hit_tileDimCard( bufferVectorBlock, 1 ); j++ ) hit_tileElemAtNoStride( bufferVectorBlock, 1, j ) = 0.0;

			// BLOCKS OF THE MATRIX IN THIS PROCESS
			if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) {
				for( j=processedBlocks[1]-1; j >= 0; j-- ) {
					dgemmv( matrixBlockAt(mat,stageRow,j), matrixBlockAt(matb,0,j), bufferVectorBlock );
				}
			}
			// BLOCKS OF THE MATRIX IN OTHER PROCESS
			else {
				// RECEPTION OF ROW
				if( processedBlocks[1] > 0 ) hit_comDo( &combloqfilrcv ); 

				for( j=processedBlocks[1]-1; j >= 0; j-- ) 
					dgemmv( matrixBlockAt(bufferStageRow,0,j), matrixBlockAt(matb,0,j), bufferVectorBlock );
			}

			// SENDING PARTIAL RESULTS
			hit_comUpdateDimSendTo( comReduceVectorBlock, 1, hit_layDimOwner( layout, 1, stage ) ); 
			hit_comDo( &comReduceVectorBlock ); 
		}

		//
		// PROCESSES WITH BLOCKS IN THE ROW INDICATED BY stage
		//
		else if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) {
			if ( processedBlocks[1] > 0 ) {
				combloqfil = hit_comSendSelect( layout, hit_ranks2( 0, hit_laySelfRanksDim( layout, 1 ) ), &mat, hit_shape2( hit_sigIndex( stageRow ), hit_sig( 0, processedBlocks[1]-1, 1) ), HIT_COM_TILECOORDS, HIT_DOUBLE); 
				hit_comDo( &combloqfil ); 
				hit_comFree( combloqfil ); 
			}
		}

		// FREE COMM. RESOURCES
		if ( hit_tileDimHasArrayCoord( matb, 0, 0 ) ) hit_comFree( combloqfilrcv ); 
	}


	//
	// PHASE 1. SOLVE Ux=y
	//
	processedBlocks[0] = hit_tileDimCard( mat, 0 )-1;
	processedBlocks[1] = hit_tileDimCard( mat, 1 )-1;
	for( stage = numStages-1; stage >= 0; stage-- ) {
		int stageRow = processedBlocks[0];
		int stageColumn = processedBlocks[1];
		if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) processedBlocks[0]--; 
		if ( hit_tileDimHasArrayCoord( mat, 1, stage ) ) processedBlocks[1]--; 

		if( processedBlocks[1]+1 < hit_tileDimCard( mat, 1 ) ) {
			if ( hit_tileDimHasArrayCoord( matb, 0, 0 ) )
				combloqfilrcv = hit_comRecvSelect( layout, hit_ranks2( hit_layDimOwner( layout, 0, stage), hit_laySelfRanksDim( layout, 1 ) ), &bufferStageRow, hit_shape2( hit_sigIndex(0), hit_sig( processedBlocks[1]+1, hit_tileDimCard(mat,1)-1, 1 ) ), HIT_COM_TILECOORDS, HIT_DOUBLE); 
		}

		//
		// PROCESS WITH THE BLOCK OF THE VECTOR INDICATED BY stage
		//
		if ( hit_tileHasArrayCoords( matb, 2, 0, stage ) ) {
			blockB = matrixBlockAt( matb, 0, stageColumn );
			
			if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) { //blocks of matrix are also mine
				for( j = stageColumn+1; j < hit_tileDimCard( matb, 1 ); j++ ) {
					dgemmv( matrixBlockAt(mat,stageRow,j), matrixBlockAt(matb,0,j), blockB );
				}
				blockA = matrixBlockAt( mat, stageRow, stageColumn );     
			}
			else {
				hit_comDo( &combloqfilrcv ); 

				for( j= stageColumn+1; j <hit_tileDimCard( matb, 1 ); j++ ) {
					dgemmv( matrixBlockAt(bufferStageRow,0,j), matrixBlockAt(matb,0,j), blockB );
				}
				blockA = matrixBlockAt( bufferStageRow, 0, stageColumn );
			}

			// REDUCTION OF PARTIAL RESULTS
			for( jj=0; jj < hit_tileDimCard( blockB, 1 ); jj++ ) 
				hit_tileElemAtNoStride( bufferVectorBlock, 1, jj ) = hit_tileElemAtNoStride( blockB, 2, 0, jj );

			hit_comUpdateDimSendTo( comReduceVectorBlock, 1, hit_layDimOwner( layout, 1, stage ) ); 
			hit_comUpdateRecvTile(comReduceVectorBlock, blockB ); 
			hit_comDo( &comReduceVectorBlock ); 
		
			strsmuv( blockA, blockB );
		}


		//
		// PROCESSES WITH OTHER BLOCKS OF THE VECTOR
		//
		else if ( hit_tileDimHasArrayCoord( matb, 0, 0 ) ) {
			// INITIALIZE ACCUMULATOR BLOCK WITH 0s
			for( j=0; j < hit_tileDimCard( bufferVectorBlock, 1 ); j++ ) hit_tileElemAtNoStride( bufferVectorBlock, 1, j ) = 0.0;

			if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) {
				for( j=processedBlocks[1]+1; j < hit_tileDimCard( matb, 1); j++ ) {
					dgemmv( matrixBlockAt(mat,stageRow,j), matrixBlockAt(matb,0,j), bufferVectorBlock );
				}
			}
			else {
				if( processedBlocks[1]+1 < hit_tileDimCard( matb, 1 ) ) hit_comDo( &combloqfilrcv ); 
				for( j=processedBlocks[1]+1; j < hit_tileDimCard( matb, 1 ); j++ )
					dgemmv( matrixBlockAt(bufferStageRow,0,j), matrixBlockAt(matb,0,j), bufferVectorBlock );
			}

			// SENDING PARTIAL RESULTS
			hit_comUpdateDimSendTo( comReduceVectorBlock, 1, hit_layDimOwner( layout, 1, stage ) ); 
			hit_comDo( &comReduceVectorBlock ); 
		}

		//
		// PROCESSES WITH BLOCKS IN THE ROW INDICATED BY stage
		//
		else if ( hit_tileDimHasArrayCoord( mat, 0, stage ) ) {
			if ( processedBlocks[1]+1 < hit_tileDimCard( mat, 1 ) ) {
				combloqfil = hit_comSendSelect( layout, hit_ranks2( 0, hit_laySelfRanksDim( layout, 1 ) ), &mat, hit_shape2( hit_sigIndex( stageRow ), hit_sig( processedBlocks[1]+1, hit_tileDimCard(mat,1)-1, 1) ), HIT_COM_TILECOORDS, HIT_DOUBLE); 
				hit_comDo( &combloqfil ); 
				hit_comFree( combloqfil ); 
			}
		}
		if ( processedBlocks[1]+1 < hit_tileDimCard( mat, 1 ) ) {
			if ( hit_tileDimHasArrayCoord( matb, 0, 0 ) ) hit_comFree(combloqfilrcv); 
		}
	}

	// FREE Hitmap RESOURCES
	hit_comOpFree(suma); 
	hit_comFree( comReduceVectorBlock );
	hit_tileFree( bufferVectorBlock );
	hit_blockTileFree( bufferStageRow ); 
}
