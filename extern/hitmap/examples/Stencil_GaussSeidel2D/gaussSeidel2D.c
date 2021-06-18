/*
* gaussSeidel2D.c
* 	Hitmap example
* 	Stencil code: Gauss-Seidel 2D for the heat equation. 
*
* v1.1
* (c) 2013-2015, Arturo Gonzalez-Escribano
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
static inline void updateCell( double *myself, double up, double down, double left, double right ) {
	*myself = ( up + down + left + right ) / 4;
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
	int i,j;

	/* 2.1. FIRST COLUMN IS MINE */
	if ( hit_sigIn( hit_tileDimSig( tileMat, 1 ), hit_tileDimBegin( root, 1 ) ) )
		hit_tileForDimDomain( tileMat, 0, i )
			hit_tileElemAt( tileMat, 2, i, 0 ) = 3;
		
	/* 2.2. LAST COLUMN IS MINE */
	if ( hit_sigIn( hit_tileDimSig( tileMat, 1 ), hit_tileDimEnd( root, 1 ) ) )
		hit_tileForDimDomain( tileMat, 0, i )
			hit_tileElemAt( tileMat, 2, i, hit_tileDimCard( tileMat, 1 )-1 ) = 4;

	/* 2.3. FIRST ROW IS MINE */
	if ( hit_sigIn( hit_tileDimSig( tileMat, 0 ), hit_tileDimBegin( root, 0 ) ) )
		hit_tileForDimDomain( tileMat, 1, j )
			hit_tileElemAt( tileMat, 2, 0, j ) = 1;
		
	/* 2.4. LAST ROW IS MINE */
	if ( hit_sigIn( hit_tileDimSig( tileMat, 0 ), hit_tileDimEnd( root, 0 ) ) )
		hit_tileForDimDomain( tileMat, 1, j )
			hit_tileElemAt( tileMat, 2, hit_tileDimCard( tileMat, 0 )-1, j ) = 2;
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

/* C. MAIN: GAUSS SEIDEL */
int main(int argc, char *argv[]) {

	hit_comInit( &argc, &argv );

	int rows, columns, numIter;

	if ( argc != 4 ) {
		fprintf(stderr, "\nUsage: %s <numRows> <numColumns> <numIterations>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	rows = atoi( argv[1] );
	columns = atoi( argv[2] );
	numIter = atoi( argv[3] );

	/* 0. INIT CLOCKS */
	hit_clockSynchronizeAll();
	hit_clockStart( mainClock );
	hit_clockStart( initClock );
	hit_clockReset( sequentialClock );

    /* 1. CREATE VIRTUAL TOPOLOGY */
    HitTopology topo = hit_topology( plug_topArray2DComplete );

	/* 2. DECLARE FULL MATRIX WITHOUT MEMORY */
	HitTile_double matrix;
	hit_tileDomain( &matrix, double, 2, rows, columns );

    /* 3. COMPUTE PARTITION */
	HitShape parallelShape = hit_tileShape( matrix );
	parallelShape = hit_shapeExpand( parallelShape, 2, -1 );
	HitLayout matLayout = hit_layout( plug_layBlocks, topo, parallelShape );

	/* 4. ACTIVE PROCESSES */
	if ( hit_layImActive( matLayout ) ) {

		/* 4.1. CREATE AND ALLOCATE LOCAL TILES */
		/* 4.1.1. LOCAL TILE WITH SPACE FOR FOREIGN DATA */
		HitTile_double tileMat;
		HitShape expandedShape = hit_shapeExpand( hit_layShape(matLayout), 2, 1 );
		hit_tileSelect( &tileMat, &matrix, expandedShape );
		hit_tileAlloc( &tileMat );

#ifdef DEBUG
		hit_dumpTileFile( tileMat, "Tile", "Matrix tile" );
#endif

		/* 4.2. COMMUNICATION PATTERNS */
		hit_comTagSet( TAG_UP, TAG_DOWN, TAG_LEFT, TAG_RIGHT );
		HitPattern pattBeforeComp = hit_pattern( HIT_PAT_UNORDERED );
		hit_patternAdd( &pattBeforeComp, 
				hit_comRecvSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 0, -1 ) , &tileMat, 
							hit_shape( 2, hit_sigIndex(0), hit_sig( 1, hit_tileDimCard(tileMat,1)-2, 1) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_DOWN )
							);
		hit_patternAdd( &pattBeforeComp, 
				hit_comRecvSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 1, -1 ) , &tileMat, 
							hit_shape( 2, hit_sig( 1, hit_tileDimCard(tileMat,0)-2, 1), hit_sigIndex(0) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_RIGHT )
							);

		HitPattern pattAfterComp = hit_pattern( HIT_PAT_UNORDERED );
		hit_patternAdd( &pattAfterComp, 
				hit_comSendRecvSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 0, -1 ) , &tileMat, 
							hit_shape( 2, hit_sigIndex(1), hit_sig( 1, hit_tileDimCard(tileMat,1)-2, 1) ),
							HIT_COM_TILECOORDS, 
							hit_layNeighbor( matLayout, 0, +1 ) , &tileMat, 
							hit_shape( 2, hit_sigIndex( hit_tileDimCard(tileMat,0)-1 ), hit_sig( 1, hit_tileDimCard(tileMat,1)-2, 1) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_UP )
							);
		hit_patternAdd( &pattAfterComp, 
				hit_comSendSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 0, +1 ) , &tileMat, 
							hit_shape( 2, hit_sigIndex( hit_tileDimCard(tileMat,0)-2 ), hit_sig( 1, hit_tileDimCard(tileMat,1)-2, 1) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_DOWN )
							);
		hit_patternAdd( &pattAfterComp, 
				hit_comSendRecvSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 1, -1 ) , &tileMat, 
							hit_shape( 2, hit_sig( 1, hit_tileDimCard(tileMat,0)-2, 1), hit_sigIndex(1) ),
							HIT_COM_TILECOORDS, 
							hit_layNeighbor( matLayout, 1, +1 ) , &tileMat, 
							hit_shape( 2, hit_sig( 1, hit_tileDimCard(tileMat,0)-2, 1), hit_sigIndex( hit_tileDimCard(tileMat,1)-1 ) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_LEFT )
							);
		hit_patternAdd( &pattAfterComp, 
				hit_comSendSelectTag( matLayout, 
							hit_layNeighbor( matLayout, 1, +1 ) , &tileMat, 
							hit_shape( 2, hit_sig( 1, hit_tileDimCard(tileMat,0)-2, 1), hit_sigIndex( hit_tileDimCard(tileMat,1)-2 ) ),
							HIT_COM_TILECOORDS, 
							HIT_DOUBLE, TAG_RIGHT )
							);

		/* 4.3. INITIALIZE MATRIX (IN PARALLEL) */
		initMatrix( tileMat );

		/* 4.4. COMPUTATION LOOP */
		int i,j;
		int loopIndex;
		for (loopIndex = 0; loopIndex < numIter; loopIndex++) {
			/* 4.4.1. COMMUNICATE BEFORE COMPUTE (Inner loop dependences) */
			hit_patternDo( pattBeforeComp );

			// CLOCK: Measure secuential time 
			hit_clockContinue( sequentialClock );

			/* 4.4.2. SEQUENTIALIZED LOOP */
			for ( i=1; i<hit_tileDimCard( tileMat, 0 )-1; i++ )
				for ( j=1; j<hit_tileDimCard( tileMat, 1 )-1; j++ )
					updateCell( 
						& hit_tileElemAt( tileMat, 2, i, j ),
						hit_tileElemAt( tileMat, 2, i-1, j ),
						hit_tileElemAt( tileMat, 2, i+1, j ),
						hit_tileElemAt( tileMat, 2, i, j-1 ),
						hit_tileElemAt( tileMat, 2, i, j+1 )
						);
			hit_clockStop( sequentialClock );

			/* 4.4.3. COMMUNICATE AFTER COMPUTE (External loop carried dependences) */
			hit_patternDo( pattAfterComp );
		}

		/* 4.5. CLOCK RESULTS */
		hit_clockStop( mainClock );
		hit_clockReduce( matLayout, mainClock );
		hit_clockReduce( matLayout, sequentialClock );
		hit_clockPrintMax( mainClock );
		hit_clockPrintMax( sequentialClock );

		/* 4.6. WRITE RESULT MATRIX */
#ifdef WRITE_RESULT
		HitTile root = *hit_tileRoot( &tileMat );
		HitTile_double outputTile;
		HitShape outputShape = hit_tileShape( tileMat );
		/* FIRST ROW IS NOT MINE */
		if ( ! hit_sigIn( hit_shapeSig( outputShape, 0 ), hit_tileDimBegin( root, 0 ) ) )
			outputShape	= hit_shapeTransform( outputShape, 0, HIT_SHAPE_BEGIN, +1 );
		/* LAST ROW IS NOT MINE */
		if ( ! hit_sigIn( hit_shapeSig( outputShape, 0 ), hit_tileDimEnd( root, 0 ) ) )
			outputShape = hit_shapeTransform( outputShape, 0, HIT_SHAPE_END, -1 );
		/* FIRST COLUMN IS NOT MINE */
		if ( ! hit_sigIn( hit_shapeSig( outputShape, 1 ), hit_tileDimBegin( root, 1 ) ) )
			outputShape = hit_shapeTransform( outputShape, 1, HIT_SHAPE_BEGIN, +1 );
		/* LAST COLUMN IS NOT MINE */
		if ( ! hit_sigIn( hit_shapeSig( outputShape, 1 ), hit_tileDimEnd( root, 1 ) ) )
			outputShape = hit_shapeTransform( outputShape, 1, HIT_SHAPE_END, -1 );
		hit_tileSelectArrayCoords( &outputTile, &tileMat, outputShape );
#ifdef WRITE_BIN
		hit_tileFileWrite( &outputTile, "Result.out", HIT_FILE_ARRAY );
#else
		hit_tileTextFileWrite( &outputTile, "Result.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 4 );
#endif
#endif
		/* 4.7. FREE RESOURCES */
		hit_tileFree( tileMat );
		hit_patternFree( &pattBeforeComp );
		hit_patternFree( &pattAfterComp );
	}
	/* 5. INACTIVE PROCESSES: ONLY COLLECTIVE CLOCK OPERATIONS */
	else {
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
