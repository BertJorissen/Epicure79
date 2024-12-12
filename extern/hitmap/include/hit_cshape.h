/**
 * Types and functions to manipulate CSR sparse domains.
 *
 * @file hit_cshape.h
 * @version 1.0
 * @author Javier Fresno Bausela
 * @date June 2012
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

#ifndef _HitCShape_
#define _HitCShape_

#include "hit_shape.h"


/** Constant for a CSR Sparse Matrix. */
#define HIT_CSHAPE_MATRIX 0
/** Constant for a CSR Sparse Graph. */
#define HIT_CSHAPE_GRAPH  1

/**
 * Null value for sparse domain shapes.
 */
extern HitShape HIT_CSR_SHAPE_NULL;

// @cond INTERNAL
/** Null internal value for CSR sparse domain shapes. */
#define HIT_CSR_SHAPE_INTERNAL_NULL_STATIC { {0,0}, NULL, NULL, {HIT_NAMELIST_NULL_STATIC,HIT_NAMELIST_NULL_STATIC} }
/** Null static value for CSR sparse domain shapes. */
// @note 
// @author javfres: C++ do not support this kind of struct initialization.
// @author arturo: Solution, do the initialization in two assignments in the .c file. Done.
#ifdef __cplusplus
#else
#define HIT_CSR_SHAPE_NULL_STATIC { HIT_CSR_SHAPE, {.csr = HIT_CSR_SHAPE_INTERNAL_NULL_STATIC} }
#endif
// @endcond


/* 1. Hit CSR Sparse Shape generating functions */
/**
 * Sparse CSR graph shape constructor.
 *
 * @memberof HitCShape
 * @param nvertices number of vertices.
 * @param nedges number of edges.
 * @return the new CSR Sparse Shape.
 */
HitShape hit_csrShape(int nvertices, int nedges);

/**
 * Sparse CSR matrix shape constructor.
 *
 * @param n Number of rows
 * @param m Number of columns
 * @param nz Number of nonzero elements.
 * @return the new CSR Sparse Shape.
 */
HitShape hit_csrShapeMatrix(int n, int m, int nz);

/**
 * Hit CSR Shape destructor.
 * @memberof HitCShape
 * @param shape A Hit CSR Shape.
 */
void hit_cShapeFree(HitShape shape);



/* 2. Hit Sparse Shape access macros */
/**
 * Get the cardinality (number of row or columns) of the CSR matrix.
 * @param shape A HitCShape.
 * @param dim The dimension (0 for rows, 1 for columns).
 * @return The cardinality
 */
#define hit_cShapeCard(shape, dim) (hit_cShapeAccess(shape).cards[(dim)])


/**
 * Get the cardinality (number of vertices) of the CSR graph.
 * @memberof HitCShape
 * @fn int hit_cShapeNvertices(HitShape shape)
 * @param shape A HitCShape.
 * @return The number of vertices of the CSR.
 */
#define hit_cShapeNvertices(shape) (hit_cShapeCard(shape, 0))


/**
 * Get the number of non zero elements.
 * @param shape A HitCShape
 * @return
 */
#define hit_cShapeNZElems(shape) (hit_cShapeXadj((shape))[hit_cShapeNvertices(shape)])


/**
 * Get the number of edges in the graph.
 * @memberof HitCShape
 * @fn int hit_cShapeNedges(HitShape shape)
 * @param shape A HitCShape.
 * @return The number of edges of the CSR.
 */
#define hit_cShapeNedges(shape) hit_cShapeNZElems(shape)

/**
 * Get the indices to adjncy from each node.
 * @memberof HitCShape
 * @fn int* hit_cShapeXadj(HitShape shape)
 * @param shape A HitCShape.
 * @return The Xadj array.
 */
#define hit_cShapeXadj(shape) (hit_cShapeAccess(shape).xadj)

/**
 * Get the edges of a node to other nodes.
 * @memberof HitCShape
 * @fn int* hit_cShapeAdjncy(HitShape shape)
 * @param shape A HitCShape.
 * @return The Adjncy array.
 */
#define hit_cShapeAdjncy(shape) (hit_cShapeAccess(shape).adjncy)

/**
 * Returns the name list for the given dimension.
 * @param shape A HitCShape
 * @param dim The dimension (0 for rows, 1 for columns).
 * @return The name list
 */
#define hit_cShapeNameList(shape,dim) (hit_cShapeAccess((shape)).names[(dim)])

/**
 * Checks if a element exists in a HitCShape
 * @param shape A HitCShape.
 * @param x The row index.
 * @param y The col index.
 * @return 1 if the element exists, 0 otherwise.
 */
int hit_cShapeElemExists(HitShape shape, int x, int y);


/**
 * Returns the fist column index from a given row.
 * @memberof HitCShape
 * @fn int hit_cShapeFistColumn(HitShape s, int row)
 * @param s A HitCShape.
 * @param row A row in local coordinates.
 * @return the first column index.
 */
#define hit_cShapeFistColumn(s, row) (hit_cShapeXadj(s)[(row)])

/**
 * Returns the last column from a given row.
 * @memberof HitCShape
 * @fn int hit_cShapeLastColumn(HitShape s, int row)
 * @param s A HitCShape.
 * @param row A row in local coordinates.
 * @return The last column index.
 */
#define hit_cShapeLastColumn(s, row) (hit_cShapeXadj(s)[(row)+1])

/**
 * Returns the number of edges from a given vertex.
 * @memberof HitCShape
 * @fn int hit_cShapeNEdgesFromVertex(HitShape s, int vertex)
 * @param s A CShape.
 * @param vertex A vertex.
 * @return The number of edges.
 */
#define hit_cShapeNEdgesFromVertex(s, vertex) (hit_cShapeLastColumn(s, vertex) - hit_cShapeFistColumn(s, vertex))

/**
 * Number of columns in a row.
 * @param s The shape
 * @param row The row
 * @return Number of columns
 */
#define hit_cShapeNColsRow(s, row) hit_cShapeNEdgesFromVertex(s, row)

/**
 * Returns the target vertex of an edge.
 * @memberof HitCShape
 * @fn int hit_cShapeEdgeTarget(HitShape s, int edge)
 * @param s A HitCShape.
 * @param edge An edge.
 * @return A vertex.
 */
#define hit_cShapeEdgeTarget(s,edge) (hit_cShapeAdjncy(s)[(edge)])


/* 3. Hit CSR Sparse Shape operations */
/**
 * Translates a vertex in the local domain (start at 0) to
 * the global domain of vertices.
 * @fn int hit_cShapeVertexToGlobal(HitShape s, int vertex)
 * @memberof HitCShape
 * @param s The CShape.
 * @param vertex The local vertex.
 * @return A global vertex.
 */
#define hit_cShapeVertexToGlobal(s,vertex) (hit_nameListIndex2Name(hit_cShapeNameList(s,0),vertex))


/**
 * Translates a vertex in the global domain to the local domain.
 * @memberof HitCShape
 * @param s the CShape
 * @param vertex the global vertex.
 * @return A local vertex.
 */
#define hit_cShapeVertexToLocal(s,vertex) (hit_nameListName2Index(hit_cShapeNameList(s,0),vertex))


/**
 * Converts a local coordinate to global.
 * @param s A HitCShape.
 * @param dim the dimension (0 or 1)
 * @param elem The element using local coordinates.
 * @return The global coordinate.
 */
#define hit_cShapeCoordToGlobal(s,dim,elem) (hit_nameListIndex2Name(hit_cShapeNameList((s),(dim)),(elem)))


/**
 * Converts a global coordinate to local.
 * @param s A HitCShape.
 * @param dim the dimension (0 or 1)
 * @param elem The element using global coordinates.
 * @return The local coordinate.
 */
#define hit_cShapeCoordToLocal(s,dim,elem)  (hit_nameListName2Index(hit_cShapeNameList((s),(dim)),(elem)))




/**
 * Checks if the sparse shape has a vertex.
 * @memberof HitCShape
 * @fn int hit_cShapeHasVertex(HitShape s, int v)
 * @param s The CShape.
 * @param v The vertex.
 * @return 0 if the shape do not have the vertex, other value otherwise.
 */
#define hit_cShapeHasVertex(s,v) (hit_cShapeVertexToLocal(s, v) != -1)

/**
 * Sparse Shape selection.
 * Makes a sparse shape with the selected vertices of the source shape.
 * @memberof HitCShape
 * @param s The source sparse shape.
 * @param nvertices The number of selected vertices.
 * @param vertices An array with the selected vertices in the global domain.
 * @return a New CShape.
 */
HitShape hit_cShapeSelect(HitShape s, int nvertices, int * vertices);


 /**
  * Sparse Shape Matrix selection.
  * Makes a sparse shape with the selected rows of the source shape.
  * @memberof HitCShape
  * @param s The source sparse shape.
  * @param n The number of selected rows.
  * @param names Selected name list in global coordinates.
  * @return a New CShape.
  *
  */
HitShape hit_cShapeSelectRows(HitShape shape, int n, int * names);


/**
 * Expand a CSR Sparse Shape adding new vertices of the original graph.
 * @memberof HitCShape
 * @param shape The shape to expand.
 * @param original The original shape.
 * @param amount The number of times that the expand algorithm is invoked.
 * @return The new expanded shape.
 */
HitShape hit_cShapeExpand(HitShape shape, HitShape original, int amount);

/**
 * Vertex reorder.
 * This function transform the local domain of a sparse shape according to a
 * new order of global vertex names.
 * @memberof HitCShape
 * @param s A CShape.
 * @param newNames An array with the new order of vertices using the global domain.
 * @return A new CShape reordered.
 */
HitShape hit_cShapeReorder(HitShape s, int * newNames);

/**
 * Adds the vertex x.
 * @memberof HitCShape
 * @param shape The CShape pointer of the graph.
 * @param x A vertex (in global coordinates).
 */
#define hit_cShapeAddVertex(shapeP, x) hit_cShapeAddEmptyRow_or_Vertex(shapeP, x, HIT_CSHAPE_GRAPH)

/**
 * Adds a empty row in a matrix or a vertex in a graph.
 * @memberof HitCShape
 * @param shape The CShape pointer of the graph.
 * @param x A row/vertex (in global coordinates).
 */
void hit_cShapeAddEmptyRow_or_Vertex(HitShape * shape, int x, int mode);

/**
 * Adds the edge x->y.
 * @memberof HitCShape
 * @note: Create the nodes if it is necessary.
 * @param shape The CShape pointer of the graph.
 * @param x A node (in global coordinates).
 * @param y A node (in global coordinates).
 */
#define hit_cShapeAddEdge(shapep, x, y) hit_cShapeAddElem_or_Edge(shapep, x, y, HIT_CSHAPE_GRAPH)

/**
 * Adds the edge x->y and y->x
 * @memberof HitCShape
 * @note: It creates the nodes if it is necessary.
 * @param shape The CShape pointer of the graph.
 * @param x A node (in global coordinates).
 * @param y A node (in global coordinates).
 */
#define hit_cShapeAddEdge2(shape,x,y) {hit_cShapeAddEdge(shape,x,y); hit_cShapeAddEdge(shape,y,x);}

/**
 * Creates the inverse translation list of vertex names.
 * Create the list for both dimensions.
 * This list is used to speedup the translation operation from global to local names.
 * This function will increase the size of the shape.
 * @memberof HitCShape
 * @param shape A pointer to HitCShape.
 */
void hit_cShapeCreateInvNames(HitShape * shape);



/* 4. Hit CSR Sparse Shape Iterators */

/**
 * Row iterator.
 * @memberof HitCShape.
 * @note vertex variable is in local coordinates.
 * @param var Loop variable
 * @param shape A HitCShape.
 */
#define hit_cShapeRowIterator(var,shape) \
	for(var=0; var<hit_cShapeCard(shape, 0); var++)

/**
 * Vertex iterator.
 * @memberof HitCShape
 * @note vertex variable is in local coordinates.
 * @note same function as hit_cShapeRowIterator.
 * @param var Loop variable
 * @param shape The CShape
 */
#define hit_cShapeVertexIterator(var,shape) hit_cShapeRowIterator(var,shape)

/**
 * Column iterator from a given row.
 * @memberof HitCShape
 * @note row variable is in local coordinates.
 * @note column variable is the index of the local coordinate.
 * @param var Loop variable
 * @param shape The HitCShape
 * @param row The row
 */
#define hit_cShapeColumnIterator(var,shape,row) \
	for(var=hit_cShapeFistColumn(shape,row); var<hit_cShapeLastColumn(shape,row); var++)


/**
 * Edge iterator from a given vertex.
 * @memberof HitCShape
 * @note vertex variable is in local coordinates.
 * @note edge variable is the index of the local coordinate.
 * @param var Loop variable
 * @param shape The HitCShape
 * @param vertex The vertex
 */
#define hit_cShapeEdgeIterator(var,shape,vertex) hit_cShapeColumnIterator(var,shape,vertex)


/**
 * Adds a new element to a matrix or and edge to a graph
 * @param shape The CShape.
 * @param x The x coordinate or source vertex.
 * @param y The y coordinate or destination vertex.
 * @param mode The Matrix or Graph mode
 */
void hit_cShapeAddElem_or_Edge(HitShape * shape, int x, int y, int mode);


/**
 * Adds a new element to a matrix or and edge to a graph
 * @param shape The CShape.
 * @param x The x coordinate.
 * @param y The y coordinate.
 */
#define hit_cShapeAddElem(shapep, x, y) hit_cShapeAddElem_or_Edge(shapep, x, y, HIT_CSHAPE_MATRIX)



/* END OF HEADER FILE _HitCShape_ */
#endif
