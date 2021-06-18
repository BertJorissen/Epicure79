/*
* seqJacobi2D.c
* 	Stencil code: Jacobi 2D for the heat equation. Implemented as a 2D cellular automata. 
* 	Sequential basic code
*
* v1.0
* (c) 2007-2015, Arturo Gonzalez-Escribano
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

int main(int argc, char *argv[]) {
	if (argc < 4 ) {
		fprintf( stderr, "Usage: %s <rows> <columns> <numIter>\n", argv[0] );
		exit( EXIT_FAILURE );
	}
	int ROWS = atoi( argv[1] );
	int COLS = atoi( argv[2] );
	int STAGES = atoi( argv[3] );

	double mat[ROWS][COLS];
	double copy[ROWS][COLS];

	int i,j;

	// Init
	for ( i=0; i<ROWS; i++ )
		for ( j=0; j<COLS; j++ )
			mat[i][j] = 0.0;

	for ( i=0; i<ROWS; i++ ) {
		mat[i][0] = 3;
		mat[i][COLS-1] = 4;
	}

	for ( j=0; j<COLS; j++ ) {
		mat[0][j] = 1;
		mat[ROWS-1][j] = 2;
	}
	
	// Compute
	int stage;
	for ( stage=0; stage<STAGES; stage++ ) {
		// Update copy
		for ( i=0; i<ROWS; i++ )
			for ( j=0; j<COLS; j++ )
				copy[i][j] = mat[i][j];
		
		// Compute iteration
		for ( i=1; i<ROWS-1; i++ )
			for ( j=1; j<COLS-1; j++ )
				mat[i][j] = ( copy[i-1][j] + copy[i+1][j] + copy[i][j-1] + copy[i][j+1] ) / 4;
	}

	// Write
	for ( i=0; i<ROWS; i++ )
		for ( j=0; j<COLS; j++ )
			printf("%014.4lf\n", mat[i][j] );

	// End	
	return 0;
}
