/**
 * @file SWcommon.c
 * Common definitions and utilities for the Smith-Waterman algorihtm.
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

// Include the header
#include "SWcommon.h"


// **********************************************************************
// Global variables definition
// **********************************************************************

// Protein and PAM matrix names
char * prot_name1;
char * prot_name2;
char * pam_name	;

// Gap penalty
double gapPenalty = DEFAULT_GAPPENALTY;

// Size of the input proteins
int size[2] = {DEFAULT_SIZE,DEFAULT_SIZE};

// Number of iterations
int iterations = DEFAULT_ITERATIONS;

// Protein files.
ProteinFile pfile1;
ProteinFile pfile2;

// The PAM matrix.
PAM pam_matrix;
// Variable for the PAM Matrix.
AAName aa_names;


/**
 * Process the input parameters
 */
void input_parameters(int argc, char * argv[]){

	if (argc < 4 || argc > 8){
		fprintfroot(stderr,
			"Usage: %s protein1 protein2 PAM [GapPenalty] [SIZE/SIZE1] [SIZE2] [ITERS]\n"
			"\t protein1:   file with the first protein.\n"
			"\t protein2:   file with the second protein.\n"
			"\t PAM:        Point accepted mutation (PAM) Matrix.\n"
			"\t GapPenalty: Penalty for gaps. Default = %.1f.\n"
			"\t SIZE/1:     Size limit of the proteins of just the first one. Default=%d.\n"
			"\t SIZE2:      Size limit of the second protein. Default=%d.\n"
			"\t ITERS       Number of iterations. Default = %d. Reads another sequence from the files.\n",
			argv[0],
			DEFAULT_GAPPENALTY, DEFAULT_SIZE, DEFAULT_SIZE, DEFAULT_ITERATIONS);
		exit(EXIT_FAILURE);
	}

	// Read the parameters
	prot_name1 = argv[1];
	prot_name2 = argv[2];
	pam_name   = argv[3];

	if(argc > 4) sscanf(argv[4], "%lf", &gapPenalty);
	if(argc > 5){
		size[0] = atoi(argv[5]);
 		size[1] = size[0];
	}
	if(argc > 6){
		size[1] = atoi(argv[6]);
	}
	if(argc > 7){
		iterations = atoi(argv[7]);
	}

	// Print summary parameters
	printfroot("=== Smith Waterman alignment parameters ===\n"
		" Protein1: %s, size: %d\n"
		" Protein2: %s, size: %d\n"
		" Iterations: %d\n"
		" PAM Matrix: %s\n"
		" Gap penalty: %f\n\n"
		,prot_name1,size[0],prot_name2,size[1],iterations,pam_name,gapPenalty);

}




/**
 * Inits the PAM Matrix with the content
 * of this file and also the AAName
 * conversion structure.
 */
void initPAM(char * filename){

	// Set the default values
	pam_matrix.num_aa = 0;
	int i,j;
	for(i=0; i<MAX_NUM_AA+1; i++){
		aa_names.AA2char[i] = NOTFOUND_CHAR;
	}
	for(i=0; i<256; i++){
		aa_names.char2AA[i] = NOTFOUND_AA;
	}

	// Open the file
	FILE * file = fopen(filename,"r");
	if(file == NULL){
		exit_error("PAM Matrix read error");
	}

	// Read the names and init the pam_matrix.
	char name;
	while(fscanf(file, "%c", &name) != EOF){
		if(name == '\n') break;
		aa_names.AA2char[pam_matrix.num_aa] = (char)tolower(name);
		aa_names.char2AA[toupper(name)] = (aa_t) pam_matrix.num_aa;
		aa_names.char2AA[tolower(name)] = (aa_t) pam_matrix.num_aa;
		pam_matrix.num_aa ++;
	}

	// Set the Gap Values
	aa_names.AA2char[GAP_AA] = GAP_CHAR;
	aa_names.char2AA[GAP_CHAR] = GAP_AA;

#ifdef DEBUG
	fflush(stdout);

	int col = 0;

	// print
	printfroot("\n");
	printfroot("=== AA2char ===\n");
	for(i=0; i<MAX_NUM_AA+1; i++){
		printfroot("AA2char(%3d)=%c ",i,AA2char((aa_t)i));
		col++;
		if(col==5){
			col = 0;
			printfroot("\n");
		}
	}
	printfroot("\n\n");


	// print just the ASCII printable characters
	col = 0;
	printfroot("=== char2AA === (only ASCII printable characters) \n");
	for(i=' '; i<'~'; i++){
		printfroot("char2AA(%c)=%3d ",(char)i,char2AA((char)i));
		col++;
		if(col==5){
			col = 0;
			printfroot("\n");
		}
	}
	printfroot("\n\n");
#endif


	// Read the values
	double temp;
	for (i=0; i<pam_matrix.num_aa; i++){
		for (j=0;j<=i;j++) {
			if (fscanf(file, "%lf", &temp) == EOF) {
				exit_error("PAM file error");
			}
			pam(i,j)= (h_t) temp;
		}
	}

	// Symmetrify
	for (i=0;i<pam_matrix.num_aa;i++){
		for (j=i+1;j<pam_matrix.num_aa;j++){
			pam(i,j)=pam(j,i);
		}
	}

#ifdef DEBUG
	// Print
	printfroot("=== PAM Matrix ===\n");
	printfroot("   ");
	for (j=0;j<pam_matrix.num_aa;j++){
		printfroot("%6c",AA2char((aa_t)j));
	}
	printfroot("\n");
	for (i=0;i<pam_matrix.num_aa;i++){
		printfroot("%c |",AA2char((aa_t)i));
		for (j=0;j<pam_matrix.num_aa;j++){
			printfroot("%6.1f",pam(i,j));
		}
		printfroot("\n");
	}
	printfroot("\n");
	fflush(stdout);
#endif

	// Close the file
	fclose(file);

}



/**
 * Open a protein file.
 */
void openProtein(ProteinFile * fprotein, char * filename){

	// Open the file
	fprotein->file = fopen(filename,"r");
	if(fprotein->file == NULL){
		exit_error("Protein file error");
	}
	fprotein->rewind=0;
}


/**
 * Close the protein file.
 */
void closeProtein(ProteinFile * fprotein){
	fclose(fprotein->file);
}


/**
 * Exit printing the given error.
 */
void exit_error(const char * s){
	fprintf(stderr,"%s\n",s);
	exit(EXIT_FAILURE);
}




#if defined(REFPAR) || defined(HITPAR)


/**
 * Function to reduce the double_3int struct to get the maximum value
 * of the H matrix in all the processors.
 */
void HMaxLoc(void * inPVoid, void * inoutPVoid, int *len, MPI_Datatype * dptr){

	NOT_USED(dptr);

	double_3int * in = (double_3int*) inPVoid;
	double_3int * inout = (double_3int*) inoutPVoid;

    int i;

    for (i=0; i< *len; ++i) {

    	if(in->val > inout->val){
    		inout[i] = in[i];
    	} else if ( in[i].val == inout[i].val ) {

        	if(in[i].i > inout[i].i || (in[i].i == inout[i].i && in[i].j > inout[i].j) ){
        		inout[i] = in[i];
        	}

    	}

    	// in->val < inout->val: do nothing

    }
}



#endif





#ifdef REFPAR


/**  MPI Rank of the current processor */
extern int rank;

/**
 * Returns 1 if the current processor is the root.
 */
int is_root(){
	return rank == 0;
}


#elif  HITPAR

// Include the hitmap header.
#include <hitmap.h>

/**
 * Returns 1 if the current processor is the root.
 */
int is_root(){
	return hit_Rank == 0;
}


#endif







