/**
 * Functionalities to manipulate Signature based domains.
 *
 * Manipulation of HitSigShape domains. Multidimensional shapes for
 * dense or strided domains of indexes in a bounding box.
 *
 * @deprecated Many functions in this file should be renamed to have the hit_sshape
 * 		prefix. They are mantained for portability of legacy Hitmap code.
 *
 * @file hit_sshape.h
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


#ifndef _HitSigShape_
#define _HitSigShape_

#include "hit_sig.h"
#include "hit_shape.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @struct HitSigShape
 * ADT for multidimensional Signature domains.
 *
 * A signature domain shape is tuple of signature triplets, one signature for each dimension.
 * These domains define a bounding box with dense or regularly strided multidimensional indexes. 
 * The cardinality of a shape is the number of all possible valid indexes combinations.
 *
 * This shape allows to define the domain of dense arrays with multiple indexes in a range 
 * of positive/negative integers with an optional stride.
 *
 * Member functions are documented in \ref hit_sshape.h.
 *
 * @ingroup Tiling
 * @implements HitShape
 */

/* 1. Hit SHAPE SPECIAL VALUES */
/** @cond INTERNAL */
/**
 * Null internal static value for multidimensional Signature domain shapes.
 * The number of dimensions of this value is -1.
 */
#define	 HIT_SIG_SHAPE_INTERNAL_NULL_STATIC		{ -1, { HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC } }

/** Null internal static value for signature shapes. */
#define HIT_SIG_SHAPE_NULL_STATIC { HIT_SIG_SHAPE, { HIT_SIG_SHAPE_INTERNAL_NULL_STATIC} }

/** Hit signature shape whole static constant. */
#define	HIT_SIG_SHAPE_WHOLE_STATIC		{ 0, { HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC, HIT_SIG_NULL_STATIC } }

/** Hit shape whole static constant.  */
#define	HIT_SHAPE_WHOLE_STATIC		{ HIT_SIG_SHAPE, { HIT_SIG_SHAPE_WHOLE_STATIC} }
/** @endcond */

/**
 * Null value for signature shapes.
 *
 * The number of dimensions of this value is -1.
 */
extern HitShape	HIT_SIG_SHAPE_NULL;


/**
 * Special value: Whole domain. 
 *
 * Special value to indicate in tile selections
 * that the whole collection of indexes of
 * another shape should be selected.
 *
 * The number of dimensions of this value is 0.
 *
 * \sa  hit_tileSelect()
 */
extern HitSigShape	HIT_SIG_SHAPE_WHOLE;

/**
 * Special value: Whole domain. 
 *
 * Special value to indicate in tile selections
 * that the whole collection of indexes of
 * another shape should be selected.
 *
 * @deprecated Use HIT_SIG_SHAPE_WHOLE for signature domains.
 *
 * The number of dimensions of this value is 0.
 *
 * \sa  hit_tileSelect()
 */
extern HitShape	HIT_SHAPE_WHOLE;

/** @cond INTERNAL */
/* 2. ACCESS METHOD TO THE SIGNATURE STRUCT */
/**
 * Access to the signature shape struct.
 * @hideinitializer
 * @return The Signature internal shape.
 */
#define hit_sShapeAccess(s) ((s).info.sig)
/** @endcond */


/* 3. Hit SHAPE GENERATING FUNCTIONS */
/** @cond INTERNAL */
/* 
 * Internal constructor, generating a shape from a internal HitSigShape structure
 *
 * @author arturo 
 * @date Feb 2013
 */
static inline HitShape hit_shapeFromSigShape( HitSigShape s ) {
	HitShape a = { HIT_SIG_SHAPE, { s } };
	return a;
}
/** @endcond */

/**
 * @name Initializers, constructors
 */
/**@{*/


/* 4.1 Hit Signature Shape generating functions */
/**
 * Constructor (1) of Signature domain shapes.
 *
 * The number of dimensions is limited by the maximum number of dimensions established
 * when compiling the library.
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @see HIT_MAXDIMS
 *
 * @hideinitializer
 *
 * @param[in] nd 	\e int Number of dimensions.
 * @param[in] "..."	\e HitSig_list A list of "nd" signature values.
 * @retval	HitShape  A new shape value.
 */
#define hit_shape(nd, ...)	hit_shape##nd(__VA_ARGS__)

/** @cond INTERNAL */
/**
 * Hit Signature Shape constructor for one dimension.
 */
static inline HitShape hit_shape1(HitSig s1) {
	HitShape a = { HIT_SIG_SHAPE, { { 1, { s1 } }} };
	return a;
}
/**
 * Hit Signature Shape constructor for two dimensions.
 */
static inline HitShape hit_shape2(HitSig s1, HitSig s2) {
	HitShape a = { HIT_SIG_SHAPE, { { 2, { s1, s2 } }} };
	return a;
}
/**
 * Hit Signature Shape constructor for three dimensions.
 */
static inline HitShape hit_shape3(HitSig s1, HitSig s2, HitSig s3) {
	HitShape a = { HIT_SIG_SHAPE, { { 3, { s1, s2, s3 } }} };
	return a;
}
/**
 * Hit Signature Shape constructor for four dimensions.
 */
static inline HitShape hit_shape4(HitSig s1, HitSig s2, HitSig s3, HitSig s4) {
	HitShape a = { HIT_SIG_SHAPE, { { 4, { s1, s2, s3, s4 } }} };
	return a;
}
/** @endcond */

/**
 * Constructor (2) of Signature domain shapes, for a particular multidimensional index.
 *
 * This construtor builds a shape that contains only one multidimensional index.
 *
 * The number of dimensions is limited by the maximum number of dimensions established
 * when compiling the library.
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 * @deprecated Not used in the Trasgo translation system or in any application example
 *
 * @see HIT_MAXDIMS
 *
 * @hideinitializer
 *
 * @param[in] nd 	\e int Number of dimensions.
 * @param[in] "..."	\e int_list A list of "nd" integer indexes.
 * @retval	HitShape  A new shape value with one multidimensional index.
 */
#define hit_shapeIndex(nd, ...)	hit_shapeIndex##nd(__VA_ARGS__)

/** @cond INTERNAL */
/**
 * Hit Signature Shape constructor for one dimension, select only one index element.
 */
static inline HitShape hit_shapeIndex1( int idx ) {
	HitShape a = { HIT_SIG_SHAPE, { { 1, { hit_sig( idx, idx, 1 ) } }} };
	return a;
}

/**
 * Hit Signature Shape constructor for two dimensions, select only one index element per dimension.
 */
static inline HitShape hit_shapeIndex2( int idx1, int idx2 ) {
	HitShape a = { HIT_SIG_SHAPE, { { 2, {
			hit_sig( idx1, idx1, 1 ),
			hit_sig( idx2, idx2, 1 ) 
			} }} };
	return a;
}

/**
 * Hit Signature Shape constructor for three dimensions, select only one index element per dimension.
 */
static inline HitShape hit_shapeIndex3( int idx1, int idx2, int idx3 ) {
	HitShape a = { HIT_SIG_SHAPE, { { 3, {
			hit_sig( idx1, idx1, 1 ),
			hit_sig( idx2, idx2, 1 ),
			hit_sig( idx3, idx3, 1 ) 
			} }} };
	return a;
}

/**
 * Hit Signature Shape constructor for four dimensions, select only one index element per dimension.
 */
static inline HitShape hit_shapeIndex4( int idx1, int idx2, int idx3, int idx4 ) {
	HitShape a = { HIT_SIG_SHAPE, { { 4, {
			hit_sig( idx1, idx1, 1 ),
			hit_sig( idx2, idx2, 1 ),
			hit_sig( idx3, idx3, 1 ),
			hit_sig( idx4, idx4, 1 ) 
			} }} };
	return a;
}
/** @endcond */


/**
 * Constructor (3) of Signature domain shapes, with standard C array domains.
 *
 * Standard array C domains have index ranges from 0 to N-1, with no stride. A size
 * for each dimension is enough to describe them.
 *
 * The number of dimensions is limited by the maximum number of dimensions established
 * when compiling the library.
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @see HIT_MAXDIMS
 *
 * @hideinitializer
 *
 * @param[in] nd 	\e int Number of dimensions.
 * @param[in] "..."	\e int_list A list of "nd" positive integer size values.
 * @retval	HitShape  A new shape value.
 */
#define hit_shapeStd(nd, ...)	hit_shapeStd##nd(__VA_ARGS__)


/** @cond INTERNAL */
/**
 * Hit Signature Shape constructor for one dimension, standard domain
 */
static inline HitShape hit_shapeStd1( int _size ) {
	HitShape a = { HIT_SIG_SHAPE, { { 1, { hit_sigStd( _size ) } } } };
	return a;
}

/**
 * Hit Signature Shape constructor for two dimensions, standard domain
 */
static inline HitShape hit_shapeStd2( int size1, int size2 ) {
	HitShape a = { HIT_SIG_SHAPE, { { 2, {
			hit_sigStd( size1 ),
			hit_sigStd( size2 ) 
			} }} };
	return a;
}

/**
 * Hit Signature Shape constructor for three dimensions, standard domain
 */
static inline HitShape hit_shapeStd3( int size1, int size2, int size3 ) {
	HitShape a = { HIT_SIG_SHAPE, { { 3, {
			hit_sigStd( size1 ),
			hit_sigStd( size2 ),
			hit_sigStd( size3 ) 
			} }} };
	return a;
}

/**
 * Hit Signature Shape constructor for four dimensions, standard domain
 */
static inline HitShape hit_shapeStd4( int size1, int size2, int size3, int size4 ) {
	HitShape a = { HIT_SIG_SHAPE, { { 4, {
			hit_sigStd( size1 ),
			hit_sigStd( size2 ),
			hit_sigStd( size3 ),
			hit_sigStd( size4 ) 
			} }} };
	return a;
}
/** @endcond */

/**@}*/

/**
 * @name Retrieving/Setting shape information
 */
/**@{*/

/**
 * Number of dimensions in a shape.
 *
 * @hideinitializer
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 * 		The correct method to use is now hit_sshapeDims().
 *
 * @hideinitializer
 *
 * @param[in] shape \e HitShape A domain shape.
 * @retval	int  Number of dimensions of the shape.
 */
#define	hit_shapeDims(shape)	(((shape).type==HIT_SIG_SHAPE)?hit_sshapeDims(shape):(1))


/* 5.1 Hit Signature Shape access macros */
/**
 * Number of dimensions in a shape.
 *
 * @hideinitializer
 *
 * @param[in] shape \e HitShape A domain shape.
 * @retval	int  Number of dimensions of the shape.
 */
#define hit_sshapeDims(shape) (hit_sShapeAccess(shape).numDims)

/**
 * Set the number of dimensions in a shape.
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 * @hideinitializer
 *
 * @param[out] shape \e HitShape A domain shape.
 * @param[in] value The new number of dimensions.
 */
#define hit_shapeDimsSet(shape,value) (hit_sshapeDims(shape) = (value))


/**
 * Access to one dimensional signature in a shape.
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 * @hideinitializer
 *
 * @param[in] shape \e HitShape A domain shape.
 * @param[in] dim 	\e int A dimension,
 * @retval	HitSig  The dimensional signature of that shape.
 */
#define	hit_shapeSig(shape,dim)	((hit_sShapeAccess((shape))).sig[(dim)])

/**
 * Cardinality of a dimensional signature in a shape.
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 * @hideinitializer
 *
 * @param[in] shape \e HitShape A domain shape.
 * @param[in] dim 	\e int A dimension,
 * @retval int	The cardinality of the dimensional signature of that shape.
 */
#define	hit_shapeSigCard(shape,dim)	hit_sigCard((hit_sShapeAccess((shape))).sig[(dim)])

/**
 * Cardinality of a whole shape.
 *
 * @warning Only for HitSigShape. 
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param[in] shape A domain shape.
 * @return The cardinality of the indexes set of that shape.
 */
static inline int hit_shapeCard( HitShape shape ) {
	int i;
	int result = 1;
	for ( i=0; i<hit_shapeDims( shape ); i++ )
		result = result * hit_shapeSigCard( shape, i );
	return result;
}

/**@}*/

/**
 * @name Binary shape operators
 */
/**@{*/

/* 7. Hit SHAPE: COMPARISON OPERATOR */
/**
 * Compare two shape values.
 *
 * @warning Only for HitSigShape. It returns 0 with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param sh1 Domain shape.
 * @param sh2 Domain shape.
 * @return Logical value. True if the two shapes are equal.
 */
int hit_shapeCmp(HitShape sh1, HitShape sh2);


/* 8. Hit INTERSECT SHAPES */
/**
 * Intersection of two shapes.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param sh1 Domain shape.
 * @param sh2 Domain shape.
 * @return A new domain shape with the indexes combinations which
 * 			are valid in both shapes at the same time.
 */
HitShape hit_shapeIntersect(HitShape sh1, HitShape sh2);

/* 8.2. Hit SUBSELECT SHAPES */
/**
 * Subset or subselection of a shape in tile coordinates.
 *
 * Returns a new domain shape with the indexes of the first shape that exist
 * 			in the second shape when renumbering the first shape in standard
 * 			C notation (starting at 0, stride 1).
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param sh1 Domain shape.
 * @param sh2 Subselection shape.
 * @return A shape with the selected indexes.
 */
HitShape hit_shapeSubset(HitShape sh1, HitShape sh2);

/* 8.3. Hit SHAPE: TILE TO ARRAY */
/**
 * Transform tile coordinates to array coordinates.
 *
 * Returns a new domain shape with the indexes of the second shape transformed to
 * 			array coordinates in the reference system of the first shape.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param sh1 Domain shape.
 * @param sh2 Subshape in tile coordinates
 * @return A shape with the transformed indexes.
 */
HitShape hit_shapeTileToArray(HitShape sh1, HitShape sh2);

/**
 * Transform array coordinates to tile coordinates.
 *
 * Returns a new domain shape with the indexes of the second shape transformed to
 * 			tile coordinates in the reference system of the first shape.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * 			If the shapes aren't compatible (their strides aren't proportional) it
 * 			returns HIT_SHAPE_NULL
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param sh1 Domain shape.
 * @param sh2 Subshape in array coordinates
 * @return A shape with the transformed indexes.
 */
HitShape hit_shapeArrayToTile(HitShape sh1, HitShape sh2);

/**@}*/

/**
 * @name Boundaries and halos
 */
/**@{*/

/* 9. Hit Signature Shape operations */
/* 9.0 Hit SHAPE ACTIONS AND FLAGS */
/* Values for hit_shapeTransform, hit_shapeBorder, hit_shapeExpand, hit_shapeDimExpand */
/**
 * Hit Shape All-dimensions flag for the shape transformation functions.
 *
 * Applies the same transformation to all dimensions.
 *
 * @hideinitializer
 * @see hit_shapeTransform()
 */
#define HIT_SHAPE_ALLDIMS -1
/**
 * Hit Shape Raw Coordinate system flag for the shape transformation functions.
 *
 * Applies actions on raw coordinates, not taken into account the strides in the shape
 *
 * @hideinitializer
 * @see hit_shapeTransform()
 */
#define HIT_SHAPE_RAW 1000
/**
 * Hit Shape Begin flag for the shape transformation functions.
 *
 * Moves the begin coordinate.
 *
 * @hideinitializer
 * @see hit_shapeTransform(), hit_shapeBorder(), hit_shapeExpand(), hit_shapeDimExpand()
 */
#define HIT_SHAPE_BEGIN 0
/**
 * Hit Shape Begin flag for the shape transformation functions.
 *
 * Moves the end coordinate.
 *
 * @hideinitializer
 * @see hit_shapeTransform(), hit_shapeBorder(), hit_shapeExpand(), hit_shapeDimExpand()
 */
#define HIT_SHAPE_END	1
/**
 * Hit Shape Move flag for the shape transformation functions.
 *
 * Moves the begin and end coordinates, displacing the shape with the specified offset.
 *
 * @hideinitializer
 * @see hit_shapeTransform(), hit_shapeBorder(), hit_shapeExpand(), hit_shapeDimExpand()
 */
#define HIT_SHAPE_MOVE	2
/**
 * Hit Shape Stretch flag for the shape transformation functions.
 *
 * Begin coordinate decreases and end coordinate increases with the given offset.
 *
 * @hideinitializer
 * @see hit_shapeTransform()
 */
#define HIT_SHAPE_STRETCH	3
/**
 * Hit Shape First-elements flag for the shape transformation functions.
 *
 * Selects the first n coordinates starting at begin, with n indicated by the offset
 *
 * @hideinitializer
 * @see hit_shapeTransform()
 */
#define HIT_SHAPE_FIRST	4
/**
 * Hit Shape Last-elements flag for the shape transformation functions.
 *
 * Selects the last n coordinates, ending at end, with n indicated by the offset
 *
 * @hideinitializer
 * @see hit_shapeTransform()
 */
#define HIT_SHAPE_LAST	5


/* 9.1 Hit SHAPE EXPAND DIM */
/**
 * Expand a boundary.
 *
 * Return a shape with a given boundary of one selected dimension expanded by an offset.
 * This function is useful to compute halos, and other boundary related shapes.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param shape A HitShape.
 * @param dim The dimension to expand.
 * @param position Flag to select the border:
 * 						\arg \c HIT_SHAPE_BEGIN For the starting border in the dimension.
 * 						\arg \c HIT_SHAPE_END For the ending border in the dimension.
 * @param offset A positive offset moves the border outside the shape.
 * 				A negative offser moves the border inside the shape.
 * @return The expanded shape.
 */
HitShape hit_shapeDimExpand( HitShape shape, int dim, int position, int offset );

/* 9.2 Hit SHAPE EXPAND */
/**
 * Expand boundaries.
 *
 * Return a shape with the boundaries of several dimensions expanded by a given offset.
 * This function is useful to compute halos, and other boundary related shapes.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param shape A HitShape.
 * @param dims The number of dimension to expand.
 * @param offset A positive offset moves the border outside the shape.
 * 				A negative offser moves the border inside the shape.
 * @return The expanded shape.
 */
HitShape hit_shapeExpand(HitShape shape,int dims,int offset);

/* 9.3 Hit SHAPE GET BORDER */
/**
 * Boundary selection.
 *
 * Return another shape with the shape of the boundary (or border) elements in a given dimension,
 * or a similar boundary shape but displaced by a given offset in that dimension.
 * This function is useful to compute halos, and other boundary related shapes.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param shape A HitShape.
 * @param dim The dimension.
 * @param position Flag to select the border:
 * 						\arg \c HIT_SHAPE_BEGIN For the starting border in the dimension.
 * 						\arg \c HIT_SHAPE_END For the ending border in the dimension.
 * @param offset A positive offset moves the border outside the shape.
 * 				A negative offser moves the border inside the shape.
 * @return A shape representing the selected border.
 */
HitShape hit_shapeBorder(HitShape shape, int dim, int position, int offset);

/* 9.4 Hit SHAPE TRANSFORM */
/**
 * Shape domain transformations (move, stretch, or cut)
 *
 * Return another shape with modified boundaries. The modification can be applied to 
 * a specific dimension or to all dimensions at the same time.
 * This function is useful to compute halos, and other boundary related shapes.
 *
 * @warning Only for HitSigShape. It returns HIT_SHAPE_NULL with sparse shapes.
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param shape A HitShape.
 * @param dim A valid number of dimension, or HIT_SHAPE_ALLDIMS
 * @param action Flag to select the type of transformation:
 * 		\arg \c HIT_SHAPE_BEGIN Move only the begin coordinate by an offset.
 * 		\arg \c HIT_SHAPE_END Move only the end coordinate by an offset.
 * 		\arg \c HIT_SHAPE_MOVE Move both the begin and end coordinates by the same offset.
 * 		\arg \c HIT_SHAPE_STRETCH Decrease the begin and increase the end by the same offset: Positive offset to grow, negative offset to shrink.
 * 		\arg \c HIT_SHAPE_FIRST Keep the begin and change the end to have the number elements indicated by the offset.
 * 		\arg \c HIT_SHAPE_LAST Keep the end and change the begin to have the number elements indicated by the offset.
 * 		\arg \c HIT_SHAPE_RAW Do an OR operation of the action with this flag to consider the offset in raw coordinates, ignoring the shape strides.
 * @param offset A positive or negative number of elements. 
 * 		The offset is expressed by default as a number of elements, 
 * 		taking into account the shape strides. Use the HIT_SHAPE_RAW flag in the action to
 * 		express the offset as raw coordinates, ignorig strides.
 * @return A shape representing the transformed shape.
 */
HitShape hit_shapeTransform(HitShape shape, int dim, int action, int offset);

/**@}*/

/**
 * @name Iterators
 */
/**@{*/

/* 11. Hit Shape Iterators */
/**
 * Loop across the array indexes of a dimension.
 *
 * @hideinitializer
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param[out] var	\e int Variable index to be used as counter in the loop.
 * @param[in] shape \e HitShape A domain shape.
 * @param[in] dim	\e int Number of the dimension.
 */
#define hit_shapeIterator(var,shape,dim) for(var=hit_shapeSig((shape),(dim)).begin;var<=hit_shapeSig((shape),(dim)).end;var+=hit_shapeSig((shape),(dim)).stride)

/**
 * Loop across the tile indexes of a dimension.
 *
 * This iterator starts at 0, advances with stride 1, and stops
 * at the cardinality of the dimension signature minus 1.
 *
 * @hideinitializer
 *
 * @deprecated The name will be changed to use the hit_sshape prefix.
 *
 * @param[out] var	\e int Variable index to be used as counter in the loop.
 * @param[in] shape \e HitShape A domain shape.
 * @param[in] dim	\e int Number of the dimension.
 */
#define hit_shapeIteratorTileCoord(var,shape,dim) for(var=0; var<=hit_sigCard(hit_shapeSig((shape),(dim)))-1;var++)

/**@}*/


/* PRINT A SHAPE*/
/**
 * Print a HitShape
 * 
 * @param sh \e HitShape A HitShape.
 */ 
void dumpShape (HitShape sh); 

#ifdef __cplusplus
	}
#endif

/* END OF HEADER FILE _HitShape_ */
#endif

