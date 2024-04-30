/**
 * Hitmap patter definition.
 * A pattern structure includes one or more communications.
 * These communications can be performed ordered or unordered.
 * 
 * @file hit_pattern.c
 * @version 1.0
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Jun 2010
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
#include <stdlib.h>
#include <hit_pattern.h>
#include <hit_com.h>
#include <hit_sshape.h>
#include <hit_cshape.h>
#include <hit_bshape.h>


/* Hit COMMUNICATION PATTERNS */
/* Hit INITIALIZE NULL PATTERN */
HitPattern HIT_PATTERN_NULL = HIT_PATTERN_NULL_STATIC;


/* Hit ADD COMM TO PATTERN */
void hit_patternAdd( HitPattern *pattern, HitCom comm ) {
	HitPatS *tmp;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(tmp,sizeof(HitPatS),HitPatS*);
	hit_malloc(tmp, HitPatS, 1);

	/* COPY COMM SPEC IN THE PATTERN */
	tmp->commSpec = comm;

	/* ADVANCE THE NUMBER OF COMM IN THE PATTERN */
	pattern->numComm++;

	/* LINK THE FIFO */
	tmp->next = NULL;
	if ( pattern->first == NULL ) {
		pattern->first = tmp;
		pattern->last = tmp;
	}
	else {
		pattern->last->next = tmp;
		pattern->last = tmp;
	}
}

void hit_patternCompose( HitPattern *pattern, HitPattern *pattern2 ) {

	pattern->last->next = pattern2->first;
	pattern->last = pattern2->last;
	pattern->numComm += pattern2->numComm;
}


/* Hit FREE PATTERN */
void hit_patternFree( HitPattern *pattern ) {
	HitPatS	*elem, *tmp;
	for (elem = pattern->first; elem != NULL; elem=tmp ) {
		tmp = elem->next;
		hit_comFree(elem->commSpec);
		free(elem);
	}
	pattern->numComm = 0;
	pattern->first = NULL;
	pattern->last = NULL;
}

/* Hit EXECUTE PATTERN: ORDERED MODE */
void hit_patternDoOrdered(HitPattern pattern) {
	HitPatS *elem;
	/* 1. EXECUTE COMM SPECIFICATIONS IN ORDER */
	for (elem = pattern.first; elem != NULL; elem = elem->next) {
		hit_comDo( &(elem->commSpec) );
	}
}


/* Hit EXECUTE PATTERN: UNORDERED MODE */
void hit_patternDoUnordered( HitPattern pattern ) {
	HitPatS	*elem;
	MPI_Request allRequest[ pattern.numComm * 2 ];
	int cont;

	// @arturo: TEST WITH THE WAIT HANDLERS IN SPLITTED ARRAYS
	//MPI_Request sendRequest[ pattern.numComm ];
	////MPI_Request recvRequest[ pattern.numComm ];

	/* 1. START ISENDs FOR ALL SPECIFICACIONS */
	cont = 0;
	for (elem = pattern.first; elem != NULL; elem=elem->next ) {
		if(elem->commSpec.commType == HIT_SENDRECV){
			hit_comStartSend( &(elem->commSpec) );
			allRequest[ cont*2 ] = elem->commSpec.requestSend;
	//		sendRequest[ cont ] = elem->commSpec.requestSend;
			hit_comStartRecv( &(elem->commSpec) );
			allRequest[ cont*2+1 ] = elem->commSpec.requestRecv;
	//		recvRequest[ cont ] = elem->commSpec.requestRecv;
			cont++;
		}
	}

	/* 2. CALL THE PROPER COUPLED/COLECTIVE FUNCTIONS TO DO THE PATTERN */
	for (elem = pattern.first; elem != NULL; elem=elem->next ) {
		switch (elem->commSpec.commType) {
			case HIT_COMTYPE_NULL:
				/* SKIP */
				break;
			case HIT_SENDRECV:
				/* NOTHING */
				break;
			case HIT_SENDRECV_REPLACE:
				hit_comDoSendRecvReplace(&(elem->commSpec));
				break;
			case HIT_REDUCE:
				hit_comDoReduce(&(elem->commSpec));
				break;
			case HIT_ALLTOALL:
				hit_comDoAlltoall(&(elem->commSpec));
				break;
			case HIT_BROADCAST:
				hit_comDoBroadcast(&(elem->commSpec));
				break;
			case HIT_ALLTOALLV:
				hit_comDoAlltoallv(&(elem->commSpec));
				break;
			default:
				hit_errInternal("patternDoUnordered", "Unknown type of communication object", "", __FILE__, __LINE__);
				break;
		}
	}

	/* 3. WAIT FOR COUPLED ISEND TO COMPLETE */
#ifdef DEBUG
		printf("CTRL %d - Waiting sends and recvs\n", hit_Rank);
#endif
	int ok = MPI_Waitall( cont*2, allRequest, MPI_STATUSES_IGNORE );
	hit_mpiTestError(ok,"Failed wait all");
#ifdef DEBUG
		printf("CTRL %d - End waiting sends and recvs\n", hit_Rank);
#endif
	//int ok = MPI_Waitall( cont, recvRequest, MPI_STATUSES_IGNORE );
	////hit_mpiTestError(ok,"Failed wait all");
	//ok = MPI_Waitall( cont, sendRequest, MPI_STATUSES_IGNORE );
	//hit_mpiTestError(ok,"Failed wait all");
}


/* Hit EXECUTE PATTERN: START ASYNC */
void hit_patternStartAsync( HitPattern pattern ) {
	HitPatS	*elem;

	/* 1. START ISEND/IRECV FOR ALL SPECIFICACIONS */
	for (elem = pattern.first; elem != NULL; elem=elem->next ) {
		if(elem->commSpec.commType == HIT_SENDRECV){
			hit_comStartSend( &(elem->commSpec) );
			hit_comStartRecv( &(elem->commSpec) );
		}
	}
}

/* Hit EXECUTE PATTERN: END ASYNC */
void hit_patternEndAsync( HitPattern pattern ) {
	HitPatS	*elem;
	MPI_Request allRequest[ pattern.numComm * 2 ];
	int cont;

	/* 1. COLLECT REQUEST OBJECTS IN AN ARRAY */
	cont = 0;
	for (elem = pattern.first; elem != NULL; elem=elem->next ) {
		if(elem->commSpec.commType == HIT_SENDRECV){
			allRequest[ cont*2 ] = elem->commSpec.requestSend;
			allRequest[ cont*2+1 ] = elem->commSpec.requestRecv;
			cont++;
		}
	}

	/* 2. CALL THE PROPER COUPLED/COLECTIVE FUNCTIONS TO DO THE PATTERN */
	for (elem = pattern.first; elem != NULL; elem=elem->next ) {
		switch (elem->commSpec.commType) {
			case HIT_COMTYPE_NULL:
				/* SKIP */
				break;
			case HIT_SENDRECV:
				/* NOTHING */
				break;
			case HIT_SENDRECV_REPLACE:
				hit_comDoSendRecvReplace(&(elem->commSpec));
				break;
			case HIT_REDUCE:
				hit_comDoReduce(&(elem->commSpec));
				break;
			case HIT_ALLTOALL:
				hit_comDoAlltoall(&(elem->commSpec));
				break;
			case HIT_BROADCAST:
				hit_comDoBroadcast(&(elem->commSpec));
				break;
			case HIT_ALLTOALLV:
				hit_comDoAlltoallv(&(elem->commSpec));
				break;
			default:
				hit_errInternal("patternDoUnordered", "Unknown type of communication object", "", __FILE__, __LINE__);
				break;
		}
	}

	/* 3. WAIT FOR COUPLED ISEND/IRECV TO COMPLETE */
#ifdef DEBUG
		printf("CTRL %d - Waiting sends and recvs\n", hit_Rank);
#endif
	int ok = MPI_Waitall( cont*2, allRequest, MPI_STATUSES_IGNORE );
	hit_mpiTestError(ok,"Failed wait all");
}



/* Hit PATTERN: SPECIFIC PATTERNS FOR SPECIAL COMPLEX COMMUNICATIONS */

/*
 * @arturo Jan 2015
 * hit_patternLayRedistribute2: Redistribute a tile allocated with a given layout,
 * 		as indicated by another layout, WHEN topologies of the layouts are different.
 * 		For layouts using the same topology see hit_patternLayRedistribute
 * 		Use hit_patterLayRedistribute as front-end for any case.
 *
 * 		Restrictions:
 * 		1) The tile should have been allocated using the local shape from the first layout
 */
#define HIT_PAT_REDISTRIBUTE2_TAG	15005
HitPattern hit_patternLayRedistribute2(	HitLayout lay1, HitLayout lay2, void *tileP1, void *tileP2, HitType baseType ) {
	int i;

	// @arturo 2015/01/03: Eliminate the requirement for topologies associated to both layouts
	//		being the same
	/* 1. CHECK THAT THE TOPOLOGY IS THE SAME IN BOTH LAYOUTS */
	/*
	if ( ! hit_ptopoCmp( lay1.topo.pTopology, lay2.topo.pTopology ) ) 
		hit_errInternal( __FUNCTION__, "Layouts with different topologies", "", __FILE__, __LINE__ );
	*/

	/* 2. SKIP IF MY PROC IS NOT ACTIVE IN ANY LAYOUT */
	if ( ! hit_layImActive( lay1 ) && ! hit_layImActive( lay2 ) ) return HIT_PATTERN_NULL;

	/* 3. DECLARE THE NEW COMM. PATTERN */
	HitPattern allToAll = hit_pattern( HIT_PAT_UNORDERED );
	// @arturo 2015/01/03 Two possible number of processors when topologies are not the same
	//int myRank = lay1.topo.linearRank;
	//int numProcs = hit_topCard( lay1.topo );
	// @arturo 2015/01/22
	//int myRank1 = lay1.topo.linearRank;
	//int myRank2 = lay1.topo.linearRank;
	int myRank1 = hit_topSelfRankInternal( lay1.topo );
	int myRank2 = hit_topSelfRankInternal( lay2.topo );
	int numProcs1 = hit_topCard( lay1.topo );
	int numProcs2 = hit_topCard( lay2.topo );

	// @arturo 2015/01/03: False layout to contain the global topology communicator
	MPI_Group gLay1;
	MPI_Group gLay2;
	MPI_Group gGlobal;
	MPI_Comm_group( lay1.topo.pTopology->global->comm, &gGlobal );
	// @arturo 2015/01/07 Bug corrected: Get info for the active group on each layout
	if ( lay1.topo.active ) MPI_Comm_group( lay1.topo.pTopology->comm, &gLay1 );
	else gLay1 = lay1.topo.pTopology->antiCommGroup;
	if ( lay2.topo.active ) MPI_Comm_group( lay2.topo.pTopology->comm, &gLay2 );
	else gLay2 = lay2.topo.pTopology->antiCommGroup;

	int myRankGlobal = lay1.topo.pTopology->global->selfRank;

	HitLayout fooGlobal = HIT_LAYOUT_NULL;
	fooGlobal.active = 1;
	fooGlobal.topo.pTopology = lay1.topo.pTopology->global;
	fooGlobal.topo.numDims = 1;
	fooGlobal.topo.card[0] = fooGlobal.topo.pTopology->numProcs;
	fooGlobal.topo.active = 1;
	//fooGlobal.topo.linearRank = myRankGlobal;

#ifdef DEBUG
printf("Ranks: Lay1: %d, Lay2: %d, Global: %d\n", myRank1, myRank2, myRankGlobal );
#endif

	/* 4. ONLY COMPUTE SENDS IF I'M ACTIVE IN THE FIRST LAYOUT (I HAVE DATA) */
	if ( hit_layImActive( lay1 ) ) {
		HitShape localShp = hit_layShape( lay1 );

#ifdef DEBUG
printf("Ranks: Lay1: %d, Lay2: %d, Global: %d ACTIVO en Lay1\n", myRank1, myRank2, myRankGlobal );
#endif
		/* FOR ALL PROCESSORS IN THE TOPOLOGY */
		/*		(FUNCTIONS INTERNALLY CHECK ACTIVE STATUS IN TOPOLOGY AND LAYOUT) */
		/*		REORDER THE ORDER OF EXPLORATION TO CREATE A SKEWED PATTERN AND
		 *		AVOID POTENTIAL COMM. BOTTLENECKS 	*/
		// @arturo 2015/01/03 Two possible number of processors when topologies are not the same
		//for ( i=0; i<numProcs; i++ ) {
		//	int foreignId = ( myRank + i ) % numProcs;
		for ( i=0; i<numProcs2; i++ ) {
			// Perhaps this processor is not active in lay2. For skewing, it does not matter
			// what is the real processor we start at. It is only important that they
			// are skewed. Thus, I use here myRank1 anyway. Thus, each processor in lay1
			// start the communications in a different processors of lay2 (if possible, lay2
			// may contain less processors than lay1)
			int foreignId = ( myRank1 + i ) % numProcs2;
			int foreignIdGlobal;
			MPI_Group_translate_ranks( gLay2, 1, &foreignId, gGlobal, &foreignIdGlobal );

			HitRanks foreignProc1 = hit_topRanksInternal( lay1.topo, foreignId );
			HitRanks foreignProc2 = hit_topRanksInternal( lay2.topo, foreignId );
			HitShape foreignShp1 = hit_layShapeOther( lay1, foreignProc1 );
			HitShape foreignShp2 = hit_layShapeOther( lay2, foreignProc2 );
#ifdef DEBUG
printf("[%d] Topology: NumDims: %d, Cards:(%d,%d)\n", hit_Rank,
				lay1.topo.numDims, lay1.topo.card[0], 
				lay1.topo.card[1] );
printf("[%d] Send Comprobando foreig: %d (%d) Ranks1:(%d,%d), Ranks2(%d,%d)\n", hit_Rank, foreignId, foreignIdGlobal,
	 			foreignProc1.rank[0],
	 			foreignProc1.rank[1],
	 			foreignProc2.rank[0],
	 			foreignProc2.rank[1]
	  );
#endif

			/* INTERSECTION: NON-EMPTY IMPLIES COMMUNICATION */
			/* FOR COPY SIG. LAYOUTS: SKIP COMM. IF THE FOREIGN PROC. ALREADY HAS THIS SHAPE */
			HitShape overlapShp2 = hit_shapeIntersect( localShp, foreignShp2 );
			HitShape alreadyThere = hit_shapeIntersect( overlapShp2, foreignShp1 );

#ifdef DEBUG
printf("[%d] Send shapes: f1 [%d:%d][%d:%d], f2:[%d:%d][%d:%d], over2:[%d:%d][%d:%d], alreadyThere:[%d:%d][%d:%d]\n", hit_Rank, 
			hit_shapeSig(foreignShp1,0).begin,
			hit_shapeSig(foreignShp1,0).end,
			hit_shapeSig(foreignShp1,1).begin,
			hit_shapeSig(foreignShp1,1).end,
			hit_shapeSig(foreignShp2,0).begin,
			hit_shapeSig(foreignShp2,0).end,
			hit_shapeSig(foreignShp2,1).begin,
			hit_shapeSig(foreignShp2,1).end,
			hit_shapeSig(overlapShp2,0).begin,
			hit_shapeSig(overlapShp2,0).end,
			hit_shapeSig(overlapShp2,1).begin,
			hit_shapeSig(overlapShp2,1).end,
			hit_shapeSig(alreadyThere,0).begin,
			hit_shapeSig(alreadyThere,0).end,
			hit_shapeSig(alreadyThere,1).begin,
			hit_shapeSig(alreadyThere,1).end
	  );
#endif
			// In this comparison, i==0 is substituted by the global rank of the local
			// proc. and the foreign proc are the same, as the local proc. can have different 
			// ranks in the different topologies.
			//if ( ! hit_shapeCmp( overlapShp2, HIT_SHAPE_NULL )
			//		&& ( i == 0 || hit_shapeCmp( alreadyThere, HIT_SHAPE_NULL ) ) ) {
			if ( ! hit_shapeCmp( overlapShp2, HIT_SHAPE_NULL )
					&& ( myRankGlobal == foreignIdGlobal 
							|| hit_shapeCmp( alreadyThere, HIT_SHAPE_NULL ) ) ) {

#ifdef DEBUG
fprintf(stderr, "%s Adding send from %d to %d with shape %d [%d:%d:%d][%d:%d:%d]\n", __FUNCTION__, myRank1, foreignId,
		hit_shapeDims( overlapShp2 ),
		hit_shapeSig( overlapShp2, 0 ).begin,
		hit_shapeSig( overlapShp2, 0 ).end,
		hit_shapeSig( overlapShp2, 0 ).stride,
		hit_shapeSig( overlapShp2, 1 ).begin,
		hit_shapeSig( overlapShp2, 1 ).end,
		hit_shapeSig( overlapShp2, 1 ).stride
	   );

printf("Ranks: Lay1: %d, Lay2: %d, Global: %d COM to RLay2: %d, RGlobal: %d\n", myRank1, myRank2, myRankGlobal, foreignId, foreignIdGlobal  );
#endif

				// @arturo 2015/01/03 Translate to global ranks for the foo Layout
				HitRanks foreignProcGlobal = HIT_RANKS_NULL;
				foreignProcGlobal.rank[0] = foreignIdGlobal;

				// @arturo 2015/01/03 Use the foo layout in order to use the global communicator
				/*
				hit_patternAdd( &allToAll, 
					hit_comSendSelectTag( 
						lay2, foreignProc, tileP1, overlapShp2, HIT_COM_ARRAYCOORDS, baseType, 
						HIT_PAT_REDISTRIBUTE_TAG
					)
				);
				*/
				hit_patternAdd( &allToAll, 
					hit_comSendSelectTag( 
						fooGlobal, foreignProcGlobal, tileP1, overlapShp2, HIT_COM_ARRAYCOORDS, 
							baseType, HIT_PAT_REDISTRIBUTE2_TAG
					)
				);
			}
		}
	}

	/* 6. ONLY COMPUTE RECEIVES IF I'M ACTIVE IN THE SECOND LAYOUT (I WILL HAVE DATA) */
	if ( hit_layImActive( lay2 ) ) {
		HitShape localShp1 = hit_layShape( lay1 );
		HitShape localShp2 = hit_layShape( lay2 );

#ifdef DEBUG
printf("Ranks: Lay1: %d, Lay2: %d, Global: %d ACTIVO en Lay2\n", myRank1, myRank2, myRankGlobal );
#endif

		/* FOR ALL PROCESSORS IN THE TOPOLOGY */
		/*		(FUNCTIONS INTERNALLY CHECK ACTIVE STATUS IN TOPOLOGY AND LAYOUT) */
		/*		REORDER THE ORDER OF EXPLORATION TO CREATE A SKEWED PATTERN AND
		 *		AVOID POTENTIAL COMM. BOTTLENECKS 	*/
		//for ( i=0; i<numProcs; i++ ) {
		//	int foreignId = ( myRank + i ) % numProcs;
		for ( i=0; i<numProcs1; i++ ) {
			int foreignId = ( myRank2 + i ) % numProcs1;
			int foreignIdGlobal;
			MPI_Group_translate_ranks( gLay1, 1, &foreignId, gGlobal, &foreignIdGlobal );
#ifdef DEBUG
printf("[%d] Recv Comprobando foreig: %d (%d)\n", hit_Rank, foreignId, foreignIdGlobal );
#endif

			HitRanks foreignProc = hit_topRanksInternal( lay1.topo, foreignId );
#ifdef DEBUG
printf("[%d] Foreign Ranks: %d,%d,%d,%d\n", hit_Rank,
			foreignProc.rank[0],
			foreignProc.rank[1],
			foreignProc.rank[2],
			foreignProc.rank[3]
	  );
#endif
			HitShape foreignShp1 = hit_layShapeOther( lay1, foreignProc );

			/* INTERSECTION: NON-EMPTY IMPLIES COMMUNICATION */
			/* FOR COPY SIG. LAYOUTS: SKIP COMM. IF THE FOREIGN PROC. ALREADY HAS THIS SHAPE */
			HitShape overlapShp = hit_shapeIntersect( localShp2, foreignShp1 );
			HitShape alreadyHere = hit_shapeIntersect( overlapShp, localShp1 );

#ifdef DEBUG
printf("[%d] After shapes: %d (%d)\n", hit_Rank, foreignId, foreignIdGlobal );
#endif
			//if ( ! hit_shapeCmp( overlapShp, HIT_SHAPE_NULL ) 
			//		&& ( i == 0 || hit_shapeCmp( alreadyHere, HIT_SHAPE_NULL ) ) ) {
			if ( ! hit_shapeCmp( overlapShp, HIT_SHAPE_NULL )
					&& ( myRankGlobal == foreignIdGlobal 
							|| hit_shapeCmp( alreadyHere, HIT_SHAPE_NULL ) ) ) {

#ifdef DEBUG
fprintf(stderr, "%s Adding recv from %d to %d with shape %d [%d:%d:%d][%d:%d:%d]\n", __FUNCTION__, foreignId, myRank2,
		hit_shapeDims( overlapShp ),
		hit_shapeSig( overlapShp, 0 ).begin,
		hit_shapeSig( overlapShp, 0 ).end,
		hit_shapeSig( overlapShp, 0 ).stride,
		hit_shapeSig( overlapShp, 1 ).begin,
		hit_shapeSig( overlapShp, 1 ).end,
		hit_shapeSig( overlapShp, 1 ).stride
	   );
#endif
				// @arturo 2015/01/03 Translate to global ranks for the foo Layout
				HitRanks foreignProcGlobal = HIT_RANKS_NULL;
				foreignProcGlobal.rank[0] = foreignIdGlobal;

#ifdef DEBUG
printf("Ranks: Lay1: %d, Lay2: %d, Global: %d COM to RLay2: %d, RGlobal: %d\n", myRank1, myRank2, myRankGlobal, foreignId, foreignIdGlobal  );
#endif
				/*
				hit_patternAdd( &allToAll, 
					hit_comRecvSelectTag( 
						lay1, foreignProc, tileP2, overlapShp, HIT_COM_ARRAYCOORDS, baseType,
						HIT_PAT_REDISTRIBUTE_TAG
					)
				);
				*/
				hit_patternAdd( &allToAll, 
					hit_comRecvSelectTag( 
						fooGlobal, foreignProcGlobal, tileP2, overlapShp, HIT_COM_ARRAYCOORDS, 
							baseType, HIT_PAT_REDISTRIBUTE2_TAG
					)
				);
			}
#ifdef DEBUG
			else 
				printf("[%d] Fail condition: %d (%d)\n", hit_Rank, foreignId, foreignIdGlobal );
#endif
		}
	}

	/* 7. RETURN */
	return allToAll;
}

/*
 * @arturo Feb 2013
 * hit_patternLayRedistribute: Redistribute a tile allocated with a given layout,
 * 		as indicated by another layout.
 * 		Restrictions:
 * 		1) The two layouts should have been built using the same topology
 * 			(Eliminated: It calls to hit_patternRedistribute2() that considers that case)
 * 		2) The tile should have been allocated using the local shape from the first layout
 */
#define HIT_PAT_REDISTRIBUTE_TAG	15001
HitPattern hit_patternLayRedistribute(	HitLayout lay1, HitLayout lay2, void *tileP1, void *tileP2, HitType baseType ) {
	int i;

	/* 1. CHECK THAT THE TOPOLOGY IS THE SAME IN BOTH LAYOUTS */
	// @arturo 2015/01/05 In case topologies are different, call the Redistribute2 function
	/*
	if ( ! hit_ptopoCmp( lay1.topo.pTopology, lay2.topo.pTopology ) ) 
		hit_errInternal( __FUNCTION__, "Layouts with different topologies", "", __FILE__, __LINE__ );
	*/
	if ( ! hit_ptopoCmp( lay1.topo.pTopology, lay2.topo.pTopology ) )
		return hit_patternLayRedistribute2( lay1, lay2, tileP1, tileP2, baseType );

	/* 2. SKIP IF MY PROC IS NOT ACTIVE IN ANY LAYOUT */
	if ( ! hit_layImActive( lay1 ) && ! hit_layImActive( lay2 ) ) return HIT_PATTERN_NULL;

	/* 3. DECLARE THE NEW COMM. PATTERN */
	HitPattern allToAll = hit_pattern( HIT_PAT_UNORDERED );
	// @arturo 2015/01/22
	//int myRank = lay1.topo.linearRank;
	int myRank = hit_topSelfRankInternal( lay1.topo );
	int numProcs = hit_topCard( lay1.topo );

	/* 4. ONLY COMPUTE SENDS IF I'M ACTIVE IN THE FIRST LAYOUT (I HAVE DATA) */
	if ( hit_layImActive( lay1 ) ) {
		HitShape localShp = hit_layShape( lay1 );

		/* FOR ALL PROCESSORS IN THE TOPOLOGY */
		/*		(FUNCTIONS INTERNALLY CHECK ACTIVE STATUS IN TOPOLOGY AND LAYOUT) */
		/*		REORDER THE ORDER OF EXPLORATION TO CREATE A SKEWED PATTERN AND
		 *		AVOID POTENTIAL COMM. BOTTLENECKS 	*/
		for ( i=0; i<numProcs; i++ ) {
			int foreignId = ( myRank + i ) % numProcs;

			HitRanks foreignProc = hit_topRanksInternal( lay2.topo, foreignId );
			HitShape foreignShp1 = hit_layShapeOther( lay1, foreignProc );
			HitShape foreignShp2 = hit_layShapeOther( lay2, foreignProc );

			/* INTERSECTION: NON-EMPTY IMPLIES COMMUNICATION */
			/* FOR COPY SIG. LAYOUTS: SKIP COMM. IF THE FOREIGN PROC. ALREADY HAS THIS SHAPE */
			HitShape overlapShp2 = hit_shapeIntersect( localShp, foreignShp2 );
			HitShape alreadyThere = hit_shapeIntersect( overlapShp2, foreignShp1 );

			if ( ! hit_shapeCmp( overlapShp2, HIT_SHAPE_NULL )
					&& ( i == 0 || hit_shapeCmp( alreadyThere, HIT_SHAPE_NULL ) ) ) {

#ifdef DEBUG
fprintf(stderr, "%s Adding send from %d to %d with shape %d [%d:%d:%d][%d:%d:%d][%d:%d:%d]\n", __FUNCTION__, myRank, foreignId,
		hit_shapeDims( overlapShp2 ),
		hit_shapeSig( overlapShp2, 0 ).begin,
		hit_shapeSig( overlapShp2, 0 ).end,
		hit_shapeSig( overlapShp2, 0 ).stride,
		hit_shapeSig( overlapShp2, 1 ).begin,
		hit_shapeSig( overlapShp2, 1 ).end,
		hit_shapeSig( overlapShp2, 1 ).stride,
		hit_shapeSig( overlapShp2, 2 ).begin,
		hit_shapeSig( overlapShp2, 2 ).end,
		hit_shapeSig( overlapShp2, 2 ).stride
	   );
#endif
				hit_patternAdd( &allToAll, 
					hit_comSendSelectTag( 
						lay2, foreignProc, tileP1, overlapShp2, HIT_COM_ARRAYCOORDS, baseType, 
						HIT_PAT_REDISTRIBUTE_TAG
					)
				);
			}
		}
	}

	/* 6. ONLY COMPUTE RECEIVES IF I'M ACTIVE IN THE SECOND LAYOUT (I WILL HAVE DATA) */
	if ( hit_layImActive( lay2 ) ) {
		HitShape localShp1 = hit_layShape( lay1 );
		HitShape localShp2 = hit_layShape( lay2 );

		/* FOR ALL PROCESSORS IN THE TOPOLOGY */
		/*		(FUNCTIONS INTERNALLY CHECK ACTIVE STATUS IN TOPOLOGY AND LAYOUT) */
		/*		REORDER THE ORDER OF EXPLORATION TO CREATE A SKEWED PATTERN AND
		 *		AVOID POTENTIAL COMM. BOTTLENECKS 	*/
		for ( i=0; i<numProcs; i++ ) {
			int foreignId = ( myRank + i ) % numProcs;

			HitRanks foreignProc = hit_topRanksInternal( lay1.topo, foreignId );
			HitShape foreignShp1 = hit_layShapeOther( lay1, foreignProc );

			/* INTERSECTION: NON-EMPTY IMPLIES COMMUNICATION */
			/* FOR COPY SIG. LAYOUTS: SKIP COMM. IF THE FOREIGN PROC. ALREADY HAS THIS SHAPE */
			HitShape overlapShp = hit_shapeIntersect( localShp2, foreignShp1 );
			HitShape alreadyHere = hit_shapeIntersect( overlapShp, localShp1 );

			if ( ! hit_shapeCmp( overlapShp, HIT_SHAPE_NULL ) 
					&& ( i == 0 || hit_shapeCmp( alreadyHere, HIT_SHAPE_NULL ) ) ) {

#ifdef DEBUG
fprintf(stderr, "%s Adding recv from %d to %d with shape %d [%d:%d:%d][%d:%d:%d][%d:%d:%d]\n", __FUNCTION__, foreignId, myRank,
		hit_shapeDims( overlapShp ),
		hit_shapeSig( overlapShp, 0 ).begin,
		hit_shapeSig( overlapShp, 0 ).end,
		hit_shapeSig( overlapShp, 0 ).stride,
		hit_shapeSig( overlapShp, 1 ).begin,
		hit_shapeSig( overlapShp, 1 ).end,
		hit_shapeSig( overlapShp, 1 ).stride,
		hit_shapeSig( overlapShp, 2 ).begin,
		hit_shapeSig( overlapShp, 2 ).end,
		hit_shapeSig( overlapShp, 2 ).stride
	   );
#endif
				hit_patternAdd( &allToAll, 
					hit_comRecvSelectTag( 
						lay1, foreignProc, tileP2, overlapShp, HIT_COM_ARRAYCOORDS, baseType,
						HIT_PAT_REDISTRIBUTE_TAG
					)
				);
			}
		}
	}

	/* 7. RETURN */
	return allToAll;
#undef DEBUG
}


/*
 * @arturo Feb 2013
 * hit_patternRedistributeCom: Redistribute data of a distributed tile into another distributed tile
 * wihout using layout information of the remote shapes. The shapes are communicated across all
 */
#define HIT_PAT_REDISTRIBUTE_TAG2	15002
HitPattern hit_patternRedistributeCom(	HitLayout lay, void *tileP1, void *tileP2, HitType baseType, int flagCompact ) {
	int i;
	HitTile tile1 = *(HitTile *)tileP1;
	HitTile tile2 = *(HitTile *)tileP2;


	/* 0. SKIP IF MY PROC IS NOT ACTIVE IN THE LAYOUT */
	if ( ! hit_layImActive( lay ) ) return HIT_PATTERN_NULL;

	/* 1. COMMUNICATE THE SHAPES OF ALL THE PROCESSES WITH ALLGATHER */
	//int myRank = lay.topo.linearRank;
	//int numProcs = hit_topCard( lay.topo );
	int myRank = lay.pTopology[0]->selfRank;
	int numProcs = lay.pTopology[0]->numProcs;
	HitSigShape tileSigShapes[ numProcs ][2];
	HitShape tileShapes[ numProcs ][2];
	// @javfres Initialize this to avoid warnings
	HitShape tileShapesOrig[2] = {HIT_SHAPE_NULL_STATIC,HIT_SHAPE_NULL_STATIC};
	int prefixSums[2][ HIT_MAXDIMS ];

	/*
	//if ( ! hit_topImActive( hit_layTopology( lay ) ) ) {
	if ( ! hit_layImActive( lay ) ) {
		tileSigShapes[ myRank ][0] = hit_sShapeAccess( HIT_SHAPE_NULL );
		tileSigShapes[ myRank ][1] = hit_sShapeAccess( HIT_SHAPE_NULL );
	} else {
	*/
		tileSigShapes[ myRank ][0] = hit_sShapeAccess( hit_tileShape( tile1 ) );
		tileSigShapes[ myRank ][1] = hit_sShapeAccess( hit_tileShape( tile2 ) );
	/*
	}
	*/

#ifdef DEBUG
printf("[%d](%d) CTRL Redistribute: Before Allgather Local Send[%d:%d:%d][%d:%d:%d] Recv[%d:%d:%d][%d:%d:%d]\n",
		hit_Rank, numProcs,
		tileSigShapes[ myRank ][ 0 ].sig[ 0 ].begin,
		tileSigShapes[ myRank ][ 0 ].sig[ 0 ].end,
		tileSigShapes[ myRank ][ 0 ].sig[ 0 ].stride,
		tileSigShapes[ myRank ][ 0 ].sig[ 1 ].begin,
		tileSigShapes[ myRank ][ 0 ].sig[ 1 ].end,
		tileSigShapes[ myRank ][ 0 ].sig[ 1 ].stride,
		tileSigShapes[ myRank ][ 1 ].sig[ 0 ].begin,
		tileSigShapes[ myRank ][ 1 ].sig[ 0 ].end,
		tileSigShapes[ myRank ][ 1 ].sig[ 0 ].stride,
		tileSigShapes[ myRank ][ 1 ].sig[ 1 ].begin,
		tileSigShapes[ myRank ][ 1 ].sig[ 1 ].end,
		tileSigShapes[ myRank ][ 1 ].sig[ 1 ].stride
	  );
#endif

	int ok = MPI_Allgather( MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, tileSigShapes, 2, HIT_SHAPE_SIG,
					//*( (MPI_Comm *)(lay.topo.pTopology.lowLevel)) );
					//*( (MPI_Comm *)(lay.pTopology[0].lowLevel)) );
					lay.pTopology[0]->comm );
	hit_mpiTestError( ok, "Communication shapes with Allgather");

	/* 2. SKIP IF MY PROC IS NOT ACTIVE IN THE TOPOLOGY */
	// if ( ! hit_topImActive( hit_layTopology( lay ) ) ) return HIT_PATTERN_NULL;

	/* 3. BUILD SHAPES */
	int acumCard[ HIT_MAXDIMS ][2];
	for( i=0; i<HIT_MAXDIMS; i++) {
		acumCard[i][0] = 0;
		acumCard[i][1] = 0;
	}
	for( i=0; i<numProcs; i++ ) {
		/* 3.1. BUILD SHAPE OBJECT FROM SIGNATURE */
		tileShapes[i][0] = hit_shapeFromSigShape( tileSigShapes[i][0] );
		tileShapes[i][1] = hit_shapeFromSigShape( tileSigShapes[i][1] );

#ifdef DEBUG
printf("[%d] CTRL 2 After Allgather(A) %d Send[%d:%d:%d][%d:%d:%d] Recv[%d:%d:%d][%d:%d:%d]\n",
		hit_Rank,
		i,
		tileSigShapes[ i ][ 0 ].sig[ 0 ].begin,
		tileSigShapes[ i ][ 0 ].sig[ 0 ].end,
		tileSigShapes[ i ][ 0 ].sig[ 0 ].stride,
		tileSigShapes[ i ][ 0 ].sig[ 1 ].begin,
		tileSigShapes[ i ][ 0 ].sig[ 1 ].end,
		tileSigShapes[ i ][ 0 ].sig[ 1 ].stride,
		tileSigShapes[ i ][ 1 ].sig[ 0 ].begin,
		tileSigShapes[ i ][ 1 ].sig[ 0 ].end,
		tileSigShapes[ i ][ 1 ].sig[ 0 ].stride,
		tileSigShapes[ i ][ 1 ].sig[ 1 ].begin,
		tileSigShapes[ i ][ 1 ].sig[ 1 ].end,
		tileSigShapes[ i ][ 1 ].sig[ 1 ].stride
	  );
#endif
		/* 3.2. TRANSLATION OF SHAPES FOR COMPACTED COORDINATES */
		if ( flagCompact ) {
			int j;
			for ( j = 0; j < hit_shapeDims( tileShapes[i][0] ); j++ ) {
				if ( i == myRank )  prefixSums[0][j] = acumCard[j][0];
				if ( ! hit_sigCmp( tileSigShapes[i][0].sig[j], HIT_SIG_NULL ) ) {
					int card0 = hit_sigCard( tileSigShapes[i][0].sig[j] );
					tileSigShapes[i][0].sig[j].begin = acumCard[j][0];
					tileSigShapes[i][0].sig[j].end = acumCard[j][0] + card0 - 1;
					tileSigShapes[i][0].sig[j].stride = 1;
					acumCard[j][0] = acumCard[j][0] + card0;
				}
			}
			for ( j = 0; j < hit_shapeDims( tileShapes[i][1] ); j++ ) {
				if ( i == myRank ) prefixSums[1][j] = acumCard[j][1];
				if ( ! hit_sigCmp( tileSigShapes[i][1].sig[j], HIT_SIG_NULL ) ) {
					int card1 = hit_sigCard( tileSigShapes[i][1].sig[j] );
					tileSigShapes[i][1].sig[j].begin = acumCard[j][1];
					tileSigShapes[i][1].sig[j].end = acumCard[j][1] + card1 - 1;
					tileSigShapes[i][1].sig[j].stride = 1;
					acumCard[j][1] = acumCard[j][1] + card1;
				}
			}

			if ( i == myRank ) {
				/* STORE ORIGINAL SHAPES TO TRANSLATE BACK */
				tileShapesOrig[0] = tileShapes[i][0];
				tileShapesOrig[1] = tileShapes[i][1];
			}
			/* SAVE SHAPES WITH NEW COMPACTED COORDINATES */
			tileShapes[i][0] = hit_shapeFromSigShape( tileSigShapes[i][0] );
			tileShapes[i][1] = hit_shapeFromSigShape( tileSigShapes[i][1] );
		}
#ifdef DEBUG
printf("[%d] CTRL 2 After Allgather(C) %d Send[%d:%d:%d][%d:%d:%d] Recv[%d:%d:%d][%d:%d:%d]\n",
		hit_Rank,
		i,
		/*
		hit_shapeSig( tileShapes[ i ][ 0 ], 0 ).begin,
		hit_shapeSig( tileShapes[ i ][ 0 ], 0 ).end,
		hit_shapeSig( tileShapes[ i ][ 0 ], 0 ).stride,
		hit_shapeSig( tileShapes[ i ][ 0 ], 1 ).begin,
		hit_shapeSig( tileShapes[ i ][ 0 ], 1 ).end,
		hit_shapeSig( tileShapes[ i ][ 0 ], 1 ).stride,
		hit_shapeSig( tileShapes[ i ][ 1 ], 0 ).begin,
		hit_shapeSig( tileShapes[ i ][ 1 ], 0 ).end,
		hit_shapeSig( tileShapes[ i ][ 1 ], 0 ).stride,
		hit_shapeSig( tileShapes[ i ][ 1 ], 1 ).begin,
		hit_shapeSig( tileShapes[ i ][ 1 ], 1 ).end,
		hit_shapeSig( tileShapes[ i ][ 1 ], 1 ).stride
		*/
		tileSigShapes[ i ][ 0 ].sig[ 0 ].begin,
		tileSigShapes[ i ][ 0 ].sig[ 0 ].end,
		tileSigShapes[ i ][ 0 ].sig[ 0 ].stride,
		tileSigShapes[ i ][ 0 ].sig[ 1 ].begin,
		tileSigShapes[ i ][ 0 ].sig[ 1 ].end,
		tileSigShapes[ i ][ 0 ].sig[ 1 ].stride,
		tileSigShapes[ i ][ 1 ].sig[ 0 ].begin,
		tileSigShapes[ i ][ 1 ].sig[ 0 ].end,
		tileSigShapes[ i ][ 1 ].sig[ 0 ].stride,
		tileSigShapes[ i ][ 1 ].sig[ 1 ].begin,
		tileSigShapes[ i ][ 1 ].sig[ 1 ].end,
		tileSigShapes[ i ][ 1 ].sig[ 1 ].stride
	  );
#endif
	}

	/* 4. DECLARE THE NEW COMM. PATTERN */
	HitPattern allToAll = hit_pattern( HIT_PAT_UNORDERED );

	/* 5. COMPUTE SENDS: SKIP IF MY TILE HAS NULL SHAPE */
	HitShape localShp = tileShapes[ myRank ][0];
	if ( ! hit_shapeCmp( localShp, HIT_SHAPE_NULL ) ) {
		/* FOR ALL PROCESSORS IN THE TOPOLOGY */
		/*		REORDER THE ORDER OF EXPLORATION TO CREATE A SKEWED PATTERN AND
		 *		AVOID POTENTIAL COMM. BOTTLENECKS 	*/
		for ( i=0; i<numProcs; i++ ) {
			int foreignId = ( myRank + i ) % numProcs;

			//HitRanks foreignProc = hit_topRanksInternal( lay.topo, foreignId );
			HitRanks foreignProc = hit_layToTopoRanks( lay, hit_ranks( 1, foreignId ) );
			HitShape foreignShp1 = tileShapes[ foreignId ][0];
			HitShape foreignShp2 = tileShapes[ foreignId ][1];

			/* INTERSECTION: NON-EMPTY IMPLIES COMMUNICATION */
			/* FOR COPY SIG. LAYOUTS: SKIP COMM. IF THE FOREIGN PROC. ALREADY HAS THIS SHAPE */
			HitShape overlapShp2 = hit_shapeIntersect( localShp, foreignShp2 );
			HitShape alreadyThere = hit_shapeIntersect( overlapShp2, foreignShp1 );

			if ( ! hit_shapeCmp( overlapShp2, HIT_SHAPE_NULL )
					&& ( i == 0 || hit_shapeCmp( alreadyThere, HIT_SHAPE_NULL ) ) ) {

				/* TRANSLATE COMPACT COORDINATES BACK TO ARRAY COORDINATES */
				if ( flagCompact ) {
#ifdef DEBUG
fprintf(stderr, "[%d] Init translation send from %d to %d with shape %d [%d:%d:%d][%d:%d:%d]\n", 
		hit_Rank, 
		myRank, foreignId,
		hit_shapeDims( overlapShp2 ),
		hit_shapeSig( overlapShp2, 0 ).begin,
		hit_shapeSig( overlapShp2, 0 ).end,
		hit_shapeSig( overlapShp2, 0 ).stride,
		hit_shapeSig( overlapShp2, 1 ).begin,
		hit_shapeSig( overlapShp2, 1 ).end,
		hit_shapeSig( overlapShp2, 1 ).stride
	   );
#endif
					//overlapShp2 = hit_shapeTileToArray( tileShapesOrig[0], overlapShp2 );
					int j;
					for (j=0; j<hit_shapeDims( overlapShp2 ); j++) {
/*
fprintf(stderr, "[%d] In translation %d, through %d, %d, %d\n", 
		hit_Rank, 
		hit_shapeSig( overlapShp2, j ).begin,
		prefixSums[0][j],
		hit_shapeSig( tileShapesOrig[0], j ).stride,
		hit_shapeSig( tileShapesOrig[0], j ).begin
	   );
*/

						hit_shapeSig( overlapShp2, j ).begin = 
							( hit_shapeSig( overlapShp2, j ).begin - prefixSums[0][j] )
							* 
							hit_shapeSig( tileShapesOrig[0], j ).stride
							+
							hit_shapeSig( tileShapesOrig[0], j ).begin;
/*
fprintf(stderr, "[%d] In translation result %d\n", 
		hit_Rank,
		hit_shapeSig( overlapShp2, j ).begin
	   );
fprintf(stderr, "[%d] In translation end %d, through %d, %d, %d\n", 
		hit_Rank, 
		hit_shapeSig( overlapShp2, j ).end,
		prefixSums[0][j],
		hit_shapeSig( tileShapesOrig[0], j ).stride,
		hit_shapeSig( tileShapesOrig[0], j ).begin
	   );
*/
						hit_shapeSig( overlapShp2, j ).end = 
							( hit_shapeSig( overlapShp2, j ).end - prefixSums[0][j] )
							* 
							hit_shapeSig( tileShapesOrig[0], j ).stride
							+
							hit_shapeSig( tileShapesOrig[0], j ).begin;
						hit_shapeSig( overlapShp2, j ).stride = 
							hit_shapeSig( tileShapesOrig[0], j ).stride;
					}
				}

#ifdef DEBUG
fprintf(stderr, "%s Adding send from %d to %d with shape %d [%d:%d:%d][%d:%d:%d]\n", __FUNCTION__, myRank, foreignId,
		hit_shapeDims( overlapShp2 ),
		hit_shapeSig( overlapShp2, 0 ).begin,
		hit_shapeSig( overlapShp2, 0 ).end,
		hit_shapeSig( overlapShp2, 0 ).stride,
		hit_shapeSig( overlapShp2, 1 ).begin,
		hit_shapeSig( overlapShp2, 1 ).end,
		hit_shapeSig( overlapShp2, 1 ).stride
	   );
#endif
				/* ADD COMM. TO PATTERN */
				hit_patternAdd( &allToAll, 
					hit_comSendSelectTag( 
						lay, foreignProc, tileP1, overlapShp2, HIT_COM_ARRAYCOORDS, baseType, 
						HIT_PAT_REDISTRIBUTE_TAG2
					)
				);
			}
		}
	}

	//MPI_Barrier( MPI_COMM_WORLD );

	/* 6. COMPUTE RECEIVES: SKIP IF MY RECEIVING TILE HAS NULL SHAPE */
	HitShape localShp1 = tileShapes[ myRank ][0];
	HitShape localShp2 = tileShapes[ myRank ][1];
	if ( ! hit_shapeCmp( localShp2, HIT_SHAPE_NULL ) ) {

		/* FOR ALL PROCESSORS IN THE TOPOLOGY */
		/*		REORDER THE ORDER OF EXPLORATION TO CREATE A SKEWED PATTERN AND
		 *		AVOID POTENTIAL COMM. BOTTLENECKS 	*/
		for ( i=0; i<numProcs; i++ ) {
			int foreignId = ( myRank + i ) % numProcs;

			//HitRanks foreignProc = hit_topRanksInternal( lay.topo, foreignId );
			HitRanks foreignProc = hit_layToTopoRanks( lay, hit_ranks( 1, foreignId ) );
			HitShape foreignShp1 = tileShapes[ foreignId ][0];

			/* INTERSECTION: NON-EMPTY IMPLIES COMMUNICATION */
			/* FOR COPY SIG. LAYOUTS: SKIP COMM. IF THE FOREIGN PROC. ALREADY HAS THIS SHAPE */
			HitShape overlapShp = hit_shapeIntersect( localShp2, foreignShp1 );
			HitShape alreadyHere = hit_shapeIntersect( overlapShp, localShp1 );

			if ( ! hit_shapeCmp( overlapShp, HIT_SHAPE_NULL ) 
					&& ( i == 0 || hit_shapeCmp( alreadyHere, HIT_SHAPE_NULL ) ) ) {

				/* TRANSLATE COMPACT COORDINATES BACK TO ARRAY COORDINATES */
				if ( flagCompact ) {
#ifdef DEBUG
fprintf(stderr, "[%d] Init translation recv from %d to %d with shape %d [%d:%d:%d][%d:%d:%d]\n", 
		hit_Rank, 
		foreignId,
		myRank, 
		hit_shapeDims( overlapShp ),
		hit_shapeSig( overlapShp, 0 ).begin,
		hit_shapeSig( overlapShp, 0 ).end,
		hit_shapeSig( overlapShp, 0 ).stride,
		hit_shapeSig( overlapShp, 1 ).begin,
		hit_shapeSig( overlapShp, 1 ).end,
		hit_shapeSig( overlapShp, 1 ).stride
	   );
#endif
					// overlapShp = hit_shapeTileToArray( tileShapesOrig[1], overlapShp );
					int j;
					for (j=0; j<hit_shapeDims( overlapShp ); j++) {
						hit_shapeSig( overlapShp, j ).begin = 
							( hit_shapeSig( overlapShp, j ).begin - prefixSums[1][j] )
							* 
							hit_shapeSig( tileShapesOrig[1], j ).stride
							+
							hit_shapeSig( tileShapesOrig[1], j ).begin;
						hit_shapeSig( overlapShp, j ).end = 
							( hit_shapeSig( overlapShp, j ).end - prefixSums[1][j] )
							* 
							hit_shapeSig( tileShapesOrig[1], j ).stride
							+
							hit_shapeSig( tileShapesOrig[1], j ).begin;
						hit_shapeSig( overlapShp, j ).stride = 
							hit_shapeSig( tileShapesOrig[1], j ).stride;
					}
				}
#ifdef DEBUG
fprintf(stderr, "%s Adding recv from %d to %d with shape %d [%d:%d:%d][%d:%d:%d]\n", __FUNCTION__, foreignId, myRank,
		hit_shapeDims( overlapShp ),
		hit_shapeSig( overlapShp, 0 ).begin,
		hit_shapeSig( overlapShp, 0 ).end,
		hit_shapeSig( overlapShp, 0 ).stride,
		hit_shapeSig( overlapShp, 1 ).begin,
		hit_shapeSig( overlapShp, 1 ).end,
		hit_shapeSig( overlapShp, 1 ).stride
	   );
#endif

				/* ADD COMM. TO PATTERN */
				hit_patternAdd( &allToAll, 
					hit_comRecvSelectTag( 
						lay, foreignProc, tileP2, overlapShp, HIT_COM_ARRAYCOORDS, baseType,
						HIT_PAT_REDISTRIBUTE_TAG2
					)
				);
			}
		}
	}

	/* 7. RETURN */
	return allToAll;
}


// @note The recv works because the rows are contiguous distributed.
void hit_patMatMultInternal(HitPattern *pattern, HitLayout lay, HitShape origin_matrix, HitShape matrix, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line){

	HIT_NOT_USED(file);
	HIT_NOT_USED(line);
	HIT_NOT_USED(origin_matrix);

	/* Get the communicator of the topology */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;


	/* Number of processors */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}


	// Calculate the recv
	int * recvcnts;
	int * rdispls;

	hit_calloc(recvcnts, int, numProcs );
	hit_calloc(rdispls, int, numProcs );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_calloc(recvcnts,  sizeof(int), (size_t) numProcs,int*);
	hit_calloc(rdispls,sizeof(int), (size_t) numProcs,int*);
	*/

	int nrecv = hit_cShapeCard(matrix,1);

	//printf("[%d] Recibo %d, other %d\n",lay.topo.linearRank,nrecv,hit_cShapeNameList(matrix,1).nNames);
	//printf("numElementsTotal1 %d\n",lay.info.layoutList.numElementsTotal);

	for(i=0; i<nrecv; i++){
		int name = hit_cShapeNameList(matrix,1).names[i];
		int owner = hit_lgr_elementGroup(lay,name);
		recvcnts[owner] ++;
	}

	rdispls[0] = 0;
	for(i=0; i<numProcs-1; i++){
		rdispls[i+1] = rdispls[i] + recvcnts[i];
	}


	int * sendcnts;
	int * sdispls;
	hit_calloc(sendcnts, int, numProcs );
	hit_calloc(sdispls, int, numProcs );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_calloc(sendcnts,  sizeof(int), (size_t) numProcs,int*);
	hit_calloc(sdispls,sizeof(int), (size_t) numProcs,int*);
	*/

	MPI_Alltoall(recvcnts, 1, MPI_INT,sendcnts, 1, MPI_INT, MPI_COMM_WORLD);

	sdispls[0] = 0;
	for(i=0; i<numProcs-1; i++){
		sdispls[i+1] = sdispls[i] + sendcnts[i];

	}

	int nsend = sdispls[numProcs-1] + sendcnts[numProcs-1];


	int * sendlist;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(sendlist,(size_t) nsend * sizeof(int),int*);
	hit_malloc(sendlist, int, nsend );

	MPI_Alltoallv(hit_cShapeNameList(matrix,1).names,  recvcnts, rdispls, MPI_INT,
				  sendlist, sendcnts, sdispls, MPI_INT, MPI_COMM_WORLD);


	const HitTile *tileSend = (const HitTile *)tilePSend;
	const HitTile *tileRecv = (const HitTile *)tilePRecv;

	for(i=0; i<nsend; i++){
		sendlist[i] -= hit_shapeSig(tileSend->shape,0).begin;
	}


	// @arturo 2015/01/22
	//int myself = lay.topo.linearRank;
	int myself = hit_topSelfRankInternal( lay.topo );


	// Create the comms
	for(i=0; i<numProcs; i++){

		//if(i == myself) continue;

		HitCom csend = HIT_COM_NULL;
		HitCom crecv = HIT_COM_NULL;

		MPI_Datatype tsend;
		MPI_Datatype trecv;

		int ok;

		// Send type
/*
		{

			int j;
			printf("[%d] displ=%d   List: ",myself,sdispls[i]);
			int * list = &(sendlist[sdispls[i]]);
			for(j=0; j<sendcnts[i]; j++){
				printf("%d, ",list[j]);
			}
			printf("\n");

		}
*/

		ok = MPI_Type_create_indexed_block(sendcnts[i],1,&(sendlist[sdispls[i]]),baseType,&tsend);
		hit_mpiTestError(ok,"Error type indexed block");
		ok = MPI_Type_commit(&tsend);
		hit_mpiTestError(ok,"Error type commit");


		// Send
		csend.myself = myself;
		csend.sendTo = i;
		//csend.recvFrom = HIT_RANK_NULL;
		csend.typeSend = tsend;
		csend.dataSend = tileSend->data;
		csend.comm = comm;
		csend.tag = i;
		csend.commType = HIT_SENDRECV;



		// Recv type
		ok = MPI_Type_contiguous(recvcnts[i],baseType,&trecv);
		hit_mpiTestError(ok,"Error type contiguous");
		ok = MPI_Type_commit(&trecv);
		hit_mpiTestError(ok,"Error type commit");

		// Recv
		crecv.myself = myself;
		//crecv.sendTo = HIT_RANK_NULL;
		crecv.recvFrom = i;
		crecv.typeRecv = trecv;
		crecv.dataRecv = (char*) tileRecv->data + (size_t) rdispls[i] * tileRecv->baseExtent;
		crecv.comm = comm;
		crecv.tag = myself;
		crecv.commType = HIT_SENDRECV;

		//{
		//	int j;
		//	printf("[%d] displ recv =%d   List: ",myself,rdispls[i]);
		//	printf("\n");
		//}



		if(sendcnts[i] != 0 )
			hit_patternAdd(pattern,csend);
		if(recvcnts[i] != 0 )
			hit_patternAdd(pattern,crecv);


	}

}








// @note The recv works because the rows are contiguous distributed.
void hit_patMatMultBitmapInternal(HitPattern *pattern, HitLayout lay, HitShape origin_matrix, HitShape matrix, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line){

	HIT_NOT_USED(file);
	HIT_NOT_USED(line);
	HIT_NOT_USED(origin_matrix);


	/* Get the communicator of the topology */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;


	/* Number of processors */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}

	//printf("Procs: %d\n",numProcs);

	// Calculate the recv
	int * recvcnts;
	int * rdispls;

	hit_calloc(recvcnts, int, numProcs );
	hit_calloc(rdispls, int, numProcs );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_calloc(recvcnts,  sizeof(int), (size_t) numProcs,int*);
	hit_calloc(rdispls,sizeof(int), (size_t) numProcs,int*);
	*/

	int nrecv = hit_bShapeCard(matrix,1);

	//printf("[%d] Recibo %d, other %d\n",lay.topo.linearRank,nrecv,hit_bShapeNameList(matrix,1).nNames);
	//printf("numElementsTotal1 %d\n",lay.info.layoutList.numElementsTotal);

	for(i=0; i<nrecv; i++){
		int name = hit_bShapeNameList(matrix,1).names[i];
		int owner = lay.info.layoutList.assignedGroups[name];
		recvcnts[owner] ++;
		//printf("**[%d] Recivo %d from %d\n",lay.topo.linearRank,name,owner);
	}



	rdispls[0] = 0;
	for(i=0; i<numProcs-1; i++){
		rdispls[i+1] = rdispls[i] + recvcnts[i];
	}

	/*
	sleep(lay.topo.linearRank);
	fflush(stdout);


	{
		int i,j;
		for(i=0; i<numProcs; i++){
			printf("Proc %d\n",i);
			for(j=rdispls[i];j<rdispls[i]+recvcnts[i];j++){
				printf("**[%d] Recivo %d from %d\n",lay.topo.linearRank,hit_bShapeNameList(matrix,1).names[j],i);

			}

		}
	}
	fflush(stdout);
*/


	int * sendcnts;
	int * sdispls;
	hit_calloc(sendcnts, int, numProcs );
	hit_calloc(sdispls, int, numProcs );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_calloc(sendcnts,  sizeof(int), (size_t) numProcs,int*);
	hit_calloc(sdispls,sizeof(int), (size_t) numProcs,int*);
	*/


	MPI_Alltoall(recvcnts, 1, MPI_INT,sendcnts, 1, MPI_INT, MPI_COMM_WORLD);


	sdispls[0] = 0;
	for(i=0; i<numProcs-1; i++){
		sdispls[i+1] = sdispls[i] + sendcnts[i];

	}

	int nsend = sdispls[numProcs-1] + sendcnts[numProcs-1];


	int * sendlist;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(sendlist,(size_t) nsend * sizeof(int),int*);
	hit_malloc(sendlist, int, nsend );

	MPI_Alltoallv(hit_bShapeNameList(matrix,1).names,  recvcnts, rdispls, MPI_INT,
				  sendlist, sendcnts, sdispls, MPI_INT, MPI_COMM_WORLD);


	const HitTile *tileSend = (const HitTile *)tilePSend;
	const HitTile *tileRecv = (const HitTile *)tilePRecv;


	/*
	{
		int i,j;
		for(i=0; i<numProcs; i++){

			for(j=sdispls[i];j<sdispls[i]+sendcnts[i];j++){
				printf("**[%d] Envio %d to %d\n",lay.topo.linearRank,sendlist[j],i);

			}

		}
	}
	*/


	for(i=0; i<nsend; i++){
		sendlist[i] -= hit_shapeSig(tileSend->shape,0).begin;
	}

	// @arturo 2015/01/22
	//int myself = lay.topo.linearRank;
	int myself = hit_topSelfRankInternal( lay.topo );


	// Create the comms
	for(i=0; i<numProcs; i++){

		//if(i == myself) continue;

		HitCom csend = HIT_COM_NULL;
		HitCom crecv = HIT_COM_NULL;

		MPI_Datatype tsend;
		MPI_Datatype trecv;

		int ok;

		// Send type

//		fflush(stdout);

	//	{
//
	//		int j;
		//	printf("[%d] displ=%d   List: ",myself,sdispls[i]);
		//	int * list = &(sendlist[sdispls[i]]);
		//	for(j=0; j<sendcnts[i]; j++){
		//		printf("%d, ",list[j]);
		//	}
		//	printf("\n");

	//	}

	//	fflush(stdout);


		ok = MPI_Type_create_indexed_block(sendcnts[i],1,&(sendlist[sdispls[i]]),baseType,&tsend);
		hit_mpiTestError(ok,"Error type indexed block");
		ok = MPI_Type_commit(&tsend);
		hit_mpiTestError(ok,"Error type commit");


		// Send
		csend.myself = myself;
		csend.sendTo = i;
		//csend.recvFrom = HIT_RANK_NULL;
		csend.typeSend = tsend;
		csend.dataSend = tileSend->data;
		csend.comm = comm;
		csend.tag = i;
		csend.commType = HIT_SENDRECV;



		// Recv type
		ok = MPI_Type_contiguous(recvcnts[i],baseType,&trecv);
		hit_mpiTestError(ok,"Error type contiguous");
		ok = MPI_Type_commit(&trecv);
		hit_mpiTestError(ok,"Error type commit");

		// Recv
		crecv.myself = myself;
		//crecv.sendTo = HIT_RANK_NULL;
		crecv.recvFrom = i;
		crecv.typeRecv = trecv;
		crecv.dataRecv = (char*) tileRecv->data + (size_t) rdispls[i] * tileRecv->baseExtent;
		crecv.comm = comm;
		crecv.tag = myself;
		crecv.commType = HIT_SENDRECV;

		//{
		//	int j;
		//	printf("[%d] displ recv =%d   List: ",myself,rdispls[i]);
		//	printf("\n");
		//}



		if(sendcnts[i] != 0 )
			hit_patternAdd(pattern,csend);
		if(recvcnts[i] != 0 )
			hit_patternAdd(pattern,crecv);


	}



}


