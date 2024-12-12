/**
 * MPI Communications management module.
 * It allow to create point to point or collective communications
 * on active processors on a Hitmap Topology or Layout.
 * The communications are encapsulated in a HitCom object.
 *
 * @file hit_com.c
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Sep 2011
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
#include <stdlib.h>
#include <hit_com.h>
#include <unistd.h>
#include <hit_funcop.h>
#include <hit_sshape.h>
#include <hit_cshape.h>
#include <hit_bshape.h>
#include <hit_error.h>
#include <pthread.h>

/* Hit NULL CONSTANTS INITIALIZATION */
HitCom HIT_COM_NULL = HIT_COM_NULL_STATIC;
HitComAlltoallv HIT_COM_ALLTOALLV_NULL = HIT_COM_ALLTOALLV_NULL_STATIC;
HitComSparse HIT_COM_SPARSE_NULL = HIT_COM_SPARSE_NULL_STATIC;

/* Hit DEFAULT REDUCTION OPERATORS */
HitOp	HIT_OP_SUM_INT;
HitOp	HIT_OP_MIN_INT;
HitOp	HIT_OP_MAX_INT;
HitOp	HIT_OP_SUM_FLOAT;
HitOp	HIT_OP_MIN_FLOAT;
HitOp	HIT_OP_MAX_FLOAT;
HitOp	HIT_OP_SUM_DOUBLE;
HitOp	HIT_OP_MIN_DOUBLE;
HitOp	HIT_OP_MAX_DOUBLE;

/* Hit EXTRA COMMITED TYPES */
HitType	HIT_SHAPE_SIG;


MPI_Errhandler mpi_errhandler;


/**
 * Error handler for MPI 
 */
static void mpi_error_handler(MPI_Comm *comm, int *err, ... ){

	HIT_NOT_USED(comm);

	// 1. Array with space for the MPI Message
	char array[1024];
	array[0] = '\0';
	int lenght;

	// 2. Check if MPI is initialized to use MPI_Error_string.
	int initialized;
	MPI_Initialized(&initialized);

	if(initialized){
		MPI_Error_string(*err, array, &lenght);
		array[lenght] = '\0';
	}

#ifdef __GNUC__
	// If using gcc we can call the get_gdb_trace function to get
	// the gdb trace of the current process.
	printf( "\x1b[31m %s \n \x1b[35m %s \x1b[0m",array,get_gdb_trace());
#else
	printf( "\x1b[31m %s \n",array);
#endif

}


// FLAG TO CHECK IF THE COM MODULE HAS BEEN INITIALIZED OF FINALIZED
static int hit_active = 0;

/* Hit MPI INITIALIZATION */
void hit_comInit(int *pargc, char **pargv[]) {
	// 0. AVOID DOUBLE INITIALIZATION
	if ( hit_active ) {
		hit_warning_here("hit_comInit: Already initialized");
		return;
	}
	hit_active = 1;

	/* 1. INITIALIZE MPI */
	int mpi_provided_thread;
	int ok = MPI_Init_thread( pargc, pargv, MPI_THREAD_MULTIPLE, &mpi_provided_thread );
	hit_mpiTestError( ok, "Initializing HitCom for MPI comm. library" );

	// Set our error handler
	MPI_Comm_create_errhandler(mpi_error_handler, &mpi_errhandler);
	MPI_Comm_set_errhandler(MPI_COMM_WORLD, mpi_errhandler);

	/* 2. GET BASIC GROUP PARAMETERS */
	hit_malloc( HIT_TOPOLOGY_INFO, HitPTopology, 1 );
	// @arturo Ago 2015: New allocP interface
	// sizeof( HitPTopology), HitPTopology*);
	MPI_Comm_rank(MPI_COMM_WORLD, &(HIT_TOPOLOGY_INFO->selfRank) );
	MPI_Comm_size(MPI_COMM_WORLD, &(HIT_TOPOLOGY_INFO->numProcs) );
	MPI_Comm_dup ( MPI_COMM_WORLD,  &(HIT_TOPOLOGY_INFO->comm) );
	HIT_TOPOLOGY_INFO->numUses = 1;
	// @arturo 2015/01/03, 
	// 	New functions to redistribute need proper initialization of fields "next" and new "global"
	HIT_TOPOLOGY_INFO->next = NULL;
	HIT_TOPOLOGY_INFO->global = HIT_TOPOLOGY_INFO;
	// @arturo 2015/01/07 
	HIT_TOPOLOGY_INFO->antiCommGroup = MPI_GROUP_EMPTY;

	/* 3. SET SOME NULL VALUES TO MPI EQUIVALENTS */
	HIT_RANK_NULL = MPI_PROC_NULL;
	int i;
	for (i=0; i<HIT_MAXDIMS; i++) HIT_RANKS_NULL.rank[i] = MPI_PROC_NULL;

	/* 4. DECLARE THE BASIC REDUCTION OPERATIONS ON int AND double */
	hit_comOp( hit_comOpSumInt, HIT_OP_SUM_INT );
	hit_comOp( hit_comOpMinInt, HIT_OP_MIN_INT );
	hit_comOp( hit_comOpMaxInt, HIT_OP_MAX_INT );
	hit_comOp( hit_comOpSumFloat, HIT_OP_SUM_FLOAT );
	hit_comOp( hit_comOpMinFloat, HIT_OP_MIN_FLOAT );
	hit_comOp( hit_comOpMaxFloat, HIT_OP_MAX_FLOAT );
	hit_comOp( hit_comOpSumDouble, HIT_OP_SUM_DOUBLE );
	hit_comOp( hit_comOpMinDouble, HIT_OP_MIN_DOUBLE );
	hit_comOp( hit_comOpMaxDouble, HIT_OP_MAX_DOUBLE );

	/* 5. COMMIT EXTRA DATATYPES */
	ok = MPI_Type_contiguous( HIT_MAXDIMS * HIT_SIG_NUM_FIELDS + 1, MPI_INT, &HIT_SHAPE_SIG );
	hit_mpiTestError(ok, "Creating shape datatype");
	ok = MPI_Type_commit( &HIT_SHAPE_SIG );
	hit_mpiTestError(ok, "Commiting shape datatype");
}


/* Hit MPI FINALIZATION */
void hit_comFinalize() {
	// 0. AVOID CALLING BEFORE INITIALIZATION
	if ( ! hit_active ) 
		hit_error_noinit("hit_comFinalize: Not initialized or already finalized");
	hit_active = 0;

	/* 1. FREE BASIC TOPOLOGY INFORMATION */
	//MPI_Comm_free( (MPI_Comm *) HIT_TOPOLOGY_INFO.lowLevel);
	//free(HIT_TOPOLOGY_INFO.lowLevel);
	//free(HIT_TOPOLOGY_INFO.lowLevel);
	if ( HIT_TOPOLOGY_INFO->numUses > 1 )
		hit_warnInternal( __FUNCTION__, "Not all topologies or layouts have been deallocted", "", __FILE__, __LINE__);
	hit_ptopFree( &HIT_TOPOLOGY_INFO );
	hit_free( HIT_TOPOLOGY_INFO );

	/* 2. FREE BASIC REDUCTION OPERATIONS ON int AND double */
	hit_comOpFree( HIT_OP_SUM_INT );
	hit_comOpFree( HIT_OP_MIN_INT );
	hit_comOpFree( HIT_OP_MAX_INT );
	hit_comOpFree( HIT_OP_SUM_FLOAT );
	hit_comOpFree( HIT_OP_MIN_FLOAT );
	hit_comOpFree( HIT_OP_MAX_FLOAT );
	hit_comOpFree( HIT_OP_SUM_DOUBLE );
	hit_comOpFree( HIT_OP_MIN_DOUBLE );
	hit_comOpFree( HIT_OP_MAX_DOUBLE );

	/* 3. FREE COMMITED EXTRA DATATYPES */
	int ok = MPI_Type_free( &HIT_SHAPE_SIG );
	hit_mpiTestError(ok, "Freeing shape datatype");

	/* 3. FINALIZE MPI */
	MPI_Finalize();
	//pthread_exit(NULL);
}

/* MPI NODE NAME/PROC INFO */
// @arturo 2023/07/04

// PRIVATE VARIABLES FOR NODE GROUP INFO
char hit_com_node_name[MPI_MAX_PROCESSOR_NAME];
int  hit_com_nodegroup_rank = MPI_PROC_NULL;
int  hit_com_nodegroup_size = -1;

/* GET NODE NAME */
char * hit_comNodeName() {
	/* AVOID CALLING BEFORE INITIALIZATION */
	if ( ! hit_active ) 
		hit_error_noinit("hit_comNodeName: Hit not initialized or already finalized");

	int length = 0;	
	int ok = MPI_Get_processor_name( hit_com_node_name, &length );
	hit_mpiTestError( ok, "MPI Getting processor name" );
	return hit_com_node_name;
}

/* GET THE RANK id IN THE NODE GROUP */
int hit_comNodeGroupRank() {
	/* AVOID CALLING BEFORE INITIALIZATION */
	if ( ! hit_active ) 
		hit_error_noinit("hit_comNodeGroupRank: Hit not initialized or already finalized");

	/* DO SPLIT ONLY ONCE, RETURN RANK IF ALREADY COMPUTED */
	if ( hit_com_nodegroup_rank != MPI_PROC_NULL ) return hit_com_nodegroup_rank;

	/* SPLIT PHYSICAL TOPOLOGY IN NODE GROUPS */
	MPI_Info info;
	MPI_Info_create( &info );

	MPI_Comm new_comm;
	int result = MPI_Comm_split_type( MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, info, &new_comm );
	hit_mpiTestError( result, "Error using MPI_Comm_split" );
	MPI_Comm_rank( new_comm, &hit_com_nodegroup_rank );
	MPI_Comm_size( new_comm, &hit_com_nodegroup_size );
	result = MPI_Comm_free( &new_comm );
	hit_mpiTestError( result, "Error freeing NodeGroup communicator" );
	result = MPI_Info_free( &info );
	hit_mpiTestError( result, "Error freeing Info for NodeGroup communicator" );

	return hit_com_nodegroup_rank;
}

/* GET THE SIZE OF THE NODE GROUP */
int hit_comNodeGroupSize() {
	/* AVOID CALLING BEFORE INITIALIZATION */
	if ( ! hit_active ) 
		hit_error_noinit("hit_comNodeGroupSize: Hit not initialized or already finalized");

	/* SPLIT ONLY ONCE, RETURN RANK IF ALREADY COMPUTED */
	if ( hit_com_nodegroup_size != -1 ) return hit_com_nodegroup_size;

	/* SPLIT PHYSICAL TOPOLOGY IN NODE GROUPS */
	MPI_Info info;
	MPI_Info_create( &info );

	MPI_Comm new_comm;
	int result = MPI_Comm_split_type( MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, info, &new_comm );
	hit_mpiTestError( result, "Error using MPI_Comm_split" );
	MPI_Comm_rank( new_comm, &hit_com_nodegroup_rank );
	MPI_Comm_size( new_comm, &hit_com_nodegroup_size );
	result = MPI_Comm_free( &new_comm );
	hit_mpiTestError( result, "Error freeing NodeGroup communicator" );
	result = MPI_Info_free( &info );
	hit_mpiTestError( result, "Error freeing Info for NodeGroup communicator" );

	return hit_com_nodegroup_size;
}


/* Hit COMPUTE MPI DERIVED DATA TYPE FOR A SINGLE TILE */
HitType hit_comType(const void *varP, HitType baseType) {
	HitType newType, prevType;
	HitType aux[HIT_MAXDIMS];
	int dim,numTypes, contiguous;
	MPI_Aint baseExtent, baseLb;
	HitTile var = *(const HitTile *)varP;

#ifdef BARELY_FASTER
	/* 1. ORIGINAL VARIABLES OR SELECTIONS OF THE WHOLE SHAPE */
	if (var.acumCard == var.origAcumCard[0]) {

		/* 1.2. CONTIGUOS TYPE */
		if (MPI_SUCCESS != MPI_Type_contiguous(var.acumCard, baseType, &newType))
			hit_error("MPI_Type_contiguous", __FILE__, __LINE__);
		if (MPI_SUCCESS != MPI_Type_commit(&newType))
			hit_error("MPI_Type_commit", __FILE__, __LINE__);

		/* 1.3. RETURN */
		return newType;
	}
#endif

	/* 2. GET MEMORY FOR THE TYPES ARRAY */
	numTypes=hit_tileDims(var);
		
	/* 3. GET EXTENT OF THE BASE TYPE FOR THE STRIDES */
	MPI_Type_get_extent(baseType, &baseLb, &baseExtent);

	/* 4. START LOOP OF DIMENSIONS BACKWARDS (ROW ORIENTED) */
	prevType = baseType;
	contiguous = 1;
	int partialAcumCard = 1;

	for (dim=numTypes-1; dim >=0; dim-- ) {
		/* 4.1. WHILE CONTIGUOUS AND THIS DIM HAS NO STRIDE, KEEP ON CONTIGUOS DATA TYPES */
		// @arturo May 2021: BUG CORRECTED, USE MEMORY STRIDE, NOT GLOBAL COORDINATES STRIDE
		if (contiguous && var.qstride[dim] == 1 ) {
#ifdef DEBUG
printf("Contiguous type for dim %d, with %d total elements\n", dim, partialAcumCard); fflush(stdout);
#endif
			/* 4.1.1. COMPUTE TYPE */
			if (MPI_SUCCESS != MPI_Type_contiguous( hit_tileDimCard(var,dim), prevType,
														&(aux[dim])))
				hit_error("MPI_Type_contiguous", __FILE__, __LINE__);

			/* 4.1.2. A NON-FULL DIMENSION FORCES TO USE HVECTOR TYPES AFTERWARDS */
			partialAcumCard = partialAcumCard * hit_tileDimCard(var, dim);
			if ( partialAcumCard != var.origAcumCard[dim]) contiguous = 0;
		}
		/* 4.2. AFTER A NON-CONTIGUOS DIMENSION, USE GENERIC HVECTOR TYPES */
		else {
			contiguous = 0;
#ifdef DEBUG
printf("Hvector type for dim %d, Card: %d, stride: %ld\n", dim, var.card[dim], baseExtent*var.origAcumCard[dim+1]* hit_tileDimStride(var,dim)
//.shape.sig[dim].stride
); fflush(stdout);
#endif
			if (MPI_SUCCESS != MPI_Type_create_hvector(var.card[dim], 1, 
					// @arturo May 2021: BUG CORRECTED, USE MEMORY STRIDE, NOT GLOBAL COORDINATES STRIDE
					baseExtent*var.origAcumCard[dim+1] * var.qstride[dim],
					prevType, &(aux[dim])) )
				hit_error("MPI_Type_create_hvector", __FILE__, __LINE__);
		}
		prevType = aux[dim];
	}
	
#ifdef DEBUG
	printf("freeing type resources\n"); fflush(stdout);
#endif
	newType=prevType;

	for (dim=numTypes-1; dim >0; dim-- ) {
		hit_comFreeType(aux[dim]);
	}
	
#ifdef DEBUG
	printf("commited hit type %d\n",newType);
#endif
	
	/* 6. RETURN THE TYPES STRUCTURE */	
	return newType;
}

/**
 * hit_comSearchData: Return the data pointer of a tile. In case it is a multilevel tile
 * 	it returns the data pointer of the first leaf tile. This pointer is used
 *	along with the MPI derived type (recursively built for multilevel tiles)
 *	to find all the data of any kind of tile.
 * @param varP Tile pointer.
 * @return Data pointer.
 */
void * hit_comSearchData (const void * varP) {
	const HitTile * var = (const HitTile *)varP;
	while ( var->hierDepth > 0 ) {
		var=(HitTile *)(var->data);
	}
	return var->data;
}

/*
 * CREATE A DERIVED DATA TYPE FOR A TILE
 * RECURSIVELY FOR MULTI-LEVEL TILES
 */
HitType hit_comTypeRec(const void *varP, HitType baseType) {
	HitTile var = *(const HitTile *)varP;
	HitType *types;
	
	MPI_Aint *addresses;
	int i, j, offset, *sizes, ind[HIT_MAXDIMS], max[HIT_MAXDIMS];
        HitTile * auxtile;
	HitType newType;
	HitType * auxtypes;

	if ( var.memStatus == HIT_MS_NULL || hit_tileDims(var)==0 ){
#ifdef DEBUG
		printf("hit_comTypeRec: NULL type, Tile is NULL or dims == 0\n");
#endif
		return HIT_TYPE_NULL;
	}

       	if( var.hierDepth > 0 ) {
			hit_malloc(sizes, int, var.acumCard );
			hit_malloc(addresses, MPI_Aint, var.acumCard );
			hit_malloc(types, HitType, var.acumCard );
			hit_malloc(auxtypes, HitType, var.acumCard );
			// @arturo Ago 2015: New allocP interface
			/* 
			hit_malloc(sizes, int, (size_t)var.acumCard*sizeof(int),int*);
			hit_malloc(addresses,(size_t)var.acumCard*sizeof(MPI_Aint),MPI_Aint*);
			hit_malloc(types,(size_t)var.acumCard*sizeof(HitType),HitType*);
			hit_malloc(auxtypes,(size_t)var.acumCard*sizeof(HitType),HitType*);
			*/
			//Initialize max with cardinalities
			memcpy(max,var.card,(size_t) hit_tileDims(var)  *sizeof(int));
			//initialize ind with 0
			memset(ind,0,(size_t)hit_tileDims(var)*sizeof(int));
#ifdef DEBUG
printf("Multilevel Struct type for acumCard: %d\n", var.acumCard ); fflush(stdout);
#endif

			for(i=0;i<var.acumCard;i++) {
				offset=0;
				for(j=0;j<hit_tileDims(var);j++) {
					offset+=ind[j]*var.qstride[j]*var.origAcumCard[j+1];
				}
				auxtile=(HitTile *)( (char *)var.data+offset*(int)var.baseExtent );
				auxtypes[i]=hit_comTypeRec(auxtile, baseType);
				types[i]=auxtypes[i];
				void * data = hit_comSearchData(auxtile);
				MPI_Get_address(data,&addresses[i]);
				// @arturo: Moved outside the loop to avoid warning: sizes may be used not initialized
				//sizes[i]=1;
				j=0;
				do {
					ind[j]=(ind[j]+1)%max[j];
					j++;
				} while(j<hit_tileDims(var) && ind[j-1]==0 && max[j]!=0);
			}
			for(i=var.acumCard-1;i>=0;i--) addresses[i]-=addresses[0];
			// @arturo: Moved outside the loop to avoid warning: sizes may be used not initialized
			for(i=0;i<var.acumCard;i++) sizes[i]=1;

			MPI_Type_create_struct(var.acumCard,sizes,addresses,types,&newType);
			MPI_Type_commit(&newType);
			for(i=0;i<var.acumCard;i++) hit_comFreeType(auxtypes[i]);
			free(auxtypes);
			free(types);
			free(sizes);
			free(addresses);
			return newType;
      	} else {
			newType = hit_comType(varP,baseType);
				/* COMMIT TYPE */
			if (MPI_SUCCESS != MPI_Type_commit(&newType))
				hit_error("Commit last type", __FILE__, __LINE__);

#ifdef DEBUG
			printf("commited hit type %d\n",newType);
#endif
			return newType;
		}
}


/* Create the low level communicator to allow collective dim communications */
void hit_comAllowDim(HitLayout * lay, int dim){

	// @arturo Mar 2013
	HitPTopology *ptopo = NULL;

	if(lay->active){
		// Get the active ranks for the processor
		HitRanks activeRanks = hit_laySelfRanks( *lay );

		// Create the color to split the low level communicator
		int colour=0, i, acum = 1;
		for (i=0;i<hit_layNumDims(*lay);i++) {
			if(i!=dim) colour+= acum * activeRanks.rank[i];
			acum *= lay->numActives[i];
		}

		// Split the low level communicator
		ptopo = hit_ptopSplit( lay->pTopology[0], colour ); 

	}

	lay->pTopology[ dim+1 ] = ptopo; 
}



HitCom hit_comSendRecvReplaceSelectTag(	HitLayout lay,
								HitRanks sendTo,
								const void *tileP,
								HitShape selection,
								int selectMode,
								HitRanks receiveFrom,
								HitType baseType,
								int tag){

	/* 1. AVOID COMMUNICATION ON INACTIVE PROCS IN THE LAYOUT */
	if ( ! lay.active ) return HIT_COM_NULL;

	/* 2. NO COMMUNICATIONS TO/FROM NULL */
	// @arturo: CORRECTED BUG: Let shifts across boundaries be transformed in communications
	// 	from/to NULL ranks, that means from/to MPI_NULL_PROC identifier
	// 	Thus, a boundary processor may send but not receive, or viceversa.
	//if ( hit_ranksCmp(sendTo,HIT_RANKS_NULL) || hit_ranksCmp(receiveFrom,HIT_RANKS_NULL) ) 
	//	return HIT_COM_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.topo.pTopology.lowLevel);
	MPI_Comm comm = lay.topo.pTopology->comm;

	/* 4. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	/* 5. IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	HitTile Select;
	const HitTile *tile = (const HitTile *)tileP;
	if ( tile->memStatus == HIT_MS_NULL || tile->memStatus == HIT_MS_NOMEM || hit_tileDims(*tile)==0 ) return HIT_COM_NULL;

	/* 6. SELECTION */
	if (selectMode == HIT_COM_TILECOORDS) hit_tileSelect( &Select, tile, selection );
	else if (selectMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &Select, tile, selection);
	else return HIT_COM_NULL;

	/* 7. TYPES */
	newCom.typeSend = hit_comTypeRec( &Select, baseType);
	newCom.dataSend = hit_comSearchData(&Select);

	/* 8. FILLING UP OTHER FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );
	newCom.comm = comm;
	newCom.tag = tag;
	newCom.commType = HIT_SENDRECV_REPLACE;

	newCom.sendTo = hit_topRankInternal(lay.topo,sendTo);
	newCom.recvFrom = hit_topRankInternal(lay.topo,receiveFrom);

	/* 9. RETURN */
	return newCom;
}



HitCom hit_comSendRecvSelectTag(HitLayout lay,
								HitRanks sendTo,
								const void *tilePSend,
								HitShape selectionSend,
								int selectSendMode,								
								HitRanks receiveFrom,
								const void *tilePRecv,
								HitShape selectionRecv,
								int selectRecvMode,								
								HitType baseType,
								int tag){

	/* @arturo Feb 2013: BUG CORRECTED
	 * 					Processors that are inactive in the layout may send/receive data
	 * 					In hit_patternRedistribute there are two layouts involved, sometimes
	 * 					the local process is not active in one layout, but it is in the other.
	 */
	/* 1. AVOID COMMUNICATION ON INACTIVE PROCS IN THE TOPOLOGY */
	//if ( ! lay.active ) return HIT_COM_NULL;
	if ( ! lay.topo.active ) return HIT_COM_NULL;

	/* 1. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.topo.pTopology.lowLevel);
	MPI_Comm comm = lay.topo.pTopology->comm;

	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	/* 3. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 4.1. IGNORE COMMUNICATIONS TO NULL */
	/* 		TO THE SAME PROCESSOR ARE NEEDED IN CASE THE SEND/RECV BUFFERS ARE DIFFERENT */
	if ( hit_ranksCmp(sendTo,HIT_RANKS_NULL) ) {
#ifdef DEBUG
		printf("%d no sending\n", hit_Rank); fflush(stdout);
#endif
		newCom.sendTo = MPI_PROC_NULL;
		newCom.typeSend = HIT_TYPE_NULL;
		newCom.dataSend = NULL;
	}
	/* 4.2. COMMUNICATIONS TO MYSELF OR ANOTHER PROCCESSOR */
	else {
		newCom.sendTo = hit_topRankInternal(lay.topo,sendTo);
#ifdef DEBUG
		printf("[%d] Create Send Selection -- SendTo: %d\n", hit_Rank, newCom.sendTo); fflush(stdout);
#endif
		const HitTile *tileSend = (const HitTile *)tilePSend;
		HitTile selectSend;
		/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
		if ( tileSend->memStatus == HIT_MS_NULL || tileSend->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileSend)==0 ) return HIT_COM_NULL;

		/* @arturo: EMPTY SELECTION LEADS TO NULL TYPE, NO COMMUNICATION */
		if((selectSendMode == HIT_COM_TILECOORDS && !hit_tileCheckBoundaryTileCoords(tileSend, selectionSend)) ||
			(selectSendMode == HIT_COM_ARRAYCOORDS && !hit_tileCheckBoundaryArrayCoords(tileSend, selectionSend))
		){
#ifdef DEBUG
			printf("send select out of boundary\n"); fflush(stdout);
#endif
			return HIT_COM_NULL;
		}


		if (selectSendMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectSend, tileSend, selectionSend );
		else if (selectSendMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectSend, tileSend, selectionSend);
		else return HIT_COM_NULL;

		//printf("Tile %p(%d), Select Tile %p(%d)\n",tileSend,tileSend->memStatus,&selectSend,selectSend.memStatus);

		newCom.typeSend = hit_comTypeRec( &selectSend, baseType);
		newCom.dataSend = hit_comSearchData(&selectSend);
	}

	/* 4.4. IGNORE COMMUNICATIONS FROM NULL */
	/* 		TO THE SAME PROCESSOR ARE NEEDED IN CASE THE SEND/RECV BUFFERS ARE DIFFERENT */
	if ( hit_ranksCmp(receiveFrom,HIT_RANKS_NULL) ) {
#ifdef DEBUG
		printf("%d no reception\n", hit_Rank); fflush(stdout);
#endif
		newCom.recvFrom = MPI_PROC_NULL;
		newCom.typeRecv = HIT_TYPE_NULL;
		newCom.dataRecv = NULL;
	}
	else {
		newCom.recvFrom = hit_topRankInternal(lay.topo,receiveFrom);
#ifdef DEBUG
		printf("[%d] Create Recv Selection -- RecvFrom: %d\n", hit_Rank, newCom.recvFrom); fflush(stdout);
#endif
		const HitTile *tileRecv = (const HitTile *)tilePRecv;
		HitTile selectRecv;
		/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
		if ( tileRecv->memStatus == HIT_MS_NULL || tileRecv->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileRecv)==0 ) return HIT_COM_NULL;

		/* @arturo: EMPTY SELECTION LEADS TO NULL TYPE, NO COMMUNICATION */

		if((selectSendMode == HIT_COM_TILECOORDS && !hit_tileCheckBoundaryTileCoords(tileRecv, selectionSend)) ||
			(selectSendMode == HIT_COM_ARRAYCOORDS && !hit_tileCheckBoundaryArrayCoords(tileRecv, selectionSend))
		){
#ifdef DEBUG
			printf("recv select out of boundary\n"); fflush(stdout);
#endif
			return HIT_COM_NULL;
		}

		if (selectRecvMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectRecv, tileRecv, selectionRecv );
		else if (selectRecvMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectRecv, tileRecv, selectionRecv);
		else return HIT_COM_NULL;

		newCom.typeRecv = hit_comTypeRec( &selectRecv, baseType);
		newCom.dataRecv = hit_comSearchData(&selectRecv);
	}
#ifdef DEBUG
	printf("[%d] communication created\n", hit_Rank); fflush(stdout);
#endif

	/* 5. FILL UP THE OTHER FIELDS */
	newCom.comm = comm;
	newCom.tag = tag;
	newCom.requestSend = MPI_REQUEST_NULL;
	newCom.commType = HIT_SENDRECV;
	/*
	newCom.statusSend = ???;
	newCom.statusRecv = ???;
	*/

	/* 6. RETURN */
	return newCom;
}



/* Hit COMMIT REDUCE */
HitCom hit_comReduceSelect(	HitLayout lay,
								HitRanks root,
								const void * tilePSend,
								HitShape selectionSend,
								int selectSendMode,								
								const void * tilePRecv,
								HitShape selectionRecv,
								int selectRecvMode,								
								HitType baseType,
								HitOp operation){


	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 5. COMMUNICATION */
	/* 5.1 SEND */
	HitRanks rootActive = hit_layToActiveRanks(lay,root);
	newCom.sendTo = hit_layActiveRanksId(lay,rootActive);

	const HitTile *tileSend = (const HitTile *)tilePSend;
	HitTile selectSend;

	// @arturo, Feb 2013
	// HIT_TILE_NULL in the send tile parameter indicates MPI_IN_PLACE
	if ( tileSend->memStatus == HIT_MS_NULL ) {
		newCom.dataSend = MPI_IN_PLACE;
	}
	else {
		if (selectSendMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectSend, tileSend, selectionSend );
		else if (selectSendMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectSend, tileSend, selectionSend);
		else return HIT_COM_NULL;

		newCom.dataSend = hit_comSearchData(&selectSend);
	}

	/* 5.2 RECV */
	const HitTile *tileRecv = (const HitTile *)tilePRecv;
	HitTile selectRecv;

	/* if tile has no memory or no dimensions return null communication */
	if ( tileRecv->memStatus == HIT_MS_NULL || tileRecv->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileRecv)==0 ) return HIT_COM_NULL;

	if (selectRecvMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectRecv, tileRecv, selectionRecv );
	else if (selectRecvMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectRecv, tileRecv, selectionRecv);
	else return HIT_COM_NULL;

	newCom.dataRecv =  hit_comSearchData(&selectRecv);

	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_REDUCE;

	/* 7. REDUCTION OPERATION */
	newCom.operation = operation;

	/* 8. TYPE */
	// @arturo, Feb 2013, Due to the new MPI_IN_PLACE option, the selection of send may not exist.
	// 	Use recv selection to compute the type
	//newCom.typeSend = hit_comTypeRec( &selectSend, baseType);
	newCom.typeSend = hit_comTypeRec( &selectRecv, baseType);
	
	/* 9. RETURN */
	return newCom;
}

HitCom hit_comReduceDimSelect(	HitLayout lay,
								int dim,
								HitRanks root,
								const void * tilePSend,
								HitShape selectionSend,
								int selectSendMode,								
	
								const void * tilePRecv,
								HitShape selectionRecv,
								int selectRecvMode,	
								HitType baseType,
								HitOp operation){


	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	// @arturo Mar 2013
	//if (lay.pTopology[dim+1].lowLevel==NULL) return HIT_COM_NULL;
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[dim+1].lowLevel);
	if ( lay.pTopology[dim+1] == NULL ) return HIT_COM_NULL;
	MPI_Comm comm = lay.pTopology[ dim+1 ]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 5. COMMUNICATION */
	/* 5.1 SEND */
	newCom.sendTo = hit_topRankInternal(lay.topo,root);
	const HitTile *tileSend = (const HitTile *)tilePSend;
	HitTile selectSend;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tileSend->memStatus == HIT_MS_NULL || tileSend->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileSend)==0 ) return HIT_COM_NULL;

	if (selectSendMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectSend, tileSend, selectionSend );
	else if (selectSendMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectSend, tileSend, selectionSend);
	else return HIT_COM_NULL;

	newCom.dataSend =  hit_comSearchData(&selectSend);

	/* 5.2 RECV */
	const HitTile *tileRecv = (const HitTile *)tilePRecv;
	HitTile selectRecv;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tileRecv->memStatus == HIT_MS_NULL || tileRecv->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileRecv)==0 ) return HIT_COM_NULL;

	if (selectRecvMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectRecv, tileRecv, selectionRecv );
	else if (selectRecvMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectRecv, tileRecv, selectionRecv);
	else return HIT_COM_NULL;
	
	newCom.dataRecv =  hit_comSearchData(&selectRecv);

	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_REDUCE;

	/* 7. REDUCTION OPERATION */
	newCom.operation = operation;

	/* 8. TYPE */
	newCom.typeSend = hit_comTypeRec( &selectSend, baseType);
	
	/* 9. RETURN */
	return newCom;
}


/* Hit COMMIT BROADCAST */
HitCom hit_comBroadcastSelect(	HitLayout lay,
						   HitRanks root,
						   const void * tileP,
						   HitShape selection,
						   int selectMode,						   
						   HitType baseType){
	
	
	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	
	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;
	
	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );
	
	/* 5. COMMUNICATION */
	/* 5.1 SEND */
	HitRanks rootActive = hit_layToActiveRanks(lay,root);
	newCom.sendTo = hit_layActiveRanksId(lay,rootActive);

	const HitTile *tile = (const HitTile *)tileP;
	HitTile Select;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tile->memStatus == HIT_MS_NULL || tile->memStatus == HIT_MS_NOMEM || hit_tileDims(*tile)==0 ) return HIT_COM_NULL;

	if (selectMode == HIT_COM_TILECOORDS) hit_tileSelect( &Select, tile, selection );
	else if (selectMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &Select, tile, selection);
	else return HIT_COM_NULL;

	newCom.dataSend =  hit_comSearchData(&Select);
	
	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_BROADCAST;
	
	/* 7. TYPE */
	newCom.typeSend = hit_comTypeRec( &Select, baseType);
	
	/* 8. RETURN */
	return newCom;
}


HitCom hit_comBroadcastDimSelect(	HitLayout lay,
						   int dim,
						   int root,
						   const void * tileP,
						   HitShape selection,
						   int selectMode,						   
						   HitType baseType){
	
	/* 1. IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	const HitTile *tile = (const HitTile *)tileP;
	if ( tile->memStatus == HIT_MS_NULL || tile->memStatus == HIT_MS_NOMEM || hit_tileDims(*tile)==0 ) return HIT_COM_NULL;

	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	
	/* 3. GET TOPOLOGY COMMUNICATOR */
	// @arturo Mar 2013
	//if (lay.pTopology[dim+1].lowLevel==NULL) return HIT_COM_NULL;	
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[dim+1].lowLevel);
	if ( lay.pTopology[dim+1] == NULL ) return HIT_COM_NULL;
	MPI_Comm comm = lay.pTopology[ dim+1 ]->comm;
	newCom.comm = comm;
	
	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );
	
	/* 5. COMMUNICATION */
	/* 5.1 SEND */
	HitRanks rootRanks = hit_laySelfRanks( lay );
	if ( root != HIT_COM_MYSELF ) rootRanks.rank[ dim ] = root;
	HitRanks rootActive = hit_layToActiveRanks( lay, rootRanks );
	newCom.sendTo = rootActive.rank[dim];
	HitTile Select;
	if (selectMode == HIT_COM_TILECOORDS) hit_tileSelect( &Select, tile, selection );
	else if (selectMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &Select, tile, selection);
	else {
		fprintf(stderr,"Hit Warning: hit_comBroadcastDimSelect:	Unknown constant for coordinate type\n");
		return HIT_COM_NULL;
	}

	newCom.dataSend = hit_comSearchData(&Select);
	
	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_BROADCAST;
	
	/* 7. DATA TYPE */
	newCom.typeSend = hit_comTypeRec( &Select, baseType);
	
	/* 8. RETURN */
	return newCom;
}


HitCom hit_comAlltoallSelect(	HitLayout lay,
								const void * tilePSend,
								HitShape selectionSend,
								int selectSendMode,								
								const void * tilePRecv,
								HitShape selectionRecv,
								int selectRecvMode,								
								HitType baseType,
								int count){


	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 5. COMMUNICATION */
	/* 5.1. SEND */
	const HitTile *tileSend = (const HitTile *)tilePSend;
	HitTile selectSend;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tileSend->memStatus == HIT_MS_NULL || tileSend->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileSend)==0 ) return HIT_COM_NULL;

	if (selectSendMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectSend, tileSend, selectionSend );
	else if (selectSendMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectSend, tileSend, selectionSend);
	else return HIT_COM_NULL;

	newCom.dataSend = selectSend.data;

	/* 5.2. RECV */
	const HitTile *tileRecv = (const HitTile *)tilePRecv;
	HitTile selectRecv;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tileRecv->memStatus == HIT_MS_NULL || tileRecv->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileRecv)==0 ) return HIT_COM_NULL;

	if (selectRecvMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectRecv, tileRecv, selectionRecv );
	else if (selectRecvMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectRecv, tileRecv, selectionRecv);
	else return HIT_COM_NULL;

	newCom.dataRecv = selectRecv.data;

	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_ALLTOALL;

	/* 7. TYPE and COUNT */
	newCom.typeSend = baseType;
	newCom.count = count;

	/* 8. RETURN */
	return newCom;
}



HitCom hit_comAlltoallSelectv(	HitLayout lay,
								const void * tilePSend,
								HitShape * selectionSend,
								int selectSendMode,
								const void * tilePRecv,
								HitShape * selectionRecv,
								int selectRecvMode,
								HitType baseType){


	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	
	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );


	/* 5. COMMUNICATION */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}

	hit_malloc(newCom.alltoallv->sendcnts, int, numProcs );
	hit_malloc(newCom.alltoallv->sdispls, int, numProcs );
	hit_malloc(newCom.alltoallv->recvcnts, int, numProcs );
	hit_malloc(newCom.alltoallv->rdispls, int, numProcs );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(newCom.alltoallv->sendcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->sdispls ,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->recvcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->rdispls ,sizeof(int) * (size_t) numProcs,int*);
	*/
	

	const HitTile *tileSend = (const HitTile *)tilePSend;
	newCom.dataSend = tileSend->data;

	const HitTile *tileRecv = (const HitTile *)tilePRecv;
	newCom.dataRecv = tileRecv->data;

	
	

	for(i=0; i<numProcs; i++){

		// Send
		newCom.alltoallv->sendcnts[i] = hit_sigCard( hit_shapeSig(selectionSend[i],0));
		// With hit_sigTileToArray we get the displacement of the first position,
		// if HIT_COM_ARRAYCOORDS is selected the real displacement is calculated
		// subtracting the displacement of the first element of the array.
		if(selectSendMode == HIT_COM_TILECOORDS){
			newCom.alltoallv->sdispls[i] = hit_sigTileToArray(hit_shapeSig(selectionSend[i],0),0);
		} else {
			newCom.alltoallv->sdispls[i] = hit_sigTileToArray(hit_shapeSig(selectionSend[i],0),0) - 
										hit_sigTileToArray(hit_shapeSig(tileSend->shape,0),0);
		}
		
		// Recv
		newCom.alltoallv->recvcnts[i] = hit_sigCard(hit_shapeSig(selectionRecv[i],0));
		if(selectRecvMode == HIT_COM_TILECOORDS){
			newCom.alltoallv->rdispls[i] = hit_sigTileToArray(hit_shapeSig(selectionRecv[i],0),0);
		} else {
			newCom.alltoallv->rdispls[i] = hit_sigTileToArray(hit_shapeSig(selectionRecv[i],0),0) - 
										hit_sigTileToArray(hit_shapeSig(tileRecv->shape,0),0);
		
		}
	}



	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_ALLTOALLV;

	/* 7. TYPE  */
	newCom.typeSend = baseType;

	/* 8. RETURN */
	return newCom;


}


HitCom hit_comAllgather(	HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, int count){

	/* 1. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	/* 2. GET TOPOLOGY COMMUNICATOR */
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 3. START FILLING UP ITS FIELDS */
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 4. COMMUNICATION */
	/* 4.1. SEND */
	const HitTile *tileSend = (const HitTile *)tilePSend;
	//HitTile selectSend;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tileSend->memStatus == HIT_MS_NULL || tileSend->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileSend)==0 ) return HIT_COM_NULL;

	//if (selectSendMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectSend, tileSend, selectionSend );
	//else if (selectSendMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectSend, tileSend, selectionSend);
	//else return HIT_COM_NULL;

	//newCom.dataSend = selectSend.data;
	newCom.dataSend = tileSend->data;

	/* 4.2. RECV */
	const HitTile *tileRecv = (const HitTile *)tilePRecv;
	//HitTile selectRecv;
	/* IF TILE HAS NO MEMORY OR NO DIMENSIONS RETURN NULL COMMUNICATION */
	if ( tileRecv->memStatus == HIT_MS_NULL || tileRecv->memStatus == HIT_MS_NOMEM || hit_tileDims(*tileRecv)==0 ) return HIT_COM_NULL;

	//if (selectRecvMode == HIT_COM_TILECOORDS) hit_tileSelect( &selectRecv, tileRecv, selectionRecv );
	//else if (selectRecvMode == HIT_COM_ARRAYCOORDS) hit_tileSelectArrayCoords( &selectRecv, tileRecv, selectionRecv);
	//else return HIT_COM_NULL;

	//newCom.dataRecv = selectRecv.data;
	newCom.dataRecv = tileRecv->data;

	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_ALLGATHER;

	/* 7. TYPE and COUNT */
	newCom.typeSend = baseType;
	newCom.count = count;

	/* 8. RETURN */
	return newCom;
}



// @note @javfres Not used anymore in MatMult.
HitCom hit_comAllGathervInternal(HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line){

	HIT_NOT_USED(file);
	HIT_NOT_USED(line);

	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;

	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );


	/* 5. COMMUNICATION */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}

	hit_malloc(newCom.alltoallv->recvcnts, int, numProcs );
	hit_malloc(newCom.alltoallv->rdispls, int, numProcs );
	/*
	hit_malloc(newCom.alltoallv->recvcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->rdispls ,sizeof(int) * (size_t) numProcs,int*);
	*/

	const HitTile *tileSend = (const HitTile *)tilePSend;
	newCom.dataSend = tileSend->data;

	const HitTile *tileRecv = (const HitTile *)tilePRecv;
	newCom.dataRecv = tileRecv->data;

// SEND
	HitShape myshape = hit_layShape(lay);
	newCom.count = hit_sigCard(hit_shapeSig(myshape,0));


// RECV
	int acum_disp = 0;
	for(i=0; i<numProcs; i++){

		HitRanks ranks = HIT_RANKS_NULL;
		ranks.rank[0] = i;

		HitShape shape = hit_layShapeOther(lay, ranks);

		//printf("[%d] From %d: [%d-%d]\n",newCom.myself,i,hit_shapeSig(shape,0).begin,hit_shapeSig(shape,0).end);

		newCom.alltoallv->recvcnts[i] = hit_sigCard(hit_shapeSig(shape,0));
		newCom.alltoallv->rdispls[i] = acum_disp;
		acum_disp += hit_sigCard(hit_shapeSig(shape,0));

		//printf("[%d] From %d: Recv[counts:%d, disp:%d]\n",newCom.myself,i,newCom.alltoallv->recvcnts[i],newCom.alltoallv->rdispls[i]);
	}


	/* 6. COMMUNICATION TYPE */
	newCom.commType = HIT_ALLGATHERV;

	/* 7. TYPE  */
	newCom.typeSend = baseType;
	newCom.typeRecv = baseType;

	/* 8. RETURN */
	return newCom;



}







HitCom hit_comSparseUpdate(HitLayout lay, const void * tileP, HitType baseType){

	const HitTile *tile = (const HitTile *)tileP;

	// Switch to select the appropriate function.
	// @arturo Aug 2015: Change in a name of internal function
	switch(hit_tileClass(*tile)){
		case HIT_GC_TILE:
			return hit_comSparseUpdateCSR(lay,tileP,baseType);
		case HIT_GB_TILE:
			return hit_comSparseUpdateBitmap(lay,tileP,baseType);
		default:
			hit_errInternal(__func__, "Unsupported shape type", "", __FILE__, __LINE__);
			return HIT_COM_NULL;
	}
}


//#define DEBUG_LAY_METIS

#ifdef DEBUG_LAY_METIS
#define debug(...) { if( hit_Rank == 0 ) { printf(__VA_ARGS__); fflush(stdout); }}
#define debugall(...) { printf(__VA_ARGS__);}
#endif

HitCom hit_comSparseUpdateCSR(HitLayout lay, const void * tileP, HitType baseType){


	/* 1. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	//@javfres 2015-10-05 Fix for inactive processes
	if(!lay.active) return newCom;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv->sparse,sizeof(HitComSparse),HitComSparse*);
	hit_malloc(newCom.alltoallv->sparse, HitComSparse, 1);
	*(newCom.alltoallv->sparse) = HIT_COM_SPARSE_NULL;


	/* 2. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 3. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 4. COMMUNICATION */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}
	
		
	/* 6. HIT TILE */
	const HitTile *tile = (const HitTile *)tileP;
	
	
	HitShape ext_shape = tile->shape;
	

	/* 5. INIT THE SEND AND RECV ARRAYS */
	
	// Shape is the original shape.
	HitShape shape = lay.origShape;
	
	// Part is the Metis output.
	int * part = lay.info.layoutList.assignedGroups;
	
	// Local is an array with the local + neighbor vertices.
	int * local;
	
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(local,sizeof(int) * (size_t) hit_cShapeNvertices(shape), int*);
	hit_malloc(local, int, hit_cShapeNvertices(shape) );

	for(i=0;i<hit_cShapeNvertices(shape);i++){
		if(part[i] == lay.group){
			local[i] = 1;
		} else {
			local[i] = 0;
		}
	}
	
	// Set as local the neighbor vertices
	for(i=hit_cShapeNvertices(lay.shape);i<hit_cShapeNvertices(ext_shape);i++){
	
		int gvertex = hit_cShapeVertexToGlobal(ext_shape,i);
		int overtex = hit_cShapeVertexToLocal(shape,gvertex);
		local[overtex] = 1;
	}
	
	
	// 5.1 Calculate the recv
	int * recvcnts;
	int * recvdispls;
	int * auxcounts;
	
	hit_calloc(recvcnts,  int, numProcs);
	hit_calloc(recvdispls,int, numProcs);
	hit_calloc(auxcounts, int, numProcs);
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_calloc(recvcnts,  sizeof(int), (size_t) numProcs,int*);
	hit_calloc(recvdispls,sizeof(int), (size_t) numProcs,int*);
	hit_calloc(auxcounts, sizeof(int), (size_t) numProcs,int*);
	*/

	int nrecv = 0;
	for(i=0; i<hit_cShapeNvertices(shape); i++){

		if( local[i] && part[i] != lay.group){
			recvcnts[part[i]] ++;
			nrecv ++;
		}
	}

	recvdispls[0] = 0;
	for(i=1;i<numProcs;i++){
		recvdispls[i] = recvdispls[i-1] + recvcnts[i-1];
	}


	int * recv;
	hit_malloc(recv, int, nrecv);
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(recv,sizeof(int) * (size_t) nrecv,int*);

	for(i=0; i<hit_cShapeNvertices(shape); i++){

		if( local[i] && part[i] != lay.group){
			int p = part[i];
			recv[ recvdispls[p] +  auxcounts[p] ] = hit_cShapeNameList(shape,0).names[i];
			auxcounts[p]++;
		}
	}

#ifdef DEBUG_LAY_METIS
	{ sleep(hit_Rank); debugall("I recv: [");
	int p, i;
	for(p=0;p<numProcs;p++){
		debugall("p%d(%d): ",p,recvcnts[p]);
		for(i=0;i<recvcnts[p];i++){
			debugall("%d ",recv[ recvdispls[p] + i ]);
		}
	} debugall("]\n");}
#endif
	
	
	// 6.2 Calculate the send
	int * sendcnts;
	int * senddispls;
	
	hit_malloc(sendcnts, int, numProcs);
	hit_malloc(senddispls, int, numProcs);
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(sendcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(senddispls,sizeof(int) * (size_t) numProcs,int*);
	*/

	int ok = MPI_Alltoall(recvcnts, 1, MPI_INT, sendcnts, 1, MPI_INT, comm);
	hit_mpiTestError(ok,"Failed while sending the recvcnts");
	
	senddispls[0] = 0;
	for(i=1;i<numProcs;i++){
		senddispls[i] = senddispls[i-1] + sendcnts[i-1];
	}
	
	
	int * send;
	hit_malloc(send, int, senddispls[numProcs-1] + sendcnts[numProcs-1] );
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(send,sizeof(int) * (size_t) ( senddispls[numProcs-1] + sendcnts[numProcs-1] ),int*);
	
	
	MPI_Alltoallv(recv, recvcnts, recvdispls,
                         MPI_INT, send, sendcnts,
                         senddispls, MPI_INT, comm);
	hit_mpiTestError(ok,"Failed while sending the recv list of vertices.");
	

#ifdef DEBUG_LAY_METIS
	{ sleep(hit_Rank); debugall("I send: [");
	int p, i;
	for(p=0;p<numProcs;p++){
		debugall("p%d(%d): ",p,sendcnts[p]);
		for(i=0;i<sendcnts[p];i++){
			debugall("%d ",send[ senddispls[p] + i ]);
		}
	} debugall("]\n");}
#endif

	
	newCom.alltoallv->sendcnts	= sendcnts;
	newCom.alltoallv->sdispls 	= senddispls;
	newCom.alltoallv->recvcnts	= recvcnts;
	newCom.alltoallv->rdispls 	= recvdispls;
	newCom.alltoallv->sparse->send = send;
	newCom.alltoallv->sparse->recv = recv;
	newCom.alltoallv->sparse->nsend = newCom.alltoallv->sdispls[numProcs-1] + newCom.alltoallv->sendcnts[numProcs-1];
	newCom.alltoallv->sparse->nrecv = newCom.alltoallv->rdispls[numProcs-1] + newCom.alltoallv->recvcnts[numProcs-1];


	hit_vmalloc(newCom.dataSend, tile->baseExtent * (size_t) newCom.alltoallv->sparse->nsend );
	hit_vmalloc(newCom.dataRecv, tile->baseExtent * (size_t) newCom.alltoallv->sparse->nrecv );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(newCom.dataSend,tile->baseExtent * (size_t) newCom.alltoallv->sparse->nsend,void*);
	hit_malloc(newCom.dataRecv,tile->baseExtent * (size_t) newCom.alltoallv->sparse->nrecv,void*);
	*/
	
	/* 7. FILL THE SPARSE RELATED FIELDS */
	newCom.alltoallv->sparse->shape = tile->shape;
	newCom.alltoallv->sparse->originData = tile->dataVertices;


	/* 8. COMMUNICATION TYPE */
	newCom.commType = HIT_SPARSE_UPDATE;

	/* 9. TYPE  */
	newCom.typeSend = baseType;
	newCom.alltoallv->sparse->baseExtent = tile->baseExtent;

	// 10. Release resources.
	free(auxcounts);
	free(local);

	
	/* 11. RETURN */
	return newCom;
}


HitCom hit_comSparseUpdateBitmap(HitLayout lay, const void * tileP, HitType baseType){

	/* 1. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	//@javfres 2015-10-05 Fix for inactive processes
	if(!lay.active) return newCom;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv->sparse,sizeof(HitComSparse),HitComSparse*);
	hit_malloc(newCom.alltoallv->sparse, HitComSparse, 1);
	*(newCom.alltoallv->sparse) = HIT_COM_SPARSE_NULL;


	/* 2. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 3. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 4. COMMUNICATION */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}
	
		
	/* 6. HIT TILE */
	const HitTile *tile = (const HitTile *)tileP;
	
	
	HitShape ext_shape = tile->shape;
	

	/* 5. INIT THE SEND AND RECV ARRAYS */
	
	// Shape is the original shape.
	HitShape shape = lay.origShape;
	
	// Part is the Metis output.
	int * part = lay.info.layoutList.assignedGroups;
	
	// Local is an array with the local + neighbor vertices.
	int * local;
	
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(local,sizeof(int) * (size_t) hit_bShapeNvertices(shape),int*);
	hit_malloc(local, int, hit_bShapeNvertices(shape));

	for(i=0;i<hit_bShapeNvertices(shape);i++){
		if(part[i] == lay.group){
			local[i] = 1;
		} else {
			local[i] = 0;
		}
	}
	
	// Set as local the neighbor vertices
	for(i=hit_bShapeNvertices(lay.shape);i<hit_bShapeNvertices(ext_shape);i++){
	
		int gvertex = hit_bShapeVertexToGlobal(ext_shape,i);
		int overtex = hit_bShapeVertexToLocal(shape,gvertex);
		local[overtex] = 1;
	}
	
	
	// 5.1 Calculate the recv
	int * recvcnts;
	int * recvdispls;
	int * auxcounts;
	
	hit_calloc(recvcnts,  int, numProcs);
	hit_calloc(recvdispls,int, numProcs);
	hit_calloc(auxcounts, int, numProcs);
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_calloc(recvcnts,  sizeof(int), (size_t) numProcs,int*);
	hit_calloc(recvdispls,sizeof(int), (size_t) numProcs,int*);
	hit_calloc(auxcounts, sizeof(int), (size_t) numProcs,int*);
	*/


	int nrecv = 0;
	for(i=0; i<hit_bShapeNvertices(shape); i++){

		if( local[i] && part[i] != lay.group){
			recvcnts[part[i]] ++;
			nrecv ++;
		}
	}

	recvdispls[0] = 0;
	for(i=1;i<numProcs;i++){
		recvdispls[i] = recvdispls[i-1] + recvcnts[i-1];
	}


	int * recv;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(recv,sizeof(int) * (size_t) nrecv, int*);
	hit_malloc(recv, int, nrecv);

	for(i=0; i<hit_bShapeNvertices(shape); i++){

		if( local[i] && part[i] != lay.group){
			int p = part[i];
			recv[ recvdispls[p] +  auxcounts[p] ] = hit_bShapeNameList(shape,0).names[i];
			auxcounts[p]++;
		}
	}

#ifdef DEBUG_LAY_METIS
	{ sleep(hit_Rank); debugall("I recv: [");
	int p, i;
	for(p=0;p<numProcs;p++){
		debugall("p%d(%d): ",p,recvcnts[p]);
		for(i=0;i<recvcnts[p];i++){
			debugall("%d ",recv[ recvdispls[p] + i ]);
		}
	} debugall("]\n");}
#endif
	
	
	// 6.2 Calculate the send
	int * sendcnts;
	int * senddispls;
	
	hit_malloc(sendcnts, int, numProcs);
	hit_malloc(senddispls, int,  numProcs);
	/*
	hit_malloc(sendcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(senddispls,sizeof(int) * (size_t) numProcs,int*);
	*/

	int ok = MPI_Alltoall(recvcnts, 1, MPI_INT, sendcnts, 1, MPI_INT, comm);
	hit_mpiTestError(ok,"Failed while sending the recvcnts");
	
	senddispls[0] = 0;
	for(i=1;i<numProcs;i++){
		senddispls[i] = senddispls[i-1] + sendcnts[i-1];
	}
	
	
	int * send;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(send,sizeof(int) * (size_t) ( senddispls[numProcs-1] + sendcnts[numProcs-1] ),int*);
	hit_malloc(send, int, senddispls[numProcs-1] + sendcnts[numProcs-1] );
	
	
	MPI_Alltoallv(recv, recvcnts, recvdispls,
                         MPI_INT, send, sendcnts,
                         senddispls, MPI_INT, comm);
	hit_mpiTestError(ok,"Failed while sending the recv list of vertices.");
	
	

#ifdef DEBUG_LAY_METIS
	{ sleep(hit_Rank); debugall("I send: [");
	int p, i;
	for(p=0;p<numProcs;p++){
		debugall("p%d(%d): ",p,sendcnts[p]);
		for(i=0;i<sendcnts[p];i++){
			debugall("%d ",send[ senddispls[p] + i ]);
		}
	} debugall("]\n");}
#endif


	
	newCom.alltoallv->sendcnts	= sendcnts;
	newCom.alltoallv->sdispls 	= senddispls;
	newCom.alltoallv->recvcnts	= recvcnts;
	newCom.alltoallv->rdispls 	= recvdispls;
	newCom.alltoallv->sparse->send = send;
	newCom.alltoallv->sparse->recv = recv;
	newCom.alltoallv->sparse->nsend = newCom.alltoallv->sdispls[numProcs-1] + newCom.alltoallv->sendcnts[numProcs-1];
	newCom.alltoallv->sparse->nrecv = newCom.alltoallv->rdispls[numProcs-1] + newCom.alltoallv->recvcnts[numProcs-1];


	hit_vmalloc(newCom.dataSend, tile->baseExtent * (size_t) newCom.alltoallv->sparse->nsend);
	hit_vmalloc(newCom.dataRecv, tile->baseExtent * (size_t) newCom.alltoallv->sparse->nrecv);
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(newCom.dataSend,tile->baseExtent * (size_t) newCom.alltoallv->sparse->nsend,void*);
	hit_malloc(newCom.dataRecv,tile->baseExtent * (size_t) newCom.alltoallv->sparse->nrecv,void*);
	*/
	
	/* 7. FILL THE SPARSE RELATED FIELDS */
	newCom.alltoallv->sparse->shape = tile->shape;
	newCom.alltoallv->sparse->originData = tile->dataVertices;

	/* 8. COMMUNICATION TYPE */
	newCom.commType = HIT_SPARSE_UPDATE;

	/* 9. TYPE  */
	newCom.typeSend = baseType;
	newCom.alltoallv->sparse->baseExtent = tile->baseExtent;

	// 10. Release resources.
	free(auxcounts);
	free(local);

	
	/* 11. RETURN */
	return newCom;
}






HitCom hit_comSparseScatterInternal(HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line){

	if(!lay.active) return HIT_COM_NULL;


	/* 1. HIT TILE */
	const HitTile *tileSend = (const HitTile *) tilePSend;
	const HitTile *tileRecv = (const HitTile *) tilePRecv;
	
	
	// @arturo 2015/01/22
	//if(lay.topo.linearRank == 0 && hit_tileType(*tileSend) != hit_tileType(*tileRecv)){
	// @arturo Aug 2015: Change in a name of internal function
	//if( hit_topSelfRankInternal( lay.topo ) == 0 && hit_tileType(*tileSend) != hit_tileType(*tileRecv)){
	if( hit_topSelfRankInternal( lay.topo ) == 0 && hit_tileClass(*tileSend) != hit_tileClass(*tileRecv)){
		hit_errInternal(__func__, "Different HitTile classes", "", file, line);
	}
	
	//if(hit_tileType(*tileRecv) != HIT_GC_TILE &&  hit_tileType(*tileRecv) != HIT_GB_TILE){
	if(hit_tileClass(*tileRecv) != HIT_GC_TILE &&  hit_tileClass(*tileRecv) != HIT_GB_TILE){
		hit_errInternal(__func__, "Unsupported HitTile class", "", __FILE__, __LINE__);
	}
	
	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;	
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv->sparse,sizeof(HitComSparse),HitComSparse*);
	hit_malloc(newCom.alltoallv->sparse, HitComSparse, 1);
	*(newCom.alltoallv->sparse) = HIT_COM_SPARSE_NULL;
	

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 5. COMMUNICATION */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}

	/* 5. INIT THE SEND AND RECV ARRAYS */
	// @arturo Aug 2015: Change in a name of internal function
	//if(hit_tileType(*tileRecv) == HIT_GC_TILE){
	if(hit_tileClass(*tileRecv) == HIT_GC_TILE){
		newCom.count = hit_cShapeNvertices(lay.shape);
	} else {
		newCom.count = hit_bShapeNvertices(lay.shape);
	}


	if(newCom.myself == 0){

		hit_calloc(newCom.alltoallv->sendcnts, int, numProcs);
		hit_malloc(newCom.alltoallv->sdispls, int, numProcs);
		// @arturo Ago 2015: New allocP interface
		/* 
		hit_calloc(newCom.alltoallv->sendcnts,(size_t) numProcs, sizeof(int),int*);
		hit_malloc(newCom.alltoallv->sdispls,sizeof(int) * (size_t) numProcs,int*);
		*/

		for(i=0;i<lay.info.layoutList.numElementsTotal;i++){
			newCom.alltoallv->sendcnts[ lay.info.layoutList.assignedGroups[i] ]++;
		}

		newCom.alltoallv->sdispls[0] = 0;
		for(i=1;i<numProcs;i++){
			newCom.alltoallv->sdispls[i] = newCom.alltoallv->sdispls[i-1] + newCom.alltoallv->sendcnts[i-1];
		}

		int total_size = newCom.alltoallv->sdispls[numProcs-1] + newCom.alltoallv->sendcnts[numProcs-1];
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(newCom.dataSend,tileSend->baseExtent * (size_t) total_size,void*);
		hit_vmalloc(newCom.dataSend, tileSend->baseExtent * (size_t) total_size);
		
		newCom.alltoallv->sparse->originData = tileSend->dataVertices;
		newCom.alltoallv->sparse->nsend = lay.info.layoutList.numElementsTotal;
		newCom.alltoallv->sparse->send = lay.info.layoutList.assignedGroups;
	}
	newCom.dataRecv = tileRecv->dataVertices;


	/* 8. COMMUNICATION TYPE */
	newCom.commType = HIT_SPARSE_SCATTER;

	/* 9. TYPE  */
	newCom.typeSend = baseType;
	newCom.typeRecv = baseType;

	newCom.alltoallv->sparse->baseExtent = tileSend->baseExtent;

	/* 10. RETURN */
	return newCom;

}


HitCom hit_comAllDistribute(	HitLayout lay,
								const void * tilePSend, 
								int * count, 
								const void * tilePRecv, 
								HitType baseType){


	/* 1. CHECK THE LAYOUT TYPE */	
	if(lay.type != HIT_LAYOUT_CONTIGUOUS){
		hit_errInternal("hit_comAlldistribute", "This communication works only with a contiguous layout list", "", __FILE__, __LINE__);
	}

	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;

	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );
	newCom.dataSend = ((const HitTile *)tilePSend)->data;
	newCom.dataRecv = ((const HitTile *)tilePRecv)->data;

	/* 5. CALCULATE NUMBER OF SETS */
	/*
	int i;
	int nSets = 1;
	for(i=0;i<hit_shapeDims(lay.origShape);i++){
		nSets *= hit_sigCard(hit_shapeSig(lay.origShape,i));
	}
	*/
	int nSets = hit_shapeCard( lay.origShape );

	/* 6. COMMUNICATION OF PART SIZES */
	int i;
	int numProcs = 1;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}

	hit_malloc(newCom.alltoallv->sendcnts, int, numProcs);
	hit_malloc(newCom.alltoallv->sdispls , int, numProcs);
	hit_malloc(newCom.alltoallv->recvcnts, int, numProcs);
	hit_malloc(newCom.alltoallv->rdispls , int, numProcs);
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(newCom.alltoallv->sendcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->sdispls ,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->recvcnts,sizeof(int) * (size_t) numProcs,int*);
	hit_malloc(newCom.alltoallv->rdispls ,sizeof(int) * (size_t) numProcs,int*);
	*/

	newCom.alltoallv->sdispls[0] = 0;
	newCom.alltoallv->sendcnts[0] = 0;
	int g,g_ant;
	g = g_ant = 0;
	for(i=0;i<nSets;i++){

		g = hit_lgr_elementGroup(lay,i);

		if(g != g_ant){
			newCom.alltoallv->sdispls[g] = newCom.alltoallv->sdispls[g-1] + newCom.alltoallv->sendcnts[g-1];
			newCom.alltoallv->sendcnts[g] = 0;
			g_ant = g;
		}
		newCom.alltoallv->sendcnts[g] += count[i];

	}

	// @arturo: 2012-Jun, BUG CORRECTED (memory overrun detected with valgrind) 
	//while( g < numProcs ){
	while( g < numProcs-1 ){
		g++;
		newCom.alltoallv->sendcnts[g] = 0;
	}

	int ok = MPI_Alltoall(newCom.alltoallv->sendcnts, 1, MPI_INT,newCom.alltoallv->recvcnts, 1, MPI_INT,comm);
	hit_mpiTestError(ok,"hit_comAllDistribute: Failed Alltoall");

	/* 7. COMPUTE DISPLACEMENTS AND SIZE OF RECEIVING BUFFER */
	newCom.alltoallv->rdispls[0] = 0;
	newCom.count = newCom.alltoallv->recvcnts[0];
	for(i=1; i<numProcs; i++) {
		newCom.alltoallv->rdispls[i] = newCom.alltoallv->rdispls[i-1] + newCom.alltoallv->recvcnts[i-1];
		newCom.count += newCom.alltoallv->recvcnts[i];
	}

	/* DEBUG ASSERT: CHECK THE SIZE OF THE RECEIVING BUFFER */
	if( newCom.count < hit_tileCard( *(const HitTile *)tilePRecv ) ){
		hit_errInternal("hit_comAlldistribute", "The receiving buffer does not have enough cardinality", "", __FILE__, __LINE__);
	}

	/* 8. COMMUNICATION TYPE */
	newCom.commType = HIT_ALLTOALLV; //HIT_ALLDISTRIBUTE;

	/* 8. TYPE  */
	newCom.typeSend = baseType;

	/* 9. RETURN */
	return newCom;

}



HitCom hit_comSparseScatterRowsInternal(HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line){

	/* 1. HIT TILE */
	const HitTile *tileSend = (const HitTile *) tilePSend;
	const HitTile *tileRecv = (const HitTile *) tilePRecv;


	// @arturo 2015/01/22
	//if(lay.topo.linearRank == 0 && hit_tileType(*tileSend) != hit_tileType(*tileRecv)){
	// @arturo Aug 2015: Change in a name of internal function
	//if( hit_topSelfRankInternal( lay.topo ) == 0 && hit_tileType(*tileSend) != hit_tileType(*tileRecv)){
	if( hit_topSelfRankInternal( lay.topo ) == 0 && hit_tileClass(*tileSend) != hit_tileClass(*tileRecv)){
		hit_errInternal(__func__, "Different HitTile classes", "", file, line);
	}

	// @arturo Aug 2015: Change in a name of internal function
	//if(hit_tileType(*tileRecv) != HIT_MC_TILE ){
	if(hit_tileClass(*tileRecv) != HIT_MC_TILE ){
		hit_errInternal(__func__, "Unsupported HitTile class", "", __FILE__, __LINE__);
	}

	/* 2. DECLARE NEW COMMUNICATION ISSUE */
	HitCom newCom = HIT_COM_NULL;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv),HitComAlltoallv*);
	hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
	*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newCom.alltoallv->sparse,sizeof(HitComSparse),HitComSparse*);
	hit_malloc(newCom.alltoallv->sparse, HitComSparse, 1);
	*(newCom.alltoallv->sparse) = HIT_COM_SPARSE_NULL;


	/* 3. GET TOPOLOGY COMMUNICATOR */
	//MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
	MPI_Comm comm = lay.pTopology[0]->comm;
	newCom.comm = comm;

	/* 4. START FILLING UP ITS FIELDS */
	// @arturo 2015/01/22
	//newCom.myself = lay.topo.linearRank;
	newCom.myself = hit_topSelfRankInternal( lay.topo );

	/* 5. COMMUNICATION */
	int numProcs = 1;
	int i;
	for(i=0;i<lay.topo.numDims;i++){
		numProcs *= lay.numActives[i];
	}

	/* 5. INIT THE SEND AND RECV ARRAYS */
	newCom.count = hit_cShapeNZElems(lay.shape);


	if(newCom.myself == 0){

		hit_calloc(newCom.alltoallv->sendcnts, int, numProcs);
		hit_malloc(newCom.alltoallv->sdispls, int, numProcs);
		/*
		hit_calloc(newCom.alltoallv->sendcnts,(size_t) numProcs, sizeof(int),int*);
		hit_malloc(newCom.alltoallv->sdispls,sizeof(int) * (size_t) numProcs,int*);
		*/

		for(i=0;i<lay.info.layoutList.numElementsTotal;i++){
			int p = lay.info.layoutList.assignedGroups[i];

			newCom.alltoallv->sendcnts[ p ] += hit_cShapeNColsRow(lay.origShape,i);

			//printf("Send row[%d] with %d cols to %d\n",i,hit_cShapeNColsRow(lay.origShape,i),p);
		}

		newCom.alltoallv->sdispls[0] = 0;
		for(i=1;i<numProcs;i++){
			newCom.alltoallv->sdispls[i] = newCom.alltoallv->sdispls[i-1] + newCom.alltoallv->sendcnts[i-1];
		}

		int total_size = newCom.alltoallv->sdispls[numProcs-1] + newCom.alltoallv->sendcnts[numProcs-1];
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(newCom.dataSend,tileSend->baseExtent * (size_t) total_size,void*);
		hit_vmalloc(newCom.dataSend, tileSend->baseExtent * (size_t) total_size );

		newCom.alltoallv->sparse->originData = tileSend->data;
		newCom.alltoallv->sparse->nsend = lay.info.layoutList.numElementsTotal;
		newCom.alltoallv->sparse->send = lay.info.layoutList.assignedGroups;

		newCom.alltoallv->sparse->rows = hit_cShapeXadj(lay.origShape);
	}
	newCom.dataRecv = tileRecv->data;


	/* 8. COMMUNICATION TYPE */
	newCom.commType = HIT_SPARSE_SCATTER_ROWS;

	/* 9. TYPE  */
	newCom.typeSend = baseType;
	newCom.typeRecv = baseType;

	newCom.alltoallv->sparse->baseExtent = tileSend->baseExtent;

	/* 10. RETURN */
	return newCom;

}


#ifdef XXXXXXX
HitCom hit_comAllDistribute(	HitLayout lay,
								const void * tilePSend, int * count, 
								const void * tilePRecv, HitType baseType){

	
	if(lay.type != HIT_LAYOUT_CONTIGUOUS){
		hit_errInternal("hit_comAlldistribute", "This communication works only with a contiguous layout list", "", "", -1);
	}

		/* 2. DECLARE NEW COMMUNICATION ISSUE */
		HitCom newCom = HIT_COM_NULL;
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(newCom.alltoallv,sizeof(HitComAlltoallv));
		hit_malloc(newCom.alltoallv, HitComAlltoallv, 1);
		*(newCom.alltoallv) = HIT_COM_ALLTOALLV_NULL;


		/* 3. GET TOPOLOGY COMMUNICATOR */
		MPI_Comm comm = *((MPI_Comm *)lay.pTopology[0].lowLevel);
		newCom.comm = comm;

		/* 4. START FILLING UP ITS FIELDS */
		// @arturo 2015/01/22
		//newCom.myself = lay.topo.linearRank;
		newCom.myself = hit_topSelfRankInternal( lay.topo );

		int i;

		/* CALCULATE NUMBER OF SETS */
		int nSets = 1;
		for(i=0;i<hit_shapeDims(lay.origShape);i++){
			nSets *= hit_sigCard(hit_shapeSig(lay.origShape,i));
		}

		/* 5. COMMUNICATION */
		int numProcs = 1;
		for(i=0;i<lay.topo.numDims;i++){
			numProcs *= lay.numActives[i];
		}


		hit_malloc(newCom.alltoallv->sendcnts, int, numProcs);
		hit_malloc(newCom.alltoallv->sdispls , int, numProcs);
		hit_malloc(newCom.alltoallv->recvcnts, int, numProcs);
		hit_malloc(newCom.alltoallv->rdispls , int, numProcs);
		// @arturo Ago 2015: New allocP interface
		/* 
		hit_malloc(newCom.alltoallv->sendcnts,sizeof(int) * (size_t) numProcs);
		hit_malloc(newCom.alltoallv->sdispls ,sizeof(int) * (size_t) numProcs);
		hit_malloc(newCom.alltoallv->recvcnts,sizeof(int) * (size_t) numProcs);
		hit_malloc(newCom.alltoallv->rdispls ,sizeof(int) * (size_t) numProcs);
		*/


		newCom.alltoallv->sdispls[0] = 0;
		newCom.alltoallv->sendcnts[0] = 0;
		int g,g_ant;
		g = g_ant = 0;
		for(i=0;i<nSets;i++){

			g = hit_lgr_elementGroup(lay,i);

			if(g != g_ant){
				newCom.alltoallv->sdispls[g] = newCom.alltoallv->sdispls[g-1] + newCom.alltoallv->sendcnts[g-1];
				newCom.alltoallv->sendcnts[g] = 0;
				g_ant = g;
			}
			newCom.alltoallv->sendcnts[g] += count[i];

		}

	    while( g < numProcs ){
			g++;
			newCom.alltoallv->sendcnts[g] = 0;
	    }






		int ok = MPI_Alltoall(newCom.alltoallv->sendcnts, 1, MPI_INT,newCom.alltoallv->recvcnts, 1, MPI_INT,comm);
		hit_mpiTestError(ok,"hit_comAllDistribute: Failed Alltoall");

		newCom.alltoallv->rdispls[0] = 0;
	    for(i=1; i<numProcs; i++)
	    	newCom.alltoallv->rdispls[i] = newCom.alltoallv->rdispls[i-1] + newCom.alltoallv->recvcnts[i-1];




		const HitTile *tileSend = (const HitTile *)tilePSend;
		newCom.dataSend = tileSend->data;

		const HitTile *tileRecv = (const HitTile *)tilePRecv;
		newCom.dataRecv = tileRecv->data;


		/* 6. COMMUNICATION TYPE */
		newCom.commType = HIT_ALLTOALLV; //HIT_ALLDISTRIBUTE;

		/* 7. TYPE  */
		newCom.typeSend = baseType;


		/* 8. RETURN */
		return newCom;

}
#endif


// @note @javfres This is used in the heat benchmark to use
// the same communication with different tiles. It is not pretty
// and it only works for vertices, not edges.
void hit_comUpdateOriginData(HitCom * com, const void * tileP){
	//@javfres 2015-10-05 Fix for inactive processes
	if(com->commType == HIT_COMTYPE_NULL) return;
	const HitTile *tile = (const HitTile *)tileP;
	com->alltoallv->sparse->originData = tile->dataVertices;
}




void hit_comFree(HitCom issue){

	switch ( issue.commType) {

		case HIT_SENDRECV:
		case HIT_REDUCE:
		case HIT_BROADCAST:
		case HIT_SENDRECV_REPLACE:
			hit_comFreeType(issue.typeSend);
			hit_comFreeType(issue.typeRecv);
			break;

		case HIT_ALLTOALL:
			break;

		case HIT_ALLTOALLV:
			free(issue.alltoallv->sendcnts);
			free(issue.alltoallv->sdispls);
			free(issue.alltoallv->recvcnts);
			free(issue.alltoallv->rdispls);
			free(issue.alltoallv);
			break;
		// @todo @javfres Free the lists that are now created in the communication.
		case HIT_SPARSE_UPDATE:
			free(issue.dataSend);
			free(issue.dataRecv);
			free(issue.alltoallv->sparse);
			free(issue.alltoallv);
			break;
		case HIT_SPARSE_SCATTER:
			if(hit_Rank == 0){
				free(issue.alltoallv->sendcnts);
				free(issue.alltoallv->sdispls);
				free(issue.dataSend);
			}
			free(issue.alltoallv->sparse);
			free(issue.alltoallv);
			break;
		case HIT_ALLGATHERV:
			free(issue.alltoallv->recvcnts);
			free(issue.alltoallv->rdispls);
			free(issue.alltoallv);
			break;
		default:
			break;
	};

}







/* Hit START COMMITED SEND COMMUNICATION */
void hit_comStartSend(HitCom *issue) {
#ifdef DEBUG
		printf("CTRL %d - Sends (async) from %d to %d\n", hit_Rank, issue->myself, issue->sendTo);
#endif

		if ( issue->sendTo == MPI_PROC_NULL ) return;
		int ok = MPI_Isend( issue->dataSend, 1, issue->typeSend, issue->sendTo, issue->tag, issue->comm, &(issue->requestSend) );
		hit_mpiTestError(ok,"Failed start send");
}

/* Hit END COMMITED SEND COMMUNICATION */
void hit_comEndSend(HitCom *issue) {
#ifdef DEBUG
		printf("CTRL %d - End Send from %d to %d\n", hit_Rank, issue->myself, issue->sendTo);
#endif

		if ( issue->sendTo == MPI_PROC_NULL ) return;
		int ok = MPI_Wait( &(issue->requestSend), MPI_STATUS_IGNORE );
		hit_mpiTestError(ok,"Failed end send");
}

/* Hit COMMITED SEND COMMUNICATION */
void hit_comDoSend(HitCom *issue) {
#ifdef DEBUG
		printf("CTRL %d - Sends from %d to %d\n", hit_Rank, issue->myself, issue->sendTo);
#endif
		if ( issue->sendTo == MPI_PROC_NULL ) return;
		int ok = MPI_Send( issue->dataSend, 1, issue->typeSend, issue->sendTo, issue->tag, issue->comm );
		hit_mpiTestError(ok,"Failed send");
}


/* Hit COMMITED RECEIVE COMMUNICATION */
void hit_comDoRecv(HitCom *issue) {
#ifdef DEBUG
		printf("CTRL %d - Receives from %d to %d\n", hit_Rank, issue->recvFrom, issue->myself);
#endif
		if ( issue->recvFrom == MPI_PROC_NULL ) return;
		int ok = MPI_Recv( issue->dataRecv, 1, issue->typeRecv, issue->recvFrom, issue->tag, issue->comm, MPI_STATUS_IGNORE );
		hit_mpiTestError(ok,"Failed receive");
}

/* Hit START COMMITED RECEIVE COMMUNICATION */
void hit_comStartRecv(HitCom *issue) {
#ifdef DEBUG
		printf("CTRL %d - Receives (async) from %d to %d\n", hit_Rank, issue->recvFrom, issue->myself);
#endif
		if ( issue->recvFrom == MPI_PROC_NULL ) return;
		int ok = MPI_Irecv( issue->dataRecv, 1, issue->typeRecv, issue->recvFrom, issue->tag, issue->comm, &(issue->requestRecv) );
		hit_mpiTestError(ok,"Failed start receive");
}

/* Hit END COMMITED RECV COMMUNICATION */
void hit_comEndRecv(HitCom *issue) {
#ifdef DEBUG
		printf("CTRL %d - End recv from %d to %d\n", hit_Rank, issue->recvFrom, issue->myself);
#endif

		if ( issue->sendTo == MPI_PROC_NULL ) return;
		int ok = MPI_Wait( &(issue->requestRecv), MPI_STATUS_IGNORE );
		hit_mpiTestError(ok,"Failed end recv");
}


/* Hit DO COMMITED SEND_RECV_REPLACE */
void hit_comDoSendRecvReplace(HitCom *issue) {

	/* 2. DO COMMUNICATION */
	int ok;
	ok = MPI_Sendrecv_replace(issue->dataSend, 1, issue->typeSend, issue->sendTo, issue->tag, issue->recvFrom ,issue->tag, issue->comm, MPI_STATUS_IGNORE);
	hit_mpiTestError(ok,"Failed send-receive-replace");
}


/* Hit DO COMMITED REDUCE */
void hit_comDoReduce(HitCom *issue) {

#ifdef DEBUG
	printf("Do reduce to %d(%d)\n",issue->sendTo,MPI_PROC_NULL);
#endif

	int ok;
	// mpiReduce is mpiAllreduce if sendTo is MPI_PROC_NULL
	if(issue->sendTo != MPI_PROC_NULL){
		ok = MPI_Reduce( issue->dataSend, issue->dataRecv, 1, issue->typeSend, issue->operation, issue->sendTo, issue->comm);
	} else {
		ok = MPI_Allreduce(issue->dataSend, issue->dataRecv, 1, issue->typeSend, issue->operation, issue->comm);
	}
	hit_mpiTestError(ok,"Failed reduction");
}

/* Hit DO COMMITED BROADCAST */
void hit_comDoBroadcast(HitCom *issue) {

#ifdef DEBUG
		printf("CTRL %d - Broadcast from %d\n", hit_Rank, issue->sendTo);
#endif

	int ok;
	ok = MPI_Bcast( issue->dataSend, 1, issue->typeSend, issue->sendTo, issue->comm);
	hit_mpiTestError(ok,"Failed broadcast");
}

/* Hit DO COMMITED ALLTOALL */
void hit_comDoAlltoall(HitCom *issue) {

	int ok;
	ok = MPI_Alltoall(issue->dataSend, issue->count, issue->typeSend, issue->dataRecv, issue->count, issue->typeSend, issue->comm);

	hit_mpiTestError(ok,"Failed all-to-all");
}


/* Hit DO COMMITED Alltoallv */
void hit_comDoAlltoallv(HitCom *issue) {

		int ok;
		ok = MPI_Alltoallv(issue->dataSend, issue->alltoallv->sendcnts, issue->alltoallv->sdispls,issue->typeSend, issue->dataRecv, issue->alltoallv->recvcnts,issue->alltoallv->rdispls, issue->typeSend, issue->comm);

		hit_mpiTestError(ok,"Failed all-to-all v");
}

/* Hit DO COMMITED ALLGATHER */
void hit_comDoAllGather(HitCom *issue) {

	int ok;
	ok = MPI_Allgather(issue->dataSend, issue->count, issue->typeSend, issue->dataRecv, issue->count, issue->typeSend, issue->comm);

	hit_mpiTestError(ok,"Failed all-gather");
}


/* Hit DO COMMITED SparseUpdate for CSR shapes */
void hit_comDoSparseUpdateCSR(HitCom *issue){


	int i;
	size_t baseExtent = issue->alltoallv->sparse->baseExtent;

	
	/* 1. Copy the local values to the send vector */
	for(i=0;i<issue->alltoallv->sparse->nsend;i++){
		int vertex = issue->alltoallv->sparse->send[i];
		int lvertex = hit_cShapeVertexToLocal(issue->alltoallv->sparse->shape,vertex);

		memcpy( (char*)issue->dataSend + (size_t) i * baseExtent,
				(char*)issue->alltoallv->sparse->originData + (size_t) lvertex * baseExtent ,
				(size_t)baseExtent);

		//debugall("Sending %d->: %d, local %d, value %f\n",issue->myself,vertex,lvertex,  *((double*)(issue->dataSend + i * baseExtent)));
	}


	/* 2. All to all communication. */
	int ok = MPI_Alltoallv(issue->dataSend, issue->alltoallv->sendcnts,
			issue->alltoallv->sdispls,issue->typeSend,
			issue->dataRecv, issue->alltoallv->recvcnts,issue->alltoallv->rdispls,
			issue->typeSend, issue->comm);
	
	hit_mpiTestError(ok,"Failed sparse update");

	/* 3. Extract the updated values from the recv vector. */
	for(i=0;i<issue->alltoallv->sparse->nrecv;i++){
		int vertex = issue->alltoallv->sparse->recv[i];
		
		int lvertex  = hit_cShapeVertexToLocal(issue->alltoallv->sparse->shape,vertex);

		memcpy( (char*)issue->alltoallv->sparse->originData + (size_t) lvertex * baseExtent,
				(char*)issue->dataRecv + (size_t) i * baseExtent,
				baseExtent);

		//debugall("Recving %d<-: %d, local %d, value %d\n",issue->myself,vertex,lvertex, *((int*)(issue->dataRecv + i * baseExtent)));
	}
}

/* Hit DO COMMITED SparseUpdate for Bitmap shapes */
void hit_comDoSparseUpdateBitmap(HitCom *issue){

	int i;
	size_t baseExtent = issue->alltoallv->sparse->baseExtent;

	
	/* 1. Copy the local values to the send vector */
	for(i=0;i<issue->alltoallv->sparse->nsend;i++){
		int vertex = issue->alltoallv->sparse->send[i];
		int lvertex = hit_bShapeVertexToLocal(issue->alltoallv->sparse->shape,vertex);
		
		memcpy( (char*)issue->dataSend + (size_t) i * baseExtent,
				(char*)issue->alltoallv->sparse->originData + (size_t) lvertex * baseExtent ,
				(size_t)baseExtent);

		//printf("Sending %d->: %d, local %d, value %d\n",issue->myself,vertex,lvertex,  *((int*)(issue->dataSend + i * baseExtent)));
	}


	/* 2. All to all communication. */
	int ok = MPI_Alltoallv(issue->dataSend, issue->alltoallv->sendcnts,
			issue->alltoallv->sdispls,issue->typeSend,
			issue->dataRecv, issue->alltoallv->recvcnts,issue->alltoallv->rdispls,
			issue->typeSend, issue->comm);
	
	hit_mpiTestError(ok,"Failed sparse update");

	/* 3. Extract the updated values from the recv vector. */
	for(i=0;i<issue->alltoallv->sparse->nrecv;i++){
		int vertex = issue->alltoallv->sparse->recv[i];
		
		int lvertex = hit_bShapeVertexToLocal(issue->alltoallv->sparse->shape,vertex);

		memcpy( (char*)issue->alltoallv->sparse->originData + (size_t) lvertex * baseExtent,
				(char*)issue->dataRecv + (size_t) i * baseExtent,
				baseExtent);

		//debugall("Recving %d<-: %d, local %d, value %d\n",issue->myself,vertex,lvertex, *((int*)(issue->dataRecv + i * baseExtent)));
	}
}

/* Hit DO COMMITED SparseUpdate */
void hit_comDoSparseUpdate(HitCom *issue){

	int type = hit_shapeType(issue->alltoallv->sparse->shape);

	switch(type){
		case HIT_CSR_SHAPE:
			hit_comDoSparseUpdateCSR(issue);
			break;
		case HIT_BITMAP_SHAPE:
			hit_comDoSparseUpdateBitmap(issue);
			break;
		default:
			hit_errInternal(__func__, "Unsupported shape type", "", __FILE__, __LINE__);
			break;
	}
}

void hit_comDoSparseScatter(HitCom *issue){

	int mpi_rank;
	MPI_Comm_rank(issue->comm,&mpi_rank);

	if(mpi_rank == 0){

		size_t baseExtent = issue->alltoallv->sparse->baseExtent;

		int nProcs;
		MPI_Comm_size(issue->comm,&nProcs);
		int nElems = issue->alltoallv->sparse->nsend;
		int * assignedGroups = issue->alltoallv->sparse->send;

		int * count_aux;
		// @arturo Ago 2015: New allocP interface
		// hit_calloc(count_aux,(size_t) nProcs, sizeof(int),int*);
		hit_calloc(count_aux, int, nProcs);
		
		int i;
		for(i=0;i<nElems;i++){

			int dst = assignedGroups[i];

			memcpy( (char*)issue->dataSend + (size_t) ( issue->alltoallv->sdispls[dst] + count_aux[dst]) * baseExtent,
					(char*)issue->alltoallv->sparse->originData + (size_t) i * baseExtent ,
					(size_t)baseExtent);


			count_aux[ dst ]++;
		}


		free(count_aux);

	} else {

		issue->dataSend = issue->alltoallv->sendcnts = issue->alltoallv->sdispls = NULL;

	}

	int ok = MPI_Scatterv( issue->dataSend,  issue->alltoallv->sendcnts, issue->alltoallv->sdispls,
	                        issue->typeSend ,  issue->dataRecv , issue->count,
	                        issue->typeRecv,
	                        0, issue->comm);
	hit_mpiTestError(ok,"Failed sparse scatter");

}



void hit_comDoSparseScatterRows(HitCom *issue){

	int mpi_rank;
	MPI_Comm_rank(issue->comm,&mpi_rank);

	if(mpi_rank == 0){

		size_t baseExtent = issue->alltoallv->sparse->baseExtent;

		int nProcs;
		MPI_Comm_size(issue->comm,&nProcs);
		int nRows = issue->alltoallv->sparse->nsend;
		int * assignedGroups = issue->alltoallv->sparse->send;

		int * count_aux;
		// @arturo Ago 2015: New allocP interface
		// hit_calloc(count_aux,(size_t) nProcs, sizeof(int),int*);
		hit_calloc(count_aux, int, nProcs);

		int i;
		for(i=0;i<nRows;i++){

			int dst = assignedGroups[i];
			int begin = issue->alltoallv->sparse->rows[i];
			int end = issue->alltoallv->sparse->rows[i+1];

			int j;
			for(j=begin; j<end; j++){

				memcpy( (char*)issue->dataSend + (size_t) ( issue->alltoallv->sdispls[dst] + count_aux[dst]) * baseExtent,
						(char*)issue->alltoallv->sparse->originData + (size_t) j * baseExtent ,
						(size_t)baseExtent);
				count_aux[ dst ]++;

			}


			//memcpy( (char*)issue->dataSend + (size_t) ( issue->alltoallv->sdispls[dst] + count_aux[dst]) * baseExtent,
			//		(char*)issue->alltoallv->sparse->originData + (size_t) i * baseExtent ,
			//		(size_t)baseExtent);


			//count_aux[ dst ]++;
		}


		free(count_aux);

	} else {

		issue->dataSend = issue->alltoallv->sendcnts = issue->alltoallv->sdispls = NULL;

	}

	int ok = MPI_Scatterv( issue->dataSend,  issue->alltoallv->sendcnts, issue->alltoallv->sdispls,
	                        issue->typeSend ,  issue->dataRecv , issue->count,
	                        issue->typeRecv,
	                        0, issue->comm);
	hit_mpiTestError(ok,"Failed sparse scatter");

}



void hit_comDoAllGatherv(HitCom *issue){

	int ok;
	ok = MPI_Allgatherv(issue->dataSend,issue->count,issue->typeSend,
			issue->dataRecv,
			issue->alltoallv->recvcnts,
			issue->alltoallv->rdispls,
			issue->typeRecv,issue->comm);
	hit_mpiTestError(ok,"Failed all_gatherv");
}






/* hit_comDo: executes a Hit Communication issue */
void hit_comDo(HitCom *issue) {
	/* 1. SKIP NULL COMM */
	if ( issue->commType == HIT_COMTYPE_NULL ) return;

	/* 2. CALL THE PROPER FUNCTION TO DO THE PATTERN */
	switch ( issue->commType) {
		case HIT_SENDRECV:
			/* a. START COUPLED ISEND */
			hit_comStartSend(issue);
			/* b. ACCEPT COUPLED RECEIVE */
			hit_comDoRecv(issue);
			/* c. WAIT FOR COUPLED ISEND TO COMPLETE */
			hit_comEndSend(issue);
			break;
		case HIT_REDUCE:
			hit_comDoReduce(issue);
			break;
		case HIT_ALLTOALL:
			hit_comDoAlltoall(issue);
			break;
		case HIT_ALLTOALLV:
			hit_comDoAlltoallv(issue);
			break;
		case HIT_BROADCAST:
			hit_comDoBroadcast(issue);
			break;
		case HIT_SENDRECV_REPLACE:
			hit_comDoSendRecvReplace(issue);
			break;
		case HIT_SPARSE_UPDATE:
			hit_comDoSparseUpdate(issue);
			break;
		case HIT_SPARSE_SCATTER:
			hit_comDoSparseScatter(issue);
			break;
		case HIT_SPARSE_SCATTER_ROWS:
			hit_comDoSparseScatterRows(issue);
			break;
		case HIT_ALLGATHERV:
			hit_comDoAllGatherv(issue);
			break;
		case HIT_ALLGATHER:
			hit_comDoAllGather(issue);
			break;
		default:
			hit_errInternal("comDo", "Unknown type of communication object", "", __FILE__, __LINE__);
			break;
	};
}

/**
 * hit_comOpSumDoubleBasic: basic sum double operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpSumDoubleBasic(void * in, void * inout) {
	
	double * din = (double *)in;
	double * dinout = (double *)inout;

	
#ifdef DEBUG
	printf("CTRL OpSumDouble: %lf+=%lf=%lf \n",*dinout,*din,*dinout+*din);
#endif
	
	*dinout+=*din;
}

/**
 * hit_comOpMaxDoubleBasic: basic maximum double operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpMaxDoubleBasic(void * in, void * inout) {

	double * din = (double *)in;
	double * dinout = (double *)inout;

#ifdef DEBUG
	printf("CTRL OpMaxDouble: hit_max(%lf,%lf)=%lf \n",*dinout,*din,hit_max(*dinout,*din));
#endif

	*dinout=hit_max(*dinout,*din);
}

/**
 * hit_comOpMinDoubleBasic: basic minimum double operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpMinDoubleBasic(void * in, void * inout) {

	double * din = (double *)in;
	double * dinout = (double *)inout;

#ifdef DEBUG
	printf("CTRL OpMinDouble: hit_min(%lf,%lf)=%lf \n",*dinout,*din,hit_min(*dinout,*din));
#endif

	*dinout=hit_min(*dinout,*din);
}

/**
 * hit_comOpSumFloatBasic: basic sum double operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpSumFloatBasic(void * in, void * inout) {
	
	float * din = (float *)in;
	float * dinout = (float *)inout;

	
#ifdef DEBUG
	printf("CTRL OpSumFloat: %f+=%f=%f \n",*dinout,*din,*dinout+*din);
#endif
	
	*dinout+=*din;
}

/**
 * hit_comOpMaxFloatBasic: basic maximum double operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpMaxFloatBasic(void * in, void * inout) {

	float * din = (float *)in;
	float * dinout = (float *)inout;

#ifdef DEBUG
	printf("CTRL OpMaxFloat: hit_max(%f,%f)=%f \n",*dinout,*din,hit_max(*dinout,*din));
#endif

	*dinout=hit_max(*dinout,*din);
}

/**
 * hit_comOpMinFloatBasic: basic minimum double operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpMinFloatBasic(void * in, void * inout) {

	float * din = (float *)in;
	float * dinout = (float *)inout;

#ifdef DEBUG
	printf("CTRL OpMinFloat: hit_min(%f,%f)=%f \n",*dinout,*din,hit_min(*dinout,*din));
#endif

	*dinout=hit_min(*dinout,*din);
}

/**
 * hit_comOpSumIntBasic: basic sum integer operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpSumIntBasic(void * in, void * inout) {

	int * din = (int *)in;
	int * dinout = (int *)inout;

	*dinout+=*din;
}


/**
 * hit_comOpMaxIntBasic: basic maximum int operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpMaxIntBasic(void * in, void * inout) {

	int * din = (int *)in;
	int * dinout = (int *)inout;

	*dinout=hit_max(*dinout,*din);
}



/**
 * hit_comOpMinIntBasic: basic minimum int operation.
 * @param in Pointer to the first operator.
 * @param inout Pointer to the second and return operator.
 */
void hit_comOpMinIntBasic(void * in, void * inout) {

	int * din = (int *)in;
	int * dinout = (int *)inout;

	*dinout=hit_min(*dinout,*din);
}



/* generic function to make operations between MPI user datatypes */
void hit_comOpGenericAnyType( void * in, void * inout, MPI_Datatype datatype, int offset, size_t tam, HitComOpFunction f )  
{ 
    int *array_of_ints; 
    MPI_Aint *array_of_adds; 
    MPI_Datatype *array_of_dtypes; 
    int num_ints, num_adds, num_dtypes, combiner; 
    int i,j,stride,jump,ac,ac2;
    MPI_Aint ext, lb;
	
    MPI_Type_get_envelope( datatype,  
						  &num_ints, &num_adds, &num_dtypes, &combiner ); 
	
    switch (combiner) { 
		case MPI_COMBINER_NAMED: // Native MPI type
			stride=offset/(int)tam;
			f( (char *)in+offset, (char *)inout+offset);
			break; 
		case MPI_COMBINER_STRUCT: //N+1-N-N N is number of datatypes, stored in ints[0]
			
			hit_malloc(array_of_ints, int, num_ints);
			hit_malloc(array_of_adds, MPI_Aint, num_adds);
			hit_malloc(array_of_dtypes, MPI_Datatype, num_dtypes);
			// @arturo Ago 2015: New allocP interface
			/* 
			hit_malloc(array_of_ints,(size_t) num_ints * sizeof(int),int*);
			hit_malloc(array_of_adds,(size_t) num_adds * sizeof(MPI_Aint),MPI_Aint*);
			hit_malloc(array_of_dtypes,(size_t) num_dtypes * sizeof(MPI_Datatype),MPI_Datatype*);
			*/
			

			MPI_Type_get_contents( datatype, num_ints, num_adds, num_dtypes, 
								  array_of_ints, array_of_adds, array_of_dtypes ); 
			//our blocklengths are always one: internal loop not tested
			for (i=0; i<array_of_ints[0]; i++) { 
				ac= (int) array_of_adds[i] + offset;
				MPI_Type_get_extent(array_of_dtypes[i], &lb, &ext);
				jump = (int) ext;
				for(j=0;j<array_of_ints[i+1];j++) {
					hit_comOpGenericAnyType( in, inout, array_of_dtypes[i],ac,tam,f);
					ac+=jump;
				}
			} 
			free( array_of_ints ); 
			free( array_of_adds ); 
			free( array_of_dtypes ); 
			break; 
		case MPI_COMBINER_HVECTOR:  //2-1-1   count in ints[0] & blocklength in ints[1] & stride in adds[0] & oldtype in dtypes[0]
			
			
			hit_malloc(array_of_ints, int, num_ints);
			hit_malloc(array_of_adds, MPI_Aint, num_adds);
			hit_malloc(array_of_dtypes, MPI_Datatype, num_dtypes);
			// @arturo Ago 2015: New allocP interface
			/* 
			hit_malloc(array_of_ints,(size_t) num_ints * sizeof(int), int* );
			hit_malloc(array_of_adds,(size_t) num_adds * sizeof(MPI_Aint), MPI_Aint*);
			hit_malloc(array_of_dtypes,(size_t) num_dtypes * sizeof(MPI_Datatype),MPI_Datatype*);
			*/
			
		
			MPI_Type_get_contents( datatype, num_ints, num_adds, num_dtypes, 
								  array_of_ints, array_of_adds, array_of_dtypes ); 
			//our blocklengths are always one: internal loop not tested
			MPI_Type_get_extent(array_of_dtypes[0], &lb, &ext);
			jump = (int) ext;
			// @javfres ac2 does nothing
			//ac=offset, ac2;
			ac=offset;
			stride = (int) array_of_adds[0];
			for(i=0;i<array_of_ints[0];i++) {
				ac2=ac;
				for(j=0;j<array_of_ints[1];j++) {
					hit_comOpGenericAnyType( in, inout, array_of_dtypes[0],ac2, tam,f );
					ac2+=jump;
				}
				ac+=stride;
			}
			free( array_of_ints ); 
			free( array_of_adds ); 
			free( array_of_dtypes ); 
			break; 
		case MPI_COMBINER_CONTIGUOUS:  //1-0-1   count in ints[0] & oldtype in dtypes[0]
			
			hit_malloc(array_of_ints, int, num_ints);
			hit_malloc(array_of_adds, MPI_Aint, num_adds);
			hit_malloc(array_of_dtypes, MPI_Datatype, num_dtypes);
			// @arturo Ago 2015: New allocP interface
			/* 
			hit_malloc(array_of_ints,(size_t) num_ints * sizeof(int) ,int* );
			hit_malloc(array_of_adds,(size_t) num_adds * sizeof(MPI_Aint),MPI_Aint*);
			hit_malloc(array_of_dtypes,(size_t) num_dtypes * sizeof(MPI_Datatype),MPI_Datatype*);
			*/
			

			MPI_Type_get_contents( datatype, num_ints, num_adds, num_dtypes, 
								  array_of_ints, array_of_adds, array_of_dtypes );
			MPI_Type_get_extent(array_of_dtypes[0], &lb, &ext);
			jump = (int) ext;
			ac=offset;
			for(i=0;i<array_of_ints[0];i++) {
				hit_comOpGenericAnyType( in, inout, array_of_dtypes[0],ac,tam,f );
				ac+=jump;
			}
			free( array_of_ints ); 
			free( array_of_adds ); 
			free( array_of_dtypes ); 
			break;
    } 
} 


/* Hit OPERATION to add doubles in reductions */
//@see decoding a datatype in mpi_forum.org
/* Hit OPERATION to calculate the sum in reductions */
void hit_comOpSumDouble (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(double),&hit_comOpSumDoubleBasic);
}

/* Hit OPERATION to calculate the maximum in reductions */
void hit_comOpMaxDouble (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(double),&hit_comOpMaxDoubleBasic);
}

/* Hit OPERATION to calculate the minimum in reductions */
void hit_comOpMinDouble (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(double),&hit_comOpMinDoubleBasic);
}

/* Hit OPERATION to calculate the sum in reductions */
void hit_comOpSumFloat (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(float),&hit_comOpSumFloatBasic);
}

/* Hit OPERATION to calculate the maximum in reductions */
void hit_comOpMaxFloat (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(float),&hit_comOpMaxFloatBasic);
}

/* Hit OPERATION to calculate the minimum in reductions */
void hit_comOpMinFloat (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(float),&hit_comOpMinFloatBasic);
}


void hit_comOpSumInt (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(int),&hit_comOpSumIntBasic);
}
void hit_comOpMaxInt (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(int),&hit_comOpMaxIntBasic);
}
void hit_comOpMinInt (void * in, void * inout, int *len, HitType * type) {
	HIT_NOT_USED(len);
	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(int),&hit_comOpMinIntBasic);
}



