/**
 * Physical and virtual topologies of processes.
 * Information about the physical topology of the system and virtual
 * topologies of processes created using plug-ins modules.
 * Support for virtual multidimensional array topologies.
 * 
 * 
 * @file hit_topology.c
 * @version 1.4
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @date Mar 2019
 *
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
 * Copyright (c) 2007-2021, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <hit_topology.h>
#include <hit_com.h>

/* INTEGER POWER OF 2 MACRO*/
#define pow2(x) ( 1<<(x) )


/* EXTERN VARIABLES */ 
/* 1.2 PHYSICAL TOPOLOGY: TO BE INITIALIZED BY THE SPECIFIC PARALLEL INTERFACE FUNCTION */
HitPTopology		*HIT_TOPOLOGY_INFO = NULL;

/* 1.3 NULL TOPOLOGY */
HitPTopology HIT_PTOPOLOGY_NULL = HIT_PTOPOLOGY_NULL_STATIC;
HitTopology	HIT_TOPOLOGY_NULL = HIT_TOPOLOGY_NULL_STATIC;

/* 2.2 NULL VRANKS */
int			HIT_RANK_NULL = HIT_RANK_NULL_STATIC;
HitRanks	HIT_RANKS_NULL = HIT_RANKS_NULL_STATIC;

/* 2.3. CONSTANT FOR THE LEADER */
HitRanks HIT_RANK_LEADER = {{0,0,0,0}};

/* 3.1. MPI: DUPLICATE COMMUNICATOR FOR A DUPLICATED TOPOLOGY */
HitPTopology *hit_ptopDup( HitPTopology *in ) {
	if ( in != NULL ) in->numUses ++;
	return in;
}

/* 3.2. MPI: SPLIT COMMUNICATOR FOR TOPOLOGIES WITH GROUPS, OR NON-ACTIVE PROCESSORS */
HitPTopology *hit_ptopSplit( HitPTopology *in, int group ) {
	if ( in == NULL ) return in;

	HitPTopology *ptopo;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc( ptopo, sizeof( HitPTopology ),HitPTopology*);
	hit_malloc( ptopo, HitPTopology, 1 );
	*ptopo = HIT_PTOPOLOGY_NULL;


	int result = MPI_Comm_split( in->comm, group, in->selfRank, &(ptopo->comm) );
	hit_mpiTestError( result, "Error using MPI_Comm_split" );
	MPI_Comm_rank( ptopo->comm, &(ptopo->selfRank) );
	MPI_Comm_size( ptopo->comm, &(ptopo->numProcs) );

	// @arturo 2015/01/03
	ptopo->next = in;
	ptopo->global = in->global;
	// @arturo 2015/01/07
	MPI_Group globalGroup, localGroup;
	MPI_Comm_group( in->global->comm, &globalGroup );
	MPI_Comm_group( ptopo->comm, &localGroup ), 
	MPI_Group_difference( globalGroup, localGroup, &(ptopo->antiCommGroup) );
	
	ptopo->numUses = 1;
	return ptopo;
}

/* 3.3. MPI: FREE PTOPOLOGY */
void hit_ptopFree( HitPTopology **in ) {
	if ( (*in) == NULL ) 
		hit_errInternal( __FUNCTION__, "Trying to free a null pointer", "", __FILE__, __LINE__);

#ifdef DEBUG
fprintf(stderr, "hit_ptopFree PTOP %d, %d, %d\n", (*in)->numUses, (*in)->selfRank, (*in)->numProcs );
#endif

	if ( (*in)->numUses < 1 ) 
		hit_errInternal( __FUNCTION__, "Trying to free a null physical topology", "", __FILE__, __LINE__);

	((*in)->numUses)--;
	if ( (*in)->numUses < 1 ) {
		MPI_Comm_free( &((*in)->comm) );
		hit_free( *in );
		*in = NULL;
	}
}

/* 4. FREE TOPOLOGY */
void hit_topFree( HitTopology topo ) {
	if ( topo.pTopology == NULL )
		hit_errInternal( __FUNCTION__, "Trying to free a null topology", "", __FILE__, __LINE__);

	hit_ptopFree( &(topo.pTopology) );
}


/* 5. Hit PREDEFINED TOPOLOGY FUNCTIONS */
/* 5.1 Hit PLAIN TOPOLOGY */
HitTopology hit_topology_plug_topPlain( HitPTopology *ptopo ) {
	HitTopology res = HIT_TOPOLOGY_NULL;

	res.type = HIT_TOPOLOGY_PLAIN;
	res.numDims = 1;
	res.card[0] = ptopo->numProcs;
	// @arturo 2015/01/22
	//res.linearRank = ptopo->selfRank;
	// @arturo 2019/09/12 Non-active processes have ranks NULL
	res.active = ( ptopo->selfRank < ptopo->numProcs ) ? 1 : 0;
	if ( res.active )
		res.self.rank[0] = ptopo->selfRank;
	else
		res.self.rank[0] = HIT_RANK_NULL_STATIC;

	res.pTopology = hit_ptopDup( ptopo );

	return res;
}

/* 5.2. Hit PLAIN TOPOLOGY WHICH RESTRICTS NUMBER OF ACTIVE PROCESSORS TO A POWER OF 2 */
HitTopology hit_topology_plug_topPlainPower2( HitPTopology *ptopo ) {
	HitTopology res = HIT_TOPOLOGY_NULL;

	res.type = HIT_TOPOLOGY_PLAIN_POWER2;
	res.numDims = 1;

	res.card[0] = (int)( pow2( (int)log2( ptopo->numProcs ) ) );
	// @arturo 2015/01/22
	//res.linearRank = ptopo->selfRank;
	// @arturo 2019/09/12 Non-active processes have ranks NULL
	res.active = ( ptopo->selfRank < res.card[0] ) ? 1 : 0;
	if ( res.active ) 
		res.self.rank[0] = ptopo->selfRank;
	else
		res.self.rank[0] = HIT_RANK_NULL_STATIC;

	/* IF NOT ALL PROCESSORS ARE ACTIVE, SPLIT PHISICAL TOPOLOGY */
	if ( res.card[0] == ptopo->numProcs ) res.pTopology = hit_ptopDup( ptopo );
	else res.pTopology = hit_ptopSplit( ptopo, res.active );

	return res;
}

/* 5.3. Hit SQUARE TOPOLOGY */
HitTopology hit_topology_plug_topSquare( HitPTopology *ptopo ) {
	HitTopology res = HIT_TOPOLOGY_NULL;

	res.type = HIT_TOPOLOGY_SQUARE;
	
	/* SQUARE ROOT OF NUM PROCS. */
	double dsize = sqrt( (double)(ptopo->numProcs) );
	int size = (int)dsize;

	/* DIMENSIONS AND SIZES */
	res.numDims = 2;
	res.card[0] = size;
	res.card[1] = size;

	/* GET FIRST COORDINATE */
	// @arturo 2015/01/22
	//res.linearRank = ptopo->selfRank;
	// @arturo 2019/09/12 Non-active processes have ranks NULL
	res.active = ( ptopo->selfRank < size*size ) ? 1 : 0;
	if ( res.active ) {
		res.self.rank[0] = ptopo->selfRank / size;
		res.self.rank[1] = ptopo->selfRank % size;
	}
	else {
		res.self.rank[0] = HIT_RANK_NULL_STATIC;
		res.self.rank[1] = HIT_RANK_NULL_STATIC;
	}

	/* IF NOT ALL PROCESSORS ARE ACTIVE, SPLIT PHISICAL TOPOLOGY */
	if ( dsize == (double)size ) res.pTopology = hit_ptopDup( ptopo );
	else res.pTopology = hit_ptopSplit( ptopo, res.active );

	/* RETURN RESULT */
	return res;
}


/* 5.4. Hit COMPLETE 2D ARRAY TOPOLOGY */
HitTopology hit_topology_plug_topArray2DComplete( HitPTopology *ptopo ) {
	HitTopology res = HIT_TOPOLOGY_NULL;
	res.type = HIT_TOPOLOGY_ARRAY2DCOMP;
	
	int low, high;
	int product;

	/* 1. START AS NEAR AS POSSIBLE TO THE SQUARE ROOT */
	low = high = (int)sqrt( (double)(ptopo->numProcs) );
	product = low*high;

	/* 2. MOVE UP/DOWN TO SEARCH THE NEAREST FACTORS */
	while ( product != ptopo->numProcs ) {
		if (product < ptopo->numProcs) {
			high++;
			product = product + low;
		}
		else {
			low--;
			if (low==1) { high=ptopo->numProcs; product=ptopo->numProcs; }
			else product = product - high;
		}
	}

	/* 3. BUILD RESULT */
	res.numDims = 2;
	res.card[0] = high;
	res.card[1] = low;
	// @arturo 2015/01/22
	//res.linearRank = ptopo->selfRank;
	//res.active = ( res.self.rank[0] < res.card[0] ) ? 1 : 0;
	// @arturo 2019/09/12 Non-active processes have ranks NULL
	res.active = ( ptopo->selfRank < high*low ) ? 1 : 0;
	if ( res.active ) {
		res.self.rank[0] = ptopo->selfRank / low;
		res.self.rank[1] = ptopo->selfRank % low;
	}
	else {
		res.self.rank[0] = HIT_RANK_NULL_STATIC;
		res.self.rank[1] = HIT_RANK_NULL_STATIC;
	}

	res.pTopology = hit_ptopDup( ptopo );

	/* 4. RETURN RESULT */
	return res;
}


/* 5.5. Hit NAS ARRAY TOPOLOGY */
HitTopology hit_topology_plug_topArrayDims( HitPTopology *ptopo , int dims ) {
	
	HitTopology res = HIT_TOPOLOGY_NULL;
	res.type = HIT_TOPOLOGY_ARRAYDIMS;
	
	int cards[dims];
	int oldcards[dims];
	int logs[dims];
	int logn;
	int i,j;
	int go_on;
	int product;


	/* LOG2 of number of processors */
	logn = (int)floor(log((double)(ptopo->numProcs))/log(2.0));

	/* Assign the log2 to the dimensions */
	for(i=0;i<dims;i++){
		logs[i] = logn / dims + ((logn % dims > i) ? 1 : 0 );
		cards[i] = pow2(logs[i]);
	}

	/* Assign the rest of the processors */
	do{
		/* Set old cards */
		for(i=0;i<dims;i++) oldcards[i] = cards[i];

		/* Increase one dimension if possible */
		for(i=0;i<dims;i++){
			product = 1;
			for(j=0;j<dims;j++){
				product *= cards[j] + ( j == i ? 1 : 0 );
			}
			if( product <= ptopo->numProcs ) cards[i]++;
		}

		/* Check out condition */
		go_on = 0;
		for(i=0;i<dims;i++){
			if(cards[i] != oldcards[i]) go_on = 1;
		}

	} while(go_on);

	/* 3. BUILD RESULT */
	res.numDims = dims;
	for(i=0;i<dims;i++) res.card[i] = cards[i];

	/* Check if active */
	product = 1;
	for(i=0;i<dims;i++){
		product *= cards[i];
	}
	res.active = ( ptopo->selfRank < product ) ? 1 : 0;

	// @arturo 2015/01/22
	//res.linearRank = ptopo->selfRank;
	// @arturo 2019/09/12 Non-active processes have ranks NULL
	if ( res.active ) {
		res.self = hit_topRanksInternal(res,ptopo->selfRank);
	}
	else {
		for(i=0;i<dims;i++) res.self.rank[i] = HIT_RANK_NULL_STATIC;
	}

	/* IF NOT ALL PROCESSORS ARE ACTIVE, SPLIT PHISICAL TOPOLOGY */
	if ( product == ptopo->numProcs ) res.pTopology = hit_ptopDup( ptopo );
	else res.pTopology = hit_ptopSplit( ptopo, res.active );

	/* 4. RETURN RESULT */
	return res;
}

/* 5.6. Hit DIMENSIONAL PROJECTION ARRAY TOPOLOGY */
HitTopology hit_topology_plug_topArrayDimProjection( HitPTopology *ptopo, int dim ) {
	HitTopology res = HIT_TOPOLOGY_NULL;
	res.type = HIT_TOPOLOGY_ARRAYDIMPROJECTION;
	
	/* 1. NUMBER OF DIMENSIONS */
	res.numDims = dim+1;
//	res.numDims = HIT_MAXDIMS;

	/* 2. CARDINALITY AND INDEX OF DIMENSIONS BELOW */
	int ind;
	// @arturo: NON-PROJECTED DIMENSIONS REPORT CARDINALITY 1 AND RANK 0, EVEN THOSE ABOVE 
	// THE PROJECTED DIMENSION
	//for ( ind=0; ind<dim; ind++ ) {
	for ( ind=0; ind<HIT_MAXDIMS; ind++ ) {
		res.card[ind] = 1;
		res.self.rank[ind] = 0;
	}

	/* 3. CARDINALITY AND INDEX ON THE SELECTED DIMENSION */
	res.card[dim] = ptopo->numProcs;
	res.self.rank[dim] = ptopo->selfRank;

	/* 4. ALL ARE ACTIVE */
	res.active = 1;
	res.pTopology = hit_ptopDup( ptopo );

	/* 5. RETURN RESULT */
	return res;
}


/* 6.1. TOPOLOGY CARDINALITY */
int hit_topCard(HitTopology topo){

	// @arturo Feb 2013: BUG Corrected, For NULL TOPOLOGY it should return 0
	if ( hit_topDims(topo) < 1 ) return 0;

	int card = 1;
	int i;
	for(i=0;i<hit_topDims(topo);i++){
		card *= hit_topDimCard(topo, i);
	}
	return card;
}


/* 6.2. COMPUTE NEIGHBORS */
int hit_topDimNeighbor( HitTopology topo, int dim, int shift ) {
	/* 1. OUT OF DIMENSION */
	if ( dim < 0 || dim > topo.numDims-1 ) return HIT_RANK_NULL;

	/* 2. OUT OF RANGE FOR NON-PERIODIC */
	int newDimRank = topo.self.rank[dim] + shift;
	// @arturo 2015/01/22
	//if ( ! topo.periods[dim] && ( newDimRank < 0 || newDimRank > topo.card[dim]-1 ) )
	if ( newDimRank < 0 || newDimRank > topo.card[dim]-1 )
			return HIT_RANK_NULL;

	/* 3. ADJUST PERIODIC */
	// @arturo 2015/01/22
	/*
	if ( topo.periods[dim] && newDimRank < 0 ) newDimRank = newDimRank + topo.card[dim];
	else if ( topo.periods[dim] && newDimRank > topo.card[dim]-1 ) 
				newDimRank = newDimRank - topo.card[dim];
	*/

	/* 4. COMPUTE NEIGHBOUR RANK */
	int acumCard = 1;
	int newRank = 0;
	int i;
	for ( i=topo.numDims-1; i>=0; i--) {
		/* IF COMPUTING THE SELECTED DIMENSION: USE THE NEW NEIGHBOUR RANK */
		if ( i == dim ) newRank = newRank + newDimRank * acumCard;
		/* ELSE, THE ORIGINAL RANK */
		else newRank = newRank + topo.self.rank[i] * acumCard;

		/* UPDATE ACUMULATED CARDINALITY */
		acumCard = acumCard * topo.card[i];
	}

	/* 5. RETURN NEW RANK */	
	return newRank;
}


/* 6.3. MULTIDIMENSIONAL RANKS TO LINEAL */
int hit_topRankInternal( HitTopology topo, HitRanks ranks ) {

	if(hit_ranksCmp(ranks,HIT_RANKS_NULL)) return HIT_RANK_NULL;

	int linear = 0;
	int acumCard = 1;

	int dim;

	for(dim=topo.numDims-1; dim>=0; dim--) {
		if (ranks.rank[dim] == HIT_RANK_NULL) return HIT_RANK_NULL;
		linear = linear + acumCard * ranks.rank[dim];
		acumCard = acumCard * topo.card[dim];
	}

	// @arturo 2015/01/07 Skip inactive processors out of the geometry
	if ( linear < 0 || linear >= acumCard ) return HIT_RANK_NULL;

	return linear;
}


/* 6.4. LINEAL TO MULTIDIMENSIONAL RANKS */
HitRanks hit_topRanksInternal( HitTopology topo, int linealRank ){

	if (linealRank == HIT_RANK_NULL) return HIT_RANKS_NULL;
	// @arturo 2015/01/07 Skip inactive processors out of the geometry
	if (linealRank < 0) return HIT_RANKS_NULL;

	HitRanks res = HIT_RANKS_NULL;
	int acumCard;
	int dim;

	acumCard = 1;
	for(dim=topo.numDims-1; dim>=0; dim--) {
		res.rank[dim] = (linealRank / acumCard) % topo.card[dim];
		acumCard *= topo.card[dim];
	}

	// @arturo 2015/01/07 Skip inactive processors out of the geometry
	if ( linealRank >= acumCard ) return HIT_RANKS_NULL;

	return res;
}

