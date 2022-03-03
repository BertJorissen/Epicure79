#include "magma_v2.h"
#include <cuda_runtime.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define SEED 6834723

#ifdef _CTRL_MAGMA_ERROR_CHECK_
#define MAGMA_OP(op)                                                                       \
	{                                                                                      \
		magma_int_t error = op;                                                            \
		if (error != MAGMA_SUCCESS) {                                                      \
			fprintf(stderr, "\tMAGMA Error at: %s::%d\n %s: ", __FILE__, __LINE__, error); \
			magma_strerror(error);                                                         \
			fflush(stderr);                                                                \
			exit(EXIT_FAILURE);                                                            \
		}                                                                                  \
	}

#define CUDA_OP(op)                                                                   \
	{                                                                                 \
		cudaError_t error = op;                                                       \
		if (error != cudaSuccess) {                                                   \
			fprintf(stderr, "\tCUDA Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
					cudaGetErrorName(error), cudaGetErrorString(error));              \
			fflush(stderr);                                                           \
			exit(EXIT_FAILURE);                                                       \
		}                                                                             \
	}
#else
#define MAGMA_OP(op) op
#define CUDA_OP(op)  op
#endif

double main_clock;
double exec_clock;

/* B. Initialize matrices */
void init_matrix(float *A, float *B, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			A[i * size + j] = 1;
			B[i * size + j] = 2;
		}
	}
}

/* C. Calculate norm */
void norm_calc(float *matrix, int size) {
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
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <matrixSize> <GPU>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size = atoi(argv[1]);
	int GPU  = atoi(argv[2]);

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

	// 2. Alloc host data structures
	float *A, *B;
	MAGMA_OP(magma_smalloc_pinned(&A, size * size));
	MAGMA_OP(magma_smalloc_pinned(&B, size * size));

	srand(SEED);
	float         alpha = 1.0;
	magma_queue_t queue = NULL;
	magma_queue_create(GPU, &queue);

	MAGMA_OP(magma_queue_sync(queue));

	// 3. Initialize host data structures
	init_matrix(A, B, size);

	// 4. Alloc device data structures
	float *d_A, *d_B;
	MAGMA_OP(magma_smalloc(&d_A, size * size));
	MAGMA_OP(magma_smalloc(&d_B, size * size));

	// 5. Sync and start timer
	MAGMA_OP(magma_queue_sync(queue));
	exec_clock = omp_get_wtime();

	// 6. Copy vectors from host memory to device memory
	magma_ssetmatrix(size, size, A, size, d_A, size, queue);
	magma_ssetmatrix(size, size, B, size, d_B, size, queue);

	// 7. Invoke kernel
	magma_saxpy(size * size, alpha, d_A, 1, d_B, 1, queue);

	// 8. Sync and stop timer
	MAGMA_OP(magma_queue_sync(queue));
	exec_clock = omp_get_wtime() - exec_clock;

	// 9. Copy result from device memory to host memory
	magma_sgetmatrix(size, size, d_B, size, B, size, queue);

	// 10. Free device memory
	MAGMA_OP(magma_free(d_A));
	MAGMA_OP(magma_free(d_B));

	// 11. Calculate NORM
	norm_calc(B, size);

	// 12. Free host memory
	MAGMA_OP(magma_free_pinned(A));
	MAGMA_OP(magma_free_pinned(B));

	// 13. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
