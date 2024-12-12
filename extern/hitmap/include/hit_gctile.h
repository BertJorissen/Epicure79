/**
 * Tiling of sparse CSR graphs.
 * Definitions and functions to manipulate gcHitTile types. 
 *
 * @file hit_gctile.h
 * @version 1.0
 * @author Javier Fresno Bausela
 * @date Nov 2011
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

#ifndef _HitGCTile_
#define _HitGCTile_


#include "hit_tile.h"
#include "hit_cshape.h"

/* 1. DECLARATION AND INITIALIZATION OF THE VARIABLES */
/**
 * Declares the shape and base type of a new sparse domain.
 * @memberof HitGCTile
 * @param newVarP A pointer to a HitTile derived type variable.
 * @param baseType The base type for the graph elements.
 * @param shape Shape of the new CSR sparse domain.
 * @param allocOpts Allocate memory for the vertices and/or the edges.
 */
#define hit_gcTileDomainShape(newVarP, baseType, shape, allocOpts) \
	hit_gcTileDomainShapeInternal(newVarP, sizeof(baseType), shape, allocOpts)

/**
 * Declares the shape and base type of a new sparse domain.
 * @memberof HitGCTile
 * @internal
 * @param newVarP		A pointer to a HitTile derived type variable.
 * @param baseExtent 	Size of the base type for the elements.
 * @param shape			Shape of the new CSR sparse domain.
 * @param allocOpts		Allocate memory for the vertices and/or the edges.
 */
void hit_gcTileDomainShapeInternal(void * newVarP, size_t baseExtent, HitShape shape, int allocOpts);

/**
 * Allocate a previously declared gcTile.
 * @memberof HitGCTile
 * @param newVarP	A pointer to the HitTile.
 */
#define hit_gcTileAlloc(var) hit_gcTileAllocInternal(var, #var, __FILE__, __LINE__)
/**
 * Allocate a previously declared gcTile.
 * @memberof HitGCTile
 * @internal
 * @param newVarP A pointer to the HitTile.
 * @param file Name of the source file.
 * @param numLine Source line.
 */
void hit_gcTileAllocInternal(void *newVarP, const char *name, const char *file, int numLine);

/**
 * Declares the shape and base type of a new CSR sparse graph and
 * allocates memory for the elements.
 * @memberof HitGCTile
 * @param var A pointer to a HitTile derived type variable.
 * @param baseType The base type for the graph elements.
 * @param shape Shape of the new graph.
 * @param allocOpts		Allocate memory for the vertices and/or the edges.
 * @see hit_gcTileDomainShape
 * @see hit_gcTileAlloc
 */
#define hit_gcTileDomainShapeAlloc(var, baseType, shape, allocOpts)	\
	hit_gcTileDomainShapeInternal(var, sizeof(baseType), shape, allocOpts); \
	hit_gcTileAllocInternal(var, #var, __FILE__, __LINE__);


/* 2. INDEX ACCESS FUNCTIONS */
/**
 * Gets the index of a sparse vertex in graph coordinates.
 * @memberof HitGCTile
 * @param var The tile pointer.
 * @param vertex The vertex coordinate.
 * @return The index element.
 */
int hit_gcTileGraphVertexAtIndex(void * varP, int vertex);

/**
 * Gets the index of a sparse element in local coordinates.
 * @memberof HitGCTile
 * @param var The tile pointer.
 * @param local1 The first coordinate.
 * @param local2 The second coordinate.
 * @return The index element.
 */
static inline int hit_gcTileEdgeAtIndex(void * varP, int local1, int local2){

	/* 1. Get the shape of the tile */
	HitTile *var = (HitTile *)varP;
	HitShape shape = hit_tileShape(*var);

	int edge;
	hit_cShapeEdgeIterator(edge,shape,local1){
		int dst = hit_cShapeEdgeTarget(shape,edge);
		if(dst == local2) return edge;
	}

	return -1;
}

/**
 * Gets the index of a sparse element in graph coordinates.
 * @memberof HitGCTile
 * @param var The tile pointer.
 * @param pos1 The first coordinate.
 * @param pos2 The second coordinate.
 * @return The index element.
 */
int hit_gcTileGraphEdgeAtIndex(void * varP, int pos1, int pos2);



/* 3. ELEMENT ACCESS FUNCTIONS */
/**
 * Gets a sparse element from a tile.
 * @memberof HitGCTile
 * @param var The tile.
 * @param vertex The vertex local coordinate.
 * @return The tile element.
 */
#define hit_gcTileVertexAt(var, vertex) ((var).dataVertices[vertex])

/**
 * Gets a edge from a tile.
 * @memberof HitGCTile
 * @param var The tile.
 * @param pos1 The first global coordinate.
 * @param pos2 The second global coordinate.
 * @return The tile element.
 */
#define hit_gcTileEdgeAt(var, pos1, pos2) ((var).data[hit_gcTileEdgeAtIndex(&(var),(pos1),(pos2))])

/**
 * Gets a sparse element from a tile defined in graph coordinates.
 * @memberof HitGCTile
 * @param var The tile.
 * @param vertex The vertex graph coordinate.
 * @return The tile element.
 */
#define hit_gcTileGraphVertexAt(var, vertex) ((var).dataVertices[hit_gcTileGraphVertexAtIndex(&(var),(vertex))])

/**
 * Gets a edge from a tile defined in graph coordinates.
 * @memberof HitGCTile
 * @param var The tile.
 * @param pos1 The first global coordinate.
 * @param pos2 The second global coordinate.
 * @return The tile element.
 */
#define hit_gcTileGraphEdgeAt(var, pos1, pos2) ((var).data[hit_gcTileGraphEdgeAtIndex(&(var),(pos1),(pos2))])


/* 4. OTHER FUNCTIONS */
/**
 * Copies a sparse tile using the globalName translation list.
 * @memberof HitGCTile
 * @param dstTileP Pointer to the destination tile.
 * @param srcTileP Pointer to the source tile.
 * @memberof HitGCTile
 */
#define hit_gcTileVerticesCopy(dstTileP,srcTileP) hit_gcTileCopyVerticesInternal(dstTileP,srcTileP,1)

/**
 * Copies a sparse tile using the globalName translation list.
 * @memberof HitGCTile
 * @internal
 * @warning It is necessary to use the clear flag if the tiles do not represent the same graph.
 * @param dstTileP Pointer to the destination tile.
 * @param srcTileP Pointer to the source tile.
 * @param clear Flag to wipe the destination tile before the copy.
 * @memberof HitGCTile
 */
void hit_gcTileCopyVerticesInternal(void * dstTileP, void * srcTileP, int clear);

/**
 * Access function to an edge element using a shape iterator.
 * @fn hit_gcTileEdgeIteratorAt(HitGBTile var, int vertex, int edge_index);
 * @memberof HitGCTile
 * @param var The tile.
 * @param vertex The vertex.
 * @param edge_index The index provided by the iterator.
 * @return The edge element in the tile.
 */
#define hit_gcTileEdgeIteratorAt(var, vertex, edge_index) ((var).data[edge_index])


// 4. OTHER FUNCTIONS  
/**
 * Clears the values of the vertices.
 * @memberof HitGCTile
 * @param varP A tile.
 */
void hit_gcTileClearVertices(void * varP);

/**
 * Copies the values from a tile to the other.
 * @memberof HitGCTile
 * @param destP Destination tile.
 * @param srcP Soruce tile
 */
void hit_gcTileCopyVertices(void * destP, void * srcP);



/* END OF HEADER FILE _HitGCTile_ */
#endif
