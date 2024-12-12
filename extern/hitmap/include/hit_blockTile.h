/**
 * Arrays of blocks
 *
 * Array tiles in which elements are pointers to block tiles of the same type and size.
 * Current implementation supports 1 and 2-dimensional arrays.
 *
 * @file hit_blockTile.h
 * @ingroup Tiling
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @date Feb 2012
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

#ifndef _HitBlockTile_
#define _HitBlockTile_

#include "hit_sshape.h"
#include "hit_tile.h"
#include "hit_dump.h"

/**
 * @struct HitBlockTile
 *
 * Class of HitTile for Arrays of blocks
 *
 * Array tiles in which elements are pointers to block tiles of the same type and size.
 * Current implementation supports 1 and 2-dimensional arrays.
 *
 * Member functions are documented in \ref hit_blockTile.h.
 *
 * @ingroup Tiling
 * @extends HitTile
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @date Feb 2012
 */


/**
 * Shortcut name for the generic hierarchical type of arrays of blocks
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * @date Ago 2015
 */
typedef HitTile_HitTile HitBlockTile;

/**
 * Initializer.
 *
 * It uses a HitTile as reference (originalVar), to get a plain domain and a base type for 
 * the elements. It initilizes the domain of an array of blocks of the given sizes, that covers
 * the domain of the originalVar.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[out] container   Pointer to the new array of blocks tile.
 * @param[in]  originalVar Reference tile to get a domain and a base type for the elements.
 * @param[in]  blockSizes  Array of integers with the block sizes. As many elements as the 
 * 							dimensions of the originalVar should exist and should be initialized.
 */
void hit_blockTileNew( HitBlockTile *container, void *originalVar, int blockSizes[ HIT_MAXDIMS ] );

/**
 * Allocate an array of blocks.
 *
 * It allocates the memory of an array of blocks with an already initilized domain.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 *
 * @hideinitializer
 * 
 * @param[in,out] varP	\e HitBlockTile*  Pointer to an array of blocks tile.
 * @param[in]  baseType \e __typeName  Type of the array elements.
 */
#define hit_blockTileAlloc( varP, baseType ) hit_blockTileAllocInternal( varP, sizeof(baseType), NULL )

/**
 * Allocate and initilize the elements of an array of blocks.
 *
 * It allocates the memory of an array of blocks with an already initilized domain,
 * and it fill the elements of all the blocks with the content of the variable pointed
 * by fillValue.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @hideinitializer
 *
 * @param[in,out] varP  \e HitBlockTile*  Array of blocks tile.
 * @param[in]  baseType	\e _typeName  Type of the array elements.
 * @param[in]  fillValue \e baseType* Pointer to a variable of type baseType with the value to fill in the array.
 */
#define hit_blockTileAllocFill( varP, baseType, fillValue ) hit_blockTileAllocInternal( varP, sizeof(baseType), fillValue )

// @cond INTERNAL
/**
 * Internal function to allocate arrays of blocks.
 */
void hit_blockTileAllocInternal( HitBlockTile *container, size_t baseSizeExtent, void *fillValue );
// @endcond

/**
 * Free the allocated memory of an array of blocks.
 *
 * It frees the allocated memory for the array and the blocks.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in,out] container   Array of blocks tile.
 */
void hit_blockTileFree( HitBlockTile container );

/**
 * Selects a subtile of an array of block.
 *
 * Generates a new tile with a subselection of the blocks of the original array.
 *
 * @hideinitializer
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in,out]	"..."	The same parameters as hit_tileSelect()
 */
#define hit_blockTileSelect( ... ) 	hit_tileSelect( __VA_ARGS__ );

/**
 * Selects a subtile of an array of block, with possible extended boundaries.
 *
 * Generates a new tile with a subselection of the blocks of the original array.
 * It allows to select elements out of the boundaries of the original parent variable.
 *
 * @hideinitializer
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in,out]	"..."	The same parameters as hit_tileSelectNoBoundary().
 */
#define hit_blockTileSelectNoBoundary( ... ) 	hit_tileSelectNoBoundary( __VA_ARGS__ );

/**
 * Selects a subtile of an array of block, using ancestor array coordinates.
 *
 * Generates a new tile with a subselection of the blocks of the original array.
 * The selection is done in ancestor array coordinates. If the original array has
 * no ancestor (it is not a subselection of another array), the ancestor array coordinates
 * are the same as the tile coordinates.
 *
 * @hideinitializer
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in,out]	"..."	The same parameters as hit_tileSelectArrayCoords().
 */
#define hit_blockTileSelectArrayCoords( ... ) 	hit_tileSelectArrayCoords( __VA_ARGS__ );

/**
 * Selects a subtile of an array of block, using ancestor array coordinates,
 * and with possible extended boundaries.
 *
 * Generates a new tile with a subselection of the blocks of the original array.
 * It allows to select elements out of the boundaries of the original parent variable.
 * The selection is done in ancestor array coordinates. If the original array has
 * no ancestor (it is not a subselection of another array), the ancestor array coordinates
 * are the same as the tile coordinates.
 *
 * @hideinitializer
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in,out]	"..."	The same parameters as hit_tileSelectArrayCoordsNoBoundary().
 */
#define hit_blockTileSelectArrayCoordsNoBoundary( ... ) 	hit_tileSelectArrayCoordsNoBoundary( __VA_ARGS__ );

// @cond INTERNAL
/**
 * Unpadded subselection of a block.
 *
 * Arrays of blocks may need to add padding elements to the last blocks of a row or column
 * when the original domain cardinalities are not integer divisible of the original array.
 * After retrieving a block from the array of blocks, this function allows to get a
 * tile which is a subseclection of the real part of the block, without the padding elements.
 * In case the block has no padding, this function returns the input block.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @hideinitializer
 *
 * @param[in] elem	\e HitTile  Element of an array of blocks (a block tile).
 * @param[in] baseType	\e __typeName		Type of the base elements in the block.
 * @retval HitTile_<baseType> A tile with an unpadded selection of the block.
 */
#define hit_blockTileUnpadElem( elem, baseType )	(((elem).unpadded != NULL) ? *(HitTile_##baseType *)((elem).unpadded) : *(HitTile_##baseType *)&(elem))
// @endcond

/**
 * Access to a block element.
 *
 * Expression to access a block of an array of blocks. 
 *
 * Arrays of blocks may need to add padding elements to the last blocks of a row or column
 * when the original domain cardinalities are not integer divisible of the original array.
 * The block accesed is always a subselection of the desired block without the padding
 * elements if necessary. Thus, the obtained block may have less rows or columns than other
 * the blocks in the array.
 *
 * @hideinitializer
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in] tile	\e HitBlockTile 	An array of blocks.
 * @param[in] baseType \e __typeName		Type of the base elements in the blocks.
 * @param[in] ndims \e int		Number of dimensions
 * @param[in] "..."		\e int_list List with ndims integer indexes
 *
 * @sa hit_tileElemAt()
 */
#define hit_blockTileElemAt( tile, baseType, ndims, ... )	hit_blockTileUnpadElem( hit_tileElemAt(tile, ndims, __VA_ARGS__ ), baseType )

/**
 * Access to a block element in an array of blocks without stride.
 *
 * Expression to access a block of an array of blocks with no stride. This version is
 * faster than hit_blockTileElemAt().
 *
 * @deprecated
 * This macro con only be used for HitBlockCTile arrays. 
 * It is equivalent to hit_blockTileElemAt(), but faster.
 * The name will be changed and it will be moved to be a member of the HitBlockCTile class.
 * The HitBlockCTile subclass has no stride in any dimension. The domain is \e contiguous 
 * or \e compact.
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * 
 * @hideinitializer
 *
 * @param[in] tile	\e HitBlockTile 	An array of blocks.
 * @param[in] baseType \e __typeName		Type of the base elements in the blocks.
 * @param[in] ndims \e int		Number of dimensions
 * @param[in] "..."		\e int_list List with ndims integer indexes
 *
 * @sa hit_tileElemAtNoStride(), hit_blockTileElemAt()
 */
#define hit_blockTileElemAtNS( tile, baseType, ndims, ... )	hit_blockTileUnpadElem( hit_tileElemAtNoStride(tile, ndims, __VA_ARGS__ ), baseType )

/**
 * Access to a block element in an array of blocks using ancestor array coordinates.
 *
 * Expression to access a block of an array of blocks using the coordinates of the ancestor
 * array. If the array has no ancestor, these are the same as tile coordinates. 
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * 
 * @hideinitializer
 *
 * @param[in] tile	\e HitBlockTile 	An array of blocks.
 * @param[in] baseType \e __typeName		Type of the base elements in the blocks.
 * @param[in] ndims \e int		Number of dimensions
 * @param[in] "..."		\e int_list List with ndims integer indexes
 *
 * @sa hit_tileElemAtArrayCoords(), hit_blockTileElemAt()
 */
#define hit_blockTileElemAtArrayCoords( tile, baseType, ndims, ... )	hit_blockTileUnpadElem( hit_tileElemAtArrayCoords(tile, ndims, __VA_ARGS__ ), baseType )

/**
 * Access to the buffer-tile of a block element.
 *
 * Expression to access the buffer-tile of a block of an array of blocks. 
 * The buffer-tile is the tile of the block with the complete size, independently of
 * padding. This function should be used to retrieve tiles to be used in communication
 * functions that involve moving data of blocks.
 *
 * @hideinitializer
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in] tile	\e HitBlockTile 	An array of blocks.
 * @param[in] baseType \e __typeName		Type of the base elements in the blocks.
 * @param[in] ndims \e int		Number of dimensions
 * @param[in] "..."		\e int_list List with ndims integer indexes
 *
 * @sa hit_blockTileElemAt()
 */
#define hit_blockTileBufferAt( tile, baseType, ndims, ... )	( *(HitTile_##baseType *) & hit_tileElemAt(tile, ndims, __VA_ARGS__ ) )

/**
 * Access to the buffer-tile of a block element without stride.
 *
 * Expression to access the buffer-tile of a block of an array of blocks with no stride. 
 * This version is faster than hit_blockTileBufferAt().
 *
 * @hideinitializer
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in] tile	\e HitBlockTile 	An array of blocks.
 * @param[in] baseType \e __typeName		Type of the base elements in the blocks.
 * @param[in] ndims \e int		Number of dimensions
 * @param[in] "..."		\e int_list List with ndims integer indexes
 *
 * @sa hit_blockTileBufferAt(), hit_blockTileElemAtNS()
 */
#define hit_blockTileBufferAtNS( tile, baseType, ndims, ... )	( *(HitTile_##baseType *) & hit_tileElemAtNoStride(tile, ndims, __VA_ARGS__ ) )

/**
 * Access to the buffer-tile of a block element using ancestor array coordinates.
 *
 * Expression to access the buffer-tile of a block of an array of blocks 
 * using the coordinates of the ancestor array. If the array has no ancestor, 
 * these are the same as tile coordinates. 
 *
 * @hideinitializer
 *
 * @version 1.2
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in] tile	\e HitBlockTile 	An array of blocks.
 * @param[in] baseType \e __typeName		Type of the base elements in the blocks.
 * @param[in] ndims \e int		Number of dimensions
 * @param[in] "..."		\e int_list List with ndims integer indexes
 *
 * @sa hit_blockTileBufferAt(), hit_tileElemAtArrayCoords()
 */
#define hit_blockTileBufferAtArrayCoords( tile, baseType, ndims, ... )	( *(HitTile_##baseType *) & hit_tileElemAtArrayCoords(tile, ndims, __VA_ARGS__ ) )

/* UTILITIES */

/**
 * Cardinality of the blocks of an array of blocks, in one dimension.
 *
 * For a given array of blocks it returns the cardinality (number of elements) in 
 * a specific dimension of the blocks of the array.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @hideinitializer
 *
 * @param[in]	tile \e HitBlockTile  Array of blocks.
 * @param[in]	dim	 \e int   Dimension.
 * @retval		int	  Cardinality of the blocks of the array in that dimension.
 */
#define hit_blockTileDimBlockSize( tile, dim )	( tile.childSize[ dim ] )

/**
 * Parent array of a given block.
 *
 * Returns the array of blocks that contains a given block element.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[in]	tile  Block of an array of blocks.
 * @return			  The array of blocks that contains it.
 */
static inline HitBlockTile * hit_blockTileRoot( HitBlockTile * tile ) {
	while ( tile->ref != NULL && tile->ref->hierDepth > 0 ) tile = tile->ref;
	return tile;
}

// @cond INTERNAL
/**
 * Debug
 */
static inline void hit_blockTileDumpTiles( HitBlockTile container, const char *fileName ) {
	hit_dumpTileFile( container, fileName, "--- Level 1 ---" );		

	int k,l;
	hit_tileForDimDomain( container, 0, k ) {
		hit_tileForDimDomain( container, 1, l ) {
			HitTile third;
			third = hit_tileElemAt( container, 2, k, l );
			hit_dumpTileFile( third, fileName, "--- Level 2 ---" );	
			if ( third.unpadded != NULL ) {
				hit_dumpTileFile( *(third.unpadded), fileName, "--- Unpadded selection: " ); 
			}
		}
	}
}
// @endcond


/* END OF HEADER FILE _HitBlockTile_ */
#endif
