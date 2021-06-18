/*
* seqJacobi1D.c
* 	Stencil code: Jacobi 1D for the heat equation. Implemented as a 1D cellular automata. 
* 	Sequential basic code
*
* v1.0
* (c) 2019, Arturo Gonzalez-Escribano
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
	if (argc < 3 ) {
		fprintf( stderr, "Usage: %s <size> <numIter>\n", argv[0] );
		exit( EXIT_FAILURE );
	}
	int SIZE = atoi( argv[1] );
	int STAGES = atoi( argv[2] );

	double *mat = (double *)malloc( (size_t)SIZE * sizeof(double) );
	double *copy = (double *)malloc( (size_t)SIZE * sizeof(double) );

	int i;

	// Init
	for ( i=1; i<SIZE-1; i++ ) mat[i] = 0.0;
	// Arbitrary fixed values on the extreme positions
	mat[0] = 2;
	mat[SIZE-1] = 3;
	
	// Compute
	int stage;
	for ( stage=0; stage<STAGES; stage++ ) {
		// Update copy
		for ( i=0; i<SIZE; i++ ) copy[i] = mat[i];
		
		// Compute iteration
		for ( i=1; i<SIZE-1; i++ )
			mat[i] = ( copy[i-1] + copy[i+1] ) / 2;
	}

	// Write
	for ( i=0; i<SIZE; i++ ) printf("%014.4lf\n", mat[i] );

	// End	
	return 0;
}
