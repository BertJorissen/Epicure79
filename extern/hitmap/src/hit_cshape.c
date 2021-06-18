/**
 * Types and functions to manipulate CSR sparse domains.
 *
 * @file hit_cshape.c
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

#include <limits.h>
#include <hit_allocP.h>
#include <hit_cshape.h>
#include <hit_funcop.h>


HitShape hit_csrShape(int nvertices, int nedges){

	HitShape s = HIT_CSR_SHAPE_NULL;

	// Number of vertices.
	hit_cShapeNvertices(s) = nvertices;

	// Allocate the arrays.
	hit_malloc(hit_cShapeXadj(s), idxtype, nvertices+1 );
	hit_malloc(hit_cShapeAdjncy(s), idxtype, nedges );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(hit_cShapeXadj(s),(size_t) (nvertices+1)*sizeof(idxtype),idxtype*);
	hit_malloc(hit_cShapeAdjncy(s),(size_t) nedges*sizeof(idxtype),idxtype*);
	*/

	hit_nameListCreate(&hit_cShapeNameList(s,0),nvertices);
	hit_cShapeNameList(s,1) = hit_cShapeNameList(s,0);

	// Return the shape
	return s;
}



HitShape hit_csrShapeMatrix(int n, int m, int nz){

	HitShape s = HIT_CSR_SHAPE_NULL;

	// Cardinalities.
	hit_cShapeCard(s,0) = n;
	hit_cShapeCard(s,1) = m;

	// Allocate the arrays.
	hit_malloc(hit_cShapeXadj(s), idxtype, n+1 );
	if(nz != 0) hit_malloc(hit_cShapeAdjncy(s), idxtype, nz );
	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(hit_cShapeXadj(s),(size_t) (n+1)*sizeof(idxtype),idxtype*);
	if(nz != 0) hit_malloc(hit_cShapeAdjncy(s),(size_t) nz*sizeof(idxtype),idxtype*);
	*/

	hit_nameListCreate(&hit_cShapeNameList(s,0),n);
	hit_nameListCreate(&hit_cShapeNameList(s,1),m);

	// Return the shape
	return s;
}




void hit_cShapeFree(HitShape shape){

	hit_free(hit_cShapeXadj(shape));
	hit_free(hit_cShapeAdjncy(shape));
	hit_nameListFree(hit_cShapeNameList(shape,0));
	if(hit_cShapeNameList(shape,0).names != hit_cShapeNameList(shape,1).names)
		hit_nameListFree(hit_cShapeNameList(shape,1));
}


HitShape hit_cShapeSelect(HitShape s, int nvertices, int * vertices){
	
	// We have to select one vertex at least.
	if(nvertices < 1) return HIT_SHAPE_NULL;
	
	// 1. If vertices is NULL we use the name vector (a.k.a. n first vertices)
	if(vertices == NULL){
		vertices = hit_cShapeNameList(s,0).names;
	}
	
	// 2. Create a new graph with edge array as big as the original.
	HitShape res = hit_csrShape(nvertices, hit_cShapeNedges(s));
	
	// 3. Duplicate vertices array as globalNames.
	memcpy(hit_cShapeNameList(res,0).names, vertices, sizeof(int) * (size_t) nvertices );
	hit_cShapeNameList(res,0).nNames = nvertices;
	hit_cShapeNameList(res,0).flagNames = HIT_SHAPE_NAMES_NOARRAY;
	hit_cShapeNameList(res,1) = hit_cShapeNameList(res,0);
	
	// 4. Construct the inverse translation list
	int vertex_min = INT_MAX;
	int vertex_max = 0;
	
	// 4.1. Get the maximum and minimum values
	int i;
	for(i=0;i<nvertices;i++){
		vertex_min = hit_min(vertex_min,vertices[i]);
		vertex_max = hit_max(vertex_max,vertices[i]);
	}
	
	int * inv_list_p;
	hit_malloc(inv_list_p, int, vertex_max-vertex_min+1 );
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(inv_list_p,sizeof(int) * (size_t)(vertex_max-vertex_min+1),int*);
	int * inv_list = inv_list_p - vertex_min;
	
	// Init list elements to -1
	for(i=vertex_min+1;i<vertex_max;i++){
		inv_list[i] = -1;
	}
	
	for(i=0;i<nvertices;i++){
		inv_list[vertices[i]] = i;
	}
	
	// 5. Construct the new sparse shape
	int nedges = 0;
	hit_cShapeXadj(res)[0] = 0;
	
	
	// 5.1 Traverse all the new vertices
	int j;
	for(i=0;i<nvertices;i++){
		
		hit_cShapeXadj(res)[i+1] = hit_cShapeXadj(res)[i];
		
		int vertex = hit_cShapeVertexToLocal(s,vertices[i]);
		int actedges = hit_cShapeNEdgesFromVertex(s,vertex);

		// 5.2 For each edge, check if we have to add it.
		for(j=0;j<actedges;j++){
			
			int dst = hit_cShapeAdjncy(s)[ hit_cShapeXadj(s)[vertex] + j ];
			int globaldst = hit_cShapeCoordToGlobal(s,0,dst);

			// 4.2.1 Search the vertices in our list.
			if(globaldst < vertex_min || globaldst > vertex_max) continue;
			
			int new_dst = inv_list[globaldst];
			
			if(new_dst == -1) continue;
			
			hit_cShapeAdjncy(res)[nedges] = new_dst;
			nedges ++;
			hit_cShapeXadj(res)[i+1] ++;
			
		}
	}
	
	// 6. Reduce the edge list to the exact size
	if(nedges == 0){
		hit_free(hit_cShapeAdjncy(res));
		hit_cShapeAdjncy(res) = NULL;
	} else {
		// @arturo Ago 2015: New allocP interface
		// hit_realloc(hit_cShapeAdjncy(res),(size_t) nedges*sizeof(idxtype),idxtype*);
		hit_realloc(hit_cShapeAdjncy(res), idxtype, nedges );
	}
	
	// 7. Free resources
	free(inv_list_p);
	
	// 8. Return
	return res;
	
}



HitShape hit_cShapeExpand(HitShape shape, HitShape original, int amount){

	//@javfres 2015-10-05 Fix for expanding null shapes
	// 0. Check the shape has vertices
	int nvertices = hit_cShapeNvertices(shape);
	if(nvertices == 0){
		HitShape res = HIT_CSR_SHAPE_NULL;
		return res;
	}

	// 1. Create a vertex list
	int orig_nvertices = hit_cShapeNvertices(original);
	int * vertices;
	// @arturo Ago 2015: New allocP interface
	// hit_calloc(vertices, (size_t)orig_nvertices, sizeof(int),int*);
	hit_calloc(vertices, int, orig_nvertices );
	
#define VLOCAL	1 // (....0001)
#define VEXPAND	2 // (....0010)
	
	// 2. Set the local vertices in the vector.
	int vertex;
	for(vertex=0; vertex<nvertices; vertex++){
	
		int global_vertex = hit_cShapeVertexToGlobal(shape,vertex);
		int local_orig_vertex = hit_cShapeVertexToLocal(original,global_vertex);
		
		vertices[local_orig_vertex] = VLOCAL;
	}
	
	// 3. Expand the list.
	int new_vertices = 0;
	int a;
	for(a=0; a<amount; a++){
	
		// Complete the local vertices and the number of vertices.
		for(vertex=0; vertex<orig_nvertices; vertex++){
			
			// Get first and last links.
			int first = hit_cShapeXadj(original)[vertex];
			int last = hit_cShapeXadj(original)[vertex+1];
			int nlink;


			for(nlink=first; nlink<last; nlink++){

				//Get the neighbor.
				int neighbor = hit_cShapeAdjncy(original)[nlink];
				
				// TO
				if((vertices[vertex] & VLOCAL) && (! vertices[neighbor] & VLOCAL)){
					vertices[neighbor] = VEXPAND;
					new_vertices++;
				}
				
				// @javfres @todo The hit_cShapeExpand function expand the shape
				// following the edges from source to target. I call this TO direction.
				// It can be interesting to add a FROM option, or maybe several functions
				// with TO/FROM/TOFROM.
			}

		}
		
		// Set the expanded vertices as local for the next loop.
		if(a<amount-1){
			for(vertex=0; vertex<orig_nvertices; vertex++){
				if(vertices[vertex] == VEXPAND) vertices[vertex] |= VLOCAL;
			}
		}
		
	}
	
	
	// 3. Create selection list
	int * vlist;
	int nvlist = new_vertices + nvertices;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(vlist,sizeof(int) * (size_t) nvlist,int*);
	hit_malloc(vlist, int, nvlist );
	
	// 4. The first vertices of the old and new shape are the same.
	memcpy(vlist,hit_cShapeNameList(shape,0).names, sizeof(int) * (size_t) nvertices);
	
	// 5. Complete the selection list.
	int vertex_index = nvertices;
	for(vertex=0; vertex<orig_nvertices; vertex++){
	
		if(vertices[vertex] > VLOCAL){
			vlist[vertex_index] = hit_cShapeVertexToGlobal(original,vertex);
			vertex_index++;
		}
	}
	
	// 6. Select
	HitShape res = hit_cShapeSelect(original,nvlist,vlist);
	
	// 7. Free resources.
	hit_free(vertices);
	hit_free(vlist);

#undef VLOCAL
#undef VEXPAND
	
	return res;

}


	

void hit_cShapeCreateInvNames(HitShape * shape){

	// 1. Check the shape type.
	if(hit_shapeType(*shape) != HIT_CSR_SHAPE){
		hit_errInternal(__func__, "Incorrect shape Type.", "Expected CSR Shape", __FILE__, __LINE__);
	}
	
	hit_nameListCreateInvNames(&hit_cShapeNameList(*shape,0));
	if(hit_cShapeNameList(*shape,0).names == hit_cShapeNameList(*shape,1).names){
		hit_cShapeNameList(*shape,1) = hit_cShapeNameList(*shape,0);
	} else {
		hit_nameListCreateInvNames(&hit_cShapeNameList(*shape,1));
	}

}
















void hit_cShapeAddEmptyRow_or_Vertex(HitShape * shape, int x, int mode){

#define s (*shape)

	int local_x, nelems_x;

	local_x = hit_cShapeCoordToLocal(s,0,x);

	if( local_x  != -1 ) return;
	local_x = hit_cShapeCard(s, 0);

	// A. Create the sparse shape.
	if(local_x == 0){

		nelems_x = 1;

		// Allocate memory
		hit_cShapeCard(s, 0) = nelems_x;
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(hit_cShapeXadj(s),(size_t) (nelems_x+1)*sizeof(idxtype),idxtype*);
		hit_malloc(hit_cShapeXadj(s), idxtype, nelems_x+1 );

		// Set element 1
		hit_cShapeXadj(s)[0] = 0;
		hit_cShapeXadj(s)[1] = 0;

		// Create the name list
		HitNameList list = HIT_NAMELIST_NULL;
		hit_nameListAdd(&list, x);
		hit_cShapeNameList(s,0) = list;

		if(mode == HIT_CSHAPE_GRAPH){
			hit_cShapeNameList(s,1) = list;
		}

	// B. The given sparse shape is not empty.
	} else {

		nelems_x = hit_cShapeCard(s, 0)  + 1;
		hit_cShapeCard(s, 0) = nelems_x;
		// @arturo Ago 2015: New allocP interface
		// hit_realloc(hit_cShapeXadj(s),(size_t) (nelems_x+1)*sizeof(idxtype),idxtype*);
		hit_realloc(hit_cShapeXadj(s), idxtype, nelems_x+1 );
		hit_cShapeXadj(s)[local_x+1] = hit_cShapeXadj(s)[local_x];

		// Set names
		hit_nameListAdd(&hit_cShapeNameList(s,0), x);

		if(mode == HIT_CSHAPE_GRAPH){
			hit_cShapeNameList(s,1) = hit_cShapeNameList(s,0);
		}
	}

#undef s

}


void hit_cShapeAddColumn(HitShape * shape, int y){

	if(hit_nameListName2Index(hit_cShapeNameList(*shape,1),y) == -1){
		hit_nameListAdd(&hit_cShapeNameList(*shape,1),y);
		hit_cShapeCard(*shape,1)++;
	}
}


int hit_cShapeElemExists(HitShape shape, int x, int y){

	int local_x = hit_cShapeCoordToLocal(shape,0,x);
	int local_y = hit_cShapeCoordToLocal(shape,1,y);

	int c;
	hit_cShapeColumnIterator(c,shape,local_x){
		if(hit_cShapeAdjncy(shape)[c] == local_y){
			return 1;
		}
	}
	return 0;
}



void hit_cShapeAddElem_or_Edge(HitShape * shape, int x, int y, int mode){

	// 1. Add the row or vertices
	hit_cShapeAddEmptyRow_or_Vertex(shape, x, mode);
	if(mode == HIT_CSHAPE_GRAPH)
		hit_cShapeAddEmptyRow_or_Vertex(shape, y, mode);
	else
		hit_cShapeAddColumn(shape, y);

#define s (*shape)

	// 2. Get the local coordinates names.
	int local_x = hit_cShapeCoordToLocal(s,0,x);
	int local_y = hit_cShapeCoordToLocal(s,1,y);


	// 3. Checks if element exists
	int c;
	hit_cShapeEdgeIterator(c,s,local_x){
		if(hit_cShapeAdjncy(s)[c] == local_y) return;
	}

	// 4. Add the element
	int nedges = hit_cShapeNedges(s) + 1;

	if(hit_cShapeAdjncy(s) == NULL){
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(hit_cShapeAdjncy(s),(size_t) nedges*sizeof(idxtype),idxtype*);
		hit_malloc(hit_cShapeAdjncy(s), idxtype, nedges );
	}else{
		// @arturo Ago 2015: New allocP interface
		// hit_realloc(hit_cShapeAdjncy(s),(size_t) nedges*sizeof(idxtype),idxtype*);
		hit_realloc(hit_cShapeAdjncy(s), idxtype, nedges );
	}

	// 4.1 Create a space for the element moving the other elements right
	int i;
	for(i=nedges-1; i>hit_cShapeLastColumn(s,local_x); i--){
		hit_cShapeAdjncy(s)[i] = hit_cShapeAdjncy(s)[i-1];
	}

	// 4.2 Insert the element
	hit_cShapeAdjncy(s)[hit_cShapeLastColumn(s,local_x)] = local_y;

	// 4.3 Update the Xadj's pointers
	for(i=local_x;i<hit_cShapeNvertices(s);i++){
		hit_cShapeXadj(s)[i+1] ++ ;
	}

#undef s

}



/**
 * Compress the columns removing the not used columns and
 * update the names and adjncy vector.
 */
void hit_cShapeSelectRows_compress_columns(HitShape * shape){

	int n = hit_cShapeCard(*shape,0);
	int m = hit_cShapeCard(*shape,1);
	int * used;
	// @arturo Ago 2015: New allocP interface
	// hit_calloc(used,(size_t)m,sizeof(int),int*);
	hit_calloc(used, int, m );

	int i;

	// Count the number of used elements.
	for(i=0; i<n; i++){
		int j;
		for(j=hit_cShapeXadj(*shape)[i]; j<hit_cShapeXadj(*shape)[i+1]; j++){
			int idx = hit_cShapeAdjncy(*shape)[j];
			used[idx] = 1;
		}
	}

	// Delete the not used elements from the name list
	// and use the used list to create a translation list.
	int nused = 0;
	for(i=0; i<m; i++){
		if(used[i]){
			hit_cShapeNameList(*shape,1).names[nused] = hit_cShapeNameList(*shape,1).names[i];
			used[i] = nused;
			nused++;
		}
	}

	// Set the new column number and realloc the name list.
	hit_cShapeCard(*shape,1) = nused;
	hit_cShapeNameList(*shape,1).nNames = nused;
	//hit_cShapeNameList(*shape,1).names = realloc(hit_cShapeNameList(*shape,1).names, (size_t) nused * sizeof(int) );
	// @arturo Ago 2015: New allocP interface
	// hit_realloc(hit_cShapeNameList(*shape,1).names,(size_t) nused * sizeof(int),int*);
	hit_realloc(hit_cShapeNameList(*shape,1).names, int, nused );

	// Rename the elements.
	for(i=0; i<hit_cShapeXadj(*shape)[n]; i++){
		hit_cShapeAdjncy(*shape)[i] = used[hit_cShapeAdjncy(*shape)[i]];
	}

	// Free the used list
	free(used);
}




HitShape hit_cShapeSelectRows(HitShape shape, int nNames, int * names){

	// Select cardinalities: matrix of n x m
	int n = nNames;
	int m = hit_cShapeCard(shape,1);

	if(n == 0){
		return HIT_CSR_SHAPE_NULL;
	}

	// Create the matrix
	HitShape res = hit_csrShapeMatrix(n,m,0);

	// Loop for copy and update the Xadj array.
	int i;
	int acumNCols = 0;
	hit_cShapeXadj(res)[0] = 0;
	for(i=0;i<n;i++){

		// Get the row and its number of cols
		int row = hit_cShapeCoordToLocal(shape,0,names[i]);
		int nCols = hit_cShapeNColsRow(shape, row);
		acumNCols += nCols;

		// Add size for this new row
		// @arturo Ago 2015: New allocP interface
		// hit_realloc(hit_cShapeAdjncy(res),(size_t) acumNCols*sizeof(idxtype),idxtype*);
		hit_realloc(hit_cShapeAdjncy(res), idxtype, acumNCols );

		// Copy the values of the column indexes
		void * dst = &(hit_cShapeAdjncy(res)[hit_cShapeFistColumn(res,i)]);
		void * src = &(hit_cShapeAdjncy(shape)[hit_cShapeFistColumn(shape,row)]);
		memcpy(dst,src, (size_t) nCols *sizeof(idxtype));

		// Update the Xadj array
		hit_cShapeXadj(res)[i+1] = acumNCols;

	}

	// Create the name lists
	memcpy(hit_cShapeNameList(res,0).names, names, (size_t) n *sizeof(idxtype) );
	hit_cShapeNameList(res,0).flagNames = HIT_SHAPE_NAMES_NOARRAY;
	hit_nameListFree(hit_cShapeNameList(res,1));

	//hit_cShapeNameList(res,1) = hit_cShapeNameList(shape,1);
	hit_nameListClone( &hit_cShapeNameList(res,1),  &hit_cShapeNameList(shape,1) );

	// Compress the columns removing the not used columns
	hit_cShapeSelectRows_compress_columns(&res);

	return res;

}







