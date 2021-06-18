/**
 * @file SWpar_ref.c
 *
 * Parallel version for the Smith-Waterman algorihtm.
 * The Smith–Waterman algorithm performs local sequence alignment
 * of protein sequences.
 *
 * This version assumes a linear gap penalty.
 * This algorithm creates the matrix H as follows:
 *
 *                   | H(i-1,j-1) + PAM(a_i,b_j)    (diagonal)
 * H(i,j) =  MAX of  | H(i-1,j)   + gappenalty      (up)
 *                   | H(i,j-1)   + gappenalty      (left)
 *                   | 0
 *
 * The PAM matrix is the amino acid substitution matrix that encode
 * the expected evolutionary change at the amino acid level.
 *
 * To obtain the optimum local alignment, we start with the highest value
 * in the matrix (i,j). Then, we go backwards to one of positions
 * (i − 1,j), (i, j − 1), and (i − 1, j − 1) depending on the direction
 * of movement used to construct the matrix.
 *
 * @author Javier Fresno
 * @date Jun 2013
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

// Include the common variables and utilities for all the versions
#include "SWcommon.h"

// Include the common variables and utilities for the C reference versions
#include "SWcommon_ref.h"

// MPI include
#include <mpi.h>


/**
 * Macro to get the protein (or H matrix) Begin index of the current processor.
 */
#define BEGIN(coord,dims,size) (coord * size / dims)

/**
 * Macro to get the protein (or H matrix) End index of the current processor.
 */
#define END(coord,dims,size)   ((( coord + 1 ) * size / dims)-1)

/**
 * Macro to get the local protein (or H matrix) size of the current processor.
 */
#define SIZE(coord,dims,size)  (END(coord,dims,size)-BEGIN(coord,dims,size)+1)

/**
 * Function that distributes the protein from the root processor to the remain procs.
 */
void distributeProtein(aa_t * protein, aa_t *lprotein, int dim);

/**
 * Function to receive the result from the sucessors at the
 * backtracking stage. They will pass the coordinate of the
 * next point of the path or -1 if the alignement do not cross
 * through our portion of H.
 */
void recv_Back(int dir, int * pos);

/**
 * Function to inform the predecessor processors if they
 * have to rebuild the path using the backtracking algorithm.
 */
void send_Back(int dir, int pos);


/**
 * Function to compose the final output in the root processor.
 */
void composeOutput(aa_t * out, int * outpos, int * outsize, aa_t * lout, int loutsize, int * gBegin, int * gEnd);





// **********************************************************************
// Declarations of the different phases
// **********************************************************************

/**
 * Read the protein sequences
 */
void phase_read_sequences();

/**
 * Distribute the protein sequences
 */
void phase_distribute_sequences();

/**
 * Receive the similarity matrix (H) values from the neighbors.
 */
void phase_recv_hmatrix();

/**
 * Calculate the similarity matrix (H)
 */
void phase_comp_hmatrix();

/**
 * Send the similarity matrix (H) values to the neighbors.
 */
void phase_send_hmatrix();

/**
 * Receive the traceback neighbor values
 */
void phase_recv_back();

/**
 * Compute the traceback
 */
void phase_comp_back();

/**
 * Receive the traceback values to the appropriate neighbor
 */
void phase_send_back();

/**
 * Compose the final sequence
 */
void phase_compose_sequence();


/**
 * Debug function to show the H and backtraking matrices
 */
void debug_show_matrices();

/**
 * Debug to show the backtracking info
 */
void debug_show_backtracking();




// **********************************************************************
// Parallel related global variables
// **********************************************************************

/**  MPI Rank of the current processor. */
int rank;
/** Number of processors in the communicator. */
int nProcs;
/** Size of the local part of the proteins. */
int lsize[2];
/** Partition dimensions. */
int dims[2] = {0,0};
/** x and y coordinates of the processor in the topology.  */
int lcoords[2];
/** Begin index for the current processor */
int lbegin[2];
/** Communicator */
MPI_Comm comm;

/** MPI reduce function for double_3int */
MPI_Op opHMaxLoc;



// **********************************************************************
// Global root variables
// **********************************************************************

// Protein sequences
aa_t * p1;
aa_t * p2;

// Output proteins with the alignment
aa_t * out1;
aa_t * out2;

// X and Y coordinates of the maximum value
int xMax;
int yMax;

// Begin coordinates of the match
int xBegin;
int yBegin;

// Match lengths (total, sequence 1, sequence 2)
int match_length;
int match_length1;
int match_length2;

// Output match begin
int match_pos;


// **********************************************************************
// Global (C) local (parallel) variables
// **********************************************************************

// Protein sequences
aa_t * lp1;
aa_t * lp2;

// Local output proteins with the alignment
aa_t * lout1;
aa_t * lout2;

// Similarity matrix
h_t * H;
#define h(i,j) (H[((i)*(lsize[1]+1))+(j)])

// Traceback matrices
trace_t * xTraceback;
trace_t * yTraceback;
#define xback(i,j) (xTraceback[((i)*(lsize[1]+1))+(j)])
#define yback(i,j) (yTraceback[((i)*(lsize[1]+1))+(j)])

// Local Max value
h_t lMax;

// X and Y coordinates of the maximum value
int lxMax;
int lyMax;

// Begin coordinates of the match
int lxBegin;
int lyBegin;

// Match lengths (total, sequence 1, sequence 2)
int lmatch_length;
int lmatch_length1;
int lmatch_length2;

// Output match begin
int lmatch_pos;



/**
 * Main function.
 */
int main(int argc, char * argv[]){

	// **********************************************************************
	// Init MPI
	// **********************************************************************
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nProcs);


	// MPI Datatype for the double_3int struct.
	double_3int struct_d3i;

	int blocks[4] = {1,1,1,1};
	MPI_Aint displ[5];
	MPI_Datatype types[4] = {MPI_DOUBLE, MPI_INT, MPI_INT, MPI_INT};

	MPI_Address( &struct_d3i.val,  &displ[0] );
	MPI_Address( &struct_d3i.rank, &displ[1] );
	MPI_Address( &struct_d3i.i,    &displ[2] );
	MPI_Address( &struct_d3i.j,    &displ[3] );
	MPI_Address( &struct_d3i,      &displ[4] );

	displ[0] -= displ[4];
	displ[1] -= displ[4];
	displ[2] -= displ[4];
	displ[3] -= displ[4];

	MPI_Type_struct(4, blocks, displ, types, &MPI_DOUBLE_3INT);
	MPI_Type_commit( &MPI_DOUBLE_3INT );

	// Create the operation to reduce the double_3int values.
    MPI_Op_create( HMaxLoc, 1,  &opHMaxLoc );



	// **********************************************************************
	// Check input parameters
	// **********************************************************************
	input_parameters(argc, argv);

	// **********************************************************************
	// Layout
	// **********************************************************************

	int periods[2] = {0,0};
	MPI_Dims_create(nProcs, 2, dims);
	MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods,0,&comm);
	MPI_Cart_coords(comm,rank,2,lcoords);

	// Print processor info
	printfroot("=== Parallel ===\n"
		" Processors: %d\n"
		" Processors: %d x %d\n\n",nProcs,dims[0], dims[1]);

	// Begin, end
	int k;

	for(k=0; k<2; k++){
		lbegin[k] = BEGIN(lcoords[k],dims[k],size[k]);
		lsize[k] =  SIZE(lcoords[k],dims[k],size[k]);

		if(lsize[k] < 1){
			fprintf(stderr,"There are inactive processors\n");
			fflush(stderr);
			MPI_Abort(comm,EXIT_FAILURE);

		}
	}

	// Print info of each processor
#ifdef DEBUG
	ALL_SEQUENTIAL_BEGIN(seq1);
	printf("Processor: %d/%d, ",rank,nProcs);
	printf(" Coords %d x %d\n",lcoords[0],lcoords[1]);
	printf(" Dim1: Block begin %2d- (size %2d)\n",lbegin[0], lsize[0]);
	printf(" Dim2: Block begin %2d- (size %2d)\n",lbegin[1], lsize[1]);
	printf("\n");
	ALL_SEQUENTIAL_END(seq1);
#endif

	// **********************************************************************
	// Timers
	// **********************************************************************
	double time_initS; /**< Time init start  */
	double time_initE; /**< Time init end  */
	double time_compS; /**< Time computation start  */
	double time_compE; /**< Time computation end  */

	double time_phaseS; /**< Aux start timer for phases */
	double time_phaseE; /**< Aux end timer for phases */

	double time_comp_read = 0;		/**< Accumulate time in read */
	double time_comp_hmatrix = 0;	/**< Accumulate time in H matrix computation */
	double time_comp_back = 0;		/**< Accumulate time in backtracking */

	double time_comp_distr = 0;		/**< Accumulate time in protein sequence distribution */
	double time_comp_syncH = 0;		/**< Accumulate time in H matrix sync */
	double time_comp_syncB = 0;		/**< Accumulate time in backtracking sync */
	double time_comp_compose = 0;	/**< Accumulate time in final protein composition */


	// **********************************************************************
	// INIT PHASE: Structure allocation and PAM matrix load
	// **********************************************************************

	// Init time
	MPI_Barrier(comm);
	time_initS = currentTime();

	// Every one do this, the pam matrix is small and all procs
	// need it, we don't bother to read it and distribute it.
	initPAM(pam_name);

	// Open the proteins files
	openProtein(&pfile1,prot_name1);
	openProtein(&pfile2,prot_name2);

	// Allocate memory for the proteins (Only root has them)
	if(is_root()){
		p1 = malloc(sizeof(aa_t)*((size_t)(size[0]+1)));
		p2 = malloc(sizeof(aa_t)*((size_t)(size[1]+1)));
		CHECK_NULL(p1);
		CHECK_NULL(p2);
	}

	// Local proteins
	lp1 = malloc((size_t) ( lsize[0]+1 ) * sizeof(aa_t));
	lp2 = malloc((size_t) ( lsize[1]+1 ) * sizeof(aa_t));
	CHECK_NULL(lp1);
	CHECK_NULL(lp2);

	// Local Similarity matrix
	H = malloc(sizeof(h_t)*(size_t)((lsize[0]+1)*(lsize[1]+1)));
	CHECK_NULL(H);

	// Traceback matrices
	xTraceback = malloc(sizeof(trace_t)*(size_t)((lsize[0]+1)*(lsize[1]+1)));
	yTraceback = malloc(sizeof(trace_t)*(size_t)((lsize[0]+1)*(lsize[1]+1)));
	CHECK_NULL(xTraceback);
	CHECK_NULL(yTraceback);

	// Allocate size of the output (local)
	lout1 = malloc(sizeof(aa_t)*((size_t)(lsize[0]+lsize[1]+1)));
	lout2 = malloc(sizeof(aa_t)*((size_t)(lsize[0]+lsize[1]+1)));
	CHECK_NULL(lout1);
	CHECK_NULL(lout2);

	// Allocate size of the output (global, only root)
	if(is_root()){
		out1 = malloc(sizeof(aa_t)*((size_t)(size[0]+size[1]+1)));
		out2 = malloc(sizeof(aa_t)*((size_t)(size[0]+size[1]+1)));
		CHECK_NULL(out1);
		CHECK_NULL(out2);
	}

	// Init time
	time_initE = currentTime();


	// **********************************************************************
	// WHOLE COMPUTATION PHASE: iteration of: Read, H matrix, Backtracking
	// **********************************************************************

	// Computation time
	time_compS = currentTime();

	int iter;
	for(iter=0;iter<iterations;iter++){

#ifdef DEBUG
		printfroot("=== Iteration: %d/%d ===\n\n",iter,iterations);
#endif

		// **********************************************************************
		// READ PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_read_sequences();
		time_phaseE = currentTime();
		time_comp_read += (time_phaseE-time_phaseS);


		// **********************************************************************
		// DISTRIBUTION PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_distribute_sequences();
		time_phaseE = currentTime();
		time_comp_distr += (time_phaseE-time_phaseS);


		// **********************************************************************
		// RECV H PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		// Recv the needed H values from the neighbors
		phase_recv_hmatrix();
		time_phaseE = currentTime();
		time_comp_syncH += (time_phaseE-time_phaseS);


		// **********************************************************************
		// H MATRIX PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_comp_hmatrix();
		time_phaseE = currentTime();
		time_comp_hmatrix += (time_phaseE-time_phaseS);


		// **********************************************************************
		// SEND H PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		// Send the needed H values to the neighbors
		phase_send_hmatrix();
		time_phaseE = currentTime();
		time_comp_syncH += (time_phaseE-time_phaseS);

#ifdef DEBUG
		debug_show_matrices();
#endif


		// **********************************************************************
		// RECV BACKTRAING PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_recv_back();
		time_phaseE = currentTime();
		time_comp_syncB += (time_phaseE-time_phaseS);


		// **********************************************************************
		// BACKTRACKING PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_comp_back();
		time_phaseE = currentTime();
		time_comp_back += (time_phaseE-time_phaseS);


		// **********************************************************************
		// SEND BACKTRAING PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_send_back();
		time_phaseE = currentTime();
		time_comp_syncB += (time_phaseE-time_phaseS);


		// **********************************************************************
		// COMPOSE FINAL PROTEIN PHASE
		// **********************************************************************
		time_phaseS = currentTime();
		phase_compose_sequence();
		time_phaseE = currentTime();
		time_comp_compose += (time_phaseE-time_phaseS);

#ifdef DEBUG
		debug_show_backtracking();
#endif


	} // Iterations Loop

	// Computation time
	MPI_Barrier(comm);
	time_compE = currentTime();


	// **********************************************************************
	// Show results
	// **********************************************************************


	// Reduce the timers
	// [local,MIN,AVG,MAX][READ,HMAT,BACK,DIST,SYNH,SYNB,COMP]

#define T_LOCAL 0
#define T_MIN 1
#define T_AVG 2
#define T_MAX 3

#define T_READ 0
#define T_HMAT 1
#define T_BACK 2
#define T_DIST 3
#define T_SYNH 4
#define T_SYNB 5
#define T_COMP 6


	double timers[4][7];
	timers[T_LOCAL][0] = time_comp_read;
	timers[T_LOCAL][1] = time_comp_hmatrix;
	timers[T_LOCAL][2] = time_comp_back;
	timers[T_LOCAL][3] = time_comp_distr;
	timers[T_LOCAL][4] = time_comp_syncH;
	timers[T_LOCAL][5] = time_comp_syncB;
	timers[T_LOCAL][6] = time_comp_compose;

#define allTimers(timer) timers[T_MIN][timer],timers[T_AVG][timer],timers[T_MAX][timer]

	MPI_Reduce(timers[0], timers[1], 7, MPI_DOUBLE, MPI_MIN, 0, comm);
	MPI_Reduce(timers[0], timers[2], 7, MPI_DOUBLE, MPI_SUM, 0, comm);
	MPI_Reduce(timers[0], timers[3], 7, MPI_DOUBLE, MPI_MAX, 0, comm);

	int i;
	for(i=0;i<7;i++) timers[T_AVG][i] /= nProcs; // AVG

	// Final result.
	if(is_root()){

		match_length1 = xMax - xBegin;
		match_length2 = yMax - yBegin;

		printf("=== Result ===\n");
		printf("Init time: %f\n",time_initE-time_initS);
		printf("Comp time: %f\n",time_compE-time_compS);
		printf("  Read protein: min: %f, avg: %f, max: %f \n",allTimers(T_READ));
		printf("  Distribute:   min: %f, avg: %f, max: %f \n",allTimers(T_DIST));
		printf("  H matrix:     min: %f, avg: %f, max: %f \n",allTimers(T_HMAT));
		printf("  sync H mat:   min: %f, avg: %f, max: %f \n",allTimers(T_SYNH));
		printf("  Backtracking: min: %f, avg: %f, max: %f \n",allTimers(T_BACK));
		printf("  Sync Back:    min: %f, avg: %f, max: %f \n",allTimers(T_SYNB));
		printf("  Compose:      min: %f, avg: %f, max: %f \n",allTimers(T_COMP));

		printf("Last Match length of A: %d\n", match_length1);
		printf("Last Match length of B: %d\n", match_length2);
		printf("Last Alignment length: %d\n", match_length);


		// Print the result for small inputsets
		if(match_length < 50){
			printf("\n");
			printf("MCH1: "); printProteinMatch(p1,size[0],xBegin,xMax);
			printf("MCH2: "); printProteinMatch(p2,size[1],yBegin,yMax);

			printf("\n");
			printf("OUT1: "); printProtein(&out1[match_pos],match_length);
			printf("OUT2: "); printProtein(&out2[match_pos],match_length);
		} else {
			printf("\nInputset too big to print to stdout\n");
		}
	}


	// Free all the resources
	free(lp1);
	free(lp2);
	free(xTraceback);
	free(yTraceback);
	free(H);
	free(lout1);
	free(lout2);
	MPI_Op_free(&opHMaxLoc);
	MPI_Comm_free(&comm);
	MPI_Type_free(&MPI_DOUBLE_3INT);

	if(is_root()){
		free(p1);
		free(p2);
		free(out1);
		free(out2);
	}

	MPI_Finalize();
	return EXIT_SUCCESS;

}



void phase_read_sequences(){

	if(! is_root() ) return;

	// Read the proteins
	readProtein(&pfile1, p1, size[0]);
	readProtein(&pfile2, p2, size[1]);

#ifdef DEBUG
	printf("PRT1: "); printProtein(p1,size[0]);
	printf("PRT2: "); printProtein(p2,size[1]);
	printf("\n");
#endif


}


void phase_distribute_sequences(){

	// Send and Recv the proteins from the root proc.
	distributeProtein(p1,lp1,0);
	distributeProtein(p2,lp2,1);

	// Print info of each processor
#ifdef DEBUG
	ALL_SEQUENTIAL_BEGIN(seq2)
	printf("Processor: %d/%d, ",rank,nProcs);
	printf("Coords %d x %d\n",lcoords[0],lcoords[1]);
	printf("Local protein 1: ");
	printProtein(lp1,lsize[0]);
	printf("Local protein 2: ");
	printProtein(lp2,lsize[1]);
	printf("\n");
	ALL_SEQUENTIAL_END(seq2)
#endif

}



void phase_recv_hmatrix(){

	int pcoords[2];
	int prank;

	// Arrays to receive
	h_t array1[lsize[1]];
	h_t array2[lsize[0]+1];

	// X-1
	pcoords[0] = lcoords[0]-1;
	pcoords[1] = lcoords[1];

	if(pcoords[0] >= 0){

		// Get the rank of the neighbor
		MPI_Cart_rank(comm,pcoords,&prank);

		// Receive the array
		MPI_Recv(array1, lsize[1], MPI_H, prank, 0, comm, MPI_STATUS_IGNORE);

		int j;
		for(j=0; j<lsize[1]; j++){
			h(0,j+1) = array1[j];
		}

	}


	// Y-1
	pcoords[0] = lcoords[0];
	pcoords[1] = lcoords[1]-1;


	if(pcoords[1] >= 0){

		// Get the rank of the neighbor
		MPI_Cart_rank(comm,pcoords,&prank);

		// Receive the array
		MPI_Recv(array2, lsize[0]+1, MPI_H, prank, 0,comm, MPI_STATUS_IGNORE);

			int i;
			for(i=0; i<lsize[0]+1; i++){
				h(i,0) = array2[i];
			}
	}


}



void phase_send_hmatrix(){

	int pcoords[2];
	int prank;

	// Arrays to send
	h_t array1[lsize[1]];
	h_t array2[lsize[0]+1];

	// X-1
	pcoords[0] = lcoords[0]+1;
	pcoords[1] = lcoords[1];

	if(pcoords[0] < dims[0]){

		// Get the rank of the neighbor
		MPI_Cart_rank(comm,pcoords,&prank);

		int j;
		for(j=0; j<lsize[1]; j++){
			array1[j] = h(lsize[0],j+1);
		}

		MPI_Send(array1, lsize[1], MPI_H, prank, 0, comm);
	}


	// Y-1
	pcoords[0] = lcoords[0];
	pcoords[1] = lcoords[1]+1;

	if(pcoords[1] < dims[1]){

		// Get the rank of the neighbor
		MPI_Cart_rank(comm,pcoords,&prank);

		int i;
			for(i=0; i<lsize[0]+1; i++){
				array2[i] = h(i,lsize[1]);
			}
			MPI_Send(array2, lsize[0]+1, MPI_H, prank, 0,comm);
	}

}






void phase_comp_hmatrix(){

	// Compute the Similarity Matrix (H)

	//  Initialize values
	int i,j;

	if(lcoords[1] == 0){
		for (i=0;i<=lsize[0];i++){
			xback(i,0) = -1;
			yback(i,0) = -1;
			h(i,0)=0;
		}
	}

	if(lcoords[0] == 0){
		for (j=0;j<=lsize[1];j++){
			xback(0,j) = -1;
			yback(0,j) = -1;
			h(0,j)=0;
		}
	}

	// Maximum values to retrieve the alignment
	lMax = -1;
	lxMax = -1;
	lyMax = -1;

	// Perform wavefront algorithm
	for(i=1; i<lsize[0]+1; i++){
		for(j=1; j<lsize[1]+1; j++){

			// Get the scores
			h_t diag  = h(i-1,j-1) + pam(lp1[i],lp2[j]);
			h_t down  = h(i-1,j  ) + gapPenalty;
			h_t right = h(i  ,j-1) + gapPenalty;

			// Calculate the maximum
			int idx;
			h_t max = MAX4(diag,down,right,0,&idx);
			h(i,j) = max;

			// Set the back trace variables.
			if ( idx == 0 ){         // max == diag
				xback(i,j) = (trace_t) (i-1);
				yback(i,j) = (trace_t) (j-1);
			} else if ( idx == 1 ) { // max == down
				xback(i,j) = (trace_t) (i-1);
				yback(i,j) = (trace_t) j;
			} else if ( idx == 2 ) { // max == right
				xback(i,j) = (trace_t) i;
				yback(i,j) = (trace_t) (j-1);
			} else {                 // max == 0
				xback(i,j) = -1;
				yback(i,j) = -1;
			}

			// Maximum
			if(max >= lMax){
				lxMax = i;
				lyMax = j;
				lMax = max;
			}

		} // End loop j;
	} // End loop i;

}



void debug_show_matrices(){

	// Print info of each processor
	#ifdef DEBUG

		int i,j;

		ALL_SEQUENTIAL_BEGIN(seq3);

		printf("Processor: %d/%d, ",rank,nProcs);
		printf("Coords %d x %d\n",lcoords[0],lcoords[1]);

		// Print
		printf("   ");
		for (j=0;j<=lsize[1];j++){
			if(j==0)
				printf("     -");
			else
				printf("%6c",AA2char(lp2[j]));
		}
		printf("\n");
		for (i=0;i<=lsize[0];i++){

			if(i==0)
				printf("- |");
			else
				printf("%c |",AA2char(lp1[i]));

			for (j=0;j<=lsize[1];j++){
				printf("%6.1f",(double)h(i,j));
			}
			printf("\n");
		}
		printf("\n");

		ALL_SEQUENTIAL_END(seq3);
	#endif



	#ifdef DEBUG

	ALL_SEQUENTIAL_BEGIN(seq4)

		printf("Processor: %d/%d, ",rank,nProcs);
		printf("coords %d x %d\n",lcoords[0],lcoords[1]);

		// Print
		printf("   ");
		for (j=0;j<=lsize[1];j++){
			if(j==0)
				printf("      -");
			else
				printf("%8c",AA2char(lp2[j]));
		}
		printf("\n");
		for (i=0;i<=lsize[0];i++){

			if(i==0)
				printf("- |");
			else
				printf("%c |",AA2char(lp1[i]));

			for (j=0;j<=lsize[1];j++){
				printf("(%2d,%2d) ",xback(i,j),yback(i,j));
			}
			printf("\n");
		}
		printf("\n");

	ALL_SEQUENTIAL_END(seq4)

	#endif


}



void phase_recv_back(){

	// Determine who has the maximum value
	double_3int	Max, outMax;

	// Fill Max with our values.
	Max.val = lMax;
	Max.rank = rank;
	Max.i = lbegin[0] + lxMax;
	Max.j = lbegin[1] + lyMax;

    // Reduce the values.
	MPI_Allreduce(&Max, &outMax, 1, MPI_DOUBLE_3INT, opHMaxLoc, comm);

#ifdef DEBUG
	// Print the maximum
	printfroot("Max %f in proc: %d\n\n",outMax.val, outMax.rank);
#endif

	// Receive the point to continue the backtracking algorithm.
	if(rank == outMax.rank){

		// Avoid possible deadlock, if we are the processor with
		// the maximum value, we receive the messages but we ignore them.
		int pos;
		recv_Back(0, &pos);
		recv_Back(1, &pos);

	// Get the position from a neighbor
	} else {

		// By default we do not have the path.
		lxMax=-1;
		lyMax=-1;

		// Receive the path from the neigh from dim 0
		int pos = -1;
		recv_Back(0, &pos);

		if(pos != -1){
			lxMax = lsize[0];
			lyMax = pos;
		}

		// Receive the path from the neigh from dim 1
		pos = -1;
		recv_Back(1, &pos);

		if(pos != -1){
			lxMax = pos;
			lyMax = lsize[1];
		}
	}


}





void phase_send_back(){

	int i = lxBegin;
	int j = lyBegin;

	// Send a message to the neighbors to continue or stop
	// backtracking algorithm
	if( i == -1 || j == -1  ){
		// The current processors do not have any point of the path of the alignment
		send_Back(0, -1);
		send_Back(1, -1);
	} else if(h(i,j) > 0 && i == 0  ){
		// Neighbor in dim 0 should continue the algorithm.
		send_Back(0, j);
		send_Back(1, -1);
	} else if (h(i,j) > 0 &&  j == 0  ){
		// Neighbor in dim 1 should continue the algorithm.
		send_Back(0, -1);
		send_Back(1, i);
	} else {
		// Neighbors do not continue the search.
		send_Back(0, -1);
		send_Back(1, -1);
	}

}



void phase_comp_back(){


	// reset to max point to do alignment
	// We start at the maximum point
	int i=lxMax;
	int j=lyMax;

	// Position of the output proteins.
	int pos=lsize[0]+lsize[1]+1;
	int outsize=0;

	// Backtracking algorithm
	while (i>0 && j>0 && h(i,j) > 0){

		int movex = (i>xback(i,j));
		int movey = (j>yback(i,j));

		pos--;
		outsize++;

		if (movex && movey) {
			lout1[pos]=lp1[i];
			lout2[pos]=lp2[j];
		} else if (movey) {
			lout1[pos]=GAP_AA;
			lout2[pos]=lp2[j];
		} else if (movex) {
			lout1[pos]=lp1[i];
			lout2[pos]=GAP_AA;
		}

		// Move to the next point.
		int tempi=i;
		int tempj=j;
		i=xback(tempi,tempj);
		j=yback(tempi,tempj);
	}

	pos--;

	lmatch_pos = pos;

	lmatch_length = outsize;
	lmatch_length1 = lxMax - i;
	lmatch_length2 = lyMax - j;

	lxBegin = i;
	lyBegin = j;

}



void debug_show_backtracking(){

#ifdef DEBUG

	// Local result print
ALL_SEQUENTIAL_BEGIN(seq5);
	printf("Processor: %d/%d, ",rank,nProcs);
	printf("Coords %d x %d\n",lcoords[0],lcoords[1]);

	printf(" First point (%d,%d) -> Last point (%d,%d)\n",xMax,yMax,lxBegin,lyBegin);

	printf(" Local Match length of A: %d\n", lmatch_length1);
	printf(" Local Match length of B: %d\n", lmatch_length2);
	printf(" Local Alignment length: %d\n", lmatch_length);

	printf(" MCH1: "); printProteinMatch(lp1,lsize[0],lxBegin,lxMax);
	printf(" MCH2: "); printProteinMatch(lp2,lsize[1],lyBegin,lyMax);

	printf(" LOCAL OUT1: "); printProtein(&lout1[lmatch_pos],lmatch_length);
	printf(" LOCAL OUT2: "); printProtein(&lout2[lmatch_pos],lmatch_length);
	printf("\n");
ALL_SEQUENTIAL_END(seq5);
#endif


}


void distributeProtein(aa_t * protein, aa_t *lprotein, int dim){

	// IRecv
	MPI_Request request;
	MPI_Irecv(&lprotein[1], lsize[dim], MPI_AA, 0, 0, comm, &request);

	// Non root processor can exit
	if(rank != 0){
		MPI_Wait(&request,MPI_STATUS_IGNORE);
		return;
	}

	// Send from root
	int p;
	for(p=0; p<nProcs; p++){

		// Get the coordinates to calculate the begin and size
		int pcoords[2];
		MPI_Cart_coords(comm,p,2,pcoords);
		int pbegin = BEGIN(pcoords[dim],dims[dim],size[dim]);
		int psize =  SIZE(pcoords[dim],dims[dim],size[dim]);

		// Pointer to the beginning of the local part
		void * buff = &protein[pbegin + 1];

		MPI_Send(buff, psize, MPI_AA, p, 0, comm);

	}

	// Root waits for its own part
	MPI_Wait(&request,MPI_STATUS_IGNORE);

}





void send_Back(int dir, int pos){

	int pcoords[2];

	pcoords[0] = lcoords[0];
	pcoords[1] = lcoords[1];

	if(dir == 0)
		pcoords[0]--;
	else
		pcoords[1]--;

	// Exit if we have no one to send.
	if(pcoords[0] < 0) return;
	if(pcoords[1] < 0) return;


	// Get the rank of the neighbor
	int prank;
	MPI_Cart_rank(comm,pcoords,&prank);
	MPI_Send(&pos, 1, MPI_INT, prank, 0, comm);

}


void recv_Back(int dir, int * pos){

	int pcoords[2];

	pcoords[0] = lcoords[0];
	pcoords[1] = lcoords[1];

	if(dir == 0)
		pcoords[0]++;
	else
		pcoords[1]++;

	if(pcoords[0] >= dims[0]) return;
	if(pcoords[1] >= dims[1]) return;

	// Get the rank of the neighbor
	int prank;
	MPI_Cart_rank(comm,pcoords,&prank);
	MPI_Recv(pos, 1, MPI_INT, prank, 0, comm, MPI_STATUS_IGNORE);

}




void phase_compose_sequence(){

	// Calculate the local part of the proteins that is being composed
	xBegin = lbegin[0] + lxBegin;
	xMax = lbegin[0] + lxMax;
	yBegin = lbegin[1] + lyBegin;
	yMax = lbegin[1] + lyMax;


	// Compose the global proteins
	composeOutput(out1,&match_pos,&match_length,&lout1[lmatch_pos+1],lmatch_length, &xBegin, &xMax);
	composeOutput(out2,&match_pos,&match_length,&lout2[lmatch_pos+1],lmatch_length, &yBegin, &yMax);


}




#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))



void composeOutput(aa_t * out, int * outpos, int * outsize, aa_t * lout, int loutsize, int * gBegin, int * gEnd){


	// MPI_Request for all the Sends.
	MPI_Request request[4];

	// Each processor sends to root its size.
	MPI_Isend(&loutsize, 1, MPI_INT, 0, 1, comm, &request[0]);

	// If the size is not 0, they also send the begin and end position of
	// the alignment and the array with the local alignment.
	if(loutsize > 0){
		MPI_Isend(gBegin, 1, MPI_INT, 0, 1, comm, &request[1]);
		MPI_Isend(gEnd, 1, MPI_INT, 0, 1, comm, &request[2]);
		MPI_Isend(lout, loutsize, MPI_AA, 0, 1, comm, &request[3]);
	}

	// Non-root processors exit after waiting the isends.
	if(rank != 0){
		MPI_Wait(&request[0],MPI_STATUS_IGNORE);
		if(loutsize > 0){
			MPI_Wait(&request[1],MPI_STATUS_IGNORE);
			MPI_Wait(&request[2],MPI_STATUS_IGNORE);
			MPI_Wait(&request[3],MPI_STATUS_IGNORE);
		}
		return;
	}

	// Reset the first index of the alignment (output)
	// and the size of the combined output (outsize).
	*outpos = size[0]+size[1];
	*outsize = 0;

	// Variables to keep the global indexes of the alignment.
	int first_flag = 1;
	int minBegin = 0;
	int maxEnd = 0;


	// Root receives from every processor
	int p;
	for(p=nProcs-1; p>=0; p--){

		// Receive the output local size of the p processor.
		int poutsize;
		MPI_Recv(&poutsize,1,MPI_INT,p,1,comm,MPI_STATUS_IGNORE);

		// Skip processors without alignment.
		if(!(poutsize > 0)) continue;

		// Get the begin and end
		int pgBegin;
		int pgEnd;
		MPI_Recv(&pgBegin,1,MPI_INT,p,1,comm,MPI_STATUS_IGNORE);
		MPI_Recv(&pgEnd,1,MPI_INT,p,1,comm,MPI_STATUS_IGNORE);

		// Update the begin and end indexes.
		if(first_flag){
			minBegin = pgBegin;
			maxEnd = pgEnd;
			first_flag = 0;
		} else {
			minBegin = MIN(minBegin,pgBegin);
			maxEnd = MAX(maxEnd,pgEnd);
		}

		// Update the outpos and outsize
		(*outpos) -= poutsize;
		(*outsize) += poutsize;

		// Receive the alignment.
		MPI_Recv(&out[(*outpos)],poutsize,MPI_AA,p,1,comm,MPI_STATUS_IGNORE);

	}

	// This is because the proteins start at 1.
	(*outpos)--;


	// Wait for the isends.
	// This could be not necessary since root uses blocking recv calls.
	MPI_Wait(&request[0],MPI_STATUS_IGNORE);
	if(loutsize > 0){
		MPI_Wait(&request[1],MPI_STATUS_IGNORE);
		MPI_Wait(&request[2],MPI_STATUS_IGNORE);
		MPI_Wait(&request[3],MPI_STATUS_IGNORE);

	}

	// Return the global Begin and End.
	*gBegin = minBegin;
	*gEnd = maxEnd;

}


