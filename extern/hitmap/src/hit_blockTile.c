/**
 * Hierarchical tiling of multi-dimensional dense array.
 * Definitions and functions to manipulate HitTile types. 
 * This type defines a handler for dense arrays and sparse structures
 * with meta-information to create and manipulate hierarchical tiles. 
 *
 * @file hit_blockTile.c
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @date Feb 2012
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

#include <math.h>
#include "hit_blockTile.h"

/* A. INITIALIZE DOMAIN */
void hit_blockTileNew( HitBlockTile *container, void *originalVar, int blockSizes[ HIT_MAXDIMS ] ) {
	HitTile *original = (HitTile *)originalVar;

	/* 1. GENERATE NEW SHAPE: COMPUTE NUMBER OF BLOCKS PER DIMENSION */	
	// ASSIGN THE ORIGINAL DOMAIN SHAPE, AND REDUCE THE SIGNATURE ON EACH DIMENSION
	HitShape blocksShape = hit_tileShape( *original );
	int dim;
	for ( dim=0; dim < hit_tileDims( *original ); dim++ ) {
		int numBlocks = (int)ceil( (double)hit_shapeSigCard( blocksShape, dim ) / blockSizes[dim] );
		hit_shapeSig( blocksShape, dim ) = hit_sigStd( numBlocks );
	}

	/* 2. CREATE CONTAINER TILE */
	hit_tileDomainShape( container, HitTile, blocksShape );

	/* 3. FILL UP CHILD-SIZES WITH BLOCK SIZES */
	for ( dim=0; dim < hit_tileDims( *original ); dim++ ) {
		container->childSize[dim] = blockSizes[dim];
	}

	/* 4. REFERENCE POINTER TO THE ORIGINAL VARIABLE */
	container->ref = (HitBlockTile *)original;

	/* 5. END */
}


/* B. ALLOC FUNCTION */
void hit_blockTileAllocInternal( HitBlockTile *container, size_t baseSizeExtent, void *fillValue  ) {

	/* 0. NULL SHAPES, SKIP */
	if ( hit_tileDims( *container ) < 1 ) return;

	/* 1. CREATE BASIC BLOCK AND FILL CONTAINER */
	/* 1.1. GENERATE A BLOCK WITH A SHAPE EXTRACTED FROM CONTAINER INFORMATION */
	HitShape blockShape = HIT_SHAPE_NULL;
	hit_shapeDimsSet( blockShape, hit_tileDims( *container ) );
	int dim;
	for ( dim=0; dim < hit_tileDims( *container ); dim++ ) {
		hit_shapeSig( blockShape, dim ) = hit_sigStd( container->childSize[dim] );
	}

	HitTile block;
	// FAKE THE UNKNOWN BASE TYPE WITH ANY FOO TYPE, CHANGE DE BASE EXTENT AFTERWARDS
	hit_tileDomainShape( &block, char, blockShape );
	block.baseExtent = baseSizeExtent;

	/* 1.2. ALLOCATE AND FILL THE BLOCK AND CONTAINER */
	hit_tileAlloc( &block );
	if ( fillValue != NULL ) hit_tileFill( &block, fillValue );
	hit_tileAlloc( container );
	hit_tileFill( container, &block );
	hit_tileFree( block );

	/* 2. PROCESS BLOCKS WITH PADDING */
	/* (Particular methods for 1D or 2D arrays. It will become obsolete in version 1.2) */

	/* 2.0. LOCATE THE ORIGINAL DOMAIN AND REFERENCE MBTILE FOR SELECTIONS */
	HitTile *original;
	HitTile *fullBlockTile = NULL;
	for ( original = (HitTile*)container; 
			original->ref != NULL; 
			original = (HitTile*)original->ref ) 
			if ( original->hierDepth != HIT_NONHIERARCHICAL ) fullBlockTile = original;

	/* 2.1. 1D-ARRAYS */
	if ( hit_tileDims( *container ) == 1 ) {
		int lastElement = hit_tileDimCard( *(original), 0 );
		int lastBlockSize = lastElement % container->childSize[0];
		if ( lastBlockSize != 0 ) {
			// IF THE LAST BLOCK IS NOT SELECTED, SKIP
			if ( hit_tileDimEnd(*container,0) == hit_tileDimEnd(*fullBlockTile,0) ) {
				// ALLOCATE NEW BLOCK AND COPY THE LAST BLOCK TO NEW ALLOCATION 
				//HitTile *oldLastBlock = (HitTile *) malloc( sizeof(HitTile) );
				//if ( oldLastBlock == NULL ) hit_errInternal(__FUNCTION__, "Allocating padded blocks", "", __FILE__, __LINE__ );
				HitTile *lastBlock = &( container->data[ container->acumCard-1 ] );
				//*oldLastBlock = *lastBlock;
				lastBlock->unpadded = (HitTile *) malloc( sizeof(HitTile) );
				if ( lastBlock->unpadded == NULL ) hit_errInternal(__FUNCTION__, "Allocating unpadded blocks", "", __FILE__, __LINE__ );

				// SELECT, PUT THE RESULT IN THE ORIGINAL POSITION, THE oldLastBlock POINTER POINTS
				// TO THE NEW ALLOCATED BLOCK WITH THE ORIGINAL BLOCK COPY
				//hit_tileSelect( lastBlock, oldLastBlock, hit_shape( 1, hit_sigStd(lastBlockSize) ) );
				hit_tileSelect( lastBlock->unpadded, lastBlock, hit_shape( 1, hit_sigStd(lastBlockSize) ) );
			}
		}
	}
	/* 2.2. 2D-ARRAYS */
	else if ( hit_tileDims( *container ) == 2 ) {
		int lastElement[2];
		int lastBlockSize[2];
		for (dim=0; dim<2; dim++) {
			lastElement[dim] = hit_tileDimCard( *(original), dim );
			lastBlockSize[dim] = lastElement[dim] % container->childSize[dim];
			if ( lastBlockSize[dim] != 0 ) {
				// IF THE LAST BLOCK IS NOT SELECTED, SKIP
				if ( hit_tileDimEnd(*container,dim) == hit_tileDimEnd(*fullBlockTile,dim) ) {
					// FOR EACH LAST BLOCK (EXCEPT CORNER BLOCK)
					int numBlock[2] = { hit_tileDimCard(*container,0)-1, hit_tileDimCard(*container,1)-1 };
					HitShape noPaddedShape = blockShape;
					hit_shapeSig( noPaddedShape, dim ) = hit_sigStd(lastBlockSize[dim]);

					// DETECT IF I HAVE THE CORNER BLOCK TO SKIP IT
					int cornerBlock = 
						hit_tileDimEnd(*container,0) == hit_tileDimEnd(*fullBlockTile,0) 
						&& hit_tileDimEnd(*container,1) == hit_tileDimEnd(*fullBlockTile,1);

					int invDim = ( dim == 0 ) ? 1:0;
					for ( numBlock[invDim]=0; 
								// LESS THAN CARDINALITY (-1 TO SKIP CORNER BLOCK IF SELECTED)
								numBlock[invDim]<hit_tileDimCard(*container, invDim)-cornerBlock; 
								numBlock[invDim]++ ) {

						// ALLOCATE NEW BLOCK AND COPY THE LAST BLOCK TO NEW ALLOCATION 
						//HitTile *oldLastBlock = (HitTile *) malloc( sizeof(HitTile) );
						//if ( oldLastBlock == NULL ) hit_errInternal(__FUNCTION__, "Allocating padded blocks", "", __FILE__, __LINE__ );
						HitTile *lastBlock = &( hit_tileGet(*container, 2, numBlock[0], numBlock[1] ) );
						//*oldLastBlock = *lastBlock;
						lastBlock->unpadded = (HitTile *) malloc( sizeof(HitTile) );
						if ( lastBlock->unpadded == NULL ) hit_errInternal(__FUNCTION__, "Allocating unpadded blocks", "", __FILE__, __LINE__ );

						// SELECT, PUT THE RESULT IN THE ORIGINAL POSITION, THE old POINTER POINTS
						// TO THE NEW ALLOCATED BLOCK WITH THE ORIGINAL BLOCK COPY
						//hit_tileSelect( lastBlock, oldLastBlock, noPaddedShape );
						hit_tileSelect( lastBlock->unpadded, lastBlock, noPaddedShape );
					}
				}
			}
			// KEEP THE BLOCK SIZE FOR USING IT ON CORNER BLOCK SHAPE
			else lastBlockSize[dim] = container->childSize[dim];
		}
		// CORNER WITH PADDING IN ANY DIMENSION
		if ( lastBlockSize[0] != 0 || lastBlockSize[1] != 0 ) {
			// IF THE LAST BLOCK IS NOT SELECTED, SKIP
			if ( hit_tileDimEnd(*container,0) == hit_tileDimEnd(*fullBlockTile,0) 
				 &&	hit_tileDimEnd(*container,1) == hit_tileDimEnd(*fullBlockTile,1) 
				) {
				// ALLOCATE NEW BLOCK AND COPY THE LAST BLOCK TO NEW ALLOCATION 
				//HitTile *oldLastBlock = (HitTile *) malloc( sizeof(HitTile) );
				//if ( oldLastBlock == NULL ) hit_errInternal(__FUNCTION__, "Allocating padded blocks", "", __FILE__, __LINE__ );
				HitTile *lastBlock = &( container->data[ container->acumCard-1 ] );
				//*oldLastBlock = *lastBlock;
				lastBlock->unpadded = (HitTile *) malloc( sizeof(HitTile) );
				if ( lastBlock->unpadded == NULL ) hit_errInternal(__FUNCTION__, "Allocating unpadded blocks", "", __FILE__, __LINE__ );

				// SELECT, PUT THE RESULT IN THE ORIGINAL POSITION, THE old POINTER POINTS
				// TO THE NEW ALLOCATED BLOCK WITH THE ORIGINAL BLOCK COPY
				HitShape noPaddedShape = 
						hit_shape( 2, hit_sigStd(lastBlockSize[0]), hit_sigStd(lastBlockSize[1]) );
				//hit_tileSelect( lastBlock, oldLastBlock, noPaddedShape );
				hit_tileSelect( lastBlock->unpadded, lastBlock, noPaddedShape );
			}
		}
	}
	else hit_warnInternal(__FUNCTION__, "MBTiles not fully implemented for more than 2 dimensions", "", __FILE__, __LINE__ );
}

/* C. FREE FULL STRUCTURE */
void hit_blockTileFree( HitBlockTile container ) {

	/* 0. SKIP NON-MEMORY-OWNER SELECTIONS */
	if ( container.memStatus != HIT_MS_OWNER ) return;

	/* 1. FOR ALL BLOCKS IN THE CONTAINER */
	int i;
	for( i=0; i<hit_tileCard( container ); i++ ) {
		HitTile *block = (HitTile *)( &( container.data[i] ) );

		/* LOCATE AND FREE MEMORY 
		 * 		PADDED BLOCK -> NOT OWNER
		 * 		NORMAL BLOCK -> OWNER */
		//if ( block->memStatus == HIT_MS_NOT_OWNER ) {
		if ( block->unpadded != NULL ) {
			//hit_tileFree( *(block->ref) );
			//free( block->ref ); // FREE ALSO THE ANCESTOR TILE STRUCTURE
			free ( block->unpadded );
		}
		//else hit_tileFree( *block );
		hit_tileFree( *block );
	}

	/* 2. FREE CONTAINER */	
	hit_tileFree( container );
}

