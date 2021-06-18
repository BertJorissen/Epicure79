/**
 * Physical and virtual topologies of processes.
 *
 * Plug-in system to introduce virtual topology generation policies, that defines
 * groups of processes and neighbor relations among them.
 * Support for virtual multidimensional grid topologies.
 * 
 * @file hit_topology.h
 * @ingroup Com
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

#ifndef _HitTopology_
#define _HitTopology_

#include <stdio.h>

#include "hit_shape.h"
#include "hit_error.h"
#include "hit_mpi.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* 1. PHYSICAL TOPOLOGY */
/* CURRENTLY, ONLY HOMOGENEOUS SYSTEM SUPPORT */

/* 1.1. PHYSICAL TOPOLOGY DATA STRUCTURE */
// @cond INTERNAL
/**
 * ADT for physical topology
 */
typedef struct HitPTopology {
	int numUses;		/**< Number of layouts, or programmer variables, using this topology */
	int numProcs;		/**< Number of processors. */
	int selfRank;		/**< Processor rank. */
	MPI_Comm comm;		/**< Low level communication object. MPI implementation */
	struct HitPTopology *next;	/**< Pointer to next element in the contexts stack */
	// @arturo 2015/01/07 New fields (Redistribute functionalities)
	struct HitPTopology *global;	/**< Pointer to root element in the contexts stack */
	MPI_Group	antiCommGroup;	/**< The complementary set of active/nonActive processors */
} HitPTopology;


/* 1.2. Hit PHYSICAL TOPOLOGY GLOBAL VARIABLE */
/**
 * Global variable for the actual physical topology
 */
extern HitPTopology			*HIT_TOPOLOGY_INFO;

/* 1.3. Hit PHYSICAL TOPOLOGY NULL CONSTANTS */
/**
 * Null value of the physical topology and stack
 */
extern HitPTopology	HIT_PTOPOLOGY_NULL;

// @arturo 2015/01/03 Add constant for new fields
/** Null static value of the physical topology. */
#define	HIT_PTOPOLOGY_NULL_STATIC	{ 0,0,0, MPI_COMM_NULL, NULL, NULL, MPI_GROUP_EMPTY }

/* 1.4. Hit PHYSICAL TOPOLOGY COMPARATORS */
/**
 * Macro to compare two physical topologies,
 * the comparison is made using the low-level communicator.
 */
#define	hit_ptopoCmp(a,b)	((a)->comm == (b)->comm)

/**
 * Macro to check if the physical topology is null.
 */
#define	hit_ptopoIsNull(a)	((a)->comm == MPI_COMM_NULL)

/* 1.5. SPLIT PHYSICAL TOPOLOGY */
/**
 * Split a physical topology object in groups of subtopologies.
 */
HitPTopology *hit_ptopSplit( HitPTopology *in, int group );

/* 1.6. DUPLICATE A PTOPOLOGY OBJECT */
/**
 * Duplicate a phisical topology object
 */
HitPTopology *hit_ptopDup( HitPTopology *in );

/* 1.7. FREE A PTOPOLOGY OBJECT */
/**
 * Free a phisical topology object
 */
void hit_ptopFree( HitPTopology **in );
// @endcond


/* 2. RANKS IN MULTIDIMENSIONAL GRID */
/* 2.1. STRUCTURE */
/**
 * ADT for ranks in a multidimensional domain.
 */
typedef struct HitRanks {
	int		rank[HIT_MAXDIMS];	/**< Array with the processor coordinates. */
} HitRanks;

/* 2.2. HitRank NULL VALUE */
/**
 * Value for null rank
 */
extern int	HIT_RANK_NULL;
/** Value for null static rank */
#define	HIT_RANK_NULL_STATIC	-9999999

/* 2.3 HitRanks NULL VALUE*/
/**
 * HitRanks null value
 *
 * Is a HitRanks structure with all the ranks set to HIT_RANK_NULL
 */
extern HitRanks	HIT_RANKS_NULL;
/** HitRanks null static value */
#define	HIT_RANKS_NULL_STATIC		{ { HIT_RANK_NULL_STATIC, HIT_RANK_NULL_STATIC, HIT_RANK_NULL_STATIC, HIT_RANK_NULL_STATIC } }

/**
 * Rank of a group leader
 */
extern HitRanks HIT_RANKS_LEADER;


/* 2.4. HitRanks CONSTRUCTOR FUNCTIONS */
/**
 * HitRanks constructor function.
 * The first argument is the dimension number, the following arguments are the value of the ranks.
 * @param numDims Dimension number for the ranks
 * @return A HitRank object.
 */
#define	hit_ranks( numDims, ... )	hit_ranks##numDims( __VA_ARGS__ )

/**
 * HitRanks constructor function for one dimension.
 * @param r1 First dimension cardinality.
 * @return A HitRank object.
 */
static inline HitRanks hit_ranks1( int r1 ) {
	 HitRanks res = { { r1, HIT_RANK_NULL_STATIC, HIT_RANK_NULL_STATIC, HIT_RANK_NULL_STATIC } };
	 return res;
}
/**
 * HitRanks constructor function for two dimensions.
 * @param r1 First dimension cardinality.
 * @param r2 Second dimension cardinality.
 * @return A HitRank object.
 */
static inline HitRanks hit_ranks2( int r1, int r2 ) {
	HitRanks res = { { r1, r2, HIT_RANK_NULL_STATIC, HIT_RANK_NULL_STATIC } };
	 return res;
}
/**
 * HitRanks constructor function for three dimensions.
 * @param r1 First dimension cardinality.
 * @param r2 Second dimension cardinality.
 * @param r3 Third dimension cardinality.
 * @return A HitRank object.
 */
static inline HitRanks hit_ranks3( int r1, int r2, int r3 ) {
	HitRanks res = { { r1, r2, r3, HIT_RANK_NULL_STATIC } };
	 return res;
}
/**
 * HitRanks constructor function for four dimensions.
 * @param r1 First dimension cardinality.
 * @param r2 Second dimension cardinality.
 * @param r3 Third dimension cardinality.
 * @param r4 Fourth dimension cardinality.
 * @return A HitRank object.
 */
static inline HitRanks hit_ranks4( int r1, int r2, int r3, int r4 ) {
	HitRanks res = { { r1, r2, r3, r4 } };
	 return res;
}

/* 	2.4. HitRanks COMPARE */
/**
 * hit_ranks: Compare two HitRanks
 * @param a A HitRanks
 * @param b A HitRanks
 */
#define	hit_ranksCmp(a,b)	((a).rank[0] == (b).rank[0] && (a).rank[1] == (b).rank[1] && (a).rank[2] == (b).rank[2] && (a).rank[3] == (b).rank[3])

/* 	2.5. HitRanks DIMENSION COORDINATE */
/**
 * hit_ranksDim: Returns the coordinate of a HitRanks object in a given dimension
 * @param ranks HitRanks object
 * @param dim	The number of a dimension
 */
#define hit_ranksDim( ranks, dim )	(ranks.rank[dim])





/* 3. Hit ARRAY VIRTUAL TOPOLOGY */
/* CURRENTLY: ONLY MULTI-DIMENSIONAL GRID TOPOLOGIES */

/* 3.1. TOPOLOGY DATA STRUCTURE */
/**
 * ADT for topologies
 * A topology is a representation of a set of processor ordered in a number of dimensions.
 * This structure also define the position of the actual processor in the topology.
 */
typedef struct HitTopology {
	int			type;					/**< Topology type. */
	int			numDims;				/**< Number of dimensions. */
	int			card[HIT_MAXDIMS];		/**< Cardinalities in each dimension. */
	// @arturo 2015/01/22
	//int			periods[HIT_MAXDIMS];	/**< Periodic dimension flag. (TODO: DEPRECATED) */
	HitRanks	self;					/**< Ranks for the current processor. */
	// @arturo 2015/01/22
	//int			linearRank;				/**< Linear rank for the current processor. */
	int			active;					/**< Active flag. */
	HitPTopology *pTopology;			/**< Pointer to the physical topology object. */
} HitTopology;


// Hit TOPOLOGY TYPES
/**
 * Plain topology type constant.
 */
#define HIT_TOPOLOGY_PLAIN			1

/**
 * Square topology type constant.
 */
#define HIT_TOPOLOGY_SQUARE			2

/**
 * Array 2D complete topology type constant.
 */
#define HIT_TOPOLOGY_ARRAY2DCOMP	3

/**
 * Array dims topology type constant.
 */
#define HIT_TOPOLOGY_ARRAYDIMS		4

/**
 * Array dimensional projection topology type constant.
 */
#define HIT_TOPOLOGY_ARRAYDIMPROJECTION		5

/**
 * Array dims complete topology type constant.
 */
#define HIT_TOPOLOGY_ARRAYDIMSCOMP		6



/**
 * Plain topology with restrictions: Number of active procs is a power of 2
 */
#define HIT_TOPOLOGY_PLAIN_POWER2	10


/* 3.2. Hit TOPOLOGY NULL VALUE */
/**
 * HitTopology null value.
 */
extern HitTopology	HIT_TOPOLOGY_NULL;

/** HitTopology null static value. */
// @arturo 2015/01/22
//#define HIT_TOPOLOGY_NULL_STATIC	{0, 0, { 0, 0, 0, 0 }, { { -1, -1, -1, -1} }, 0, NULL }
// @arturo 2019/09/12
#define HIT_TOPOLOGY_NULL_STATIC	{0, 0, { 0, 0, 0, 0 }, HIT_RANKS_NULL_STATIC, 0, NULL }

/* 3.3. Hit TOPOLOGY GENERATION */
/**
 * Topology constructor
 * Hitmap provides this function to create some of the most used topologies.
 * This predefined topologies are: plain, square, array2D, array2DComplete, array3D and array4D.
 * @param name The name of the topology.
 * @return A new HitTopology
 */
#define hit_topology( name, ... )	hit_topology_##name(HIT_TOPOLOGY_INFO, ##__VA_ARGS__ )

/* 3.4. Hit PREDEFINED TOPOLOGY FUNCTIONS */

/* 3.4.1 Hit PLAIN TOPOLOGY */
/**
 * Plain topology generator
 *
 * All the processor are arranged in one dimension.
 * All of them are active.
 */
HitTopology hit_topology_plug_topPlain( HitPTopology *topo );

/* 3.4.2 Hit PLAIN TOPOLOGY */
/**
 * Plain topology generator, which restricts the number of active processors to a power of 2
 *
 * All the processor are arranged in one dimension.
 * Only a power of 2 number of processors are active
 */
HitTopology hit_topology_plug_topPlainPower2( HitPTopology *topo );

/* 3.4.3 Hit SQUARE TOPOLOGY */
/**
 * Square topology generator
 *
 * The processors are arranged in a perfect square two-dimensional topology
 * Uneven processors are inactive
 */
HitTopology hit_topology_plug_topSquare( HitPTopology *topo );

/* 3.4.4 Hit COMPLETE 2D ARRAY TOPOLOGY */
/**
 * Complete 2D Array topology generator
 *
 * First dimension bigger.
 * All processes are arranged in a two-dimensional topology 
 * with the two cardinalities being the highest integer divisors
 * of the number of processes 
 * All of the processes are active.
 * NOTE: This means a single row if the number of processes is prime
 */
HitTopology hit_topology_plug_topArray2DComplete( HitPTopology *topo );

/* 3.4.5 Hit X-D ARRAY TOPOLOGY */
/**
 * X-D Array topology generator
 *
 * The processors are arranged in a X-dimensional topology
 * Some processor may be inactive.
 * If the input is power of two the result cardinalities keep this property
 * First dimensions are bigger
 *
 * This function is meant to be used through the macros below.
 */
HitTopology hit_topology_plug_topArrayDims( HitPTopology *topo , int dims );

/**
 * 1D Array topology generator
 *
 * The processors are arranged in a 1-dimensional topology
 * Some processor may be inactive.
 * If the input is power of two the result cardinality keep this property
 */
#define hit_topology_plug_topArray1D(topo) hit_topology_plug_topArrayDims(topo, 1)

/**
 * 2D Array topology generator
 *
 * The processors are arranged in a 2-dimensional topology
 * Some processor may be inactive.
 * If the input is power of two the result cardinalities keep this property
 * First dimensions are bigger
 */
#define hit_topology_plug_topArray2D(topo) hit_topology_plug_topArrayDims(topo, 2)

/**
 * 3D Array topology generator
 *
 * The processors are arranged in a 3-dimensional topology
 * Some processor may be inactive.
 * If the input is power of two the result cardinalities keep this property
 * First dimensions are bigger
 */
#define hit_topology_plug_topArray3D(topo) hit_topology_plug_topArrayDims(topo, 3);

/**
 * 4D Array topology generator
 *
 * The processors are arranged in a 4-dimensional topology
 * Some processor may be inactive.
 * If the input is power of two the result cardinalities keep this property
 * First dimensions are bigger
 */
#define hit_topology_plug_topArray4D(topo) hit_topology_plug_topArrayDims(topo, 4);

/* 3.4.6 Hit DIMENSIONAL PROJECTION TOPOLOGY */
/**
 * DIMENSIONAL PROJECTOR TOPOLOGY
 *
 * The processors are arranged in a sequence in a selected dimension
 * Dimensions higher than the selected one do not exist.
 * Dimensions lower than the selected one have cardinality one, and all the 
 * processes share the same index: 0.
 *
 * This topology allows to easily map a selected dimension with the
 * desired layout function, assigning the whole signature of the
 * rest of dimensions to all the processors. The distribution is
 * applied only to the dimension of the topology.
 *
 */
HitTopology hit_topology_plug_topArrayDimProjection( HitPTopology *topo , int dim );

/* 3.5. Hit ACCESS TO TOPOLOGY INFORMATION */
/* 3.5.1. ACTIVE */
/**
 * Active state of the processor in the topology
 */
#define	hit_topSelfActive(topo)	(topo.active)

/* 3.5.2. NUM DIMENSIONS */
/**
 * Number of dimensions of the topology
 */
#define	hit_topDims(topo)	(topo.numDims)

/* 3.5.3. CARDINALITY OF A DIMENSION */
/**
 * Cardinality of a topology dimension
 */
#define	hit_topDimCard(topo, i)	(topo.card[i])

/* 3.5.4. TOPOLOGY CARDINALITY */
/**
 * Topology cardinality: number of active processors in the topology.
 */
int hit_topCard(HitTopology topo);

/* 3.5.5. SELF RANK */
/**
 * Self rank of the current processor in the topology
 */
#define	hit_topRanks(topo)	(topo.self)

/* 3.5.6. SELF COORDITATES */
/**
 * Self topology coordinate in a given dimension
 */
#define	hit_topDimRank(topo, i)	(topo.self.rank[i])

/* 3.5.7. SELF RANK IN LINEAR FORM */
/**
 * Self topology linear rank
 */
// @arturo 2015/01/22
//#define	hit_topSelfRankInternal(topo)	(topo.linearRank)
//#define	hit_topSelfRankInternal(topo)	( (topo).pTopology->selfRank )
// @arturo 2019/09/12
#define	hit_topSelfRankInternal(topo)	( (!topo.active) ? HIT_RANK_NULL_STATIC : (topo).pTopology->selfRank )

/* 3.6 FREE TOPOLOGY METHOD */
/**
 * Free the resources used by a topology
 */
void hit_topFree( HitTopology topo );

/* 3.7. COMPUTE NEIGHBORS */
/**
 * Return the neighbor index in the selected dimension with a shift.
 */
int	hit_topDimNeighbor(HitTopology topo, int dim, int shift );

/* 3.8. CONVERSIONS OF RANKS */
/* 3.8.1 MULTIDIMENSIONAL RANKS TO LINEAL */
/**
 * Converts a multidimensional HitRanks in a linear value
 */
int hit_topRankInternal(HitTopology topo, HitRanks ranks );

/* 3.8.2 LINEAL TO MULTIDIMENSIONAL RANKS */
/**
 * Converts a linear value in a HitRanks
 */
HitRanks hit_topRanksInternal( HitTopology topo, int linealRank );

/* 3.9 LOOPs and IFs SHORTCUTS */
/* 3.9.1. LOOPS ACROSS VIRTUAL PROCESSORS */
/**
 * Perform a loop across virtual processors of the given dimension
 */
#define	hit_topDimForeach(topo, dim, index)	for( index=0; index<topo.card[dim]; index++)

/* 3.9.2 CHECK IF THE LOCAL PROCESSOR IS ACTIVATED
 * USED TO DO SOMETHING ONLY FOR ACTIVATED PROCESSORS IN THE TOPOLOGY */
/**
 * Check if the processor is active in the topology
 */
#define hit_topImActive( topo ) ((topo).active)

/* 3.9.3 CHECK IF THE LOCAL PROCESSOR IS THE LEADER (rank 0) */
/**
 * Check if the processor is the leader (rank 0) in the topology
 */
// @arturo 2015/01/22
//#define hit_topImLeader( topo ) ((topo).linearRank==0)
#define hit_topImLeader( topo ) ((topo).pTopology->selfRank == 0 )


/**
 * Issue a warning in case of inactive processors in a topology
 */
#define hit_topInactiveWarning( topo )	\
	if ( hit_topImLeader(topo) ) { \
		int numInactive = HIT_TOPOLOGY_INFO.numProcs - hit_topCard( topo );	\
		if ( numInactive > 0 ) {	\
			char message[256];	\
			sprintf( message, "%d Processors inactive in topology \"%s\"", numInactive, #topo);	\
			hit_warning( message, __FILE__, __LINE__ );	\
		}	\
	}

#ifdef __cplusplus
}
#endif

/* END OF HEADER FILE _HitTopology_ */
#endif
