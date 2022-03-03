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

int GPU   = 0;
int POWER = 0;

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

typedef struct Init_Tiles_Args {
	float *matrix_a, *matrix_b, *matrix_c;
	int    rows, columns;
} Init_Tiles_Args_t;

float RandomFloat(float min, float max) {
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;
	float range  = max - min;
	return (random * range) + min;
}

void Init_Tiles(void *args) {
	Init_Tiles_Args_t *tmp      = (Init_Tiles_Args_t *)args;
	float             *matrix_a = tmp->matrix_a;
	float             *matrix_b = tmp->matrix_b;
	float             *matrix_c = tmp->matrix_c;
	int                rows     = tmp->rows;
	int                columns  = tmp->columns;

	srand(SEED);
	for (int j = 0; j < columns; j++) {
		float col_sum_a = 0;
		float col_sum_b = 0;
		for (int i = 0; i < rows; i++) {
			float a = RandomFloat(-(1 - col_sum_a) + EPSILON, 1 - col_sum_a - EPSILON);
			float b = RandomFloat(-(1 - col_sum_b) + EPSILON, 1 - col_sum_b - EPSILON);

			matrix_a[i * columns + j] = a;
			matrix_b[i * columns + j] = b;
			matrix_c[i * columns + j] = 0;
			col_sum_a += fabsf(a);
			col_sum_b += fabsf(b);
		}
	}
}

typedef struct Host_Compute_Args {
	int     ITER;
	double *p_sum;
	double *p_res;
	float  *matrix;
	float  *matrix_res;
	int     rows;
	int     columns;
} Host_Compute_Args_t;

void Host_Compute(void *args) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host task");
	#endif //_PROFILING_ENABLED_

	Host_Compute_Args_t *tmp = (Host_Compute_Args_t *)args;

	static int ITER       = 0;
	double    *p_sum      = tmp->p_sum;
	double    *p_res      = tmp->p_res;
	float     *matrix     = tmp->matrix;
	float     *matrix_res = tmp->matrix_res;
	int        rows       = tmp->rows;
	int        columns    = tmp->columns;

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

	ITER = (ITER + 1) % POWER;

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
	int SIZE        = atoi(argv[1]);
	POWER           = atoi(argv[2]);
	GPU             = atoi(argv[3]);
	int MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, GPU);
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, ", cu_dev_prop.name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", POWER);
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n POLICY SYNC");
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	double *p_res = (double *)malloc(POWER * sizeof(double));
	double *p_sum = (double *)malloc(POWER * sizeof(double));

	// By default, we use device 0, otherwise we override the device ID based on
	// what is provided at the command line

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
	Init_Tiles_Args_t init_args = (Init_Tiles_Args_t){.matrix_a = A,
													  .matrix_b = B,
													  .matrix_c = C,
													  .rows     = SIZE,
													  .columns  = SIZE};
	Init_Tiles(&init_args);

	Host_Compute_Args_t host_args =
		(Host_Compute_Args_t){.ITER       = 0,
							  .p_sum      = p_sum,
							  .p_res      = p_res,
							  .matrix     = NULL,
							  .matrix_res = matrix_tmp,
							  .rows       = SIZE,
							  .columns    = SIZE};

	/*Start timer*/
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	/*Copy to the device*/
	cudaMemcpy(d_A, A, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, B, MATRIX_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, C, MATRIX_SIZE, cudaMemcpyHostToDevice);
	CUDA_CHECK();

	for (int i = 0; i < POWER; i++) {
		host_args.ITER = i;

		if ((i % 2) == 0) {
			matrixMulCUDA<<<grid, threads>>>(d_C, d_A, d_B, SIZE);
			CUDA_CHECK();

			cudaMemcpy(C, d_C, MATRIX_SIZE, cudaMemcpyDeviceToHost);
			CUDA_CHECK();

			host_args.matrix = C;
			Host_Compute(&host_args);
		} else {
			matrixMulCUDA<<<grid, threads>>>(d_B, d_A, d_C, SIZE);
			CUDA_CHECK();

			cudaMemcpy(B, d_B, MATRIX_SIZE, cudaMemcpyDeviceToHost);
			CUDA_CHECK();

			host_args.matrix = B;
			Host_Compute(&host_args);
		}
	}
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	/* Stop timer */

	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
	CUDA_CHECK();

	/* PRINT RESULTS */
	#ifdef _CTRL_EXAMPLES_TEST_MODE_
	for (int i = 0; i < POWER; i++) {
		printf("%lf, %lf, ", p_sum[i], p_res[i]);
	}
	fflush(stdout);
	#elif _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", p_sum[POWER - 1], p_res[POWER - 1]);
	fflush(stdout);
	#else
	printf("\n ----------------------- NORM ----------------------- \n\n");
	fflush(stdout);
	for (int i = 0; i < POWER; i++) {
		printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]);
		fflush(stdout);
	}
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif

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
	fflush(stdout);
	#else
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif

	return 0;
}
