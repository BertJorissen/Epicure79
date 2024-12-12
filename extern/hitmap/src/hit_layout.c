/**
 * Layout management module.
 * A layout maps the indexes in a shape domain
 * into the processes from a virtual topology
 * 
 * @file hit_layout.c
 * @version 1.7
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Sep 2019
 *
 */

/*
 * <license>
 * 
 * Hitmap v1.4
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
 * Copyright (c) 2007-2024, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include <stdio.h>
#include <math.h>

#include <hit_layout.h>
#include <hit_funcop.h>

#include <hit_allocP.h>

#include <hit_sshape.h>
#include <hit_cshape.h>
#include <hit_bshape.h>

/* MACROS FOR LAYOUT SIGNATURE COMPUTATION */
#define HIT_LAY_SIG_DIVISOR hit_min(blocksCard, procsCard)
#define HIT_LAY_SIG_BEGIN( procId ) ((procId) * (blocksCard / HIT_LAY_SIG_DIVISOR)) \
	+ (hit_min(procId, blocksCard % HIT_LAY_SIG_DIVISOR))

/* TOOL: EXTERN DECLARATION FOR vfor LOOPS */
int hit_lsig_vfor_index[HIT_MAXDIMS];

/* NULL SIGNATURE LAYOUT VALUE */
HitLayoutSig HIT_LAYOUTSIG_NULL = HIT_SIGLAYOUT_NULL_STATIC;

/* NULL GROUP VALUE */
HitGroup HIT_GROUP_NULL = HIT_GROUP_NULL_STATIC;

/* NULL GROUP LAYOUT VALUE */
HitLayoutList HIT_LAYOUTLIST_NULL = HIT_LAYOUTLIST_NULL_STATIC;

/* NULL LAYOUT VALUE */
HitLayout HIT_LAYOUT_NULL = HIT_LAYOUT_NULL_STATIC;


/* PREDEFINED LAYOUT FUNCTIONS */

/* A.1. GENERIC SIGNATURE FUNCTIONS */
/* NOTE: AN INVERSE FUNCTION IS ALSO PROVIDED FOR EACH SIGNATURE */

/* A.1.1. GENERIC SIGNATURES: COPY SIGNATURE -- NON-PARTITIONED DIMENSION */
int	hit_layout_plug_layCopy_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	HIT_NOT_USED( procId );
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( extraParameter );

	/* COPY SIGNATURE */
	(*res) = input;
	/* RETURN ACTIVE */
	return 1;
}
int	hit_layout_plug_layCopy_SigInv(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input,
												int ind ) {
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( input );
	HIT_NOT_USED( extraParameter );

	/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
	if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;

	/* RETURN PROC ID: MYSELF */
	return procId;
}

/* A.1.2. GENERIC SIGNATURES: ALL IN ONE -- NON-PARTITIONED DIMENSION */
int	hit_layout_plug_layAllInOne_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( extraParameter );

	int active;

	/* COPY SIGNATURE */
	if ( procId == 0 ) {
		(*res) = input;
		active = 1;
	}
	else {
		(*res) = HIT_SIG_NULL;
		active = 0;
	}

	/* RETURN ACTIVE */
	return active;
}
int	hit_layout_plug_layAllInOne_SigInv(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input,
												int ind ) {
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( input );
	HIT_NOT_USED( extraParameter );

	/* CHECK: ONLY THE LEADER HAS SOMETHING */
	if ( procId == 0 ) {
		/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
		if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;
		return 0;
	}
	else return HIT_RANK_NULL;
}

/* A.2 GENERIC MAX/MIN CARD FUNCTIONS */
/* A.2.1 GENERIC MAX CARD: COPY SIGNATURE  -- NON-PARTITIONED DIMENSION */
int	hit_layout_plug_layCopy_maxCard( int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( extraParameter );

	return blocksCard;
}

/* A.2.2. GENERIC MAX CARD: REGULAR (BLOCKING/CYCLIC) */
int	hit_layout_plug_layRegular_maxCard( int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( extraParameter );

	return (int)ceil( (double)blocksCard / procsCard);
}

/* A.2.3 GENERIC MIN CARD: COPY SIGNATURE  -- NON-PARTITIONED DIMENSION */
int	hit_layout_plug_layCopy_minCard( int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( extraParameter );

	return blocksCard;
}

/* A.2.4. GENERIC MIN CARD: REGULAR (BLOCKING/CYCLIC) */
int	hit_layout_plug_layRegular_minCard( int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( extraParameter );

	return (int)floor( (double)blocksCard / procsCard);
}

/* A.3 GENERIC NUM ACTIVES FUNCTIONS */
/* A.3.1. GENERIC NUM. ACTIVES: REGULAR (BLOCKING/CYCLIC) */
int	hit_layout_plug_layRegular_numActives( int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( extraParameter );

//	printf("CTRL REGULAR_numActive: Rank: %d --- %d, %d, %f\n", hit_Rank, procsCard, blocksCard, extraParameter );
	return hit_min( blocksCard, procsCard );
}

/* A.3.2. GENERIC NUM. ACTIVES: WHOLE STRUCTURE IN ONE */
int	hit_layout_plug_layAllInOne_numActives( int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( extraParameter );

//	printf("CTRL ALL_IN_ONE_numActive: Rank: %d --- %d, %d, %f\n", hit_Rank, procsCard, blocksCard, extraParameter );
	return 1;
}


/* A.4 GENERIC FUNCTIONS TO TRASNFORM TOPOLOGY RANKS TO ACTIVE RANKS AND VICEVERSA */
/* A.4.1. GENERIC REGULAR CONTIGUOUS: REGULAR (BLOCKING/CYCLIC) WITH NON-ACTIVE AT THE END */
int	hit_layout_plug_layRegularContiguos_ranks( char topoActiveMode, 
							int procId, int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( extraParameter );

	/* MODE: TOPO TO ACTIVE */
	if ( topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE ) {
		/* GET NUM ACTIVES, ASSERT: procId IS ACTIVE */
		int actives = hit_min( blocksCard, procsCard );
		if ( procId < 0 || procId > actives ) return HIT_RANK_NULL;
		else return procId;
	}
	/* MODE: ACTIVE TO TOPO */
	else return procId;

}

/* A.4.2. GENERIC REGULAR F: REGULAR (BLOCKING/CYCLIC) WITH GROUPS, FIRST IS LEADER */
int	hit_layout_plug_layRegularF_ranks( char topoActiveMode, 
							int procId, int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( extraParameter );

	/* MODE INDEPENDENT */
	if ( blocksCard >= procsCard ) {
		if ( procId < 0 || procId > procsCard ) return HIT_RANK_NULL;
		else return procId;
	}

	/* MODE: TOPO TO ACTIVE */
	if ( topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE ) 
		return (int)(procId * (double)blocksCard / procsCard );
	/* MODE: ACTIVE TO TOPO */
	else return (int)ceil( procId * (double)procsCard / blocksCard );
}

/* A.4.3. GENERIC REGULAR L: REGULAR (BLOCKING/CYCLIC) WITH GROUPS, LAST IS LEADER */
int	hit_layout_plug_layRegularL_ranks( char topoActiveMode, 
							int procId, int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( extraParameter );

	/* MODE INDEPENDENT */
	if ( blocksCard >= procsCard ) {
		if ( procId < 0 || procId > procsCard ) return HIT_RANK_NULL;
		else return procId;
	}

	/* MODE: TOPO TO ACTIVE */
	if ( topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE ) 
		return (int)(procId * (double)blocksCard / procsCard );
	/* MODE: ACTIVE TO TOPO */
	else return (int)ceil( (procId+1) * (double)procsCard / blocksCard ) - 1;
}

/* A.4.4. GENERIC ALL IN ONE: ONLY ONE ACTIVE (LEADER) */
int	hit_layout_plug_layAllInOne_ranks( char topoActiveMode, 
							int procId, int procsCard, int blocksCard, float* extraParameter ) {
	HIT_NOT_USED( topoActiveMode );
	HIT_NOT_USED( procsCard );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( extraParameter );

	/* MODE: TOPO TO ACTIVE */
	/* MODE: ACTIVE TO TOPO */
	if ( procId != 0 ) return HIT_RANK_NULL;
	else return 0;
}




/* 1.0. BASIC LAYOUT, COPY IN ALL: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layCopy(int freeTopo, HitTopology topo, HitShape shape ) {
	HitLayout res;

	/* ACTIVE */
	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layCopy_Sig),
				&(hit_layout_plug_layCopy_SigInv),
				NULL,
				NULL,
				&(hit_layout_plug_layRegularContiguos_ranks),
				NULL,
				&(hit_layout_plug_layCopy_maxCard),
				NULL,
				&(hit_layout_plug_layCopy_minCard),
				NULL,
				&(hit_layout_plug_layRegular_numActives),
				NULL,
				NULL, //&extraParameterValue,
				HIT_LAYOUT_NODIM
			);
	
	res.type = HIT_LAYOUT_COPY;

	if(! res.active) {
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if(  res.active  ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId( res, group );
	*/
	res.group = 0;

	// @arturo: 2019-09-04 Copy layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}



/* 1. LAYOUT (SIGNATURES): BLOCKS */
/* 1.1. BLOCKS: SIGNATURE */
int	hit_layout_plug_layBlocks_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	HIT_NOT_USED( extraParameter );

	/* REJECT EMPTY INPUT SIGNATURES */
	if ( blocksCard == 0 ) {
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	int begin = HIT_LAY_SIG_BEGIN(procId);

	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	if ( begin >= blocksCard ) {
		/* RETURN NUL SIGNATURE AND NON-ACTIVE */
		(*res) = HIT_SIG_NULL;
		return 0;
	}
	else {
		/* BEGIN */
		(*res).begin = begin * input.stride + input.begin;

		/* END */
		(*res).end = (HIT_LAY_SIG_BEGIN( procId + 1 )-1)
					* input.stride + input.begin;

		/* STRIDE */
		(*res).stride = input.stride;
	}

	/* RETURN ACTIVE */
	return 1;
}
int	hit_layout_plug_layBlocks_SigInv(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input,
												int ind ) {
	HIT_NOT_USED( procId );
	HIT_NOT_USED( extraParameter );

	/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
	if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;

	/* RETURN PROC ID */
	int tileInd = (ind - input.begin) / input.stride;
	int id = tileInd * hit_min(blocksCard, procsCard) / blocksCard;
	if ( (id+1) * blocksCard / hit_min(blocksCard, procsCard) == tileInd ) return id+1;
	else return id;
}


/* 1.4. BLOCKS: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layBlocks(int freeTopo, HitTopology topo, HitShape shape) {
	HitLayout res;
	//float extraParameterValue = 0.0;

	/* ACTIVE */
	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layBlocks_Sig),
				&(hit_layout_plug_layBlocks_SigInv),
				NULL,
				NULL,
				&(hit_layout_plug_layRegularContiguos_ranks),
				NULL,
				&(hit_layout_plug_layRegular_maxCard),
				NULL,
				&(hit_layout_plug_layRegular_minCard),
				NULL,
				&(hit_layout_plug_layRegular_numActives),
				NULL,
				NULL, //&extraParameterValue,
				HIT_LAYOUT_NODIM
			);
	
	res.type = HIT_LAYOUT_BLOCKS;

	if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if(  res.active  ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId( res, group );
	*/
	res.group = 0;

	// @arturo: 2019-09-04 Block layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}


/* 1.4.S. BLOCKS IN A RESTRICTED DIMENSION: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layDimBlocks(int freeTopo, HitTopology topo, HitShape shape, int restrictDim ) {
	HitLayout res;
	//float extraParameterValue = 0.0;

	/* ACTIVE */
	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layCopy_Sig),
				&(hit_layout_plug_layCopy_SigInv),
				&(hit_layout_plug_layBlocks_Sig),
				&(hit_layout_plug_layBlocks_SigInv),
				&(hit_layout_plug_layRegularContiguos_ranks),
				&(hit_layout_plug_layRegularContiguos_ranks),
				&(hit_layout_plug_layCopy_maxCard),
				&(hit_layout_plug_layRegular_maxCard),
				&(hit_layout_plug_layCopy_minCard),
				&(hit_layout_plug_layRegular_minCard),
				&(hit_layout_plug_layRegular_numActives),
				&(hit_layout_plug_layRegular_numActives),
				NULL, //&extraParameterValue,
				restrictDim
			);
	
	res.type = HIT_LAYOUT_DIMBLOCKS;

	if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if(  res.active  ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId( res, group );
	*/
	res.group = 0;

	// @arturo: 2019-09-04 DimBlock layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

/* 1.2.1. BLOCKS WITH MINIMUM: SIGNATURE */
int	hit_layout_plug_layMinBlocks_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	int minimum = (int)*extraParameter;

	/* REJECT EMPTY INPUT SIGNATURES */
	if ( blocksCard == 0 ) {
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	/* COMPUTE SIZE */
	// INTEGER DIVISION: DETECT IF ANYONE COULD RECEIVE LESS THAN MINIMUM
	int size = blocksCard / hit_min(blocksCard, procsCard);
	if ( size < minimum ) {
		/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
		int begin = procId * minimum;
		if ( begin >= blocksCard ) {
			/* RETURN NUL SIGNATURE AND NON-ACTIVE */
			(*res) = HIT_SIG_NULL;
			return 0;
		}
		/* COMPUTE SIGNATURE */
		(*res).begin = begin * input.stride + input.begin;
		(*res).end = ( begin + minimum ) * input.stride + input.begin;
		(*res).stride = input.stride;
		return 1;
	}
	else
		//return hit_layout_plug_layBlocks_Sig(procId, procsCard, blocksCard, 0.0f, input, res );
		return hit_layout_plug_layBlocks_Sig(procId, procsCard, blocksCard, NULL, input, res );
}
int	hit_layout_plug_layMinBlocks_SigInv(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input,
												int ind ) {
	HIT_NOT_USED( procId );
	int minimum = (int)*extraParameter;

	/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
	if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;

	/* COMPUTE SIZE */
	// INTEGER DIVISION: DETECT IF ANYONE CAN RECEIVE LESS THAN MINIMUM
	int size = blocksCard / hit_min(blocksCard, procsCard);

	/* RETURN PROC ID */
	int tileInd = (ind - input.begin) / input.stride;
	if ( size < minimum ) 
		return tileInd / minimum;
	else 
		//return hit_layout_plug_layBlocks_SigInv(procId, procsCard, blocksCard, 0.0, input, ind );
		return hit_layout_plug_layBlocks_SigInv(procId, procsCard, blocksCard, NULL, input, ind );
}

/* 1.2.2. BLOCKS WITH MINIMUM: maxCard */
int	hit_layout_plug_layMinBlocks_maxCard( int procsCard, int blocksCard, float* extraParameter ) {
	int minimum = (int)*extraParameter;

	int regularMaxSize = hit_layout_plug_layRegular_maxCard( procsCard, blocksCard, extraParameter );
	if ( regularMaxSize < minimum ) return minimum;
	else return regularMaxSize;
}
/* 1.2.3. BLOCKS WITH MINIMUM: minCard */
int	hit_layout_plug_layMinBlocks_minCard( int procsCard, int blocksCard, float* extraParameter ) {
	int minimum = (int)*extraParameter;

	int regularMinSize = hit_layout_plug_layRegular_minCard( procsCard, blocksCard, extraParameter );
	if ( regularMinSize < minimum ) return minimum;
	else return regularMinSize;
}
/* 1.2.4. BLOCKS WITH MINIMUM: numActives */
int	hit_layout_plug_layMinBlocks_numActives( int procsCard, int blocksCard, float* extraParameter ) {
	int minimum = (int)*extraParameter;
	return hit_min( procsCard, blocksCard/minimum );
}
/* 1.2.5. BLOCKS WITH MINIMUM: NON-ACTIVE AT THE END */
int	hit_layout_plug_layMinBlocks_ranks( char topoActiveMode, 
							int procId, int procsCard, int blocksCard, float* extraParameter ) {
	/* MODE: TOPO TO ACTIVE */
	if ( topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE ) {
		/* GET NUM ACTIVES, ASSERT: procId IS ACTIVE */
		int actives = hit_layout_plug_layMinBlocks_numActives( procsCard, blocksCard, extraParameter );
		if ( procId < 0 || procId > actives ) return HIT_RANK_NULL;
		else return procId;
	}
	/* MODE: ACTIVE TO TOPO */
	else return procId;
}


/* 1.4.M. BLOCKS WITH A MINIMUM SIZE: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layMinBlocks(int freeTopo, HitTopology topo, HitShape shape, int minElems ) {
	HitLayout res;
	float extraParameterValue = (float)minElems;

	/* ACTIVE */
	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layMinBlocks_Sig),
				&(hit_layout_plug_layMinBlocks_SigInv),
				NULL,
				NULL,
				&(hit_layout_plug_layMinBlocks_ranks),
				NULL,
				&(hit_layout_plug_layMinBlocks_maxCard),
				NULL,
				&(hit_layout_plug_layMinBlocks_minCard),
				NULL,
				&(hit_layout_plug_layMinBlocks_numActives),
				NULL,
				&extraParameterValue,
				/* TODO: ?????? */
				HIT_LAYOUT_NODIM
			);
	
	res.type = HIT_LAYOUT_MINBLOCKS;

	if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if(  res.active  ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId( res, group );
	*/
	res.group = 0;

	// @arturo: 2019-09-04 MinBlock layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}


/* 1.X LAYOUT (SIGNATURES): BLOCKS NO REPEATED, NON-ACTIVES AT THE END 
 * 		THIS IS AN ALTERNATIVE IMPLEMENTATION TO THE CURRENT BLOCKS LAYOUT */
/* 1.1. BLOCKS X: SIGNATURE */
int	hit_layout_plug_layBlocksX_Sig(int procId, int procsCard, int blocksCard, HitSig input, HitSig *res ) {
	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	if ( blocksCard <= procsCard ) {
		if ( procId >= blocksCard ) {
			/* RETURN NUL SIGNATURE AND NON-ACTIVE */
			(*res) = HIT_SIG_NULL;
			return 0;
		}
		else {
			/* ACTIVE ELEMENT */
			(*res).begin = procId;
			(*res).end = procId;
			(*res).stride = 1;
		}
	}
	/* MORE LOGICAL PROCESSES THAN VIRTUAL PROCESSORS */
	else {
		int begin = HIT_LAY_SIG_BEGIN(procId);

		/* BEGIN */
		(*res).begin = begin * input.stride + input.begin;

		/* END */
		(*res).end = (HIT_LAY_SIG_BEGIN( procId + 1 )-1)
					* input.stride + input.begin;

		/* STRIDE */
		(*res).stride = input.stride;
	}

	/* RETURN ACTIVE */
	return 1;
}


/* 1.4. BLOCKS: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layBlocksX(int freeTopo, HitTopology topo, HitShape shape) {

	/* CHECK IF ACTIVE */
	HitLayout res;
	//float extraParameterValue = 0.0;

	/* ACTIVE */
	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layBlocks_Sig),
				&(hit_layout_plug_layBlocks_SigInv),
				NULL,
				NULL,
				&(hit_layout_plug_layRegularContiguos_ranks),
				NULL,
				&(hit_layout_plug_layRegular_maxCard),
				NULL,
				&(hit_layout_plug_layRegular_minCard),
				NULL,
				&(hit_layout_plug_layRegular_numActives),
				NULL,
				NULL, //&extraParameterValue,
				HIT_LAYOUT_NODIM
			);

	res.type = HIT_LAYOUT_BLOCKSX;

	if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if(  res.active  ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}

	// Transform ranks to process id
	res.group =  hit_layActiveRanksId(res,group);
	*/

	// @arturo: 2019-09-04 BlocksX layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}


/* 2. LAYOUT (SIGNATURES): BLOCK LOCATED ON GROUP LEADERS (First processor is leader) */
/* 2.1. BLOCKSF: SIGNATURE */
int	hit_layout_plug_layBlocksF_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	HIT_NOT_USED( extraParameter );

	/* COMPUTE BLOCK SIZE AND STARTING POINT */
	double ratio = (double)blocksCard / procsCard;
	double beginFrac = procId * ratio;
	int begin = (int)beginFrac;

	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	if ( floor(beginFrac) == floor(beginFrac-ratio) ) {
		/* RETURN NUL SIGNATURE AND NON-ACTIVE */
		(*res) = HIT_SIG_NULL;
		return 0;
	}
	else {
		/* COMPUTE SIGNATURE */
		/* BEGIN */
		(*res).begin = begin * input.stride + input.begin;

		/* END */
		int adjust = (blocksCard > procsCard) ? 1 : 0;
		int end = ((procId + adjust) * blocksCard / procsCard ) - adjust;
		(*res).end = end * input.stride + input.begin;

		/* STRIDE */
		(*res).stride = input.stride;
	}

	/* RETURN ACTIVE */
	return 1;
}

/* 2.4. BLOCKSF: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layBlocksF(int freeTopo, HitTopology topo, HitShape shape) {

	HitLayout res;
	//HitRanks leader;
	//float extraParameterValue = 0.0;

	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layBlocksF_Sig),
				&(hit_layout_plug_layBlocks_SigInv),
				NULL,
				NULL,
				&(hit_layout_plug_layRegularF_ranks),
				NULL,
				&(hit_layout_plug_layRegular_maxCard),
				NULL,
				&(hit_layout_plug_layRegular_minCard),
				NULL,
				&(hit_layout_plug_layRegular_numActives),
				NULL,
				NULL, //&extraParameterValue, //0.0,
				HIT_LAYOUT_NODIM
			);

	res.type = HIT_LAYOUT_BLOCKSF;

	if(! res.active ){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}


	/* MY GROUP & MY LEADER */
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		double ratio = (double) hit_sigCard(hit_shapeSig(shape,dim)) / topo.card[dim];
		double ratioInv = (double) topo.card[dim] / hit_sigCard(hit_shapeSig(shape,dim));

		if(  ratio >= 1  ){
			group.rank[dim] = topo.self.rank[dim];
			//leader.rank[dim] = group.rank[dim];
			res.leaderRanks.rank[dim] = group.rank[dim];
		} else {
			group.rank[dim] = (int)(topo.self.rank[dim] * ratio);
			//leader.rank[dim] =  (int) ceil(ratioInv * group.rank[dim]);
			res.leaderRanks.rank[dim] =  (int) ceil(ratioInv * group.rank[dim]);
		}
	}

	res.group =  hit_layActiveRanksId(res,group);
	//res.leader = hit_topRankInternal(topo,leader);
	// @arturo 2015/01/22
	//res.leader = ( hit_topRankInternal(topo,res.leaderRanks) == topo.linearRank ) ? 1 : 0;
	res.leader = ( hit_topRankInternal(topo,res.leaderRanks) == 
						hit_topSelfRankInternal( topo ) ) ? 1 : 0 ;

	/* RETURN */
	if( freeTopo ) hit_topFree( topo );
	return res;
}


/* 3. LAYOUT (SIGNATURES): BLOCK LOCATED AT LAST ELEMENT OF GROUP (Last processor is leader) */
/* 3.1. BLOCKSL: SIGNATURE */
int	hit_layout_plug_layBlocksL_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	HIT_NOT_USED( extraParameter );

	/* COMPUTE BLOCK SIZE AND STARTING POINT */
	double ratio = (double)blocksCard / procsCard;
	double beginFrac = procId * ratio;
	int begin = (int)beginFrac;

#ifdef DEBUG_HITLIB
	printf("CTRL procId: %d, procsCard %d, blocksCard: %d\n", procId, procsCard, blocksCard);
	printf("CTRL Input[%d:%d:%d]\n", 
				input.begin,
				input.end,
				input.stride
			);
#endif


	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	//if ( (beginFrac+ratio) != floor(beginFrac+ratio) ) { // Old
	//if( floor(beginFrac) == floor(beginFrac-ratio) ) { // BlocksF
	if( floor(beginFrac) == floor(beginFrac+ratio) ) {
		/* RETURN NUL SIGNATURE AND NON-ACTIVE */
		(*res) = HIT_SIG_NULL;
		return 0;
	}
	else {
		/* COMPUTE SIGNATURE */
		/* BEGIN */
		(*res).begin = begin * input.stride + input.begin;

		/* END */
		int adjust = (blocksCard > procsCard) ? 1 : 0;
		int end = ((procId + adjust) * blocksCard / procsCard ) - adjust;
		(*res).end = end * input.stride + input.begin;

		/* STRIDE */
		(*res).stride = input.stride;
	}

	/* RETURN ACTIVE */
	return 1;
}



/* 3.4. BLOCKSL: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layBlocksL(int freeTopo, HitTopology topo, HitShape shape) {

	HitLayout res;
	//HitRanks leader;
	//float extraParameterValue = 0.0;

	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layBlocksL_Sig),
				&(hit_layout_plug_layBlocks_SigInv),
				NULL,
				NULL,
				&(hit_layout_plug_layRegularL_ranks),
				NULL,
				&(hit_layout_plug_layRegular_maxCard),
				NULL,
				&(hit_layout_plug_layRegular_minCard),
				NULL,
				&(hit_layout_plug_layRegular_numActives),
				NULL,
				NULL, //&extraParameterValue, //0.0,
				HIT_LAYOUT_NODIM
			);
	
	res.type = HIT_LAYOUT_BLOCKSL;

	if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	/* MY GROUP & MY LEADER */
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		double ratio = (double) hit_sigCard(hit_shapeSig(shape,dim)) / topo.card[dim];
		double ratioInv = (double) topo.card[dim] / hit_sigCard(hit_shapeSig(shape,dim));

		if(  ratio >= 1  ){
			group.rank[dim] = topo.self.rank[dim];
			//leader.rank[dim] = group.rank[dim];
			res.leaderRanks.rank[dim] = group.rank[dim];
		} else {
			group.rank[dim] = (int)(topo.self.rank[dim] * ratio);
			//leader.rank[dim] =  ((int) ceil(ratioInv * (group.rank[dim]+1)) ) -1;
			res.leaderRanks.rank[dim] =  ((int) ceil(ratioInv * (group.rank[dim]+1)) ) -1;
		}

	}
	res.group =  hit_layActiveRanksId(res,group);
	//res.leader = hit_topRankInternal(topo,leader);
	// @arturo 2015/01/22
	//res.leader = ( hit_topRankInternal(topo,res.leaderRanks) == topo.linearRank ) ? 1 : 0;
	res.leader = ( hit_topRankInternal(topo,res.leaderRanks) == 
						hit_topSelfRankInternal( topo ) ) ? 1 : 0 ;

	/* RETURN */
	if( freeTopo ) hit_topFree( topo );
	return res;
}


/* 4. LAYOUT (SIGNATURES): CYCLIC */
/* 4.1. CYCLIC: SIGNATURE */
int	hit_layout_plug_layCyclic_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {
	HIT_NOT_USED( extraParameter );

	/* NOT ENOUGH INPUT DOMAIN ELEMENTS FOR THIS PROC ID */
	if ( blocksCard <= procId ) {
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	/* BEGIN */
	(*res).begin = input.begin + input.stride * procId;

	/* END */
	int tmp = (blocksCard / procsCard - 1) * procsCard + procId;
	if ( procId < blocksCard % procsCard ) tmp = tmp + procsCard;
	(*res).end = input.begin + tmp * input.stride;

	/* STRIDE */
	(*res).stride = input.stride * procsCard;

	/* RETURN ACTIVE */
	return 1;
}
int	hit_layout_plug_layCyclic_SigInv(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input,
												int ind ) {
	HIT_NOT_USED( procId );
	HIT_NOT_USED( blocksCard );
	HIT_NOT_USED( extraParameter );

	/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
	if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;

	/* RETURN PROC ID */
	int tileInd = (ind - input.begin) / input.stride;
	return tileInd % procsCard;
}


/* 4.4. CYCLIC: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layCyclic(int freeTopo, HitTopology topo, HitShape shape, int restrictToDim) {

	//float extraParameterValue = 0.0;

	HitLayout res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layCyclic_Sig),
				&(hit_layout_plug_layCyclic_SigInv),
				&(hit_layout_plug_layCopy_Sig),
				&(hit_layout_plug_layCopy_SigInv),
				&(hit_layout_plug_layRegularContiguos_ranks),
				&(hit_layout_plug_layRegularContiguos_ranks),
				&(hit_layout_plug_layRegular_maxCard),
				&(hit_layout_plug_layCopy_maxCard),
				&(hit_layout_plug_layRegular_minCard),
				&(hit_layout_plug_layCopy_minCard),
				&(hit_layout_plug_layRegular_numActives),
				&(hit_layout_plug_layRegular_numActives),
				NULL, //&extraParameterValue, //0.0,
				restrictToDim
			);
	
	res.type = HIT_LAYOUT_CYCLIC;

	/* MY GROUP & MY LEADER */
	if ( ! res.active ){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}


	//HitRanks leader;

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if ( restrictToDim != dim ) { 
			group.rank[dim] = 0;
			//leader.rank[dim] = 0;
			res.leaderRanks.rank[dim] = 0;
		}
		else {
			group.rank[dim] = topo.self.rank[dim];
			//leader.rank[dim] = topo.self.rank[dim];
			res.leaderRanks.rank[dim] = topo.self.rank[dim];
		}
	}
	res.group =  hit_layActiveRanksId(res,group);
	*/
	res.group = 0;

	// @arturo: 2019-09-04 Cyclic layouts, the leader is always the topology leader
	/*
	//res.leader = hit_topRankInternal(topo,leader);
	// @arturo 2015/01/22
	//res.leader = ( hit_topRankInternal(topo,res.leaderRanks) == topo.linearRank ) ? 1 : 0;
	res.leader = ( hit_topRankInternal(topo,res.leaderRanks) == 
						hit_topSelfRankInternal( topo ) ) ? 1 : 0;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	/* RETURN */
	if( freeTopo ) hit_topFree( topo );
	return res;
}



/* 5. ALL THE STRUCTURE IN THE LEADER: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layInLeader(int freeTopo, HitTopology topo, HitShape shape) {
	HitLayout res;
	//float extraParameterValue = 0.0;

	res = hit_layout_wrapper(
			topo,
			shape,
			&(hit_layout_plug_layAllInOne_Sig),
			&(hit_layout_plug_layAllInOne_SigInv),
			NULL,
			NULL,
			&(hit_layout_plug_layAllInOne_ranks),
			NULL,
			&(hit_layout_plug_layCopy_maxCard),
			NULL,
			&(hit_layout_plug_layCopy_minCard),
			NULL,
			&(hit_layout_plug_layAllInOne_numActives),
			NULL,
			NULL, //&extraParameterValue, //0.0,
			HIT_LAYOUT_NODIM
		);
	
	res.type = HIT_LAYOUT_ALLINLEADER;

	if(! res.active) {
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		group.rank[dim] = 0;
	}
	res.group =  hit_layActiveRanksId( res, group );
	*/
	res.group = 0;

	// @arturo: 2019-09-04 InLeader layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

/* 8.  LAYOUT (SIGNATURES): BLOCKS DIM WEIGHTED TO RESTRICTED DIM */
/* 8.1. BLOCKS DIM WEIGHTED: SIGNATURE */
int hit_layout_plug_layDimBlocksWeighted_Sig(int procId, int procsCard, int blocksCard, float *extraParam, HitSig input, HitSig *res)
{
	HIT_NOT_USED(procsCard);

	/* REJECT EMPTY INPUT SIGNATURES */
	if (blocksCard <= 0)
	{
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	/* Load ratios */
	HitWeights *load_ratios = (HitWeights *)extraParam;

	if (procId < 0 || procId >= load_ratios->num_procs)
	{
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	int begin = (int)(load_ratios->ratios[procId] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard);
	int end = ((int)(load_ratios->ratios[procId + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard)) - 1;

	if (end - begin < 0)
	{
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	/* BEGIN */
	(*res).begin = begin * input.stride + input.begin;

	/* END */
	(*res).end = end * input.stride + input.begin;

	/* STRIDE */
	(*res).stride = input.stride;

	/* RETURN ACTIVE */
	return 1;
}
int hit_layout_plug_layDimBlocksWeighted_SigInv(int procId, int procsCard, int blocksCard,
										   float *extraParameter, HitSig input, int ind)
{
	HIT_NOT_USED(procId);
	HIT_NOT_USED(procsCard);

	/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
	if (!hit_sigIn(input, ind))
		return HIT_RANK_NULL;

	/* REJECT EMPTY INPUT SIGNATURES */
	if (blocksCard <= 0)
		return HIT_RANK_NULL;

	/* Load ratios */
	HitWeights *load_ratios = (HitWeights *)extraParameter;

	float *weights = load_ratios->ratios;

	/* FIND WEIGHT */
	int tileInd = (ind - input.begin) / input.stride;
	float weightAprox = (float)tileInd * weights[load_ratios->num_procs] / (float)blocksCard;

	/* BIN-LIKE	SEARCH IN ARRAY OF WEIGHTS (SORTED)*/
	int l = 0, r = load_ratios->num_procs - 1, c;
	int proc = 0;

	while (l <= r)
	{
		c = (l + r) / 2;
		proc = c;

		if (weightAprox >= weights[c] && weightAprox < weights[c + 1])
			break;

		if (weightAprox < weights[c])
			r = c - 1;
		else
			l = c + 1;
	}

	/* SKIP PROCS WITH 0 BLOCKS */
	int i = proc + 1;
	while (i < load_ratios->num_procs)
	{

		int begin =(int)(weights[i] / weights[load_ratios->num_procs] * (float)blocksCard);
		int end = ((int)(weights[i + 1] / weights[load_ratios->num_procs] * (float)blocksCard)) - 1;

		if (end - begin >= 0)
			break;
		i++;
	}

	/* RETURN PROC */
	/* IF PRECISION WAS LOST, AND THE INDEX IS AT THE BEGINNING OF THE NEXT PROC(NOT EMPTY) */
	if ((int)(weights[i]/ weights[load_ratios->num_procs] * (float)blocksCard ) == tileInd)
		return i;
	else
		return proc;
}

/* 8.2.A BLOCKS DIM WEIGHTED: MAX CARDINALITY */
int hit_layout_plug_layDimBlocksWeighted_maxCard(int procsCard, int blocksCard, float *extraParameter)
{
	HIT_NOT_USED(procsCard);

	/* REJECT EMPTY INPUT SIGNATURES */
	if (blocksCard <= 0)
		return 0;

	/* Load ratios */
	HitWeights *load_ratios = (HitWeights *)extraParameter;

	float threshold = load_ratios->ratios[load_ratios->num_procs] / (float)blocksCard;
	int i, proc = 0;
	float max = 0;

	/* Find the maximum ratio (max Card) */
	for (i = 0; i < load_ratios->num_procs; i++)
	{
		if (load_ratios->ratios[i + 1] - load_ratios->ratios[i] + threshold >= max)
		{
			if ((int)(load_ratios->ratios[i + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) -
					(int)(load_ratios->ratios[i] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) >
				(int)(load_ratios->ratios[proc + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) -
					(int)(load_ratios->ratios[proc] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard))
			{
				max = load_ratios->ratios[i + 1] - load_ratios->ratios[i];
				proc = i;
			}
	
		}
	}

	return (int)(load_ratios->ratios[proc + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) -
		   (int)(load_ratios->ratios[proc] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard);
	;
}

/* 8.2.B BLOCKS DIM WEIGHTED: MIN CARDINALITY */
int hit_layout_plug_layDimBlocksWeighted_minCard(int procsCard, int blocksCard, float *extraParameter)
{
	HIT_NOT_USED(procsCard);

	/* REJECT EMPTY INPUT SIGNATURES */
	if (blocksCard <= 0)
		return 0;

	/* Load ratios */
	HitWeights *load_ratios = (HitWeights *)extraParameter;

	if (load_ratios->num_procs <= 0)
		return 0;

	float threshold = load_ratios->ratios[load_ratios->num_procs] / (float)blocksCard;
	int i = 0, proc = 0;
	float min = load_ratios->ratios[i + 1] - load_ratios->ratios[i];

	/* Find the minimum ratio (min Card) */
	for (i = 1; i < load_ratios->num_procs; i++)
	{
		if (load_ratios->ratios[i + 1] - load_ratios->ratios[i] - threshold <= min)
		{
			if ((int)(load_ratios->ratios[i + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) -
					(int)(load_ratios->ratios[i] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) <
				(int)(load_ratios->ratios[proc + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) -
					(int)(load_ratios->ratios[proc] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard))
			{
				min = load_ratios->ratios[i + 1] - load_ratios->ratios[i];
				proc = i;
			}
	
		}
	}

	return (int)(load_ratios->ratios[proc + 1] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard) -
		   (int)(load_ratios->ratios[proc] / load_ratios->ratios[load_ratios->num_procs] * (float)blocksCard);
}

/* 8.3 BLOCKS DIM WEIGHTED: RANKS */
int hit_layout_plug_layDimBlocksWeighted_ranks(char topoActiveMode,
										  int procId, int procsCard, int blocksCard, float *extraParameter)
{
	HIT_NOT_USED(procsCard);

	/* REJECT EMPTY INPUT SIGNATURES */
	if (blocksCard <= 0)
		return HIT_RANK_NULL;

	/* Load ratios */
	HitWeights *load_ratios = (HitWeights *)extraParameter;

	if (procId < 0 || procId >= load_ratios->num_procs)
		return HIT_RANK_NULL;

	float *weights = load_ratios->ratios;

	int i = 0, num_proc = -1, begin, end;

	/* MODE: TOPO TO ACTIVE */
	if (topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE)
	{
		for (i = 0; i <= procId; i++)
		{
			begin = (int) (weights[i] / weights[load_ratios->num_procs] * (float)blocksCard);
			end = ((int)(weights[i + 1] / weights[load_ratios->num_procs] * (float)blocksCard)) - 1;

			if (end - begin >= 0)
				num_proc++;
		}

		if (end - begin < 0)
			return HIT_RANK_NULL;
		else
			return num_proc;
	}
	/* MODE: ACTIVE TO TOPO */
	else
	{
		while (num_proc < procId && i < load_ratios->num_procs)
		{
			begin = (int) (weights[i] / weights[load_ratios->num_procs] * (float)blocksCard);
			end = ((int)(weights[i + 1] / weights[load_ratios->num_procs] * (float)blocksCard)) - 1;

			if (end - begin >= 0)
				num_proc++;
			i++;
		}

		i--; /* To get the last proccess that entered in the loop */

		if (num_proc == procId)
			return i;
		else
			return HIT_RANK_NULL;
	}
}

/* 8.4 BLOCKS DIM WEIGHTED: NUM ACTIVES */
int hit_layout_plug_layDimBlocksWeighted_numActives(int procsCard, int blocksCard, float *extraParameter)
{

	HIT_NOT_USED(procsCard);

	/* REJECT EMPTY INPUT SIGNATURES */
	if (blocksCard <= 0)
		return 0;

	/* Load ratios */
	HitWeights *load_ratios = (HitWeights *)extraParameter;

	float *weights = load_ratios->ratios;

	int i = 0, actives = 0;

	for (i = 0; i < load_ratios->num_procs; i++)
	{
			int begin = (int) (weights[i] / weights[load_ratios->num_procs] * (float)blocksCard);
			int end = ((int)(weights[i + 1] / weights[load_ratios->num_procs] * (float)blocksCard)) - 1;

		if (end - begin >= 0)
			actives++;
	}

	return actives;
}

/* 8.5 BLOCKS DIM WEIGHTED: LAYOUT FUNCTION INTERFACE */
HitLayout hit_layout_plug_layDimWeighted_Copy(int freeTopo, HitTopology topo, HitShape shape, int restrictDim, HitWeights l_ratios)
{
	int i = 0;
	HitLayout res;

	// Struct for the actual store of the ratios
	HitWeights *current_ld = (HitWeights *)malloc(sizeof(HitWeights));

	// Compute the actual number of procs in the restricted dim
	current_ld->num_procs = topo.card[restrictDim] < l_ratios.num_procs ? topo.card[restrictDim] : l_ratios.num_procs;
	current_ld->ratios = (float *)malloc(sizeof(float) * (unsigned long)(current_ld->num_procs + 1));

	current_ld->ratios[0] = 0;

	// Compute the accumulate ratios
	for (i = 0; i < current_ld->num_procs; i++)
	{
		current_ld->ratios[i + 1] = current_ld->ratios[i] + l_ratios.ratios[i];
	}

	/* ACTIVE */
	res = hit_layout_wrapper(
		topo,
		shape,
		&(hit_layout_plug_layCopy_Sig),
		&(hit_layout_plug_layCopy_SigInv),
		&(hit_layout_plug_layDimBlocksWeighted_Sig),
		&(hit_layout_plug_layDimBlocksWeighted_SigInv),
		&(hit_layout_plug_layRegularContiguos_ranks),
		&(hit_layout_plug_layDimBlocksWeighted_ranks),
		&(hit_layout_plug_layCopy_maxCard),
		&(hit_layout_plug_layDimBlocksWeighted_maxCard),
		&(hit_layout_plug_layCopy_minCard),
		&(hit_layout_plug_layDimBlocksWeighted_minCard),
		&(hit_layout_plug_layRegular_numActives),
		&(hit_layout_plug_layDimBlocksWeighted_numActives),
		(float *)current_ld,
		restrictDim);

	res.type = HIT_LAYOUT_DIMWEIGHTED_AND_COPY;

	if (!res.active)
	{
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	/* MY GROUP */
	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim = 0; dim < hit_shapeDims(shape); dim++)
	{
		if (res.active)
		{
			group.rank[dim] = topo.self.rank[dim];
		}
		else
		{
			group.rank[dim] = -1;
		}
	}
	res.group = hit_layActiveRanksId(res, group);
	*/

	/* MY LEADER */
	// @arturo: 2019-09-04 The leader is always the topology leader
	/*
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

/* 9.  LAYOUT (SIGNATURES): BLOCKS WEIGHTED IN SELECTED DIM, BLOCKS CONSTANT IN OTHER DIMS */
HitLayout hit_layout_plug_layDimWeighted_Blocks(int freeTopo, HitTopology topo, HitShape shape, int restrictDim, HitWeights l_ratios)
{
	int i = 0;
	HitLayout res;

	// Struct for the actual store of the ratios
	HitWeights *current_ld = (HitWeights *)malloc(sizeof(HitWeights));

	// Compute the actual number of procs in the restricted dim
	current_ld->num_procs = topo.card[restrictDim] < l_ratios.num_procs ? topo.card[restrictDim] : l_ratios.num_procs;
	current_ld->ratios = (float *)malloc(sizeof(float) * (unsigned long)(current_ld->num_procs + 1));

	current_ld->ratios[0] = 0;

	// Compute the accumulate ratios
	for (i = 0; i < current_ld->num_procs; i++)
	{
		current_ld->ratios[i + 1] = current_ld->ratios[i] + l_ratios.ratios[i];
	}

	/* ACTIVE */
	res = hit_layout_wrapper(
		topo,
		shape,
		&(hit_layout_plug_layBlocks_Sig),
		&(hit_layout_plug_layBlocks_SigInv),
		&(hit_layout_plug_layDimBlocksWeighted_Sig),
		&(hit_layout_plug_layDimBlocksWeighted_SigInv),
		&(hit_layout_plug_layRegularContiguos_ranks),
		&(hit_layout_plug_layDimBlocksWeighted_ranks),
		&(hit_layout_plug_layRegular_maxCard),
		&(hit_layout_plug_layDimBlocksWeighted_maxCard),
		&(hit_layout_plug_layRegular_minCard),
		&(hit_layout_plug_layDimBlocksWeighted_minCard),
		&(hit_layout_plug_layRegular_numActives),
		&(hit_layout_plug_layDimBlocksWeighted_numActives),
		(float *)current_ld,
		restrictDim);

	res.type = HIT_LAYOUT_DIMWEIGHTED_AND_BLOCKS;

	if (!res.active)
	{
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	/* MY GROUP */
	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/*
	HitRanks group;
	int dim;
	for (dim = 0; dim < hit_shapeDims(shape); dim++)
	{
		if (res.active)
		{
			group.rank[dim] = topo.self.rank[dim];
		}
		else
		{
			group.rank[dim] = -1;
		}
	}
	res.group = hit_layActiveRanksId(res, group);
	*/

	/* MY LEADER */
	// @arturo: 2019-09-04 BlocksX layouts, the leader is always the topology leader
	/*
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

/* 10. LAYOUT (SIGNATURES): BLOCKS WITH LOAD BALANCE IN ONE DIMENSION */
/* 10.1. BLOCKS BALANCE: SIGNATURE */
int	hit_layout_plug_layBlocksBalance_Sig(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input, HitSig *res ) {

	/* IF LESS THAN TWO PROCS. RETURN FULL SIGNATURE */
	if ( procsCard < 2 && procId == 0 ) {
		(*res) = input;
		return 1;
	}

	/* COMPUTE FIRST BLOCK OF ACTIVE PROCESSORS PART: PROPORTIONAL TO LOAD */
	int firstSize = (int)ceilf(*extraParameter * (float)blocksCard);
	/* COMPUTE LAST ACTIVE PROCESSOR PART: PROPORTIONAL TO INVERSE LOAD */
	int lastSize = blocksCard - firstSize;

	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	int lastProc = hit_min( procsCard-1, firstSize );
	if ( procId > lastProc ) {
		/* RETURN NUL SIGNATURE AND NON-ACTIVE */
		(*res) = HIT_SIG_NULL;
		return 0;
	}

	/* LAST PROCESSOR */
	if ( procId == lastProc ) {
		if ( lastSize == 0 ) {
			(*res) = HIT_SIG_NULL;
			return 0;
		}
		else {
			(*res).begin = input.end - ( lastSize-1 ) * input.stride;
			(*res).end = input.end;
			(*res).stride = input.stride;
		}
		return 1;
	}

	/* REST OF PROCCESORS: CORRECT INPUT END */
	input.end = input.end - lastSize * input.stride;

	/* CALL TO REGULAR BLOCKS PLUG-IN */
	return hit_layout_plug_layBlocks_Sig( procId, lastProc, firstSize, extraParameter, input, res );
}
int	hit_layout_plug_layBlocksBalance_SigInv(int procId, int procsCard, int blocksCard,
												float* extraParameter,
												HitSig input,
												int ind ) {

	/* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
	if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;

	/* IF LESS THAN TWO PROCS. RETURN THE SAME Id */
	if ( procsCard < 2 && procId == 0 ) return 0;

	/* COMPUTE FIRST BLOCK OF ACTIVE PROCESSORS PART: PROPORTIONAL TO LOAD */
	int firstSize = (int)ceilf(*extraParameter * (float)blocksCard);
	/* COMPUTE LAST ACTIVE PROCESSOR PART: PROPORTIONAL TO INVERSE LOAD */
	int lastSize = blocksCard - firstSize;
	int lastProc = hit_min( procsCard-1, firstSize );

	/* REST OF PROCCESORS: CORRECT INPUT END */
	input.end = input.end - lastSize * input.stride;

	/* RETURN PROC ID */
	int tileInd = (ind - input.begin) / input.stride;
	if ( tileInd >= firstSize ) return procsCard-1;
	else return hit_layout_plug_layBlocks_SigInv( procId, lastProc, firstSize, extraParameter, input, ind );
}


/* 10.2. BLOCKS BALANCE: MAX CARDINALITY */
int	hit_layout_plug_layBlocksBalance_maxCard( int procsCard, int blocksCard, float* extraParameter ) {
	/* IF LESS THAN TWO PROCS. RETURN FULL SIGNATURE */
	if ( procsCard < 2 ) return blocksCard;

	/* COMPUTE FIRST BLOCK OF ACTIVE PROCESSORS PART: PROPORTIONAL TO LOAD */
	int firstSize = (int)ceilf(*extraParameter * (float)blocksCard);
	/* COMPUTE LAST ACTIVE PROCESSOR PART: PROPORTIONAL TO INVERSE LOAD */
	int lastSize = blocksCard - firstSize;
	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	int lastProc = hit_min( procsCard-1, firstSize );

	/* COMPUTE MAX CARDS */
	int others = hit_layout_plug_layRegular_maxCard( lastProc, firstSize, extraParameter );
	return (lastSize > others) ? lastSize:others;
}

/* 10.2.b BLOCKS BALANCE: MIN CARDINALITY */
int	hit_layout_plug_layBlocksBalance_minCard( int procsCard, int blocksCard, float* extraParameter ) {
	/* IF LESS THAN TWO PROCS. RETURN FULL SIGNATURE */
	if ( procsCard < 2 ) return blocksCard;

	/* COMPUTE FIRST BLOCK OF ACTIVE PROCESSORS PART: PROPORTIONAL TO LOAD */
	int firstSize = (int)ceilf(*extraParameter * (float)blocksCard);
	/* COMPUTE LAST ACTIVE PROCESSOR PART: PROPORTIONAL TO INVERSE LOAD */
	int lastSize = blocksCard - firstSize;
	/* DETECT NON-ACTIVE VIRTUAL PROCESS (NOT ENOUGH LOGICAL PROCESSES) */
	int lastProc = hit_min( procsCard-1, firstSize );

	/* COMPUTE MAX CARDS */
	int others = hit_layout_plug_layRegular_minCard( lastProc, firstSize, extraParameter );
	return (lastSize < others) ? lastSize:others;
}


/* 10.3. BLOCKS BALANCE: NUM. ACTIVES */
int	hit_layout_plug_layBlocksBalance_numActives( int procsCard, int blocksCard, float* extraParameter ) {
	/* ASSERT: BLOCKS CARD BIGGER THAN 0 */
	if ( blocksCard < 1 ) return 0;

	/* IF LESS THAN TWO PROCS. RETURN ONE PROC. ACTIVE WITH THE FULL SIGNATURE */
	if ( procsCard < 2 ) return 1;

	/* COMPUTE FIRST BLOCK PART: PROPORTIONAL TO LOAD */
	int firstSize = (int)ceilf(*extraParameter * (float)blocksCard);

	/* DEGENERATED CASE */
	if ( firstSize == 0 )  return 1;

	/* GENERAL CASE FOR FIRST SIZE */
	int numActivesFirst = hit_layout_plug_layRegular_numActives( procsCard-1, firstSize, extraParameter );

	/* DEGENERATED CASE */
	if ( firstSize == blocksCard ) return numActivesFirst;
	else return numActivesFirst + 1;
}

/* 10.4. BLOCKS BALANCE: RANKS */
int	hit_layout_plug_layBlocksBalance_ranks( char topoActiveMode, 
							int procId, int procsCard, int blocksCard, float* extraParameter ) {

	/* MODE: TOPO TO ACTIVE */
	if ( topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE ) {
		/* GET NUM ACTIVES, ASSERT: procId IS ACTIVE */
		int actives = hit_layout_plug_layBlocksBalance_numActives( procsCard, blocksCard, extraParameter );
		if ( procId < 0 || procId > actives ) return HIT_RANK_NULL;
		else return procId;
	}
	/* MODE: ACTIVE TO TOPO */
	else return procId;
}


/* 10.5. BLOCKS BALANCE: LAYOUT FUNCTION INTERFACE */
HitLayout	hit_layout_plug_layBlocksBalance(int freeTopo, HitTopology topo, HitShape shape, int restrictToDim, float load ) {

	/* ASSERT: LOAD RANGE [0.0:1.0] */
	if ( load < 0.0 || load > 1.0 ) {
		hit_errInternal( __FUNCTION__, "Incorrect load parameter", "", __FILE__, __LINE__);
	}

	/* CHECK IF ACTIVE */
	HitLayout res;

	/* ACTIVE */
	res = hit_layout_wrapper(
				topo,
				shape,
				&(hit_layout_plug_layBlocks_Sig),
				&(hit_layout_plug_layBlocks_SigInv),
				&(hit_layout_plug_layBlocksBalance_Sig),
				&(hit_layout_plug_layBlocksBalance_SigInv),
				&(hit_layout_plug_layRegularContiguos_ranks),
				&(hit_layout_plug_layBlocksBalance_ranks),
				&(hit_layout_plug_layRegular_maxCard),
				&(hit_layout_plug_layBlocksBalance_maxCard),
				&(hit_layout_plug_layRegular_minCard),
				&(hit_layout_plug_layBlocksBalance_minCard),
				&(hit_layout_plug_layRegular_numActives),
				&(hit_layout_plug_layBlocksBalance_numActives),
				&load, //load,
				restrictToDim
			);
	
	res.type = HIT_LAYOUT_BLOCKS_BALANCE;

	if( ! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if( res.active ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId(res,group);
	*/
	res.group = 0;

	// @arturo: 2019-09-04 Copy layouts, the leader is always the topology leader
	/* MY LEADER */
	/*
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;
	*/
	for( int dim=0; dim<topo.numDims; dim++ ) res.leaderRanks.rank[dim] = 0;
	res.leader = ( hit_topRankInternal( topo, topo.self ) == 0 ) ? 1 : 0;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

int     perform_weighted_distribution(int procsCard, int blocksCard, float* weights, int* result){

        /*SUM WEIGHTS & SAFETY CHECK: NO NEGATIVE LOADS*/
        int i;
        float sumDimWeights = 0;
        for (i = 0; i < procsCard; i++)
                {
                if (weights[i] < 0.0)
                        {hit_errInternal(__FUNCTION__,"Weight input data is incorrect (check workload expressions)","",__FILE__,__LINE__);}
                else
                        {sumDimWeights += weights[i];}
                }

        //If normalization is not required, skip the block below until the END_SKIP comment
        double sumWeights = 0;

        for (i = 0; i < procsCard; i++)
                sumWeights += weights[i];

        /* SAFETY CHECK: CONSISTENCY OF INPUT DATA, TOTAL WEIGHT 0 */
        if (sumWeights == 0.0) sumWeights = 1.0;  /* Avoid division by 0 */
        //END_SKIP

        double floatElemDimProcess[procsCard];  //Number of data elements assigned to process i for this dim
        double remElemDimProcess[procsCard];    //Fractional part of floatElemDimProcess assigned to process i
        int    intElemDimProcess[procsCard];    //Integer part of floatElemDimProcess assigned to process i
        int    sumIntElemDimProcess = 0;

        for (i = 0; i < procsCard; i++)
                {
                //Number of data elements that should be assigned to process i (float)
                floatElemDimProcess[i] = (weights[i] / sumWeights) * blocksCard;
                //If normalization is not required, use the line below instead
                //      floatElemDimProcess[i] = weights[i] * blocksCard;
                //Since the number of data elements assigned must be an integer, truncate the previous number
                //  This number of elements will be directly assigned.
                intElemDimProcess[i] = (int) floatElemDimProcess[i];
                //Get the fractional part of floatElemDimProcess for the process i. As the number of data elements
                //  assigned to processes has to be an integer, this is the "part of a data element" unassigned for
                //  process i
                remElemDimProcess[i] = floatElemDimProcess[i] - intElemDimProcess[i];
                //Sum the fractional parts of all floatElemDimProcess. Those are the total unassigned data elements.
                sumIntElemDimProcess += intElemDimProcess[i];
                }

        //Assign the remamining and unassigned fractional parts as whole data elements to the processors
        for (i = 0; i < (blocksCard - sumIntElemDimProcess); i++)
                {
                double maxRemElemDimProcessValue = remElemDimProcess[0];
                int    maxRemElemDimProcessPosit = 0;
                int j;
                //Look for the maximum (unassigned) fractional part among all the processes for this dimension
                //  Store the maximum value and its position in the array.
                for (j = 0; j < procsCard; j++)
                        {
                        if (remElemDimProcess[j] > maxRemElemDimProcessValue)
                                {
                                maxRemElemDimProcessValue = remElemDimProcess[j];
                                maxRemElemDimProcessPosit = j;
                                }
                        }
                //Assign one more data element to the process with the highest fractional part
                intElemDimProcess[maxRemElemDimProcessPosit]++;
                //Substract one to the fractional part of the process which has been asigned one
                //  more data element. This way, the fractional part for this process will be a
                //  negative number and therefore will not be eligible for another assignation.
                remElemDimProcess[maxRemElemDimProcessPosit]--;
                }

        //TODO: This array copy seems somewhat absurd. There is a safety reason behind this that cannot be solved
	//  this way. Find a way and delete this two lines below.
        for (i = 0; i < procsCard; i++)
                {result[i] = intElemDimProcess[i];}
	//TODO: This function should return 1 only when all was OK.
	return 1;

}

int     hit_layout_plug_layWeighted_Sig(int procId, int procsCard, int blocksCard,
                                                                float* extraParameter,
                                                                HitSig input, HitSig *res ) {
        /* REJECT EMPTY INPUT SIGNATURES */
        if ( blocksCard == 0 ) {
                (*res) = HIT_SIG_NULL;
                return 0;
        }

	//nDimDataElements = blocksCard;//hit_sigCard(hit_shapeSig(shape, currDim));
	//nDimProcesses = procsCard;//topo.card[currDim];
        int weightedDistRes[procsCard];
        perform_weighted_distribution(procsCard, blocksCard, extraParameter, weightedDistRes);

	//If the number of data elements assigned to our process is zero, return null signature and non-active process
	if (weightedDistRes[procId] == 0.0)
                {
                (*res) = HIT_SIG_NULL;
                return 0;
		}
	else
		{
                //We are going to compute the signature for our process (procId). First let's compute the cumulative sum of all
                //  assigned elements for each process, from (the zeroth process to the current process id) of our dimension.
                int j;
                int cumSumIntElemDimProcess = 0;
                for (j = 0; j < procId ; j++)
                        cumSumIntElemDimProcess += weightedDistRes[j];

                /* BEGIN */
                (*res).begin  = input.begin + (cumSumIntElemDimProcess * input.stride);

                /* END */
                //If our intElemDimProcess[procId] were zero, we would not have a problem here since
                //  this condiction is avoided by the if block.
                (*res).end    = (*res).begin + ((weightedDistRes[procId] - 1) * input.stride);

                /* STRIDE */
                (*res).stride = input.stride;
                }

        /* RETURN ACTIVE */
        return 1;
}

int     hit_layout_plug_layWeighted_SigInv(int procId, int procsCard, int blocksCard,
                                                                                                float* extraParameter,
                                                                                                HitSig input,
                                                                                                int ind ) {
        HIT_NOT_USED( procId );

        /* CHECK: THE INDEX SHOULD BE IN THE INPUT DOMAIN */
        if ( ! hit_sigIn( input, ind ) ) return HIT_RANK_NULL;

        //TODO: Quick & dirty solution. Do the weighted distribution and look for the index within the result.
        //  It might be much faster to undo the equations of the distribution to directly compute the processor Id.
	//  Marked as a pending job.
        //UPDATE: Improved, but there is still room for more improvements.

        int weightedDistRes[procsCard];
        perform_weighted_distribution(procsCard, blocksCard, extraParameter, weightedDistRes);//, HIT_LAYWEIGHT_NORMALIZE);

        //begin_of_the_ind_proces_block  = input.begin + (cumSumIntElemDimProcess * input.stride);
        //Thus, we have ind = input.begin + ((cumSumIntElemDimProcess + k) * input.stride);, where k in {0 to the
        //  weightedDistRes value attributed to that processor}
        //Therefore     ((ind - input.begin) / input.stride) = (cumSumIntElemDimProcess + k);
        //For k = 0 (first element of the block assigned to the processor), we have
        //  ((ind - input.begin) / input.stride) = (cumSumIntElemDimProcess);
        //Let's scan for its value
        int cumSumIntElemDimTARGETProcess = ((ind - input.begin) / input.stride);

        int i;
        int cumSumIntElemDimProcess = 0;
        for (i = 0; i <  procsCard; i++){
                cumSumIntElemDimProcess += weightedDistRes[i];
                if (cumSumIntElemDimProcess >= cumSumIntElemDimTARGETProcess) break;
                }
#ifdef DEBUG
        int begin, end;
        begin  = input.begin + (cumSumIntElemDimProcess * input.stride);
        end    = begin + ((weightedDistRes[i] - 1) * input.stride);
        if (! ((ind >= begin) && (ind <= end)))
                printf("Error @ %s:%i: index %i not within the bounds of block (%i, %i, %i) for processor %i", __FILE__,__LINE__, ind, input.begin, input.end, input.stride, i);
#endif

        return i;
}

int     hit_layout_plug_layWeighted_ranks( char topoActiveMode,
                                        int procId, int procsCard, int blocksCard, float* extraParameter ) {

        HIT_NOT_USED( extraParameter );

        //TODO: Hacer esta función

        /* MODE: TOPO TO ACTIVE */
        if ( topoActiveMode == HIT_LAY_RANKS_TOPO_TO_ACTIVE ) {
        /* GET NUM ACTIVES, ASSERT: procId IS ACTIVE */
                int actives = hit_min( blocksCard, procsCard );
                if ( procId < 0 || procId > actives ) return HIT_RANK_NULL;
                else return procId;
        }
        /* MODE: ACTIVE TO TOPO */
        else return procId;

}

int     hit_layout_plug_layWeighted_maxCard( int procsCard, int blocksCard, float* extraParameter ) {

        int weightedDistRes[procsCard];
        perform_weighted_distribution(procsCard, blocksCard, extraParameter, weightedDistRes);

        int i = 0;
        int maxCard = weightedDistRes[0];
        for (i = 0; i < procsCard; i++)
                {if (weightedDistRes[i] > maxCard) maxCard = weightedDistRes[i];}
        return maxCard;
}

int     hit_layout_plug_layWeighted_minCard( int procsCard, int blocksCard, float* extraParameter ) {

        int weightedDistRes[procsCard];
        perform_weighted_distribution(procsCard, blocksCard, extraParameter, weightedDistRes);

        int i = 0;
        int minCard = weightedDistRes[0];
        for (i = 0; i < procsCard; i++)
                {if (weightedDistRes[i] < minCard) minCard = weightedDistRes[i];}
        return minCard;
}

int     hit_layout_plug_layWeighted_numActives( int procsCard, int blocksCard, float* extraParameter ) {

        int weightedDistRes[procsCard];
        perform_weighted_distribution(procsCard, blocksCard, extraParameter, weightedDistRes);

	int i;
        int numDimActiveLayProcesses = 0;
        for (i = 0; i < procsCard; i++)
                {if (weightedDistRes[i] != 0) numDimActiveLayProcesses++;}
        return numDimActiveLayProcesses;
}

/* 7.5 WEIGHTED: LAYOUT FUNCTION INTERFACE */
HitLayout       hit_layout_plug_layDimWeighted(int freeTopo, HitTopology topo, HitShape shape, int restrictDim, float* weights) {
        HitLayout res;

        /* ACTIVE */
        res = hit_layout_wrapper(
                                topo,
                                shape,
				&(hit_layout_plug_layCopy_Sig),
				&(hit_layout_plug_layCopy_SigInv),
                                &(hit_layout_plug_layWeighted_Sig),
                                &(hit_layout_plug_layWeighted_SigInv),
                                &(hit_layout_plug_layRegularContiguos_ranks),
                                &(hit_layout_plug_layRegularContiguos_ranks), //TODO: Ver a ver si hay que hacer esta función o qué
                                &(hit_layout_plug_layCopy_maxCard),
                                &(hit_layout_plug_layWeighted_maxCard),
                                &(hit_layout_plug_layCopy_minCard),
                                &(hit_layout_plug_layWeighted_minCard),
				&(hit_layout_plug_layRegular_numActives),
                                &(hit_layout_plug_layWeighted_numActives),
                                weights,
                                restrictDim
                        );

        res.type = HIT_LAYOUT_DIMWEIGHTED;

        if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	// @arturo: 2019-09-04 Signature layouts have a single group of active processors
	/* MY GROUP */
	/*
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if( res.active ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId(res,group);
	*/
	res.group = 0;

	// @arturo: 2019-09-04 Weighted layouts, the leader is always the first proc with elements
	// TODO: Locate the first processes that is active
	
	/* MY LEADER */
	//res.leader = topo.linearRank;
	res.leaderRanks = res.topo.self;
	res.leader = 1;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

/* 7.5 WEIGHTED: LAYOUT FUNCTION INTERFACE */
HitLayout       hit_layout_plug_layWeighted(int freeTopo, HitTopology topo, HitShape shape, float* weights) {
        HitLayout res;

        /* ACTIVE */
        res = hit_layout_wrapper(
                                topo,
                                shape,
                                &(hit_layout_plug_layWeighted_Sig),
                                &(hit_layout_plug_layWeighted_SigInv),
                                NULL,
                                NULL,
                                &(hit_layout_plug_layRegularContiguos_ranks), //TODO: Ver a ver si hay que hacer esta función o qué
                                NULL,
                                &(hit_layout_plug_layWeighted_maxCard),
                                NULL,
                                &(hit_layout_plug_layWeighted_minCard),
                                NULL,
                                &(hit_layout_plug_layWeighted_numActives),
                                NULL,
                                weights,
                                HIT_LAYOUT_NODIM
                        );

        res.type = HIT_LAYOUT_WEIGHTED;

        if(! res.active){
		// DEFAULT VALUES
		//res.group = HIT_GROUP_ID_NULL;
		//res.leaderRanks = HIT_RANKS_NULL;
		//res.leader = 0;
		if( freeTopo ) hit_topFree( topo );
		return res;
	}

	/* MY GROUP */
	HitRanks group = HIT_RANKS_NULL;
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		if( res.active ){
			group.rank[dim] = topo.self.rank[dim];
		} else {
			group.rank[dim] = -1;
		}
	}
	res.group =  hit_layActiveRanksId(res,group);

	/* MY LEADER */
	//res.leader = topo.linearRank;
	res.leaderRanks = group;
	res.leader = 1;

	if( freeTopo ) hit_topFree( topo );
	return res;
}

/* 8. LAYOUT (SIGNATURES): WRAP/UNWRAP NEIGHBOURS */
/* 8.1. WRAP ALL DIMENSIONS */
void	hit_layWrapNeighbors(HitLayout *lay) {
	/* FOR EACH DIMENSION, WRAP */
	int dim;
	for (dim=0; dim< hit_shapeDims(  hit_layShape(*lay) ); dim++) {
		(*lay).wrap[dim] = HIT_WRAPPED;
	}
}

/* 8.2. WRAP GIVEN DIMENSION */
void	hit_layWrapNeighborsDim(HitLayout *lay, int dim) {
	(*lay).wrap[dim] = HIT_WRAPPED;
}

/* 8.3. UNWRAP ALL DIMENSIONS */
void	hit_layUnwrapNeighbors(HitLayout *lay) {
	/* FOR EACH DIMENSION, UNWRAP */
	int dim;
	for (dim=0; dim< hit_shapeDims(hit_layShape(*lay)); dim++) {
		(*lay).wrap[dim] = HIT_NOWRAPPED;
	}
}

/* 8.4. UNWRAP GIVEN DIMENSION */
void	hit_layUnwrapNeighborsDim(HitLayout *lay, int dim) {
	(*lay).wrap[dim] = HIT_NOWRAPPED;
}



/* 9.1. EXTENDED SHAPE TO CREATE MAXIMAL BUFFERS FOR DIFFERENT SIZED TILES ON THE LAYOUT */
HitShape hit_layMaxShape( HitLayout lay ) {

	HitShape newShp = HIT_SHAPE_NULL;

	if(!lay.active) return newShp;

	if(hit_layout_class(lay) == HIT_LAYOUT_SIG_CLASS){

		/* 1A. COPY ORIGINAL SHAPE */
		newShp = hit_layShape(lay);

		/* 2A. EXTEND THE END TO ALLOW THE BIGGEST PIECES TO FIT */
		int i;
		for (i=0; i<hit_shapeDims(newShp); i++)
			hit_shapeSig(newShp,i).end = hit_shapeSig(newShp,i).begin + 
										( lay.maxSize[i] - 1 ) * hit_shapeSig(newShp,i).stride ;

	} else{

		/* 1B. MAKE A NEW SHAPE */
		return hit_shape(1,hit_sig(0,lay.maxSize[0],1));
	}

	/* RETURN EXTENDED SHAPE */
	return newShp;
}

HitShape hit_layDimMaxShape( HitLayout lay, int dim ) {

	HitShape newShp = HIT_SHAPE_NULL;

	if(!lay.active) return newShp;

	if(hit_layout_class(lay) == HIT_LAYOUT_SIG_CLASS){

		/* 1A. COPY ORIGINAL SHAPE */
		newShp = hit_layShape(lay);

		/* 2A. EXTEND THE END TO ALLOW THE BIGGEST PIECES TO FIT (ONLY FOR THE GIVEN DIMENSION) */
		hit_shapeSig(newShp,dim).end = hit_shapeSig(newShp,dim).begin +
										( lay.maxSize[dim] - 1 ) * hit_shapeSig(newShp,dim).stride ;

	} else{

		/* 1B. MAKE A NEW SHAPE */
		return hit_shape(1,hit_sig(0,lay.maxSize[0],1));
	}

	/* RETURN EXTENDED SHAPE */
	return newShp;
}


/* 9.2. EXTENDED SHAPE TO CREATE MINIMAL BUFFERS FOR DIFFERENT SIZED TILES ON THE LAYOUT */
HitShape hit_layMinShape( HitLayout lay ) {

	HitShape newShp = HIT_SHAPE_NULL;

	if(!lay.active) return newShp;

	if(hit_layout_class(lay) == HIT_LAYOUT_SIG_CLASS){

		/* 1A. COPY ORIGINAL SHAPE */
		newShp = hit_layShape(lay);

		/* 2A. EXTEND THE END TO ALLOW THE BIGGEST PIECES TO FIT */
		int i;
		for (i=0; i<hit_shapeDims(newShp); i++)
			hit_shapeSig(newShp,i).end = hit_shapeSig(newShp,i).begin + 
										( lay.minSize[i] - 1 ) * hit_shapeSig(newShp,i).stride ;

	} else{

		/* 1B. MAKE A NEW SHAPE */
		return hit_shape(1,hit_sig(0,lay.minSize[0],1));
	}

	/* RETURN EXTENDED SHAPE */
	return newShp;
}

HitShape hit_layDimMinShape( HitLayout lay, int dim ) {

	HitShape newShp = HIT_SHAPE_NULL;

	if(!lay.active) return newShp;

	if(hit_layout_class(lay) == HIT_LAYOUT_SIG_CLASS){

		/* 1A. COPY ORIGINAL SHAPE */
		newShp = hit_layShape(lay);

		/* 2A. EXTEND THE END TO ALLOW THE BIGGEST PIECES TO FIT (ONLY FOR THE GIVEN DIMENSION) */
		hit_shapeSig(newShp,dim).end = hit_shapeSig(newShp,dim).begin +
										( lay.minSize[dim] - 1 ) * hit_shapeSig(newShp,dim).stride ;

	} else{

		/* 1B. MAKE A NEW SHAPE */
		return hit_shape(1,hit_sig(0,lay.minSize[0],1));
	}

	/* RETURN EXTENDED SHAPE */
	return newShp;
}


/* 10. INTERNAL: WRAPPERS FOR SIGNATURE LAYOUTS */
/* 10.1. INTERNAL WRAPPER: SHAPE (1st TIME LAYOUT IS COMPUTED) */
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
								) {
	HitShape res = HIT_SHAPE_NULL;

	// PROJECTION ON PLAIN TOPOLOGY @arturo: 2019/01/11
	int plainProjection = ( topoType == HIT_TOPOLOGY_PLAIN && restrictToDim != HIT_LAYOUT_NODIM );

	/* FOR EACH DIMENSION IN THE SHAPE, COMPUTE LAYOUT */
	hit_shapeDimsSet(res, hit_shapeDims(shape));
	int active = ( hit_shapeDims(shape) > 0 );
	int dim;
	for (dim=0; dim<hit_shapeDims(shape); dim++) {
		// PROJECTION ON PLAIN TOPOLOGY @arturo: 2019/01/11
		/* NO TOPOLOGY DIMENSION, COPY ALL THE SIGNATURE */
		if ( ( plainProjection && dim != restrictToDim ) || ( dim >= topoNumDims ) ) {
			hit_shapeSig(res,dim) = hit_shapeSig(shape,dim);
		}
		/* COMPUTE SIGNATURE LAYOUT FOR TOPOLOGY CARDINALITY */
		else {
			int procId = proc.rank[dim];

			/* NULL RANK */
			if ( procId == HIT_RANK_NULL || procId < 0 || procId >= card[dim] ) {
				hit_shapeSig(res,dim) = HIT_SIG_NULL;
				active = 0;
			}
			/* NORMAL RANK */
			else {
				int procsCard = card[dim];
				int blocksCard = hit_sigCard(hit_shapeSig(shape,dim));
				int tmpActive;

				/* GENERIC vs. RESTRICTED DIMENSION: DIFFERENT SIGNATURE FUNCTION */
				if ( restrictToDim != HIT_LAYOUT_NODIM && dim == restrictToDim )
					tmpActive = sigFunctionRestricted(procId, procsCard, blocksCard, 
												extraParameter,
												hit_shapeSig(shape,dim),
												&(hit_shapeSig(res,dim))
												);
				else
					tmpActive = sigFunctionGeneric(procId, procsCard, blocksCard, 
												extraParameter,
												hit_shapeSig(shape,dim),
												&(hit_shapeSig(res,dim))
												);
				active = active && tmpActive;
			}
		}
	}

	/* RETURN ACTIVE */
	*newShape = res;
	return active;
}

/* 10.2. INTERNAL WRAPPER: SHAPE FOR GIVEN RANKS (LAYOUT ALREADY COMPUTED) */
HitShape	hit_layout_wrapperOtherShape(HitLayout self, HitRanks ranks) {
	HitShape res;

	/* COMPUTE SHAPE */
	// @arturo 2015/01/07 Add check for non-active processors
	int active = hit_layout_wrapperShape(
				self.topo.type,
				self.topo.numDims,
				ranks,
				self.topo.card,
				self.origShape,
				&(res),
				self.info.layoutSig.signatureGenericF,
				self.info.layoutSig.signatureRestrictedF,
				self.info.layoutSig.extraParameter,
				self.info.layoutSig.restrictToDim
		);
	if ( ! active ) return HIT_SHAPE_NULL;

	/* RETURN */
	return res;
}

/* 10.3. INTERNAL WRAPPER: SHAPE FOR NEIGHBORS (LAYOUT ALREADY COMPUTED) */
HitShape hit_layout_wrapperNeighborShape(HitLayout self, int dim, int shift) {
        HitShape res = HIT_SHAPE_NULL;

        /* FOR EACH DIMENSION IN THE SHAPE, COMPUTE LAYOUT */
        hit_shapeDimsSet(res, hit_shapeDims(self.origShape));

	int d;
	for (d=0; d<hit_layNumDims(self); d++) {
		/* SHIFTING IN NON-TOPOLOGY DIMENSIONS */
		if ( d == dim && d >= self.topo.numDims ) {
			/* FAKED SHIFT: THE SAME PROC SELECTED (SHIFT 0, OR WRAPPED LAYOUT) */
			if ( shift == 0 || self.wrap[d] == HIT_WRAPPED ) {
				hit_shapeSig(res,d) = hit_shapeSig(hit_layShape(self),d);

				if ( hit_sigCard(hit_shapeSig(res,d)) < 1 ) return HIT_SHAPE_NULL;
			}
			/* REAL SHIFT ALWAYS GOES OUTSIDE OF THE TOPOLOGY */
			else {
				 return HIT_SHAPE_NULL;
			}
		}
		/* COPY SIGNATURES OF NON-SHIFTED DIMENSION */
	   	else if ( d != dim ) {
			hit_shapeSig(res,d) = hit_shapeSig(hit_layShape(self),d);
			if ( hit_sigCard(hit_shapeSig(res,d)) < 1 ) return HIT_SHAPE_NULL;
		}

		/* COMPUTE NEW SIGNATURE FOR THE SHIFTED DIMENSION */
		else {
			/* GET NEW PROC RANK */
			int newProcId =	hit_layDimNeighbor( self, dim, shift );

			/* NULL RANK */
			if (newProcId == HIT_RANK_NULL || newProcId<0 || newProcId>=self.topo.card[d]) {
				return HIT_SHAPE_NULL;
			}
			/* NORMAL RANK */
			else {
				int tmpActive;
				if ( self.info.layoutSig.restrictToDim != HIT_LAYOUT_NODIM 
									&& self.info.layoutSig.restrictToDim == d ) {
					tmpActive = self.info.layoutSig.signatureRestrictedF( newProcId,
											self.topo.card[d],
										   	hit_sigCard( hit_shapeSig(self.origShape,d) ),
											self.info.layoutSig.extraParameter,
										   	hit_shapeSig(self.origShape,d),
											&(hit_shapeSig(res,d))
										);
				}
				else {
					tmpActive = self.info.layoutSig.signatureGenericF( newProcId,
											self.topo.card[d],
										   	hit_sigCard( hit_shapeSig(self.origShape,d) ),
											self.info.layoutSig.extraParameter,
										   	hit_shapeSig(self.origShape,d),
											&(hit_shapeSig(res,d))
										);
				}
				if(!tmpActive) return HIT_SHAPE_NULL;

			}
		}
	}

	/* RETURN */
	return res;
}



/* 10.8. TRANSLATE TOPOLOGY RANKS FROM/TO TOPOLOGY TO/FROM ACTIVE RANKS */
HitRanks hit_layTransformRanks( char topoActiveMode, HitLayout self, HitRanks ranks ) {
	HitRanks active = HIT_RANKS_NULL;

	/* 0. NON-SIGNATURE LAYOUTS: RETURN THE SAME RANKS */
	if ( self.type != HIT_LAYOUT_SIG_CLASS ) return ranks;

	/* 1. ALIASES FOR LAYOUT INFORMATION (DIMENSION INDEPENDENT) */
	float* extraParameter = self.info.layoutSig.extraParameter;
	int restrictedDim = self.info.layoutSig.restrictToDim;

	/* 2. FOR EACH DIMENSION */
	int dim;
	for (dim = 0; dim < hit_shapeDims(self.shape); dim++) {
		/* 2.1. ALIASES FOR LAYOUT INFORMATION (DIMENSION DEPENDENT) */
		int procsCard = self.topo.card[dim];
		int blocksCard = hit_sigCard(hit_shapeSig(self.origShape,dim));

		/* 2.2. TOPO TO ACTIVE RANK */
		if ( restrictedDim == dim )
			active.rank[dim] = self.info.layoutSig.ranksRestrictedF( topoActiveMode, ranks.rank[dim], procsCard, blocksCard, extraParameter );
		else
			active.rank[dim] = self.info.layoutSig.ranksGenericF( topoActiveMode, ranks.rank[dim], procsCard, blocksCard, extraParameter );
	}

	/* 3. RETURN ACTIVE RANKS */
	return active;
}


/* 10.9. TRANSFORM MULTIDIMENSIONAL ACTIVE RANKS TO PROCESS IDENTIFIER (LINEAR RANK)  */
int hit_layActiveRanksId( HitLayout lay, HitRanks ranks ) {

	if ( hit_ranksCmp(ranks,HIT_RANKS_NULL) ) return HIT_RANK_NULL;

	// @arturo 2019/09/09 Return null for non-active processes
	if ( ! lay.active ) return HIT_RANK_NULL;

	int linear = 0;
	int acumCard = 1;

	int dim;
	for(dim=lay.topo.numDims-1; dim>=0; dim--) {
		linear = linear + acumCard * ranks.rank[dim];
		acumCard = acumCard *  lay.numActives[dim];
	}

	return linear;
}



/* 10.10. TRANSFORM PROCESS IDENTIFIER (LINEAR RANK) IN MULTIDIMENSIONAL ACTIVE RANKS */
HitRanks hit_layActiveIdRanks(HitLayout lay, int linear){

	// @arturo 2019/09/09 Return null for non-active processes
	if ( linear <0 || linear >= hit_layNumActives( lay ) ) return HIT_RANKS_NULL;

	HitRanks ranks = HIT_RANKS_NULL;
	int acumCard[HIT_MAXDIMS];

	acumCard[lay.topo.numDims-1] = 1;

	int dim;
	for(dim=lay.topo.numDims-2; dim>=0; dim--) 	acumCard[dim] = acumCard[dim+1] *  lay.numActives[dim+1];

	for(dim=0;dim<lay.topo.numDims;dim++){
		ranks.rank[dim] = linear / acumCard[dim];
		linear %= acumCard[dim];
	}

	return ranks;
}


/* 10.4.1. GENERIC SHIFTED NEIGHBOUR: FROM ACTIVE RANK TO TOPOLOGY RANK */
int	hit_layNeighborFrom(HitLayout self, int source, int dim, int shift) {
	/* 1. ALIASES FOR LAYOUT INFORMATION */
	int procsCard = self.topo.card[dim];
	int blocksCard = hit_sigCard(hit_shapeSig(self.origShape,dim));
	float* extraParameter = self.info.layoutSig.extraParameter;
	int numActives = self.numActives[ dim ];
	int restrictedDim = self.info.layoutSig.restrictToDim;

	/* 2. SHIFT AND WRAP */
	int shiftedActive = source + shift;
	if ( self.wrap[dim] == HIT_NOWRAPPED ) {
		if ( shiftedActive < 0 || shiftedActive >= numActives ) return HIT_RANK_NULL;
	}
	else {
		shiftedActive = shiftedActive % numActives;
		if ( shiftedActive < 0 ) shiftedActive = shiftedActive + numActives;
	}

	/* 3. ACTIVE TO TOPO RANK */
	if ( restrictedDim == dim )
		return self.info.layoutSig.ranksRestrictedF( HIT_LAY_RANKS_ACTIVE_TO_TOPO, shiftedActive, procsCard, blocksCard, extraParameter );
	else
		return self.info.layoutSig.ranksGenericF( HIT_LAY_RANKS_ACTIVE_TO_TOPO, shiftedActive, procsCard, blocksCard, extraParameter );
}


/* 10.4.2. GENERIC SHIFTED NEIGHBOR: FROM TOPOLOGY RANK TO TOPOLOGY RANK */
int	hit_layNeighborFromTopoRank(HitLayout self, int source, int dim, int shift) {
	/* 1. ALIASES FOR LAYOUT INFORMATION */
	int procsCard = self.topo.card[dim];
	int blocksCard = hit_sigCard(hit_shapeSig(self.origShape,dim));
	float* extraParameter = self.info.layoutSig.extraParameter;
	int restrictedDim = self.info.layoutSig.restrictToDim;

	/* 2. TOPO TO ACTIVE RANK */
	int active;
	if ( restrictedDim == dim )
		active = self.info.layoutSig.ranksRestrictedF( HIT_LAY_RANKS_TOPO_TO_ACTIVE, source, procsCard, blocksCard, extraParameter );
	else
		active = self.info.layoutSig.ranksGenericF( HIT_LAY_RANKS_TOPO_TO_ACTIVE, source, procsCard, blocksCard, extraParameter );

	return hit_layNeighborFrom(self, active, dim, shift);
}


/* 10.5. INTERNAL WRAPPER: SHIFTED NEIGHBOUR DISTANCE */
int	hit_layNeighborDistance(HitLayout self, int dim, int shift) {
	int procId = self.topo.self.rank[dim];
	int neigh = hit_layNeighborFromTopoRank( self, procId, dim, shift );
	return neigh - procId;
}

/* 10.6.1. INTERNAL WRAPPER: GENERIC SHIFTED NEIGHBOUR RANKS */
HitRanks	hit_layNeighborRanksFrom(HitLayout self, HitRanks source, int dim, int shift) {
	HitRanks neighbor = HIT_RANKS_NULL;

	/* CHECK dim PARAMETER */
	if(dim < 0) return HIT_RANKS_NULL;

	/* NEIGHBORS IN LAYOUT DIMENSIONS WHICH ARE NOT IN THE TOPOLOGY, ONLY EXIST
	 * WHEN WRAPPING IS ACTIVE FOR THAT DIMENSION. THE NEIGHBOR IS THE SAME 
	 * VIRTUAL PROCCESOR  */
	if(dim >= self.topo.numDims ) {
		if ( self.wrap[dim] ) return self.topo.self;
		else return HIT_RANKS_NULL;
	}

	int d;
	for (d=0; d<hit_shapeDims(hit_layShape(self)); d++) {
		/* COPY RANK FOR NOT SELECTED DIM, OR DIM ABOVE THE DIMENSIONS IN THE TOPOLOGY */
		if (dim != d ) {
			neighbor.rank[d] = source.rank[d];
		}
		else neighbor.rank[d] = hit_layNeighborFrom( self, source.rank[d], d, shift );

		/* IF ANY RANK IS NULL, RETURN NULL RANKS */
		if ( neighbor.rank[d] == HIT_RANK_NULL ) return HIT_RANKS_NULL;
	}

	/* RETURN RESULT */
	return neighbor;
}

/* 10.6.2. INTERNAL WRAPPER: GENERIC SHIFTED NEIGHBOUR RANKS IN SEVERAL DIMENSIONS */
HitRanks hit_layNeighborRanksFromRanks(HitLayout self, HitRanks source, HitRanks shifts) {
	HitRanks neighbor = source;

	int d;
	for (d=0; d<hit_shapeDims(hit_layShape(self)); d++) {
		/* COPY RANKs FOR DIMs ABOVE THE DIMENSIONS IN THE TOPOLOGY */
		if( d >= self.topo.numDims ) {
			if ( shifts.rank[d] == 0 )
				neighbor.rank[d] = self.topo.self.rank[d];
			else
				neighbor.rank[d] = HIT_RANK_NULL;
		}
		/* SHIFT RANK IN THE DIMENSION */
		else neighbor.rank[d] = hit_layNeighborFrom( self, source.rank[d], d, shifts.rank[d] );

		/* IF ANY RESULTING RANK IS NULL, RETURN NULL RANKS */
		if ( neighbor.rank[d] == HIT_RANK_NULL ) return HIT_RANKS_NULL;
	}

	/* RETURN RESULT */
	return neighbor;
}

/* 10.6.3. INTERNAL WRAPPER: SHIFTED NEIGHBOUR RANKS */
HitRanks	hit_layNeighborRanks(HitLayout self, int dim, int shift) {

	HitRanks neighbor = HIT_RANKS_NULL;

	/* CHECK dim PARAMETER */
	if(dim < 0) return HIT_RANKS_NULL;

	/* NEIGHBORS IN LAYOUT DIMENSIONS WHICH ARE NOT IN THE TOPOLOGY, ONLY EXIST
	 * WHEN WRAPPING IS ACTIVE FOR THAT DIMENSION. THE NEIGHBOR IS THE SAME 
	 * VIRTUAL PROCCESOR  */
	if(dim >= self.topo.numDims ) {
		if ( self.wrap[dim] ) return self.topo.self;
		else return HIT_RANKS_NULL;
	}

	int d;
	for (d=0; d<hit_shapeDims(hit_layShape(self)); d++) {
		/* COPY RANK FOR NOT SELECTED DIM, OR DIM ABOVE THE DIMENSIONS IN THE TOPOLOGY */
		if (dim != d ) {
			neighbor.rank[d] = self.topo.self.rank[d];
		}
		else neighbor.rank[d] = hit_layDimNeighbor( self, d, shift );

		/* IF ANY RANK IS NULL, RETURN NULL RANKS */
		if ( neighbor.rank[d] == HIT_RANK_NULL ) return HIT_RANKS_NULL;

	}

	/* RETURN RESULT */
	return neighbor;
}


/* 10.7. INTERNAL WRAPPER: DO LAYOUT FOR ONE OR ALL DIMENSIONS */
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
								int	restrictToDim
								) {

	/* 0. CHECK NULL TOPOLOGY */
	if ( hit_ptopoIsNull( topo.pTopology ) ) return HIT_LAYOUT_NULL;

	/* 1. INIT RESULT STRUCTURE */
	HitLayout res = HIT_LAYOUT_NULL;
	res.info.layoutSig = HIT_LAYOUTSIG_NULL;

	/* 2. COPY ORIGINAL SHAPE AND PARAMETERS FOR FUTURE REFERENCE */
	res.origShape = shape;
	res.topo = topo;
	res.info.layoutSig.restrictToDim = restrictToDim;
	res.info.layoutSig.extraParameter = extraParameter;

	/* 3. SKIP PROCCESSROS WHICH ARE NON-ACTIVE IN THE TOPOLOGY */
	int active = 0;
	// @arturo: 2015/01/06 Processors non-active in the topology should be able to
	// 			ask and answer about other processors which ARE active. 
	// 			Used for Redistribution funtion with different topologies.
	// 			Thus, the pointers to compute layouts (in other procs.) should be set
	//if ( topo.active ) {
		/* 4. INITIALIZE POINTERS TO METHODS */
		res.info.layoutSig.signatureGenericF = signatureGenericF;
		res.info.layoutSig.signatureInvGenericF = signatureInvGenericF;
		res.info.layoutSig.signatureRestrictedF = signatureRestrictedF;
		res.info.layoutSig.signatureInvRestrictedF = signatureInvRestrictedF;
		//res.info.layoutSig.neighborGenericF = neighborGenericF;
		//res.info.layoutSig.neighborRestrictedF = neighborRestrictedF;
		res.info.layoutSig.ranksGenericF = ranksGenericF;
		res.info.layoutSig.ranksRestrictedF = ranksRestrictedF;

		/* 5. COMPUTE NUMBER OF ACTIVE PROCCESORS */
		int dim;
		for (dim=0; dim<hit_shapeDims(shape); dim++) {
			/* OLD: It works only for regular blocking/cyclic signatures */
			/*
			res.numActives[dim] =
				( topo.card[dim] <= hit_sigCard(hit_shapeSig(shape,dim) ) ) ?
					topo.card[dim] :
					hit_sigCard(hit_shapeSig(shape,dim));
			*/

			int blocksCard = hit_sigCard(hit_shapeSig(shape,dim));
			
			/* 5.0. PROJECTION ON PLAIN TOPOLOGY @arturo: 2019/01/11 */
			int plainProjection = ( topo.type == HIT_TOPOLOGY_PLAIN && restrictToDim != HIT_LAYOUT_NODIM );
			if ( plainProjection ) {
				// SELECTED DIM
				if ( dim == restrictToDim ) 
					res.numActives[dim] = activesRestrictedF( topo.card[0], blocksCard, extraParameter);
				// DIMENSIONS OUT OF TOPOLOGY: ALL ACTIVE
				else 
					res.numActives[dim] = blocksCard;
			}
			/* 5.1. DIMENSIONS OUT OF TOPOLOGY: ALL ACTIVE */
			else if ( dim >= topo.numDims ) res.numActives[dim] = blocksCard;
			/* 5.2. DIMENSION RESTRICTED TO A SPECIFIC SIGNATURE FUNCTION */
			else if ( dim == restrictToDim ) 
				res.numActives[dim] = activesRestrictedF( topo.card[dim], blocksCard, extraParameter);
			/* 5.3. OTHER DIMENSIONS USE THE GENERIC SIGNATURE FUNCTION */
			else 
				res.numActives[dim] = activesGenericF( topo.card[dim], blocksCard, extraParameter);

			// It can't be 0. TODO: REVISE THE FUNCTIONS TO COMPUTE numActives, 
			//  CHECK THAT THEY CANNONT	RETURN 0 IF THE INPUT SHAPE IS NOT NULL
			// res.numActives[dim] = hit_max(1,res.numActives[dim]);
		}

	// @arturo 2015/01/06
	if ( topo.active ) {

		/* 6. COMPUTE BLOCK AND ACTIVE STATUS */
		active = hit_layout_wrapperShape(		topo.type,
								topo.numDims,
								topo.self,
								topo.card,
								shape,
								&(hit_layShape(res)),
								signatureGenericF,
								signatureRestrictedF,
								extraParameter,
								restrictToDim
								);
		res.active = active;
		if(! active) hit_layShape(res) = HIT_SHAPE_NULL;

		/* 7. COMPUTE MAX/MIN SIZES */
		for (dim=0; dim<hit_shapeDims(shape); dim++) {
			int blocksCard = hit_sigCard(hit_shapeSig(shape,dim));
			/* 7.1. DIMENSIONS OUT OF TOPOLOGY: NON-PARTITIONED, SIGNATURE COPIED IN ALL PROCS */
			if ( dim >= topo.numDims ) {
				res.maxSize[dim] = blocksCard;
				res.minSize[dim] = blocksCard;
			}
			/* 7.2. DIMENSION RESTRICTED TO A SPECIFIC SIGNATURE FUNCTION */
			else if ( dim == restrictToDim ) {
				res.maxSize[dim] = maxCardRestrictedF( topo.card[dim], blocksCard, extraParameter);
				res.minSize[dim] = minCardRestrictedF( topo.card[dim], blocksCard, extraParameter);
			}
			/* 7.3. OTHER DIMENSIONS USE THE GENERIC SIGNATURE FUNCTION */
			else {
				res.maxSize[dim] = maxCardGenericF( topo.card[dim], blocksCard, extraParameter);
				res.minSize[dim] = minCardGenericF( topo.card[dim], blocksCard, extraParameter);
			}
		}
	}

	/* 8. COMPUTE THE MPI COMUNICATOR FOR THE ACTIVE PROCESSORS */
	int allactive = 1;
	//int dim;
	for (dim=0; dim<topo.numDims; dim++) {
		if(topo.card[dim] > hit_sigCard(hit_shapeSig(shape,dim))) { allactive = 0; break; }
	}

	if( allactive ) {
	//if( active ) {
		res.pTopology[0] = hit_ptopDup( topo.pTopology );
	}
	else {
	// @arturo: Feb 2012, TODO: This check should not be needed any more.
	// else if( topo.pTopology.lowLevel != NULL ) {
#ifdef DEBUG
	printf("Not all processes are active -> mpi communicator split\n");	
#endif
		/* @arturo: Feb 2012 Split physical topology communicators is embedded in a function */
		res.pTopology[0] = hit_ptopSplit( topo.pTopology, active );
	}

	/* 9. RETURN RESULT */
	return res;
}


/* COMPUTE THE TOTAL NUMBER OF ACTIVE PROCESSORS IN THE LAYOUT */
int hit_layNumActives( HitLayout lay ) {
	int nProcs = 1;
	int i;
	for( i=0; i<lay.topo.numDims; i++ ) nProcs *= lay.numActives[i];
	return nProcs;
}



/* COMPUTE THE ACTIVE RANK OF THE OWNER OF A GIVEN INDEX */
int hit_layDimOwner( HitLayout lay, int dim, int ind ) {

	/* 1. DIMENSIONS OUT OF TOPOLOGY */
	if ( dim >= lay.topo.numDims ) return HIT_RANK_NULL;

	/* 2. DIMENSION RESTRICTED TO A SPECIFIC SIGNATURE FUNCTION */
	if ( dim == lay.info.layoutSig.restrictToDim )
		return lay.info.layoutSig.signatureInvRestrictedF( 
				hit_laySelfRanksDim( lay, dim ),
				lay.topo.card[dim],
				hit_shapeSigCard( lay.origShape, dim ),
				lay.info.layoutSig.extraParameter,
				hit_shapeSig( lay.origShape, dim ),
				ind
				);
	/* 3. DIMENSION RESTRICTED TO A SPECIFIC SIGNATURE FUNCTION */
	else return lay.info.layoutSig.signatureInvGenericF( 
				hit_laySelfRanksDim( lay, dim ),
				lay.topo.card[dim],
				hit_shapeSigCard( lay.origShape, dim ),
				lay.info.layoutSig.extraParameter,
				hit_shapeSig( lay.origShape, dim ),
				ind
				);
}


/* FREE LAYOUT INTERNAL RESOURCES */
void hit_layFree(HitLayout lay){

	int dim;

	if(hit_layout_class(lay) == HIT_LAYOUT_SIG_CLASS){

		// @arturo Mar 2013 CURRENTLY IT IS NOT A COPY, DO NOT FREE
		// Free the main topology
		// hit_topFree( lay.topo );

		// Free axis topologies if activated
		for( dim=0; dim < hit_layNumDims(lay); dim++ ) {
			// @arturo Mar 2013
			/*
			if(!hit_ptopoCmp(lay.pTopology[dim+1],HIT_PTOPOLOGY_NULL)){
				MPI_Comm_free( (MPI_Comm *) lay.pTopology[dim+1].lowLevel);
				free( lay.pTopology[dim+1].lowLevel );
			}
			*/
			if ( lay.pTopology[dim+1] != NULL ) hit_ptopFree( &(lay.pTopology[dim+1]) );
		}

	} else if( hit_layout_class(lay) == HIT_LAYOUT_LIST_CLASS ){
		
		// Groups
		free(lay.info.layoutList.assignedGroups);
		free(lay.info.layoutList.groups);

		// Sparse
		hit_shapeFree( hit_layShape(lay) );

	}

	// Free the active pTopology
	// @arturo Mar 2013
	/*
	if( ! hit_ptopoCmp(lay.pTopology[0], lay.topo.pTopology) ){
		MPI_Comm_free( (MPI_Comm *) lay.pTopology[0].lowLevel);
		free( lay.pTopology[0].lowLevel );
	}
	*/
	hit_ptopFree( &(lay.pTopology[0]) );

	// Free group topology if activated
	/*
	if( ! hit_ptopoCmp(lay.pTopologyGroup, HIT_PTOPOLOGY_NULL ) ){
		MPI_Comm_free( (MPI_Comm *) lay.pTopologyGroup.lowLevel);
		free( lay.pTopologyGroup.lowLevel );
	}
	*/
	if( lay.pTopologyGroup != NULL ) hit_ptopFree( &(lay.pTopologyGroup) );
}



/* @arturo: Feb 2013
 * BUILD THE SUBTOPOLOGY OF A GIVEN GROUP */
/* TODO:
 * 	Groups are those processors with the same part of the layout assigned.
 * 	NOTE: Most signature layouts, by definition, could not group processors. All active processors
 * 	have their own group, and inactive processors form a group without topology structure (they
 * 	are again a cloud or plain topology)
 * 	The exceptions are the layouts with Copy signature functions. For these layouts, forming
 * 	the groups is associated with rank indexes. It should be easy to provide functions to derive
 * 	groups.
 */
/* TODO: Only works for list layouts, do implementation for Signature layouts */
HitTopology hit_layGroupTopo( HitLayout lay, int groupId ) {
	HitTopology newTopo = HIT_TOPOLOGY_NULL;

	/* 0.1. CHECK THAT IT IS A LIST LAYOUT */
	if ( lay.type < HIT_LAYOUT_LIST_FIRST ) {
		hit_warnInternal(__FUNCTION__, "Sorry. Implementation of Group topologies for signature layouts is not yey implemented.", "", __FILE__, __LINE__ );
		return HIT_TOPOLOGY_NULL;
	}

	/* 0.2. CHECK THAT THE GROUP INDEX EXIST IN THE LAYOUT */
	if ( groupId < 0 || groupId >= hit_lgr_numGroups(lay) ) {
		hit_warnInternal(__FUNCTION__, "Group Id does not exist", "", __FILE__, __LINE__ );
		return HIT_TOPOLOGY_NULL;
	}

	/* 1. IF NOT DONE BEFORE, GENERATE THE SUB-GROUP PTOPOLOGY */
	//if ( hit_ptopoIsNull( lay.pTopologyGroup ) ) {
	if ( lay.pTopologyGroup == NULL ) {
	 	/* 1.1. ONLY ONE GROUP, DUPLICATE THE ACTIVES PTOPOLOGY */
		if ( hit_lgr_numGroups( lay ) == 1 )
			lay.pTopologyGroup = hit_ptopDup( lay.pTopology[0] );
	 	/* 1.2. SEVERAL GROUPS, SPLIT THE TOPOLOGY, KEEP THE SAME RANK ORDER IN THE GROUPS */
		else
			lay.pTopologyGroup = hit_ptopSplit( lay.pTopology[0], lay.group );
	}

	/* 2. FILL COMMON TOPOLOGY FIELDS */
	newTopo.type = HIT_TOPOLOGY_PLAIN;
	// @arturo Mar 2013, Main pTopology field is not currently freed. Use a direct copy.
	// 					TODO: This field will get obsolete in the next refactorization.
	//newTopo.pTopology = hit_ptopDup( lay.pTopologyGroup );
	newTopo.pTopology = lay.pTopologyGroup;
	newTopo.numDims = 1;

	/* 3. IF THE LOCAL PROCESS IS IN THE GROUP */
	if ( groupId == lay.group ) {
		newTopo.card[0] = newTopo.pTopology->numProcs;
		newTopo.self.rank[0] = newTopo.pTopology->selfRank;
		//newTopo.linearRank = newTopo.self.rank[0];
		//newTopo.pTopology = hit_ptopDup( lay.pTopologyGroup );
		newTopo.active = 1;
	}
	/* 4. LOCAL PROCESS NOT IN THE GROUP */
	else {
		newTopo.card[0] = hit_lgr_groupNProcs( lay, groupId );
	}

	/* 4. RETURN */
	return newTopo;
}


/* @arturo Feb, 2013 */
/* BUILD A TOPOLOGY OBJECT WITH THE ACTIVE PROCESSORS */
HitTopology hit_layActivesTopology( HitLayout lay ) {
	HitTopology newTopo = HIT_TOPOLOGY_NULL;

	/* NON-ACTIVE PROCESSORS: RETURN NULL */
	if ( ! hit_layImActive( lay ) ) return newTopo;

	/* BUILD A PLAIN TOPOLOGY OBJECT */
	newTopo.pTopology = hit_ptopDup( lay.pTopology[0] );
	newTopo.type = HIT_TOPOLOGY_PLAIN;
	newTopo.numDims = 1;
	newTopo.card[0] = newTopo.pTopology->numProcs;
	newTopo.self.rank[0] = newTopo.pTopology->selfRank;
	//newTopo.linearRank = newTopo.pTopology->selfRank;
	newTopo.active = 1;

	/* RETURN */
	return newTopo;
}

/***************************************************************/
/***************************************************************/
/***************************************************************/



/*
 * hit_layout_list_initGroups
 * Init the layout structure whit n elements
 */
void hit_layout_list_initGroups(HitLayout * lay, int numElementsTotal) {

	lay->group = HIT_GROUP_ID_NULL;

	lay->info.layoutList.numGroups = 0;
	lay->info.layoutList.groups = NULL;

	lay->info.layoutList.numElementsTotal = numElementsTotal;
	// @arturo Ago 2015: New allocP interface
	// hit_calloc(lay->info.layoutList.assignedGroups, sizeof(int), (size_t)numElementsTotal,int*);
	hit_calloc(lay->info.layoutList.assignedGroups, int, numElementsTotal );
	
}


/**
 * hit_layout_list_addGroup
 * Adds a new group to a list layout
 * @param lay A pointer to the layout
 * @param leader, the leader of the group, if it is -1 the function will calculate it automatically
 * @param np, the number of processors in the group
 */
void hit_layout_list_addGroup(HitLayout * lay, int leader, int np) {


	// @arturo Ago 2015: New allocP interface
	// hit_realloc(lay->info.layoutList.groups,(size_t)sizeof(HitGroup)*((size_t)lay->info.layoutList.numGroups+1),HitGroup*);
	hit_realloc(lay->info.layoutList.groups, HitGroup, lay->info.layoutList.numGroups+1 );

	if(leader == -1){
		if(lay->info.layoutList.numGroups == 0){
			leader = 0;
		} else {
			leader = lay->info.layoutList.groups[lay->info.layoutList.numGroups-1].leader +
					 lay->info.layoutList.groups[lay->info.layoutList.numGroups-1].numProcs;
		}
	}

#ifdef DEBUG
	printf("Adding group %d, leader: %d, nprocs: %d\n",lay->info.layoutList.numGroups,leader,np);
#endif

	lay->info.layoutList.groups[lay->info.layoutList.numGroups].leader = leader;
	lay->info.layoutList.groups[lay->info.layoutList.numGroups].numProcs = np;

	lay->info.layoutList.numGroups++;
}



/* List of the elements assigned to a group */
void hit_lay_elements(HitLayout layout, int group, int ** els, int * nEls){

	HitLayout * layg = &layout;
	int i;
	int nElements = 0;
	int * elements = NULL;

	for( i=0; i<layg->info.layoutList.numElementsTotal; i++){

		if( layg->info.layoutList.assignedGroups[i] == group ){
			nElements++;
			// @arturo Ago 2015: New allocP interface
			// hit_realloc(elements,sizeof(int)*(size_t)nElements,int*);
			hit_realloc(elements, int, nElements);
			elements[ nElements -1 ] = i;
		}
	}

	(*els) = elements;
	(*nEls) = nElements;

}


/* Calculate the group number for a given processor */
int hit_lay_procGroup(HitLayout layout, int processor){

	HitLayout * layg = &(layout);
	int i;
	int sum;

	sum = 0;
	for(i=0;i<layg->info.layoutList.numGroups;i++){
		sum += layg->info.layoutList.groups[i].numProcs;
		if(processor < sum) return i;
	}

	return -1;
}



/* SCHEDULING OF n INDEPENDENT BLOCKS TO m PROCS, ACCORDING TO BLOCK WEIGHTs */
HitLayout hit_layout_plug_layIndependentLB( int freeTopo, HitTopology topo , HitShape elements, const double *weights ) {

	HitLayout lay = HIT_LAYOUT_NULL;
	double totWeight = 0.0;
	int i;
	int proc;
	int nElements;
	int totProcs;
	int numGroups;
	int nProcs;


	lay.topo = topo;
	lay.origShape = elements;
	// lay.pTopology[0] = topo.pTopology;
	lay.pTopology[0] = hit_ptopDup( topo.pTopology );
	lay.type = HIT_LAYOUT_INDEPENDENTLB;

	/* NUMBER OF PROCESSORS */
	nProcs = 1;
	for(i=0;i<topo.numDims;i++){
		nProcs *= topo.card[i];
		lay.numActives[i] = topo.card[i];
	}

	/* CALCULATE NUMBER OF ELEMENTS */
	nElements = hit_sigCard(hit_shapeSig(elements,0));

	int numProcs[nElements];
	double normWeight[nElements];
	double normWeightPerGroup[nElements];

#ifdef DEBUG
double normW = 0.0;
setbuf(stdout, NULL);
#endif

	/* INIT SCHED STRUCTURE */
	hit_layout_list_initGroups(&lay, nElements);

	/* SAFETY CHECK: CONSISTENCY OF INPUT DATA, NO NEGATIVE LOADS */
#ifndef Hit_UNSAFE
	for (i=0; i<nElements; i++) {
		if (weights[i]<0.0){
			hit_errInternal(__FUNCTION__,"Weight input data is incorrect (check workload expressions)","",__FILE__,__LINE__);	
		}
	}
#endif

	/* COMPUTE TOTAL WORKLOAD */
	totProcs = 0;
	for (i=0; i<nElements; i++) totWeight = totWeight + weights[i];
#ifdef DEBUG
printf("TotWeight-A %lf\n", totWeight);
#endif

	/* SAFETY CHECK: CONSISTENCY OF INPUT DATA, TOTAL WEIGHT 0 */
	if (totWeight == 0.0) totWeight = 1.0;	/* Avoid division by 0 */
#ifdef DEBUG
printf("TotWeight-B %lf\n", totWeight);
#endif

	/* NORMALIZE THE NUMBER OF PROCESSOR PER ELEMENT, ROUND DOWN */
	for (i=0; i<nElements; i++) {
		normWeight[i] = nProcs * weights[i] / totWeight;
		numProcs[i] = (int)normWeight[i];
		normWeight[i] = normWeight[i] - numProcs[i];
		totProcs = totProcs + numProcs[i];
#ifdef DEBUG
printf("CTRL numProcs[%i] =  %d . %lf\n", i, numProcs[i],normWeight[i]);
normW = normW + numProcs[i] + normWeight[i];
#endif
	}

#ifdef DEBUG
printf("CTRL Total norm. weight %lf\n", normW);
#endif

	/* ASSIGN THE REST OF PROCESSORS */
	for (i=0; i<(nProcs-totProcs); i++) {
		/* ASSIGN IT TO THE ELEMENT WITH THE BIGGEST REST OF NORM. WEIGHT */
		double maxWeight = normWeight[0];
		int maxPos = 0;
		int j;
		for (j=1; j<nElements; j++) {
			if (normWeight[j]>maxWeight) {
				maxWeight = normWeight[j];
				maxPos = j;
			}
		}
		numProcs[maxPos]++;
		normWeight[maxPos]=normWeight[maxPos]-1;
	}

#ifdef DEBUG
for (i=0;i<nElements;i++) {
printf("CTRL numProcs[%i] =  %d . %lf\n", i, numProcs[i],normWeight[i]);
}
#endif

	/* CREATE PROCESSOR-GROUPS AND ASSIGN ELEMENTS */
	proc = 0;
	numGroups = 0;
	for (i=0; i<nElements; i++) {
		if ( numProcs[i] > 0 ) {
			hit_layout_list_addGroup(&lay, proc, numProcs[i]);
			proc = proc + numProcs[i];

			lay.info.layoutList.assignedGroups[i] = numGroups;

			normWeightPerGroup[numGroups]=normWeight[i];
			numGroups++;
		}
	}

	/* ELEMENTS WITH NO PROCCESSOR YET ASSIGNED */
	for (i=0; i<nElements; i++) {
		if (numProcs[i]==0) {
			/* ASSIGN IT TO THE LESS REST OF NORM. WEIGHT GROUP */
			double minWeight = normWeightPerGroup[0];
			int minPos = 0;
			int j;
			for (j=1; j<numGroups; j++) {
				if ( normWeightPerGroup[j] < minWeight) {
					minWeight = normWeightPerGroup[j];
					minPos = j;
				}
			}
			lay.info.layoutList.assignedGroups[i] = minPos;
			normWeightPerGroup[minPos] = normWeightPerGroup[minPos] + normWeight[i];
#ifdef DEBUG
printf("Block %d, assigned to group %d, new normWeight %lf\n",i,minPos,normWeightPerGroup[minPos]);
#endif
		}
	}

	// @arturo 2015/01/22
	//int group = hit_lay_procGroup(lay,topo.linearRank);
	int group = hit_lay_procGroup(lay, hit_topSelfRankInternal( topo ) );
	lay.group = group;

	int leaderId = lay.info.layoutList.groups[group].leader;
	// @arturo 2015/01/22
	//lay.leader = (topo.linearRank == leaderId ) ? 1 : 0;
	lay.leader = ( hit_topSelfRankInternal( topo ) == leaderId ) ? 1 : 0;

	HitRanks leaderRanks = HIT_RANKS_NULL;
	leaderRanks.rank[0] = leaderId;
	lay.leaderRanks = leaderRanks;

	/* RETURN */
	if( freeTopo ) hit_topFree( topo );
	return lay;
}



/* SCHEDULING OF n CONTIGUOUS BLOCKS TO m PROCS, ACCORDING TO BLOCK WEIGHTs like IS NAS benckmark */
HitLayout hit_layout_plug_layContiguous(int freeTopo, HitTopology topo, HitShape elements, const double *weights ) {

	HitLayout lay = HIT_LAYOUT_NULL;
	double totWeight;		// Total weight
	double avgWeight;		// Average weight
	int i;
	int proc;
	int nElements;
	int nProcs;

	lay.topo = topo;
	lay.origShape = elements;
	lay.pTopology[0] = hit_ptopDup( topo.pTopology );
	lay.type = HIT_LAYOUT_CONTIGUOUS;

	/* NUMBER OF PROCESSORS */
	nProcs = 1;
	for(i=0;i<topo.numDims;i++){
		nProcs *= topo.card[i];
		lay.numActives[i] = topo.card[i];
	}

	/* CALCULATE NUMBER OF ELEMENTS */
	nElements = hit_shapeCard( elements );

	/* INIT STRUCTURE */
	hit_layout_list_initGroups(&lay, nElements);

	/* SAFETY CHECK: CONSISTENCY OF INPUT DATA, NO NEGATIVE LOADS */
#ifndef Hit_UNSAFE
	for (i=0; i<nElements; i++) {
		if (weights[i]<0.0){
			hit_errInternal(__FUNCTION__,"Weight input data is incorrect (check workload expressions)","",__FILE__,__LINE__);
		}
	}
#endif

	/* COMPUTE TOTAL WORKLOAD */
	totWeight = 0.0;
	for (i=0; i<nElements; i++) totWeight += weights[i];

	/* SAFETY CHECK: CONSISTENCY OF INPUT DATA, TOTAL WEIGHT 0 */
	if (totWeight == 0.0) totWeight = 1.0;	/* Avoid division by 0 */

	/* AVERAGE WEIGHT */
	avgWeight = totWeight / nProcs;

#ifdef DEBUG
	printf("[%d] plugContiguous CTRL totalWeight: %lf, nprocs: %d, avg: %lf\n", hit_Rank, totWeight, nProcs, avgWeight );
#endif

	proc = 0;
	double localWeight = 0.0;
	double acumWeight = 0.0;
	int	localElements = 0;
	int	acumElements = 0;

	/* ASSIGN ELEMENTS TO PROCESSORS */
	for(i=0; i<nElements; i++) {
		localElements++;
		localWeight = localWeight + weights[i];
		lay.info.layoutList.assignedGroups[i] = proc;

		if ( acumWeight + localWeight >= avgWeight * (proc+1) ) {
			if ( proc == hit_topSelfRankInternal( topo ) ) {
				lay.ownLoad = localWeight;
				lay.predecessorsLoad[0] = acumWeight;
				lay.successorsLoad[0] = totWeight - acumWeight - localWeight;
				lay.info.layoutList.cardOwnElements = localElements;
				lay.info.layoutList.cardPredElements = acumElements;
				lay.info.layoutList.cardSuccElements = nElements - acumElements - localElements;
			}
			hit_layout_list_addGroup(&lay, proc, 1);
			proc++;

			acumWeight += localWeight;
			acumElements += localElements;
			localWeight = 0.0;
			localElements = 0;
		}
	}
	/* LAST ELEMENTS, IN CASE THAT ROUNDING ERRORS IN INNER COMPARISON LET THEM UNASSIGNED */
	if ( localWeight != 0.0 ) {
		hit_layout_list_addGroup(&lay, proc, 1);
		if ( proc == hit_topSelfRankInternal( topo ) ) {
			lay.ownLoad = localWeight;
			lay.predecessorsLoad[0] = acumWeight;
			lay.successorsLoad[0] = totWeight - acumWeight - localWeight;
			lay.info.layoutList.cardOwnElements = localElements;
			lay.info.layoutList.cardPredElements = acumElements;
			lay.info.layoutList.cardSuccElements = nElements - acumElements - localElements;
		}
		proc++;
		acumWeight += localWeight;
		acumElements += localElements;
	}

	/* ASSIGN FREE PROCESSORS  */
	// @arturo: IS policy: EVERY PROC. HAS ITS OWN GROUP, THERE ARE GROUPS WITH NO LOAD
	for ( ; proc < nProcs; proc++ ) {
		hit_layout_list_addGroup(&lay, proc, 1);
		if ( proc == hit_topSelfRankInternal( topo ) ) {
			lay.ownLoad = 0.0;
			lay.predecessorsLoad[0] = acumWeight;
		}
	}
	/*
	nProcs -= (proc+1);
	while(nProcs > 0){
		int j, maxg = 0;
		double maxw = 0, current;

		for(i=0;i<lay.info.layoutList.numGroups;i++){
			current = 0;
			for(j=0;j<lay.info.layoutList.numElementsTotal;j++){
				if(lay.info.layoutList.assignedGroups[j] == i){
					current += weights[j];
				}
			}

			current /= lay.info.layoutList.groups[i].numProcs;

			if(current > maxw){
				maxw = current;
				maxg = i;
			}
		}

		lay.info.layoutList.groups[maxg].numProcs++;
		nProcs--;
	}
	*/

	/* CALCULATE THE LEADERS */
	int act_leader = 0;
	for(i=0;i<lay.info.layoutList.numGroups;i++){
		lay.info.layoutList.groups[i].leader = act_leader;
		act_leader += lay.info.layoutList.groups[i].numProcs;
	}
	// @arturo 2015/01/22
	//int group = hit_lay_procGroup(lay,topo.linearRank);
	int group = hit_lay_procGroup(lay, hit_topSelfRankInternal( topo ) );
	lay.group = group;
	int leader = lay.info.layoutList.groups[group].leader;
	// @arturo 2015/01/22
	//lay.leader = (topo.linearRank == leader ) ? 1 : 0;
	lay.leader = ( hit_topSelfRankInternal( topo ) == leader ) ? 1 : 0;

	/* RETURN */
	if( freeTopo ) hit_topFree( topo );
	return lay;
}



/**
 * Performs a Broadcast communication to share a sparse shape.
 */
void hit_bShapeBcastInternal(HitShape * shape, HitTopology topo){

	int ok;
	int cards[2];
	// @arturo Mar 2013
	//MPI_Comm comm = *((MPI_Comm *)topo.pTopology.lowLevel);
	MPI_Comm comm = topo.pTopology->comm;

	if(topo.pTopology->selfRank == 0){
		cards[0] = hit_bShapeCard(*shape,0);
		cards[1] = hit_bShapeCard(*shape,1);
		MPI_Bcast(cards, 2, MPI_INT, 0, comm);
	} else {
		MPI_Bcast(cards, 2, MPI_INT, 0, comm);

		HitShape newShp = hit_bitmapShapeMatrix(cards[0],cards[1]);
		*shape = newShp;
	}

	// Send the flag for names.
	ok = MPI_Bcast(&(hit_bShapeNameList(*shape,0).flagNames),1,MPI_INT,0,comm);
	hit_mpiTestError(ok,"Error in bitmap hit_bitmapShapeBcastInternal");
	ok = MPI_Bcast(&(hit_bShapeNameList(*shape,1).flagNames),1,MPI_INT,0,comm);
	hit_mpiTestError(ok,"Error in bitmap hit_bitmapShapeBcastInternal");


	if(topo.pTopology->selfRank != 0){
		if(hit_bShapeNameList(*shape,0).flagNames == HIT_SHAPE_NAMES_ARRAY)
			hit_bShapeNameList(*shape,0).flagNames = HIT_SHAPE_NAMES_NOARRAY;
		if(hit_bShapeNameList(*shape,1).flagNames == HIT_SHAPE_NAMES_ARRAY)
			hit_bShapeNameList(*shape,1).flagNames = HIT_SHAPE_NAMES_NOARRAY;
	}

	int nbits = cards[0] * cards[1];
	int ndata = (int)(hit_bitmapShapeIndex(nbits) + (hit_bitmapShapeOffset(nbits)==0 ? 0 : 1));

	ok = MPI_Bcast(hit_bShapeData(*shape),ndata,HIT_BITMAP_COMM_TYPE,0,comm);
	hit_mpiTestError(ok,"Error in bitmap hit_bitmapShapeBcastInternal");

	ok = MPI_Bcast(hit_cShapeNameList(*shape,0).names, cards[0], MPI_INT, 0, comm);
	hit_mpiTestError(ok,"Error in bitmap hit_bitmapShapeBcastInternal");
	ok = MPI_Bcast(hit_cShapeNameList(*shape,1).names, cards[1], MPI_INT, 0, comm);
	hit_mpiTestError(ok,"Error in bitmap hit_bitmapShapeBcastInternal");

	hit_cShapeNameList(*shape,0).nNames = cards[0];
	hit_cShapeNameList(*shape,1).nNames = cards[1];
	
}


HitLayout hit_layout_plug_layBitmap(int freeTopo, HitTopology topo, HitShape * shapeP){

	// 0. Broadcast the sparse shape
	hit_bShapeBcastInternal(shapeP,topo);
	
	HitShape shape = *shapeP;
	
	// 1. Create the layout
	HitLayout lay = HIT_LAYOUT_NULL;
	lay.topo = topo;
	lay.origShape = shape;

	// 2. Obtain the number of processors
	int procsCard = hit_topCard(topo);
	// @arturo 2015/01/22
	//int procId = topo.linearRank;
	int procId = hit_topSelfRankInternal( topo );
	HitSig input = hit_sig(0,hit_bShapeNvertices(shape),1);
	HitSig output;

	// Split the communicator
	// @javfres 2015-10-05 Split to support inactive processes
	//lay.active = hit_layout_plug_layBlocks_Sig(procId, procsCard,hit_bShapeNvertices(shape), 0.0, input, &output);
	lay.active = hit_layout_plug_layBlocks_Sig(procId, procsCard,hit_bShapeNvertices(shape), NULL, input, &output);
	lay.pTopology[0] = hit_ptopSplit(topo.pTopology, lay.active);
	int nactives = hit_min(hit_bShapeNvertices(shape), procsCard);
	lay.numActives[0] = nactives;

	hit_layout_list_initGroups(&lay, hit_bShapeNvertices(shape));
	int i;
	for(i=0;i<nactives;i++){
		hit_layout_list_addGroup(&lay, i, 1);
		HitSig part_i;
		//hit_layout_plug_layBlocks_Sig(i, nactives,hit_bShapeNvertices(shape), 0.0, input, &part_i);
		hit_layout_plug_layBlocks_Sig(i, nactives,hit_bShapeNvertices(shape), NULL, input, &part_i);
		int j;
		for(j=part_i.begin; j<=part_i.end; j++){
			lay.info.layoutList.assignedGroups[j] = i;
		}
	
	}
	
	int * names =  hit_bShapeNameList(shape,0).names + output.begin;
	lay.shape = hit_bShapeSelect(shape, hit_sigCard(output), names);

	if( freeTopo ) hit_topFree( topo );
	return lay;
}




void hit_cShapeBcastInternal(HitShape * shape, HitTopology topo){

	int ok;
	int cards[2];
	int nz;
	// @arturo Mar 2013
	//MPI_Comm comm = *((MPI_Comm *)topo.pTopology.lowLevel);
	MPI_Comm comm = topo.pTopology->comm;

	if(topo.pTopology->selfRank == 0){
		cards[0] = hit_cShapeCard(*shape,0);
		cards[1] = hit_cShapeCard(*shape,1);
		nz = hit_cShapeNZElems(*shape);
		MPI_Bcast(cards, 2, MPI_INT, 0, comm);
		MPI_Bcast(&nz, 1, MPI_INT, 0, comm);
	} else {
		MPI_Bcast(cards, 2, MPI_INT, 0, comm);
		MPI_Bcast(&nz, 1, MPI_INT, 0, comm);

		HitShape newShp = hit_csrShapeMatrix(cards[0],cards[1],nz);
		*shape = newShp;
	}

	// Send the flag for names.
	ok = MPI_Bcast(&(hit_cShapeNameList(*shape,0).flagNames),1,MPI_INT,0,comm);
	hit_mpiTestError(ok,"Broadcast error hit_cShapeBcastInternal");
	ok = MPI_Bcast(&(hit_cShapeNameList(*shape,1).flagNames),1,MPI_INT,0,comm);
	hit_mpiTestError(ok,"Broadcast error hit_cShapeBcastInternal");


	if(topo.pTopology->selfRank != 0){
		if(hit_cShapeNameList(*shape,0).flagNames == HIT_SHAPE_NAMES_ARRAY)
			hit_cShapeNameList(*shape,0).flagNames = HIT_SHAPE_NAMES_NOARRAY;
		if(hit_cShapeNameList(*shape,1).flagNames == HIT_SHAPE_NAMES_ARRAY)
			hit_cShapeNameList(*shape,1).flagNames = HIT_SHAPE_NAMES_NOARRAY;
	}

	ok = MPI_Bcast(hit_cShapeXadj(*shape), cards[0]+1, MPI_INT, 0, comm);
	hit_mpiTestError(ok,"Error in sparseShapeBcast");
	ok = MPI_Bcast(hit_cShapeAdjncy(*shape), nz, MPI_INT, 0, comm);
	hit_mpiTestError(ok,"Error in sparseShapeBcast");
	ok = MPI_Bcast(hit_cShapeNameList(*shape,0).names, cards[0], MPI_INT, 0, comm);
	hit_mpiTestError(ok,"Error in sparseShapeBcast");
	ok = MPI_Bcast(hit_cShapeNameList(*shape,1).names, cards[1], MPI_INT, 0, comm);
	hit_mpiTestError(ok,"Error in sparseShapeBcast");

	hit_cShapeNameList(*shape,0).nNames = cards[0];
	hit_cShapeNameList(*shape,1).nNames = cards[1];

}



HitLayout hit_layout_plug_laySparseRows(int freeTopo, HitTopology topo, HitShape * shapeP){

	// 0. Broadcast the sparse shape
	hit_cShapeBcastInternal(shapeP,topo);
	HitShape shape = *shapeP;

	// 1. Create the layout
	HitLayout lay = HIT_LAYOUT_NULL;
	lay.topo = topo;
	lay.origShape = shape;

	// 2. Obtain the number of processors
	int procsCard = hit_topCard(topo);
	// @arturo 2015/01/22
	//int procId = topo.linearRank;
	int procId = hit_topSelfRankInternal( topo );
	HitSig input = hit_sig(0,hit_cShapeNvertices(shape),1);
	HitSig output;

	// Split the communicator
	// @javfres 2015-10-05 Split to support inactive processes
	//lay.active = hit_layout_plug_layBlocks_Sig(procId, procsCard,  hit_cShapeCard(shape,0), 0.0, input, &output);
	lay.active = hit_layout_plug_layBlocks_Sig(procId, procsCard,  hit_cShapeCard(shape,0), NULL, input, &output);
	lay.pTopology[0] = hit_ptopSplit(topo.pTopology, lay.active);
	int nactives = hit_min(hit_cShapeCard(shape,0), procsCard);
	lay.numActives[0] = nactives;

	// @note @javfres This only work for whole rows
	hit_layout_list_initGroups(&lay, hit_cShapeCard(shape,0));

	int i;
	for(i=0;i<procsCard;i++){
		hit_layout_list_addGroup(&lay, i, 1);
		HitSig part_i;
		//hit_layout_plug_layBlocks_Sig(i, procsCard,hit_cShapeCard(shape,0), 0.0, input, &part_i);
		hit_layout_plug_layBlocks_Sig(i, procsCard,hit_cShapeCard(shape,0), NULL, input, &part_i);
		int j;
		for(j=part_i.begin; j<=part_i.end; j++){
			lay.info.layoutList.assignedGroups[j] = i;
		}

	}

	int * names = hit_cShapeNameList(shape,0).names + output.begin;
	lay.shape = hit_cShapeSelectRows(shape, hit_sigCard(output), names);

	if( freeTopo ) hit_topFree( topo );
	return lay;

}



HitLayout hit_layout_plug_laySparseBitmapRows(int freeTopo, HitTopology topo, HitShape * shapeP){

	// 0. Broadcast the sparse shape
	hit_bShapeBcastInternal(shapeP,topo);
	HitShape shape = *shapeP;

	// 1. Create the layout
	HitLayout lay = HIT_LAYOUT_NULL;
	lay.topo = topo;
	lay.pTopology[0] = hit_ptopDup( topo.pTopology );
	lay.origShape = shape;

	// 2. Obtain the number of processors
	int procsCard = hit_topCard(topo);
	// @arturo 2015/01/22
	//int procId = topo.linearRank;
	int procId = hit_topSelfRankInternal( topo );
	HitSig input = hit_sig(0,hit_bShapeNvertices(shape),1);
	HitSig output;

	// @javfres 2015-10-19 Split to support inactive processes
	//lay.active = hit_layout_plug_layBlocks_Sig(procId, procsCard,  hit_bShapeCard(shape,0), 0.0, input, &output);
	lay.active = hit_layout_plug_layBlocks_Sig(procId, procsCard,  hit_bShapeCard(shape,0), NULL, input, &output);
	lay.pTopology[0] = hit_ptopSplit(topo.pTopology, lay.active);
	int nactives = hit_min(hit_bShapeCard(shape,0), procsCard);
	lay.numActives[0] = nactives;


	// @note @javfres Esto solo funciona para filas enteras.
	hit_layout_list_initGroups(&lay, hit_bShapeCard(shape,0));

	int i;
	for(i=0;i<procsCard;i++){
		hit_layout_list_addGroup(&lay, i, 1);
		HitSig part_i;
		//hit_layout_plug_layBlocks_Sig(i, procsCard,hit_bShapeCard(shape,0), 0.0, input, &part_i);
		hit_layout_plug_layBlocks_Sig(i, procsCard,hit_bShapeCard(shape,0), NULL, input, &part_i);
		int j;
		for(j=part_i.begin; j<=part_i.end; j++){
			lay.info.layoutList.assignedGroups[j] = i;
		}
	}

	int * names = hit_bShapeNameList(shape,0).names + output.begin;
	lay.shape = hit_bShapeSelectRows(shape, hit_sigCard(output), names);

	if( freeTopo ) hit_topFree( topo );
	return lay;
}

