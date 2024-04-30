#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <stdbool.h>

#include <omp.h>

#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

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

double main_clock;
double exec_clock;

__global__ void Increment(float *input, float *output, int size, int n) {
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < size && j < size) {
		float res = input[i * size + j];
		for (int k = 0; k < n; k++)
			res++;
		output[i * size + j] = res;
		// output[i * size + j] = input[i * size + j] + n;
	}
}

typedef struct Host_Func_Args {
	float *matrix;
	int    size;
} Host_Func_Args_t;

void init_matrix(void *args) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Init tile");
	#endif //_PROFILING_ENABLED_
	Host_Func_Args_t *tmp = (Host_Func_Args_t *)args;

	float *matrix = tmp->matrix;
	int    size   = tmp->size;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i * size + j] = 0;
		}
	}
	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

void norm_calc(void *args) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Norm calc");
	#endif //_PROFILING_ENABLED_
	Host_Func_Args_t *tmp = (Host_Func_Args_t *)args;

	float *matrix    = tmp->matrix;
	int    size      = tmp->size;
	double resultado = 0;
	double suma      = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			suma += pow(matrix[i * size + j], 2);
		}
	}
	resultado = sqrt(suma);

	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

int main(int argc, char const *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 3) {
		fprintf(stderr, "\nUsage: %s <size> <n_iters>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int CU_DEV0 = 0;
	int CU_DEV1 = 1;

	int    SIZE        = atoi(argv[1]);
	int    N_ITER      = atoi(argv[2]);
	size_t MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n POLICY %s", "Async");
	struct cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, CU_DEV0);
	printf("\n CU_DEV 0: %s", cu_dev_prop.name);
	cudaGetDeviceProperties(&cu_dev_prop, CU_DEV1);
	printf("\n CU_DEV 1: %s", cu_dev_prop.name);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	dim3 dimBlock(BLOCKSIZE_0, BLOCKSIZE_1);
	dim3 dimGrid((SIZE + BLOCKSIZE_0 - 1) / dimBlock.x,
				 (SIZE + BLOCKSIZE_1 - 1) / dimBlock.y);

	float *h_m1, *h_m2, *h_m3;
	float *d_m1, *d0_m2, *d1_m2, *d_m3;

	cudaStream_t stream_DtH_d0, stream_HtD_d0, stream_kernel_d0, stream_host;
	cudaStream_t stream_DtH_d1, stream_HtD_d1, stream_kernel_d1;

	cudaEvent_t event_host_init, event_HtD_m1, event_kernel_d0, event_DtH_m2;
	cudaEvent_t event_HtD_m2, event_kernel_d1, event_DtH_m3, event_host_norm;

	int peer_access_01, peer_access_10;

	// Check for peer access between participating GPUs:
	cudaDeviceCanAccessPeer(&peer_access_01, CU_DEV0, CU_DEV1);
	cudaDeviceCanAccessPeer(&peer_access_10, CU_DEV1, CU_DEV0);
	CUDA_CHECK();

	printf("access 0->1 %d\n", peer_access_01);
	printf("access 1->0 %d\n", peer_access_10);
	fflush(stdout);

	// create stuff for dev 0
	cudaSetDevice(CU_DEV0);
	cudaDeviceEnablePeerAccess(CU_DEV1, 0);

	cudaStreamCreate(&stream_DtH_d0);
	cudaStreamCreate(&stream_HtD_d0);
	cudaStreamCreate(&stream_kernel_d0);
	cudaStreamCreate(&stream_host);

	cudaEventCreateWithFlags(&event_host_init, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_HtD_m1, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_kernel_d0, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_DtH_m2, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_host_norm, cudaEventDisableTiming);

	cudaHostAlloc((void **)&h_m1, MATRIX_SIZE, cudaHostAllocPortable);
	cudaHostAlloc((void **)&h_m2, MATRIX_SIZE, cudaHostAllocPortable);
	cudaHostAlloc((void **)&h_m3, MATRIX_SIZE, cudaHostAllocPortable);

	cudaMalloc((void **)&d_m1, MATRIX_SIZE);
	cudaMalloc((void **)&d0_m2, MATRIX_SIZE);
	CUDA_CHECK();

	// create stuff for dev 1
	cudaSetDevice(CU_DEV1);
	cudaDeviceEnablePeerAccess(CU_DEV0, 0);
	cudaStreamCreate(&stream_HtD_d1);
	cudaStreamCreate(&stream_DtH_d1);
	cudaStreamCreate(&stream_kernel_d1);

	cudaEventCreateWithFlags(&event_HtD_m2, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_kernel_d1, cudaEventDisableTiming);
	cudaEventCreateWithFlags(&event_DtH_m3, cudaEventDisableTiming);

	cudaMalloc((void **)&d1_m2, MATRIX_SIZE);
	cudaMalloc((void **)&d_m3, MATRIX_SIZE);

	cudaSetDevice(CU_DEV0);
	cudaDeviceSynchronize();
	cudaSetDevice(CU_DEV1);
	cudaDeviceSynchronize();

	CUDA_CHECK();

	Host_Func_Args_t init_matrix_args = (Host_Func_Args_t){.matrix = h_m1, .size = SIZE};
	Host_Func_Args_t norm_calc_args   = (Host_Func_Args_t){.matrix = h_m3, .size = SIZE};

	exec_clock = omp_get_wtime();

	for (int i = 0; i < N_ITER; i++) {
		// device 0
		cudaSetDevice(CU_DEV0);

		// init m1
		cudaStreamWaitEvent(stream_host, event_HtD_m1);
		cudaLaunchHostFunc(stream_host, init_matrix, &init_matrix_args);
		cudaEventRecord(event_host_init, stream_host);
		CUDA_CHECK();

		// HTD d0 m1
		cudaStreamWaitEvent(stream_HtD_d0, event_host_init);
		cudaStreamWaitEvent(stream_HtD_d0, event_kernel_d0);
		cudaMemcpyAsync(d_m1, h_m1, MATRIX_SIZE, cudaMemcpyHostToDevice, stream_HtD_d0);
		cudaEventRecord(event_HtD_m1, stream_HtD_d0);
		CUDA_CHECK();

		// K m1->m2 d0
		cudaStreamWaitEvent(stream_kernel_d0, event_HtD_m1);
		cudaStreamWaitEvent(stream_kernel_d0, event_DtH_m2);
		Increment<<<dimGrid, dimBlock, 0, stream_kernel_d0>>>(d_m1, d0_m2, SIZE, 5000);
		cudaEventRecord(event_kernel_d0, stream_kernel_d0);
		CUDA_CHECK();

		#ifdef CUDA_DEV2DEV
		cudaStreamWaitEvent(stream_DtH_d0, event_kernel_d0);
		cudaStreamWaitEvent(stream_DtH_d0, event_kernel_d1);
		cudaMemcpyAsync(d1_m2, d0_m2, MATRIX_SIZE, cudaMemcpyDeviceToDevice, stream_DtH_d0);
		cudaEventRecord(event_DtH_m2, stream_DtH_d0);
		CUDA_CHECK();
		cudaSetDevice(CU_DEV1);
		#else // CUDA_DEV2DEV
		//   DTH m2 d0
		cudaStreamWaitEvent(stream_DtH_d0, event_kernel_d0);
		cudaStreamWaitEvent(stream_DtH_d0, event_HtD_m2);
		cudaMemcpyAsync(h_m2, d0_m2, MATRIX_SIZE, cudaMemcpyDeviceToHost, stream_DtH_d0);
		cudaEventRecord(event_DtH_m2, stream_DtH_d0);

		// device 1
		cudaSetDevice(CU_DEV1);
		// HTD m2 d1
		cudaStreamWaitEvent(stream_HtD_d1, event_DtH_m2);
		cudaStreamWaitEvent(stream_HtD_d1, event_kernel_d1);
		cudaMemcpyAsync(d1_m2, h_m2, MATRIX_SIZE, cudaMemcpyHostToDevice, stream_HtD_d1);
		cudaEventRecord(event_HtD_m2, stream_HtD_d1);
		CUDA_CHECK();
		#endif // CUDA_DEV2DEV

		// K m2->m3 d1
		#ifdef CUDA_DEV2DEV
		cudaStreamWaitEvent(stream_kernel_d1, event_DtH_m2);
		#else // CUDA_DEV2DEV
		cudaStreamWaitEvent(stream_kernel_d1, event_HtD_m2);
		#endif // CUDA_DEV2DEV
		cudaStreamWaitEvent(stream_kernel_d1, event_DtH_m3);
		Increment<<<dimGrid, dimBlock, 0, stream_kernel_d1>>>(d1_m2, d_m3, SIZE, 5000);
		cudaEventRecord(event_kernel_d1, stream_kernel_d1);
		CUDA_CHECK();
	}

	cudaSetDevice(CU_DEV0);
	cudaDeviceSynchronize();
	cudaSetDevice(CU_DEV1);
	cudaDeviceSynchronize();

	exec_clock = omp_get_wtime() - exec_clock;

	// DTH m3 d1
	cudaStreamWaitEvent(stream_DtH_d1, event_kernel_d1);
	cudaStreamWaitEvent(stream_DtH_d1, event_host_norm);
	cudaMemcpyAsync(h_m3, d_m3, MATRIX_SIZE, cudaMemcpyDeviceToHost, stream_DtH_d1);
	cudaEventRecord(event_DtH_m3, stream_DtH_d1);

	// norm m3
	cudaStreamWaitEvent(stream_host, event_DtH_m3);
	cudaLaunchHostFunc(stream_host, norm_calc, &norm_calc_args);
	cudaEventRecord(event_host_norm, stream_host);

	cudaFreeHost(h_m1);
	cudaFreeHost(h_m2);
	cudaFreeHost(h_m3);

	cudaFree(d_m1);
	cudaFree(d0_m2);
	cudaFree(d1_m2);
	cudaFree(d_m3);

	// free events and streams
	cudaEventDestroy(event_host_init);
	cudaEventDestroy(event_HtD_m1);
	cudaEventDestroy(event_kernel_d0);
	cudaEventDestroy(event_DtH_m2);
	cudaEventDestroy(event_HtD_m2);
	cudaEventDestroy(event_kernel_d1);
	cudaEventDestroy(event_DtH_m3);
	cudaEventDestroy(event_host_norm);

	cudaStreamDestroy(stream_DtH_d0);
	cudaStreamDestroy(stream_HtD_d0);
	cudaStreamDestroy(stream_kernel_d0);
	cudaStreamDestroy(stream_host);
	cudaStreamDestroy(stream_DtH_d1);
	cudaStreamDestroy(stream_HtD_d1);
	cudaStreamDestroy(stream_kernel_d1);
	CUDA_CHECK();

	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
