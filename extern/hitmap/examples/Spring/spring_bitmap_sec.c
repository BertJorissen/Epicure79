/**
 * Calculate the stable position of a spring system.
 * Sequential hitmap version of the benchmark using a bitmap structure.
 *
 * @file spring_bitmap_sec.c
 * @author Javier Fresno
 * @date 19-6-2012
 * @version 1.0 (18-6-2012)  First version based on spring_csr_sec.
 *
 * This is the same code from spring_csr_sec with the following changes:
 * Changed hit_cShape* -> hit_bShape*
 * Changed hit_gcTile* -> hit_gbTile*
 * Changed HIT_CSR_SHAPE_NULL -> HIT_BITMAP_SHAPE_NULL
 * Changed hit_fileHBRead -> hit_fileHBReadBitmap
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
#include <unistd.h>
#include <string.h>


/** Number of dimensions. */
#define D (3)

/** Spring constant. */
#define K (1)
/** Spring length. */
#define L (0.001)

/** Percentage of fixed points */
#define P_FIXED 10

/** Default number of iterations for the Newton-Raphson method. */
#define ITER1 10
/** Default number of iterations for the Jacobi method. */
#define ITER2 100

/** Number of iterations for the Newton-Raphson method. */
int iter1 = ITER1;
/** Number of iterations for the Jacobi method. */
int iter2 = ITER2;


/**
 * Vector typedef.
 */
typedef struct {
	double v[D];
} Vector;

/**
 * Matrix typedef.
 */
typedef struct {
	double m[D][D];
} Matrix;


// Define the tile_Vector type.
hit_tileNewType(Vector);

// Define the tile_Matrix type.
hit_tileNewType(Matrix);

// Define the tile_int type.
hit_tileNewType(int);

/** Gradient array part for each node. */
HitTile_Vector gradient;

/** Hessian matrix part for each edge. */
HitTile_Matrix hessian;

/** Vector with the position increase. */
HitTile_Vector e;

/** Vector with the new position increase. */
HitTile_Vector new_e;

/** Hit shape defining the spring structure. */
HitShape shape;

/** Tile with the node values. */
HitTile_Vector graph;

/** Tile with the fixed/free status */
HitTile_int fixed;

/*
 * Function to calculate the gradient and hessian for a given node.
 * @param vertex The vertex index.
 */
void calculate_GH(int vertex);

/**
 * Function that solves a iteration of the jacobi method for a given node.
 * @param vertex The vertex index.
 */
void solve_system_iter(int vertex);


#ifdef DEBUG
/**
 * Debug function to print matrices.
 * @param m A matrix.
 */
void print_matrix(Matrix m){

	int i,j;
	for(i=0;i<D;i++){
		for(j=0;j<D;j++){
			printf("%.2lf\t",m.m[i][j]);
		}
		printf("\n");
	}
}

/**
 * Debug function to print a vector.
 * @param v A vector.
 */
void print_vector(Vector v){

	int i;
	for(i=0;i<D;i++){
		printf("%.2lf\t",v.v[i]);
	}
	printf("\n");
}
#endif

/**
 * Clear a vector.
 */
#define clearV(vv) clearVInternal((vv).v)
#define clearVInternal(v) \
	((v)[0] = (v)[1] = (v)[2] = 0.0 )

/**
 * Copy a vector.
 */
#define cpyV(r,a) cpyVInternal((r).v,(a).v)
#define cpyVInternal(r,a) \
	{(r)[0] = (a)[0]; (r)[1] = (a)[1]; (r)[2] = (a)[2]; }

/**
 * Add two vectors, r = a + b.
 */
#define addV(r,a,b) addVInternal((r).v,(a).v,(b).v)
#define addVInternal(r,a,b) \
	{(r)[0] = (a)[0] + (b)[0]; (r)[1] = (a)[1] + (b)[1]; (r)[2] = (a)[2] + (b)[2]; }


/**
 * Subtract two vectors, r = a - b.
 */
#define subV(r,a,b) subVInternal((r).v,(a).v,(b).v)
#define subVInternal(r,a,b) \
	{(r)[0] = (a)[0] - (b)[0]; (r)[1] = (a)[1] - (b)[1]; (r)[2] = (a)[2] - (b)[2]; }

/**
 * Calculate the norm of a vector
 */
#define norm(vv) normInternal((vv).v)
#define normInternal(v) \
	(sqrt( pow((v)[0],2) + pow((v)[1],2) + pow((v)[2],2) ))

/**
 * Multiplication scalar x vector.
 */
#define multSV(r,s,vv) multSVInternal((r).v,s,(vv).v)
#define multSVInternal(r,s,v) \
	{(r)[0] = (s) * (v)[0]; (r)[1] = (s) * (v)[1]; (r)[2] = (s) * (v)[2]; }

/**
 * Clear a matrix.
 */
#define clearM(mm) clearMInternal((mm).m)
#define clearMInternal(m) \
	((m)[0][0] = (m)[0][1] = (m)[0][2] = \
	 (m)[1][0] = (m)[1][1] = (m)[1][2] = \
	 (m)[2][0] = (m)[2][1] = (m)[2][2] = 0.0 )


/**
 * Calculate the determinant.
 *
 * a b c
 * d e f
 * g h i
 *
 * det = aei + bfg + cdh - afh - bdi - ceg
 */
#define det(mm) detInternal((mm).m)
#define detInternal(m) \
	(  (m)[0][0] * (m)[1][1] * (m)[2][2] \
	 + (m)[0][1] * (m)[1][2] * (m)[2][0] \
	 + (m)[0][2] * (m)[1][0] * (m)[2][1] \
	 - (m)[0][0] * (m)[1][2] * (m)[2][1] \
	 - (m)[0][1] * (m)[1][0] * (m)[2][2] \
	 - (m)[0][2] * (m)[1][1] * (m)[2][0] )

/**
 * Initialization of a matrix.
 */
#define init_matrix(mm,a,b,c,d,e,f,g,h,i) init_matrixInternal((mm).m,a,b,c,d,e,f,g,h,i)
#define init_matrixInternal(m,a,b,c,d,e,f,g,h,i) \
	{(m)[0][0] = a; (m)[0][1] = b; (m)[0][2] = c; \
	 (m)[1][0] = d; (m)[1][1] = e; (m)[1][2] = f; \
	 (m)[2][0] = g; (m)[2][1] = h; (m)[2][2] = i; }

/**
 * Multiplication scalar x matrix.
 */
#define multSM(r,s,mm) multSMInternal((r).m,s,(mm).m)
#define multSMInternal(r,s,m) \
	{(r)[0][0] = (s) * (m)[0][0]; (r)[0][1] = (s) * (m)[0][1]; (r)[0][2] = (s) * (m)[0][2]; \
	 (r)[1][0] = (s) * (m)[1][0]; (r)[1][1] = (s) * (m)[1][1]; (r)[1][2] = (s) * (m)[1][2]; \
	 (r)[2][0] = (s) * (m)[2][0]; (r)[2][1] = (s) * (m)[2][1]; (r)[2][2] = (s) * (m)[2][2]; }

/**
 * Multiplication scalar x identity matrix.
 */
#define multSI(r,s) multSIInternal((r).m,s)
#define multSIInternal(r,s) \
	{(r)[0][0] = (s); (r)[0][1] = 0;   (r)[0][2] = 0;   \
	 (r)[1][0] = 0;   (r)[1][1] = (s); (r)[1][2] = 0;   \
	 (r)[2][0] = 0;   (r)[2][1] = 0;   (r)[2][2] = (s); }


/**
 * Add two matrix, r = a + b.
 */
#define addM(r,a,b) addMInternal((r).m,(a).m,(b).m)
#define addMInternal(r,a,b) \
	{(r)[0][0] = (a)[0][0] + (b)[0][0]; (r)[0][1] = (a)[0][1] + (b)[0][1]; (r)[0][2] = (a)[0][2] + (b)[0][2]; \
	 (r)[1][0] = (a)[1][0] + (b)[1][0]; (r)[1][1] = (a)[1][1] + (b)[1][1]; (r)[1][2] = (a)[1][2] + (b)[1][2]; \
	 (r)[2][0] = (a)[2][0] + (b)[2][0]; (r)[2][1] = (a)[2][1] + (b)[2][1]; (r)[2][2] = (a)[2][2] + (b)[2][2]; }

/**
 * Multiply a vector with a transposed vector.
 * the result is a matrix.
 */
#define multVVt(r,a,b) multVVtInternal((r).m,(a).v,(b).v)
#define multVVtInternal(r,a,b) \
	{(r)[0][0] = (a)[0] * (b)[0]; (r)[0][1] = (a)[0] * (b)[1]; (r)[0][2] = (a)[0] * (b)[2]; \
	 (r)[1][0] = (a)[1] * (b)[0]; (r)[1][1] = (a)[1] * (b)[1]; (r)[1][2] = (a)[1] * (b)[2]; \
	 (r)[2][0] = (a)[2] * (b)[0]; (r)[2][1] = (a)[2] * (b)[1]; (r)[2][2] = (a)[2] * (b)[2]; }


/**
 * Multiplies a matrix with a vector.
 */
#define multMV(r,mm,vv) multMVInternal((r).v,(mm).m,(vv).v)
#define multMVInternal(r,m,v) \
	{(r)[0] = (m)[0][0] * (v)[0] + (m)[0][1] * (v)[1] + (m)[0][2] * (v)[2]; \
	 (r)[1] = (m)[1][0] * (v)[0] + (m)[1][1] * (v)[1] + (m)[1][2] * (v)[2]; \
	 (r)[2] = (m)[2][0] * (v)[0] + (m)[2][1] * (v)[1] + (m)[2][2] * (v)[2]; }


/**
 * Inverses a matrix.
 */
#define inv(r,mm) invInternal((r).m,(mm).m)
#define invInternal(r,m) \
	{(r)[0][0] =   (m)[1][1] * (m)[2][2] - (m)[1][2] * (m)[2][1]; \
	 (r)[0][1] = - (m)[0][1] * (m)[2][2] + (m)[0][2] * (m)[2][1]; \
	 (r)[0][2] =   (m)[0][1] * (m)[1][2] - (m)[0][2] * (m)[1][1]; \
	 (r)[1][0] = - (m)[1][0] * (m)[2][2] + (m)[1][2] * (m)[2][0]; \
	 (r)[1][1] =   (m)[0][0] * (m)[2][2] - (m)[0][2] * (m)[2][0]; \
	 (r)[1][2] = - (m)[0][0] * (m)[1][2] + (m)[0][2] * (m)[1][0]; \
	 (r)[2][0] =   (m)[1][0] * (m)[2][1] - (m)[1][1] * (m)[2][0]; \
	 (r)[2][1] = - (m)[0][0] * (m)[2][1] + (m)[0][1] * (m)[2][0]; \
	 (r)[2][2] =   (m)[0][0] * (m)[1][1] - (m)[0][1] * (m)[1][0]; \
	 multSMInternal(r,1/detInternal(m),r); }

/**
 * Multiplies two matrices.
 */
#define multMM(r,a,b) multMMInternal((r).m,(a).m,(b).m) 
#define multMMInternal(r,a,b) \
	{(r)[0][0] = (a)[0][0] * (b)[0][0] + (a)[0][1] * (b)[1][0] + (a)[0][2] * (b)[2][0]; \
	 (r)[0][1] = (a)[0][0] * (b)[0][1] + (a)[0][1] * (b)[1][1] + (a)[0][2] * (b)[2][1]; \
	 (r)[0][2] = (a)[0][0] * (b)[0][2] + (a)[0][1] * (b)[1][2] + (a)[0][2] * (b)[2][2]; \
	 (r)[1][0] = (a)[1][0] * (b)[0][0] + (a)[1][1] * (b)[1][0] + (a)[1][2] * (b)[2][0]; \
	 (r)[1][1] = (a)[1][0] * (b)[0][1] + (a)[1][1] * (b)[1][1] + (a)[1][2] * (b)[2][1]; \
	 (r)[1][2] = (a)[1][0] * (b)[0][2] + (a)[1][1] * (b)[1][2] + (a)[1][2] * (b)[2][2]; \
	 (r)[2][0] = (a)[2][0] * (b)[0][0] + (a)[2][1] * (b)[1][0] + (a)[2][2] * (b)[2][0]; \
	 (r)[2][1] = (a)[2][0] * (b)[0][1] + (a)[2][1] * (b)[1][1] + (a)[2][2] * (b)[2][1]; \
	 (r)[2][2] = (a)[2][0] * (b)[0][2] + (a)[2][1] * (b)[1][2] + (a)[2][2] * (b)[2][2]; }


/**
 * Solve a linear system using the jacobi method.
 * Ax=b.
 * @param A The coefficients matrix.
 * @param b The constant terms vector.
 * @return The variable vector (s).
 */
Vector solve_jacobi(Matrix A, Vector b){

// Number of iteration of the jacobi iterative method.
#define ITER_SJ 10

	Vector x;

	// The new approximation.
	Vector x_1;

	// Clear the vectors.
	clearV(x);
	clearV(x_1);

	int iter;
	int i,j;
	
	// Iterative method.
	for(iter=0; iter<ITER_SJ; iter++){
		for(i=0;i<D;i++){

			if( A.m[i][i] == 0 ) continue;

			// calculate the new value.
			x_1.v[i] = 0.0;
			for(j=0;j<D;j++){
				if(i != j) x_1.v[i] += A.m[i][j] * x.v[j];
			} 
			x_1.v[i] = (b.v[i] - x_1.v[i]) / A.m[i][i];	
		
		}
		cpyV(x,x_1);
	}

	return x;
}



/**
 * Solve a linear system.
 * It calculates the inverse matrix.
 * If the matrix is singular, it use the jacobi method to
 * get an approximation.
 * Ax=b.
 * @param A The coefficients matrix.
 * @param b The constant terms vector.
 * @return The variable vector (x).
 */
Vector solve(Matrix A, Vector b){

	Vector x;

	double detA = det(A);

	// If the matrix has a determinant, we can get the inverse.
	if(detA != 0){

		Matrix InvA;
		inv(InvA,A);
		multMV(x,InvA,b);
	} else {
		//printf("#Warning: singular matrix\n");
		x = solve_jacobi(A,b);
	}
	return x;
}

/**
 * Allocate memory of all the variables
 */
void init_structures(){

	hit_gbTileDomainShapeAlloc(&e, Vector, shape, HIT_VERTICES);
	hit_gbTileDomainShapeAlloc(&new_e, Vector, shape, HIT_VERTICES);
	hit_gbTileDomainShapeAlloc(&gradient, Vector, shape, HIT_VERTICES);
	hit_gbTileDomainShapeAlloc(&hessian, Matrix, shape, HIT_EDGES);
}

/**
 * Release the memory
 */
void free_structures(){

	hit_tileFree(graph);
	hit_tileFree(fixed)
	hit_tileFree(e);
	hit_tileFree(new_e);
	hit_tileFree(gradient);
	hit_tileFree(hessian)

	hit_shapeFree(shape);
}


/**
 * Command-line help
 * @param name program name
 */
void print_help(char * name){
	printf("%s [-n NEWTON METHOD ITERATIONS] [-j JACOBI METHOD ITERATIONS] FILE \n",name);

	printf("  -n NEWTON METHOD ITERATIONS   number of iterations (default 10)\n");
	printf("  -j JACOBI METHOD ITERATIONS   number of iterations (default 100)\n");
	printf("  FILE                          input graph file\n");

}



/**
 * Generate random coordinates for the spring nodes.
 * It also set the fixed nodes
 */
void random_coordinates(){

	srandom(0);
	if(hit_Rank != 0) return;

	int n = hit_bShapeNvertices(shape);

	int j,i;

	// Set the coordinates
	for(j=0;j<D;j++){
		for(i=0;i<n;i++){
			double number = (double) random();
			hit_gbTileVertexAt(graph,i).v[j] = number;
		}
	}

 	srandom(0);
	for(i=0;i<n;i++){

		if ((random() % 100) < P_FIXED){
			hit_gbTileVertexAt(fixed,i) = 1;
		} else {
			hit_gbTileVertexAt(fixed,i) = 0;
		}
	}


}






/**
 * Init the graph
 */
void init_graph(int argc, char ** argv){

	// Define the spring structure.
	shape = HIT_BITMAP_SHAPE_NULL;

	// Argument.
	char c;

	// Parse the command-line arguments.
	while ((c = (char) getopt (argc, argv, "hn:j:")) != -1)
	switch (c) {
		case 'n':
			sscanf(optarg,"%d",&iter1);
			break;
		case 'j':
			sscanf(optarg,"%d",&iter2);
			break;
		case 'h':
			print_help(argv[0]);
			exit(EXIT_SUCCESS);
		default:
			abort ();
			break;
	}


	if(optind == (argc-1)){

		// Graph file name.
		char * graph_file = argv[optind];

		printf("# Graph: %s\n",graph_file);
		printf("# iterations %d x %d\n",iter1,iter2);

		shape = hit_fileHBReadBitmap(graph_file);

		printf("# Vertices %d\n",hit_bShapeNvertices(shape));

		// Allocate memory
		hit_gbTileDomainShapeAlloc(&graph, Vector, shape, HIT_VERTICES);
		hit_gbTileDomainShapeAlloc(&fixed,    int, shape, HIT_VERTICES);


		// Random coordinates
		random_coordinates();

		int i;
		int nfixed = 0;
		for(i=0;i<hit_bShapeNvertices(shape);i++){
			nfixed += hit_gbTileVertexAt(fixed,i);
		}

		printf("# N fixed: %d\n",nfixed);



	} else {

		iter1 = iter2 = 3;

		hit_bShapeAddEdge2(&shape,0,1);
		hit_bShapeAddEdge2(&shape,1,2);
		hit_bShapeAddEdge2(&shape,2,3);
		hit_bShapeAddEdge2(&shape,4,5);
		hit_bShapeAddEdge2(&shape,5,6);
		hit_bShapeAddEdge2(&shape,6,7);
		hit_bShapeAddEdge2(&shape,0,4);
		hit_bShapeAddEdge2(&shape,1,5);
		hit_bShapeAddEdge2(&shape,2,6);
		hit_bShapeAddEdge2(&shape,3,7);
		hit_bShapeAddEdge2(&shape,1,4);
		hit_bShapeAddEdge2(&shape,2,5);
		
		// Include each (i,i) edge.
		hit_bShapeAddEdge2(&shape,0,0);
		hit_bShapeAddEdge2(&shape,1,1);
		hit_bShapeAddEdge2(&shape,2,2);
		hit_bShapeAddEdge2(&shape,3,3);
		hit_bShapeAddEdge2(&shape,4,4);
		hit_bShapeAddEdge2(&shape,5,5);
		hit_bShapeAddEdge2(&shape,6,6);
		hit_bShapeAddEdge2(&shape,7,7);
		
		// Allocate memory
		hit_gbTileDomainShapeAlloc(&graph, Vector, shape, HIT_VERTICES);
		hit_gbTileDomainShapeAlloc(&fixed,    int, shape, HIT_VERTICES);

		// Set initial positions
		Vector v0 = {{0,0,0}};
		hit_gbTileVertexAt(graph,0) = v0;
		Vector v1 = {{100,0,0}};
		hit_gbTileVertexAt(graph,1) = v1;
		Vector v2 = {{200,0,0}};
		hit_gbTileVertexAt(graph,2) = v2;
		Vector v3 = {{300,0,0}};
		hit_gbTileVertexAt(graph,3) = v3;
		Vector v4 = {{0,100,200}};
		hit_gbTileVertexAt(graph,4) = v4;
		Vector v5 = {{100,100,0}};
		hit_gbTileVertexAt(graph,5) = v5;
		Vector v6 = {{200,100,0}};
		hit_gbTileVertexAt(graph,6) = v6;
		Vector v7 = {{300,100,0}};
		hit_gbTileVertexAt(graph,7) = v7;

		// Set the fix/free status
		hit_gbTileVertexAt(fixed,0) = 1;
		hit_gbTileVertexAt(fixed,1) = 0;
		hit_gbTileVertexAt(fixed,2) = 0;
		hit_gbTileVertexAt(fixed,3) = 1;
		hit_gbTileVertexAt(fixed,4) = 1;
		hit_gbTileVertexAt(fixed,5) = 0;
		hit_gbTileVertexAt(fixed,6) = 0;
		hit_gbTileVertexAt(fixed,7) = 1;

	}

}

/**
 * Calculate the norm of the whole gradient vector.
 * @return the norm
 */
double gradient_norm(){

	double norm = 0.0;

	int vertex;

	hit_bShapeVertexIterator(vertex,shape) {
			if (!hit_gbTileVertexAt(fixed,vertex)){

				calculate_GH(vertex);

				Vector Gi = hit_gbTileVertexAt(gradient,vertex);

				int d;
				for(d=0;d<D;d++){
					norm += pow(Gi.v[d],2);
				}
			}
		}

	return sqrt(norm);
}




/**
 * Main function
 */
int main(int argc, char ** argv) {

	int i;
	int vertex;
	HitClock init_time, comp_time;

	hit_comInit(&argc,&argv);

	hit_clockStart(init_time);
	init_graph(argc,argv);

	// Allocate memory of all the variables
	init_structures();

	// printf("# Initial gradient norm: %lf\n",gradient_norm());
	hit_clockStop(init_time);
	hit_clockStart(comp_time);

	// Main loop for the newton method
	for(i=0;i<iter1;i++){
		printf("# iter: %d/%d\n",i,iter1);

		// Iterate trough all the vertices and
		// obtain the gradient and the hessian.
		hit_bShapeVertexIterator(vertex,shape) {
			if (!hit_gbTileVertexAt(fixed,vertex))
				calculate_GH(vertex);
		}
		

		// The initial position displacement is zero.
		hit_gbTileClearVertices(&e);

		// Loop for the jacobi method to solve the system.
		int j;
		for(j=0;j<iter2;j++){
			
			// Perform a iteration.
			hit_bShapeVertexIterator(vertex,shape) {
				if (!hit_gbTileVertexAt(fixed,vertex))
					solve_system_iter(vertex);
			}

			// Update the displacement.
			hit_gbTileCopyVertices(&e,&new_e);

		}

		// Update the position with the final displacement.
		hit_bShapeVertexIterator(vertex,shape) {
			if (!hit_gbTileVertexAt(fixed,vertex)){
				Vector * current = &(hit_gbTileVertexAt(graph,vertex));
				subV(*current,*current,hit_gbTileVertexAt(e,vertex));
			}
		}

	}

	hit_clockStop(comp_time);

	printf("# Final gradient norm: %lf\n",gradient_norm());
	printf("# Init time: %lf\n",hit_clockGetSeconds(init_time));
	printf("# Comp time: %lf\n",hit_clockGetSeconds(comp_time));
	printf("# Total time: %lf\n",hit_clockGetSeconds(init_time)+hit_clockGetSeconds(comp_time));

	free_structures();
	hit_comFinalize();

	return 0;
}



/**
 * Auxiliar function to get the gradient.
 */
Vector g(int i, int j){

	Vector gij;

	if(i == j){
		clearV(gij);
		return gij;
	}

	// Get the two vectors
	Vector r_i = (hit_gbTileVertexAt(graph,i));
	Vector r_j = (hit_gbTileVertexAt(graph,j));

	// Auxiliar variables.	
	Vector v;

	// v = r_i - r_j	
	subV(v,r_i,r_j);
	// n = ||v||
	double n = norm(v);
	
	if( n == 0){
		clearV(gij);
		return gij;
	}

	// gij = - K * ((L - n) / n) * (v)
	double scalar = - K * ((L - n) / n);
	multSV(gij,scalar,v);
	
	return gij;
}


/**
 * Auxiliary function to get the hessian.
 */
Matrix W(int i, int j){

	Matrix Wij;

	if(i == j){
		clearM(Wij);
		return Wij;
	}
	
	// Get the two vectors
	Vector r_i = (hit_gbTileVertexAt(graph,i));
	Vector r_j = (hit_gbTileVertexAt(graph,j));

	// Auxiliary variables.
	Vector v;
	Matrix a, b;
	double scalar_a, scalar_b;

	// v = r_i - r_j;	
	subV(v,r_i,r_j);
	// n = ||v||
	double n = norm(v);

	if( n == 0){
		clearM(Wij);
		return Wij;
	}

	// h = -((L-n)/n) * eye(D)  + (L/(n^3)) * (v * v')
	scalar_a = -((L-n)/(n));
	multSI(a,scalar_a);
	scalar_b = (L/(pow(n,3)));
	multVVt(b,v,v);
	multSM(b,scalar_b,b);
	addM(Wij,a,b);
	// h = K * h
	multSM(Wij,K,Wij);
	
	return Wij;
}


/**
 * Function to calculate the gradient and hessian for a given node.
 * Gradient: gs_i = Sum_j gij 
 * @param vertex The vertex index.
 */
void calculate_GH(int vertex){

	Vector gs;
	Matrix Ws;
	
	clearV(gs);
	clearM(Ws);
	
	int edge;
	hit_bShapeEdgeIterator(edge,shape,vertex){

		// Get the neighbor.
		int nghb_index = hit_bShapeEdgeTarget(shape,edge);

		// Calculate gradient[i]
		Vector gij = g(vertex,nghb_index);
		addV(gs,gs,gij);

		// Calculate part of the hessian[i][i]
		Matrix Wij = W(vertex,nghb_index);
		addM(Ws,Ws,Wij);

		// Calculate hessian[i][j]  if j is variable.
		//Node nj = hit_gbTileVertexAt(graph,nghb_index);
		if(!hit_gbTileVertexAt(fixed,nghb_index)){
		
			Matrix Hij = W(nghb_index,vertex);
			multSM(Hij,-1,Hij);
		
			hit_gbTileEdgeIteratorAt(hessian,vertex,edge) = Hij;
		}
	}
	
	hit_gbTileVertexAt(gradient,vertex) = gs;
	hit_gbTileEdgeAt(hessian,vertex,vertex) = Ws;

}

/**
 * Function that solves a iteration of the jacobi method for a given node.
 * e_i = H_ii^(-1) * (Gi - SUM_{i not j} ( Hij e_j ) )  
 * @param vertex The vertex index.
 */
void solve_system_iter(int vertex){

	Matrix Hii = hit_gbTileEdgeAt(hessian,vertex,vertex);
	Vector Gi  = hit_gbTileVertexAt(gradient,vertex);
	
	Vector new_ei;
	
	// Aux sum vector.
	Vector sum;
	clearV(sum);

	int edge;
	hit_bShapeEdgeIterator(edge,shape,vertex){

		// Get the neighbor.
		int nghb_index = hit_bShapeEdgeTarget(shape,edge);
		if(nghb_index == vertex) continue;

		// If the node is fixed, it is no part of the equation system.
		//Node nj = hit_gbTileVertexAt(graph,nghb_index);
		if(hit_gbTileVertexAt(fixed,nghb_index)) continue;

		// Get the j displacement and ij hessian part.
		Vector ej = hit_gbTileVertexAt(e,nghb_index);
		Matrix Hij = hit_gbTileEdgeIteratorAt(hessian,vertex,edge);
		
		// Aux par vector.
		Vector part;
		
		// Calculate and add the contribution of vertex j.
		// SUM_{i not j} ( Hij e_j ) 
		multMV(part,Hij,ej);
		addV(sum,sum,part);

	}
	
	// Gi - SUM_{i not j} ( Hij e_j ) 
	subV(sum,Gi,sum);

	// e_i = H_ii^(-1) * (Gi - SUM_{i not j} ( Hij e_j ) )  
	new_ei = solve(Hii,sum);
	hit_gbTileVertexAt(new_e,vertex) = new_ei;
}


