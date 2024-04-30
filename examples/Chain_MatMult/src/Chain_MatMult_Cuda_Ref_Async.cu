/**
 * @file Chain_MatMult_Cuda_Ref_Async.cu
 * @author Trasgo Group
 * @brief Chain matrix multiplication: Asynchronous native CUDA version
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

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <stdbool.h>

#include <omp.h>

#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

#define SEED 6834723

// #define DEBUG
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

#define SWAP(x, y, T)  \
	do {               \
		T SWAP = x;    \
		x      = y;    \
		y      = SWAP; \
	} while (0)

double main_clock;
double exec_clock;

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
		idx                              = row * n + sub * BLOCKSIZE + threadIdx.x;
		tile_a[threadIdx.y][threadIdx.x] = d_a[idx];
		idx                              = (sub * BLOCKSIZE + threadIdx.y) * n + col;
		tile_b[threadIdx.y][threadIdx.x] = d_b[idx];
		__syncthreads();

		for (int k = 0; k < BLOCKSIZE; ++k) {
			tmp += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
		}
		__syncthreads();
	}
	d_c[row * n + col] = tmp;
}

typedef struct InitArgs {
	float *matrix;
	int    size;
} InitArgs;

typedef struct Host_Compute_Args {
	float  *matrix;
	int     size;
	double *p_sum;
	double *p_res;
} NormArgs;

void init_matrix_rand(float *matrix, int size) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Init tile rand");
	#endif //_PROFILING_ENABLED_
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i * size + j] = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

void init_matrix_null(float *matrix, int size) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Init tile null");
	#endif //_PROFILING_ENABLED_
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i * size + j] = 0;
		}
	}
	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

void init_matrix_diag(void *args) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Init tile diag");
	#endif //_PROFILING_ENABLED_
	InitArgs *tmp = (InitArgs *)args;

	float *matrix = tmp->matrix;
	int    size   = tmp->size;
	for (int i = 0; i < size; i++) {
		matrix[i * size + i] = -1 + 2 * (float)rand() / (float)RAND_MAX;
	}
	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

void norm_calc(void *args) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Norm calc");
	#endif //_PROFILING_ENABLED_
	NormArgs *tmp = (NormArgs *)args;

	static int ITER = 0;

	double *p_sum  = tmp->p_sum;
	double *p_res  = tmp->p_res;
	float  *matrix = tmp->matrix;
	int     size   = tmp->size;
	double  suma   = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			suma += matrix[i * size + j] * matrix[i * size + j];
		}
	}
	p_sum[ITER] = suma;
	p_res[ITER] = sqrt(suma);

	ITER++;

	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

int main(int argc, char const *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 4) {
		fprintf(stderr, "\nUsage: %s <size> <n_iters> <device>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int    SIZE        = atoi(argv[1]);
	int    N_ITER      = atoi(argv[2]);
	int    GPU         = atoi(argv[3]);
	size_t MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	struct cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, GPU);
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, null, ", cu_dev_prop.name);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n POLICY %s", "Async");
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	double *p_res = (double *)malloc(N_ITER * sizeof(double));
	double *p_sum = (double *)malloc(N_ITER * sizeof(double));

	cudaSetDevice(GPU);
	CUDA_CHECK();

	dim3 dimBlock(BLOCKSIZE, BLOCKSIZE);
	dim3 dimGrid((SIZE + BLOCKSIZE - 1) / dimBlock.x,
				 (SIZE + BLOCKSIZE - 1) / dimBlock.y);

	// declare and allocate matrices
	float *h_in, *h_out;
	cudaMallocHost((void **)&h_in, MATRIX_SIZE);
	cudaMallocHost((void **)&h_out, MATRIX_SIZE);
	float *d_m1, *d_m2, *d_m3, *d_m4, *d_m5;
	cudaMalloc(&d_m1, MATRIX_SIZE);
	cudaMalloc(&d_m2, MATRIX_SIZE);
	cudaMalloc(&d_m3, MATRIX_SIZE);
	cudaMalloc(&d_m4, MATRIX_SIZE);
	cudaMalloc(&d_m5, MATRIX_SIZE);

	#ifdef _2BUF
	float *h_in_aux, *h_out_aux;
	cudaMallocHost((void **)&h_in_aux, MATRIX_SIZE);
	cudaMallocHost((void **)&h_out_aux, MATRIX_SIZE);
	float *d_m1_aux, *d_m2_aux, *d_m3_aux, *d_m4_aux, *d_m5_aux;
	cudaMalloc(&d_m1_aux, MATRIX_SIZE);
	cudaMalloc(&d_m2_aux, MATRIX_SIZE);
	cudaMalloc(&d_m3_aux, MATRIX_SIZE);
	cudaMalloc(&d_m4_aux, MATRIX_SIZE);
	cudaMalloc(&d_m5_aux, MATRIX_SIZE);
	#endif // _2BUF

	float *h_const1, *h_const2, *h_const3, *h_const4;
	float *d_const1, *d_const2, *d_const3, *d_const4;
	h_const1 = (float *)malloc(MATRIX_SIZE);
	h_const2 = (float *)malloc(MATRIX_SIZE);
	h_const3 = (float *)malloc(MATRIX_SIZE);
	h_const4 = (float *)malloc(MATRIX_SIZE);
	cudaMalloc(&d_const1, MATRIX_SIZE);
	cudaMalloc(&d_const2, MATRIX_SIZE);
	cudaMalloc(&d_const3, MATRIX_SIZE);
	cudaMalloc(&d_const4, MATRIX_SIZE);
	CUDA_CHECK();

	// create streams and events
	cudaStream_t stream_init, stream_kernel, stream_htd, stream_dth, stream_norm;
	cudaStreamCreate(&stream_init);
	cudaStreamCreate(&stream_norm);
	cudaStreamCreate(&stream_kernel);
	cudaStreamCreate(&stream_htd);
	cudaStreamCreate(&stream_dth);

	cudaEvent_t event_init, event_htd, event_kernel, event_dth, event_norm;
	cudaEventCreateWithFlags(&event_init, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_htd, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_kernel, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_dth, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_norm, cudaEventDisableTiming);

	#ifdef _2BUF
	cudaEvent_t event_init_aux, event_htd_aux, event_kernel_aux, event_dth_aux, event_norm_aux;
	cudaEventCreateWithFlags(&event_init_aux, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_htd_aux, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_kernel_aux, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_dth_aux, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_norm_aux, cudaEventDisableTiming);
	#endif // _2BUF

	NormArgs norm_calc_args = {
		.matrix = h_out,
		.size   = SIZE,
		.p_sum  = p_sum,
		.p_res  = p_res};

	InitArgs init_matrix_args = {
		.matrix = h_in,
		.size   = SIZE};

	NormArgs *p_norm_calc_args   = &norm_calc_args;
	InitArgs *p_init_matrix_args = &init_matrix_args;

	#ifdef _2BUF
	NormArgs norm_calc_args_aux = {
		.matrix = h_out_aux,
		.size   = SIZE,
		.p_sum  = p_sum,
		.p_res  = p_res};
	InitArgs init_matrix_args_aux = {
		.matrix = h_in_aux,
		.size   = SIZE};
	NormArgs *p_norm_calc_args_aux   = &norm_calc_args_aux;
	InitArgs *p_init_matrix_args_aux = &init_matrix_args_aux;
	#endif // _2BUF

	srand(SEED);
	// init const matrices in host and move to dev
	init_matrix_null(h_in, SIZE);
	#ifdef _2BUF
	init_matrix_null(h_in_aux, SIZE);
	#endif // _2BUF
	init_matrix_rand(h_const1, SIZE);
	init_matrix_rand(h_const2, SIZE);
	init_matrix_rand(h_const3, SIZE);
	init_matrix_rand(h_const4, SIZE);
	cudaMemcpy(d_const1, h_const1, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_const2, h_const2, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_const3, h_const3, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_const4, h_const4, MATRIX_SIZE, cudaMemcpyHostToDevice);

	cudaDeviceSynchronize();
	CUDA_CHECK();
	exec_clock = omp_get_wtime();

	for (int i = 0; i < N_ITER; i++) {
		// init
		cudaStreamWaitEvent(stream_init, event_htd);
		cudaLaunchHostFunc(stream_init, init_matrix_diag, p_init_matrix_args);
		cudaEventRecord(event_init, stream_init);
		CUDA_CHECK();

		// HTD
		cudaStreamWaitEvent(stream_htd, event_init);
		cudaStreamWaitEvent(stream_htd, event_kernel);
		cudaMemcpyAsync(d_m1, h_in, MATRIX_SIZE, cudaMemcpyHostToDevice, stream_htd);
		cudaEventRecord(event_htd, stream_htd);
		CUDA_CHECK();

		// Kernels
		cudaStreamWaitEvent(stream_kernel, event_htd);
		matrixMulCUDA<<<dimGrid, dimBlock, 0, stream_kernel>>>(d_m2, d_m1, d_const1, SIZE);
		matrixMulCUDA<<<dimGrid, dimBlock, 0, stream_kernel>>>(d_m3, d_m2, d_const2, SIZE);
		matrixMulCUDA<<<dimGrid, dimBlock, 0, stream_kernel>>>(d_m4, d_m3, d_const3, SIZE);
		cudaStreamWaitEvent(stream_kernel, event_dth);
		matrixMulCUDA<<<dimGrid, dimBlock, 0, stream_kernel>>>(d_m5, d_m4, d_const4, SIZE);
		cudaEventRecord(event_kernel, stream_kernel);
		CUDA_CHECK();

		// DTH
		cudaStreamWaitEvent(stream_dth, event_kernel);
		cudaStreamWaitEvent(stream_dth, event_norm);
		cudaMemcpyAsync(h_out, d_m5, MATRIX_SIZE, cudaMemcpyDeviceToHost, stream_dth);
		cudaEventRecord(event_dth, stream_dth);
		CUDA_CHECK();

		// norm calc
		cudaStreamWaitEvent(stream_norm, event_dth);
		cudaLaunchHostFunc(stream_norm, norm_calc, p_norm_calc_args);
		cudaEventRecord(event_norm, stream_norm);
		CUDA_CHECK();

		// swap stuff
		#ifdef _2BUF
		SWAP(h_in, h_in_aux, float *);
		SWAP(h_out, h_out_aux, float *);
		SWAP(d_m1, d_m1_aux, float *);
		SWAP(d_m2, d_m2_aux, float *);
		SWAP(d_m3, d_m3_aux, float *);
		SWAP(d_m4, d_m4_aux, float *);
		SWAP(d_m5, d_m5_aux, float *);
		SWAP(event_init, event_init_aux, cudaEvent_t);
		SWAP(event_htd, event_htd_aux, cudaEvent_t);
		SWAP(event_kernel, event_kernel_aux, cudaEvent_t);
		SWAP(event_dth, event_dth_aux, cudaEvent_t);
		SWAP(event_norm, event_norm_aux, cudaEvent_t);
		SWAP(p_init_matrix_args, p_init_matrix_args_aux, InitArgs *);
		SWAP(p_norm_calc_args, p_norm_calc_args_aux, NormArgs *);
		#endif // _2BUF
	}

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", p_sum[N_ITER - 1], p_res[N_ITER - 1]);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- NORM ----------------------- \n\n");
	for (int i = 0; i < N_ITER; i++) {
		printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]);
	}
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	// destroy events and streams
	cudaEventDestroy(event_init);
	cudaEventDestroy(event_htd);
	cudaEventDestroy(event_kernel);
	cudaEventDestroy(event_dth);
	cudaEventDestroy(event_norm);
	#ifdef _2BUF
	cudaEventDestroy(event_init_aux);
	cudaEventDestroy(event_htd_aux);
	cudaEventDestroy(event_kernel_aux);
	cudaEventDestroy(event_dth_aux);
	cudaEventDestroy(event_norm_aux);
	#endif // _2BUF

	// free matrices
	cudaFreeHost(h_in);
	cudaFreeHost(h_out);
	cudaFree(d_m1);
	cudaFree(d_m2);
	cudaFree(d_m3);
	cudaFree(d_m4);
	cudaFree(d_m5);
	#ifdef _2BUF
	cudaFreeHost(h_in_aux);
	cudaFreeHost(h_out_aux);
	cudaFree(d_m1_aux);
	cudaFree(d_m2_aux);
	cudaFree(d_m3_aux);
	cudaFree(d_m4_aux);
	cudaFree(d_m5_aux);
	#endif // _2BUF

	free(h_const1);
	free(h_const2);
	free(h_const3);
	free(h_const4);
	cudaFree(d_const1);
	cudaFree(d_const2);
	cudaFree(d_const3);
	cudaFree(d_const4);
	CUDA_CHECK();

	main_clock = omp_get_wtime() - main_clock;
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n");
	printf("Clock main: %lf\n", main_clock);
	printf("Clock exec: %lf\n", exec_clock);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	return 0;
}
