/** @cond INTERNAL */
/**
 * Hierarchical tiling of multi-dimensional dense and sparse structures.
 * Private definitions and functions to manipulate HitTile types.
 *
 * @file hit_tileP.h
 * @ingroup Tiling
 * @version 1.8
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @author Yuri Torres de la Sierra
 * @author Ana Moreton-Fernandez
 * @date Mar 2019
 */
/** @endcond */

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

#ifndef _HitTileP_
#define _HitTileP_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hit_error.h"
#include "hit_shape.h"
#include "hit_bshape.h"


#ifdef __cplusplus
	extern "C" {
#endif

/* 0. TYPE CONSTANTS */
/** @cond INTERNAL */
#define HIT_TILE	0
#define HIT_C_TILE	1
#define HIT_M_TILE	2
#define HIT_GC_TILE	3
#define HIT_GB_TILE	4
#define HIT_MC_TILE	5
#define HIT_MB_TILE	6
/** @endcond */


/* 1. ABSTRACT DATA-STRUCTURE DEFINITION */

/* 1.a. GENERIC ABSTRACT TYPE */
typedef struct HitTile {
/** @privatesection */
	int			type;				/**< Type of the HitTile: Tile, cTile, mTile, gcTile, gbTile, mcTile, mbTile. */
	size_t			baseExtent;			/**< Size of the variable type. */
	HitShape		shape;				/**< Shape of the Tile. */
	int			card[HIT_MAXDIMS];		/**< Dimension cardinalities. */
	int			acumCard;			/**< Flattened cardinality. */
	int			origAcumCard[HIT_MAXDIMS+1];	/**< Dimension accumulated cardinalities. */
	int			qstride[HIT_MAXDIMS];		/**< Memory stride for stride subselections */
	void			*data;				/**< Data pointer. It may be different to memPtr if it is a subselection tile. */
	void			*dataVertices;			/**< Data pointer to store the vertices in a graph tile. */
	void			*memPtr;			/**< Memory pointer. It keeps the original pointer to the allocated memory. */
	void			*memPtrVertices;		/**< Memory pointer. It keeps the original pointer to the allocated memory. */
	struct		 	HitTile	*ref;			/**< Pointer to the parent HitTile for hierarchical tiles. */
	char			memStatus;			/**< Memory status. */
	void			*ext;				/**< Extension for Controller model data */
	void			*distributed;			/**< Extension for distributed tiles (Hitmap2) */
/* ------------------------ MULTILEVEL TILES ---------------------------------------- */
	char			hierDepth;			/**< Hierarchical depth status. */
	struct		 	HitTile	*ancestor;		/**< Pointer to the ancestor in a multilevel hierarchy. */
	struct		 	HitTile	*unpadded;		/**< Pointer to a selection without padding. */
	int			childBegin[HIT_MAXDIMS];	/**< Starting array coordinate of first child in regular hierarchical structures, where all children have the same type and shape. */
	int			childSize[HIT_MAXDIMS];		/**< Normalized sizes of children in regular hierarchical structures, where all children have the same type and shape. */
} HitTile ;

/** @cond INTERNAL */
/**
 * Shortcut for pointer to the \e struct \e HitTile type.
 */
typedef HitTile *HitPTile;

// TODO: created by ismael @ 2017-12-19 16:40:50
// Add stream member to structure for cudaStream_t at GPU controllers
/**
 * Generate particular polymorphic types of HitTile.
 *
 * This macro allows to define particular polymorphic types of HitTile. The new defined type
 * will be named: \e HitTile_\<baseType\>.
 *
 * @version 1.6
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 *
 * @param[in] baseType	\e __typeName	Name of a valid native or derived C type.
 */
 #define 	hit_tileNewTypeInternal(baseType) 					\
 typedef struct HitTile_##baseType {							\
 	int			type;							\
 	size_t			baseExtent;						\
 	HitShape		shape;							\
 	int			card[HIT_MAXDIMS];					\
 	int			acumCard;						\
 	int			origAcumCard[HIT_MAXDIMS+1];				\
 	int			qstride[HIT_MAXDIMS];					\
 	baseType		*data;							\
 	baseType		*dataVertices;						\
 	baseType		*memPtr;						\
 	baseType		*memPtrVertices;					\
 	struct HitTile_##baseType *ref;							\
 	char			memStatus;						\
	void			*ext;							\
	void			*distributed;						\
 	char			hierDepth;						\
 	struct HitTile		*ancestor;						\
 	struct HitTile		*unpadded;						\
 	int			childBegin[HIT_MAXDIMS];				\
 	int			childSize[HIT_MAXDIMS];					\
 } HitTile_##baseType
//, *HitPTile_##baseType

/* Declare the type for the hierarchical Tiles */
hit_tileNewTypeInternal(HitTile);

/* 2.a CONSTANTS FOR MEMORY STATUS FIELD */
/**
 * Memory Status Constant
 */
#define	HIT_MS_NULL		0	/**< Reserved for HIT_TILE_NULL. */
#define	HIT_MS_NOMEM		1	/**< Root array with no memory allocated. */
#define	HIT_MS_OWNER		2	/**< Allocated array or tile. */
#define	HIT_MS_NOT_OWNER	3	/**< Tile selection pointing to the memory of another tile. */

/* 2b.1. CONSTANTS FOR HIERARCHICAL DEPTH FIELD */
/**
 * Constants to indicate Multilevel Hierarchical Tile property
 */
#define HIT_NONHIERARCHICAL		0	/**< Base elements are of native or programmer defined types. */

/* 2b.2. CONSTANTS FOR MULTILEVEL CARDINALITIES */
#define HIT_MULTILEVEL_UNDEFINED_SIZES	{ 1, 1, 1, 1 }
#define HIT_MULTILEVEL_UNDEFINED_BEGINS	{ 0, 0, 0, 0 }

/* 2c. CONSTANTS FOR MODE IN DATA UPDATE FROM/TO PARENT TILE */
#define HIT_UPDATE_FROM		0
#define HIT_UPDATE_TO		1

/* 4.g. SELECTIONS */
/** Out of bounds flag. */
#define	HIT_OUTOFBOUNDS_CHECK		0
/** No nout of bounds flag.*/
#define	HIT_NO_OUTOFBOUNDS_CHECK	1


/**
 * Null value for HitPTile derived types.
 */
extern HitPTile			HIT_TILE_NULL_POINTER;
#define	HIT_TILE_NULL_STATIC	{ 0, 0, HIT_SHAPE_NULL_STATIC, { 0,0,0,0 }, 0, { 0,0,0,0,0 }, { 0,0,0,0 }, NULL, NULL, NULL, NULL, NULL, HIT_MS_NULL, NULL, NULL, HIT_NONHIERARCHICAL, NULL, NULL, HIT_MULTILEVEL_UNDEFINED_BEGINS, HIT_MULTILEVEL_UNDEFINED_SIZES }

/**
 * Returns the class of the tile.
 *
 * @version 1.6.1
 * @author Javier Fresno Bausela
 * @author Arturo Gonzalez-Escribano
 * @date Aug 2015
 *
 * @param[in] tile 	\e HitTile	A HitTile variable.
 * @retval	int	 A constant with the subclass of HitTile.
 * @name Retrieving information about the tile shape
 */
#define hit_tileClass(tile) ((tile).type)


/* PROTOTYPES OF INTERNAL FUNCTIONS */

/**
 * Checks if a shape is in the boundaries of a tile using tile coordinates.
 *
 * @param[in] tileP	A pointer to an initialized HitTile derived type variable.
 * @param[in] sh	A subselection shape in coordinates relative to the tile domain.
 * @return 	True if the shape is inside the boundaries. False otherwise.
 */
int hit_tileCheckBoundaryTileCoords(const void *tileP, HitShape sh);

/**
 * Checks if a shape is in the boundaries of a tile using array coordinates.
 *
 * @param[in] tileP	A pointer to an initialized HitTile derived type variable.
 * @param[in] sh	A subselection shape in coordinates relative to the original array domain.
 * @return 	True if the shape is inside the boundaries. False otherwise.
 */
int hit_tileCheckBoundaryArrayCoords(const void *tileP, HitShape sh);

/* @arturo apr 2016 */
void hit_tileSingleInternal( void *tileP, void *var, size_t size );
void hit_tileDomainInternal(void *newVarP, size_t baseExtent, int hierDepth, int numDims, ...);
void hit_tileDomainShapeInternal(void *newVarP, size_t baseExtent, int hierDepth, HitShape shape);
void hit_tileAllocInternal(void *newVarP, const char *name, const char *file, int numLine);
void hit_tileFreeRecInternal(void * var);
void hit_tileFillInternal(void * var, void * value, const char *name, const char *file, int numLine);
void hit_tileCloneInternal(void *newVar, const void *oldVar, const char *name, const char *file, int numLine);
void hit_tileUpdateFromToAncestorInternal(void *shadow, int fromTo, const char *name, const char *file, int numLine);

/**
 * Initialize a hierarchical subselection.
 *
 * The output tile is a selection of the input tile, where the new domain is expressed
 * using a shape with tile coordinates of the originale variable.
 *
 * @hideinitializer
 *
 * @param[out] newVar	A pointer to a non-initialized HitTile derived type variable.
 * @param[in] oldVar	A pointer to a initialized and allocated HitTile derived type variable.
 * @param[in] shape		A shape indicating the domain to select.
 * @param[in]  out		Flag to indicate if the shape may be out-of-bounds of the domain of the oldVar tile.
 */
int hit_tileSelectInternal(void *newVar, const void *oldVar, HitShape sh, int out);

/**
 * Initialize a hierarchical subselection using array coordinates.
 *
 * The output tile is a selection of the input tile, where the new domain is expressed
 * using a shape with the ancestor array coordinates of the originale variable.
 *
 * @hideinitializer
 *
 * @param[out] newVar	A pointer to a non-initialized HitTile derived type variable.
 * @param[in] oldVar	A pointer to a initialized and allocated HitTile derived type variable.
 * @param[in] shape		A shape indicating the domain to select.
 * @param[in]  out		Flag to indicate if the shape may be out-of-bounds of the domain of the oldVar tile.
 */
int hit_tileSelectArrayCoordsInternal(void *newVar, const void *oldVar, HitShape sh, int out);

/** @endcond */

// @cond OLD
/* 6. SELECTING IFOUT OF BOUNDS (Deprecated v1.0) */
/* AVOID EVALUATION OF THE SECOND ARGUMENT IF POSSIBLE */
/* GLOBAL TEMPORAL VARIABLE */
#define hit_tileSelectOut(select1,select2) { if (!select1) select2; }
/** @endcond */


/* 8.5. ACCESS DATA ELEMENTS WITH TILE COORDINATES */
// @cond OLD
/**
 * hit_tileGet: obtains an element of a HitTile.
 * @param var		A HitTile derived type variable.
 * @param ndims		Number of dimensions of the array. It must be known in compilation time. Limited by HIT_MAXDIMS.
 * @param ...		A list of integer positive numbers defining the coordinate in each dimension.
 * @return 		Element read.
 */
#define hit_tileGet(var,ndims,...)	hit_tileElemAt(var,ndims,__VA_ARGS__)

/**
 * hit_tileSet: write a value in the indicate position of a HitTile.
 * @param var		A HitTile derived type variable.
 * @param ndims		Number of dimensions of the array. It must be known in compilation time. Limited by HIT_MAXDIMS.
 * @param val		Value to write.
 * @param ...		A list of integer positive numbers defining the coordinate in each dimension.
 */
#define hit_tileSet(var,ndims,val,...)	hit_tileElemAt(var,ndims,__VA_ARGS__) = (val)

/* 8.6. ACCESS DATA ELEMENTS WITH ARRAY COORDINATES */
/**
 * hit_tileGetArrayCoords: obtains an element of a HitTile.
 * @param var		A HitTile derived type variable.
 * @param ndims		Number of dimensions of the array. It must be known in compilation time. Limited by HIT_MAXDIMS.
 * @param ...		A list of integer positive numbers defining the array coordinate in each dimension.
 * @return 		Element read.
 */
#define hit_tileGetArrayCoords(var,ndims,...)	hit_tileElemAtArrayCoords(var,ndims,__VA_ARGS__)

/**
 * hit_tileSetArrayCoords: write a value in the indicate position of a HitTile.
 * @param var		A HitTile derived type variable.
 * @param ndims		Number of dimensions of the array. It must be known in compilation time. Limited by HIT_MAXDIMS.
 * @param val		Value to write.
 * @param ...		A list of integer positive numbers defining the array coordinate in each dimension.
 */
#define hit_tileSetArrayCoords(var,ndims,val,...)	hit_tileElemAtArrayCoords(var,ndims,__VA_ARGS__) = (val)


/* 8.7. ACCESS DATA ELEMENTS WITH TILE COORDINATES IN TILES WITH NO STRIDE */
/**
 * hit_tileGetNoStride: obtains an element of a HitTile (with no stride in the selection).
 * @param var		A HitTile derived type variable.
 * @param ndims		Number of dimensions of the array. It must be known in compilation time. Limited by HIT_MAXDIMS.
 * @param ...		A list of integer positive numbers defining the coordinate in each dimension.
 * @return 		Element read.
 */
#define hit_tileGetNoStride(var,ndims,...)	hit_tileElemAtNoStride(var,ndims,__VA_ARGS__)

/**
 * hit_tileSetNoStride: write a value in an element of a HitTile (with no stride in the selection).
 * @param var		A HitTile derived type variable.
 * @param ndims		Number of dimensions of the array. It must be known in compilation time. Limited by HIT_MAXDIMS.
 * @param val		Value to write.
 * @param ...		A list of integer positive numbers defining the coordinate in each dimension.
 */
#define hit_tileSetNoStride(var,ndims,val,...)	hit_tileElemAtNoStride(var,ndims,__VA_ARGS__) = (val)
/** @endcond */

/* 8.8. INTERNAL: MACROS TO OBTAIN THE POSITION OF AN ELEMENT */
/** @cond INTERNAL */
/* 8.8.1. TILE COORDINATES */
#define hit_tileElemAt1(var, pos)	((var).data[(pos)*(var).qstride[0]])

#define hit_tileElemAt2(var, pos1, pos2)	((var).data[(pos1)*(var).qstride[0]*(var).origAcumCard[1]+(pos2)*(var).qstride[1]])

#define hit_tileElemAt3(var, pos1, pos2, pos3)	((var).data[(pos1)*(var).qstride[0]*(var).origAcumCard[1]+(pos2)*(var).qstride[1]*(var).origAcumCard[2]+(pos3)*(var).qstride[2]])

#define hit_tileElemAt4(var, pos1, pos2, pos3, pos4)	((var).data[(pos1)*(var).qstride[0]*(var).origAcumCard[1]+(pos2)*(var).qstride[1]*(var).origAcumCard[2]+(pos3)*(var).qstride[2]*(var).origAcumCard[3]+(pos4)*(var).qstride[3]])

/* 8.8.2. ARRAY COORDINATES */
#define hit_tileElemAtArrayCoords1(var, pos)	((var).data[ hit_tileArray2Tile(var,0,pos) ])

#define hit_tileElemAtArrayCoords2(var, pos1, pos2)	((var).data[ hit_tileArray2Tile((var),0,(pos1))*(var).origAcumCard[1] + hit_tileArray2Tile((var), 1, (pos2))])

#define hit_tileElemAtArrayCoords3(var, pos1, pos2, pos3)	((var).data[ hit_tileArray2Tile((var),0,(pos1))*(var).origAcumCard[1] + hit_tileArray2Tile((var), 1, (pos2))*(var).origAcumCard[2] + hit_tileArray2Tile((var), 2, (pos3))])

#define hit_tileElemAtArrayCoords4(var, pos1, pos2, pos3, pos4)	((var).data[ hit_tileArray2Tile((var),0,(pos1))*(var).origAcumCard[1] + hit_tileArray2Tile((var), 1, (pos2))*(var).origAcumCard[2] + hit_tileArray2Tile((var), 2, (pos3))*(var).origAcumCard[3] + hit_tileArray2Tile((var), 3, (pos4))])

/* 8.8.3. TILE COORDINATES IN TILES WITH NO STRIDE */
#define hit_tileElemAtNoStride1(var, pos)	((var).data[pos])

#define hit_tileElemAtNoStride2(var, pos1, pos2)	((var).data[(pos1)*(var).origAcumCard[1]+(pos2)])

#define hit_tileElemAtNoStride3(var, pos1, pos2, pos3)	((var).data[(pos1)*(var).origAcumCard[1]+(pos2)*(var).origAcumCard[2]+(pos3)])

#define hit_tileElemAtNoStride4(var, pos1, pos2, pos3, pos4)	((var).data[(pos1)*(var).origAcumCard[1]+(pos2)*(var).origAcumCard[2]+(pos3)*(var).origAcumCard[3]+(pos4)])


/* 8.10. CHECK IF ARRAY COORDINATES INDEX IS IN TILE */
#define hit_tileHasArrayCoords1(var, pos)	\
	( pos>=hit_tileDimBegin(var,0) && (pos<=hit_tileDimEnd(var,0)) && \
	((pos-hit_tileDimBegin(var,0))%hit_tileDimStride(var,0) == 0) )

#define hit_tileHasArrayCoords2(var, pos0, pos1) \
		( pos0>=hit_tileDimBegin(var,0) && (pos0<=hit_tileDimEnd(var,0)) && \
		pos1>=hit_tileDimBegin(var,1) && (pos1<=hit_tileDimEnd(var,1)) && \
		((pos0-hit_tileDimBegin(var,0))%hit_tileDimStride(var,0) == 0) && \
		((pos1-hit_tileDimBegin(var,1))%hit_tileDimStride(var,1) == 0) )

#define hit_tileHasArrayCoords3(var, pos0, pos1, pos2) \
		( pos0>=hit_tileDimBegin(var,0) && (pos0<=hit_tileDimEnd(var,0)) && \
		pos1>=hit_tileDimBegin(var,1) && (pos1<=hit_tileDimEnd(var,1)) && \
		pos2>=hit_tileDimBegin(var,2) && (pos2<=hit_tileDimEnd(var,2)) && \
		((pos0-hit_tileDimBegin(var,0))%hit_tileDimStride(var,0) == 0) && \
		((pos1-hit_tileDimBegin(var,1))%hit_tileDimStride(var,1) == 0) && \
		((pos2-hit_tileDimBegin(var,2))%hit_tileDimStride(var,2) == 0) )

#define hit_tileHasArrayCoords4(var, pos0, pos1, pos2, pos3) \
		( pos0>=hit_tileDimBegin(var,0) && (pos0<=hit_tileDimEnd(var,0)) && \
		pos1>=hit_tileDimBegin(var,1) && (pos1<=hit_tileDimEnd(var,1)) && \
		pos2>=hit_tileDimBegin(var,2) && (pos2<=hit_tileDimEnd(var,2)) && \
		pos3>=hit_tileDimBegin(var,3) && (pos3<=hit_tileDimEnd(var,3)) && \
		((pos0-hit_tileDimBegin(var,0))%hit_tileDimStride(var,0) == 0) && \
		((pos1-hit_tileDimBegin(var,1))%hit_tileDimStride(var,1) == 0) && \
		((pos2-hit_tileDimBegin(var,2))%hit_tileDimStride(var,2) == 0) && \
		((pos3-hit_tileDimBegin(var,3))%hit_tileDimStride(var,3) == 0) )

/** @endcond */

#ifdef __cplusplus
	}
#endif


/* END OF HEADER FILE _HitTile_ */
#endif
