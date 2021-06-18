/**
 * Types and functions to work with files.
 *
 * @file hit_file.c
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @date Jul 2011
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

#include <hit_file.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include <hit_error.h>
#include <hit_allocP.h>
#include <hit_funcop.h>
#include <hit_cshape.h>
#include <hit_bshape.h>

#ifndef INCLUDE_MATRIX_IO__H
	#define INCLUDE_MATRIX_IO__H
	#include "matrix_io.h"
#endif

/**
 * Compare function for idxtype, to be used in stdlib's qsort(3).
 * @param a Pointer to an idxtype.
 * @param b Pointer to an idxtype.
 * @return An integer less than, equal to, or greater than zero
 * if the first argument is less  than, equal to, or greater than the second.
 */
int compare_idxtype (const void * a, const void * b){
	return (*(const idxtype *)a - *(const idxtype *)b);
}


/** */
void hit_csr_renameIndexes(int Nr, int * xadj, int * adjncy){

	int i;
	for(i=0;i<=Nr;i++){
		xadj[i]--;
	}

	for(i=0;i<xadj[Nr];i++){
		adjncy[i]--;
	}
}


/** */
int hit_csr_renameIndexes_and_Symmetrize(int * pNr, int * pNc, int nz, int ** pxadj, int ** padjncy,
                                         const char * file, int line){

	// 0. Original CSR
	int i,j;
	int	*colptr = *pxadj;
	int *rowind = *padjncy;

	// 1. Get the number of vertices
	int N = hit_max(*pNr,*pNc);

	//printf("[%d,%d]: max: %d\n",*pNr,*pNc,N);

	if(*pNr < N){

		// @arturo Ago 2015: New allocP interface
		// hit_realloc(colptr,(size_t) (N+1) * sizeof(idxtype),idxtype*);
		hit_realloc(colptr, idxtype, N+1 );

		if (colptr == NULL) {
			hit_errInternal(__FUNCTION__,"Symmetrize: not enough memory for data structures","",file,line);
			return HIT_ERROR;
		}
		for(i=(*pNr)+1;i<N+1;i++){
			colptr[i] = colptr[i-1];
		}
	}
	*pNr = N;
	*pNc = N;




	// 2. Allocate memory for CSR format
	(*pxadj) = idxmalloc(N+1, (char *)&"Symmetrize: xadj");
	(*padjncy) = idxmalloc(2*nz, (char *)&"Symmetrize: adjncy");

	// 3. Allocate memory for edge counters
	int * n_edge;
	// @arturo Ago 2015: New allocP interface
	// hit_calloc(n_edge,(size_t)N,sizeof(int),int*);
	hit_calloc(n_edge, int, N );

	// 4. Check allocations.
	if ((*pxadj)==NULL || (*padjncy)==NULL  || n_edge==NULL) {
		hit_errInternal(__FUNCTION__,"Symmetrize: not enough memory for data structures","",file,line);
		return HIT_ERROR;
	}

	// 5. Count the number of edges that do not appear in the HB (unsymmetric)
	for(i=0;i<N;i++){

		int begin = colptr[i]-1;
		int end = colptr[i+1]-1;
		for(j=begin;j<end;j++){

			int target = rowind[j]-1;
			int begin2 = colptr[target]-1;
			int end2 = colptr[target+1]-1;
			int j2;
			int notfound = 1;

			for(j2=begin2;j2<end2;j2++){
				int target2 = rowind[j2]-1;
				if(target2 == i){
					notfound = 0;
					break;
				}
			}
			n_edge[target] += notfound;
		}
	}


	// 6. Init the pxadj with the correct values
	(*pxadj)[0] = 0;
	for(i=0;i<N;i++){
		int begin = colptr[i]-1;
		int end = colptr[i+1]-1;
		int length = (end-begin) + n_edge[i];

		(*pxadj)[i+1] = (*pxadj)[i] + length;
	}

	// 7. Copy the HB unsymmetric edges in adjncy
	for(i=0;i<N;i++){

		int begin = colptr[i]-1;
		int end = colptr[i+1]-1;
		int length = (end-begin);

		int newbegin = (*pxadj)[i];

		for(j=0;j<length;j++){

			int target = rowind[begin+j]-1;
			(*padjncy)[newbegin+j] = target;
	 	}
	}

	// 8. Copy the remain edges
	for(i=0;i<N;i++){

		int begin = colptr[i]-1;
		int end = colptr[i+1]-1;
		for(j=begin;j<end;j++){

			int target = rowind[j]-1;
			int begin2 = colptr[target]-1;
			int end2 = colptr[target+1]-1;
			int j2;
			int notfound = 1;

			// Optimization to skip complete edges
			if(n_edge[target] == 0) continue;

			for(j2=begin2;j2<end2;j2++){
				int target2 = rowind[j2]-1;
				if(target2 == i){
					notfound = 0;
					break;
				}
			}

			// Copy the edge
			if(notfound){
				int pos = (*pxadj)[target+1]-n_edge[target];
				(*padjncy)[pos] = i;
				n_edge[target]--;
			}
		}
	}


	// 9. Reorder the edges
	for(i=0;i<N;i++){

		int begin = (*pxadj)[i];
		int end = (*pxadj)[i+1];

		qsort(&(*padjncy)[begin], (size_t)(end-begin), sizeof(idxtype), compare_idxtype);
	}

	// 10. Free extra memory
	// @arturo Ago 2015: New allocP interface
	// hit_realloc(*padjncy,(size_t) ((*pxadj)[N]) * sizeof(idxtype),idxtype*);
	hit_realloc(*padjncy, idxtype, (*pxadj)[N] );

	// 99. FREE DATA STRUCTURES
	free(colptr);
	free(rowind);
	free(n_edge);


	return HIT_OK;

}



HitShape hit_fileMMRead_toCSR_Internal(const char *fileName, int create_graph, int rank,
		                          const char * file, int line){

	// 0. If the current processor is not root return a null shape.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

	// 1. READ HB HEADER
	// Rows and columns sizes
	int Nr, Nc;
	// Nonzero elements
	int nz;
	// Matrix type
	char * type = NULL;
	// Number of right-hand-sides
	//int Nrhs;

	// 2. READ THE MATRIX

	// Row and column pointer.
	int	*xadj = NULL, *adjncy = NULL;
	// Array with the values.
	double	*val = NULL;

	// 2.1 Read the matrix allocating storage arrays

	read_mm_matrix ((char *)fileName, &Nr, &Nc, &nz, &xadj, &adjncy, &val);


	// 3. TRANSFORM THE CSR TO 0-index AND SIMETRIZE
	if( create_graph){
	//	hit_csr_renameIndexes_and_Symmetrize(&Nr, &Nc, nz, &xadj, &adjncy, file, line);
	} else {
		//hit_csr_renameIndexes(Nr, xadj, adjncy);
	}


#ifdef DEBUG
	{
		int i,j;
		printf("\nMM: Nr: %d Nc: %d \n", Nr, Nc);
		printf("\t");
		for(i=0;i<Nr;i++){
			int begin = xadj[i];
			int end = xadj[i+1];
			for(j=begin;j<end;j++){
				printf(" %d ", adjncy[j]);
			}
			printf("|\n\t");
		}
		printf("\n");
	}
#endif


	// 4. CREATE THE NAME LISTS
	// Row and Column lists.
	HitNameList listR;
	HitNameList listC;

	hit_nameListCreate(&listR, Nr);

	if(create_graph){
		listC = listR;
	} else {
		hit_nameListCreate(&listC, Nc);
	}


	// 5. BUILD THE SHAPE
	HitShape shape = HIT_CSR_SHAPE_NULL;
	hit_cShapeCard(shape,0) = Nr;
	hit_cShapeCard(shape,1) = Nc;
	hit_cShapeXadj(shape) = xadj;
	hit_cShapeAdjncy(shape) = adjncy;
	hit_cShapeNameList(shape,0) = listR;
	hit_cShapeNameList(shape,1) = listC;


	// 99. Free resources
	free(type);
	free(val);

	return shape;
}







HitShape hit_fileHBRead_toCSR_Internal(const char *fileName, int create_graph, int rank,
		                          const char * file, int line){

	// 0. If the current processor is not root return a null shape.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

	// 1. READ HB HEADER
	// Rows and columns sizes
	int Nr, Nc;
	// Nonzero elements
	int nz;
	// Matrix type
	char * type;
	// Number of right-hand-sides
	int Nrhs;

	// 1.1 Read HB header
	if( !readHB_info(fileName, &Nr, &Nc, &nz, &type, &Nrhs) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing file header","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

#ifdef DEBUG
	fprintf(stderr,"[HB file] Nr: %d, Nc: %d, nz: %d, type: %s, Nrhs: %d\n",Nr,Nc,nz,type,Nrhs);
#endif


	// 1.2. Reject Righ-hand side matrices
	if( Nrhs > 0 ) {
		hit_warnInternal(__FUNCTION__,"Hitmap ignores Harwell-Boeing matrices with right-hand-sides","",file,line);
	}

	// 1.3 Reject not pattern matrices
	// (P)attern, (R)eal or (C)omplex.
	if( type[0] != 'P' ){
		hit_warnInternal(__FUNCTION__,"Hitmap ignores Harwell-Boeing matrix values","",file,line);
	}

	// 1.4 Reject not assembled matrices
	if( type[1] != 'U' && type[1] != 'R' && type[1] != 'S' ){
		hit_errInternal(__FUNCTION__,"Harwell-Boeing matrix type not supported","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

	// 1.5 Reject not assembled matrices
	if( type[2] != 'A' ){
		hit_errInternal(__FUNCTION__,"Harwell-Boeing matrix is not assembled","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}



	// 2. READ THE MATRIX

	// Row and column pointer.
	int	*xadj = NULL, *adjncy = NULL;
	// Array with the values.
	double	*val = NULL;

	// 2.1 Read the matrix allocating storage arrays
	if( !readHB_newmat_double(fileName, &Nr, &Nc, &nz, &xadj, &adjncy, &val) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing matrix data","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}


	// 3. TRANSFORM THE CSR TO 0-index AND SIMETRIZE
	if(type[1] == 'S' || create_graph){
		hit_csr_renameIndexes_and_Symmetrize(&Nr, &Nc, nz, &xadj, &adjncy, file, line);
	} else {
		hit_csr_renameIndexes(Nr, xadj, adjncy);
	}

#ifdef DEBUG
	{
		int i,j;
		printf("\nHB: Nr: %d Nc: %d \n", Nr, Nc);
		printf("\t");
		for(i=0;i<Nr;i++){
			int begin = xadj[i];
			int end = xadj[i+1];
			for(j=begin;j<end;j++){
				printf(" %d ", adjncy[j]);
			}
			printf("|\n\t");
		}
		printf("\n");
	}
#endif

	// 4. CREATE THE NAME LISTS
	// Row and Column lists.
	HitNameList listR;
	HitNameList listC;

	hit_nameListCreate(&listR, Nr);

	if(create_graph){
		listC = listR;
	} else {
		hit_nameListCreate(&listC, Nc);
	}


	// 5. BUILD THE SHAPE
	HitShape shape = HIT_CSR_SHAPE_NULL;
	hit_cShapeCard(shape,0) = Nr;
	hit_cShapeCard(shape,1) = Nc;
	hit_cShapeXadj(shape) = xadj;
	hit_cShapeAdjncy(shape) = adjncy;
	hit_cShapeNameList(shape,0) = listR;
	hit_cShapeNameList(shape,1) = listC;


	// 99. Free resources
	free(type);
	free(val);

	return shape;
}



HitShape hit_fileHBRead_toBitmap_Internal(const char *fileName, int create_graph, int rank,
		                          const char * file, int line){

	// 0. If the current processor is not root return a null shape.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

	// 1. READ HB HEADER
	// Rows and columns sizes
	int Nr, Nc;
	// Nonzero elements
	int nz;
	// Matrix type
	char * type;
	// Number of right-hand-sides
	int Nrhs;

	// 1.1 Read HB header
	if( !readHB_info(fileName, &Nr, &Nc, &nz, &type, &Nrhs) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing file header","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

#ifdef DEBUG
	fprintf(stderr,"[HB file] Nr: %d, Nc: %d, nz: %d, type: %s, Nrhs: %d\n",Nr,Nc,nz,type,Nrhs);
#endif


	// 1.2. Reject Righ-hand side matrices
	if( Nrhs > 0 ) {
		hit_warnInternal(__FUNCTION__,"Hitmap ignores Harwell-Boeing matrices with right-hand-sides","",file,line);
	}

	// 1.3 Reject not pattern matrices
	// (P)attern, (R)eal or (C)omplex.
	if( type[0] != 'P' ){
		hit_warnInternal(__FUNCTION__,"Hitmap ignores Harwell-Boeing matrix values","",file,line);
	}

	// 1.4 Reject not assembled matrices
	if( type[1] != 'U' && type[1] != 'R' && type[1] != 'S' ){
		hit_errInternal(__FUNCTION__,"Harwell-Boeing matrix type not supported","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}

	// 1.5 Reject not assembled matrices
	if( type[2] != 'A' ){
		hit_errInternal(__FUNCTION__,"Harwell-Boeing matrix is not assembled","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}



	// 2. READ THE MATRIX

	// Row and column pointer.
	int	*xadj = NULL, *adjncy = NULL;
	// Array with the values.
	double	*val = NULL;

	// 2.1 Read the matrix allocating storage arrays
	if( !readHB_newmat_double(fileName, &Nr, &Nc, &nz, &xadj, &adjncy, &val) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing matrix data","",file,line);
		return HIT_CSR_SHAPE_NULL;
	}


	/* 3. LOAD THE MATRIX IN THE SHAPE */

	/* 3.1 New HitBShape */
	HitShape shape = HIT_BITMAP_SHAPE_NULL;

	if(create_graph){
		shape = hit_bitmapShape(Nr);
	} else {
		shape = hit_bitmapShapeMatrix(Nr,Nc);
	}

	/* 3.2 Fill the tile */
	int i,j;
	for(i=0; i<Nr; i++) {
		int cstyleIni = xadj[i]-1;
		int cstyleEnd = xadj[i+1]-1;

		for(j=cstyleIni; j<cstyleEnd; j++) {
			hit_bShapeSet(shape,i,adjncy[j]-1);
			if(type[1] == 'S' || create_graph){
				hit_bShapeSet(shape,adjncy[j]-1,i);
			}
		}
	}

	// 99. Free resources
	free(type);
	free(val);
	free(xadj);
	free(adjncy);

	return shape;
}





void hit_fileHBWriteInternal(const char * hbfile, HitShape shape, int rank, const char * cfile, int cline) {
	HIT_NOT_USED( rank );

// 10 x 8 = 80 characters per line.
#define NUMBERS_LINE 10
#define NUMBERS_SIZE 8


	if(hit_shapeType(shape) != HIT_CSR_SHAPE){
		hit_errInternal(__FUNCTION__,"Wrong shape type","",cfile,cline);
	}


	FILE * file = fopen(hbfile, "w");

	// Number of Vertices and edges
	int nvertices = hit_cShapeNvertices(shape);
	int nedges = hit_cShapeNedges(shape);

	// LINE 1
	// 1.1 Title (72 characters)
	fprintf(file,"%-72s","HitMap HB Export");
	// 1.2, (8 characters)
	fprintf(file,"%-8s","key");
	fprintf(file,"\n");

	// LINE 2
	//2.1 TOTCRD, total number of data lines, (14 characters)
	//2.2 PTRCRD, number of data lines for pointers, (14 characters)
	//2.3 INDCRD, number of data lines for row or variable indices, (14 characters)
	int ptrcrd = (int) ceil(((double) nvertices + 1) / NUMBERS_LINE );
	int indcrd = (int) ceil((double) nedges / NUMBERS_LINE );
	fprintf(file,"%14d",ptrcrd + indcrd);
	fprintf(file,"%14d",ptrcrd);
	fprintf(file,"%14d",indcrd);
	//2.4 VALCRD, number of data lines for numerical values of matrix entries, (14 characters)
	//2.5 RHSCRD, number of data lines for right hand side vectors, starting guesses, and solutions, (14 characters)
	fprintf(file,"%14d",0);
	fprintf(file,"%14s","");
	fprintf(file,"\n");

	// LINE 2
	// MXTYPE, matrix type, (3 characters)
	// pua: Pattern Unsymmetric Assembled
	fprintf(file,"%3s","pua");
	// blank space, (11 characters)
	fprintf(file,"%11s","");
	// NROW, integer, number of rows or variables, (14 characters)
	fprintf(file,"%14d", nvertices);
	// NCOL, integer, number of columns or elements, (14 characters)
	fprintf(file,"%14d", nvertices);
	// NNZERO, number of nonzero entries. (14 characters)
	fprintf(file,"%14d", nedges);
	// NELTVL, this is 0. (14 characters)
	fprintf(file,"%14d", 0);
	fprintf(file,"\n");


	// LINE 4
	// PTRFMT, FORTRAN I/O format for pointers, (16 characters)
	fprintf(file,"%-16s","(10I8)");
	// INDFMT, FORTRAN I/O format for row or variable indices, (16 characters)
	fprintf(file,"%-16s","(10I8)");
	// VALFMT, FORTRAN I/O format for matrix entries, (20 characters)
	fprintf(file,"%-20s","");
	// RHSFMT, FORTRAN I/O format for right hand sides, initial guesses, and solutions, (20 characters)
	fprintf(file,"%-20s","");
	//fprintf(file,"\n");

	int i;

	for(i=0;i<=nvertices;i++){

		if(i % NUMBERS_LINE == 0 ) fprintf(file,"\n");
		int vertex = hit_cShapeXadj(shape)[i];
		fprintf(file, "%8d",vertex+1);
	}


	for(i=0;i<nedges;i++){

		if(i % NUMBERS_LINE == 0 ) fprintf(file,"\n");
		int edge = hit_cShapeAdjncy(shape)[i];
		fprintf(file, "%8d",edge+1);
	}

	fclose(file);

}


void hit_fileHBWriteBitmapInternal(const char * hbfile, HitShape shape, int rank, const char * cfile, int cline){
	HIT_NOT_USED( rank );

// 10 x 8 = 80 characters per line.
#define NUMBERS_LINE 10
#define NUMBERS_SIZE 8


	if(hit_shapeType(shape) != HIT_BITMAP_SHAPE){
		hit_errInternal(__FUNCTION__,"Wrong shape type","",cfile,cline);
	}


	FILE * file = fopen(hbfile, "w");
	//FILE * file = stdout;

	// Number of Vertices and edges
	int nvertices = hit_bShapeNvertices(shape);
	int nedges = hit_bShapeNedges(shape);

	// LINE 1
	// 1.1 Title (72 characters)
	fprintf(file,"%-72s","HitMap HB Export");
	// 1.2, (8 characters)
	fprintf(file,"%-8s","key");
	fprintf(file,"\n");

	// LINE 2
	//2.1 TOTCRD, total number of data lines, (14 characters)
	//2.2 PTRCRD, number of data lines for pointers, (14 characters)
	//2.3 INDCRD, number of data lines for row or variable indices, (14 characters)
	int ptrcrd = (int) ceil(((double) nvertices + 1) / NUMBERS_LINE );
	int indcrd = (int) ceil((double) nedges / NUMBERS_LINE );
	fprintf(file,"%14d",ptrcrd + indcrd);
	fprintf(file,"%14d",ptrcrd);
	fprintf(file,"%14d",indcrd);
	//2.4 VALCRD, number of data lines for numerical values of matrix entries, (14 characters)
	//2.5 RHSCRD, number of data lines for right hand side vectors, starting guesses, and solutions, (14 characters)
	fprintf(file,"%14d",0);
	fprintf(file,"%14s","");
	fprintf(file,"\n");

	// LINE 2
	// MXTYPE, matrix type, (3 characters)
	// pua: Pattern Unsymmetric Assembled
	fprintf(file,"%3s","pua");
	// blank space, (11 characters)
	fprintf(file,"%11s","");
	// NROW, integer, number of rows or variables, (14 characters)
	fprintf(file,"%14d", nvertices);
	// NCOL, integer, number of columns or elements, (14 characters)
	fprintf(file,"%14d", nvertices);
	// NNZERO, number of nonzero entries. (14 characters)
	fprintf(file,"%14d", nedges);
	// NELTVL, this is 0. (14 characters)
	fprintf(file,"%14d", 0);
	fprintf(file,"\n");


	// LINE 4
	// PTRFMT, FORTRAN I/O format for pointers, (16 characters)
	fprintf(file,"%-16s","(10I8)");
	// INDFMT, FORTRAN I/O format for row or variable indices, (16 characters)
	fprintf(file,"%-16s","(10I8)");
	// VALFMT, FORTRAN I/O format for matrix entries, (20 characters)
	fprintf(file,"%-20s","");
	// RHSFMT, FORTRAN I/O format for right hand sides, initial guesses, and solutions, (20 characters)
	fprintf(file,"%-20s","");
	//fprintf(file,"\n");


	int indexId = 0;
	int vertex;
	hit_bShapeVertexIterator(vertex,shape){

		if(vertex % NUMBERS_LINE == 0 ) fprintf(file,"\n");
		fprintf(file, "%8d",indexId+1);

		int edge;
		hit_bShapeEdgeIterator(edge,shape,vertex){
			indexId++;
		}

	}
	if(vertex % NUMBERS_LINE == 0 ) fprintf(file,"\n");
	fprintf(file, "%8d",indexId+1);

	int edge_count = 0;
	hit_bShapeVertexIterator(vertex,shape){

		int edge;
		hit_bShapeEdgeIterator(edge,shape,vertex){

			if(edge_count % NUMBERS_LINE == 0 ) fprintf(file,"\n");
			edge_count++;

			int target = hit_bShapeEdgeTarget(shape,edge);

			fprintf(file, "%8d",target+1);

		}

	}



	fclose(file);

}





int hit_fileHBVerticesInternal(const char * hbfile, int rank, const char * file, int line){

	// Check if the current processor is not root.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",file,line);
		return 0;
	}

	/* 1. READ HB HEADER */
	// Rows and columns sizes.
	int M, N;
	// Nonzero elements,
	int nz;
	// Matrix type.
	char * type;
	// Number of right-hand-sides
	int Nrhs;

	/* 1.1 Read HB header */
	if( !readHB_info(hbfile, &M, &N, &nz, &type, &Nrhs) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing file header","",file,line);
		return HIT_ERROR;
	}

	/* 1.3. Reject non-squeare matrices */
	if( M != N ) {
		hit_errInternal(__FUNCTION__,"Recognized Harwell-Boeing matrices must be square","",file,line);
		return HIT_ERROR;
	}

	// Free type string.
	free(type);

	return N;
}



int hit_fileHBReadDenseInternal(const char * fileName, int rank, void * tileP, const char * file, int line){

	// Check if the current processor is not root.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",file,line);
		return HIT_ERROR;
	}

	int i,j;

	/* 0. CHECK TILE */
	hit_tileNewType(int);

	HitTile_int *tile = (HitTile_int *)tileP;
	if(hit_tileIsNull(*tile)){
		hit_errInternal(__FUNCTION__,"The tile must be allocated","",file,line);
	}


	/* 1. READ HB HEADER */
	// Rows and columns sizes.
	int M, N;
	// Nonzero elements,
	int nz;
	// Matrix type.
	char * type;
	// Number of right-hand-sides
	int Nrhs;

	/* 1.1 Read HB header */
	if( !readHB_info(fileName, &M, &N, &nz, &type, &Nrhs) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing file header","",file,line);
		return HIT_ERROR;
	}

#ifdef DEBUG
	fprintf(stderr,"DATOS: M: %d, N: %d, nz: %d, type: %s, Nrhs: %d\n",M,N,nz,type,Nrhs);
#endif

	/* 1.2. Reject Righ-hand side matrices */
	if( Nrhs > 0 ) {
		hit_errInternal(__FUNCTION__,"Recognized Harwell-Boeing files must not have RHS","",file,line);
		return HIT_ERROR;
	}

	/* 1.3. Reject non-square matrices */
	if( M != N ) {
		hit_errInternal(__FUNCTION__,"Recognized Harwell-Boeing matrices must be square","",file,line);
		return HIT_ERROR;
	}

	// Free type string.
	free(type);



	/* 2. READ THE MATRIX */

	// Row and column pointer.
	int	*colptr = NULL, *rowind = NULL;
	// Array with the values.
	double	*val = NULL;


	/* 2. Read the matrix allocating storage arrays */
	if( !readHB_newmat_double(fileName, &M, &N, &nz, &colptr, &rowind, &val) ) {
		hit_errInternal(__FUNCTION__,"Reading Harwell-Boeing matrix data","",file,line);
		return HIT_ERROR;
	}

#ifdef DEBUG_LEVEL2
	/*
	{int i;
	for (i=0; i<N+1; i++) {
		fprintf(stderr,"colptr[%i] = %i\n",i,colptr[i]-1);
	}
	for (i=0; i<nz; i++) {
		fprintf(stderr,"rowind[%i] = %i\n",i,rowind[i]-1);
	}}
	*/
	{int i,j;
	printf("\t");
	for(i=0;i<N;i++){
		int begin = colptr[i]-1;
		int end = colptr[i+1]-1;
		for(j=begin;j<end;j++){
			printf(" %d ", rowind[j]-1);
		}
		printf("|\n\t");
	}
	printf("\n");}
#endif


	/* 3. LOAD THE MATRIX IN THE TILE STRUCTURE */

	/* 3.1 Clean the tile */
	int zero = 0;
	hit_tileFill(tileP,&zero);

	/* 3.2 Fill the tile */
	for(i=0; i<N; i++) {
		int cstyleIni = colptr[i]-1;
		int cstyleEnd = colptr[i+1]-1;

		for(j=cstyleIni; j<cstyleEnd; j++) {

			hit_tileElemAt2(*tile,i,rowind[j]-1) = 1;
			hit_tileElemAt2(*tile,rowind[j]-1,i) = 1;
			//Mat[i][rowind[j]-1] = 1;
			//Mat[rowind[j]-1][i] = 1;
		}
	}


	/* 4. FREE DATA STRUCTURES */
	free(colptr);
	free(rowind);
	free(val);

	/* END */
	return HIT_OK;

}


HitShape hit_fileCSRReadInternal(const char * csrfile, int rank, const char * cfile, int line){

	// If the current processor is not root return a null shape.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",cfile,line);
		return HIT_CSR_SHAPE_NULL;
	}

	FILE * file = fopen(csrfile,"r");
	if(file == NULL) return HIT_CSR_SHAPE_NULL;


	HitShape shape = HIT_CSR_SHAPE_NULL;
	int n;

	char c = (char) getc(file);
	while( c != EOF ){

		if(c == '#' || c == ' ' || c == '\n'){
			while( c != '\n' ) c = (char) getc(file);
		} else  {
			ungetc(c,file);
			break;
		}
		c = (char) getc(file);
	}


	int r;
	r = fscanf(file,"%d\n",&n);
	if(r != 1) hit_errInternal(__func__, "Error reading CSR format", "", __FILE__, __LINE__ );


	hit_cShapeNvertices(shape) = n;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(hit_cShapeXadj(shape), sizeof(int) * (size_t) (n+1),int*);
	hit_malloc(hit_cShapeXadj(shape), int, n+1 );

	int i;
	for(i=0;i<n+1;i++){
		r = fscanf(file,"%d",& (hit_cShapeXadj(shape)[i]) );
		if(r != 1) hit_errInternal(__func__, "Error reading CSR format", "", __FILE__, __LINE__ );
	}

	int nedges = hit_cShapeXadj(shape)[n];
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(hit_cShapeAdjncy(shape), sizeof(int) * (size_t) nedges, int*);
	hit_malloc(hit_cShapeAdjncy(shape), int, nedges );

	for(i=0;i<nedges;i++){
		r = fscanf(file,"%d",& (hit_cShapeAdjncy(shape)[i]) );
		if(r != 1) hit_errInternal(__func__, "Error reading CSR format", "", __FILE__, __LINE__ );
	}

	hit_nameListCreate(&hit_cShapeNameList(shape,0), n);
	// second list


	fclose(file);

	return shape;
}




void hit_fileCSRWriteInternal(const char * csrfile, HitShape shape, int rank, const char * cfile, int line){

	// If the current processor is not root return a null shape.
	if(rank != 0){
		hit_warnInternal(__FUNCTION__,"Only root processor should use this function","",cfile,line);
		return;
	}

	FILE * file = fopen(csrfile,"w");
	if(file == NULL) return;


	fprintf(file,"# CSR graph generated by the Hitmap library\n");
	fprintf(file,"# Format:\n");
	fprintf(file,"#  - Number of vertices.\n");
	fprintf(file,"#  - Xadj list.\n");
	fprintf(file,"#  - Adjncy list.\n");
	fprintf(file,"#  - Vertices name list.\n\n");


	// Print the number of vertices
	int n = hit_cShapeNvertices(shape);
	fprintf(file,"%d\n",n);

	// Print xadj.
	int i;
	for(i=0;i<n+1;i++){
		fprintf(file,"%d ",hit_cShapeXadj(shape)[i]);
	}
	fprintf(file,"\n");

	// Print adjncy
	int nedges = hit_cShapeXadj(shape)[n];
	for(i=0;i<nedges;i++){
		fprintf(file,"%d ",hit_cShapeAdjncy(shape)[i]);
	}
	fprintf(file,"\n");


	fclose(file);

}
