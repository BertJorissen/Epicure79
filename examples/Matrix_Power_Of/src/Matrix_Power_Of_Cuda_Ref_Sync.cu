/**
 * @file Matrix_Power_Of_Cuda_Ref_Sync.cu
 * @author Trasgo Group
 * @brief MatrixPow: Synchronous native CUDA version
 * @version 4.0
 * @date 2021-07-31
 *
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
 */

// System includes
#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>

#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

#define SEED    6834723
#define EPSILON 0.0001

double main_clock;
double exec_clock;

#ifdef DEBUG
#define CUDA_CHECK()                                                      \
	{                                                                     \
		cudaError_t error;                                                \
		if ((error = cudaGetLastError()) != cudaSuccess) {                \
			fprintf(stdout, "%s::%d ERROR: %s: %s\n", __FILE__, __LINE__, \
					cudaGetErrorName(error), cudaGetErrorString(error));  \
			exit(EXIT_FAILURE);                                           \
		}                                                                 \
	}
#else
#define CUDA_CHECK()
#endif

/*
 * function name: gpu_square_matrix_mult
 *
 * source: https://github.com/lzhengchun/matrix-cuda/blob/master/matrix_cuda.cu
 */
__global__ void matrixMulCUDA(float *d_c, float *d_a, float *d_b, int n) {
	__shared__ float tile_a[BLOCKSIZE][BLOCKSIZE];
	__shared__ float tile_b[BLOCKSIZE][BLOCKSIZE];

	int   row = blockIdx.y * BLOCKSIZE + threadIdx.y;
	int   col = blockIdx.x * BLOCKSIZE + threadIdx.x;
	float tmp = 0.0;
	int   idx;

	for (int sub = 0; sub < gridDim.x; ++sub) {
		idx = row * n + sub * BLOCKSIZE + threadIdx.x;
		if (idx >= n * n) {
			// n may not divisible by BLOCKSIZE
			tile_a[threadIdx.y][threadIdx.x] = 0;
		} else {
			tile_a[threadIdx.y][threadIdx.x] = d_a[idx];
		}

		idx = (sub * BLOCKSIZE + threadIdx.y) * n + col;
		if (idx >= n * n) {
			tile_b[threadIdx.y][threadIdx.x] = 0;
		} else {
			tile_b[threadIdx.y][threadIdx.x] = d_b[idx];
		}
		__syncthreads();

		for (int k = 0; k < BLOCKSIZE; ++k) {
			tmp += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
		}
		__syncthreads();
	}
	if (row < n && col < n) {
		d_c[row * n + col] = tmp;
	}
}

void Init_Tiles(float *matrix_a, float *matrix_b, float *matrix_c, int rows, int columns) {
	srand(SEED);
	for (int j = 0; j < columns; j++) {
		float col_sum_a = 0;
		for (int i = 0; i < rows; i++) {
			// generate random floats in a way matrixes don't turn into NaN
			float min    = -(1 - col_sum_a) + EPSILON;
			float max    = 1 - col_sum_a - EPSILON;
			float random = ((float)rand()) / (float)RAND_MAX;
			float range  = max - min;
			float value  = (random * range) + min;

			matrix_a[i * columns + j] = value;
			matrix_b[i * columns + j] = value;
			matrix_c[i * columns + j] = 0;
			col_sum_a += fabsf(value);
		}
	}
}

void Host_Compute(int ITER, double *p_sum, double *p_res, float *matrix, float *matrix_res, int rows, int columns) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host task");
	#endif //_PROFILING_ENABLED_

	double minimum = matrix[0 * columns + 0];
	double maximum = matrix[0 * columns + 0];

	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			if (minimum > matrix[j * columns + k]) {
				minimum = matrix[j * columns + k];
			}
			if (maximum < matrix[j * columns + k]) {
				maximum = matrix[j * columns + k];
			}
		}
	}

	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			matrix[j * columns + k] = matrix[j * columns + k] - minimum;
			matrix[j * columns + k] = matrix[j * columns + k] / maximum;
		}
	}

	p_sum[ITER] = 0;
	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			p_sum[ITER] += pow(matrix[j * columns + k], 2);
		}
	}
	p_res[ITER] = sqrt(p_sum[ITER]);

	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			matrix_res[j * columns + k] = matrix[j * columns + k] / p_res[ITER];
		}
	}
	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	// Simpler interface for square matrices (only one parameter)
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <matrix_size> <n_power> <device>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int    SIZE        = atoi(argv[1]);
	int    POWER       = atoi(argv[2]);
	int    GPU         = atoi(argv[3]);
	size_t MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	// Extra information for collecting results
	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, GPU);
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, ", cu_dev_prop.name);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", POWER);
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n POLICY SYNC");
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	double *p_res = (double *)malloc(POWER * sizeof(double));
	double *p_sum = (double *)malloc(POWER * sizeof(double));

	cudaSetDevice(GPU);
	CUDA_CHECK();

	dim3 dimsA;
	dim3 dimsB;

	dimsA.x = dimsA.y = dimsB.x = dimsB.y = SIZE;

	dim3 threads(BLOCKSIZE, BLOCKSIZE);
	dim3 grid((dimsB.x + BLOCKSIZE - 1) / threads.x,
			  (dimsA.y + BLOCKSIZE - 1) / threads.y);

	/*Variables*/
	float *A, *B, *C;
	cudaMallocHost((void **)&A, MATRIX_SIZE);
	cudaMallocHost((void **)&B, MATRIX_SIZE);
	cudaMallocHost((void **)&C, MATRIX_SIZE);
	CUDA_CHECK();
	float *matrix_tmp = (float *)malloc(MATRIX_SIZE);

	/*Device variables*/
	float *d_A, *d_B, *d_C;
	cudaMalloc(&d_A, MATRIX_SIZE);
	cudaMalloc(&d_B, MATRIX_SIZE);
	cudaMalloc(&d_C, MATRIX_SIZE);
	CUDA_CHECK();

	/*Inicializa variables*/
	Init_Tiles(A, B, C, SIZE, SIZE);

	/*Start timer*/
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	/*Copy to the device*/
	cudaMemcpy(d_A, A, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, B, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, C, MATRIX_SIZE, cudaMemcpyHostToDevice);

	for (int i = 0; i < POWER; i++) {
		float *matrix1;
		float *dmatrix1;
		float *dmatrix2;
		if ((i % 2) == 0) {
			matrix1  = C;
			dmatrix1 = d_C;
			dmatrix2 = d_B;
		} else {
			matrix1  = B;
			dmatrix1 = d_B;
			dmatrix2 = d_C;
		}
		matrixMulCUDA<<<grid, threads>>>(dmatrix1, d_A, dmatrix2, SIZE);
		CUDA_CHECK();
		cudaMemcpy(matrix1, dmatrix1, MATRIX_SIZE, cudaMemcpyDeviceToHost);
		CUDA_CHECK();
		Host_Compute(i, p_sum, p_res, matrix1, matrix_tmp, SIZE, SIZE);
	}
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	/* Stop timer */

	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
	CUDA_CHECK();

	/* PRINT RESULTS */
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", p_sum[POWER - 1], p_res[POWER - 1]);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- NORM ----------------------- \n\n");
	for (int i = 0; i < POWER; i++) {
		printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]);
	}
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	cudaFreeHost(A);
	cudaFreeHost(B);
	cudaFreeHost(C);
	CUDA_CHECK();

	free(p_sum);
	free(p_res);
	free(matrix_tmp);

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n");
	printf("Clock main: %lf\n", main_clock);
	printf("Clock exec: %lf\n", exec_clock);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	return EXIT_SUCCESS;
}
