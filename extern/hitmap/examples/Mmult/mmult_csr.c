/**
 * Sparse matrix - vector multiplication benchmark (with CSR structure)
 *
 * @file mmult_csr.c
 * @author Javier Fresno
 * @date 10-7-2012
 * @version 1.0 (17-4-2012)
 * @version 1.1 (10-7-2012)
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
 * Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include <hit_com.h>
#include <sys/time.h>
#include <unistd.h>

/**
 * Layout to distribute the matrix
 */
HitLayout lay = HIT_LAYOUT_NULL_STATIC;

/**
 * Layout to distribute the vector
 */
HitLayout lay_v = HIT_LAYOUT_NULL_STATIC;

/**
 * Hit double tile
 */
hit_tileNewType(double);

/**
 * Number of iterations
 */
int iter = 100;



/**
 * Multiply the matrix M and the vector b and return the vector c.
 * @param M A sparse matrix
 * @param b A dense vector
 * @param c A dense vector as result
 */
void mult(HitTile_double M, HitTile_double b, HitTile_double c){

	int i;
	hit_cShapeRowIterator(i,hit_tileShape(M)){

		int j;
		double sum = 0.0;
		hit_cShapeColumnIterator(j,hit_tileShape(M),i){

			// Get the name of the column.
			int index2 = hit_cShapeEdgeTarget(hit_tileShape(M),j);

			// Multiply element M_ij * b_j
			sum +=  hit_mcTileElemIteratorAt(M,i,j) * hit_tileElemAtNoStride(b,1,index2);
		}
		// Set the elemetn c_i
		hit_tileElemAtNoStride(c,1,i) = sum;

	}
}


/**
 * Calculate the norm of a vector
 * @param v The vector
 * @return The vector norm
 */
double vector_norm(HitTile_double v){

	HitTile_double norm, sum_norm;

  	hit_tileDomainAlloc(&norm, double, 1,1);
  	hit_tileDomainAlloc(&sum_norm, double, 1,1);

  	hit_tileElemAt(norm,1,0) = 0.0;

  	// Caculate the partial norm
	int i;
	for(i=0; i<hit_tileDimCard(v,0); i++){
		hit_tileElemAt(norm,1,0) += pow(hit_tileElemAt(v,1,i),2);
	}

	HitOp op_sum;
	hit_comOp(hit_comOpSumDouble,op_sum);

	// Reduce the norm to the root processor
    HitRanks root = {{0,0,0,-1}};
  	HitCom com_sum = hit_comReduce(lay, root, &norm, &sum_norm, HIT_DOUBLE, op_sum);
  	hit_comDo(&com_sum);
	hit_comFree(com_sum);

	double res = 0;

  	if( hit_Rank == 0  ){
  		res = sqrt(hit_tileElemAt(sum_norm,1,0));
  	}

  	// Free the tiles
	hit_tileFree(norm);
	hit_tileFree(sum_norm);

	return res;
}


/**
 * Print help msg.
 * @param name Name of the program
 */
void print_help(char * name){
	printf("%s [-n ITERATIONS] FILE \n",name);
	printf("  -n    number of iterations\n");
	printf("  FILE  input HB matrix file\n");
}


/**
 * Init the matrix structure
 * @param argc
 * @param argv
 */
void init_matrix(int argc, char ** argv, HitShape * shape){

	if(argc > 1){

		// Argument.
		char c;

		// Parse the command-line arguments.
		while ((c = (char) getopt (argc, argv, "hn:")) != -1)
		switch (c) {
			case 'n':
				sscanf(optarg,"%d",&iter);
				break;
			case 'h':
				if(hit_Rank == 0) print_help(argv[0]);
				hit_comFinalize();
				exit(EXIT_SUCCESS);
			default:
				abort ();
				break;
		}

		char * matrix_file = argv[optind];

		if(hit_Rank != 0) return;

		printf("# Matrix: %s\n",matrix_file);
		printf("# Iterations %d\n",iter);
		*shape = hit_fileHBMatrixRead(matrix_file);

		if(hit_cShapeCard(*shape,0) != hit_cShapeCard(*shape,1)){
			printf("Matrix must to be square");
			exit(EXIT_FAILURE);
		}

	} else {

		if(hit_Rank != 0) return;

		int i;
		for(i=0; i<5; i++){
			hit_cShapeAddElem(shape, i, i);
		}
		hit_cShapeAddElem(shape, 0, 3);

	}
}



/**
 * Main function.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char ** argv) {

	// Init Hitmap communications.
	hit_comInit(&argc,&argv);

	// Timers.
	HitClock init_time, comp_time;
	hit_clockStart(init_time);

	// Load the global matrix.
	HitShape shape_global = HIT_CSR_SHAPE_NULL;
	init_matrix(argc, argv, &shape_global);

	// Create the topology object.
	HitTopology topo = hit_topology(plug_topPlain);

	// Distribute the matrix among the processors.
	lay = hit_layout(plug_laySparseRows,topo,&shape_global);

	// Get the shape for the local matrix.
	HitShape shape = hit_layShape(lay);

	// Allocate the matrix.
	HitTile_double M;
	hit_mcTileDomainShapeAlloc(&M, double, shape);

	// Init the matrix values.
	int i;
	hit_cShapeRowIterator(i,hit_tileShape(M)){
		int j;
		hit_cShapeColumnIterator(j,hit_tileShape(M),i){
			hit_mcTileElemIteratorAt(M,i,j) = 1.0 / (double) hit_cShapeNColsRow(shape,i);
		}
	}

	// Create the shape for the b vector.
	//HitShape shape_b = hit_shape(1,hit_sig(0,hit_cShapeCard(shape_global,0)-1,1));
	HitShape shape_b = hit_shape(1,hit_sig(0,hit_cShapeCard(shape,1)-1,1));

	// Distribute the vector with the result.
	HitShape full_vector = hit_shape(1,hit_sig(0,hit_cShapeCard(shape_global,0)-1,1));
	lay_v = hit_layout(plug_layBlocks,topo,full_vector);
	HitShape shape_c = hit_layShape(lay_v);

	// Allocate the two vectors
	HitTile_double b,c;

	hit_tileDomainShapeAlloc(&b,double,shape_b);
	hit_tileDomainShapeAlloc(&c,double,shape_c);

	// Init the vector
	for(i=0; i< hit_shapeSigCard(shape_c,0); i++){
		hit_tileElemAt(c,1,i) = hit_shapeSig(shape_c,0).begin + i;
	}

	// Create the communication to gather the result vector.
	HitPattern pat = hit_pattern(HIT_PAT_UNORDERED);
	hit_patMatMult(&pat,lay,shape_global,shape,&c,&b,HIT_DOUBLE);

	// Comunicate
	hit_patternDo(pat);

	hit_clockStop(init_time);
	hit_clockStart(comp_time);

	// Main loop
	for(i=0; i<iter; i++){
		mult(M, b, c);
		hit_patternDo(pat);
	}

	hit_clockStop(comp_time);
	hit_clockWorldReduce(init_time);
	hit_clockWorldReduce(comp_time);

	// Obtain the norm of the vector.
	double norm = vector_norm(c);

	// Print times
	if(hit_Rank == 0){
		printf("# Result norm: %5.2f\n",norm);
		printf("# Init time: %lf\n",hit_clockGetSeconds(init_time));
		printf("# Comp time: %lf\n",hit_clockGetSeconds(comp_time));
		printf("# Total time: %lf\n",hit_clockGetSeconds(init_time)+hit_clockGetSeconds(comp_time));

	}

	// Free resources
	hit_layFree(lay);
	hit_layFree(lay_v);
	hit_topFree(topo);
	hit_shapeFree(shape);
	hit_tileFree(M);
	hit_tileFree(b);
	hit_tileFree(c);

	hit_comFinalize();

	return 0;
}


