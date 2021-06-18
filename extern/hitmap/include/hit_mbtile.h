/**
 * Tiling of sparse Bitmap matrices.
 * Definitions and functions to manipulate mbHitTile types.
 *
 * @file hit_mbTile.h
 * @version 1.0
 * @author Javier Fresno Bausela
 * @date Nov 2011
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

#ifndef _HitMBTile_
#define _HitMBTile_


#include "hit_tile.h"


/* 1. DECLARATION AND INITIALIZATION OF THE VARIABLES */
/**
 * Declares the shape and base type of a new sparse domain.
 * @memberof HitMBTile
 * @param newVarP A pointer to a HitTile derived type variable.
 * @param baseType The base type for the graph elements.
 * @param shape Shape of the new CSR sparse domain.
 * @param allocOpts Allocate memory for the vertices and/or the edges.
 */
#define hit_mbTileDomainShape(newVarP, baseType, shape) \
	hit_mbTileDomainShapeInternal(newVarP, sizeof(baseType), shape)

/**
 * Declares the shape and base type of a new sparse domain.
 * @memberof HitMBTile
 * @internal
 * @param newVarP		A pointer to a HitTile derived type variable.
 * @param baseExtent 	Size of the base type for the elements.
 * @param shape			Shape of the new bitmap sparse domain.
 */
void hit_mbTileDomainShapeInternal(void * newVarP, size_t baseExtent, HitShape shape);


/**
 * Allocate a previously declared mbTile.
 * @memberof HitMBTile
 * @param newVarP	A pointer to the HitTile.
 */
#define hit_mbTileAlloc(var) hit_mbTileAllocInternal(var, #var, __FILE__, __LINE__)


/**
 * Allocate a previously declared mbTile.
 * @memberof HitMBTile
 * @internal
 * @param newVarP A pointer to the HitTile.
 * @param file Name of the source file.
 * @param numLine Source line.
 */
void hit_mbTileAllocInternal(void *newVarP, const char *name, const char *file, int numLine);

/**
 * Declares the shape and base type of a new Bitmap sparse matrix and
 * allocates memory for the elements.
 * @memberof HitMBTile
 * @param var A pointer to a HitTile derived type variable.
 * @param baseType The base type for the graph elements.
 * @param shape Shape of the new graph.
 * @param allocOpts		Allocate memory for the vertices and/or the edges.
 * @see hit_mbTileDomainShape
 * @see hit_mbTileAlloc
 */
#define hit_mbTileDomainShapeAlloc(var, baseType, shape)	\
	hit_mbTileDomainShapeInternal(var, sizeof(baseType), shape); \
	hit_mbTileAllocInternal(var, #var, __FILE__, __LINE__);




/* 2. INDEX ACCESS FUNCTIONS */
/**
 * Gets the index of a sparse element in local coordinates.
 * @memberof HitMBTile
 * @param var The tile pointer.
 * @param local1 The first coordinate.
 * @param local2 The second coordinate.
 * @return The index element.
 */
int hit_mbTileElemAtIndex(void * varP, int local1, int local2);

/**
 * Gets the index of a sparse element in global coordinates.
 * @memberof HitMBTile
 * @param var The tile pointer.
 * @param local1 The first coordinate.
 * @param local2 The second coordinate.
 * @return The index element.
 */
int hit_mbTileGraphElemAtIndex(void * varP, int pos1, int pos2);



/* 3. ELEMENT ACCESS FUNCTIONS */
/**
 * Gets a edge from a tile.
 * @memberof HitMBTile
 * @param var The tile.
 * @param pos1 The first global coordinate.
 * @param pos2 The second global coordinate.
 * @return The tile element.
 */
#define hit_mbTileElemAt(var, pos1, pos2) ((var).data[hit_bTileElemAtIndex(&(var),(pos1),(pos2))])

/**
 * Gets a edge from a tile defined in graph coordinates.
 * @memberof HitMBTile
 * @param var The tile.
 * @param pos1 The first global coordinate.
 * @param pos2 The second global coordinate.
 * @return The tile element.
 */
#define hit_mbTileGraphElemAt(var, pos1, pos2) ((var).data[hit_mbTileGraphElemAtIndex(&(var),(pos1),(pos2))])


/**
 * Gets a element from the bitmap matrix tile when using an iterator.
 * @param var Tile bitmap variable.
 * @param iterX Variable iterator x.
 * @param iterY Variable iterator y.
 */
#define hit_mbTileElemIteratorAt(var, iterX, iterY) \
	((var).data[(iterX)*(hit_bShapeCard(hit_tileShape(var),1))+(iterY)])



// 4. OTHER FUNCTIONS  
/**
 * Cleans a Bitmap Tile Matrix.
 * @param tileP The Tile.
 */
void hit_mbTileClear(void * tileP);



/* END OF HEADER FILE _HitMBTile_ */
#endif
