/**
 * MPI Communications management module.
 * It allow to create point to point or collective communications
 * on active processors on a Hitmap Topology or Layout.
 * The communications are encapsulated in a HitCom object.
 *
 * @file hit_com.h
 * @version 1.5
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Mar 2013
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

#ifndef _HitCom_
#define _HitCom_

#include <stdio.h>

#include "hit_mpi.h"
#include "hit_error.h"
#include "hit_tile.h"
#include "hit_layout.h"
#include "hit_allocP.h"

#ifdef __cplusplus
	extern "C" {
#endif
/* Hit MPI ERROR TEST */
/** This macro checks if the MPI operation was successful. */
#define	hit_mpiTestError(ok,cad)	\
    if ( ok != MPI_SUCCESS ) {		\
        fprintf(stderr,"Hit MPI RunTime-Error, Rank %d: %s - %d\n", hit_Rank, cad, ok);	fflush(stderr); \
        exit(-1);					\
    }


/** @name Hitmap Base types */
/** @{ */
/* Hit MPI BASE TYPES EQUIVALENCE */
/** Typedef for the HitType. */
typedef MPI_Datatype HitType;
/**
 * Constant for char basic type. 
 * @hideinitializer
 */
#define	HIT_CHAR		MPI_CHAR
/**
 * Constant for integer basic type. 
 * @hideinitializer
 */
#define	HIT_INT			MPI_INT
/**
 * Constant for single precision real basic type. 
 * @hideinitializer
 */
#define	HIT_FLOAT		MPI_FLOAT
/**
 * Constant for double precision real basic type. 
 * @hideinitializer
 */
#define	HIT_DOUBLE		MPI_DOUBLE
/**
 * Constant for boolean basic type. 
 * @hideinitializer
 */
#define	HIT_BOOL		MPI_BOOL
/** Typedef for addresses. */
typedef MPI_Aint HitAint;
/** @} */


/** Extra HitTypes committed in initialization. */
extern HitType	HIT_SHAPE_SIG;

/** @name HitCom Types */
/** @{ */
/* HitCOM TYPES (They are private) */
/** Constant for Null communication.
 * @hideinitializer
 */
#define HIT_COMTYPE_NULL		0
/** Constant for Send and/or Receive communication.
 * @hideinitializer
 */
#define HIT_SENDRECV			3
/** Constant for Reduce communication.
 * @hideinitializer
 */
#define HIT_REDUCE				4
/**
 * Constant for All Reduce communication.
 * @hideinitializer
 */
#define HIT_ALLREDUCE			5
/**
 * Constant for All to all communication.
 * @hideinitializer
 */
#define HIT_ALLTOALL			6
/**
 * Constant for All to all v communication.
 * @hideinitializer
 */
#define HIT_ALLTOALLV			7
/**
 * Constant for Broadcast communication.
 * @hideinitializer
 */
#define HIT_BROADCAST			8
/**
 * Constant for Send and/or Receive Replace communication.
 * @hideinitializer
 */
#define HIT_SENDRECV_REPLACE	9

/**
 * Constant for Sparse Update communication.
 * @hideinitializer
 */
#define HIT_SPARSE_UPDATE		10
/**
 * Constant for Sparse Broadcast.
 * @hideinitializer
 */
#define HIT_SPARSE_SCATTER	11
/**
 * Constant for All Distribute.
 * @hideinitializer
 */
#define HIT_ALLDISTRIBUTE	12
/**
 * Constant for Sparse Scatter Rows.
 * @hideinitializer
 */
#define HIT_SPARSE_SCATTER_ROWS	13
/**
 * Constant for All GatherV.
 * @hideinitializer
 */
#define HIT_ALLGATHERV 14
/**
 * Constant for All Gather.
 * @hideinitializer
 */
#define HIT_ALLGATHER 15
/** @} */


/* HitCOM OPERATION TYPES */
/** Hit Communication type */
typedef MPI_Op HitOp;
/** Hit Reduce function */
typedef MPI_User_function HitReduceFunction;

/* Hit PREDEFINED REDUCTION OPERATORS ON Tile TYPES */
/** @name HitOp predefined reduction operators on Tile types */
/**@{ */
/** Operator for integer sum reduction operation */
extern HitOp	HIT_OP_SUM_INT;
/** Operator for integer min reduction operation */
extern HitOp	HIT_OP_MIN_INT;
/** Operator for integer max reduction operation */
extern HitOp	HIT_OP_MAX_INT;
/** Operator for float sum reduction operation */
extern HitOp	HIT_OP_SUM_FLOAT;
/** Operator for float min reduction operation */
extern HitOp	HIT_OP_MIN_FLOAT;
/** Operator for float max reduction operation */
extern HitOp	HIT_OP_MAX_FLOAT;
/** Operator for double sum reduction operation */
extern HitOp	HIT_OP_SUM_DOUBLE;
/** Operator for float min reduction operation */
extern HitOp	HIT_OP_MIN_DOUBLE;
/** Operator for float max reduction operation */
extern HitOp	HIT_OP_MAX_DOUBLE;
/**@} */


/**
 * Constant for communications of any tag
 * @hideinitializer
 */
#define HIT_ANY_TAG 0

/** 
 * Constant for selecting my own rank in dimensional collective communications
 * @hideinitializer
 */
#define HIT_COM_MYSELF	-100


/* Hit BASIC RANKS AND COMMUNICATOR GLOBAL DEFINITIONS */
/**
 * Linear rank for the current processor
 * @hideinitializer
 */
#define	hit_Rank		(HIT_TOPOLOGY_INFO->selfRank)
/**
 * Number of total processors
 * @hideinitializer
 */
#define	hit_NProcs		(HIT_TOPOLOGY_INFO->numProcs)
/**
 * Low level communicator
 * @hideinitializer
 */
//#define	hit_Comm		((MPI_Comm *) HIT_TOPOLOGY_INFO.lowLevel)
#define	hit_Comm		(HIT_TOPOLOGY_INFO->comm)


/* Hit MPI INITIALIZATION FUNCTION */
/**
 * hit_comInit: initializes the communication environment
 * @param pargc pointer to the counter arguments of the main routine
 * @param pargv pointer to the arguments of the main routine
 */
void hit_comInit(int *pargc, char **pargv[]);

/* Hit MPI FINALIZATION FUNCTION */
/**
 * Finalize the communication environment freeing all internal resources.
 */
void hit_comFinalize();


/* Hit COM TYPES: CONSTRUCTOR */
/**
 * Create a MPI derived datatype.
 * @param varP Pointer to a HitTile.
 * @param baseType HitTile base type.
 * @return HitType The derived datatype.
 */
HitType hit_comType(const void *varP, HitType baseType);

/**
 * Recursive creation of MPI derived datatypes for hierarchical tiles.
 * @param varP Pointer to a HitTile.
 * @param baseType HitTile base type.
 * @return HitType The derived datatype.
 */
HitType hit_comTypeRec(const void *varP, HitType baseType);

/**
 * Null datatype.
 * @hideinitializer
 */
#define HIT_TYPE_NULL	MPI_DATATYPE_NULL

/* Hit COM TYPES: FREE */
/**
 * Free a derived HitType.
 * 
 * @hideinitializer
 * 
 * @param type The HitType to free.
 */
#define hit_comFreeType(type) 	if (type!=HIT_TYPE_NULL) MPI_Type_free( &type );



/**
 * Creates a Hit datatype from a general set of datatypes, and block sizes.
 * 
 * @hideinitializer
 * 
 * @param new_type Pointer to a HitType.
 * @param Nstruct Native struct name (Use typedef to declare it previously).
 * @param n Number of fields in the datatype.
 * @param ... List with the name, number of elements, and type of each field.
 * @code
 * // Code example that creates a type with three doubles and a integer.
 * typedef struct {
 *  double dname[3];
 *  int iname;
 * } Cnode;
 * HitType HitNode;
 * hit_comTypeStruct(&HitNode,Cnode,2, dname,3,HIT_DOUBLE, iname,1,HIT_INT);
 * @endcode
 */
#define hit_comTypeStruct(new_type,Nstruct,n, ...) hit_comTypeStruct##n(new_type,Nstruct,__VA_ARGS__)
#include "hit_comTypeStruct.h"



/* Hit COMMUNICATION STRUCTURE */
/*
struct HitCom;
struct HitComAlltoallv;
struct HitComSparse;
*/

/**
 * HitCom: Structure for communications.
 * It can represent a send-receive pair communication
 * or a collective communication
 */
typedef struct {
	int				commType;		/**< Communication type. */
	int				myself;			/**< Processor rank. */
	int				sendTo;			/**< Send destination. */
	int				recvFrom;		/**< Recv origin. */
	int				tag;			/**< MPI communication tag. */
	HitType			typeSend;		/**< MPI send type. */
	HitType			typeRecv;		/**< MPI recv type */
	void 			*dataSend;		/**< Pointer to the data to send. */
	void 			*dataRecv;		/**< Pointer to the data to recv. */
	MPI_Comm		comm;			/**< MPI Communicator. */
	MPI_Request		requestSend;	/**< MPI request send struct. */
	MPI_Request		requestRecv;	/**< MPI request recv struct. */
	MPI_Status		statusSend;		/**< MPI status send struct. */
	MPI_Status		statusRecv;		/**< MPI status recv struct. */
	HitOp			operation;		/**< MPI operation. */

	int				count;			/**< Count for all to all. */
	struct HitComAlltoallv * alltoallv; /**< Pointer to the struct to extend. */

} HitCom;


/**
 * This struct extends the HitCom struct to allow alltoallv communications.
 * @implements HitCom
 */
typedef struct HitComAlltoallv {

	int	*sendcnts;	/**< Send counts for all to all v. */
	int	*sdispls;	/**< Send displacements for all to all v.  */
	int	*recvcnts;	/**< Recv counts for all to all v. */
	int	*rdispls;	/**< Recv displacements for all to all v. */

	struct HitComSparse * sparse;	/**< Pointer to the struct to extend. */

} HitComAlltoallv;

/**
 * This struct extends the HitComAlltoallv struct to allow sparse updates.
 * @implements HitComAlltoallv
 */
typedef struct HitComSparse {

	HitShape shape;		/**< Shape of the graph to update. */
	void * originData;	/**< Pointer of the array with the node values. */

	int * recv;	/**< Vertex to receive. */
	int * send;	/**< Vertex to send. */
	int nsend;	/**< Number of vertices to send. */
	int nrecv;	/**< Number of vertices to send. */

	size_t baseExtent; /**< Base size of the tile type. */

	int * rows; /**<rows for scatter rows */

} HitComSparse;



/**
 * Hit COM: NULL VALUE
 * @hideinitializer
 */
extern HitCom	HIT_COM_NULL;


/**
 * Hit COM: Static null value
 * @hideinitializer
 */
/* @arturo: A change in the internal representation of Request/Status object in 
 * 				MPICH version 3.0.2. It come back to the previous representation in
 * 				later versions
 */
/* @javfres: Struct initialization following C99 syntax */
/*
#define HIT_COM_NULL_STATIC	{ HIT_COMTYPE_NULL, MPI_PROC_NULL, MPI_PROC_NULL, MPI_PROC_NULL, \
								0, HIT_TYPE_NULL, HIT_TYPE_NULL, NULL, NULL, MPI_COMM_NULL, \
								MPI_REQUEST_NULL, MPI_REQUEST_NULL, \
								{ 0,0,0,0,0,{0,0} }, { 0,0,0,0,0,{0,0} }, \
								0, 0, NULL }
								*/
/*
#define HIT_COM_NULL_STATIC	{ HIT_COMTYPE_NULL, MPI_PROC_NULL, MPI_PROC_NULL, MPI_PROC_NULL, \
								0, HIT_TYPE_NULL, HIT_TYPE_NULL, NULL, NULL, MPI_COMM_NULL, \
								MPI_REQUEST_NULL, MPI_REQUEST_NULL, { 0,0,0,0,0 }, \
								{ 0,0,0,0,0 },0,0,NULL}
*/
#define HIT_COM_NULL_STATIC	{ \
	.commType = HIT_COMTYPE_NULL, \
	.myself=MPI_PROC_NULL, \
	.sendTo=MPI_PROC_NULL, \
	.recvFrom=MPI_PROC_NULL, \
	.tag=0, \
	.typeSend=HIT_TYPE_NULL, \
	.typeRecv=HIT_TYPE_NULL, \
	.dataSend=NULL, \
	.dataRecv=NULL, \
	.comm=MPI_COMM_NULL, \
	.requestSend=MPI_REQUEST_NULL, \
	.requestRecv=MPI_REQUEST_NULL, \
	.operation=0, \
	.count=0, \
	.alltoallv=NULL}



/** Hit COM alltoallv: NULL VALUE for HitComAlltoallv. */
extern HitComAlltoallv HIT_COM_ALLTOALLV_NULL;

/** Hit COM alltoallv: NULL STATIC VALUE for HitComAlltoallv. @hideinitializer */
#define HIT_COM_ALLTOALLV_NULL_STATIC	{NULL,NULL,NULL,NULL,NULL}

/** Hit COM sparse: NULL VALUE for HitComSparse. */
extern HitComSparse HIT_COM_SPARSE_NULL;
/** Hit COM sparse: NULL STATIC VALUE for HitComSparse. @hideinitializer */
#define HIT_COM_SPARSE_NULL_STATIC	{HIT_SHAPE_NULL_STATIC, NULL, NULL, NULL, 0, 0, 0, NULL}


/* Hit COM: COMMIT COMMUNICATIONS IN SEVERAL FORMS */
/* GENERIC DOUBLE COMMUNICATION COMMIT */

/** @name Constants for specifying if a selection is made with array or tile coordinates */
/** @{ */
/* Constants for specifying if the selection is made with array or tile coordinates */
/**
 * Constant for specifying that the selection is made with tile coordinates.
 * @hideinitializer
 */
#define HIT_COM_TILECOORDS 0x43

/**
 * Constant for specifying that the selection is made with array coordinates.
 * @hideinitializer
*/
#define HIT_COM_ARRAYCOORDS 0x44
/** @} */

/** @name Send-Receive Communication */
/** @{ */
/**
 * Creates a communication for a send-receive pair communication.
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tilePSend Pointer to the HitTile to send
 * @param receiveFrom HitRanks coordinates of the process from which to receive
 * @param tilePRecv Pointer to the HitTile to receive
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
#define hit_comSendRecv(lay, sendTo, tilePSend, receiveFrom, tilePRecv, baseType) hit_comSendRecvSelect(lay, sendTo, tilePSend, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, receiveFrom, tilePRecv, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType)

/**
 * Creates a communication with tag for a send-receive pair communication.
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tilePSend Pointer to the HitTile to send
 * @param receiveFrom HitRanks coordinates of the process from which to receive
 * @param tilePRecv Pointer to the HitTile to receive
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag Tag of the communication
 * @return \e HitCom communication structure
 */
#define hit_comSendRecvTag(lay, sendTo, tilePSend, receiveFrom, tilePRecv, baseType, tag) hit_comSendRecvSelectTag(lay, sendTo, tilePSend, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, receiveFrom, tilePRecv, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, tag)

/**
 * Creates a communication with tag for a send-receive pair communication.
 * In this function a selection is applied to each tile to obtain the elements to communicate.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param modeSelectSend Constant indicating if the send tile is selected in tile or array coordinates
 * @param receiveFrom HitRanks coordinates of the process from which to receive
 * @param tilePRecv Pointer to the HitTile to receive
 * @param selectionRecv HitShape to applied to the receive tile
 * @param modeSelectRecv Constant indicating if the send tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
#define hit_comSendRecvSelect(lay, sendTo, tilePSend, selectionSend, modeSelectSend, receiveFrom, tilePRecv, selectionRecv, modeSelectRecv, baseType) hit_comSendRecvSelectTag(lay, sendTo, tilePSend, selectionSend, modeSelectSend, receiveFrom, tilePRecv, selectionRecv, modeSelectRecv, baseType, HIT_ANY_TAG)

/**
 * Creates a communication with tag for a send-receive pair communication.
 * In this function a selection is applied to each tile to obtain the elements to communicate.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param modeSelectSend Constant indicating if the send tile is selected in tile or array coordinates
 * @param receiveFrom HitRanks coordinates of the process from which to receive
 * @param tilePRecv Pointer to the HitTile to receive
 * @param selectionRecv HitShape to applied to the receive tile
 * @param modeSelectRecv Constant indicating if the receive tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag Tag of the communication
 * @return \e HitCom communication structure
 */
HitCom hit_comSendRecvSelectTag(HitLayout lay, HitRanks sendTo, const void *tilePSend, HitShape selectionSend, int modeSelectSend, HitRanks receiveFrom, const void *tilePRecv, HitShape selectionRecv, int modeSelectRecv, HitType baseType, int tag);
/** @} */

/** @name Explicit Send and Receive Communications */
/** @{ */

/* EXPLICIT SEND, RECV */
/**
 * Creates a communications with tag for an only-send communication.
 * In this function a selection is applied to the tile to obtain the elements to send.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to send
 * @param selection HitShape to applied to the send tile
 * @param modeSelect Constant indicating if the send tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag Tag of the communication
 * @return \e HitCom communication structure
 */
#define hit_comSendSelectTag(lay, sendTo, tileP, selection, modeSelect, baseType, tag) hit_comSendRecvSelectTag(lay, sendTo, tileP, selection, modeSelect, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, tag)

/**
 * Creates a communications with tag for an only-send communication.
 * In this function all the elements of the tile are sent.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to send
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag Tag of the communication
 * @return \e HitCom communication structure
 */
#define hit_comSendTag(lay, sendTo, tileP, baseType, tag) hit_comSendRecvSelectTag(lay, sendTo, tileP, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, tag)

/**
 * Creates a communications for an only-send communication.
 * In this function a selection is applied to the tile to obtain the elements to send.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to send
 * @param selection HitShape to applied to the send tile
 * @param modeSelect Constant indicating if the send tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
#define hit_comSendSelect(lay, sendTo, tileP, selection, modeSelect, baseType) hit_comSendRecvSelect(lay, sendTo, tileP, selection, modeSelect, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType)

/**
 * Creates a communications for an only-send communication.
 * In this function all the elements of the tile are sent.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will send
 * @param tileP Pointer to the HitTile to send
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
#define hit_comSend(lay, sendTo, tileP, baseType) hit_comSendRecvSelect(lay, sendTo, tileP, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType)

/**
 * hit_comRecvSelectTag: creates a communications with tag for an only-receive communication.
 * 
 * In this function a selection is applied to the tile to obtain the elements to receive.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param receiveFrom HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to receive
 * @param selection HitShape to applied to the receive tile
 * @param modeSelect Constant indicating if the receive tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag Tag of the communication
 * @return \e HitCom communication structure
 */
#define hit_comRecvSelectTag(lay, receiveFrom, tileP, selection, modeSelect, baseType, tag) hit_comSendRecvSelectTag(lay, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, receiveFrom, tileP, selection, modeSelect, baseType, tag)

/**
 * hit_comRecvTag: creates a communications with tag for an only-receive communication.
 * 
 * In this function all the elements of the tile are received.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param receiveFrom HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to receive
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag Tag of the communication
 * @return \e HitCom communication structure
 */
#define hit_comRecvTag(lay, receiveFrom, tileP, baseType, tag) hit_comSendRecvSelect(lay, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, receiveFrom, tileP, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, tag)

/**
 * hit_comRecvSelect: creates a communications for an only-receive communication.
 * 
 * In this function a selection is applied to the tile to obtain the elements to receive.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param receiveFrom HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to receive
 * @param selection HitShape to applied to the receive tile
 * @param modeSelect Constant indicating if the receive tile is selected in tile or array coordinates * 
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
#define hit_comRecvSelect(lay, receiveFrom, tileP, selection, modeSelect, baseType) hit_comSendRecvSelect(lay, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, receiveFrom, tileP, selection, modeSelect, baseType)

/**
 * hit_comRecvTag: creates a communications for an only-receive communication.
 * In this function all the elements of the tile are received.
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param receiveFrom HitRanks coordinates of the process that will receive
 * @param tileP Pointer to the HitTile to receive
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
#define hit_comRecv(lay, receiveFrom, tileP, baseType) hit_comSendRecvSelect(lay, HIT_RANKS_NULL, &HIT_TILE_NULL, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, receiveFrom, tileP, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType)
/** @} */

/** @name Update Communication */
/** @{ */
/**
 * Updates the tile from where data is sent.
 * 
 * Shortcut to create a new communication when the new tile to communicate has the same type.
 * 
 * @hideinitializer
 * @warning Do not use this function unless you are sure that the new tile has the same data
 * structure as the one used to create the comm object.
 * 
 * @param comm communication
 * @param sendTile New tile to send
 */
#define hit_comUpdateSendTile(comm, sendTile) comm.dataSend=sendTile.data

/**
 * Updates the tile where data is received.
 * 
 * Shortcut to create a new communication when the new tile to communicate has the same type.
 * 
 * @hideinitializer
 * @warning Do not use this function unless you are sure that the new tile has the same data
 * structure as the one used to create the comm object.
 * 
 * @param comm communication
 * @param recvTile New tile to receive
 */
#define hit_comUpdateRecvTile(comm, recvTile) comm.dataRecv=recvTile.data

/**
 * Updates the ranks of the processor that sends.
 * 
 * @hideinitializer
 * @deprecated
 * 
 * @param comm Communication
 * @param topo Topology
 * @param SendTo Ranks of the new process that sends
 */
#define hit_comUpdateSendTo(comm, topo, SendTo) comm.sendTo=hit_topRankInternal(topo,SendTo)

/**
 * Updates the ranks of the processor that receives.
 * 
 * @hideinitializer
 * @deprecated
 * 
 * @param comm Communication
 * @param topo Topology
 * @param RecvFrom Ranks of the new process that receives
 */
#define hit_comUpdateRecvFrom(comm, topo, RecvFrom) comm.recvFrom=hit_topRankInternal(topo,RecvFrom)

/**
 * hit_comUpdateDimSendTo: updates the ranks of the processor that sends
 * in a dimension collective communication.
 * 
 * @hideinitializer
 * 
 * @param comm Communication
 * @param dim Dimension 
 * @param SendTo Ranks of the new process that sends
 */
//#define hit_comUpdateDimSendTo(comm, dim, SendTo) comm.sendTo=SendTo.rank[dim]
#define hit_comUpdateDimSendTo(comm, dim, SendTo) comm.sendTo=SendTo

/**
 * hit_comUpdateDimRecvFrom: updates the ranks of the processor that receives
 * in a dimension collective communication.
 * 
 * @hideinitializer
 * 
 * @param comm Communication
 * @param dim Dimension 
 * @param RecvFrom Ranks of the new process that receives
 */
//#define hit_comUpdateDimRecvFrom(comm, dim, RecvFrom) comm.recvFrom=RecvFrom.rank[dim]
#define hit_comUpdateDimRecvFrom(comm, dim, RecvFrom) comm.recvFrom=RecvFrom

/**
 * hit_comUpdateTag: updates the tag of a communication.
 * 
 * @hideinitializer
 * @deprecated
 * 
 * @param comm Communication
 * @param Tag New tag
 */
#define hit_comUpdateTag(comm, Tag) comm.tag=Tag


/**
 * hit_comUpdateOriginData: updates the origin data of a communication.
 * It's used to change the tile in a sparse update communication.
 * 
 * @param[in,out] com A pointer to the communication.
 * @param[in] tileP The new tile pointer with the origin data.
 */
void hit_comUpdateOriginData(HitCom * com, const void * tileP);
/** @} */

/**
 * Prepare resources to communicate with hit_com{Broadcast,Reduce}Dim.
 * 
 * It creates the low level communicator to allow collective dim communications for all dimensions.
 * 
 * @hideinitializer
 * 
 * @param lay The Layout
 */
#define hit_comAllowDims(lay) {\
	int hit_i;\
	for (hit_i=0;hit_i<hit_layNumDims(lay); hit_i++) hit_comAllowDim(&lay,hit_i); \
	}


 /**
  * Enables the collective dimension communications preparing
  * resources to communicate with hit_com{Broadcast,Reduce}Dim with a specific dimension.
  * 
  * @param lay Pointer to the layout.
  * @param dim Dimension to enable the dimension communications.
  *
  */
void hit_comAllowDim(HitLayout * lay, int dim);


/** @name Shift Communication */
/** @{ */
/* SHIFT DATA ACROSS PROCESSORS: EVERYONE SENDS AND RECEIVES */
/**
 * hit_comShiftDimSelect: creates a communication in which all processes from the same dimension of the layout send and receive a tile. It allows to select part of a tile.
 * 
 * The tile is sent to a neighbor in shift positions right and received (and overwritten) from the neighbor shift positions left.
 * 
 * In this function a selection is applied to the tile to obtain the elements to communicate.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param dim Dimension of the layout
 * @param shift	distance to the neighbors to communicate
 * @param tileP Pointer to the HitTile to send and receive
 * @param selection HitShape to applied to the tile 
 * @param modeSelect Constant indicating if the tile is selected in tile or array coordinates 
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag A tag to allow its use in asynchronous communications
 * @return \e HitCom communication structure
 */
#define hit_comShiftDimSelect(lay, dim, shift, tileP, selection, modeSelect, baseType, tag) hit_comSendRecvReplaceSelectTag(lay, hit_layNeighbor(lay,dim,shift), tileP, selection, modeSelect, hit_layNeighbor(lay,dim,-shift), baseType, tag)
/** @} */

/** @name Send-Receive Communication */
/**
 * hit_comSendRecvReplaceSelectTag: Sends and receives using a single buffer.
 * 
 * @param lay Layout.
 * @param sendTo HitRanks coordinates of the process that will send.
 * @param tileP Pointer to the HitTile to send and receive.
 * @param selection HitShape to applied to the tile.
 * @param modeSelect Constant indicating if the tile is selected in tile or array coordinates.
 * @param receiveFrom HitRanks coordinates of the process that will receive
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @param tag Tag of the communication.
 * @return \e HitCom communication structure.
 */
HitCom hit_comSendRecvReplaceSelectTag(HitLayout lay, HitRanks sendTo, const void *tileP, HitShape selection, int modeSelect, HitRanks receiveFrom, HitType baseType, int tag);


/** @name Shift Communication */
/** @{ */
/**
 * hit_comShiftDim: creates a communication in which all processes from the same dimension of the layout send and receive a tile.
 * 
 * The tile is sent to a neighbor in shift positions right and received (and overwritten) from the neighbor shift positions left.
 * 
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param dim Dimension of the layout.
 * @param shift	Distance to the neighbors to communicate.
 * @param tileP Pointer to the HitTile to send and receive.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure.
 */
#define hit_comShiftDim(lay, dim, shift, tileP, baseType) hit_comShiftDimSelect(lay, dim, shift, tileP, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, HIT_ANY_TAG)

/**
 * hit_comShiftDimAsync: creates a communication in which all processes from the same dimension of the layout send and receive a tile.
 * 
 * The tile is sent to a neighbor in shift positions right and received (and overwritten) from the neighbor shift positions left.
 * 
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param dim Dimension of the layout.
 * @param shift	Distance to the neighbors to communicate.
 * @param tileSend Pointer to the HitTile to send.
 * @param tileRecv Pointer to the HitTile to receive.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param tag A tag to identify an asynchronous communication.
 * @return \e HitCom communication structure.
 */
#define hit_comShiftDimAsync(lay, dim, shift, tileSend, tileRecv, baseType, tag) hit_comSendRecvTag(lay, hit_layNeighbor( lay, dim, shift ), tileSend, hit_layNeighbor( lay, dim, -shift ), tileRecv, baseType, tag)
/** @} */

/** @name Broadcast Communication */
/** @{ */
/**
 * hit_comBroadcast: creates a communication for a broadcast communication.
 * 
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param root HitRanks coordinates of the process that will send.
 * @param tile Pointer to the HitTile to send or receive.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @return \e HitCom communication structure.
 */
#define hit_comBroadcast(lay, root, tile, baseType) hit_comBroadcastSelect(lay, root, tile, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType)

/**
 * hit_comBroadcastSelect: creates a communication for a broadcast communication.
 * 
 * In this function a selection is applied to the tile to obtain the elements to communicate.
 * 
 * @param lay Layout.
 * @param root HitRanks coordinates of the process that will send.
 * @param tile Pointer to the HitTile to send or receive.
 * @param selection HitShape to applied to the tile.
 * @param modeSelect Constant indicating if the tile is selected in tile or array coordinates .
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @return \e HitCom communication structure.
 */
HitCom hit_comBroadcastSelect(HitLayout lay, HitRanks root, const void *tile, HitShape selection, int modeSelect, HitType baseType);

/**
 * hit_comBroadcastDim: creates a communication for a one-dimensional broadcast communication.
 * 
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param dim dimension where the broadcast will be applied. It should be lower than the dimensions of the layout.
 * @param root Rank coordinate in the selected dimension of the process that will send.
 * @param tile Pointer to the HitTile to send or receive.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @return \e HitCom communication structure.
 */
#define hit_comBroadcastDim(lay, dim, root, tile, baseType) hit_comBroadcastDimSelect(lay, dim, root, tile, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType)

/**
 * hit_comBroadcastDimSelect: creates a communication for a one-dimensional broadcast communication.
 * 
 * In this function a selection is applied to the tile to obtain the elements to communicate.
 * 
 * @param lay Layout.
 * @param dim dimension where the broadcast will be applied. It should be lower than the dimensions of the layout
 * @param root Rank coordinate in the selected dimension of the process that will send 
 * @param tile Pointer to the HitTile to send or receive
 * @param selection HitShape to applied to the tile
 * @param modeSelect Constant indicating if the tile is selected in tile or array coordinates 
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure
 */
HitCom hit_comBroadcastDimSelect(HitLayout lay, int dim, int root, const void *tile, HitShape selection, int modeSelect, HitType baseType);
/** @} */


// NOTE: If HitRanks is NULL -> AllReduce
/** @name Reduce Communication */
/** @{ */
/**
 * hit_comReduce: creates a communication for a reduce communication.
 * If root is null all processes get the result data.
 * 
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param root HitRanks coordinates of the process that will receive the final data
 * @param tilePSend Pointer to the HitTile to send
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSend
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param operation HitOp created to reduce the elements of the tile @see hit_comOpCreate 
 * @return \e HitCom communication structure
 */
#define hit_comReduce(lay, root, tilePSend, tilePRecv, baseType, operation) hit_comReduceSelect(lay, root, tilePSend, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, tilePRecv, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, operation)

/**
 * hit_comReduceSelect: creates a communication for a reduce communication.
 * If root is null all processes get the result data.
 * 
 * In this function a selection is applied to the tiles to obtain the elements to communicate.
 * 
 * @param lay Layout.
 * @param root HitRanks coordinates of the process that will receive the final data
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param modeSelectSend Constant indicating if the send tile is selected in tile or array coordinates 
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSend
 * @param selectionRecv HitShape to applied to the receive tile
 * @param modeSelectRecv Constant indicating if the receive tile is selected in tile or array coordinates 
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param operation HitOp created to reduce the elements of the tile @see hit_comOpCreate 
 * @return \e HitCom communication structure
 */
HitCom hit_comReduceSelect(HitLayout lay, HitRanks root, const void * tilePSend, HitShape selectionSend, int modeSelectSend, const void * tilePRecv, HitShape selectionRecv, int modeSelectRecv, HitType baseType, HitOp operation);

/**
 * hit_comReduceDim: creates a communication for a one-dimensional reduce communication.
 * If root is null all processes get the result data.
 * 
 * In this function all the elements of the tiles are communicated.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param dim Dimension where the broadcast will be applied. It should be lower than the dimensions of the layout
 * @param root HitRanks coordinates of the process that will receive the final data
 * @param tilePSend Pointer to the HitTile to send
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSendç
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param operation HitOp created to reduce the elements of the tile @see hit_comOpCreate 
 * @return \e HitCom communication structure
 */
#define hit_comReduceDim(lay, dim, root, tilePSend, tilePRecv, baseType, operation) hit_comReduceDimSelect(lay, dim, root, tilePSend, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, tilePRecv, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, operation)

/**
 * hit_comReduceDimSelect: creates a communication for a one-dimensional reduce communication.
 * If root is null all processes get the result data.
 * 
 * In this function a selection is applied to the tiles to obtain the elements to communicate.
 * 
 * @param lay Layout.
 * @param dim Dimension where the broadcast will be applied. It should be lower than the dimensions of the layout
 * @param root HitRanks coordinates of the process that will receive the final data
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param modeSelectSend Constant indicating if the send tile is selected in tile or array coordinates 
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSendç
 * @param selectionRecv HitShape to applied to the receive tile
 * @param modeSelectRecv Constant indicating if the receive tile is selected in tile or array coordinates 
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param operation HitOp created to reduce the elements of the tile @see hit_comOpCreate 
 * @return \e HitCom communication structure
 */
HitCom hit_comReduceDimSelect(HitLayout lay, int dim, HitRanks root, const void * tilePSend, HitShape selectionSend, int modeSelectSend, const void * tilePRecv, HitShape selectionRecv, int modeSelectRecv, HitType baseType, HitOp operation);
/** @} */

/** @name All-To-All Communication */
/** @{ */
/**
 * hit_comAlltoall: Perform an all-to-all communication.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param tilePSend Pointer to the HitTile to send
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSend
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param count Number of elements to send/receive.
 * @return \e HitCom communication structure.
 */
#define hit_comAlltoall(lay, tilePSend, tilePRecv, baseType, count) hit_comAlltoallSelect(lay, tilePSend, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, tilePRecv, HIT_SHAPE_WHOLE, HIT_COM_TILECOORDS, baseType, count)

/**
 * hit_comAlltoallSelect: Perform an all-to-all communication.
 * 
 * @param lay Layout.
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param modeSelectSend Constant indicating if the send tile is selected in tile or array coordinates
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSendç
 * @param selectionRecv HitShape to applied to the receive tile
 * @param modeSelectRecv Constant indicating if the receive tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param count Number of elements to send/receive.
 * @return \e HitCom communication structure.
 */
HitCom hit_comAlltoallSelect(HitLayout lay, const void * tilePSend, HitShape selectionSend, int modeSelectSend, const void * tilePRecv, HitShape selectionRecv, int modeSelectRecv, HitType baseType, int count);

/**
 * hit_comAlltoallv: Perform an all-to-allv communication.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSendç
 * @param selectionRecv HitShape to applied to the receive tile
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure.
 */
#define hit_comAlltoallv(lay, tilePSend, selectionSend, tilePRecv, selectionRecv, baseType) hit_comAlltoallSelectv(lay, tilePSend, selectionSend, HIT_COM_TILECOORDS, tilePRecv, selectionRecv, HIT_COM_TILECOORDS, baseType)

/**
 * hit_comAlltoallv: Perform a all-to-allv communication.
 * 
 * @param lay Layout.
 * @param tilePSend Pointer to the HitTile to send
 * @param selectionSend HitShape to applied to the send tile
 * @param modeSelectSend Constant indicating if the send tile is selected in tile or array coordinates
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSendç
 * @param selectionRecv HitShape to applied to the receive tile
 * @param modeSelectRecv Constant indicating if the receive tile is selected in tile or array coordinates
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @return \e HitCom communication structure.
 */
HitCom hit_comAlltoallSelectv(HitLayout lay, const void * tilePSend, HitShape * selectionSend, int modeSelectSend, const void * tilePRecv, HitShape * selectionRecv, int modeSelectRecv, HitType baseType);
/** @} */


/** @name All-Gather Communication */
/** @{ */
/**
 * hit_comAllGather: Perform an all-gather communication.
 * 
 * @hideinitializer
 * 
 * @param lay Layout.
 * @param tilePSend Pointer to the HitTile to send
 * @param tilePRecv Pointer to the HitTile to receive. It must be of the same type that tilePSend
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...)
 * @param count Number of elements to send.
 * @return \e HitCom communication structure.
 */
HitCom hit_comAllgather(HitLayout lay, const void *tilePSend, const void *tilePRecv, HitType baseType, int count);


/** @name Sparse Update Communication */
/** @{ */

/**
 * Performs a sparse update communication.
 * 
 * In a sparse update communication, each processor sends the
 * vertices uses by its neighbors and updates the values of its
 * own neighbor vertices. This communication uses an alltoallv mpi communication.
 * 
 * @param lay Layout.
 * @param tileP Pointer to the HitTile.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @return \e HitCom communication structure.
 */
HitCom hit_comSparseUpdate(HitLayout lay, const void * tileP, HitType baseType);

/**
 * Performs a sparse update communication using CSR sparse data format.
 * 
 * In a sparse update communication, each processor sends the
 * vertices uses by its neighbors and updates the values of its
 * own neighbor vertices. This communication uses an alltoallv mpi communication.
 * 
 * @param lay Layout.
 * @param tileP Pointer to the HitTile.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @return \e HitCom communication structure.
 */
HitCom hit_comSparseUpdateCSR(HitLayout lay, const void * tileP, HitType baseType);

/**
 * Performs a sparse update communication using Bitmap sparse data format.
 * 
 * In a sparse update communication, each processor sends the
 * vertices uses by its neighbors and updates the values of its
 * own neighbor vertices. This communication uses an alltoallv mpi communication.
 * @param lay Layout.
 * @param tileP Pointer to the HitTile.
 * @param baseType Basic type of the elements of the tile (HIT_INT, HIT_DOUBLE, HIT_FLOAT,...).
 * @return \e HitCom communication structure.
 */
HitCom hit_comSparseUpdateBitmap(HitLayout lay, const void * tileP, HitType baseType);

/**
 * This communication scatters the data in a tile among the processors according
 * with the partition of a layout.
 * 
 * @hideinitializer
 * 
 * @param lay The layout with the partition.
 * @param tilePSend The tile to send by the root processor.
 * @param tilePRecv The tile to receive by each processor.
 * @param baseType The Hitmap base type of the tile.
 * @return \e HitCom The new commucation object.
 */
#define hit_comSparseScatter(lay, tilePSend, tilePRecv, baseType) hit_comSparseScatterInternal(lay, tilePSend, tilePRecv, baseType, __FILE__, __LINE__)

/** @cond INTERNAL */
HitCom hit_comSparseScatterInternal(HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line);
/** @endcond */
/** @} */


/** @name Scatter and Gather Communications */
/** @{ */
/**
 * This communication scatters a sparse matrix by rows.
 * 
 * @hideinitializer
 * 
 * @param lay The layout with the partition.
 * @param tilePSend The tile with the data to send.
 * @param tilePRecv The tile to receive the data.
 * @param baseType The Hitmap base type of the tile.
 * @return \e HitCom The new commucation object.
 */
#define hit_comSparseScatterRows(lay, tilePSend, tilePRecv, baseType) hit_comSparseScatterRowsInternal(lay, tilePSend, tilePRecv, baseType, __FILE__, __LINE__)

/** @cond INTERNAL */
HitCom hit_comSparseScatterRowsInternal(HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line);
/** @endcond */
/** @} */

/** @name All-To-All Communication */
/**
 * This communication distributes different sets of data to the processors using
 * a plug_layContiguous. The processors have part of each set. When the communication
 * is done, each processors will have the data of its assigned sets.
 * 
 * @param lay The layout with the partition.
 * @param tilePSend The tile with the partial sets.
 * @param count The number of elements in each partial set.
 * @param tilePRecv The tile to reive the complete sets for each processors.
 * @param baseType The hitmap base type of the data. 
 * @note works only with plug_layContiguous
 * @return The new HitCom object.
 */
// @arturo: TEST, redistribute function that internally allocates and substitutes tile by new buffer
HitCom hit_comAllDistribute(HitLayout lay, const void * tilePSend, int * count, const void * tilePRecv, HitType baseType);


/** @name Scatter and Gather Communications */
/** @{ */
/**
 * Gathers data from all tasks and distribute the combined data to all tasks.
 * 
 * @hideinitializer
 * 
 * @param lay The layout
 * @param tilePSend The send tile.
 * @param tilePRecv The recv tile.
 * @param baseType Datatype.
 * @return \e HitCom communication structure.
 */
#define hit_comAllGatherv(lay, tilePSend, tilePRecv, baseType) hit_comAllGathervInternal(lay, tilePSend, tilePRecv, baseType, __FILE__, __LINE__)

/** @cond INTERNAL */
HitCom hit_comAllGathervInternal(HitLayout lay, const void * tilePSend, const void * tilePRecv, HitType baseType, const char * file, int line);
/** @endcond */
/** @} */


/* Hit COM: FREE COMMUNICATION ISSUE */
/**
 * hit_comFree: frees the resources of a communication structure
 * @param issue communication
 */
void hit_comFree(HitCom issue);
//#define hit_comFree( issue )	{ hit_comFreeType( (issue).typeSend ); hit_comFreeType( (issue).typeRecv ); }


/* Hit COM: DECLARE TAGS */
/**
 * Defines an enumerate type of tags.
 * These tags can be used in communications.
 * 
 * @hideinitializer
 * 
 * @param ... Set of tags.
 */
#define hit_comTagSet( ... )	enum { __VA_ARGS__ }

/** @name Activate/Wait for Communication*/
/** @{ */ 
/* Hit COM ACTIVATE/WAIT FOR COMMUNICATION */
/**
 * hit_comStartSend: start to send in a send-receive communication
 * @param issue pointer to the communication
 */
void hit_comStartSend(HitCom *issue);

/**
 * hit_comEndSend: end to send in a send-receive communication
 * @param issue pointer to the communication
 */
void hit_comEndSend(HitCom *issue);

/**
 * hit_comDoSend: send in a send-receive communication
 * @param issue pointer to the communication
 */
void hit_comDoSend(HitCom *issue);

/**
 * hit_comDoRecv: receive in a send-receive communication
 * @param issue pointer to the communication
 */
void hit_comDoRecv(HitCom *issue);

/**
 * hit_comStartRecv: start to receive in a send-receive communication
 * @param issue pointer to the communication
 */
void hit_comStartRecv(HitCom *issue);

/**
 * hit_comEndRecv: end to receive in a send-receive communication
 * @param issue pointer to the communication
 */
void hit_comEndRecv(HitCom *issue);

/**
 * hit_comDoSendRecvReplace: perform a Sendrecv_replace communication
 * @param issue pointer to the communication
 */
void hit_comDoSendRecvReplace(HitCom *issue);

/**
 * hit_comDoAlltoall: perform an all-to-all communication
 * @param issue pointer to the communication
 */
void hit_comDoAlltoall(HitCom *issue);

/**
 * hit_comDoAlltoallv: perform an all-to-all v communication
 * @param issue pointer to the communication
 */
void hit_comDoAlltoallv(HitCom *issue);

/**
 * hit_comDoReduce: perform a reduce communication
 * @param issue pointer to the communication
 */
void hit_comDoReduce(HitCom *issue);

/**
 * hit_comDoBroascast: perform a broadcast communication
 * @param issue pointer to the communication
 */
void hit_comDoBroadcast(HitCom *issue);

/**
 * hit_comDoSparseUpdate: perform a sparse update communication.
 * @param issue pointer to the communication.
 */
void hit_comDoSparseUpdate(HitCom *issue);

/**
 * hit_comDo: do a communication (send and receive) in a send-receive communication.
 * 
 * @hideinitializer
 * 
 * @param comm \e HitCom communication
 */
#define hit_comDoSendRecv( comm )	{	\
	hit_comStartSend( &(comm) );		\
	hit_comDoRecv( &(comm) );			\
	hit_comEndSend( &(comm) );			\
}


/* Hit MPI COMM DO A COMPLETE COMM. ISSUE */
/**
 * hit_comDo: do a communication.
 * It checks the communication type to select the apropiate function.
 * @param issue pointer to HitCom communication.
 */
void hit_comDo(HitCom *issue);

/* Hit MPI COMM DO A COMPLETE COMM. ISSUE, AND FREE THE COMM OBJECT */
/**
 * hit_comDoOnce: do a communication and free the comm object.
 * @hideinitializer
 * @param com A HitCom object or a constructor
 */
#define	hit_comDoOnce( com )	{ HitCom __HIT_COM__ = com; hit_comDo( & __HIT_COM__ ); hit_comFree( __HIT_COM__ ); }
/** @} */

#undef hit_error
/** hit_error: prints an error.
 * 
 * @hideinitializer
 * 
 * @param name Error message.
 * @param file Code file.
 * @param numLine Line number.
 */
#define hit_error(name,file,numLine)	\
	{							\
	fprintf(stderr,"Hit Programmer, RunTime-Error Rank(%d): %s, in %s[%d]\n", hit_Rank, name, file, numLine); \
	MPI_Finalize();				\
	exit( HIT_ERR_USER );		\
	}


/* Com Barrier, use the active communicator in layout */
/**
 * hit_comBarrier: barrier for all active processes in a layout.
 * 
 * @hideinitializer
 * 
 * @param lay A HitLayout.
 * @author arturo Mar 2013
 */
#define hit_comBarrier( lay ) MPI_Barrier( lay.pTopology[0]->comm )
//#define hit_comBarrier( lay ) MPI_Barrier( *(MPI_Comm *) lay.pTopology[0].lowLevel )

/* REDUCE HIT OPERATIONS */
/**
 * hit_comOp: creates a new conmutative HitOp operation. Used in reductions.
 * 
 * @hideinitializer
 * 
 * @param function conmutative function to create that matches to ( void * a, void * b, int * len, HitType * )
 * @param operation HitOp operation to create
 */
#define hit_comOp(function,operation)	MPI_Op_create((HitReduceFunction *)function, 1, &(operation) );

/**
 * hit_comOpNoCommutative: creates a new no-conmutative HitOp operation. Used in reductions.
 * 
 * @hideinitializer
 * 
 * @param function no-conmutative function to create the operator. Operator function prototype should be ( void * a, void * b, int * len, HitType * )
 * @param operation HitOp operation to create
 */
#define hit_comOpNoCommutative(function,operation)	MPI_Op_create(function, 0, &(operation) );

/**
 * hit_comOpFree: frees a HitOp operation.
 * 
 * @hideinitializer
 * 
 * @param operation HitOp operation to free.
 */
#define hit_comOpFree( operation )	MPI_Op_free( &(operation) );

/**
 * hit_comOpSumFloat: defined operation to add tiles of floats
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpSumFloat (void *, void *, int *, HitType *);

/**
 * hit_comOpMaxFloat: defined operation to calculate the maximum value in tiles of doubles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpMaxFloat (void *, void *, int *, HitType *);

/**
 * hit_comOpMinFloat: defined operation to calculate the minimum value in tiles of doubles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpMinFloat (void *, void *, int *, HitType *);

/**
 * hit_comOpSumDouble: defined operation to add tiles of doubles
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpSumDouble (void *, void *, int *, HitType *);

/**
 * hit_comOpMaxDouble: defined operation to calculate the maximum value in tiles of doubles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpMaxDouble (void *, void *, int *, HitType *);

/**
 * hit_comOpMinDouble: defined operation to calculate the minimum value in tiles of doubles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpMinDouble (void *, void *, int *, HitType *);

/**
 * hit_comOpSumInt: defined operation to calculate the sum value in tiles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpSumInt (void *, void *, int *, HitType *);

/**
 * hit_comOpMinInt: defined operation to calculate the min value in tiles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpMinInt (void *, void *, int *, HitType *);

/**
 * hit_comOpMaxInt: defined operation to calculate the max value in tiles.
 * You must use hit_comOp before use it, and hit_comOpFree at the end of the program.
 * @see creation of operations.
 */
void hit_comOpMaxInt (void *, void *, int *, HitType *);


/** HitComOpFunction: basic functions to reduce operations. */
typedef void (*HitComOpFunction) (void *, void * );

/**
 * hit_comOpGenericAnyType
 * @see creation of operations
 */
void hit_comOpGenericAnyType( void * in, void * inout, HitType datatype, int offset, size_t tam, HitComOpFunction f );


/**
 * CREATION OF OPERATORS
 * void myReduceFunction (void * inout, void * in) {
 *   *((int *)inout) += *((int *)in);
 * }
 * 
 * void myAddIntFunction (void * in, void * inout, int *len, HitType * type) {
 *	hit_comOpGenericAnyType(in,inout,*type,0,sizeof(int),&myReduceFunction);
 * }
 *
 * int main () {
 *    ...
 *	HitOp myOp;
 *  hit_comOp(myAddIntFunction,myOp);
 *    ...
 *     // Use your operator  
 *    ...
 *  hit_comOpFree(op);
 *    ...
 * }
*/
#ifdef __cplusplus
}
#endif

/* END OF HEADER FILE _HitCom_ */
#endif
