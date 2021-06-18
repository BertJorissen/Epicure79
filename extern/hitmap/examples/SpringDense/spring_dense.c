/**
 * Calculate the stable position of a spring system.
 * This version uses a dense incident matrix to hold the sparse graph.
 * Parallel hitmap version.
 *
 * @file spring_dense.c
 * @author Javier Fresno
 * @date 10-1-2012
 * @version 1.0 (12-8-2011)
 * @version 1.1 (10-1-2012) Updated version.
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
#include <time.h>
#include <string.h>
#include <sys/time.h>




/* OUTPUT FUNCIONS */
#define printfRootInternal(...) { if( hit_Rank == 0 ) { printf(__VA_ARGS__); fflush(stdout); }}
#define printfRoot(...) printfRootInternal(__VA_ARGS__)


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

/**
 * Node struct
 */
typedef struct {
	Vector r;	/**< The position vector. */
	int fixed;	/**< Fixed position flag. */
} Node;



// Define the tile_Node.
hit_tileNewType(Node);

// Define the tile_VectorA.
hit_tileNewType(Vector);

// Define the tile_double
hit_tileNewType(double);

// Define the tile_Matrix
hit_tileNewType(Matrix);

hit_tileNewType(int);


int Nvertices;
HitTile_int incident;



/** Hit shape defining the spring structure. */
HitShape global_shape;
HitShape incident_shape;


/** Gradient array part for each node. */
HitTile_Vector gradient;

/** Hessian matrix part for each edge. */
HitTile_Matrix hessian;

/** Vector with the position increase. */
HitTile_Vector e;

/** Vector with the new position increase. */
HitTile_Vector new_e;

/** Hit shape defining the local spring structure. */
HitShape shape;

/** Tile with the  node values. */
HitTile_Node graph;



/** Layout */
HitLayout lay = HIT_LAYOUT_NULL_STATIC;



/**
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

	
	if(hit_Rank != 0){
		hit_tileDomainShapeAlloc(&graph,Node,global_shape);
	}
	
	hit_tileDomainShapeAlloc(&e,Vector,global_shape);
	hit_tileDomainShapeAlloc(&new_e,Vector,global_shape);

	hit_tileDomainShapeAlloc(&gradient,Vector,global_shape);
	hit_tileDomainShapeAlloc(&hessian,Matrix,incident_shape);
}



/**
 * Release the memory
 */
void free_structures(){

	hit_tileFree(graph);
	hit_tileFree(e);
	hit_tileFree(new_e);
	hit_tileFree(gradient);
	hit_tileFree(hessian);

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
 * Replace string
 * @param str
 * @param orig
 * @param rep
 * @return
 */
char *replace_str(char *str, const char *orig, const char *rep){

	static char buffer[4096];
	char *p;

	if(!(p = strstr(str, orig))) return NULL;

	strncpy(buffer, str, (size_t) (p-str)); // Copy characters from 'str' start to 'orig' st$
	buffer[p-str] = '\0';

	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

	return buffer;
}




/**
 * Generate random coordinates for the spring nodes.
 * It also set the fixed nodes
 */
void random_coordinates(HitShape shape_rc, HitTile_Node graph_rc){

	srandom(0);
	if(hit_Rank != 0) return;

	int n = hit_sigCard(hit_shapeSig(shape_rc,0));

	int j,i;

	// Read the coordinates
	for(j=0;j<D;j++){
		for(i=0;i<n;i++){

			double number = (double) random();
			hit_tileElemAt(graph_rc,1,i).r.v[j] = number;
			
		}
	}
	

 	srandom(0);

	for(i=0;i<n;i++){

		if ((random() % 100) < P_FIXED){
			hit_tileElemAt(graph_rc,1,i).fixed = 1;
		} else {
			hit_tileElemAt(graph_rc,1,i).fixed = 0;
		}
	}

}




/*
void read_coordinates(char * name, HitShape shape_rc, HitTile_Node graph_rc){

	if(hit_Rank != 0) return;


	FILE * file = fopen(name,"r");


	if(file == NULL){
		printf("File not exists\n");
		return;
	}



	char c = (char) getc(file);

	// Skip first comments
	if(c == '%'){
	while( c != '\n' ) c = (char) getc(file);
	}
	ungetc(c,file);

	int res,n,d,i,j;

	int n_shape = hit_sigCard(hit_shapeSig(shape_rc,0));
	res = fscanf(file,"%d %d\n",&n,&d);

	if(n != n_shape) printf("Coordinates: Number of vertices do not match %d\n",n); n = min(n,n_shape);
	if(d != D) printf("Coordinates: Number of dimensions do not match %d\n",d); d = min(d,D);

	srandom(0);


	// Read the coordinates
	for(j=0;j<D;j++){
		for(i=0;i<n;i++){
			if(j<d){
				double number;
				res = fscanf(file,"%lf\n",&number);
				hit_tileElemAt(graph_rc,1,i).r.v[j] = number;
			} else {
				hit_tileElemAt(graph_rc,1,i).r.v[j] = 0.0;
			}
		}
	}


	for(i=0;i<n;i++){

		if ((random() % 100) < P_FIXED){
			hit_tileElemAt(graph_rc,1,i).fixed = 1;
		} else {
			hit_tileElemAt(graph_rc,1,i).fixed = 0;
		}

	}

	fclose(file);

}
*/








/**
 * Init the graph
 */
void init_graph(int argc, char ** argv){


	if(argc > 1){

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
				if(hit_Rank == 0) print_help(argv[0]);
				hit_comFinalize();
				exit(EXIT_SUCCESS);
			default:
				abort();
				break;
		}

		if (hit_Rank != 0) return;

		// Graph file name.
		char * graph_file = argv[optind];

		printfRoot("# Graph: %s\n",graph_file);
		printfRoot("# Iterations %dx%d\n",iter1,iter2);

		Nvertices = hit_fileHBVertices(graph_file);
		
		printfRoot("# Vertices %d\n",Nvertices);
		

		HitSig sig = hit_sig(0,Nvertices-1,1);
		incident_shape = hit_shape(2,sig,sig);
		
		hit_tileDomainShapeAlloc(&incident,int,incident_shape);
		

		hit_fileHBReadDense(graph_file,&incident);
		
#ifdef DEBUG
		{int i,j;
		for(i=0;i<Nvertices;i++){
			for(j=0;j<Nvertices;j++){
				if(hit_tileElemAt2(incident,i,j))
					printf("X");
				else
					printf(" ");
			}
			printf("\n");
		}}
#endif		
		

		global_shape = hit_shape(1,hit_sig(0,Nvertices-1,1));
		hit_tileDomainShapeAlloc(&graph,Node,global_shape);
		
		// Coordinates file
		char * coord_file = replace_str(graph_file,".rb","_coord.mtx");
		printfRoot("# Coordinates: %s\n",coord_file);

		// Random coordinates
		random_coordinates(global_shape,graph);

		//read_coordinates(coord_file,global_shape,graph);
		

		int i;
		int nfixed = 0;
		for(i=0;i<Nvertices;i++){

			nfixed += hit_tileElemAt(graph,1,i).fixed;
		}

		printf("# N fixed: %d\n",nfixed);

	} else {

		iter1 = iter2 = 3;
		Nvertices = 8;
		HitSig sig = hit_sig(0,Nvertices-1,1);
		incident_shape = hit_shape(2,sig,sig);

		if(hit_Rank != 0) return;

		hit_tileDomainShapeAlloc(&incident,int,incident_shape);
		
		int i,j;
		
		for(i=0;i<Nvertices;i++)
		for(j=0;j<Nvertices;j++)
		hit_tileElemAt2(incident,i,j) = 0; 

		hit_tileElemAt2(incident,0,1) = 1;
		hit_tileElemAt2(incident,1,2) = 1;
		hit_tileElemAt2(incident,2,3) = 1;
		hit_tileElemAt2(incident,4,5) = 1;
		hit_tileElemAt2(incident,5,6) = 1;
		hit_tileElemAt2(incident,6,7) = 1;
		hit_tileElemAt2(incident,0,4) = 1;
		hit_tileElemAt2(incident,1,5) = 1;
		hit_tileElemAt2(incident,2,6) = 1;
		hit_tileElemAt2(incident,3,7) = 1;
		hit_tileElemAt2(incident,1,4) = 1;
		hit_tileElemAt2(incident,2,5) = 1;
		
		hit_tileElemAt2(incident,1,0) = 1;
		hit_tileElemAt2(incident,2,1) = 1;
		hit_tileElemAt2(incident,3,2) = 1;
		hit_tileElemAt2(incident,5,4) = 1;
		hit_tileElemAt2(incident,6,5) = 1;
		hit_tileElemAt2(incident,7,6) = 1;
		hit_tileElemAt2(incident,4,0) = 1;
		hit_tileElemAt2(incident,5,1) = 1;
		hit_tileElemAt2(incident,6,2) = 1;
		hit_tileElemAt2(incident,7,3) = 1;
		hit_tileElemAt2(incident,4,1) = 1;
		hit_tileElemAt2(incident,5,2) = 1;
		
		// Include each (i,i) edge.
		hit_tileElemAt2(incident,0,0) = 1;
		hit_tileElemAt2(incident,1,1) = 1;
		hit_tileElemAt2(incident,2,2) = 1;
		hit_tileElemAt2(incident,3,3) = 1;
		hit_tileElemAt2(incident,4,4) = 1;
		hit_tileElemAt2(incident,5,5) = 1;
		hit_tileElemAt2(incident,6,6) = 1;
		hit_tileElemAt2(incident,7,7) = 1;

		// Allocate memory
		global_shape = hit_shape(1,hit_sig(0,Nvertices-1,1));
		hit_tileDomainShapeAlloc(&graph,Node,global_shape);

		// Set initial positions
		Node n0 = {{{0,0,0}},1};
		hit_tileElemAt(graph,1,0) = n0;
		Node n1 = {{{100,0,0}},0};
		hit_tileElemAt(graph,1,1) = n1;
		Node n2 = {{{200,0,0}},0};
		hit_tileElemAt(graph,1,2) = n2;
		Node n3 = {{{300,0,0}},1};
		hit_tileElemAt(graph,1,3) = n3;
		Node n4 = {{{0,100,200}},1};
		hit_tileElemAt(graph,1,4) = n4;
		Node n5 = {{{100,100,0}},0};
		hit_tileElemAt(graph,1,5) = n5;
		Node n6 = {{{200,100,0}},0};
		hit_tileElemAt(graph,1,6) = n6;
		Node n7 = {{{300,100,0}},1};
		hit_tileElemAt(graph,1,7) = n7;
		
	}


}



/**
 * Calculate the norm of the whole gradient vector.
 * @return the norm
 */
double gradient_norm(){

	HitTile_double norm, sum_norm;

  	hit_tileDomainAlloc(&norm, double, 1,1);
  	hit_tileDomainAlloc(&sum_norm, double, 1,1);

  	hit_tileElemAt(norm,1,0) = 0.0;

	int vertex;
	
	hit_shapeIterator(vertex,shape,0){
	
			Node current = hit_tileElemAt(graph,1,vertex);

			if (!current.fixed){

				calculate_GH(vertex);

				Vector Gi = hit_tileElemAt(gradient,1,vertex);

				int d;
				for(d=0;d<D;d++){
					hit_tileElemAt(norm,1,0) += pow(Gi.v[d],2);
				}
				
				//printf("vertex: %d: %.3f - %.3f - %.3f \n",vertex,pow(Gi.v[0],2),pow(Gi.v[1],2),pow(Gi.v[2],2));
				
			}
		}

	
	HitOp op_sum;
	hit_comOp(hit_comOpSumDouble,op_sum);


    HitRanks root = {{0,0,0,-1}};
  	HitCom com_sum = hit_comReduce(lay, root, &norm, &sum_norm, HIT_DOUBLE, op_sum);
  	hit_comDo(&com_sum);
	hit_comFree(com_sum);

	double res = 0;

  	if( hit_Rank == 0  ){
  		res = sqrt(hit_tileElemAt(sum_norm,1,0));
  	}

	hit_tileFree(norm);
	hit_tileFree(sum_norm);


	return res;

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
	
	//if(hit_NProcs == 1){
	//	printf("\nERROR: Lauch with at least 2 processes\n\n");
	//	exit(EXIT_FAILURE);
	//}
	
	init_graph(argc,argv);

	// Create the topology object.
	HitTopology topo = hit_topology(plug_topPlain);

	// Create the layout to send the incident matrix
	HitLayout lay_all = hit_layout(plug_layBlocks,topo,hit_shape(1,hit_sig(0,hit_NProcs-1,1)));

	// Root processor.
	HitRanks root = {{0,-1,-1,-1}};
	
	// Send the number of vertices to allocate the tile.
	HitTile_int vertices;
	hit_tileDomainAlloc(&vertices, int, 1,1);
  	hit_tileElemAt(vertices,1,0) = Nvertices;
	HitCom sendNVertices = hit_comBroadcast(lay_all,root,&vertices,HIT_INT);
	hit_comDo(&sendNVertices);
	Nvertices = hit_tileElemAt(vertices,1,0);
	
	// Allocate the tile for the incident matrix
	if(hit_Rank != 0){
		HitSig sig = hit_sig(0,Nvertices-1,1);
		incident_shape = hit_shape(2,sig,sig);
		hit_tileDomainShapeAlloc(&incident,int,incident_shape);
	}
	
	// Send the incident matrix
	HitCom sendIncident = hit_comBroadcast(lay_all,root,&incident,HIT_INT);
	hit_comDo(&sendIncident);
	
	
	// Create the global shape
	global_shape = hit_shape(1,hit_sig(0,Nvertices-1,1));
	
	// Distribute the graph vertices among the processors.
	lay = hit_layout(plug_layBlocks,topo,global_shape);

	// Get shape
	shape = hit_layShape(lay);

	// Allocate memory of all the variables
	init_structures();
		
	// Create the HitVector type.
	HitType HitVector;
	hit_comTypeStruct(&HitVector,Vector,1,  v,3,HIT_DOUBLE);

	// Create the HitNode type
	HitType HitNode;
	hit_comTypeStruct(&HitNode,Node,2,  r,1,HitVector,fixed,1,HIT_INT);

	
	// Create the communication objects.
	HitCom comA = hit_comBroadcast(lay_all,root,&graph,HitNode);
	HitPattern patB = hit_pattern( HIT_PAT_UNORDERED );
	HitPattern patC = hit_pattern( HIT_PAT_UNORDERED );
	
	// @javfres 2015-10-09 Support for more processes than nodes
	//for(i=0;i<hit_NProcs;i++){
	if(hit_layImActive( lay ))
	for(i=0;i<hit_layNumActives(lay);i++){

		HitRanks ranks = {{i,-1,-1,-1}};
		HitShape otherShape = hit_layShapeOther(lay, ranks);
		hit_patternAdd(&patB,
			hit_comBroadcastSelect(lay,ranks,&graph,otherShape,HIT_COM_ARRAYCOORDS,HitNode));
		hit_patternAdd(&patC,
			hit_comBroadcastSelect(lay,ranks,&e,otherShape,HIT_COM_ARRAYCOORDS,HitVector));
	
	}
	

	hit_clockStop(init_time);
	hit_clockStart(comp_time);
	
	// Send all the data from the root proc to the other procs.
	hit_comDo(&comA);

	// Calculate the inital gradient norm.
	//double ignorm = gradient_norm();
	//printfRoot("# Initial gradient norm: %lf\n",ignorm);
	

	// Main loop for the newton method
	for(i=0;i<iter1;i++){
		printfRoot("# iter: %d/%d\n",i,iter1);

		// Iterate trough all the vertices and
		// obtain the gradient and the hessian.
		
		hit_shapeIterator(vertex,shape,0){
		
			Node current = hit_tileElemAt(graph,1,vertex);
			if (!current.fixed)
				calculate_GH(vertex);
		}


		// The initial position displacement is zero.
		bzero(e.data,sizeof(Vector) * (size_t) Nvertices); 
		
		// Loop for the jacobi method to solve the system.
		int j;
		for(j=0;j<iter2;j++){
			


			// Perform a iteration.
			hit_shapeIterator(vertex,shape,0){
				Node current = hit_tileElemAt(graph,1,vertex);
				if (!current.fixed)
					solve_system_iter(vertex);
			}



			// Update the displacement.
			memcpy(e.data,new_e.data,sizeof(Vector)* (size_t) Nvertices);
			hit_patternDo(patC);

		}

		// Update the position with the final displacement.
		hit_shapeIterator(vertex,shape,0){

			Node * current = &(hit_tileElemAt(graph,1,vertex));
			if (!current->fixed){
				int d;
				for(d=0;d<D;d++)
					current->r.v[d] -= hit_tileElemAt(e,1,vertex).v[d];
			}
		}

		hit_patternDo(patB);
	}


	double gnorm = gradient_norm();
	printfRoot("# Final gradient norm: %lf\n",gnorm);

	hit_clockStop(comp_time);
	hit_clockWorldReduce(init_time);
	hit_clockWorldReduce(comp_time);

	printfRoot("# Init time: %lf\n",hit_clockGetSeconds(init_time));
	printfRoot("# Comp time: %lf\n",hit_clockGetSeconds(comp_time));
	printfRoot("# Total time: %lf\n",hit_clockGetSeconds(init_time)+hit_clockGetSeconds(comp_time));

	hit_comFree(comA);
	hit_patternFree(&patB);
	hit_patternFree(&patC);
	free_structures();

	hit_layFree( lay );
	hit_layFree( lay_all );
	hit_topFree( topo );	
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
	Vector r_i = (hit_tileElemAt(graph,1,i)).r;
	Vector r_j = (hit_tileElemAt(graph,1,j)).r;

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
	Vector r_i = (hit_tileElemAt(graph,1,i)).r;
	Vector r_j = (hit_tileElemAt(graph,1,j)).r;

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
 * @param vertex The vertex index.
 */
void calculate_GH(int vertex){

	Vector gs;
	Matrix Ws;
	
	clearV(gs);
	clearM(Ws);
	
	int edge;
	for(edge=0;edge<Nvertices;edge++){

		
		if(! hit_tileElemAt(incident,2,vertex,edge)){
			continue;
		}		
		
		
		// Get the neighbor.
		int nghb_index = edge;

		// Calculate gradient[i]
		Vector gij = g(vertex,nghb_index);
		addV(gs,gs,gij);

		// Calculate part of the hessian[i][i]
		Matrix Wij = W(vertex,nghb_index);
		addM(Ws,Ws,Wij);

		
		// Calculate hessian[i][j]  if j is variable.
		Node nj = hit_tileElemAt(graph,1,nghb_index);

		if(!nj.fixed){
			Matrix Hij = W(nghb_index,vertex);
			multSM(Hij,-1,Hij);
		
			hit_tileElemAt(hessian,2,vertex,nghb_index) = Hij;
			
			
		}
		 
	}

	hit_tileElemAt(gradient,1,vertex) = gs;
	hit_tileElemAt(hessian,2,vertex,vertex) = Ws;
}



/**
 * Function that solves a iteration of the jacobi method for a given node.
 * e_i = H_ii^(-1) * (Gi - SUM_{i not j} ( Hij e_j ) )  
 * @param vertex The vertex index.
 */
void solve_system_iter(int vertex){

	Matrix Hii = hit_tileElemAt(hessian,2,vertex,vertex);
	Vector Gi  = hit_tileElemAt(gradient,1,vertex);
	
	Vector new_ei;
	
	// Aux sum vector.
	Vector sum;
	clearV(sum);

	int edge;
	
	for(edge=0;edge<Nvertices;edge++){

		
		if(! hit_tileElemAt(incident,2,vertex,edge)){
			continue;
		}
		
		// Get the neighbor.
		int nghb_index = edge;
		if(nghb_index == vertex) continue;

		// If the node is fixed, it is no part of the ecuation system.
		Node nj = hit_tileElemAt(graph,1,nghb_index);
		if(nj.fixed) continue;

		// Get the j displacement and ij hessian part.
		Vector ej = hit_tileElemAt(e,1,nghb_index);	
		Matrix Hij = hit_tileElemAt(hessian,2,vertex,nghb_index);
		
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

	hit_tileElemAt(new_e,1,vertex) = new_ei;
	
	
}
