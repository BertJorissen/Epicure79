/**
 * @file SWcommon.h
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

#ifndef _SW_COMMON_
#define _SW_COMMON_



/**
 * Flag to see the debug messages.
 */
// #define DEBUG



// **********************************************************************
// Data types for the amino acids, H matrix and traceback matrices
// This types can be changed for all the benchmarks.
// You must change the parallel types accordingly.
// **********************************************************************

/** Typedef for the amino acid. */
typedef short aa_t;
/** Typedef of the H and PAM matrices */
typedef double h_t;
/** Typedef for the traceback matrix. */
typedef short trace_t;



// **********************************************************************
// Parallel types
// **********************************************************************
#if defined(REFPAR) || defined(HITPAR)
#include <mpi.h>

/** MPI_Datatype for the amino acid. */
#define MPI_AA MPI_SHORT
/** MPI_Datatype for the H and PAM matrices. */
#define MPI_H MPI_DOUBLE

#endif



// **********************************************************************
// Constants and Default values
// **********************************************************************

/** Max number of amino acids. */
#define MAX_NUM_AA	30
/** Default protein size. */
#define DEFAULT_SIZE        8
/** Default Gap Penalty. */
#define DEFAULT_GAPPENALTY   -1.0
/** Default number of iterations */
#define DEFAULT_ITERATIONS   1



// **********************************************************************
// Standard library includes
// **********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>




// **********************************************************************
// is_root() function
// **********************************************************************
#if defined(REFPAR) || defined(HITPAR)

/**
 * Returns 1 if the current processor is the root.
 */
int is_root();

#else // defined(REFSEQ) || defined(HITSEQ)

/**
 * In sequential there is only one processor.
 */
#define is_root() (1)

#endif



// **********************************************************************
// Root print functions
// **********************************************************************
/**
 * printf function for the root processor.
 */
#define printfroot(...)  if( is_root() ) { printf(__VA_ARGS__); }

/**
 * fprintf function for the root processor.
 */
#define fprintfroot(...) if( is_root() ) { fprintf(__VA_ARGS__); }

















// **********************************************************************
// PAM matrix definition and functions
// **********************************************************************

/**
 * Struct that contains the pam matrix.
 */
typedef struct {
	/** Number of amino acids. */
	int num_aa;

	/** Table */
	h_t data[MAX_NUM_AA * MAX_NUM_AA];
} PAM;

/**
 * The PAM matrix.
 */
extern PAM pam_matrix;

/**
 * Macro to access the PAM Matrix.
 */
#define pam(a,b) (pam_matrix.data[(a)*(pam_matrix.num_aa)+(b)])

/**
 * Inits the PAM Matrix with the content
 * of this file and also the AAName
 * conversion structure.
 */
void initPAM(char * file);





// **********************************************************************
// Protein file definition and functions
// **********************************************************************
// - The read functions are different for the C and Hitmap versions.

/**
 * Struct for a protein file.
 * The protein file is red several times to get different sequences.
 */
typedef struct {
	FILE * file;	/**< The file of the protein. */
	int rewind;		/**< Number of times the file has been rewritten. */
} ProteinFile;

/**
 * Open a protein file.
 */
void openProtein(ProteinFile * fprotein, char * filename);

/**
 * Close the protein file.
 */
void closeProtein(ProteinFile * fprotein);




// **********************************************************************
// Amino acid name values and functions.
// **********************************************************************

/**
 * Struct that contains the conversion between
 * amino acid names and value to compute.
 */
typedef struct {

	/** Conversion from name to amino acid */
	aa_t char2AA[256];
	/** Conversion from amino acid to name */
	char AA2char[MAX_NUM_AA+1]; // +1 is for the GAP

} AAName;

/** Value for not found AA code. */
#define NOTFOUND_AA -2
/** Value for not found char name. */
#define NOTFOUND_CHAR '!'
/** Value for the gap AA code. */
#define GAP_AA MAX_NUM_AA
/** Value for the gap char name. */
#define GAP_CHAR '-'

/** Variable for the PAM Matrix. */
extern AAName aa_names;

/**
 * Function to translate a amino acid
 * name to a integer value.
 */
static inline aa_t char2AA(char c){ return aa_names.char2AA[(int)c]; }

/**
 * Function to translate a amino acid
 * code to its character representation.
 */
static inline char AA2char(aa_t aa){ return aa_names.AA2char[aa]; }





// **********************************************************************
// Other utilities
// **********************************************************************

/** Hides the not used warning */
#define NOT_USED(p) ((void)(p))


/** Check NULL pointers */
#define CHECK_NULL(check){\
   if ((check)==NULL) \
      exit_error("Null Pointer allocating memory\n");\
   }


/**
 * Exit printing the given error.
 */
void exit_error(const char * s);



// **********************************************************************
// Global variables
// **********************************************************************

// Protein and PAM matrix names
extern char * prot_name1;
extern char * prot_name2;
extern char * pam_name	;

// Gap penalty
extern double gapPenalty;

// Size of the input proteins
extern int size[2];

// Number of iterations
extern int iterations;

// Protein files.
extern ProteinFile pfile1;
extern ProteinFile pfile2;




// **********************************************************************
// Common benchmark functions
// **********************************************************************

/**
 * Function that returns the index of the MAX value.
 */
static inline h_t MAX4(h_t a, h_t b, h_t c, h_t d, int * idx){

	*idx = 0;
	h_t max = a;

	if(b > max){
		max = b;
		*idx = 1;
	}

	if(c > max){
		max = c;
		*idx = 2;
	}

	if(d > max){
		max = d;
		*idx = 3;
	}

	return max;
}


/**
 * Process the input parameters
 */
void input_parameters(int argc, char * argv[]);







// **********************************************************************
// Parallel types and functions
// **********************************************************************

#if defined(REFPAR) || defined(HITPAR)

/**
 * Struct used to calculate the processor where
 * the maximum H matrix value is.
 */
typedef struct {
	double	val;	/**< value. */
	int		rank;	/**< processor rank. */
	int		i;		/**< x coordinate of the value. */
	int		j;		/**< y coordinate of the value. */
} double_3int;

/**
 * MPI_Datatype for the double_3int struct.
 */
MPI_Datatype MPI_DOUBLE_3INT;

/**
 * Function to reduce the double_3int struct to get the maximum value
 * of the H matrix in all the processors.
 */
void HMaxLoc(void * in, void * inout, int *len, MPI_Datatype * dptr);





#endif





/*
 * Macros to force the processor to print the output
 * in order based on the rank number.
 * This is used for debug.
 */
#ifdef DEBUG

#include <time.h>


#if defined(REFPAR)


#define ALL_SEQUENTIAL_BEGIN(var) \
	struct timespec var ## ts = {0,1000000}; \
	fflush(stdout); \
	int var = 0; \
	for(var=-1;var<nProcs+1;var++){ \
		if(rank == var){

#define ALL_SEQUENTIAL_END(var) \
		} else { \
			nanosleep(&(var ## ts),NULL); \
		} \
		fflush(stdout); \
		MPI_Barrier(comm); \
	}

#elif defined(HITPAR)


#define ALL_SEQUENTIAL_BEGIN(var) \
	struct timespec var ## ts = {0,1000000}; \
	fflush(stdout); \
	int var = 0; \
	for(var=-1;var<hit_NProcs+1;var++){ \
		if(hit_Rank == var){

#define ALL_SEQUENTIAL_END(var) \
		} else { \
			nanosleep(&(var ## ts),NULL); \
		} \
		fflush(stdout); \
		MPI_Barrier(MPI_COMM_WORLD); \
	}


#endif
#endif




#endif // _SW_COMMON_
