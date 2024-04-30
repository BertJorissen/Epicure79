// System includes
#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>

#define SEED    6834723
#define EPSILON 0.0001

double main_clock;
double exec_clock;

int GPU   = 0;


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
		for (int i = 0; i < rows; i++) {
			float min = -(1 - col_sum_a) + EPSILON;
			float max = 1 - col_sum_a - EPSILON;
			float random = ((float)rand()) / (float)RAND_MAX;
			float range  = max - min;
			float value = (random * range) + min;
			matrix_a[i * columns + j] = value;
			matrix_b[i * columns + j] = value;
			matrix_c[i * columns + j] = 0;
			col_sum_a += fabsf(value);
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
	Host_Compute_Args_t *tmp = (Host_Compute_Args_t *)args;

	int       ITER        = tmp->ITER;
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
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	// Simpler interface for square matrices (only one parameter)
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <matrix_size> <n_power> <device>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE        = atoi(argv[1]);
	int POWER       = atoi(argv[2]);
	int MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	double *p_res = (double *)malloc(POWER * sizeof(double));
	double *p_sum = (double *)malloc(POWER * sizeof(double));

	cudaSetDevice(GPU);

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
	float *matrix_tmp = (float *)malloc(MATRIX_SIZE);

	/*Device variables*/
	float *d_A, *d_B, *d_C;
	cudaMalloc(&d_A, MATRIX_SIZE);
	cudaMalloc(&d_B, MATRIX_SIZE);
	cudaMalloc(&d_C, MATRIX_SIZE);

	cudaStream_t stream_DtH, stream_kernel, stream_host;

	cudaStreamCreate(&stream_DtH);
	cudaStreamCreate(&stream_kernel);
	cudaStreamCreate(&stream_host);

	cudaEvent_t event_kernel, event_DtH_C, event_DtH_B, event_host_C, event_host_B;

	cudaEventCreateWithFlags(&event_kernel, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_DtH_C, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_DtH_B, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_host_C, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_host_B, cudaEventDisableTiming);


	/*Initialize variables*/
	Init_Tiles_Args_t init_args = (Init_Tiles_Args_t){.matrix_a = A,
													  .matrix_b = B,
													  .matrix_c = C,
													  .rows     = SIZE,
													  .columns  = SIZE};
	cudaLaunchHostFunc(stream_host, Init_Tiles, &init_args);

	Host_Compute_Args_t host_args_c =
		(Host_Compute_Args_t){.ITER       = 0,
							  .p_sum      = p_sum,
							  .p_res      = p_res,
							  .matrix     = C,
							  .matrix_res = matrix_tmp,
							  .rows       = SIZE,
							  .columns    = SIZE};
	Host_Compute_Args_t host_args_b =
		(Host_Compute_Args_t){.ITER       = 0,
							  .p_sum      = p_sum,
							  .p_res      = p_res,
							  .matrix     = B,
							  .matrix_res = matrix_tmp,
							  .rows       = SIZE,
							  .columns    = SIZE};

	/* Start timer */
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	/*Copy to the device*/
	cudaMemcpyAsync(d_A, A, MATRIX_SIZE, cudaMemcpyHostToDevice, stream_kernel);

	cudaMemcpyAsync(d_B, B, MATRIX_SIZE, cudaMemcpyHostToDevice, stream_kernel);

	cudaMemcpyAsync(d_C, C, MATRIX_SIZE, cudaMemcpyHostToDevice, stream_kernel);

	/* Wait for data in the device to start first kernel computation */

	for (int i = 0; i < POWER; i++) {
		if ((i % 2) == 0) {
			cudaStreamWaitEvent(stream_kernel, event_DtH_C, 0); // Wait for previous C transfer to finish
			matrixMulCUDA<<<grid, threads, 0, stream_kernel>>>(d_C, d_A, d_B, SIZE);
			cudaEventRecord(event_kernel, stream_kernel); // kernel C = A x B record

			cudaStreamWaitEvent(stream_DtH, event_kernel, 0); // Wait for kernel C = A x B
			cudaStreamWaitEvent(stream_DtH, event_host_C, 0); // Wait for previous host function using C
			cudaMemcpyAsync(C, d_C, MATRIX_SIZE, cudaMemcpyDeviceToHost, stream_DtH);
			cudaEventRecord(event_DtH_C, stream_DtH); // transfer Device to Host of C

			cudaStreamWaitEvent(stream_host, event_DtH_C, 0); // Wait for transfer of C to host
			cudaLaunchHostFunc(stream_host, Host_Compute, &host_args_c);
			cudaEventRecord(event_host_C, stream_host); // C is being used in the host
		} else {
			cudaStreamWaitEvent(stream_kernel, event_DtH_B, 0); // Wait for previous B transfer to finish
			matrixMulCUDA<<<grid, threads, 0, stream_kernel>>>(d_B, d_A, d_C, SIZE);
			cudaEventRecord(event_kernel, stream_kernel); // kernel B = A x C record

			cudaStreamWaitEvent(stream_DtH, event_kernel, 0); // Wait for kernel B = A x C
			cudaStreamWaitEvent(stream_DtH, event_host_B, 0); // Wait for previous host function using B
			cudaMemcpyAsync(B, d_B, MATRIX_SIZE, cudaMemcpyDeviceToHost, stream_DtH);
			cudaEventRecord(event_DtH_B, stream_DtH); // transfer Device to Host of B

			cudaStreamWaitEvent(stream_host, event_DtH_B, 0); // Wait for transfer of B to host
			cudaLaunchHostFunc(stream_host, Host_Compute, &host_args_b);
			cudaEventRecord(event_host_B, stream_host); // B is being used in the host
		}
	}

	/* Stop timer */
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	cudaEventDestroy(event_kernel);
	cudaEventDestroy(event_DtH_C);
	cudaEventDestroy(event_DtH_B);
	cudaEventDestroy(event_host_C);
	cudaEventDestroy(event_host_B);
	cudaStreamDestroy(stream_DtH);
	cudaStreamDestroy(stream_kernel);
	cudaStreamDestroy(stream_host);
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);

	cudaFreeHost(A);
	cudaFreeHost(B);
	cudaFreeHost(C);
	free(p_sum);
	free(p_res);
	free(matrix_tmp);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
