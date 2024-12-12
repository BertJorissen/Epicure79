/**
 * @file Hotspot_Cuda_Ref_Async.cu
 * @brief Hotspot: Asynchronous native CUDA version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 *
 * @copyright This file is part of a modified version of a Rodinia benchmark. Thus the following applies:
 * @copyright Copyright (c)2008-2014 University of Virginia. All rights reserved.
 *
 * @copyright Redistribution and use in source and binary forms, with or without modification, are permitted
 * without royalty fees or other restrictions, provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Virginia, the Dept. of Computer Science,
 *    nor the names of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE UNIVERSITY OF VIRGINIA OR THE SOFTWARE AUTHORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <omp.h>
#include <stdbool.h>

#include "Hotspot_Constants.h"
#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

typedef struct Hotspot_Compute_Args {
	float *dst, *src;
	int    rows, cols;
} Hotspot_Compute_Args_t;

#define STREAM_KERNEL 0
#define STREAM_CPY    1
#define STREAM_HOST   2

/* define timer macros */
double main_clock;
double exec_clock;

void init_matrix(float *matrix_temp, float *matrix_power, int rows, int cols) {
	srand(SEED);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_temp[i * cols + j] = (-1 + (2 * (((float)rand()) / (float)RAND_MAX)));
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_power[i * cols + j] = (-1 + (2 * (((float)rand()) / (float)RAND_MAX)));
		}
	}
}

void host_compute(void *args) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host task");
	#endif //_PROFILING_ENABLED_

	Hotspot_Compute_Args_t *tmp = (Hotspot_Compute_Args_t *)args;
	for (int i = 0; i < tmp->rows * tmp->cols; i++) {
		tmp->dst[i] = tmp->src[i];
	}

	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

void calc_norm(float *matrix, int rows, int cols) {
	double resultado = 0;
	double suma      = 0;

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

__global__ void calculate_temp(int    iteration,   // number of iterations
							   float *power,       // power input
							   float *temp_src,    // temperature input/output
							   float *temp_dst,    // temperature input/output
							   int    grid_cols,   // Col of grid
							   int    grid_rows,   // Row of grid
							   int    border_cols, // border offset
							   int    border_rows, // border offset
							   float  Cap,         // Capacitance
							   float Rx, float Ry, float Rz, float step) {

	__shared__ float temp_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float power_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float temp_t[BLOCKSIZE_1][BLOCKSIZE_0]; // saving temparary temperature result

	float amb_temp = 80.0;
	float step_div_Cap;
	float Rx_1, Ry_1, Rz_1;

	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	step_div_Cap = step / Cap;

	Rx_1 = 1 / Rx;
	Ry_1 = 1 / Ry;
	Rz_1 = 1 / Rz;

	// each block finally computes result for a small block
	// after N iterations.
	// it is the non-overlapping small blocks that cover
	// all the input data

	// calculate the small block size
	int small_block_rows = BLOCKSIZE_0 - iteration * 2; // EXPAND_RATE
	int small_block_cols = BLOCKSIZE_1 - iteration * 2; // EXPAND_RATE

	// calculate the boundary for the block according to
	// the boundary of its small block
	int blkY    = small_block_rows * by - border_rows;
	int blkX    = small_block_cols * bx - border_cols;
	int blkYmax = blkY + BLOCKSIZE_0 - 1;
	int blkXmax = blkX + BLOCKSIZE_1 - 1;

	// calculate the global thread coordination
	int yidx = blkY + ty;
	int xidx = blkX + tx;

	// load data if it is within the valid input range
	int loadYidx = yidx, loadXidx = xidx;
	int index = grid_cols * loadYidx + loadXidx;

	if (IN_RANGE(loadYidx, 0, grid_rows - 1) && IN_RANGE(loadXidx, 0, grid_cols - 1)) {
		temp_on_cuda[ty][tx]  = temp_src[index]; // Load the temperature data from global memory to shared memory
		power_on_cuda[ty][tx] = power[index];    // Load the power data from global memory to shared memory
	}
	__syncthreads();

	// effective range within this block that falls within
	// the valid range of the input data
	// used to rule out computation outside the boundary.
	int validYmin = (blkY < 0) ? -blkY : 0;
	int validYmax = (blkYmax > grid_rows - 1)
						? BLOCKSIZE_1 - 1 - (blkYmax - grid_rows + 1)
						: BLOCKSIZE_1 - 1;
	int validXmin = (blkX < 0) ? -blkX : 0;
	int validXmax = (blkXmax > grid_cols - 1)
						? BLOCKSIZE_0 - 1 - (blkXmax - grid_cols + 1)
						: BLOCKSIZE_0 - 1;

	int N = ty - 1;
	int S = ty + 1;
	int W = tx - 1;
	int E = tx + 1;

	N = (N < validYmin) ? validYmin : N;
	S = (S > validYmax) ? validYmax : S;
	W = (W < validXmin) ? validXmin : W;
	E = (E > validXmax) ? validXmax : E;

	bool computed;
	for (int i = 0; i < iteration; i++) {
		computed = false;
		if (IN_RANGE(tx, i + 1, BLOCKSIZE_1 - i - 2) && IN_RANGE(ty, i + 1, BLOCKSIZE_0 - i - 2) &&
			IN_RANGE(tx, validXmin, validXmax) && IN_RANGE(ty, validYmin, validYmax)) {
			computed       = true;
			temp_t[ty][tx] = temp_on_cuda[ty][tx] + step_div_Cap * (power_on_cuda[ty][tx] +
																	(temp_on_cuda[S][tx] + temp_on_cuda[N][tx] - 2.0 * temp_on_cuda[ty][tx]) * Ry_1 +
																	(temp_on_cuda[ty][E] + temp_on_cuda[ty][W] - 2.0 * temp_on_cuda[ty][tx]) * Rx_1 +
																	(amb_temp - temp_on_cuda[ty][tx]) * Rz_1);
		}
		__syncthreads();
		if (i == iteration - 1) {
			break;
		}
		if (computed) { // Assign the computation range
			temp_on_cuda[ty][tx] = temp_t[ty][tx];
		}
		__syncthreads();
	}

	// update the global memory
	// after the last iteration, only threads coordinated within the
	// small block perform the calculation and switch on ``computed''
	if (computed) {
		temp_dst[index] = temp_t[ty][tx];
	}
}

cudaStream_t kernel_stream, dtoh_stream, host_stream;
/*
   compute N time steps
*/
int compute_tran_temp(float *MatrixPower, float *MatrixTemp[2], int col, int row,
					  int total_iterations, int num_iterations,
					  int blockCols, int blockRows, int borderCols, int borderRows,
					  int iters_per_copy, float *FilesavingTemp[2], float *MatrixCopy,
					  cudaStream_t streams[3], cudaEvent_t MatrixTemp_event[2],
					  cudaEvent_t FilesavingTemp_event[2], Hotspot_Compute_Args_t *args) {
	dim3 dimBlock(BLOCKSIZE_0, BLOCKSIZE_1);
	dim3 dimGrid(blockCols, blockRows);

	float grid_height = chip_height / row;
	float grid_width  = chip_width / col;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx  = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry  = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz  = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step      = PRECISION / max_slope / 1000.0;

	cudaStreamWaitEvent(streams[STREAM_KERNEL], MatrixTemp_event[0], 0);

	int real_iter = 1;
	int src       = 1;
	int dst       = 0;

	for (int t = 0; t < total_iterations; t += num_iterations) {
		int temp = src;
		src      = dst;
		dst      = temp;

		cudaStreamWaitEvent(streams[STREAM_KERNEL], MatrixTemp_event[dst], 0);
		calculate_temp<<<dimGrid, dimBlock, 0, streams[STREAM_KERNEL]>>>(
			MIN(num_iterations, total_iterations - t), MatrixPower,
			MatrixTemp[src], MatrixTemp[dst], col, row, borderCols, borderRows,
			Cap, Rx, Ry, Rz, step);
		cudaEventRecord(MatrixTemp_event[dst], streams[STREAM_KERNEL]);

		if ((real_iter % iters_per_copy) == 0) {
			cudaStreamWaitEvent(streams[STREAM_CPY], MatrixTemp_event[dst], 0);
			cudaStreamWaitEvent(streams[STREAM_CPY], FilesavingTemp_event[dst], 0);
			cudaMemcpyAsync(FilesavingTemp[dst], MatrixTemp[dst], sizeof(float) * row * col,
							cudaMemcpyDeviceToHost, streams[STREAM_CPY]);
			cudaEventRecord(MatrixTemp_event[dst], streams[STREAM_CPY]);
			cudaEventRecord(FilesavingTemp_event[dst], streams[STREAM_CPY]);

			cudaStreamWaitEvent(streams[STREAM_HOST], FilesavingTemp_event[dst], 0);
			cudaLaunchHostFunc(streams[STREAM_HOST], host_compute, (void *)&(args[dst]));
			cudaEventRecord(FilesavingTemp_event[dst], streams[STREAM_HOST]);
		}
		real_iter++;
	}

	return dst;
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	if (argc != 6) {
		fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy> <device>\n", argv[0]);
		fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
		fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
		fprintf(stderr, "\t<sim_time>   - number of iterations\n");
		fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
		fprintf(stderr, "\t<device> - GPU index\n");
		exit(EXIT_FAILURE);
	}
	int grid_rows        = atoi(argv[1]);
	int grid_cols        = atoi(argv[1]);
	int pyramid_height   = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy   = atoi(argv[4]);
	int device           = atoi(argv[5]);
	cudaSetDevice(device);

	/* --------------- pyramid parameters --------------- */

	int borderCols    = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows    = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = BLOCKSIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = BLOCKSIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols     = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows     = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	int size = grid_rows * grid_cols;

	float *FilesavingTemp[2];
	float *FilesavingPower;
	float *MatrixCopy;

	cudaMallocHost((void **)&FilesavingTemp[0], size * sizeof(float));
	cudaMallocHost((void **)&FilesavingTemp[1], size * sizeof(float));
	cudaMallocHost((void **)&FilesavingPower, size * sizeof(float));
	MatrixCopy = (float *)malloc(size * sizeof(float));

	if (!FilesavingPower || !FilesavingTemp[0] || !FilesavingTemp[1] || !MatrixCopy) {
		fprintf(stderr, "unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	// Extra information for collecting results
	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, device);
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("CUDA-%s, ", cu_dev_prop.name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
	printf("\n PYRAMID HEIGHT: %d", pyramid_height);
	printf("\n N_ITER: %d", total_iterations);
	printf("\n ITERS_PER_COPY: %d", iters_per_copy);
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n POLICY ASYNC");
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif

	init_matrix(FilesavingTemp[0], FilesavingPower, grid_rows, grid_cols);

	float *MatrixTemp[2];
	float *MatrixPower;
	cudaMalloc((void **)&MatrixTemp[0], sizeof(float) * size);
	cudaMalloc((void **)&MatrixTemp[1], sizeof(float) * size);
	cudaMalloc((void **)&MatrixPower, sizeof(float) * size);

	cudaStream_t streams[3];

	cudaStreamCreate(&streams[STREAM_KERNEL]);
	cudaStreamCreate(&streams[STREAM_CPY]);
	cudaStreamCreate(&streams[STREAM_HOST]);

	cudaEvent_t MatrixTemp_event[2];
	cudaEvent_t FilesavingTemp_event[2];

	cudaEventCreateWithFlags(&MatrixTemp_event[0], cudaEventDisableTiming);
	cudaEventCreateWithFlags(&MatrixTemp_event[1], cudaEventDisableTiming);
	cudaEventCreateWithFlags(&FilesavingTemp_event[0], cudaEventDisableTiming);
	cudaEventCreateWithFlags(&FilesavingTemp_event[1], cudaEventDisableTiming);

	Hotspot_Compute_Args_t args[2] = {
		(Hotspot_Compute_Args_t){.dst  = MatrixCopy,
								 .src  = FilesavingTemp[0],
								 .rows = grid_rows,
								 .cols = grid_cols},
		(Hotspot_Compute_Args_t){.dst  = MatrixCopy,
								 .src  = FilesavingTemp[1],
								 .rows = grid_rows,
								 .cols = grid_cols}};

	cudaLaunchHostFunc(streams[STREAM_HOST], host_compute, (void *)&(args));

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	cudaMemcpyAsync(MatrixPower, FilesavingPower, sizeof(float) * size,
					cudaMemcpyHostToDevice, streams[STREAM_CPY]);
	cudaMemcpyAsync(MatrixTemp[0], FilesavingTemp[0], sizeof(float) * size,
					cudaMemcpyHostToDevice, streams[STREAM_CPY]);
	cudaEventRecord(MatrixTemp_event[0], streams[STREAM_CPY]);

	int ret = compute_tran_temp(
		MatrixPower, MatrixTemp, grid_cols, grid_rows, total_iterations,
		pyramid_height, blockCols, blockRows, borderCols, borderRows,
		iters_per_copy, FilesavingTemp, MatrixCopy, streams, MatrixTemp_event,
		FilesavingTemp_event, args);

	cudaStreamWaitEvent(streams[STREAM_CPY], FilesavingTemp_event[ret], 0);
	cudaMemcpyAsync(FilesavingTemp[ret], MatrixTemp[ret], sizeof(float) * size,
					cudaMemcpyDeviceToHost, streams[STREAM_CPY]);
	cudaEventRecord(FilesavingTemp_event[ret], streams[STREAM_CPY]);

	cudaStreamWaitEvent(streams[STREAM_HOST], FilesavingTemp_event[ret], 0);
	cudaLaunchHostFunc(streams[STREAM_HOST], host_compute, (void *)&(args[ret]));

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	calc_norm(MatrixCopy, grid_rows, grid_cols);

	cudaEventDestroy(MatrixTemp_event[0]);
	cudaEventDestroy(MatrixTemp_event[1]);
	cudaEventDestroy(FilesavingTemp_event[0]);
	cudaEventDestroy(FilesavingTemp_event[1]);

	cudaStreamDestroy(streams[STREAM_KERNEL]);
	cudaStreamDestroy(streams[STREAM_CPY]);
	cudaStreamDestroy(streams[STREAM_HOST]);

	cudaFree(MatrixPower);
	cudaFree(MatrixTemp[0]);
	cudaFree(MatrixTemp[1]);
	cudaFreeHost(FilesavingTemp[0]);
	cudaFreeHost(FilesavingTemp[1]);
	cudaFreeHost(FilesavingPower);
	free(MatrixCopy);

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	return EXIT_SUCCESS;
}
