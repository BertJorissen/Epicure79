/**
 * @file SWseq.c
 *
 * Hitmap Parallel version for the Smith-Waterman algorihtm.
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
 * @date June 2013
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

// Hitmap Include
#include <hitmap.h>

// Include the common variables and utilities for all the versions
#include "SWcommon.h"

// Include the common variables and utilities for the Hitmap versions
#include "SWcommon_hit.h"



// Hitmap tile integer type
hit_tileNewType(int);


/** Hitmap comm type for the amino acid. */
#define aa_hit_t MPI_SHORT
/** Hitmap comm type of the H and PAM matrices */
#define h_hit_t MPI_DOUBLE


/**
 * Function that distributes the protein from the root processor to the remain procs.
 */
void distributeProtein(HitTile_aa_t protein, HitTile_aa_t lprotein, int dim);


/**
 * Struct used to calculate the processor where
 * the maximum H matrix value is.
 */
hit_tileNewType(double_3int);


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
void composeOutput(HitTile_aa_t out, int * outpos, int * outsize, HitTile_aa_t lout, int loutpos, int loutsize, int * gBegin, int * gEnd);





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

/** Size of the local part of the proteins. */
int lsize[2];

/** MPI reduce function for double_3int */
MPI_Op opHMaxLoc;

/** Hitmap layout */
HitLayout layout;

/** Shape from the layout */
HitShape layshape;



// **********************************************************************
// Global root variables
// **********************************************************************

// Protein sequences
HitTile_aa_t p1;
HitTile_aa_t p2;

// Output proteins with the alignment
HitTile_aa_t out1;
HitTile_aa_t out2;

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
HitTile_aa_t  lp1;
HitTile_aa_t  lp2;

// Local output proteins with the alignment
HitTile_aa_t  lout1;
HitTile_aa_t  lout2;

// Similarity matrix
HitTile_h_t H;
#define h(i,j) (hit_tileElemAtNoStride(H,2,i,j))

// Traceback matrices
HitTile_trace_t xTraceback;
HitTile_trace_t yTraceback;
#define xback(i,j) (hit_tileElemAtNoStride(xTraceback,2,i,j))
#define yback(i,j) (hit_tileElemAtNoStride(yTraceback,2,i,j))


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


HitShape lshape;



/**
 * Main function.
 */
int main(int argc, char *argv[]) {

	// **********************************************************************
	// Hitmap init
	// **********************************************************************
	hit_comInit( &argc, &argv );


	// **********************************************************************
	// HIT datatypes
	// **********************************************************************

	hit_comTypeStruct(&MPI_DOUBLE_3INT, double_3int, 4,
			val, 1, HIT_DOUBLE,
			rank, 1, HIT_INT,
			i, 1, HIT_INT,
			j, 1, HIT_INT);


	// Create the operation to reduce the double_3int values.
    MPI_Op_create( HMaxLoc, 1,  &opHMaxLoc );

	// **********************************************************************
	// Check input parameters
	// **********************************************************************
	input_parameters(argc, argv);

	// **********************************************************************
	// Layout
	// **********************************************************************

	// Topology
    HitTopology topo = hit_topology( plug_topArray2DComplete );

	// Print processor info
	printfroot("=== Parallel ===\n"
		" Processors: %d\n"
		" Processors: %d x %d\n\n",hit_NProcs,hit_topDimCard(topo,0), hit_topDimCard(topo,1));

    // Layout
    HitShape shape = hit_shape(2,hit_sig(1,size[0],1),hit_sig(1,size[1],1));
    layout = hit_layout( plug_layBlocks, topo, shape );

    // Shape from the layout
    layshape = hit_layShape(layout);
    lsize[0] = hit_shapeSigCard(layshape,0);
    lsize[1] = hit_shapeSigCard(layshape,1);

    // Local shape to alloc the H matriz
    lshape = hit_shapeTransform(layshape,0,HIT_SHAPE_BEGIN,-1);
    lshape = hit_shapeTransform(lshape  ,1,HIT_SHAPE_BEGIN,-1);

    // Shapes for the proteins
    HitShape protshape[2];
    protshape[0] = hit_shape(1,hit_shapeSig(lshape,0));
    protshape[1] = hit_shape(1,hit_shapeSig(lshape,1));

	// Print info of each processor
#ifdef DEBUG
	ALL_SEQUENTIAL_BEGIN(seq1)
	printf("Processor: %d/%d, ",hit_Rank,hit_NProcs);
	printf("coords %d x %d\n", hit_laySelfRanksDim(layout,0), hit_laySelfRanksDim(layout,1));
	printf("Dim1: Block %2d- (%2d)\n", hit_shapeSig(layshape,0).begin , hit_shapeSigCard(layshape,0));
	printf("Dim2: Block %2d- (%2d)\n", hit_shapeSig(layshape,1).begin , hit_shapeSigCard(layshape,1));
	printf("\n");
	ALL_SEQUENTIAL_END(seq1)
#endif


	// **********************************************************************
	// Timers
	// **********************************************************************

	HitClock time_init; /**< Time init  */
	HitClock time_comp; /**< Time computation  */

	HitClock time_comp_read;	/**< Accumulate time in read */
	HitClock time_comp_hmatrix;	/**< Accumulate time in H matrix computation */
	HitClock time_comp_back;	/**< Accumulate time in backtracking */

	HitClock time_comp_distr;	/**< Accumulate time in protein sequence distribution */
	HitClock time_comp_syncH;	/**< Accumulate time in H matrix sync */
	HitClock time_comp_syncB;	/**< Accumulate time in backtracking sync */
	HitClock time_comp_compose;	/**< Accumulate time in final protein composition */

	hit_clockReset(time_comp_read);
	hit_clockReset(time_comp_hmatrix);
	hit_clockReset(time_comp_back);

	hit_clockReset(time_comp_distr);
	hit_clockReset(time_comp_syncH);
	hit_clockReset(time_comp_syncB);
	hit_clockReset(time_comp_compose);


	// **********************************************************************
	// INIT PHASE: Structure allocation and PAM matrix load
	// **********************************************************************

	// Init time
	hit_clockSynchronizeAll()
	hit_clockStart(time_init);

	// read the PAM matrix
	// Every one do this, the pam matrix is small and all procs
	// need it, we don't bother to read it and distribute it.
	initPAM(pam_name);

	// Open the proteins files
	openProtein(&pfile1,prot_name1);
	openProtein(&pfile2,prot_name2);

	// Allocate memory for the proteins (Only root has them)
	if(is_root()){
		hit_tileDomainAlloc(&p1,aa_t,1,size[0]+1);
		hit_tileDomainAlloc(&p2,aa_t,1,size[1]+1);
	}

	// Local proteins
	hit_tileDomainShapeAlloc(&lp1,aa_t,protshape[0]);
	hit_tileDomainShapeAlloc(&lp2,aa_t,protshape[1]);

	// Local Similarity matrix
	hit_tileDomainShapeAlloc(&H,h_t,lshape);

	// Traceback matrices
	hit_tileDomainShapeAlloc(&xTraceback, trace_t, lshape);
	hit_tileDomainShapeAlloc(&yTraceback, trace_t, lshape);

	// Allocate size of the output (local)
	hit_tileDomainAlloc(&lout1, aa_t, 1 ,lsize[0]+lsize[1]+1 );
	hit_tileDomainAlloc(&lout2, aa_t, 1 ,lsize[0]+lsize[1]+1 );


	// Allocate size of the output (global, only root)
	if(is_root()){
		hit_tileDomainAlloc(&out1, aa_t, 1 ,size[0]+size[1]+1 );
		hit_tileDomainAlloc(&out2, aa_t, 1 ,size[0]+size[1]+1 );
	}

	// Init time
	hit_clockStop(time_init);



	// **********************************************************************
	// WHOLE COMPUTATION PHASE: iteration of: Read, H matrix, Backtracking
	// **********************************************************************

	// Computation time
	hit_clockStart(time_comp);

	int iter;
	for(iter=0;iter<iterations;iter++){


#ifdef DEBUG
		printfroot("=== Iteration: %d/%d ===\n\n",iter,iterations);
#endif

		// **********************************************************************
		// READ PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_read);
		phase_read_sequences();
		hit_clockStop(time_comp_read);

		// **********************************************************************
		// DISTRIBUTION PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_distr);
		phase_distribute_sequences();
		hit_clockStop(time_comp_distr);


		// **********************************************************************
		// RECV H PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_syncH);
		// Recv the needed H values from the neighbors
		phase_recv_hmatrix();
		hit_clockStop(time_comp_syncH);


		// **********************************************************************
		// H MATRIX PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_hmatrix);
		phase_comp_hmatrix();
		hit_clockStop(time_comp_hmatrix);


		// **********************************************************************
		// SEND H PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_syncH);
		// Send the needed H values to the neighbors
		phase_send_hmatrix();
		hit_clockStop(time_comp_syncH);

#ifdef DEBUG
		debug_show_matrices();
#endif


		// **********************************************************************
		// RECV BACKTRAING PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_syncB);
		phase_recv_back();
		hit_clockStop(time_comp_syncB);


		// **********************************************************************
		// BACKTRACKING PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_back);
		phase_comp_back();
		hit_clockStop(time_comp_back);


		// **********************************************************************
		// SEND BACKTRAING PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_syncB);
		phase_send_back();
		hit_clockStop(time_comp_syncB);


		// **********************************************************************
		// COMPOSE FINAL PROTEIN PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_compose);
		phase_compose_sequence();
		hit_clockStop(time_comp_compose);


#ifdef DEBUG
		debug_show_backtracking();
#endif

	} // Iterations Loop

	// Computation time
	hit_clockSynchronizeAll()
	hit_clockStop(time_comp);


	// **********************************************************************
	// Show results
	// **********************************************************************

	// Reduce the timers
	hit_clockReduce(layout,time_comp_read);
	hit_clockReduce(layout,time_comp_hmatrix);
	hit_clockReduce(layout,time_comp_back);

	hit_clockReduce(layout,time_comp_distr);
	hit_clockReduce(layout,time_comp_syncH);
	hit_clockReduce(layout,time_comp_syncB);
	hit_clockReduce(layout,time_comp_compose);


#define allTimers(timer) hit_clockGetMinSeconds(timer),hit_clockGetAvgSeconds(timer),hit_clockGetMaxSeconds(timer)



	// Final result.
		if(is_root()){

			match_length1 = xMax - xBegin;
			match_length2 = yMax - yBegin;

			printf("=== Result ===\n");
			printf("Init time: %f\n",hit_clockGetSeconds(time_init));
			printf("Comp time: %f\n",hit_clockGetSeconds(time_comp));
			printf("  Read protein: min: %f, avg: %f, max: %f \n",allTimers(time_comp_read));
			printf("  Distribute:   min: %f, avg: %f, max: %f \n",allTimers(time_comp_distr));
			printf("  H matrix:     min: %f, avg: %f, max: %f \n",allTimers(time_comp_hmatrix));
			printf("  sync H mat:   min: %f, avg: %f, max: %f \n",allTimers(time_comp_syncH));
			printf("  Backtracking: min: %f, avg: %f, max: %f \n",allTimers(time_comp_back));
			printf("  Sync Back:    min: %f, avg: %f, max: %f \n",allTimers(time_comp_syncB));
			printf("  Compose:      min: %f, avg: %f, max: %f \n",allTimers(time_comp_compose));

			printf("Last Match length of A: %d\n", match_length1);
			printf("Last Match length of B: %d\n", match_length2);
			printf("Last Alignment length: %d\n", match_length);


			// Print the result for small inputsets
			if(match_length < 50){
				printf("\n");
				printf("MCH1: "); printProteinMatch(p1,xBegin,xMax);
				printf("MCH2: "); printProteinMatch(p2,yBegin,yMax);

				printf("\n");
				printf("OUT1: "); printProtein(out1,match_pos,match_length);
				printf("OUT2: "); printProtein(out2,match_pos,match_length);
			} else {
				printf("\nInputset too big to print to stdout\n");
			}
		}


    // @todo
	// Free all the resources
	hit_tileFree(lp1);
	hit_tileFree(lp2);
	hit_tileFree(xTraceback);
	hit_tileFree(yTraceback);
	hit_tileFree(H);
	hit_tileFree(lout1);
	hit_tileFree(lout2);
	MPI_Op_free(&opHMaxLoc);
	MPI_Type_free(&MPI_DOUBLE_3INT);

    hit_layFree(layout);
    hit_topFree(topo);

	if(is_root()){
		hit_tileFree(p1);
		hit_tileFree(p2);
		hit_tileFree(out1);
		hit_tileFree(out2);
	}


	// Finalize Hitmap
	hit_comFinalize();

	return EXIT_SUCCESS;


}





void phase_read_sequences(){

	if(! is_root() ) return;

	// Read the proteins
	readProtein(&pfile1, &p1, size[0]);
	readProtein(&pfile2, &p2, size[1]);

#ifdef DEBUG
	printf("PRT1: "); printProtein(p1,0,size[0]);
	printf("PRT2: "); printProtein(p2,0,size[1]);
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
	printf("Processor: %d/%d, ",hit_Rank,hit_NProcs);
	printf("Coords %d x %d\n", hit_laySelfRanksDim(layout,0), hit_laySelfRanksDim(layout,1));
	printf("Local protein 1: ");
	printProtein(lp1,0,lsize[0]);
	printf("Local protein 2: ");
	printProtein(lp2,0,lsize[1]);
	printf("\n");
	ALL_SEQUENTIAL_END(seq2)
#endif

}





void phase_recv_hmatrix(){

	HitPattern pattern = hit_pattern( HIT_PAT_UNORDERED );

	HitShape selectX = hit_shape(2, hit_sigIndex(0), hit_sig(1,hit_tileDimCard(H,1)-1,1) );
	HitRanks neighX = hit_layNeighbor(layout, 0, -1);
	HitCom commX = hit_comRecvSelect(layout,neighX,&H,selectX,HIT_COM_TILECOORDS, h_hit_t);
	hit_patternAdd(&pattern,commX);


	HitShape selectY = hit_shape(2,hit_sig(0,hit_tileDimCard(H,0)-1,1),hit_sigIndex(0));
	HitRanks neighY = hit_layNeighbor(layout, 1, -1);
	HitCom commY = hit_comRecvSelect(layout,neighY,&H,selectY,HIT_COM_TILECOORDS, h_hit_t);
	hit_patternAdd(&pattern,commY);

	hit_patternDo(pattern);
	hit_patternFree(&pattern);

}



void phase_send_hmatrix(){


	HitPattern pattern = hit_pattern( HIT_PAT_UNORDERED );

	HitShape selectX = hit_shape(2, hit_sigIndex(hit_tileDimCard(H,0)-1), hit_sig(1,hit_tileDimCard(H,1)-1,1 ) );
	HitRanks neighX = hit_layNeighbor(layout, 0, 1);
	HitCom commX = hit_comSendSelect(layout,neighX,&H,selectX,HIT_COM_TILECOORDS, h_hit_t);
	hit_patternAdd(&pattern,commX);


	HitShape selectY = hit_shape(2,hit_sig(0,hit_tileDimCard(H,0)-1,1 ), hit_sigIndex(hit_tileDimCard(H,1)-1));
	HitRanks neighY = hit_layNeighbor(layout, 1, 1);
	HitCom commY = hit_comSendSelect(layout,neighY,&H,selectY,HIT_COM_TILECOORDS, h_hit_t);
	hit_patternAdd(&pattern,commY);

	hit_patternDo(pattern);
	hit_patternFree(&pattern);


}



void phase_comp_hmatrix(){

	// Compute the Similarity Matrix (H)

	//  Initialize values
	int i,j;

	if(hit_laySelfRanks(layout).rank[1] == 0){
		for (i=0;i<=lsize[0];i++){
			xback(i,0) = -1;
			yback(i,0) = -1;
			h(i,0)=0;
		}
	}

	if(hit_laySelfRanks(layout).rank[0] == 0){
		for (j=0;j<=lsize[1];j++){
			xback(0,j) = -1;
			yback(0,j) = -1;
			h(0,j)=0;
		}
	}

	// Maximum values to retrieve the alignment
	lMax = -1;
	xMax = -1;
	yMax = -1;

	// Perform wavefront algorithm
	for(i=1; i<lsize[0]+1; i++){
		for(j=1; j<lsize[1]+1; j++){

			// Get the scores
			h_t diag  = h(i-1,j-1) + pam(hit_tileElemAt(lp1,1,i),hit_tileElemAt(lp2,1,j));
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

		ALL_SEQUENTIAL_BEGIN(seq3)

		printf("Processor: %d/%d, ",hit_Rank,hit_NProcs);
		printf("coords %d x %d\n", hit_laySelfRanksDim(layout,0), hit_laySelfRanksDim(layout,1));

		// Print
		printf("   ");
		for (j=0;j<=lsize[1];j++){
			if(j==0)
				printf("     -");
			else
				printf("%6c",AA2char(hit_tileElemAt(lp2,1,j)));
		}
		printf("\n");
		for (i=0;i<=lsize[0];i++){

			if(i==0)
				printf("- |");
			else
				printf("%c |",AA2char(hit_tileElemAt(lp1,1,i)));

			for (j=0;j<=lsize[1];j++){
				printf("%6.1f",(double)h(i,j));
			}
			printf("\n");
		}
		printf("\n");

		ALL_SEQUENTIAL_END(seq3)
	#endif



	#ifdef DEBUG

	ALL_SEQUENTIAL_BEGIN(seq4)

	printf("Processor: %d/%d, ",hit_Rank,hit_NProcs);
	printf("coords %d x %d\n", hit_laySelfRanksDim(layout,0), hit_laySelfRanksDim(layout,1));

		// Print
		printf("   ");
		for (j=0;j<=lsize[1];j++){
			if(j==0)
				printf("      -");
			else
				printf("%6c",AA2char(hit_tileElemAt(lp2,1,j)));
		}
		printf("\n");
		for (i=0;i<=lsize[0];i++){

			if(i==0)
				printf("- |");
			else
				printf("%c |",AA2char(hit_tileElemAt(lp1,1,i)));

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
	HitTile_double_3int Max, outMax;
	hit_tileDomainAlloc(&Max,double_3int,1,1);
	hit_tileDomainAlloc(&outMax,double_3int,1,1);

	// Fill Max with our values.
	hit_tileElemAt(Max,1,0).val = lMax;
	hit_tileElemAt(Max,1,0).rank = hit_Rank;
	hit_tileElemAt(Max,1,0).i = hit_shapeSig(layshape,0).begin + xMax;
	hit_tileElemAt(Max,1,0).j = hit_shapeSig(layshape,1).begin + yMax;

    // Reduce the values.
	HitCom comm_reduce = hit_comReduce(layout,HIT_RANKS_NULL,&Max,&outMax,MPI_DOUBLE_3INT,opHMaxLoc);
	hit_comDo(&comm_reduce);
	hit_comFree(comm_reduce);

#ifdef DEBUG
	// Print the maximum
	printfroot("Max %f in proc: %d\n\n",hit_tileElemAt(outMax,1,0).val, hit_tileElemAt(outMax,1,0).rank);
#endif

	// Receive the point to continue the backtracking algorithm.
	if(hit_Rank == hit_tileElemAt(outMax,1,0).rank){

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


	hit_tileFree(Max);
	hit_tileFree(outMax);

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
			hit_tileElemAt(lout1,1,pos)=hit_tileElemAt(lp1,1,i);
			hit_tileElemAt(lout2,1,pos)=hit_tileElemAt(lp2,1,j);
		} else if (movey) {
			hit_tileElemAt(lout1,1,pos)=GAP_AA;
			hit_tileElemAt(lout2,1,pos)=hit_tileElemAt(lp2,1,j);
		} else if (movex) {
			hit_tileElemAt(lout1,1,pos)=hit_tileElemAt(lp1,1,i);
			hit_tileElemAt(lout2,1,pos)=GAP_AA;
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
	printf("Processor: %d/%d, ",hit_Rank,hit_NProcs);
	printf("Coords %d x %d\n", hit_laySelfRanksDim(layout,0), hit_laySelfRanksDim(layout,1));

	printf(" First point (%d,%d) -> Last point (%d,%d)\n",lxMax,lyMax,lxBegin,lyBegin);

	printf(" Local Match length of A: %d\n", lmatch_length1);
	printf(" Local Match length of B: %d\n", lmatch_length2);
	printf(" Local Alignment length: %d\n", lmatch_length);

	printf(" MCH1: "); printProteinMatch(lp1,lxBegin,lxMax);
	printf(" MCH2: "); printProteinMatch(lp2,lyBegin,lyMax);

	printf(" LOCAL OUT1: "); printProtein(lout1,lmatch_pos,lmatch_length);
	printf(" LOCAL OUT2: "); printProtein(lout2,lmatch_pos,lmatch_length);
	printf("\n");
ALL_SEQUENTIAL_END(seq5);
#endif


}

void phase_compose_sequence(){

	// Calculate the local part of the proteins that is being composed
	xBegin =hit_shapeSig(lshape,0).begin + lxBegin;
	xMax = hit_shapeSig(lshape,0).begin + lxMax;
	yBegin = hit_shapeSig(lshape,1).begin + lyBegin;
	yMax = hit_shapeSig(lshape,1).begin + lyMax;

	// Compose the global proteins
	composeOutput(out1,&match_pos,&match_length,lout1,lmatch_pos+1,lmatch_length, &xBegin, &xMax);
	composeOutput(out2,&match_pos,&match_length,lout2,lmatch_pos+1,lmatch_length, &yBegin, &yMax);

}


void distributeProtein(HitTile_aa_t protein, HitTile_aa_t lprotein, int dim){


	HitPattern pattern = hit_pattern( HIT_PAT_UNORDERED );

	if(hit_Rank == 0){
		int p;
		for(p=0; p<hit_layNumActives(layout); p++){

			HitRanks ranks = hit_layActiveIdRanks(layout, p);
			HitShape matrixshape = hit_layShapeOther(layout,ranks);
			HitShape shape = hit_shape(1,hit_shapeSig(matrixshape,dim));

			HitCom comm = hit_comSendSelect(layout,ranks,&protein,shape,HIT_COM_ARRAYCOORDS, aa_hit_t);
			hit_patternAdd(&pattern,comm);
		}
	}

	// @todo Use the one precalcualted
    HitShape lprotein_shape = hit_shape(1,hit_shapeSig(hit_layShape(layout),dim));


	HitCom comm = hit_comRecvSelect(layout, hit_ranks2(0,0), &lprotein, lprotein_shape, HIT_COM_ARRAYCOORDS, aa_hit_t);
	hit_patternAdd(&pattern,comm);
	hit_patternDo(pattern);

	hit_patternFree(&pattern);

}




void send_Back(int dir, int pos){

	HitTile_int hitpos;
	hit_tileDomainAlloc(&hitpos,int,1,1);
	hit_tileElemAt(hitpos,1,0) = pos;
	HitRanks neigh = hit_layNeighbor(layout, dir, -1);
	HitCom comm = hit_comSend(layout,neigh,&hitpos,HIT_INT);
	hit_comDo(&comm);

	hit_tileFree(hitpos);
	hit_comFree(comm);

}



void recv_Back(int dir, int * pos){

	HitTile_int hitpos;
	hit_tileDomainAlloc(&hitpos,int,1,1);
	hit_tileElemAt(hitpos,1,0) = -1;
	HitRanks neigh = hit_layNeighbor(layout, dir, 1);
	HitCom comm = hit_comRecv(layout,neigh,&hitpos,HIT_INT);
	hit_comDo(&comm);
	*pos = hit_tileElemAt(hitpos,1,0);

	hit_tileFree(hitpos);
	hit_comFree(comm);

}




#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))



void composeOutput(HitTile_aa_t out, int * outpos, int * outsize, HitTile_aa_t lout, int loutpos, int loutsize, int * gBegin, int * gEnd){

	// Send the local out size
	HitTile_int hitdata;
	hit_tileDomainAlloc(&hitdata,int,1,3);
	hit_tileElemAt(hitdata,1,0) = loutsize;
	hit_tileElemAt(hitdata,1,1) = *gBegin;
	hit_tileElemAt(hitdata,1,2) = *gEnd;

	// Each processor sends to root its size.
	HitCom comm1 = hit_comSend(layout,hit_ranks2(0,0),&hitdata,HIT_INT);
	hit_comStartSend(&comm1);

	HitCom comm2;


	// If the size is not 0, they also send the begin and end position of
	// the alignment and the array with the local alignment.
	if(loutsize > 0){

		HitShape selection = hit_shape(1,hit_sig(loutpos,loutpos+loutsize-1,1));

		comm2 = hit_comSendSelect(layout,hit_ranks2(0,0),&lout,selection,HIT_COM_TILECOORDS,aa_hit_t);
		hit_comStartSend(&comm2);

	}


	// Non-root processors exit after waiting the isends.
	if(hit_Rank != 0){
		hit_comEndSend(&comm1);
		if(loutsize > 0){
			hit_comEndSend(&comm2);
			hit_comFree(comm2);
		}
		hit_comFree(comm1);
		hit_tileFree(hitdata);
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

	// Recv data
	HitTile_int hitdataRecv;
	hit_tileDomainAlloc(&hitdataRecv,int,1,3);

	// Root receives from every processor
	int p;
	for(p=hit_NProcs-1; p>=0; p--){

		// Receive the output local size of the p processor.
		HitCom comm = hit_comRecv(layout,hit_layActiveIdRanks(layout,p),&hitdataRecv,HIT_INT);
		hit_comDo(&comm);
		hit_comFree(comm);

		// Each processor sends to root its size.
		int poutsize = hit_tileElemAt(hitdataRecv,1,0);

		// Get the begin and end
		int pgBegin = hit_tileElemAt(hitdataRecv,1,1);
		int pgEnd = hit_tileElemAt(hitdataRecv,1,2);



		// Skip processors without alignment.
		if(!(poutsize > 0)) continue;

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
		HitShape selection = hit_shape(1,hit_sig((*outpos),(*outpos)+poutsize-1,1));

		HitCom commB = hit_comRecvSelect(layout,hit_layActiveIdRanks(layout,p),&out,selection,HIT_COM_TILECOORDS,aa_hit_t);
		hit_comDo(&commB);
		hit_comFree(commB);

	}

	hit_tileFree(hitdataRecv);

	// This is because the proteins start at 1.
	(*outpos)--;

	// Return the global Begin and End.
	*gBegin = minBegin;
	*gEnd = maxEnd;

	hit_comEndSend(&comm1);
	hit_comFree(comm1);

	if(loutsize > 0){
		hit_comEndSend(&comm2);
		hit_comFree(comm2);

	}

	hit_tileFree(hitdata);

}




