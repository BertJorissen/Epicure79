/**
 * Types and basic functions to manipulate domains.
 *
 * Definition of the HitShape interface type, for dense and sparse domains.
 *
 * @file hit_shape.h
 * @ingroup Tiling
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Mar 2011
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

#ifndef _HitShape_
#define _HitShape_

#include "hit_sig.h"

/** @cond INTERNAL */
#ifndef INCLUDE_METIS__H
	#define INCLUDE_METIS__H
	#include "metis.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif
/** @endcond */


/* MAXIMUM NUMBER OF DIMENSIONS, FIXED AT COMPILATION TIME */
#ifndef HIT_MAXDIMS
/**
 * Maximum number of array dimensions supported in a dense domain.
 *
 * Determined at compile time. This is the default value. It may be changed by the
 * programmer, although higher numbers of dimensions may need to define more access
 * macros in hit_sshape.h, hit_tileP.h
 */
#define HIT_MAXDIMS	4
#endif

/**
 * Maximum number of dimensions in a sparse domain.
 *
 * @note Sparse CSR format supports only and exactly 2.
 * @note Sparse Bitmap format supports 1 or 2 dimensions.
 */
#define HIT_SPARSE_MAXDIMS 2



/* Hit SHAPES */

/* 1. DATA STRUCTURE */
/* 1.1 SigShape data structure */
typedef struct {
	/** @privatesection */
	int numDims;				/**< Number of dimensions.**/
	HitSig	sig[HIT_MAXDIMS];	/**< HitSig objects to define each dimension.*/
} HitSigShape;


/* 1.2 LIST OF NAME ALIASES FOR THE INDEX IN AN SPARSE DOMAIN */
/** @cond INTERNAL */
/**
 * Internal structure for translation of tile to array coordinates for sparse
 * data formats such as CSR.
 */
typedef struct {
	/** @privatesection */
	int * names;		/**< Contains the list to translate between local and global names. */
	int nNames;			/**< Number of names. */
	int flagNames;		/**< Flag with information for the global to local translation. */
	int * invNames;		/**< Array with the global to local translation of vertices. */
	int nInvNames;		/**< Number of inv names. */
} HitNameList;
/** @endcond */


/* 1.3 CShape data structure */
/**
 * ADT for sparse domains in CSR format.
 *
 * A sparse CSR shape represents the index-domain of a graph, or a sparse matrix in
 * CSR format (Compressed Sparse Row matrix).
 * Standard CSR format can only represent a two dimensional space.
 *
 * @ingroup Tiling
 * @implements HitShape
 */
typedef struct{
	/** @privatesection */
	int cards[HIT_SPARSE_MAXDIMS];			/**< The cardinalities. */
	idxtype * xadj;							/**< Is an array with indices to adjncy to define the rows. */
	idxtype * adjncy;						/**< Contains the columns of each row element. */
	HitNameList names[HIT_SPARSE_MAXDIMS];	/**< Name list */
} HitCShape;


/* 1.3 BitmapShape data structure */
/** @cond INTERNAL */
/** Bitmap datatype */
#define HIT_BITMAP_TYPE unsigned int
/** Bitmap communication datatype */
#define HIT_BITMAP_COMM_TYPE MPI_UNSIGNED
/** Number of bits per Bitmap datatype */
#define HIT_BITMAP_SIZE (sizeof(HIT_BITMAP_TYPE) * (size_t) 8)
/** @endcond */

/**
 * ADT for sparse domains in Bitmap format.
 *
 * This struct uses a Bitmap to represent the non-zero elements in a graph
 * or sparse matrix. It can represent a full structure
 * or a selection of a global structure as far as the selected nodes/elements have
 * contigous indexes.
 *
 * The current implementaton supports up to two dimensions.
 *
 * @ingroup Tiling
 * @implements HitShape
 */
typedef struct{
	/** @privatesection */
	int cards[HIT_SPARSE_MAXDIMS];			/**< The cardinalities. */
	int nz;									/**< Number of non-zero elements or edges of the graph. */
	HIT_BITMAP_TYPE * data;					/**< Array with the bitmap. */
	HitNameList names[HIT_SPARSE_MAXDIMS];	/**< Name list */
} HitBShape;


/* 1.4 Shape data structure */
/* Constants for the class of shape: Signature and sparse shapes */
/**
 * Subclass identifier: Hit Signature dense Shape.
 * @hideinitializer
 */
#define HIT_SIG_SHAPE 0

/**
 * Subclass identifier: Hit CSR Sparse Shape.
 * @hideinitializer
 */
#define HIT_CSR_SHAPE 1

/**
 * Subclass identifier: Hit Bitmap Sparse Shape.
 * @hideinitializer
 */
#define HIT_BITMAP_SHAPE 2

/**
 * ADT for multidimensional domains.
 *
 * This abstract structure supports implementations of different
 * formats to represent domains of indexes.
 *
 * \sa HitSigShape, HitCShape, HitBShape
 */
typedef struct {
	/** @privatesection */
	int type;						/**< Shape type. */
	/**
	 * Union with the different types of Shapes.
	 */
	union{
		/** @privatesection */
		HitSigShape	sig;	/**< Signature shape. */
		HitCShape	csr;	/**< Sparse shape. */
		HitBShape	bitmap;	/**< Bitmap shape. */
	} info;
} HitShape;


/**@{*/
// CONSTANTS FOR THE INVERSE TRANSLATION LIST IN THE CSR AND BITMAP SHAPES.
/** @cond INTERNAL */
/**
 * Constant to indicate that the cShape or bShape does not have an inverse
 * translation is of vertices from global to local.
 */
#define HIT_SHAPE_NAMES_NOARRAY	0
/**
 * Constant to indicate that the cShape or bShape has an inverse
 * translation is of vertices from global to local.
 */
#define HIT_SHAPE_NAMES_ARRAY	1
/**
 * Constant to indicate that the cShape or bShape does not need an inverse
 * translation of vertices from global to local, because the vertices
 * are already numbered from 0 to N-1. The two coordinates spaces are the same.
 */
#define HIT_SHAPE_NAMES_ORDERED	2
/** @endcond */
/**@}*/

/**@{*/
/* 2. Hit SHAPE NULL CONSTANT */
/**
 * Null value for abstract shapes.
 *
 * The number of dimensions of this value is -1.
 */
extern HitShape	HIT_SHAPE_NULL;
/** @cond INTERNAL */
/** Null internal static value for signature shapes. */
#define HIT_SHAPE_NULL_STATIC { HIT_SIG_SHAPE, { { -1, { HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC } } } }
/** @endcond */


/** @cond INTERNAL */
/**
 * Null value for Name list.
 */
extern HitNameList HIT_NAMELIST_NULL;
/**
 * Null static value for Name list.
 */
#define HIT_NAMELIST_NULL_STATIC {NULL,0,HIT_SHAPE_NAMES_NOARRAY,NULL,0}
/** @endcond */
/**@}*/

/**
 * @name HitShape access macros
 */
/**@{*/

/* HIT SHAPE ACCESS MACROS */
/**
 * Access to the shape subclass.
 *
 * @hideinitializer
 *
 * @param[in] s \e HitShape A domain shape.
 * @retval	int	Identifier of the HitShape subclass.
 *
 * @sa HIT_SIG_SHAPE, HIT_CSR_SHAPE, HIT_BITMAP_SHAPE
 */
#define hit_shapeType(s) ((s).type)


/** @cond INTERNAL */
/**
 * Access to the sparse shape struct.
 * @hideinitializer
 * @return The CSR internal shape.
 */
#define hit_cShapeAccess(s) ((s).info.csr)

/**
 * Accesss to the bitmap shape struct
 * @param s A shape.
 * @hideinitializer
 * @return The Bitmap internal shape.
 */
#define hit_bShapeAccess(s) ((s).info.bitmap)
/** @endcond */
/**@}*/


/**@{*/
/* 6. HIT SHAPE FREE FUNTION */
/**
 * Free resources of a sparse shape.
 *
 * @note The subclass HitSigShape does not allocate dynamic resources.
 * The use of this function may be skipped for HitSigShape variables.
 *
 * @param[in,out] s A domain shape.
 */
void hit_shapeFree(HitShape s);
/**@}*/

/** @cond INTERNAL */
/* 12. Name list functionalities for sparse (csr and bitmap) shapes */
/**
 * Returns a name of the list
 * @param list The name list.
 * @param index The index.
 * @return The name of the index
 */
#define hit_nameListIndex2Name(list, index) (list.names[(index)])

/**
 * Returns the index of a name.
 * @param list The name list.
 * @param name The name.
 * @return The index.
 */
int hit_nameListName2Index(HitNameList list, int name);

/**
 * Creates a new name list.
 * @param list List pointer.
 * @param nelems Number of elements.
 */
void hit_nameListCreate(HitNameList * list, int nelems);

/**
 * Frees a name list.
 * @param list The list.
 */
void hit_nameListFree(HitNameList list);

/**
 * Adds a new element to the list
 * @param list A name list pointer.
 * @param x The name
 */
void hit_nameListAdd(HitNameList * list, int x);

/**
 * Create the inverse list name to make the translation.
 * @param list A name list pointer.
 */
void hit_nameListCreateInvNames(HitNameList * list);

/**
 * Clones a name list.
 * @param dst Destination list.
 * @param src Source list.
 */
void hit_nameListClone(HitNameList * dst, HitNameList * src);
/** @endcond */

#ifdef __cplusplus
	}
#endif


/* END OF HEADER FILE _HitShape_ */
#endif
