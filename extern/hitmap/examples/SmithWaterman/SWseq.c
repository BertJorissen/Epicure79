/**
 * @file SWseq.c
 *
 * Hitmap Sequential version for the Smith-Waterman algorihtm.
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


// **********************************************************************
// Declarations of the different phases
// **********************************************************************

/**
 * Read the protein sequences
 */
void phase_read_sequences();

/**
 * Calculate the similarity matrix (H)
 */
void phase_comp_hmatrix();

/**
 * Compute the traceback
 */
void phase_comp_back();


// **********************************************************************
// Global variables
// **********************************************************************

// Protein sequences
HitTile_aa_t p1;
HitTile_aa_t p2;

// Similarity matrix
HitTile_h_t H;
#define h(i,j) (hit_tileElemAtNoStride(H,2,i,j))

// Traceback matrices
HitTile_trace_t xTraceback;
HitTile_trace_t yTraceback;
#define xback(i,j) (hit_tileElemAtNoStride(xTraceback,2,i,j))
#define yback(i,j) (hit_tileElemAtNoStride(yTraceback,2,i,j))

// Output proteins with the alignment
HitTile_aa_t out1;
HitTile_aa_t out2;

// X and Y coordinates of the maximum value (End coordinates of the match)
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




/**
 * Main function.
 */
int main(int argc, char *argv[]) {

	// **********************************************************************
	// Hitmap init
	// **********************************************************************
	// We need this just for the clocks.
	hit_comInit( &argc, &argv );

	// **********************************************************************
	// Check input parameters
	// **********************************************************************
	input_parameters(argc, argv);


	// **********************************************************************
	// Timers
	// **********************************************************************
	HitClock time_init; /**< Time init  */
	HitClock time_comp; /**< Time computation  */

	HitClock time_comp_read;	/**< Accumulate time in read */
	HitClock time_comp_hmatrix;	/**< Accumulate time in H matrix computation */
	HitClock time_comp_back;	/**< Accumulate time in backtracking */

	hit_clockReset(time_comp_read);
	hit_clockReset(time_comp_hmatrix);
	hit_clockReset(time_comp_back);


	// **********************************************************************
	// INIT PHASE: Structure allocation and PAM matrix load
	// **********************************************************************

	// Init time
	hit_clockStart(time_init);

	// read the PAM matrix
	initPAM(pam_name);

	// Open the proteins files
	openProtein(&pfile1,prot_name1);
	openProtein(&pfile2,prot_name2);

	// Allocate memory for the proteins
	hit_tileDomainAlloc(&p1, aa_t, 1, size[0]+1);
	hit_tileDomainAlloc(&p2, aa_t, 1, size[1]+1);

	// Similarity matrix
	hit_tileDomainAlloc(&H, h_t, 2, size[0]+1, size[1]+1);

	// Traceback matrices
	hit_tileDomainAlloc(&xTraceback, trace_t, 2 ,size[0]+1, size[1]+1);
	hit_tileDomainAlloc(&yTraceback, trace_t, 2 ,size[0]+1, size[1]+1);

	// Allocate size of the output
	hit_tileDomainAlloc(&out1, aa_t, 1 ,size[0]+size[1]+1);
	hit_tileDomainAlloc(&out2, aa_t, 1 ,size[0]+size[1]+1);

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
		printf("=== Iteration: %d/%d ===\n\n",iter,iterations);
#endif

		// **********************************************************************
		// READ PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_read);
		phase_read_sequences();
		hit_clockStop(time_comp_read);


		// **********************************************************************
		// H MATRIX PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_hmatrix);
		phase_comp_hmatrix();
		hit_clockStop(time_comp_hmatrix);


		// **********************************************************************
		// BACKTRACKING PHASE
		// **********************************************************************
		hit_clockContinue(time_comp_back);
		phase_comp_back();
		hit_clockStop(time_comp_back);

	} // Iterations Loop

	// Computation time
	hit_clockStop(time_comp);



	// **********************************************************************
	// Show results
	// **********************************************************************

	printf("=== Result ===\n");
	printf("Init time: %f\n",hit_clockGetSeconds(time_init));
	printf("Comp time: %f\n",hit_clockGetSeconds(time_comp));
	printf("  Read:         %f\n",hit_clockGetSeconds(time_comp_read));
	printf("  H matrix:     %f\n",hit_clockGetSeconds(time_comp_hmatrix));
	printf("  Backtracking: %f\n\n",hit_clockGetSeconds(time_comp_back));

	printf("Last Match length of A: %d\n", match_length1);
	printf("Last Match length of B: %d\n", match_length2);
	printf("Last Alignment length: %d\n", match_length);

	// Print the result for small inputsets
	if(match_length < 50){
		printf("\n");
		printf("Last MCH1: "); printProteinMatch(p1,xBegin,xMax);
		printf("Last MCH2: "); printProteinMatch(p2,yBegin,yMax);
		printf("\n");

		printf("Last OUT1: "); printProtein(out1,match_pos,match_length);
		printf("Last OUT2: "); printProtein(out2,match_pos,match_length);
	} else {
		printf("\nInputset too big to print to stdout\n");
	}

	if( pfile1.rewind || pfile2.rewind ){
		printf("\nSmall inputset, it has been rewinded:\n"
				" %s: %8d times\n"
				" %s: %8d times\n", prot_name1,pfile1.rewind, prot_name2,  pfile2.rewind);
	}



	// **********************************************************************
	// Free all the resources
	// **********************************************************************

	// Free all the resources
	hit_tileFree(p1);
	hit_tileFree(p2);
	hit_tileFree(xTraceback);
	hit_tileFree(yTraceback);
	hit_tileFree(H);
	hit_tileFree(out1);
	hit_tileFree(out2);


	// We need this just for the clocks.
	hit_comFinalize();

	return EXIT_SUCCESS;

}




void phase_read_sequences(){

	// Read the proteins
	readProtein(&pfile1, &p1, size[0]);
	readProtein(&pfile2, &p2, size[1]);


#ifdef DEBUG
	printf("PRT1: "); printProtein(p1,0,size[0]);
	printf("PRT2: "); printProtein(p2,0,size[1]);
	printf("\n");
#endif


}



void phase_comp_hmatrix(){


	// Compute the Similarity Matrix (H)

	//  Initialize values
	int i,j;
	for (i=0;i<=size[0];i++){
		xback(i,0) = -1;
		yback(i,0) = -1;
		h(i,0)=0;
	}

	for (j=0;j<=size[1];j++){
		xback(0,j) = -1;
		yback(0,j) = -1;
		h(0,j)=0;
	}


	// Maximum values to retrieve the alignment
	h_t Max = -1;
	xMax = -1;
	yMax = -1;

	// Perform wavefront algorithm
	for(i=1; i<size[0]+1; i++){
		for(j=1; j<size[1]+1; j++){

			// Get the scores
			h_t diag  = h(i-1,j-1) + pam(hit_tileElemAt(p1,1,i),hit_tileElemAt(p2,1,j));
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
			if(max >= Max){
				xMax = i;
				yMax = j;
				Max = max;
			}

		} // End loop j;
	} // End loop i;


#ifdef DEBUG
	// Print
	printf("   ");
	for (j=0;j<=size[1];j++){
		if(j==0)
			printf("     -");
		else
			printf("%6c",AA2char(hit_tileElemAt(p2,1,j)));
	}
	printf("\n");
	for (i=0;i<=size[0];i++){

		if(i==0)
			printf("- |");
		else
			printf("%c |",AA2char(hit_tileElemAt(p1,1,i)));

		for (j=0;j<=size[1];j++){
			printf("%6.1f",(double)h(i,j));
		}
		printf("\n");
	}
	printf("\n");
#endif


#ifdef DEBUG
	// Print
	printf("   ");
	for (j=0;j<=size[1];j++){
		if(j==0)
			printf("      -");
		else
			printf("%8c",AA2char(hit_tileElemAt(p2,1,j)));
	}
	printf("\n");
	for (i=0;i<=size[0];i++){

		if(i==0)
			printf("- |");
		else
			printf("%c |",AA2char(hit_tileElemAt(p1,1,i)));

		for (j=0;j<=size[1];j++){
			printf("(%2d,%2d) ",xback(i,j),yback(i,j));
		}
		printf("\n");
	}
	printf("\n");
#endif



}




void phase_comp_back(){


	// reset to max point to do alignment
	// We start at the maximum point
	int i=xMax;
	int j=yMax;

	// Position of the output proteins.
	int pos=size[0]+size[1]+1;
	int outsize=0;

	while (i>0 && j>0 && h(i,j) > 0){

		int movex = (i>xback(i,j));
		int movey = (j>yback(i,j));

		pos--;
		outsize++;

		if (movex && movey) {
			hit_tileElemAt(out1,1,pos)=hit_tileElemAt(p1,1,i);
			hit_tileElemAt(out2,1,pos)=hit_tileElemAt(p2,1,j);
		} else if (movey) {
			hit_tileElemAt(out1,1,pos)=GAP_AA;
			hit_tileElemAt(out2,1,pos)=hit_tileElemAt(p2,1,j);
		} else if (movex) {
			hit_tileElemAt(out1,1,pos)=hit_tileElemAt(p1,1,i);
			hit_tileElemAt(out2,1,pos)=GAP_AA;
		}

		// Move to the next point.
		int tempi=i;
		int tempj=j;
		i=xback(tempi,tempj);
		j=yback(tempi,tempj);
	}

	pos--;

	match_length = outsize;
	match_length1 = xMax - i;
	match_length2 = yMax - j;

	xBegin = i;
	yBegin = j;

	match_pos = pos;

#ifdef DEBUG

	printf("Match length of A: %d\n", match_length1);
	printf("Match length of B: %d\n", match_length2);
	printf("Alignment length: %d\n", match_length);
	printf("\n");

	// Print the result for small inputsets
	if(match_length < 50){
		printf("\n");
		printf("MCH1: "); printProteinMatch(p1,xBegin,xMax);
		printf("MCH2: "); printProteinMatch(p2,yBegin,yMax);
		printf("\n");

		printf("OUT1: "); printProtein(out1,match_pos,outsize);
		printf("OUT2: "); printProtein(out2,match_pos,outsize);
		printf("\n");
	}

#endif

}



