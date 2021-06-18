/*
* jacobi1D.c
* 	Hitmap example
* 	Stencil code: Jacobi 1D for the heat equation. Implemented as a 1D cellular automata. 
*
* v1.0
* (c) 2019, Arturo Gonzalez-Escribano
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

hit_tileNewType( double );

HitClock	mainClock;
HitClock	initClock;
HitClock	sequentialClock;

/* A. SEQUENTIAL: CELL UPDATE */
static inline void updateCell( double *myself, double left, double right ) {
	*myself = ( left + right ) / 2;
}


/* B. INITIALIZE MATRIX */
void initMatrix( HitTile_double tileMat ) {

	/* 0. OPTIONAL COMPILATION: READING THE INPUT MATRIX FROM A FILE */
#ifdef READ_INPUT
#ifdef READ_BIN
	hit_tileFileRead( &tileMat, "Matrix.in", HIT_FILE_ARRAY );
#else
	hit_tileTextFileRead( &realTileA, "Matrix.in.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#else
	/* 1. INIT Mat = 0 */
	double zero=0; 
	hit_tileFill( &tileMat, &zero );

	/* 2. INIT BORDERS  UP(i)=1, DOWN(i)=2, LEFT(i)=3, RIGHT(i)=4 */
	HitTile root = *hit_tileRoot( &tileMat );

	/* 2.1. FIRST ELEMENT IS MINE */
	if ( hit_sigIn( hit_tileDimSig( tileMat, 0 ), hit_tileDimBegin( root, 0 ) ) )
		hit_tileElemAt( tileMat, 1, 0 ) = 2;
		
	/* 2.2. LAST ELEMENT IS MINE */
	if ( hit_sigIn( hit_tileDimSig( tileMat, 0 ), hit_tileDimEnd( root, 0 ) ) )
		hit_tileElemAt( tileMat, 1, hit_tileDimCard( tileMat, 0 )-1 ) = 3;
#endif

#ifdef WRITE_INPUT
	/* 4. WRITE MAT ON A FILE */
#ifdef WRITE_BIN
	hit_tileFileWrite( &tileMat, "Matrix.out", HIT_FILE_ARRAY );
#else
#ifdef WRITE_TILE_FILES
	char name[8];
	sprintf(name, "Matrix.%d", hit_Rank );
	hit_tileTextFileWrite( &timeMat, name, HIT_FILE_TILE, HIT_FILE_DOUBLE, 14, 4 );
#endif
	hit_tileTextFileWrite( &tileMat, "Matrix.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#endif
}

/* C. MAIN: CELLULAR AUTOMATA */
int main(int argc, char *argv[]) {

	hit_comInit( &argc, &argv );

	int size, numIter;

	if ( argc != 3 ) {
		fprintf(stderr, "\nUsage: %s <size> <numIterations>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	size = atoi( argv[1] );
	numIter = atoi( argv[2] );

	/* 0. INIT CLOCKS */
	hit_clockSynchronizeAll();
	hit_clockStart( mainClock );
	hit_clockStart( initClock );
	hit_clockReset( sequentialClock );

	/* 1. CREATE VIRTUAL TOPOLOGY */
	HitTopology topo = hit_topology( plug_topPlain );

	/* 2. DECLARE FULL MATRIX WITHOUT MEMORY */
	HitTile_double matrix;
	hit_tileDomain( &matrix, double, 1, size );

	/* 3. COMPUTE PARTITION */
	HitShape parallelShape = hit_tileShape( matrix );
	parallelShape = hit_shapeExpand( parallelShape, 1, -1 );
	HitLayout matLayout = hit_layout( plug_layBlocks, topo, parallelShape );

	/* 4. ACTIVE PROCESSES */
	if ( hit_layImActive( matLayout ) ) {

		/* 4.1. CREATE AND ALLOCATE LOCAL TILES */
		/* 4.1.1. LOCAL TILE WITH SPACE FOR FOREIGN DATA */
		HitTile_double tileMat, tileCopy;
		HitShape expandedShape = hit_shapeExpand( hit_layShape(matLayout), 1, 1 );
		hit_tileSelect( &tileMat, &matrix, expandedShape );
		hit_tileAlloc( &tileMat );

#ifdef DEBUG
printf("Process[%d] Shape: (%d:%d)\n", hit_Rank,
		hit_tileDimBegin( tileMat, 0 ),
		hit_tileDimEnd( tileMat, 0 ) );
#endif

		/* 4.1.2. COPY OF THE TILE, TO AVOID SEQUENTIAL SEMANTICS ON UPDATES */
		hit_tileSelect( &tileCopy, &tileMat, HIT_SHAPE_WHOLE );
		hit_tileAlloc( &tileCopy );

#ifdef DEBUG
		hit_dumpTileFile( tileMat, "Tile", "Matrix tile" );
#endif

		/* 4.2. COMMUNICATION PATTERN */
		hit_comTagSet( TAG_LEFT, TAG_RIGHT );
		HitPattern neighSync = hit_pattern( HIT_PAT_UNORDERED );
		hit_patternAdd( &neighSync, 
				hit_comSendRecvSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 0, -1 ) , &tileMat, 
							hit_shape( 1, hit_sigIndex(1) ),
							HIT_COM_TILECOORDS, 
							hit_layNeighbor( matLayout, 0, +1 ) , &tileMat, 
							hit_shape( 1, hit_sigIndex( hit_tileDimCard(tileMat,0)-1 ) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_LEFT )
							);
		hit_patternAdd( &neighSync, 
				hit_comSendRecvSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 0, +1 ) , &tileMat, 
							hit_shape( 1, hit_sigIndex( hit_tileDimCard(tileMat,0)-2 ) ),
							HIT_COM_TILECOORDS, 
							hit_layNeighbor( matLayout, 0, -1 ) , &tileMat, 
							hit_shape( 1, hit_sigIndex(0) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_RIGHT )
							);

		/* 4.3. INITIALIZE MATRIX (IN PARALLEL) */
		initMatrix( tileMat );

		/* 4.4. COMPUTATION LOOP */
		int i;
		int loopIndex;
		for (loopIndex = 0; loopIndex < numIter-1; loopIndex++) {
			// CLOCK: Measure secuential time 
			hit_clockContinue( sequentialClock );

			/* 4.4.1. UPDATE TILE COPY */
			hit_tileUpdateFromAncestor( &tileCopy );

			/* 4.4.2. SEQUENTIALIZED LOOP */
			for ( i=1; i<hit_tileDimCard( tileMat, 0 )-1; i++ )
				updateCell( & hit_tileElemAt( tileMat, 1, i ),
						hit_tileElemAt( tileCopy, 1, i-1 ),
						hit_tileElemAt( tileCopy, 1, i+1 )
						);
			hit_clockStop( sequentialClock );

			/* 4.4.3. COMMUNICATE */
			hit_patternDo( neighSync );
		}

		/* 4.5. LAST ITERATION UPDATE: NO COMMUNICATION AFTER */
		hit_clockContinue( sequentialClock );
		hit_tileUpdateFromAncestor( &tileCopy );
		for ( i=1; i<hit_tileDimCard( tileMat, 0 )-1; i++ )
			updateCell( & hit_tileElemAt( tileMat, 1, i ),
					hit_tileElemAt( tileCopy, 1, i-1 ),
					hit_tileElemAt( tileCopy, 1, i+1 )
					);
		hit_clockStop( sequentialClock );

		/* 4.6. CLOCK RESULTS */
		hit_clockStop( mainClock );
		hit_clockReduce( matLayout, mainClock );
		hit_clockReduce( matLayout, sequentialClock );
		hit_clockPrintMax( mainClock );
		hit_clockPrintMax( sequentialClock );

		/* 4.7. WRITE RESULT MATRIX */
#ifdef WRITE_RESULT
		HitTile root = *hit_tileRoot( &tileMat );
		HitTile_double outputTile;
		HitShape outputShape = hit_tileShape( tileMat );
		/* FIRST ELEM IS NOT MINE */
		if ( ! hit_sigIn( hit_shapeSig( outputShape, 0 ), hit_tileDimBegin( root, 0 ) ) )
			outputShape	= hit_shapeTransform( outputShape, 0, HIT_SHAPE_BEGIN, +1 );
		/* LAST ELEM IS NOT MINE */
		if ( ! hit_sigIn( hit_shapeSig( outputShape, 0 ), hit_tileDimEnd( root, 0 ) ) )
			outputShape = hit_shapeTransform( outputShape, 0, HIT_SHAPE_END, -1 );
		hit_tileSelectArrayCoords( &outputTile, &tileMat, outputShape );
#ifdef WRITE_BIN
		hit_tileFileWrite( &outputTile, "Result.out", HIT_FILE_ARRAY );
#else
		hit_tileTextFileWrite( &outputTile, "Result.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#endif
		/* 4.8. FREE RESOURCES */
		hit_tileFree( tileMat );
		hit_tileFree( tileCopy );
		hit_patternFree( &neighSync );
	}
	/* 5. INACTIVE PROCESSES: ONLY COLLECTIVE CLOCK OPERATIONS */
	else {
		printf("Warning -- Non-active process %d\n", hit_Rank );
		hit_clockStop( initClock );
		hit_clockStop( mainClock );
		hit_clockReduce( matLayout, mainClock );
		hit_clockReduce( matLayout, sequentialClock );
		hit_clockPrintMax( mainClock );
		hit_clockPrintMax( sequentialClock );
	}

	/* 6. FREE OTHER RESOURCES */
	hit_layFree( matLayout );
	hit_topFree( topo );
	hit_comFinalize();
	return 0;
}
