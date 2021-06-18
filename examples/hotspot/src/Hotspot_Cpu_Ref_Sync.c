/*
LICENSE TERMS

Copyright (c)2008-2014 University of Virginia
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted without royalty fees or other restrictions, provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the University of Virginia, the Dept. of Computer Science, nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF VIRGINIA OR THE SOFTWARE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include "Hotspot_Constants.h"
#define ANY_SIZE

// Returns the current system time in microseconds 
long long get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000) + tv.tv_usec;
}

#define BLOCK_SIZE_C BLOCKSIZE_0
#define BLOCK_SIZE_R BLOCKSIZE_1

#define OPEN
//#define NUM_THREAD 4

#ifdef OMP_OFFLOAD
#pragma offload_attribute(push, target(mic))
#endif

int num_omp_threads;

/* Single iteration of the transient solver in the grid model.
 * advances the solution of the discretized difference equations 
 * by one time step
 */
void single_iteration(float *result, float *temp, float *power, int row, int col,
					  float Cap_1, float Rx_1, float Ry_1, float Rz_1, 
					  float step) {
	float delta;
	int r, c;
	int chunk;
	#ifdef ANY_SIZE
	int chunks_in_row = (col+BLOCK_SIZE_C-1)/BLOCK_SIZE_C;
	int chunks_in_col = (row+BLOCK_SIZE_R-1)/BLOCK_SIZE_R;
	int num_chunk = chunks_in_row * chunks_in_col;
	#else //ANY_SIZE
	int num_chunk = row*col / (BLOCK_SIZE_R * BLOCK_SIZE_C);
	int chunks_in_row = col/BLOCK_SIZE_C;
	int chunks_in_col = row/BLOCK_SIZE_R;
	#endif //ANY_SIZE

#ifdef OPEN
	#ifndef __MIC__
	omp_set_num_threads(num_omp_threads);
	#endif
	#pragma omp parallel for shared(power, temp, result) private(chunk, r, c, delta) firstprivate(row, col, num_chunk, chunks_in_row) schedule(static)
#endif
	for ( chunk = 0; chunk < num_chunk; ++chunk ) {
		int r_start = BLOCK_SIZE_R*(chunk/chunks_in_col);
		int c_start = BLOCK_SIZE_C*(chunk%chunks_in_row); 
		int r_end = r_start + BLOCK_SIZE_R > row ? row : r_start + BLOCK_SIZE_R;
		int c_end = c_start + BLOCK_SIZE_C > col ? col : c_start + BLOCK_SIZE_C;
	   
		if ( r_start == 0 || c_start == 0 || r_end == row || c_end == col ) {
		#ifdef ANY_SIZE
			for ( r = r_start; r < r_end; ++r ) {
				for ( c = c_start; c < c_end; ++c ) {
		#else //ANY_SIZE
			for ( r = r_start; r < r_start + BLOCK_SIZE_R; ++r ) {
				for ( c = c_start; c < c_start + BLOCK_SIZE_C; ++c ) {
		#endif //ANY_SIZE
					/* Corner 1 */
					if ( (r == 0) && (c == 0) ) {
						delta = (Cap_1) * (power[0] +
							(temp[1] - temp[0]) * Rx_1 +
							(temp[col] - temp[0]) * Ry_1 +
							(amb_temp - temp[0]) * Rz_1);
					}	/* Corner 2 */
					else if ((r == 0) && (c == col-1)) {
						delta = (Cap_1) * (power[c] +
							(temp[c-1] - temp[c]) * Rx_1 +
							(temp[c+col] - temp[c]) * Ry_1 +
						(   amb_temp - temp[c]) * Rz_1);
					}	/* Corner 3 */
					else if ((r == row-1) && (c == col-1)) {
						delta = (Cap_1) * (power[r*col+c] + 
							(temp[r*col+c-1] - temp[r*col+c]) * Rx_1 + 
							(temp[(r-1)*col+c] - temp[r*col+c]) * Ry_1 + 
						(   amb_temp - temp[r*col+c]) * Rz_1);					
					}	/* Corner 4	*/
					else if ((r == row-1) && (c == 0)) {
						delta = (Cap_1) * (power[r*col] + 
							(temp[r*col+1] - temp[r*col]) * Rx_1 + 
							(temp[(r-1)*col] - temp[r*col]) * Ry_1 + 
							(amb_temp - temp[r*col]) * Rz_1);
					}	/* Edge 1 */
					else if (r == 0) {
						delta = (Cap_1) * (power[c] + 
							(temp[c+1] + temp[c-1] - 2.0*temp[c]) * Rx_1 + 
							(temp[col+c] - temp[c]) * Ry_1 + 
							(amb_temp - temp[c]) * Rz_1);
					}	/* Edge 2 */
					else if (c == col-1) {
						delta = (Cap_1) * (power[r*col+c] + 
							(temp[(r+1)*col+c] + temp[(r-1)*col+c] - 2.0*temp[r*col+c]) * Ry_1 + 
							(temp[r*col+c-1] - temp[r*col+c]) * Rx_1 + 
							(amb_temp - temp[r*col+c]) * Rz_1);
					}	/* Edge 3 */
					else if (r == row-1) {
						delta = (Cap_1) * (power[r*col+c] + 
							(temp[r*col+c+1] + temp[r*col+c-1] - 2.0*temp[r*col+c]) * Rx_1 + 
							(temp[(r-1)*col+c] - temp[r*col+c]) * Ry_1 + 
							(amb_temp - temp[r*col+c]) * Rz_1);
					}	/* Edge 4 */
					else if (c == 0) {
						delta = (Cap_1) * (power[r*col] + 
							(temp[(r+1)*col] + temp[(r-1)*col] - 2.0*temp[r*col]) * Ry_1 + 
							(temp[r*col+1] - temp[r*col]) * Rx_1 + 
							(amb_temp - temp[r*col]) * Rz_1);
					}
					else{
						delta = ( Cap_1 * (power[r*col+c] + 
							(temp[(r+1)*col+c] + temp[(r-1)*col+c] - 2.f*temp[r*col+c]) * Ry_1 + 
							(temp[r*col+c+1] + temp[r*col+c-1] - 2.f*temp[r*col+c]) * Rx_1 + 
							(amb_temp - temp[r*col+c]) * Rz_1));
					}
					result[r*col+c] =temp[r*col+c]+ delta;
				}
			}
			continue;
		}

		for ( r = r_start; r < r_start + BLOCK_SIZE_R; ++r ) {
#pragma omp simd        
			for ( c = c_start; c < c_start + BLOCK_SIZE_C; ++c ) {
			/* Update Temperatures */
				result[r*col+c] =temp[r*col+c]+ 
					 ( Cap_1 * (power[r*col+c] + 
					(temp[(r+1)*col+c] + temp[(r-1)*col+c] - 2.f*temp[r*col+c]) * Ry_1 + 
					(temp[r*col+c+1] + temp[r*col+c-1] - 2.f*temp[r*col+c]) * Rx_1 + 
					(amb_temp - temp[r*col+c]) * Rz_1));
			}
		}
	}
}

#ifdef OMP_OFFLOAD
#pragma offload_attribute(pop)
#endif

void host_compute(float *dst, float *src, int rows, int cols){
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			dst[i * cols + j] = src[i * cols + j];
		}
	}
}

/* Transient solver driver routine: simply converts the heat 
 * transfer differential equations to difference equations 
 * and solves the difference equations by iterating
 */
void compute_tran_temp(float *result, int num_iterations, float *temp, float *power, int row, int col, int iters_per_copy, float *MatrixCopy) {
	#ifdef VERBOSE
	int i = 0;
	#endif

	float grid_height = chip_height / row;
	float grid_width = chip_width / col;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step = PRECISION / max_slope / 1000.0;

	float Rx_1=1.f/Rx;
	float Ry_1=1.f/Ry;
	float Rz_1=1.f/Rz;
	float Cap_1 = step/Cap;
	#ifdef VERBOSE
	fprintf(stdout, "total iterations: %d s\tstep size: %g s\n", num_iterations, step);
	fprintf(stdout, "Rx: %g\tRy: %g\tRz: %g\tCap: %g\n", Rx, Ry, Rz, Cap);
	#endif

#ifdef OMP_OFFLOAD
		int array_size = row*col;
#pragma omp target \
		map(temp[0:array_size]) \
		map(to: power[0:array_size], row, col, Cap_1, Rx_1, Ry_1, Rz_1, step, num_iterations) \
		map( result[0:array_size])
#endif
		{
			float* r = result;
			float* t = temp;
			for (int i = 0; i < num_iterations ; i++) {
				#ifdef VERBOSE
				fprintf(stdout, "iteration %d\n", i);
				#endif
				single_iteration(r, t, power, row, col, Cap_1, Rx_1, Ry_1, Rz_1, step);
				if ((i % iters_per_copy) == 0) {
					host_compute(MatrixCopy, r, row, col);
				}
				float* tmp = t;
				t = r;
				r = tmp;
			}	
		}
	#ifdef VERBOSE
	fprintf(stdout, "iteration %d\n", i);
	#endif
}

void fatal(char *s) {
	fprintf(stderr, "error: %s\n", s);
	exit(1);
}

void calc_norm(float *matrix, int rows, int cols) {
	double resultado = 0;
	double suma = 0;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			suma += pow(matrix[i * cols + j], 2);
		}
	}

	resultado = sqrt(suma);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%lf, %lf, ", suma, resultado);
		fflush(stdout);
	#else
		printf("\n ----------------------- NORM ----------------------- \n\n");
		printf(" Sum: %lf \n", suma);
		printf(" Result: %lf \n", resultado);
		printf("\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif
}

void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <sim_time> <iters_per_copy> <no. of threads>\n", argv[0]);
	fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<sim_time>   - number of iterations\n");
	fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
	fprintf(stderr, "\t<no. of threads>   - number of threads\n");
	exit(1);
}

void init_matrix(float *matrix_temp, float *matrix_power, int rows, int cols) {
	srand(SEED);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_temp[i * cols + j] = (-1 + (2 * (((float)rand()) / RAND_MAX)));
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_power[i * cols + j] = (-1 + (2 * (((float)rand()) / RAND_MAX)));
		}
	}
}

int main(int argc, char **argv) {
	double main_clock = omp_get_wtime();
	int grid_rows, grid_cols, sim_time, iters_per_copy;
	float *temp, *power, *result, *MatrixCopy;
	
	/* check validity of inputs	*/
	if (argc != 5)
		usage(argc, argv);
	if ((grid_rows = atoi(argv[1])) <= 0 ||
		(grid_cols = atoi(argv[1])) <= 0 ||
		(sim_time = atoi(argv[2])) <= 0 || 
		(iters_per_copy = atoi(argv[3])) <=0 ||
		(num_omp_threads = atoi(argv[4])) <= 0
		)
		usage(argc, argv);

	/* allocate memory for the temperature and power arrays	*/
	temp = (float *) calloc (grid_rows * grid_cols, sizeof(float));
	power = (float *) calloc (grid_rows * grid_cols, sizeof(float));
	result = (float *) calloc (grid_rows * grid_cols, sizeof(float));
	MatrixCopy = (float *) calloc(grid_rows * grid_cols, sizeof(float));
	if(!temp || !power || !result || !MatrixCopy)
		fatal("unable to allocate memory");

	#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
		printf("\n N_ITER: %d", sim_time);
		printf("\n ITERS_PER_COPY: %d", iters_per_copy);
		printf("\n N_THREADS: %d", num_omp_threads);
		printf("\n POLICY SYNC");
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif //_CTRL_EXAMPLES_EXP_MODE_

	/* read initial temperatures and input power	*/
	init_matrix(temp, power, grid_rows, grid_cols);

	long long start_time = get_time();

	compute_tran_temp(result,sim_time, temp, power, grid_rows, grid_cols, iters_per_copy, MatrixCopy);
	host_compute(MatrixCopy, (1&sim_time) ? result : temp, grid_rows, grid_cols);

	long long end_time = get_time();

	calc_norm(MatrixCopy, grid_rows, grid_cols);

	/* output results	*/
	#ifdef OUTPUT
		for(int i=0; i < grid_rows; i++){
			for(int j=0; j<grid_cols;j++)
				fprintf(stdout, "%g\n", MatrixCopy[i*grid_cols+j]);
		}
	#endif
	/* cleanup	*/
	free(temp);
	free(power);
	
	main_clock = omp_get_wtime() - main_clock;
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%lf, %lf\n", main_clock, ((float) (end_time - start_time)) / (1000*1000));
		fflush(stdout);
	#else
		printf("\n ----------------------- TIME ----------------------- \n\n");
		printf(" Clock main: %lf\n", main_clock);
		printf(" Clock exec: %lf\n", ((float) (end_time - start_time)) / (1000*1000));
		printf("\n ---------------------------------------------------- \n");
	#endif

	return 0;
}