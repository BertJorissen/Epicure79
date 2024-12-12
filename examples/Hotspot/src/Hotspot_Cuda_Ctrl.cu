/**
 * @file Hotspot_Cuda_Ctrl.cu
 * @brief Hotspot: Ctrl CUDA version
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
#include "Ctrl.h"
#include "Hotspot_Constants.h"
#include "../../examples/Utils/ctrl_print_info.h"
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>

double main_clock;
double exec_clock;

int grid_rows;
int grid_cols;

Ctrl_NewType(float);

CTRL_KERNEL_CHAR(Hotspot, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);

#define hotspot_params 11, INVAL, int, iteration, IN, HitTile_float, power, IN, HitTile_float, temp_src, OUT, HitTile_float, temp_dst, INVAL, int, border_cols, INVAL, int, border_rows, INVAL, float, Cap, INVAL, float, Rx, INVAL, float, Ry, INVAL, float, Rz, INVAL, float, step
#define init_params    2, OUT, HitTile_float, matrix_temp, OUT, HitTile_float, matrix_power
#define compute_params 2, INVAL, HitTile_float, matrix_dst, IN, HitTile_float, matrix_src

CTRL_KERNEL(Hotspot, CUDA, DEFAULT, CTRL_KPARAMS(hotspot_params), {
	__shared__ float temp_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float power_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float temp_t[BLOCKSIZE_1][BLOCKSIZE_0]; // saving temparary temperature result

	float amb_temp = 80.0;
	float step_div_Cap;
	float Rx_1;
	float Ry_1;
	float Rz_1;

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
	int loadYidx = yidx;
	int loadXidx = xidx;
	int index    = hit_tileDimCard(power, 1) * loadYidx + loadXidx;

	if (IN_RANGE(loadYidx, 0, hit_tileDimCard(power, 0) - 1) && IN_RANGE(loadXidx, 0, hit_tileDimCard(power, 1) - 1)) {
		temp_on_cuda[ty][tx]  = hit(temp_src, index); // Load the temperature data from global memory to shared memory
		power_on_cuda[ty][tx] = hit(power, index);    // Load the power data from global memory to shared memory
	}
	__syncthreads();

	// effective range within this block that falls within
	// the valid range of the input data
	// used to rule out computation outside the boundary.
	int validYmin = (blkY < 0) ? -blkY : 0;
	int validYmax = (blkYmax > hit_tileDimCard(power, 0) - 1) ? BLOCKSIZE_1 - 1 - (blkYmax - hit_tileDimCard(power, 0) + 1) : BLOCKSIZE_1 - 1;
	int validXmin = (blkX < 0) ? -blkX : 0;
	int validXmax = (blkXmax > hit_tileDimCard(power, 1) - 1) ? BLOCKSIZE_0 - 1 - (blkXmax - hit_tileDimCard(power, 1) + 1) : BLOCKSIZE_0 - 1;

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
		hit(temp_dst, index) = temp_t[ty][tx];
	}
});

CTRL_HOST_TASK(Init_Tiles, CTRL_HPARAMS(init_params)) {
	srand(SEED);
	for (int i = 0; i < grid_rows; i++) {
		for (int j = 0; j < grid_cols; j++) {
			hit(matrix_temp, i, j) = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
	for (int i = 0; i < grid_rows; i++) {
		for (int j = 0; j < grid_cols; j++) {
			hit(matrix_power, i, j) = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
}

CTRL_HOST_TASK(Host_Compute, CTRL_HPARAMS(compute_params)) {
	for (int i = 0; i < grid_rows * grid_cols; i++) {
		hit(matrix_dst, i) = hit_as(matrix_src, matrix_dst, i);
	}
}

CTRL_KERNEL_PROTO(Hotspot, 1, CUDA, DEFAULT, hotspot_params);
CTRL_HOST_TASK_PROTO(Init_Tiles, init_params);
CTRL_HOST_TASK_PROTO(Host_Compute, compute_params);

// Extra HostTask added for error checking
#define norm_params 1, INVAL, HitTile_float, matrix
CTRL_HOST_TASK(Norm_Calc, CTRL_HPARAMS(norm_params)) {
	double resultado = 0;
	double suma      = 0;

	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			suma += pow(hit(matrix, i, j), 2);
		}
	}

	resultado = sqrt(suma);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", suma, resultado);
	#else
	printf("\n ----------------------- NORM ----------------------- \n\n");
	printf(" Sum: %lf \n", suma);
	printf(" Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);
}

CTRL_HOST_TASK_PROTO(Norm_Calc, norm_params);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	if (argc != 7) {
		fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy> <policy> <config_file>\n", argv[0]);
		fprintf(stderr, "\t<grid_rows/grid_cols> - number of rows/cols in the grid (positive integer)\n");
		fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
		fprintf(stderr, "\t<sim_time> - number of iterations\n");
		fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
		fprintf(stderr, "\t<policy> - 0 for sync or 1 for async\n");
		fprintf(stderr, "\t<config_file> - path to ctrl config file\n");
		exit(EXIT_FAILURE);
	}

	grid_rows                    = atoi(argv[1]);
	grid_cols                    = atoi(argv[1]);
	int         pyramid_height   = atoi(argv[2]);
	int         total_iterations = atoi(argv[3]);
	int         iters_per_copy   = atoi(argv[4]);
	Ctrl_Policy policy           = (Ctrl_Policy)atoi(argv[5]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[6];

	int borderCols    = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows    = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = BLOCKSIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = BLOCKSIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols     = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows     = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	float grid_height = chip_height / grid_rows;
	float grid_width  = chip_width / grid_cols;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx  = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry  = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz  = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step      = PRECISION / max_slope / 1000.0;

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, BLOCKSIZE_1 * blockRows, BLOCKSIZE_0 * blockCols);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		// Extra information for collecting results
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
		printf("\n PYRAMID HEIGHT: %d", pyramid_height);
		printf("\n N_ITER: %d", total_iterations);
		printf("\n ITERS_PER_COPY: %d", iters_per_copy);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		Ctrl_PrintInfo();
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		printf("Threads: %d, %d\n", BLOCKSIZE_0 * blockRows, BLOCKSIZE_1 * blockCols);
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		HitTile_float MatrixTemp[2], MatrixPower;
		HitShape      shape      = hitShapeSize(grid_rows, grid_cols);
		MatrixTemp[0]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixTemp[1]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixPower              = Ctrl_DomainAlloc(ctrl, float, shape);
		HitTile_float MatrixCopy = hitTile(float, shape);

		Ctrl_HostTask(Init_Tiles, MatrixTemp[0], MatrixPower);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		int real_iter = 1;
		int src       = 1;
		int dst       = 0;

		for (int i = 0; i < total_iterations; i += pyramid_height) {
			int temp = src;
			src      = dst;
			dst      = temp;

			int aux_iterations = MIN(pyramid_height, total_iterations - i);
			Ctrl_Launch(ctrl, Hotspot, threads, CTRL_THREAD_NULL, aux_iterations, MatrixPower, MatrixTemp[src], MatrixTemp[dst],
						borderCols, borderRows, Cap, Rx, Ry, Rz, step);

			if ((real_iter % iters_per_copy) == 0) {
				Ctrl_HostTask(Host_Compute, MatrixCopy, MatrixTemp[dst]);
			}
			real_iter++;
		}

		Ctrl_HostTask(Host_Compute, MatrixCopy, MatrixTemp[dst]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		// Extra HostTask added for error checking
		Ctrl_HostTask(Norm_Calc, MatrixCopy);
		Ctrl_Synchronize();

		Ctrl_Free(ctrl, MatrixTemp[0], MatrixTemp[1], MatrixPower);
		hit_tileFree(MatrixCopy);
		Ctrl_EndBlock();
	}

	Ctrl_Finalize();
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
