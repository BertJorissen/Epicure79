/**
 * Hierarchical tiling of multi-dimensional dense and sparse structures.
 * Definitions and functions to manipulate HitTile types. 
 * This type defines a handler for dense arrays and sparse structures
 * with meta-information to create and manipulate hierarchical tiles. 
 *
 * @file hit_tile.h
 * @ingroup Tiling
 * @version 1.6
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @author Yuri Torres de la Sierra
 * @date Mar 2013
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

#ifndef _HitTile_
#define _HitTile_

#include "hit_tileP.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* 1. ABSTRACT DATA-STRUCTURE DEFINITION */

/* 1.a. GENERIC ABSTRACT TYPES */

/**
 * @struct HitTileInterface
 * Virtual interface for all the tile classes.
 *
 * @ingroup Tiling
 */

/**
 * @struct HitGCTile
 *
 * Class of HitTile to implement Graphs using CSR sparse data format.
 *
 * @implements HitTileInterface
 * @ingroup Tiling
 */

/**
 * @struct HitGBTile
 *
 * Class of HitTile to implement Graphs using Bitmap sparse data format.
 *
 * @implements HitTileInterface
 * @ingroup Tiling
 */

/**
 * @struct HitMCTile
 *
 * Class of HitTile to implement Sparse Matrices using CSR sparse data format.
 *
 * @implements HitTileInterface
 * @ingroup Tiling
 */

/**
 * @struct HitMBTile
 *
 * Class of HitTile to implement Sparse Matrices using Bitmap sparse data format.
 *
 * @implements HitTileInterface
 * @ingroup Tiling
 */


/**
 * @struct HitATile
 *
 * Class of HitTile to implement classical dense Arrays, 
 * with contiguous indexes (stride equal to 1 in all dimensions).
 *
 * @extends HitTile
 * @ingroup Tiling
 */


/**
 * @struct HitTile
 *
 * Generic abstract data type for HitTile handler, and class of HitTile for dense arrays with
 * 		optional strided indexes.
 *
 * Hierarchical tiling of multi-dimensional dense and sparse structures.
 * This type defines a handler for dense arrays and sparse structures
 * with meta-information to create and manipulate hierarchical tiles. 
 *
 * @todo The member names for the classical dense Arrays subclass still use the 
 * @e hit_tile prefix, the hit_tile.h header file, and the @e HitTile type as if the
 * HitTile structure would be only supporting dense arrays. 
 * In the future, dense arrays based on signatures should have their own type name,
 * HitTile class, and prefix for function names.
 *
 * Member functions are documented in \ref hit_tile.h.
 *
 * @implements HitTileInterface
 * @ingroup Tiling
 * @version 1.6
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @author Yuri Torres de la Sierra
 * @date Mar 2013
 */

/**
 * @typedef HitTile
 * Shortcut for the \e struct \e HitTile type.
 */
typedef struct HitTile HitTile;

/**
 * Generate particular polymorphic types of HitTile.
 *
 * This macro allows to define particular polymorphic types of HitTile. The new defined type
 * will be named: \e HitTile_\<baseType\>.
 *
 * @hideinitializer
 *
 * @version 1.6
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 *
 * @param[in] baseType	\e __typeName	Name of a valid native or derived C type.
 */
#define 	hit_tileNewType(baseType)	hit_tileNewTypeInternal(baseType)

/**
 * @name Flags
 */
/**@{*/

/* 1. CONSTANTS FOR ALLOCATING VERTICES AND/OR EDGES IS SPARSE STRUCTURES */
/**
 * Flag to allocate only the vertices in a CSR/Bitmap sparse shape.
 * @hideinitializer
 */
#define HIT_VERTICES 1

/**
 * Flag to allocate  edges in a CSR/Bitmap sparse shape.
 * @hideinitializer
 */
#define HIT_EDGES 2

/**
 * Flag to allocate vertices and edges in a CSR/Bitmap sparse shape.
 * @hideinitializer
 */
#define HIT_VERTICES_EDGES	(HIT_VERTICES | HIT_EDGES)

/**@}*/


/* 2. NULL VARIABLES */
/**
 * Null value for HitTile derived types. 
 */
extern HitTile			HIT_TILE_NULL;

/**
 * Check if a tile variable is set to the Null value.
 *
 * @param[in] var HitTile A tile variable
 * @retval int Boolean value. True if the variable is set to Null. False otherwise.
 *
 * @hideinitializer
 *
 * @see HIT_TILE_NULL
 */
#define hit_tileIsNull(var)	((var).memStatus==HIT_MS_NULL)

/**
 * @name Initializers and tile memory management
 */
/**@{*/

/* 4. VARIABLE MANAGEMENT */
/* 4.a. CREATE A TILE OVER A SINGLE VARIABLE OF ANY TYPE */
/**
 * Initializer: Build a tile with a single element that points to a previous declared variable.
 *
 * This constructor is useful to create tiles that allow to use 
 * generate Hitmap communication objects that do operations on single variables.
 *
 * @hideinitializer
 *
 * @param[out] tile	\e HitTile_\<baseType\> A pointer to a HitTile derived type variable to be initialized.
 * @param[in] var	\e \<type\>  A variable of any type.
 * @param[in] type	\e __typeName The type name of the variable.
 *
 * @internal
 * @author arturo
 * @date Nov 2012
 */
#define	hit_tileSingle( tile, var, type )	hit_tileSingleInternal( tile, &var, sizeof( type ) )

/* 4.b. DECLARING VARIABLES WITHOUT MEMORY YET: ARRAY */
/**
 * Initializer: Initialize the sizes and base type of a new multidimensional array tile.
 *
 * The sizes of the array are a list of integers. The elements are numbered from 0 to n-1
 * on each dimension.
 *
 * This function initializes the tile variable but does not allocate memory for the elements.
 *
 * @hideinitializer
 *
 * @param[out] newVarP	\e HitTile_\<baseType\> A pointer to a HitTile derived type variable to be initialized.
 * @param[in] baseType	\e __typeName	The name of the type of the elements in the new array tile.
 * @param[in] numDims 	\e int Number of dimensions of the array. Limited by HIT_MAXDIMS.
 * @param[in] "..." 	\e int_list A list of numDims integer positive numbers defining the cardinalities on each dimension.
 *
 * @see hit_tileAlloc(), hit_tileDomainAlloc()
 */
#define hit_tileDomain( newVarP, baseType, numDims, ...)	hit_tileDomainInternal( newVarP, sizeof(baseType), 0==strncmp("HitTile",#baseType,7), numDims, __VA_ARGS__)

/**
 * Initializer: Initialize the shape and base type of a new multidimensional array tile.
 *
 * This initializer based on a shape object allows to specify arrays with indexes starting 
 * and ending at any integer number (positive or negative).
 *
 * This function initializes the tile variable but does not allocate memory for the elements.
 *
 * @hideinitializer
 *
 * @param[out] newVarP	\e HitTile_\<baseType\> A pointer to a HitTile derived type variable to be initialized.
 * @param[in] baseType	\e __typeName	The name of the type of the elements in the new array tile.
 * @param[in] shape		\e HitShape 	Shape object for the new array.
 *
 * @see hit_tileAlloc(), hit_tileDomainShapeAlloc()
 */
#define hit_tileDomainShape( newVarP, baseType, shape )	hit_tileDomainShapeInternal( newVarP, sizeof(baseType), 0==strncmp("HitTile",#baseType,7), shape )


/* 4.c. INITIALIZING VARIABLES: ARRAY */
/**
 * Allocate memory for the elements of an array tile.
 *
 * The parameter variable should have been previously initialized with
 * hit_tileDomain() or hit_tileDomainShape(). This function cannot be used
 * on a previously allocated variable before using hit_tileFree().
 *
 * @hideinitializer
 *
 * @param[in,out] var	\e HitTile* A pointer to a HitTile derived type variable.
 */
#define hit_tileAlloc( var )	hit_tileAllocInternal( var, #var, __FILE__, __LINE__);

/**
 * Initializer: Initialize and allocate memory for a new multidimensional array tile.
 *
 * The sizes of the array are a list of integers. The elements are numbered from 0 to n-1
 * on each dimension.
 *
 * @hideinitializer
 *
 * @param[out] newVarP	\e HitTile_\<baseType\> A pointer to a HitTile derived type variable to be initialized.
 * @param[in] baseType	\e __typeName	The name of the type of the elements in the new array tile.
 * @param[in] numDims 	\e int Number of dimensions of the array. Limited by HIT_MAXDIMS.
 * @param[in] "..." 	\e int_list A list of numDims integer positive numbers defining the cardinalities on each dimension.
 *
 * @see hit_tileAlloc(), hit_tileDomain(), hit_tileDomainShape(), hit_tileDomainShapeAlloc()
 */
#define hit_tileDomainAlloc(newVarP, baseType, numDims, ...)	\
	hit_tileDomainInternal(newVarP, sizeof(baseType), 0==strncmp("HitTile",#baseType,7), numDims, __VA_ARGS__);		\
	hit_tileAllocInternal( newVarP, #newVarP, __FILE__, __LINE__);

/**
 * Initializer: Initialize and allocate memory for a new multidimensional array tile.
 *
 * This initializer based on a shape object allows to specify arrays with indexes starting 
 * and ending at any integer number (positive or negative).
 *
 * @hideinitializer
 *
 * @param[out] newVarP	\e HitTile_\<baseType\> A pointer to a HitTile derived type variable to be initialized.
 * @param[in] baseType	\e __typeName	The name of the type of the elements in the new array tile.
 * @param[in] shape		\e HitShape 	Shape object for the new array.
 *
 * @see hit_tileAlloc(), hit_tileDomain(), hit_tileDomainShape(), hit_tileDomainAlloc()
 */
#define hit_tileDomainShapeAlloc(newVarP, baseType, shape)	\
	hit_tileDomainShapeInternal(newVarP, sizeof(baseType), 0==strncmp("HitTile",#baseType,7), shape); \
	hit_tileAllocInternal( newVarP, #newVarP, __FILE__, __LINE__);

/* 4.e. FREE VARIABLE (FREE INTERNAL DATA BUFFER) */
/**
 * Free allocated memory for the elements of an array tile.
 *
 * In case that the array tile has not been previously allocated, or it does not
 * use its own memory (see hit_tileSelect()), the function call is ignored.
 *
 * @hideinitializer
 *
 * @param[in,out] var	\e HitTile 	A HitTile derived type variable.
 */
#define hit_tileFree(var)	{ \
	if ( (var).hierDepth ) { \
		hit_tileFreeRecInternal(&(var)); \
	} \
	if ((var).memStatus==HIT_MS_OWNER) {\
		free((var).memPtr); (var).memPtr=NULL; (var).data=NULL; (var).memStatus=HIT_MS_NOMEM; \
	} \
}

/**
 * Initialize the elements of a tile array with a value.
 *
 * The tile variable should have been previously initialized and allocated.
 * The value is provided through a pointer to a variable of the base type which contains the
 * value. The value is copied to all the elements of the array tile.
 *
 * @hideinitializer
 *
 * @param[in,out] var	\e HitTile_\<baseType\>*	A pointer to a HitTile derived type variable.
 * @param[in]	value	\e \<baseType\>*	Pointer to a variable of the base type with the value.
 */
#define hit_tileFill( var, value )	hit_tileFillInternal( var, value, #var, __FILE__, __LINE__);

/* 4.f. DUPLICATE A WHOLE VARIABLE WITH ITS MEMORY AND DATA */
/**
 * Clone the array tile. 
 *
 * This function duplicates the handler and the array elements on a new not yet
 * initialized array tile variable.
 *
 * @hideinitializer
 *
 * @param[out] newVar	\e HitTile*	A pointer to a non-initialized HitTile derived type variable.
 * @param[in]  oldVar	\e HitTile* A pointer to a initialized and allocated HitTile derived type variable.
 */
#define hit_tileClone( newVar, oldVar ) hit_tileCloneInternal(newVar, oldVar, #oldVar, __FILE__, __LINE__)

/**@}*/


/* 4.g. SELECTIONS */

/**
 * @name Selection functions
 */
/**@{*/

/* 4.g.1. SELECTING FROM VARIABLES TO RETURN ANOTHER VARIABLE (LOCAL COORDINATES) */
/**
 * Initialize a hierarchical subselection using tile coordinates and no boundaries check.
 *
 * The output tile is a selection of the input tile, where the new domain is expressed
 * using a shape with tile coordinates of the originale variable. 
 * This function skips to check if the selection domain is out of bounds of the original variable.
 * Elements out of bounds can not be used is such selections, unless the new selection
 * tile is allocated to have its own memory. 
 *
 * @hideinitializer
 *
 * @param[out] newVar	\e HitTile*	A pointer to a non-initialized HitTile derived type variable.
 * @param[in] oldVar	\e HitTile*	A pointer to a initialized and allocated HitTile derived type variable.
 * @param[in] shape		\e HitShape	A shape indicating the domain to select.
 *
 * @sa hit_tileSelect(), hit_tileSelectArrayCoordsNoBoundary()
 */
#define hit_tileSelectNoBoundary( newVar, oldVar, shape )	\
			hit_tileSelectInternal( newVar, oldVar, shape, HIT_NO_OUTOFBOUNDS_CHECK )

/**
 * Initialize a hierarchical subselection using tile coordinates.
 *
 * The output tile is a selection of the input tile, where the new domain is expressed
 * using a shape with tile coordinates of the originale variable. 
 * It checks if the subselection domain is out of bounds of the original variable and
 * returns HIT_TILE_NULL in that case.
 *
 * @hideinitializer
 *
 * @param[out] newVar	\e HitTile*	A pointer to a non-initialized HitTile derived type variable.
 * @param[in] oldVar	\e HitTile*	A pointer to a initialized and allocated HitTile derived type variable.
 * @param[in] shape		\e HitShape	A shape indicating the domain to select.
 *
 * @sa hit_tileSelectNoBoundary(), hit_tileSelectArrayCoords()
 */
#define hit_tileSelect( newVar, oldVar, shape )	\
			hit_tileSelectInternal( newVar, oldVar, shape, HIT_OUTOFBOUNDS_CHECK )


/* 4.g.2. SELECTING FROM VARIABLES TO RETURN ANOTHER VARIABLE (GLOBAL COORDINATES) */
/**
 * Initialize a hierarchical subselection using array coordinates and no boundaries check.
 *
 * The output tile is a selection of the input tile, where the new domain is expressed
 * using a shape with the ancestor array coordinates of the originale variable. 
 * This function skips to check if the selection domain is out of bounds of the original variable.
 * Elements out of bounds can not be used is such selections, unless the new selection
 * tile is allocated to have its own memory. 
 *
 * @hideinitializer
 *
 * @param[out] newVar	\e HitTile*	A pointer to a non-initialized HitTile derived type variable.
 * @param[in] oldVar	\e HitTile*	A pointer to a initialized and allocated HitTile derived type variable.
 * @param[in] shape		\e HitShape	A shape indicating the domain to select.
 *
 * @sa hit_tileSelectArrayCoords(), hit_tileSelectNoBoundary()
 */
#define hit_tileSelectArrayCoordsNoBoundary( newVar, oldVar, shape )	\
			hit_tileSelectArrayCoordsInternal( newVar, oldVar, shape, HIT_NO_OUTOFBOUNDS_CHECK )

/**
 * Initialize a hierarchical subselection using ancestor array coordinates.
 *
 * The output tile is a selection of the input tile, where the new domain is expressed
 * using a shape with the ancestor array coordinates of the originale variable. 
 * It checks if the subselection domain is out of bounds of the original variable and
 * returns HIT_TILE_NULL in that case.
 *
 * @hideinitializer
 *
 * @param[out] newVar	\e HitTile*	A pointer to a non-initialized HitTile derived type variable.
 * @param[in] oldVar	\e HitTile*	A pointer to a initialized and allocated HitTile derived type variable.
 * @param[in] shape		\e HitShape	A shape indicating the domain to select.
 *
 * @sa hit_tileSelect(), hit_tileSelectArrayCoordsNoBoundary()
 */
#define hit_tileSelectArrayCoords( newVar, oldVar, shape )	\
			hit_tileSelectArrayCoordsInternal( newVar, oldVar, shape, HIT_OUTOFBOUNDS_CHECK )

/**@}*/


/**
 * @name Access to tile elements
 */
/**@{*/

/* 8.8. INTERNAL: MACROS TO OBTAIN THE POSITION OF AN ELEMENT */
/* 8.8.1. TILE COORDINATES */
/** 
 * Access to an element of an array tile, using tile coordinates.
 *
 * This macro is used in expressions and assignments to access or modify
 * the value of an element of the array tile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] ndims	\e int		Number of dimensions of the array. It must be known at compilation time. Limited to HIT_MAXDIMS.
 * @param[in] "..."	\e int_list	List of ndims integer indexes.
 */
#define hit_tileElemAt(var,ndims,...)	hit_tileElemAt##ndims(var,__VA_ARGS__)

/* 8.8.2. ARRAY COORDINATES */
/** 
 * Access to an element of an array tile, using ancestor array coordinates.
 *
 * This macro is used in expressions and assignments to access or modify
 * the value of an element of the array tile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] ndims	\e int		Number of dimensions of the array. It must be known at compilation time. Limited to HIT_MAXDIMS.
 * @param[in] "..."	\e int_list	List of ndims integer indexes.
 */
#define hit_tileElemAtArrayCoords(var,ndims,...)	hit_tileElemAtArrayCoords##ndims(var,__VA_ARGS__)

/* 8.8.3. TILE COORDINATES IN TILES WITH NO STRIDE */
/** 
 * Access to an element of an array tile, using tile coordinates in a ATile.
 *
 * This macro is used in expressions and assignments to access or modify
 * the value of an element of the array tile variable.
 *
 * @deprecated
 * This macro con only be used for HitATile arrays. 
 * It is equivalent to hit_tileElemAt(), but faster.
 * The name will be changed and it will be moved to be a member of the HitATile class.
 * The HitATile subclass has no stride in any dimension. The domain is \e contiguous 
 * or \e compact.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] ndims	\e int		Number of dimensions of the array. It must be known at compilation time. Limited to HIT_MAXDIMS.
 * @param[in] "..."	\e int_list	List of ndims integer indexes.
 *
 * \sa hit_tileElemAt()
 */
#define hit_tileElemAtNoStride(var,ndims,...)	hit_tileElemAtNoStride##ndims(var,__VA_ARGS__)

/**@}*/


/**
 * @name Domain iterators
 */
/**@{*/

/* 8.4. LOOPS FOR TRAVERSING COORDINATE SPACES */
/**
 * Loop across the ancestor array indexes of a dimension of an array tile.
 * 
 * @hideinitializer
 * 
 * @param[in] tile	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @param[out] index	\e int Variable index to be used as counter in the loop.
 */
#define	hit_tileForDimDomainArray(tile, dim, index)	for( index=hit_tileDimBegin(tile, dim);	\
												index<=hit_tileDimEnd(tile,dim);	\
												index=index+hit_tileDimStride(tile,dim) )

/**
 * Loop across the tile indexes of a dimension of an array tile.
 *
 * @hideinitializer
 * 
 * @param[in] tile	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @param[out] index	\e int Variable index to be used as counter in the loop.
 */
#define	hit_tileForDimDomain(tile, dim, index)	for( index=0;	\
												index<hit_tileDimCard(tile,dim);	\
												index++ )

/**@}*/


/**
 * @name Data movement to/from ancestor
 */
/**@{*/

/* 5. SHADOW COPY */
/* 5.a. UPDATE VALUES IN SHADOW COPY */
/**
 * Update elements of the ancestor tile with the input tile values.
 *
 * Update the elements of the nearest ancestor variable in the selection
 * chain, with the values of the same indexed elements of the input variable.
 * The input tile variable should be a selection of another tile, and it has
 * to be allocated to have its own memory.
 *
 * @hideinitializer
 *
 * @param[in] shadow	\e HitTile* A pointer to a HitTile derived type variable.
 */
#define hit_tileUpdateToAncestor( shadow ) hit_tileUpdateFromToAncestorInternal( shadow, HIT_UPDATE_TO, #shadow, __FILE__, __LINE__ );

/**
 * Update elements of the input tile with its ancestor values.
 *
 * Update the elements of the input tile with the values in the same indexed elements
 * of the nearest ancestor variable in the selection chain.
 * The input tile variable should be a selection of another tile, and it has
 * to be allocated to have its own memory.
 *
 * @hideinitializer
 *
 * @param[in,out] shadow	\e HitTile* A pointer to a HitTile derived type variable.
 */
#define hit_tileUpdateFromAncestor( shadow ) hit_tileUpdateFromToAncestorInternal( shadow, HIT_UPDATE_FROM, #shadow, __FILE__, __LINE__ );

/**@}*/


/**
 * @name Other tile structure utilities
 */
/**@{*/

/**
 * Reduce dimensions.
 *
 * If the shape of a HitTile variable has some starting cardinalities which
 * are 1 (e.g. a matrix of 1xn elements), the variable can be accesed faster
 * if the initial trivial dimensions are eliminated (e.g. accesing the 1xn matrix as
 * a vector of n elements).
 *
 * This function eliminates the number of required starting dimensions of a variable
 * shape as long as they are equal to 1, without modifying the values of the elements
 * or its storage.
 *
 * For example, if a variable have four dimensions, and its cardinalities
 * are 1,1,2,4, we may request to reduce 2 dimensions. The result is the same
 * variable, with the same data, but having only 2 dimensions
 * with cardinalities 2,4.
 *
 * @deprecated Not needed in the Trasgo translation system anymore, and not
 * 		needed for any application example.
 * @param[in,out] newVar	A pointer to a HitTile derived type variable.
 * @param[in] numReductions Number of starting dimensions to eliminate.
 * @return  0 if the reduction is correct, 1 in case the number of required dimensions cannot
 * 						be reduced.
 */
int hit_tileReduceDims(void *newVar, int numReductions);

/**
 * Flatten dimensions.
 *
 * Changes the shape of the tile to have the same number of elements but in the few number
 * of dimesions as possible. For memory owners the resulting shape has only one dimension.
 * For subselections the dimensions after the last selection are flattened. The values of 
 * the elements and its storage are not modified.
 *
 * @param[in,out] varP	A pointer to a HitTile.
 * @return  0 if the reduction is correct, 1 in case the no dimension can be flattened.
 */
int hit_tileFlattenDims(void *varP);

/* 7. MERGE/FUSION OF TILES WITH A TOUCHING BOUNDARY WITH THE SAME GEOMETRY */
/* GLUE TILES WITH TOUCHING BOUNDARIES OF THE SAME GEOMETRY */
/**
 * Glue two 1-dimensional array tiles with contiguous domains.
 *
 * For two array tile variables of 1-dimension, and
 * with contigous domains ( [a:b] and [b+1,c] ), this
 * function generates a new tile variable with the whole domain ( [a:c] ) and
 * a single contigous memory zone for the data of the two variables.
 *
 * @deprecated Not needed in the Trasgo translation system anymore, and not
 * 		needed for any application example.
 *
 * @param[in,out] tileInA	A pointer to a HitTile derived type variable.
 * @param[in,out] tileInB	A pointer to a HitTile derived type variable.
 * @param[out] tileOut	A pointer to a non-initialized HitTile derived type variable.
 *
 * @internal
 * @author Arturo Gonzalez-Escribano
 * @date Feb 2013
 *
 * Alternative name
 * void hit_tileFusion( const void *tileInA, const void *tileInB, void *tileOut );
 */
void hit_tileGlue( void *tileInA, void *tileInB, void *tileOut );

/**@}*/




/* 
* 8. Hit MACROS FOR ACCESING THE VARIABLES
*/


/**
 * @name Retrieving information about the tile shape
 */
/**@{*/

/* 8.1. BASIC SHAPE DATA */
/**
 * Get the number of dimensions of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in]	 var	\e HitTile	A HitTile derived type variable.
 * @retval	int 		Number of dimensions of the HitTile variable.
 */
#define	hit_tileDims(var)	(hit_sshapeDims((var).shape))

/**
 * Get the shape of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in]	 var	\e HitTile	A HitTile derived type variable.
 * @retval HitShape 		Shape of the HitTile variable.
 */
#define	hit_tileShape(var)	((var).shape)

/**
 * Get the shape of a HitTile variable in tile coordinates.
 * @param[in] inTile		A HitTile derived type variable.
 * @return 		Shape of the HitTile variable in tile coordinates.
 *
 * @internal
 * @author arturo 
 * @date Feb 2013
 */
HitShape hit_tileShapeLocal( const void *inTile );


/* 8.2. CARDINALITIES OF A GIVEN DIMENSION */
/* 8.2.1. CARDINALITY OF A GIVEN DIMENSION */
/**
 * Get the cardinality of a HitTile variable in a given dimension.
 *
 * Returns the number of rows, columns, etc. of the domain of the array tile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int	Number of the dimension.
 * @retval	int 		Cardinality in the specified dimension of the HitTile variable.
 */
#define	hit_tileDimCard(var,dim)	((var).card[dim])

/* 8.2.2. CARDINALITY OF THE WHOLE DOMAIN */
/**
 * Get the cardinality of a HitTile variable.
 *
 * Returns the total number of elements of the whole array tile variable.
 *
 * @hideinitializer
 *
 * @param[in]	 var	\e HitTile	A HitTile derived type variable.
 * @retval int 		Cardinality of the HitTile variable.
 */
#define	hit_tileCard(var)	((var).acumCard)


/* 8.3. SIGNATURE OF A GIVEN DIMENSION */
/**
 * Get the signature of a specific dimension of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @retval	HitSig 		Signature in the specified dimension of the HitTile variable.
 */
#define	hit_tileDimSig(var,dim)		(hit_shapeSig(hit_tileShape((var)),dim))
/**
 * Get the begin index of the signature of a specific dimension of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @retval	int 		Begin index of the signature in the specified dimension of the HitTile variable.
 */
#define	hit_tileDimBegin(var,dim)		(hit_tileDimSig(var,dim).begin)
/**
 * Get the end index of the signature of a specific dimension of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @retval	int 		End index of the signature in the specified dimension of the HitTile variable.
 */
#define	hit_tileDimEnd(var,dim)		(hit_tileDimSig(var,dim).end)
/**
 * Get the stride of the signature of a specific dimension of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @retval	int 		Stride of the signature in the specified dimension of the HitTile variable.
 */
#define	hit_tileDimStride(var,dim)		(hit_tileDimSig(var,dim).stride)
/**
 * Get the last index in tile-coordinates of a specific dimension of a HitTile variable.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension.
 * @retval 		Last tile-coordinate index in the specified dimension of the HitTile variable.
 */
#define	hit_tileDimLast(var,dim)		(hit_tileDimCard(var,dim)-1)

/**@}*/

/**
 * @name Index spaces information and transformation
 */
/**@{*/

/* 8.10. CHECK IF ARRAY COORDINATES INDEX IS IN TILE */
/** 
 * Check if an index is in the domain space of a given dimension of the array tile.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Selected dimension.
 * @param[in] pos	\e int		Index to check.
 * @retval	int	True if the index is in the dimensional domain space. False otherwise.
 */
#define hit_tileDimHasArrayCoord(var, dim, pos)	\
	( pos>=hit_tileDimBegin(var,dim) && (pos<=hit_tileDimEnd(var,dim)) && \
	((pos-hit_tileDimBegin(var,dim))%hit_tileDimStride(var,dim) == 0) )

/** 
 * Check if a multi-dimensional index is in the domain space of the array tile.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] ndims	\e int		Number of dimensions of the array.
 * @param[in] "..."	\e int_list	List of ndims integer indexes.
 * @retval	int	True if the multi-dimensonal index is in the domain space. False otherwise.
 */
#define hit_tileHasArrayCoords(var, ndims, ...)	hit_tileHasArrayCoords##ndims(var, __VA_ARGS__)


/* 8.9. TRANSFORM COORDINATES: TILE TO ARRAY, ARRAY TO TILE */
/** 
 * Transforms a dimensional index from tile to ancestor array coordinates space.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension of the array.
 * @param[in] pos	\e int		Index to trasnform.
 * @retval	int	Transformed index.
 */
#define hit_tileTile2Array(var, dim, pos)	((pos)*hit_tileDimStride(var,dim) + hit_tileDimBegin(var,dim))
/** 
 * Transforms a dimensional index from ancestor array to tile coordinates space.
 *
 * @hideinitializer
 *
 * @param[in] var	\e HitTile	A HitTile derived type variable.
 * @param[in] dim	\e int		Number of the dimension of the array.
 * @param[in] pos	\e int		Index to trasnform.
 * @retval	int	Transformed index.
 */
#define hit_tileArray2Tile(var, dim, pos)	(((pos)-hit_tileDimBegin(var,dim))/ hit_tileDimStride(var,dim))

/** 
 * Transforms a shape from tile to ancestor array coordinates space.
 *
 * @param[in] var	A HitTile derived type variable.
 * @param[in] sh	Shape to be transformed.
 * @retval	Transformed shape.
 */
HitShape hit_tileShapeTile2Array(void *var, HitShape sh);

/** 
 * Transforms a shape from ancestor array to tile coordinates space.
 *
 * @param[in] var	A HitTile derived type variable.
 * @param[in] sh	Shape to be transformed.
 * @retval	Transformed shape.
 */
HitShape hit_tileShapeArray2Tile(void *var, HitShape sh);


/**
 * Root ancestor.
 *
 * Finds the root array tile variable from which the selection chain of the input
 * tile variable started. The root ancestor is the one the defines the array coordinates.
 * The root ancestor of a new array which is not a selection of another one, is itself.
 *
 * @param[in] tileP		A pointer to a HitTile derived type variable.
 * @return  A pointer to the root ancestor of the input tile variable.
 */
static inline HitTile *	hit_tileRoot( void * tileP ) {
	HitTile *tile = (HitTile *)tileP;
	while( tile->ref != NULL ) tile = tile->ref;
	return tile;
}

/**
 * Nearest memory owner ancestor.
 *
 * Finds the nearest tile ancestor with allocated memory on the selection chain 
 * of the input tile variable. The memory ancestor of a tile with allocated memory is itself.
 *
 * @param[in] tileP		A pointer to a HitTile derived type variable.
 * @return  A pointer to the nearest memory owner ancestor of the input tile variable.
 */
static inline HitTile *	hit_tileMemoryAncestor( void * tileP ) {
	HitTile *tile = (HitTile *)tileP;
	while( tile->memStatus != HIT_MS_OWNER ) tile = tile->ref;
	return tile;
}

/**@}*/



#ifdef __cplusplus
	}
#endif

/* END OF HEADER FILE _HitTile_ */
#endif
