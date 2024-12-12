/**
 * Shapes for bitmap sparse domains.
 *
 * Types and functions to manipulate sparse domains stored using a bitmap to locate
 * the elements that are zero or non-zero.
 *
 * @file hit_bshape.h
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

#ifndef _HitBShape_
#define _HitBShape_


#include "hit_shape.h"

/** Constant for a Bitmap Sparse Matrix. */
#define HIT_BSHAPE_MATRIX 0
/** Constant for a Bitmap Sparse Graph. */
#define HIT_BSHAPE_GRAPH  1

/**
 * Null value for bitmap sparse shapes.
 */
extern HitShape HIT_BITMAP_SHAPE_NULL;
/** @cond INTERNAL */
/**
 *  Null internal value for bitmap sparse shapes.
 */
#define HIT_BITMAP_SHAPE_INTERNAL_NULL_STATIC { {0, 0}, 0, NULL, {HIT_NAMELIST_NULL_STATIC,HIT_NAMELIST_NULL_STATIC} }
/** Null static value for bitmap sparse shapes. */
// @author javfres: C++ do not support this kind of struct initialization.
// @author arturo: Solution, do the initialization in two assignments in the .c file. Done.
#ifdef __cplusplus
extern "C" {
#else
#define HIT_BITMAP_SHAPE_NULL_STATIC { HIT_BITMAP_SHAPE, { .bitmap = HIT_BITMAP_SHAPE_INTERNAL_NULL_STATIC } }
#endif
/** @endcond */

/** @cond INTERNAL */
/** Index of a bitmap element */
#define hit_bitmapShapeIndex(b) 	(((size_t)(b))/HIT_BITMAP_SIZE)
/** Offset of a bitmap element */
#define hit_bitmapShapeOffset(b)	(((size_t)(b))%HIT_BITMAP_SIZE)
/** @endcond */

/** @cond INTERNAL */
/** Bitmap type with a 1 in its first bit */
#define HIT_BITMAP_1 (((HIT_BITMAP_TYPE) 1) << (HIT_BITMAP_SIZE-1))
/**
 * Debug
 */
static inline const char * hit_bitmap_tostring(HIT_BITMAP_TYPE element){

	static char buffer[HIT_BITMAP_SIZE+1];
	buffer[HIT_BITMAP_SIZE] = '\0';

	HIT_BITMAP_TYPE mask = HIT_BITMAP_1;
	HIT_BITMAP_TYPE i;
	for(i=0;i<HIT_BITMAP_SIZE;i++){

		if( (mask & element) == 0 ){
			buffer[i] = '0';
		} else{
			buffer[i] = '1';
		}
		mask = mask >> 1;
	}
	return buffer;
}
/** @endcond */

/* 1. Hit Bitmap Shape generating functions */
/**
 * Creates a new Bitmap shape with n vertices.
 * This first vertex name is 0.
 * @memberof HitBShape
 * @param nvertices Number of vertices.
 * @return The new bitmap sparser shape.
 */
HitShape hit_bitmapShape(int nvertices);

/**
 * Sparse Bitmap matrix shape constructor.
 *
 * @param n Number of rows
 * @param m Number of columns
 * @return the new bitmap sparse Shape.
 */
HitShape hit_bitmapShapeMatrix(int n, int m);

/**
 * Hit Bitmap Shape destructor.
 * @memberof HitBShape
 * @param shape A Hit Bitmap Shape.
 */
void hit_bShapeFree(HitShape shape);






/* 2 Hit Bitmap Shape access macros */
/**
 * Get the cardinality (number of row or columns) of the Bitmap matrix.
 * @param shape A HitBShape.
 * @param dim The dimension (0 for rows, 1 for columns).
 * @return The cardinality
 */
#define hit_bShapeCard(shape, dim) (hit_bShapeAccess(shape).cards[(dim)])

/**
 * Get the number of vertices from a bitmap shape.
 * @memberof HitBShape
 * @fn int hit_bShapeNvertices(HitShape shape)
 * @param shape A bitmap shape.
 * @return The number of vertices.
 */
#define hit_bShapeNvertices(shape) (hit_bShapeCard(shape, 0))

/**
 * Get the number of edges from a bitmap shape.
 * @memberof HitBShape
 * @fn int hit_bShapeNedges(HitShape shape)
 * @param shape A bitmap shape.
 * @return The number of edges.
 */
#define hit_bShapeNedges(shape) (hit_bShapeAccess(shape).nz)


/**
 * Returns the name list for the given dimension.
 * @param shape A HitBShape
 * @param dim The dimension (0 for rows, 1 for columns).
 * @return The name list
 */
#define hit_bShapeNameList(shape,dim) (hit_bShapeAccess((shape)).names[(dim)])

/**
 * Get the data array with the bitmap.
 * @memberof HitBShape
 * @fn HIT_BITMAP_TYPE* hit_bShapeDate(HitShape shape)
 * @param shape A bitmap shape.
 * @return The bitmap data array.
 */
#define hit_bShapeData(shape) (hit_bShapeAccess(shape).data)



/**
 * Converts a local coordinate to global.
 * @param s A HitBShape.
 * @param dim the dimension (0 or 1)
 * @param elem The element using local coordinates.
 * @return The global coordinate.
 */
#define hit_bShapeCoordToGlobal(s,dim,elem) (hit_nameListIndex2Name(hit_bShapeNameList((s),(dim)),(elem)))


/**
 * Converts a global coordinate to local.
 * @param s A HitBShape.
 * @param dim the dimension (0 or 1)
 * @param elem The element using global coordinates.
 * @return The local coordinate.
 */
#define hit_bShapeCoordToLocal(s,dim,elem)  (hit_nameListName2Index(hit_bShapeNameList((s),(dim)),(elem)))



/**
 * Returns the target vertex of an edge.
 * @memberof HitBShape
 * @fn int hit_bShapeEdgeTarget(HitShape s, int edge)
 * @param s A HitBShape.
 * @param edge An edge.
 * @return A vertex.
 */
#define hit_bShapeEdgeTarget(s,edge) (edge)

/**
 * Returns the target vertex of an edge, it work when using the Skip iterator.
 * @memberof HitBShape
 * @fn int hit_bShapeEdgeTargetSkip(HitShape s, int edge)
 * @param s A HitBShape.
 * @param edge An edge.
 * @return A vertex.
 */
#define hit_bShapeEdgeTargetSkip(s,edge) (edge%hit_bShapeCard(s,1))


/* 3. Hit Bitmap Sparse Shape operations */
/**
 * Return the value of a bitmap element.
 * @memberof HitBShape
 * @fn int hit_bShapeGet(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 * @return 0 if the element (i,j) is not set, !=0 otherwise.
 */
#define hit_bShapeGet(bitshape,i,j) \
	(hit_bShapeData(bitshape)[hit_bitmapShapeIndex((i)*hit_bShapeCard(bitshape,1)+(j))] \
	& \
	((HIT_BITMAP_1) >> hit_bitmapShapeOffset((i)*hit_bShapeCard(bitshape,1)+(j))))
	
/**
 * Sets the value of a bitmap element to 1.
 * @memberof HitBShape
 * @fn void hit_bShapeSet(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 */
#define hit_bShapeSet(bitshape,i,j) \
	(hit_bShapeData(bitshape)[hit_bitmapShapeIndex((i)*hit_bShapeCard(bitshape,1)+(j))] \
	|= \
	(HIT_BITMAP_TYPE) \
	((HIT_BITMAP_1) >> hit_bitmapShapeOffset((i)*hit_bShapeCard(bitshape,1)+(j))))

/** 
 * Sets the value of a bitmap element to 0.
 * @memberof HitBShape
 * @fn void hit_bShapeClear(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 */
#define hit_bShapeClear(bitshape,i,j) \
	(hit_bShapeData(bitshape)[hit_bitmapShapeIndex((i)*hit_bShapeCard(bitshape,1)+(j))] \
	&= \
	(HIT_BITMAP_TYPE) \
	~((HIT_BITMAP_1) >> hit_bitmapShapeOffset((i)*hit_bShapeCard(bitshape,1)+(j))))
	
/**
 * Sets the value of a bitmap element and its symmetric to 1.
 * @memberof HitBShape
 * @fn void hit_bShapeSet2(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 */
#define hit_bShapeSet2(bitshape,i,j) {hit_bShapeSet(bitshape,i,j); hit_bShapeSet(bitshape,j,i);}

/**
 * Return the value of a bitmap element using global coordinates.
 * @memberof HitBShape
 * @fn int hit_bShapeGetGlobal(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 * @return 0 if the element (i,j) is not set, !=0 otherwise.
 */
#define hit_bShapeGetGlobal(bitshape,i,j) \
	(hit_bShapeData(bitshape)[hit_bitmapShapeIndex((hit_bShapeCoordToLocal(bitshape,0, (i)))* \
	hit_bShapeCard(bitshape,1)+(hit_bShapeCoordToLocal(bitshape,1, (j))))] \
	& \
	((HIT_BITMAP_1) >> hit_bitmapShapeOffset((hit_bShapeCoordToLocal(bitshape,0, (i)))* \
	hit_bShapeCard(bitshape,1)+(hit_bShapeCoordToLocal(bitshape,1, (j))))))

/**
 * Sets the value of a bitmap element to 1 in global coordinates.
 * @memberof HitBShape
 * @fn void hit_bShapeSetGlobal(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 */
#define hit_bShapeSetGlobal(bitshape,i,j) \
	(hit_bShapeData(bitshape)[hit_bitmapShapeIndex((hit_bShapeCoordToLocal(bitshape,0, (i)))* \
	hit_bShapeCard(bitshape,1)+(hit_bShapeCoordToLocal(bitshape,1, (j))))] \
	|= \
	(HIT_BITMAP_TYPE) \
	((HIT_BITMAP_1) >> hit_bitmapShapeOffset((hit_bShapeCoordToLocal(bitshape,0, (i)))* \
	hit_bShapeCard(bitshape,1)+(hit_bShapeCoordToLocal(bitshape,1, (j))))))

/** 
 * Sets the value of a bitmap element to 0 in global coordinates.
 * @memberof HitBShape
 * @fn void hit_bShapeClearGlobal(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 */
#define hit_bShapeClearGlobal(bitshape,i,j) \
	(hit_bShapeData(bitshape)[hit_bitmapShapeIndex((hit_bShapeCoordToLocal(bitshape,0, (i)))* \
	hit_bShapeCard(bitshape,1)+(hit_bShapeCoordToLocal(bitshape,1, (j))))] \
	&= ~ \
	(HIT_BITMAP_TYPE) \
	((HIT_BITMAP_1) >> hit_bitmapShapeOffset((hit_bShapeCoordToLocal(bitshape,0, (i)))* \
	hit_bShapeCard(bitshape,1)+(hit_bShapeCoordToLocal(bitshape,1, (j))))))

/**
 * Sets the value of a bitmap element and its symmetric to 1 in global coordinates.
 * @memberof HitBShape
 * @fn void hit_bShapeSetGlobal2(HitShape bitshape, int i, int j)
 * @param bitshape A BitmapShape.
 * @param i First coordinate.
 * @param j Second coordinate.
 */
#define hit_bShapeSetGlobal2(bitshape,i,j) {hit_bShapeSetGlobal(bitshape,i,j); hit_bShapeSetGlobal(bitshape,j,i);}

/**
 * Translates a vertex in the local domain (start at 0) to
 * the global domain of vertices.
 * @fn int hit_bShapeVertexToGlobal(HitShape s, int vertex)
 * @memberof HitBShape
 * @param s The BShape.
 * @param vertex The local vertex.
 * @return A global vertex.
 */
#define hit_bShapeVertexToGlobal(s,vertex) (hit_nameListIndex2Name(hit_bShapeNameList(s,0),vertex))

/**
 * Translates to the local coordinates.
 * @memberof HitBShape
 * @param s The BShape.
 * @param vertex The global vertex.
 * @return A local vertex.
 */
#define hit_bShapeVertexToLocal(s,vertex) (hit_nameListName2Index(hit_bShapeNameList(s,0),vertex))


/**
 * Adds the vertex x.
 * @memberof HitBShape
 * @param shape The BShape pointer of the graph.
 * @param x A node (in global coordinates).
 */
#define hit_bShapeAddVertex(shapeP, x) hit_bShapeAddEmptyRow_or_Vertex(shapeP, x, HIT_BSHAPE_GRAPH)

/**
 * Adds an empty row in a bitmap sparse matrix or a vertex in a graph.
 * @param shape The shape.
 * @param x The name of the row or vertex.
 * @param mode The mode matrix or graph.
 */
void hit_bShapeAddEmptyRow_or_Vertex(HitShape * shape, int x, int mode);



/**
 * Create a new bitmap shape selection several vertices.
 * @memberof HitBShape.
 * @param shape A BShape.
 * @param nvertices The number of vertices selected.
 * @param vertices The list of vertices.
 * @return The selection.
 */
HitShape hit_bShapeSelect(HitShape shape, int nvertices, int * vertices);

/**
 * Expand a Bitmap Sparse Shape adding new vertices of the original graph.
 * @memberof HitBShape
 * @param shape The shape to expand.
 * @param original The original shape.
 * @param amount The number of times that the expand algorithm is invoked.
 * @return The new expanded shape.
 */
HitShape hit_bShapeExpand(HitShape shape, HitShape original, int amount);

/**
 * Creates the inverse translation list of vertex names.
 * This list is used to speedup the translation operation from global to local names.
 * This function will increase the size of the shape.
 * @memberof HitBShape
 * @param shape A pointer to HitBShape.
 */
void hit_bShapeCreateInvNames(HitShape * shape);

/**
 * Adds the vertex x.
 * @memberof HitBShape
 * @param shape The BShape pointer of the graph.
 * @param x A node (in global coordinates).
 */
//void hit_bShapeAddVertex(HitShape * shape, int x);

/**
 * Adds the edge x->y.
 * @memberof HitBShape
 * @note: Create the nodes if it is necessary.
 * @param shape The BShape pointer of the graph.
 * @param x A node (in global coordinates).
 * @param y A node (in global coordinates).
 */
#define hit_bShapeAddEdge(shapep, x, y) hit_bShapeAddElem_or_Edge(shapep, x, y, HIT_BSHAPE_GRAPH)


/**
 * Adds the edge x->y and y->x
 * @memberof HitBShape
 * @note: It creates the nodes if it is necessary.
 * @param shape The BShape pointer of the graph.
 * @param x A node (in global coordinates).
 * @param y A node (in global coordinates).
 */
#define hit_bShapeAddEdge2(shape,x,y) {hit_bShapeAddEdge(shape,x,y); hit_bShapeAddEdge(shape,y,x);}


/* 4. Hit Sparse Bitmap Shape Iterators */

/**
 * Row iterator.
 * @memberof HitBShape
 * @note row variable is in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 */
#define hit_bShapeRowIterator(var,shape) \
	for(var=0; var<hit_bShapeCard(shape, 0); var++)


/**
 * Vertex iterator.
 * @memberof HitBShape
 * @note vertex variable is in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 */
#define hit_bShapeVertexIterator(var,shape) hit_bShapeRowIterator(var,shape)


/**
 * Column iterator from a given row.
 * @memberof HitBShape
 * @note column and row variables are in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 * @param row The row
 */
#define hit_bShapeColumnIterator(var,shape,row) \
for(var=hit_bShapeEdgeIteratorNextInternal(shape, -1, row);var<hit_bShapeCard(shape,1);var=hit_bShapeEdgeIteratorNextInternal(shape, var, row))



/**
 * Column iterator from a given row for the Skip iterator
 * @memberof HitBShape
 * @note column and row variables are in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 * @param row The row
 */
#define hit_bShapeColumnIteratorSkip(var,shape,row) \
for(var=hit_bShapeEdgeIteratorNextInternalSkip(shape, row * hit_bShapeCard(shape,1));var<(row+1) * hit_bShapeCard(shape,1);var = hit_bShapeEdgeIteratorNextInternalSkip(shape, var+1))
#define hit_bShapeColumnIteratorDense(var,shape,row) \
for(var=0;var<hit_bShapeCard(shape,1);var++)



/**
 * Edge iterator from a given vertex.
 * @memberof HitBShape
 * @note vertex and edges variables are in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 * @param vertex The vertex
 */
#define hit_bShapeEdgeIterator(var,shape,vertex) hit_bShapeColumnIterator(var,shape,vertex)

/**
 * Edge iterator from a given vertex using the Skip iterator
 * @memberof HitBShape
 * @note vertex and edges variables are in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 * @param vertex The vertex
 */
#define hit_bShapeEdgeIteratorSkip(var,shape,vertex) hit_bShapeColumnIteratorSkip(var,shape,vertex)

/**
 * Edge iterator from a given vertex using the Dense iterator
 * @memberof HitBShape
 * @note vertex and edges variables are in local coordinates.
 * @param var Loop variable
 * @param shape The BShape
 * @param vertex The vertex
 */
#define hit_bShapeEdgeIteratorDense(var,shape,vertex) hit_bShapeColumnIteratorDense(var,shape,vertex)


/**
 * Obtains the new value of the variable used in the iterator. This variable
 * is an index that is used by the other iterator macros.
 * @internal
 * @memberof HitBShape
 * @param shape A HitBShape
 * @param var The variable of the iterator.
 * @param vertex The vertex.
 * @return The next variable value.
 */
// @todo @javfres Possible improvement: Speedup this function comparing the whole byte.
static inline int hit_bShapeEdgeIteratorNextInternal(HitShape shape, int var, int vertex){
	//printf("--- row(%d) looking for next column (%d) \n",vertex,var);
	do{
		var++;
		if(var == hit_bShapeCard(shape,1)){
			//printf("--- exit because card ended (%d) \n",hit_bShapeCard(shape,1));
			break;
		}
	} while(hit_bShapeGet(shape,vertex,var)==0);

	//printf("--- next column is (%d) \n",var);
	return var;
}


// This works different form the other iterator, this uses the bitmap index as var
static inline int hit_bShapeEdgeIteratorNextInternalSkip(HitShape shape, int var){

	size_t i;

	// 1. Get the current index of the element and the offset of the bit in the element
	size_t xind = hit_bitmapShapeIndex(var);
	size_t xoff = hit_bitmapShapeOffset(var);

	//printf("xind %d, xoff %d\n",xind,xoff);

	// 2. Check if there is a bit his the current element (byte, or bytes)
	HIT_BITMAP_TYPE element = hit_bShapeData(shape)[xind];
	//printf("Element %s\n",hit_bitmap_tostring(element));
	HIT_BITMAP_TYPE mask = HIT_BITMAP_1 >> xoff;
	for(i=0;i<HIT_BITMAP_SIZE-xoff;i++){
		if( (mask & element) != 0 ){
			return var + (int) i; //@todo maybe this should return size_t types.
		}
		mask >>= 1;
	}

	// 3. There is no 1 bits in the current element, find the next element that have 1s.
	xind ++;
	while( hit_bShapeData(shape)[xind] == 0 ){
		xind++;
	}

	// 4. We do the same as 2. to get the bit location
	element = hit_bShapeData(shape)[xind];
	mask = HIT_BITMAP_1;
	for(i=0;i<HIT_BITMAP_SIZE;i++){
		if( (mask & element) != 0 ){
			return (int) (xind * HIT_BITMAP_SIZE + i);
		}
		mask >>= 1;
	}


	// This cannot occur
	return -1;
}











/**
 * Adds an element to a matrix or a edge to a graph.
 * @param shape The shape
 * @param x The row/vertex
 * @param y The element/edge
 * @param mode The matrix/graph mode.
 */
void hit_bShapeAddElem_or_Edge(HitShape * shape, int x, int y, int mode);


/**
 * Adds an element to a matrix.
 * @param shape The shape
 * @param x The row
 * @param y The element
 */
#define hit_bShapeAddElem(shapep, x, y) hit_bShapeAddElem_or_Edge(shapep, x, y, HIT_BSHAPE_MATRIX)


/**
 * Returns a new bitmap shape with a selection of rows.
 * @param shape The source shape.
 * @param nNames The number of rows.
 * @param names The list with the names of the rows.
 * @return A new bitmap shape.
 */
HitShape hit_bShapeSelectRows(HitShape shape, int nNames, int * names);


/**
 * Return the number of nonzero elements of a row.
 * @param shape A bitmap shape.
 * @param row The index of the row.
 * @return The number of non-zero elements.
 */
int hit_bShapeNColsRow(HitShape shape, int row);


/**
 * Vertex iterator in a bitmap shape
 * @param var Variable to iterate
 * @param shape Bitmap shape
 */
#define hit_bitmapShapeVertexIterator(var,shape) for(var=0; var<hit_bitmapShapeNvertices(shape); var++)
#ifdef __cplusplus
}
#endif


/* END OF HEADER FILE _HitBShape_ */
#endif


