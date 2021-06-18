/*
* quickSort.c
* 	Hitmap example
* 	Parallel Quick-Sort algorithm for generic numbers of native type 'double'
*
* v1.0
* (c) 2013, Arturo Gonzalez-Escribano
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
HitClock	seqCompClock;

#define swap(a,b)	{ double c=a; a=b; b=c; }

/* PROTOTYPES */
void parallelQuickSort( HitTile_double *data, HitLayout whole ) ;
int sequentialPivoting( double pivot, HitTile_double data ) ;
void initV( HitTile_double tileV ) ;

// ALTERNATIVE USING qsort(3), SLOWER
#ifdef USE_QSORT3
int comparisonOp( const void *a, const void *b ) {
	const double *da = (const double *)a;
	const double *db = (const double *)b;
	if ( *da < *db ) return -1;
	else if ( *da == *db ) return 0;
	else return 1;
}
#endif

/* A. MAIN */
int main(int argc, char *argv[]){
	hit_comInit( &argc, &argv );

	int numElems;

	/* 0. READ PARAMETER */
	if ( argc < 2 ) {
		fprintf(stderr, "Usage: %s <numElems>\n", argv[0] );
		exit( 1 );
	}
	else {
		numElems = atoi( argv[1] );
		if ( numElems < 0 ) {
			fprintf(stderr, "Usage: %s <numElems>\n", argv[0] );
			exit( 1 );
		}
	}

	/* 1. INIT CLOCKS */
	hit_clockSynchronizeAll();
	hit_clockStart( mainClock );
	hit_clockReset( seqCompClock );

    /* 2. CREATE VIRTUAL TOPOLOGY */
    HitTopology topo = hit_topology( plug_topPlain );

	/* 3. DECLARE FULL VECTOR WITHOUT MEMORY */
	HitTile_double V;
	hit_tileDomain( &V, double, 1, numElems );

    /* 4. COMPUTE ORIGINAL PARTITION */
	HitLayout lay = hit_layout(plug_layBlocks, topo, hit_tileShape( V ) );

	/* 5. CREATE AND ALLOCATE LOCAL TILE */
	HitTile_double tileV;
	hit_tileSelect( &tileV, &V, hit_layShape(lay) );
	hit_tileAlloc( &tileV );

	/* 6. INITIALIZE LOCAL TILE */
	initV( tileV );

	/* 7. START RECURSIVE PARTITION AND SORTING */
	parallelQuickSort( &tileV, lay );

	/* 8. CLOCK RESULTS */
	hit_clockStop( mainClock );
	hit_clockReduce( lay, mainClock );
	hit_clockReduce( lay, seqCompClock );
	hit_clockPrintMax( mainClock );
	hit_clockPrintMax( seqCompClock );

#ifdef WRITE_RESULT
	/* 9. WRITE RESULT VECTOR */
	hit_tileTextFileWrite( &tileV, "V.result.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 12 );
#endif

	/* 10. FREE RESOURCES */
	hit_tileFree( tileV );
	hit_layFree( lay );
	hit_topFree( topo );

	hit_comFinalize();
	return 0;
}


/* B. INITIALIZE VECTOR */
void initV( HitTile_double tileV ) {
	int i;

	/* 1. RANDOM SEED */
	srand48( 364543 );

	/* 2. COMPUTE SEED AT FIRST ELEMENT */
	for (i=0; i < hit_tileDimBegin( tileV, 0 ); i++) drand48();

	/* 3. INIT VALUES */
	hit_tileForDimDomain( tileV, 0, i )
		hit_tileElemAt( tileV, 1, i ) = drand48();

#ifdef WRITE_DATA
	/* 4. WRITE V ON FILE FOR CHECKING */
	hit_tileTextFileWrite( &tileV, "V.orig.dtxt", HIT_FILE_ARRAY, HIT_FILE_DOUBLE, 14, 12 );
#endif
}


/* C. FUNCTION TO COMPUTE SEQUENTIAL PIVOTING */
int sequentialPivoting( double pivot, HitTile_double data ) {

	int lo = 0;
	int hi = hit_tileCard( data )-1;

	double save = hit_tileElemAt1( data, lo );

	while( lo < hi ) { 
		/* SEARCH BACKWARD FOR LESSER THAN PIVOT */
		while ( hit_tileElemAt1( data, hi ) >= pivot && ( lo < hi )) { hi--; }
		if ( lo != hi ) 
		{
			hit_tileElemAt1( data, lo ) = hit_tileElemAt1( data, hi );
			lo++;
		}

		/* SEARCH FORWARD FOR GREATER THAN PIVOT */
		while ( hit_tileElemAt1( data, lo ) <= pivot && lo < hi ) { lo++; } 
		if (lo != hi)
		{
			hit_tileElemAt1( data, hi ) = hit_tileElemAt1( data, lo );
			hi--;
		}
	} 
	hit_tileElemAt1( data, lo ) = save;

	/* RETURN VALUE */
	if ( save <= pivot ) return lo + 1;
	else return lo;
}


/* D. COMPLETE SEQUENTIAL QUICKSORT */
int sequentialQuickSort( HitTile_double data, int begin, int end ) {

	if ( begin >= end ) return 0;

	int lo = begin;
	int hi = end;

	double pivot = hit_tileElemAt1( data, begin );

	while( lo < hi ) { 
		/* SEARCH BACKWARD FOR LESSER THAN PIVOT */
		while ( hit_tileElemAt1( data, hi ) >= pivot && ( lo < hi )) { hi--; }
		if ( lo != hi ) 
		{
			hit_tileElemAt1( data, lo ) = hit_tileElemAt1( data, hi );
			lo++;
		}

		/* SEARCH FORWARD FOR GREATER THAN PIVOT */
		while ( hit_tileElemAt1( data, lo ) <= pivot && lo < hi ) { lo++; } 
		if (lo != hi)
		{
			hit_tileElemAt1( data, hi ) = hit_tileElemAt1( data, lo );
			hi--;
		}
	} 
	hit_tileElemAt1( data, lo ) = pivot;

	/* RECURSION */
	sequentialQuickSort( data, begin, lo );
	sequentialQuickSort( data, lo+1, end );

	/* RETURN VALUE */
	return lo;
}

/* E. PARALLEL QUICKSORT ALGORITHM */
void parallelQuickSort( HitTile_double *data, HitLayout whole ) {
	HitTile_double left, right;
	int leftCard, totalLeftCard, totalRightCard;

	/* 1. STOP RECURSION */
	/* 1.a. TRIVIAL CASE: NON ACTIVE IN THE LAYOUT */
	if ( ! hit_layImActive( whole ) ) return;
	/* 1.b. TRIVIAL CASE: ONE DATA ELEMENT IF THE WHOLE LAYOUT */
	if ( hit_shapeCard( hit_layFullShape( whole) ) <= 1 ) return;
	/* 1.c. TRIVIAL SEQUENTIAL CASES FOR ONLY ONE ACTIVE PROCESS */
	if ( hit_layNumActives( whole ) == 1 ) {
		if ( hit_tileCard( *data ) <= 1 ) return;
		if ( hit_tileCard( *data ) == 2 ) {
			if ( hit_tileGet( *data, 1, 0 ) > hit_tileGet( *data, 1, 1 ) ) 
				swap( hit_tileElemAt( *data, 1, 0 ), hit_tileElemAt( *data, 1, 1 ) );
			return;
		}
	}

	/* OPTIMIZATION: FOR ONLY ONE ACTIVE PROCESS, USE A COMPLETE SEQUENTIAL QUICKSORT */
	if ( hit_layNumActives( whole ) == 1 ) {
		hit_clockContinue( seqCompClock );
#ifdef USE_QSORT3
		// SLOWER ALTERNATIVE USING qsort(3)
		qsort( data->data, (size_t)hit_tileCard(*data), sizeof(double), comparisonOp );
#else   // DIRECT IMPLEMENTATION USING Hitmap TILES
		sequentialQuickSort( *data, 0, hit_tileCard(*data)-1 );
#endif
		hit_clockStop( seqCompClock );
		return;
	}

	/* 2. CHOOSE PIVOT */
	/* 2.a. LOCAL PIVOT */
	double pivot = hit_tileGet( *data, 1, 0 );
	/* 2.b. MORE THAN ONE ACTIVE PROCESS: PIVOT IS THE MEAN OF LOCAL PIVOTS IN ALL ACTIVE PROCs. */
	if ( hit_layNumActives( whole ) > 1 ) {
		HitTile_double pivotTile;
	   	hit_tileSingle( &pivotTile, pivot, double );
		hit_comDoOnce( hit_comReduce( whole, HIT_RANKS_NULL, 
									&HIT_TILE_NULL, &pivotTile, HIT_DOUBLE, HIT_OP_SUM_DOUBLE ) );
		pivot = pivot / hit_layNumActives( whole );
	}

	/* 3. SEQUENTIAL PIVOTING OF LOCAL DATA: RETURNS THE NUMBER OF ELEMENTS SMALLER THAN pivot */
	hit_clockContinue( seqCompClock );
	leftCard = sequentialPivoting( pivot, *data );
	hit_clockStop( seqCompClock );

	/* 4. SELECT LOCAL SUBTILEs */
	hit_tileSelect( &left, data, hit_shape( 1, hit_sigStd( leftCard ) ) );
	hit_tileSelect( &right, data, hit_shape( 1, hit_sig( leftCard, hit_tileCard( *data )-1, 1 ) ) );

	/* 5. OPTIMIZATION FOR ONLY ONE ACTIVE PROCESS: SEQUENTIAL RECURSION USING THE ORIGINAL ARRAY */
	if ( hit_layNumActives( whole ) == 1 ) {
		parallelQuickSort( &left, whole );
		parallelQuickSort( &right, whole );
		return;
	}

	/* 6. ALL-REDUCE THE CARDINALITY OF LOCAL LEFT PARTS */
	HitTile_double leftCardTile;
	hit_tileSingle( &leftCardTile, leftCard, int );
	HitTile_double totalLeftCardTile;
	hit_tileSingle( &totalLeftCardTile, totalLeftCard, int );
	hit_comDoOnce( hit_comReduce( whole, HIT_RANKS_NULL, 
									& leftCardTile, & totalLeftCardTile, HIT_INT, HIT_OP_SUM_INT ) );
	totalRightCard = hit_shapeCard( hit_layFullShape(whole) ) - totalLeftCard;

	/* 7. SPLIT WORK IN WEIGHTED GROUPS, ACCORDING TO TOTAL LEFT vs. RIGHT CARDINALITIES */
	double weights[2] = { totalLeftCard, totalRightCard };
	HitShape twoElements = hit_shape( 1, hit_sigStd( 2 ) );
	HitTopology activesTopo = hit_layActivesTopology(whole);
	HitLayout groups = hit_layout( plug_layIndependentLB, activesTopo, twoElements, weights );
	hit_topFree( activesTopo );

	/* 8. REDISTRIBUTE THE left,right PARTS TO THE PROCS. IN THE CORRESPONDING GROUP */
	/* 8.a. DECLARE THE DOMAIN OF VIRTUAL left AND right ARRAYS */
	HitShape leftGlobalShp = hit_shapeSubset( hit_layFullShape(whole), hit_shapeStd( 1, totalLeftCard ) );
	HitShape rightGlobalShp = hit_shapeSubset( hit_layFullShape(whole), hit_shape( 1, hit_sig( totalLeftCard, totalLeftCard+totalRightCard-1, 1 ) ) );

	/* 8.b. COMPUTE LAYOUTS FOR THE DATA REDISTRIBUTION */
	HitTopology gt0 = hit_layGroupTopo( groups, hit_lgr_elementGroup( groups, 0 ) );
	HitTopology gt1 = hit_layGroupTopo( groups, hit_lgr_elementGroup( groups, 1 ) );
	HitLayout leftLayout = hit_layout( plug_layBlocks, gt0, leftGlobalShp );
	HitLayout rightLayout = hit_layout( plug_layBlocks, gt1, rightGlobalShp );

	/* 8.c. ALLOCATE BUFFERS FOR LOCAL PARTS */
	HitTile_double newLeft, newRight;
	hit_tileDomainShapeAlloc( &newLeft, double, hit_layShape( leftLayout ) );
	hit_tileDomainShapeAlloc( &newRight, double, hit_layShape( rightLayout ) );

	/* 8.d. DATA REDISTRIBUTION */
	hit_patternDoOnce( hit_patternRedistributeCom( whole, &left, &newLeft, HIT_DOUBLE, 1 ) );   
	hit_patternDoOnce( hit_patternRedistributeCom( whole, &right, &newRight, HIT_DOUBLE, 1 ) );

	/* 9. FREE MEMORY SPACE OF ORIGINAL DATA BEFORE RECURSION */
	hit_tileFree( *data );

	/* 10. RECURSIVE CALLS */
	if ( hit_layHasElem( groups, 0 ) ) parallelQuickSort( &newLeft, leftLayout );
	if ( hit_layHasElem( groups, 1 ) ) parallelQuickSort( &newRight, rightLayout );

	/* 11.a. IF THE GROUP HAS BOTH, LEFT AND RIGHT PARTS, REDISTRIBUTE THEM IN ORDER */
	if ( hit_layHasElem( groups, 0 ) && hit_layHasElem( groups, 1 ) ) {
		hit_tileAlloc( data );
		hit_tileSelectArrayCoords( &left, data, hit_shapeIntersect( hit_tileShape( *data ), leftGlobalShp ) );
		hit_tileSelectArrayCoords( &right, data, hit_shapeIntersect( hit_tileShape( *data ), rightGlobalShp ) );

		hit_patternDoOnce( hit_patternRedistributeCom( whole, &newLeft, &left, HIT_DOUBLE, 0 ) );   
		hit_patternDoOnce( hit_patternRedistributeCom( whole, &newRight, &right, HIT_DOUBLE, 0 ) );
		hit_tileFree( newLeft );
		hit_tileFree( newRight );
	}
	/* 11.b. IF THE GROUP HAS ONLY LEFT, OR ONLY RIGHT PART, IT IS THE OUTPUT */
	else if ( hit_layHasElem( groups, 0 ) ) *data = newLeft;
	else if ( hit_layHasElem( groups, 1 ) ) *data = newRight;
	//else *data = *(HitTile_double *)&HIT_TILE_NULL;

	/* 12. FREE RESOURCES */
	hit_layFree( groups );
	hit_layFree( leftLayout );
	hit_layFree( rightLayout );
	hit_topFree( gt0 );
	hit_topFree( gt1 );
}
