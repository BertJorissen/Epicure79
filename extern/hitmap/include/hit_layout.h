/**
 * Layout management funtionalities.
 * A layout maps the indexes of domain expressed as a shape
 * to the processes of a virtual topology.
 * 
 * @file hit_layout.h
 * @version 1.5
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Mar 2013
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

#ifndef _HitLayout_
#define _HitLayout_

#include <stdio.h>
#include <stdlib.h>

#include "hit_shape.h"
#include "hit_topology.h"
#include "hit_error.h"


#ifdef __cplusplus
	extern "C" {
#endif
/* 1. Hit SIGNATURE LAYOUT */

/* 1.1. TYPES FOR FUNCTION POINTERS TO LAYOUT DEFINITIONS */
/**
 * Function type to calculate the Signature of the current processor in one dimension
 * @param procId The processor ID
 * @param procsCard Processor cardinality
 * @param blocksCard Blocks cardinality
 * @param extraParam Optional extra parameter for some signatures
 * @param input Global Signature for the actual dimension
 * @param res Output pointer with the local Signature
 * @return Active status
 */
typedef int	(*HitLayoutSignatureFunction)(	int procId, 
											int procsCard, 
											int blocksCard, 
											float* extraParam,
											HitSig input, 
											HitSig *res );

/**
 * Function type to calculate the inverse of the Signature. Returns who has a given index.
 * @param procId The processor ID
 * @param procsCard Processor cardinality
 * @param blocksCard Blocks cardinality
 * @param input Original global Signature for the actual dimension
 * @param ind Index to locate
 * @return Rank of the process who has the index
 */
typedef int	(*HitLayoutSignatureInvFunction)(	int procId, 
											int procsCard, 
											int blocksCard, 
											float* extraParam,
											HitSig input, 
											int ind );

/** @name Constants to specify the direction of the transformation of ranks
 * @see hit_layToActiveRanks, hit_layToTopoRanks
 */
/** @{ */
/**
 * Flag to transform active ranks to topology ranks
 * @hideinitializer
 */ 
#define HIT_LAY_RANKS_ACTIVE_TO_TOPO	0
/**
 * Flag to transform topology ranks to active ranks
 * @hideinitializer
 */ 
#define HIT_LAY_RANKS_TOPO_TO_ACTIVE	1
/** @} */ 

/**
 * Function type to transform topology ranks to/from active processes ranks
 * @param procId The processor ID
 * @param procsCard Processor cardinality
 * @param blocksCard Blocks cardinality
 * @param wrap The wrap flat
 * @return The transformed rank
 */
typedef int	(*HitLayoutRanksFunction)(		char topoActiveMode,
											int procId,
											int procsCard,
											int blocksCard,
											float* extraParam );

/**
 * Function type to calculate the maximum cardinality of the assigned blocks
 *
 * @param procsCard Processors cardinality
 * @param blocksCard Blocks cardinality
 * @return The maximum cardinality
 */
typedef int	(*HitLayoutSignatureMaxCardFunction)(	int procsCard, 
													int blocksCard,
	   												float* extraParameter );

/**
 * Function type to calculate the minimum cardinality of the assigned blocks
 *
 * @param procsCard Processors cardinality
 * @param blocksCard Blocks cardinality
 * @return The maximum cardinality
 */
typedef int	(*HitLayoutSignatureMinCardFunction)(	int procsCard, 
													int blocksCard,
	   												float* extraParameter );

/**
 * Function type to calculate the number of active proceesors 
 *
 * @param procsCard Processors cardinality
 * @param blocksCard Blocks cardinality
 * @return The number of active processors
 */
typedef int	(*HitLayoutSignatureNumActivesFunction)(	int procsCard, 
														int blocksCard,
	   													float* extraParameter );


/* 1.2. STRUCTURE DEFINITION */
/**
 * ADT for the specific part of a layout signature.
 * A Signature Layout contains the shape that is a partition of the original shape,
 * a pointer to a function to calculate the signatures of the other processors shapes
 * and a integer that is used to restrict the shape to a given number of dimensions.
 *
 * @implements HitLayout
 */
typedef struct HitLayoutSig {

	/** FUNCTIONS (METHODS) TO COMPUTE DATA FOR OTHER VIRTUAL PROCESSORS */
	HitLayoutSignatureFunction signatureGenericF;	/**< Method to compute local signature. */
	HitLayoutSignatureInvFunction signatureInvGenericF;	/**< Method to compute inverse of signature. */
	HitLayoutSignatureFunction signatureRestrictedF;	/**< Method to compute local signature. */
	HitLayoutSignatureInvFunction signatureInvRestrictedF;	/**< Method to compute inverse of signature. */
	HitLayoutRanksFunction ranksGenericF;		/**< Method to transform ranks. */
	HitLayoutRanksFunction ranksRestrictedF;	/**< Method to transform ranks. */
	float* extraParameter;	/**< Extra optional parameter for some layout functions */
	int	restrictToDim;	/**< Dimension in which a different signature function is applied */

} HitLayoutSig;

/* 1.3. NULL CONSTANTS */
/**
 * Indicate that the dimension are not restricted
 * @hideinitializer
 */
#define	HIT_LAYOUT_NODIM	-1

/**
 * Null value for Signature Layouts
 */
extern HitLayoutSig HIT_LAYOUTSIG_NULL;
/**
 * Null static value for Signature Layouts.
 * @hideinitializer
 */
//#define HIT_SIGLAYOUT_NULL_STATIC	{NULL, NULL, NULL, NULL, NULL, NULL, 0.0, HIT_LAYOUT_NODIM}
#define HIT_SIGLAYOUT_NULL_STATIC	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, HIT_LAYOUT_NODIM}

/* 1.4. CONSTANTS FOR NO-WRAPPED AND WRAPPED (PERIODIC) SIGNATURE LAYOUTS */
/**
 * No wrapped (no periodic) signature layouts
 * @hideinitializer
 */
#define		HIT_NOWRAPPED		0
/**
 * Wrapped (periodic) signature layouts
 * @hideinitializer
 */
#define		HIT_WRAPPED			1



/* 2. Hit LIST LAYOUT */
/* 2.1. GROUP STRUCTURE DEFINITION */
/**
 * ADT for a group of processors
 * Each group have a leader and many no-leader processors.
 */
typedef struct {
	int leader;		/**< Leader rank of the group. */
	int numProcs;	/**< Number of processors in the group. */
} HitGroup;

/* 2.2 GROUP NULL VALUE */
/**
 * Null value for Groups.
 */
extern HitGroup HIT_GROUP_NULL;
/**
 * Null static value for Groups
 * @hideinitializer
 */
#define HIT_GROUP_NULL_STATIC	{0, 0}

/* 2.3. LAYOUT LIST STRUCTURE DEFINITION */
/**
 * ADT for the specific part of a layout list.
 * A layout list is a distribution of a set of elements or a graph into a set of groups.
 * This struct have list of elements, the list of groups and the information needed to
 * calculate the own list of elements or the list of elements of other processor.
 *
 * @implements HitLayout
 */
typedef struct {
	int numGroups;			/**< The number of groups created. */
	HitGroup *groups;		/**< The list of groups of processors. */
	int numElementsTotal;	/**< The total number of nodes/elements. */
	int *assignedGroups;	/**< The list of assigned group to each node/element. */
	int cardOwnElements;	/**< Local cardinality. */
	int cardPredElements;	/**< Cardinality of the predecessor. */
	int cardSuccElements;	/**< Cardinality of the successor. */
} HitLayoutList;

/* 2.4. LAYOUT LIST NULL VALUE */
/**
 * Null value for List Layout.
 */
extern HitLayoutList HIT_LAYOUTLIST_NULL;
/**
 * Null static value for List Layout.
 * @hideinitializer
 */
#define HIT_LAYOUTLIST_NULL_STATIC	{0, NULL, 0, NULL, 0, 0, 0}


/* 3. Hit LAYOUT UNION STRUCTURE */
/* 3.1. STRUCTURE DEFINITION */
/**
 * ADT for layouts.
 *
 * This ADT keeps both signature and list layouts.
 * In layouts groups the relations of neighbors is only defined between leaders.
 */
typedef struct HitLayout {

	int	type;	/**< Type of the layout. */

	/* TOPOLOGY AND VIRTUAL PROCESSOR NETWORK INFORMATION */
	HitTopology	topo; 				/**< Virtual topology. */
	int	wrap[HIT_MAXDIMS];			/**< Boundary conditions. */
	int	numActives[HIT_MAXDIMS];	/**< Number of active processors at each dimension. */

	/* DATA FOR THIS VIRTUAL PROCESSOR */
	int maxSize[HIT_MAXDIMS];		/**< Dimensional size of the biggest part across all processors in the given axis. */
	int minSize[HIT_MAXDIMS];		/**< Dimensional size of the smallest part across all processors in the given axis. */

	int	active;		/**< Is the current processor active? */

	// My group leader
	int group;		/**< Group where the processor belongs. */
	char leader;	/**< True if the local processor is the leader of the group */
	HitRanks leaderRanks;		/**< Ranks of the group's leader. */

	/* DATA FOR THIS VIRTUAL PROCESSOR */
	HitShape shape;	/**< Shape of the current processor */

	/* ORIGINAL SHAPE, COMMON FOR SIGs AND LISTSs */
	HitShape origShape;	/**< Original shape before perform the layout. */

	/** Physical virtual topology objects.  There is 1 element for
	 * all the processors and another one for each dimension.*/
	HitPTopology *pTopology[ HIT_MAXDIMS+1 ]; /**< Physical virtual topology. */
	HitPTopology *pTopologyGroup;	/**< Physical virtual group topology. */

	union {
		/* SUPPORT FOR SIGNATURE LAYOUTS */
		HitLayoutSig	layoutSig;	/**< Signature Layout. */
		/* SUPPORT FOR GROUP LAYOUTS */
		HitLayoutList	layoutList; /**< List Layout. */
	} info; /**< Union with the signature layout and list layout. */

	/* LOAD INFORMATION */
	double predecessorsLoad[HIT_MAXDIMS]; /**<Load of the predecessor. */
	double successorsLoad[HIT_MAXDIMS]; /**<Load of the successor. */
	double ownLoad;	/**<Local load. */
} HitLayout;

/* 3.2. NULL CONSTANT */
/**
 * Null value for a layout.
 */
extern HitLayout HIT_LAYOUT_NULL;
/**
 * Null static value for a layout.
 * @hideinitializer
 */
#define HIT_LAYOUT_NULL_STATIC	{0, HIT_TOPOLOGY_NULL_STATIC, { HIT_NOWRAPPED, HIT_NOWRAPPED, HIT_NOWRAPPED, HIT_NOWRAPPED }, { 0, 0, 0, 0 },  { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, HIT_GROUP_ID_NULL, 0, HIT_RANKS_NULL_STATIC, HIT_SHAPE_NULL_STATIC, HIT_SHAPE_NULL_STATIC, {NULL, NULL, NULL, NULL, NULL},  NULL, {HIT_SIGLAYOUT_NULL_STATIC}, { 0.0, 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0, 0.0 }, 0.0 }

/** @name Constants for classes and types of layouts */
/** @{ */

/* 3.3. CONSTANTS FOR CLASSES AND TYPES OF LAYOUTS */
/**
 * Constant for signature layout class.
 * @hideinitializer
 */
#define	HIT_LAYOUT_SIG_CLASS	1

/**
 * Constant for list layout class.
 * @hideinitializer
 */
#define	HIT_LAYOUT_LIST_CLASS	2

/**
 * Blocks layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_BLOCKS			1

/**
 * Blocks restricted to one dimension, layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_DIMBLOCKS		2

/**
 * Blocks with a minimum size, layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_MINBLOCKS		3

/**
 * BlocksX layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_BLOCKSX			5

/**
 * BlocksF (first) layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_BLOCKSF			6

/**
 * BlocksL (last) layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_BLOCKSL			7

/**
 * Whole structure in the leader
 * @hideinitializer
 */
#define HIT_LAYOUT_ALLINLEADER			5

/**
 * Whole structure copied in all processes.
 * @hideinitializer
 */
#define HIT_LAYOUT_COPY				6

/**
 * Blocks size determined by weights in one dimension, copy in the others.
 * @hideinitializer
 */
#define HIT_LAYOUT_DIMWEIGHTED_AND_COPY		8

/**
 * Blocks size determined by weights in one dimension, balanced blocks in the others.
 * @hideinitializer
 */
#define HIT_LAYOUT_DIMWEIGHTED_AND_BLOCKS	9

/**
 * Blocks size determined by weights, signature type layout.
 * @hideinitializer
 * @deprecated
 */
#define HIT_LAYOUT_WEIGHTED			70	// TODO: OBSOLETE
/**
 * Blocks size determined by weights in one dimension, signature type layout.
 * @hideinitializer
 * @deprecated
 */
#define HIT_LAYOUT_DIMWEIGHTED			71	// TODO: OBSOLETE

/**
 * Cyclic layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_CYCLIC			10

/**
 * Blocks with load balancing in one dimension.
 * @hideinitializer
 */
#define HIT_LAYOUT_BLOCKS_BALANCE	20

/**
 * Constant to distinguish list layout from signature layout.
 * @hideinitializer
 */
#define	HIT_LAYOUT_LIST_FIRST	100

/**
 * Contiguous layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_CONTIGUOUS		(HIT_LAYOUT_LIST_FIRST+1)

/**
 * Independent LB layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_INDEPENDENTLB	(HIT_LAYOUT_LIST_FIRST+2)

/**
 * Metis layout type constant.
 * @hideinitializer
 */
#define HIT_LAYOUT_METIS			(HIT_LAYOUT_LIST_FIRST+3)

/** @} */


/**
 * Get the class of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay The layout.
 * @retval HIT_LAYOUT_SIG_CLASS if it is a Signature Layout
 * @retval HIT_LAYOUT_LIST_CLASS if it is a List Layout
 */
#define hit_layout_class(lay) (((lay).type<HIT_LAYOUT_LIST_FIRST)?(HIT_LAYOUT_SIG_CLASS):(HIT_LAYOUT_LIST_CLASS))



/* 3.4 GROUP ID AND LEADER NULL VALUES */
/**
 * Null value for group ID
 * @hideinitializer
 */
#define HIT_GROUP_ID_NULL -1


/* 4. LAYOUT CONSTRUCTORS AND PREDEFINED LAYOUT FUNCTIONS */
/**
 * Hit layout constructor macro (1)
 * 
 * @hideinitializer
 * 
 * @param name Name of the layout type.
 * @param topo \e HitTopology Topology of the processors.
 * @param ... Extra parameters needed by the layout type constructor.
 * @return \e HitLayout A Layout.
 */
#define	hit_layout( name, topo, ...)	hit_layout_##name( 0, topo, __VA_ARGS__ )

/**
 * Hit layout constructor macro (2)
 * Free topology after using it.
 * 
 * @hideinitializer
 * 
 * @param name Name of the layout type.
 * @param topo \e HitTopology Topology of the processors.
 * @param ... Extra parameters needed by the layout type constructor.
 * @return \e HitLayout A Layout.
 */
#define	hit_layout_freeTopo( name, topo, ...)	hit_layout_##name( 1, topo, __VA_ARGS__ )


/** @name Signature Layout Constructors */
/** @{ */

/* 4.1 LAYOUT SIGNATURE CREATION FUNCTIONS */
/* 4.1.1 BLOCKS */
/**
 * Hit Layout Blocks constructor function
 * This layout leave the inactive processor at the end of the dimension
 * when there are more processors than data.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @return HitLayout A HitLayout Blocks.
 */
HitLayout	hit_layout_plug_layBlocks(int freeTopo, HitTopology topo, HitShape shape);

/**
 * Hit Layout DimBlocks constructor function
 * Applies blocks in a given dimension.
 * This layout leave the inactive processor at the end of the dimension
 * when there are more processors than data.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @param restrictDim The dimension to apply the block/band partition.
 * @return HitLayout A HitLayout DimBlocks.
 */
HitLayout	hit_layout_plug_layDimBlocks(int freeTopo, HitTopology topo, HitShape shape, int restrictDim );

/**
 * Hit Layout MinBlocks constructor function
 * The resulting blocks will have a minimum number of elements.
 * This layout leave the inactive processor at the end of the dimension
 * when there are more processors than data.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @param minElems The minimum number of elements on each partition.
 * @return HitLayout A HitLayout MinBlocks.
 */
HitLayout	hit_layout_plug_layMinBlocks(int freeTopo, HitTopology topo, HitShape shape, int minElems );

/* 4.1.2 BLOCKS X  */
/**
 * An alternative implementation of Blocks.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @return HitLayout A HitLayout Blocks X.
 */
HitLayout	hit_layout_plug_layBlocksX(int freeTopo, HitTopology topo, HitShape shape);

/* 4.1.3 BLOCKSF */
/**
 * Hit Layout BlocksF constructor function (Blocks at First)
 * This layout mix active and inactive processors in a regular way
 * making groups when there are more processors than data.
 * The leader is the first processor in the group.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @return HitLayout A HitLayout Blocks at First.
 */
HitLayout	hit_layout_plug_layBlocksF(int freeTopo, HitTopology topo, HitShape shape);

/* 4.1.3 BLOCKSL */
/**
 * Hit Layout BlocksL constructor function (Blocks at Last)
 * This layout mix active and inactive processors in a regular way
 * making groups when there are more processors than data.
 * The leader is the last processor in the group.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @return HitLayout A HitLayout Blocks at Last.
 */
HitLayout	hit_layout_plug_layBlocksL(int freeTopo, HitTopology topo, HitShape shape);

/* 4.1.4 CYCLIC */
/**
 * Hit Layout Cyclic constructor function
 * Distribute the data defined by a shape among the processors in
 * a cyclic way.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @param dim Number of dimensions.
 * @return HitLayout A HitLayout Cyclic.
 */
HitLayout	hit_layout_plug_layCyclic(int freeTopo, HitTopology topo, HitShape shape, int dim);

/* 4.1.5 ALL IN LEADER */
/**
 * Hit Layout AllInLeader constructor function
 * Distribute the whole domain to a single proccess: The leader
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @return HitLayout A HitLayout AllInLeader.
 */
HitLayout	hit_layout_plug_layInLeader(int freeTopo, HitTopology topo, HitShape shape);

#ifdef NOT_IMPLEMENTED
/* 4.1.6 NOT IN LEADER */
/**
 * Hit Layout NotInLeader constructor function
 * Distribute the whole domain among all processes different of 
 * the leader. This layout leaves the inactive processor at the end 
 * of the dimension when there are more processors than data.
 * @param topo The topology
 * @param shape The global shape
 */
HitLayout	hit_layout_plug_layNotInLeader(int freeTopo, HitTopology topo, HitShape shape);
#endif

/* 4.1.6 COPY */
/**
 * Hit Layout Copy constructor function
 * The whole domain is copied on all processes
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @return HitLayout A HitLayout Copy.
 */
HitLayout	hit_layout_plug_layCopy(int freeTopo, HitTopology topo, HitShape shape);

/**@}*/

/** @name Weighted Layout Constructors */
/** @{ */

/* 4.1.7.A STRUCTURE FOR STORING WEIGHTS (FOR WEIGHTED LAYOUTS) */
/**
 * Array to store a list of float weights for unbalanced signature layouts
 * 
 * @param num_procs The number of proccess that are going to be used
 * @param ratios A float array to place the weight of each process. The possition indicates the process number and its content the weight.
 */
typedef struct
{
	int num_procs; /**< The number of processes that are going to be used. */
	float *ratios; /**<A float array to place the weight of each process. The possition indicates the process number and its weight content. */
} HitWeights;

/**
 * Constructor (1) of HitWeights.
 * 
 * This constructor creates an empty ratios array.
 * 
 * @hideinitializer
 * 
 * @param[in] n The number of processes.
 * @return A new HitWeights object with all ratios initialized to 0.
 */ 
static inline HitWeights hitWeightsEmpty( int n ) {
	HitWeights res = { 0, NULL };
	res.ratios = (float *)calloc( (size_t)n, sizeof(float) );
	if (res.ratios == NULL) return res;
	res.num_procs = n;
	return res;
}

/**
 * Constructor (2) of HitWeights.
 * 
 * This constructor receives a custom array of ratios for the processes.
 * 
 * @hideinitializer
 * 
 * @param[in] n The number of processes.
 * @param[in] w A pointer to the array with the weights.
 * @return A new HitWeights object.
 */ 
static inline HitWeights hitWeights( int n, float *w ) {
	HitWeights res;
	res.num_procs = n;
	res.ratios = w;
	return res;
}

/* 4.1.7.B WEIGHTED TO SELECTED DIMENSION AND COPY IN THE OTHER DIMENSIONS */
/**
 * Hit Layout DimBlocksWeighted constructor function.
 * Distributes the selected dimension by the weights indicated as
 * parameters and gives the entire remaining dimmensions to every 
 * process.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @param restrictDim The dimension in which the weights are applyied.
 * @param l_ratios The weights that are given to each processor (automatically normalized, it is possible to use any scale).
 * @return HitLayout A HitLayout DimBlocksWeighted.
 */
HitLayout hit_layout_plug_layDimWeighted_Copy(int freeTopo, HitTopology topo, HitShape shape, int restrictDim, HitWeights l_ratios);


/* 4.1.8.C WEIGHTED TO SELECTED DIMENSION AND BLOCKS IN THE OTHER DIMENSIONS*/
/**
 * Hit Layout BlocksWeightedToSelectedDim constructor function.
 * Distributes the selected dimension by the weights indicated as
 * parameters and makes a block layout in the remaining dimensions.
 * 
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @param restrictDim The dimension in which the weights are applyied.
 * @param l_ratios The weights that are given to each processor (automatically normalized, it is possible to use any scale).
 * @return HitLayout A HitLayout BlocksWeightedToSelectedDim.
 */
HitLayout hit_layout_plug_layDimWeighted_Blocks(int freeTopo, HitTopology topo, HitShape shape, int restrictDim, HitWeights l_ratios);

#ifdef OBSOLETE
/* 4.1.7 WEIGHTED */
/**
 * TODO: Write the documentation
 */
HitLayout       hit_layout_plug_layWeighted(int freeTopo, HitTopology topo, HitShape shape, float* weights);


/* 4.1.7 WEIGHTED WITH LOAD BALANCE IN ONE DIMENSION */
/**
 * TODO: Write the documentation
 */
HitLayout       hit_layout_plug_layDimWeighted(int freeTopo, HitTopology topo, HitShape shape, int restrictDim, float* weights);
#endif

/* 4.1.10 BLOCKS WITH LOAD BALANCE IN ONE DIMENSION */
/**
 * Hit Layout Blocks with Load Balance in One Dimension constructor function
 * This layout leave the inactive processor at the end of the dimension
 * when there are more processors than data.
 *
 * TODO: This is a test implementation for only two types of devices that 
 * simply applies a trivial load-balancing in one dimension
 *
 * @param freeTopo 1 if the topology resources are freed after its use.
 * @param topo The topology.
 * @param shape The global shape.
 * @param dim The dimension in which load balance is applyied.
 * @param load The normalized capability of the first device (between 0.0 and 1.0).
 * @return HitLayout A HitLayout.
 */
HitLayout hit_layout_plug_layBlocksBalance(int freeTopo, HitTopology topo, HitShape shape, int dim, float load);

/** @} */

/** @name List Layout Constructors */
/** @{ */

/* 4.2 LIST LAYOUT CREATION FUNCTIONS */
/* 4.2.1  SCHEDULING FOR CONTIGUOUS TASKS*/
/**
 * hit_layout_plug_layContiguous: generic sheduling of n contiguos blocks to m processors,
 * 							 according to block weights.
 * 
 * @param 	freeTopo 1 if the topology resources are freed after its use.
 * @param	topo the topology.
 * @param	elements the shape that represent the element domain.
 * @param	weights the block weights.
 * @return \e HitLayout a list layout.
 */
HitLayout hit_layout_plug_layContiguous(int freeTopo, HitTopology topo, HitShape elements, const double *weights);

/* 4.2.2 GENERIC SCHEDULING FOR INDEPENDENT TASKS: LOAD BALANCING */
/**
 * hit_layout_plug_layIndependentLB: generic sheduling of n independent blocks to m processors,
 * 							 according to block weights.
 * 
 * @param	freeTopo 1 if the topology resources are freed after its use.
 * @param	topo the topology.
 * @param	elements the shape that represent the element domain.
 * @param	weights the block weights
 * @return \e HitLayout a list layout.
 */
HitLayout hit_layout_plug_layIndependentLB(int freeTopo, HitTopology topo, HitShape elements, const double *weights);

/* 4.2.3 METIS GRAPH PARTITION */
/**
 * Use METIS library to distribute a graph into groups of processors.
 * 
 * @param 	freeTopo 1 if the topology resources are freed after its use.
 * @param	topo The topology.
 * @param	shape A pointer to the sparse shape that represents the graph.
 * @return \e HitLayout a list layout.
 */
HitLayout hit_layout_plug_layMetis(int freeTopo, HitTopology topo, HitShape * shape);

/**
 * Distributes a graph into groups of processors by "rows".
 * 
 * @param 	freeTopo 1 if the topology resources are freed after its use.
 * @param	topo The topology.
 * @param	shape A pointer to the sparse shape that represents the graph.
 * @return \e HitLayout a list layout.
 */
HitLayout hit_layout_plug_layBitmap(int freeTopo, HitTopology topo, HitShape * shape);

/**
 * Distributes a matrix into groups of processors by rows.
 * 
 * @param 	freeTopo 1 if the topology resources are freed after its use.
 * @param	topo The topology.
 * @param	shape  A pointer to the sparse shape that represents the matrix.
 * @return \e HitLayout a list layout.
 */
HitLayout hit_layout_plug_laySparseRows(int freeTopo, HitTopology topo, HitShape * shape);

/**
 * Distributes a matrix into groups of processors by rows.
 * 
 * @param 	freeTopo 1 if the topology resources are freed after its use.
 * @param	topo The topology.
 * @param	shapeP A pointer to the sparse shape that represents the matrix.
 * @return \e HitLayout a list layout
 * @note Layout for bitmap shapes.
 */
HitLayout hit_layout_plug_laySparseBitmapRows(int freeTopo, HitTopology topo, HitShape * shapeP);

/** @} */

/** @name Access to basic and static information in Signature and List Layouts */
/** @{ */
/* 5. ACCESS TO SIGNATURE AND LIST LAYOUTS */
/* 5.1. ACCESS TO BASIC STATIC INFORMATION */

/**
 * Return the topology used to create the layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 */
#define	hit_layTopology( lay )	((lay).topo)

/**
 * Return the topology with the active processors
 * 
 * @param lay \e HitLayout Layout.
 * @return HitTopology Topology with the active processors in the layout.
 */
HitTopology hit_layActivesTopology( HitLayout lay );

/**
 * Return the id of the local group
 * 
 * @hideinitializer
 * 
 * @param lay a HitLayout.
 * @return \e int the id of the processor's group.
 */
#define	hit_layGroup( lay )	((lay).group)

/**
 * Build and return the subtopology of a given group
 * 
 * @param lay A HitLayout.
 * @param groupId The id of a group.
 * @return HitTopology The subtopology of the specified group in the layout.
 */
HitTopology hit_layGroupTopo( HitLayout lay, int groupId );

/**
 * Return the ranks of the local-group's leader.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e HitRanks Ranks of the group's leader.
 */
#define	hit_layLeader( lay )	((lay).leaderRanks)

/**
 * Return the load of the local group.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e double The local load.
 */
#define hit_layLoad( lay )	((lay).ownLoad)

/**
 * Return the load of the predecessor in a dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @return \e double The predecessor's load.
 */
#define hit_layPredLoad( lay, dim )	((lay).predecessorsLoad[dim])

/**
 * Return the load of the successor in a dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @return \e double The successor's load.
 */
#define hit_laySuccLoad( lay, dim )	((lay).successorsLoad[dim])

/**
 * Active status: TODO: Duplicated A.K.A. "hit_layImActive" (see bellow)
 */
//#define	hit_layActive(lay)	((lay).active)

/**
 * Return the original Shape of a Layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return HitShape The full shape of the layout.
 */
#define	hit_layFullShape(lay)	((lay).origShape)

/** @} */

/** @name Access to basic and static information in Signature Layouts */
/** @{ */

/* 6. ACCESS TO SIGNATURE LAYOUT */
/* 6.1. ACCESS TO BASIC STATIC INFORMATION */
/**
 * Get the local shape of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return HitShape The local shape of the processor.
 */
#define	hit_layShape(lay)	((lay).shape)

/**
 * Get the number of dimensions of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int The number of dimensions.
 */
#define	hit_layNumDims(lay)	(hit_shapeDims((lay).shape))

/**
 * Max size of the biggest shape of the layout in a given dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int The dimension.
 * @return \e int Maximum shape size in the dimension.
 */
#define	hit_layDimMaxSize(lay, dim)	((lay).maxSize[dim])

/**
 * Number of active processors in a dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int The dimension.
 * @return \e int The number of active processors.
 */
#define	hit_layDimNumActives(lay, dim)	((lay).numActives[dim])

/**
 * Number of total active processors in the layout
 * 
 * @param[in] lay A HitLayout.
 * @return \e int number of active processors.
 */
int hit_layNumActives( HitLayout lay );

/**
 * Get if the layout is restricted to dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int The dimension the layout is restricted in.
 */
#define	hit_layOnDim(lay)	((lay).info.layoutSig.restrictToDim)

/**
 * Shortcut: Signature of a given dimension for the local shape of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int The dimension.
 * @return HitSig The signature of the layout in that dimension.
 */
#define hit_layDimSig( lay, dim ) hit_shapeSig( (lay).shape, dim )

/**
 * Shortcut: Cardinality of the signature in a given dimension, for the local shape of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim int The dimension.
 * @return \e int The cardinality of the signature of the layout in that dimension.
 */
#define hit_layDimCard( lay, dim ) hit_sigCard( hit_layDimSig(lay,dim) )

/**
 * Shortcut: Cardinality of the whole local shape of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int The total cardinality of the layout.
 */
#define hit_layCard( lay ) hit_shapeCard( (lay).shape )

/**
 * Shortcut: Begin of a signature of the local shape.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int The dimension of the signature.
 * @return \e int The begin index of the signature. 
 */
#define hit_layDimBegin( lay, dim ) hit_layDimSig(lay,dim).begin

/**
 * Shortcut: End of a signature of the local shape.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int The dimension of the signature.
 * @return \e int The end index of the signature.
 */
#define hit_layDimEnd( lay, dim ) hit_layDimSig(lay,dim).end

/**
 * Shortcut: Stride of a signature of the local shape.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int The dimension of the signature.
 * @return \e int The stride of the signature.
 */
#define hit_layDimStride( lay, dim ) hit_layDimSig(lay,dim).stride
/** @} */

/** @name Obtain data of other virtual processor */
/** @{ */
/* 6.2. METHODS TO OBTAIN DATA FOR OTHER VIRTUAL PROCESSOR */
/* 6.2.1. EXTERNAL DECLARATIONS */

/**
 * Get the neighbor of a processor in a dimension with a shift.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @param shift \e int Shift.
 * @return \e int The rank of the neighbor.
 */
#define hit_layDimNeighbor( lay, dim, shift )	hit_layNeighborFromTopoRank(lay, lay.topo.self.rank[dim], dim, shift)

/**
 * Get the neighbor in a dimension with a given active coordinate
 */
//#define hit_layDimNeighborAbs( lay, dim, coord )	hit_ranks2( lay.topo.self.rank[dim], dim, shift)

/**
 * Get the ranks of neighbor shifting in a dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @param shift \e int Shift.
 * @return \e HitRanks The ranks of the neighbor.
 */
#define hit_layNeighbor( lay, dim, shift )	hit_layNeighborRanks(lay, dim, shift)

/**
 * Get the ranks of a neighbor shifting by a given factor in one dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param shift0 \e int Shift.
 * @return \e HitRanks The ranks of the neighbor.
 */
#define hit_layNeighbor1( lay, shift0 )		hit_layNeighborRanks(lay, 0, shift0)
/**
 * Return the ranks of a neighbor shifting in two dimensions.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param shift0 \e int Shift in the dimension 0.
 * @param shift1 \e int Shift in the dimension 1.
 * @return \e HitRanks The ranks of the neighbor.
 */
#define hit_layNeighbor2( lay, shift0, shift1 ) hit_layNeighborRanksFrom(lay, hit_layNeighborRanks(lay, 0, shift0), 1, shift1 )
/**
 * Return the ranks of a neighbor shifting by a given factor in three dimensions
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param shift0 \e int Shift in the dimension 0.
 * @param shift1 \e int Shift in the dimension 1.
 * @param shift2 \e int Shift in the dimension 2.
 * @return \e HitRanks The ranks of the neighbor.
 */
#define hit_layNeighbor3( lay, shift0, shift1, shift2 ) hit_layNeighborRanksFrom( lay, hit_layNeighborRanksFrom(lay, hit_layNeighborRanks(lay, 0, shift0), 1, shift1 ), 2, shift2 )

/**
 * Get all the ranks of neighbors shifting in several dimensions indicated by a single ranks parameter.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param shiftRanks \e HitRanks The shifts.
 * @return \e HitRanks The ranks of the neighbors.
 */
#define hit_layNeighborN( lay, shiftRanks )	hit_layNeighborRanksFromRanks(lay, lay.topo.self, shiftRanks)

/**
 * Get the shape of a neighbor shifting in a given dimension.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @param shift \e int Shift.
 * @return HitShape The shape of a neighbor.
 */
#define	hit_layShapeNeighbor(lay, dim, shift)	(hit_layout_wrapperNeighborShape( lay, dim, shift ))

/**
 * Get the shape of a neighbor defined by its ranks.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param ranks \e HitRanks the ranks of the neighbor.
 * @return HitShape The shape of a neighbor.
 */
#define	hit_layShapeOther(lay, ranks)	(hit_layout_wrapperOtherShape( lay, ranks ))

/**
 * Return the maximum shape of the neighbors in the layout. This function can be used
 * to make a buffer to share data among the processors.
 * 
 * @param lay The Layout.
 * @return HitShape The maximum shape.
 */
HitShape hit_layMaxShape(HitLayout lay);

/**
 * Return the maximum shape of the neighbors in a given dimension in the layout. 
 * This function can be used to make a buffer to share data among neighbors in a dimension.
 * 
 * @param lay The Layout.
 * @param dim Dimension.
 * @return HitShape The maximum shape.
 */
HitShape hit_layDimMaxShape(HitLayout lay, int dim);

/**
 * Return the minimum shape of the neighbors in the layout. This function can be used
 * to make a buffer to share data among the processors.
 * 
 * @param lay The Layout.
 * @return HitShape The minimum shape.
 */
HitShape hit_layMinShape(HitLayout lay);

/**
 * Return the minimum shape of the neighbors in a given dimension in a the layout. 
 * This function can be used to make a buffer to share date among neighbors in a dimension.
 * 
 * @param lay The Layout.
 * @param dim Dimension.
 * @return HitShape The minimum shape.
 */
HitShape hit_layDimMinShape(HitLayout lay, int dim);
/** @} */

/* 6.3. FUNCTIONS TO CHANGE LAYOUT OPTIONS (SIGNATURE LAYOUTS) */
/**
 * Set the periodic boundary conditions
 * 
 * @param[in,out] lay A pointer to the HitLayout to wrap.
 */
void hit_layWrapNeighbors(HitLayout *lay);

/**
 * Set the periodic boundary conditions for one dimension
 * 
 * @param[in,out] lay A pointer to the HitLayout to wrap.
 * @param dim Dimension to wrap.
 */
void hit_layWrapNeighborsDim(HitLayout *lay, int dim);

/**
 * Unset the periodic boundary conditions
 * 
 * @param[in,out] lay A pointer to the HitLayout to unwrap.
 */
void hit_layUnwrapNeighbors(HitLayout *lay);

/**
 * Unset the periodic boundary conditions for one dimension
 * 
 * @param[in,out] lay A pointer to the HitLayout to unwrap.
 * @param dim Dimension to unwrap.
 */
void hit_layUnwrapNeighborsDim(HitLayout *lay, int dim);



/* 6.4.1. DO ONLY FOR ACTIVATED PROCESSORS IN THE LAYOUT */
/**
 * Check if the local process is activated in the layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \c True if the layout is active.
 */
#define hit_layImActive( lay )	((lay).active)

/**
 * Issue a warning in case of inactive processors in a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 */
#define hit_layInactiveWarning( lay )	\
	if ( hit_layImLeader(lay) ) { \
		int numInactive = hit_topCard( (lay).topo ) - hit_layNumActives( lay );	\
		if ( numInactive > 0 ) {	\
			char message[256];	\
			sprintf( message, "%d Processors inactive in layout \"%s\"", numInactive, #lay);	\
			hit_warning( message, __FILE__, __LINE__ );	\
		}	\
	}

/* 6.4.2 DO ONLY FOR LEADER OF ACTIVATED PROCESSORS IN THE LAYOUT */
/**
 * Check if the local process is the leader (rank 0) in the topology.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \c True if the process is leader.
 */
#define hit_layImLeader( lay ) 	hit_topImLeader( (lay).topo )

/** @name Iterators */
/** @{ */	
/* 6.5. LOOPS ACROSS VIRTUAL PROCESSORS */
/**
 * Extern array used by the hit_lsig_vfor function.
 * @see hit_lsig_vfor.
 */
extern int hit_lsig_vfor_index[HIT_MAXDIMS];
/**
 * Loop across virtual process.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @param index Variable index.
 */
#define	hit_lsig_vfor(lay, dim, index)	\
	for( hit_lsig_vfor_index[dim]=0, index=0;			\
	hit_lsig_vfor_index[dim]<hit_lsig_numActives(lay,dim);	\
	hit_lsig_vfor_index[dim]++, index=hit_lsig_vDimNeighbor(lay,index,dim,1) )

/* 6.6. LOOPS ACROSS ELEMENTS IN THE VIRTUAL PROCESS */
/**
 * Loop across elements in the virtual process.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param dim \e int Dimension.
 * @param index Variable index.
 */
#define	hit_layForDimDomain(lay, dim, index)	for( index=hit_layDimBegin(lay, dim);	\
												index<=hit_layDimEnd(lay,dim);	\
												index=index+hit_layDimStride(lay,dim) )
/** @} */

/* 6.7 RANKS */
/**
 * Return the ranks of the local virtual processor in the network of active
 * virtual processors defined by a given layout
 * 
 * @hideinitializer
 * 
 * @param	lay	layout which define the active processors
 * @return	HitRanks with the coordinates of the virtual processor
 */
#define hit_laySelfRanks( lay )	(hit_layToActiveRanks( lay, (lay).topo.self ))

/**
 * Return a given dimension of the ranks of the local virtual processor
 * in the network of active virtual processors defined by a given layout.
 * 
 * @hideinitializer
 * 
 * @param	lay	layout which define the active processors
 * @param	dim	selected dimension
 * @return	HitRanks with the coordinates of the virtual processor
 */
#define hit_laySelfRanksDim( lay, dim )	(hit_laySelfRanks( lay ).rank[dim])

/**
 * Return the rank of the local virtual processor in the group of active
 * virtual processors defined by a given layout.
 * 
 * @hideinitializer
 * 
 * @param	lay	Layout which defines the active processors
 * @return	\e int The rank of the virtual processor
 * @author arturo 2015/01/22
 */
#define hit_laySelfRank( lay )	( (!(lay).active ) ? HIT_RANK_NULL_STATIC : hit_topSelfRankInternal( (lay).topo ) )




/* 7. LIST LAYOUT FUNCTIONS */
/**
 * List of the elements assigned to a group
 * @param layout The layout.
 * @param group The asked group number.
 * @param elements pointer to a int array where the elements number are allocated.
 * @param nElements pointer to a int where the number of elements are saved.
 */
void hit_lay_elements(HitLayout layout, int group, int ** elements, int * nElements);

/**
 * Calculate the group number for a given processor
 * @param layout The layout.
 * @param processor The processor number.
 * @return The group number of the select processor.
 */
int hit_lay_procGroup(HitLayout layout, int processor);

/** @name Access to information related to Groups */
/** @{ */
/* 7.1. SHORT-CUTS */
/* SHORT-CUTS TO GET THE GROUP STRUCTURE */
/**
 * Short-cut to get the number of groups.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @return \e int The number of groups.
 */
#define hit_lgr_numGroups(lay)	(lay.info.layoutList.numGroups)

/**
 * Short-cut to get the group identified by an index.
 * 
 * @deprecated
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param group Group index.
 * @return \e HitGroup The group object.
 */
#define hit_lgr_group(lay, group)	(lay.info.layoutList.groups[group])

/* SHORT-CUTS TO GET THE INFORMATION ABOUT THE LOCAL NUMBER OF ELEMENTS */
/**
 * Get the local number of elements.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int Local cardinality.
 */ 
#define hit_lgr_card(lay)	(lay.info.layoutList.cardOwnElements)

/**
 * Get the total number of elements.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int Total cardinality.
 */ 
#define hit_lgr_cardTotal(lay)	(lay.info.layoutList.numElementsTotal)

/**
 * Get the total number of elements of the predecessor.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int Cardinality of the predecessor.
 */
#define hit_lgr_cardPred(lay)	(lay.info.layoutList.cardPredElements)

/**
 * Get the total number of elements of the predecessor.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return \e int Cardinality of the successor.
 */
#define hit_lgr_cardSucc(lay)	(lay.info.layoutList.cardSuccElements)

/* SHORT-CUTS TO GET INFORMATION FOR ELEMENTS */
/**
 * Get the leader of the group that has the element.
 * 
 * @deprecated
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param element Data element.
 * @return The leader.
 */
#define hit_lgr_leader(lay, element)	(lay.info.layoutList.groups[lay.info.layoutList.assignedGroups[element]].leader)
/**
 * Get the number of processors of the group that has the element.
 * 
 * @deprecated
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param element Data element.
 * @return The number of processors.
 */
#define hit_lgr_nProcs(lay, element)	(lay.info.layoutList.groups[lay.info.layoutList.assignedGroups[element]].numProcs)
/**
 * Get the group index that has the element.
 * 
 * @deprecated
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param element Data element.
 * @return The group index.
 */
#define hit_lgr_elementGroup(lay, element)	((lay).info.layoutList.assignedGroups[element])
/** @} */

/**
 * Return if the local layout has an element.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param element Domain index
 * @return \c True if the index is in the local domain
 * @todo NOTE: This is the implementation only for the list layouts. Easy to extent for signature
 */
#define hit_layHasElem(lay, element)	((lay).group == (lay).info.layoutList.assignedGroups[element])


/** @name Access to information related to Groups */
/** @{ */
/* SHORT-CUTS TO GET INFORMATION FOR GROUPS */
/**
 * Get the index of the leader of the group.
 * 
 * @deprecated
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param group Group index.
 * @return \int The leader.
 */
#define hit_lgr_groupLeader(lay, group)	(lay.info.layoutList.groups[group].leader)

/**
 * Get the number of processors in the group.
 * 
 * @deprecated
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param group Group index.
 * @return The number of processors.
 */
#define hit_lgr_groupNProcs(lay, group)	(lay.info.layoutList.groups[group].numProcs)

/**@} */

/**
 * Return the extended sparse shape of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @return The extended shape.
 */
#define hit_layExtendedShape(lay) ((lay).info.layoutList.extendedShape)



/* 8. HIT LAYOUT FREE */
/**
 * Free allocated resources used by the layout.
 * 
 * @param lay A HitLayout.
 */
void hit_layFree(HitLayout lay);



/* 9. INTERNAL FUNCTIONS */
/** @cond INTERNAL */
HitRanks hit_layTransformRanks( char topoActiveMode, HitLayout lay, HitRanks ranks );
int	hit_layNeighborFrom(HitLayout self, int source, int dim, int shift);
int	hit_layNeighborFromTopoRank(HitLayout self, int source, int dim, int shift);
int	hit_layNeighborDistance(HitLayout self, int dim, int shift);
HitRanks	hit_layNeighborRanks(HitLayout self, int dim, int shift);
HitRanks	hit_layNeighborRanksFrom(HitLayout self, HitRanks source, int dim, int shift);
HitRanks	hit_layNeighborRanksFromRanks(HitLayout self, HitRanks source, HitRanks shifts );
HitShape	hit_layout_wrapperNeighborShape(HitLayout self, int dim, int shift);
HitShape	hit_layout_wrapperOtherShape(HitLayout self, HitRanks ranks);




/* 9.1. INTERNAL FUNCTIONS TO BUILD LAYOUTS */
int	hit_layout_wrapperShape(	int topoType,
								int topoNumDims, 
								HitRanks proc, 
								int card[HIT_MAXDIMS],
								HitShape shape, 
								HitShape *newShape,
								HitLayoutSignatureFunction	sigFunctionGeneric,
								HitLayoutSignatureFunction	sigFunctionRestricted,
								float* extraParameter,
								int restrictToDim
								); 
HitLayout	hit_layout_wrapper(	HitTopology topo, 
								HitShape shape, 
								HitLayoutSignatureFunction signatureGenericF,
								HitLayoutSignatureInvFunction signatureInvGenericF,
								HitLayoutSignatureFunction signatureRestrictedF,
								HitLayoutSignatureInvFunction signatureInvRestrictedF,
								HitLayoutRanksFunction ranksGenericF,
								HitLayoutRanksFunction ranksRestrictedF,
								HitLayoutSignatureMaxCardFunction maxCardGenericF,
								HitLayoutSignatureMaxCardFunction maxCardRestrictedF,
								HitLayoutSignatureMinCardFunction minCardGenericF,
								HitLayoutSignatureMinCardFunction minCardRestrictedF,
								HitLayoutSignatureNumActivesFunction activesGenericF,
								HitLayoutSignatureNumActivesFunction activesRestrictedF,
								float* extraParameter,
								int restrictToDim
								);
/** @endcond */

/**
 * Transforms multidimensional active ranks to a process id in the active grid.
 * 
 * @param lay A HitLayout.
 * @param ranks Active Ranks.
 * @return Process identifier (linear rank).
 */
int hit_layActiveRanksId( HitLayout lay, HitRanks ranks );

/**
 * Transforms a process id to multidimensional active ranks in the active grid.
 * 
 * @param lay A HitLayout.
 * @param id Process id.
 * @return \e HitRanks Active ranks.
 */
HitRanks hit_layActiveIdRanks(HitLayout lay, int id);

/**
 * Transforms topology ranks to active ranks.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param ranks Topology ranks.
 * @return Active ranks.
 */
#define hit_layToActiveRanks( lay, ranks )	hit_layTransformRanks( HIT_LAY_RANKS_TOPO_TO_ACTIVE, lay, ranks )

/**
 * Transforms active ranks to topology ranks.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @param ranks Active ranks.
 * @return Topology ranks.
 */
#define hit_layToTopoRanks( lay, ranks )	hit_layTransformRanks( HIT_LAY_RANKS_ACTIVE_TO_TOPO, lay, ranks )

/**
 * Transforms shape index to active rank of the owner process
 * 
 * @param lay A HitLayout.
 * @param dim Dimension.
 * @param ind Shape index.
 * @return \e int Active rank of the owner.
 */
int hit_layDimOwner( HitLayout lay, int dim, int ind );

#ifdef __cplusplus
}
#endif

/* END OF HEADER FILE _HitLayout_ */
#endif
