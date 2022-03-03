#include "cublas_v2.h"
#include <cuda_runtime.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define SEED 6834723

#ifdef _CTRL_EXAMPLES_CUDA_ERROR_CHECK_
#define CUBLAS_OP(op)                                                                       \
	{                                                                                       \
		cublasStatus_t error = op;                                                          \
		if (error != CUBLAS_STATUS_SUCCESS) {                                               \
			fprintf(stderr, "\tCUBLAS Error at: %s::%d\n %s: ", __FILE__, __LINE__, error); \
			printMessageFromCuBLASError(error);                                             \
			fflush(stderr);                                                                 \
			exit(EXIT_FAILURE);                                                             \
		}                                                                                   \
	}

#define CUDA_OP(op)                                                                                            \
	{                                                                                                          \
		cudaError_t error = op;                                                                                \
		if (error != cudaSuccess) {                                                                            \
			fprintf(stderr, "\tCUDA Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, cudaGetErrorName(error), \
					cudaGetErrorString(error));                                                                \
			fflush(stderr);                                                                                    \
			exit(EXIT_FAILURE);                                                                                \
		}                                                                                                      \
	}
#else
#define CUBLAS_OP(op) op
#define CUDA_OP(op)   op
#endif

double main_clock;
double exec_clock;

void printMessageFromCuBLASError(cublasStatus_t error_code) {
	switch (error_code) {
		case 0: fprintf(stderr, "CUBLAS_STATUS_SUCCESS"); break;
		case 1: fprintf(stderr, "CUBLAS_STATUS_NOT_INITIALIZED"); break;
		case 3: fprintf(stderr, "CUBLAS_STATUS_ALLOC_FAILED"); break;
		case 7: fprintf(stderr, "CUBLAS_STATUS_INVALID_VALUE"); break;
		case 8: fprintf(stderr, "CUBLAS_STATUS_ARCH_MISMATCH"); break;
		case 11: fprintf(stderr, "CUBLAS_STATUS_MAPPING_ERROR"); break;
		case 13: fprintf(stderr, "CUBLAS_STATUS_EXECUTION_FAILED"); break;
		case 14: fprintf(stderr, "CUBLAS_STATUS_INTERNAL_ERROR"); break;
		case 15: fprintf(stderr, "CUBLAS_STATUS_NOT_SUPPORTED"); break;
		case 16: fprintf(stderr, "CUBLAS_STATUS_LICENSE_ERROR"); break;
	}
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <matrixSize> <n_iter> <GPU>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size   = atoi(argv[1]);
	int n_iter = atoi(argv[2]);
	int GPU    = atoi(argv[3]);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", size);
	printf("\n POLICY: Sync");
	struct cudaDeviceProp cu_dev_prop;
	CUDA_OP(cudaGetDeviceProperties(&cu_dev_prop, GPU));
	printf("\n DEVICE: %s", cu_dev_prop.name);
	#ifdef _CTRL_QUEUE_
	printf("\n QUEUES: ON");
	#else
	printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	float *A, *dA;

	CUDA_OP(cudaMallocHost((void **)&A, size * size * sizeof(float)));
	CUDA_OP(cudaMalloc((void **)&dA, sizeof(float) * size * size));

	cublasHandle_t handle;

	float cu_one   = 1.0;
	float cu_zero  = 0.0;
	float cu_alpha = cu_one;
	float cu_beta  = cu_zero;

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			A[i * size + j] = ((float)i * j) / size;
		}
	}

	CUBLAS_OP(cublasCreate(&handle));

	CUDA_OP(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime();

	CUBLAS_OP(cublasSetMatrix(size, size, sizeof(float), A, size, dA, size));

	for (int power = 0; power > n_iter; power++) {
		CUBLAS_OP(cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, size, size, size, &cu_alpha, dA, size, dA, size,
							  &cu_beta, dA, size)); // C = αAB + βC
	}

	CUDA_OP(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime() - exec_clock;

	CUBLAS_OP(cublasGetMatrix(size, size, sizeof(float), dA, size, A, size));

	CUBLAS_OP(cublasDestroy(handle));
	CUDA_OP(cudaFree(dA));

	double resultado = 0, suma = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			#ifdef DEBUG
			printf("%f \n", A[i * size + j]);
			#endif
			suma += pow(A[i * size + j], 2);
		}
	}
	resultado = sqrt(suma);
	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);

	CUDA_OP(cudaFreeHost(A));

	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	return 0;
}
