#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>

#define SEED 6834723

#ifdef _CTRL_EXAMPLES_CUDA_ERROR_CHECK_
	#define CUBLAS_OP( op ) { \
		cublasStatus_t error = op; \
		if (error != CUBLAS_STATUS_SUCCESS) { \
			fprintf(stderr, "\tCUBLAS Error at: %s::%d\n %s: ", __FILE__, __LINE__, error); \
			printMessageFromCuBLASError(error); \
			fflush(stderr); \
			exit(EXIT_FAILURE); \
		} \
	}

	#define CUDA_OP( op ) { \
		cudaError_t error = op; \
		if (error != cudaSuccess) { \
			fprintf(stderr, "\tCUDA Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
					cudaGetErrorName(error), cudaGetErrorString(error)); \
			fflush(stderr); \
			exit(EXIT_FAILURE); \
		} \
	}
#else
	#define CUBLAS_OP( op ) op
	#define CUDA_OP( op ) op
#endif

double main_clock;
double exec_clock;

/* B. Initialize matrices */
void init_matrix(float* A, float* B, int size) {
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			A[i * size + j]=1;
			B[i * size + j]=2;
		}
	}
}

/* C. Calculate norm */
void norm_calc(float* matrix, int size) {
	double resultado=0;
	double suma=0;
	for (int i=0; i<size; i++ ) {
		for (int j=0; j<size; j++ ) {
			suma += pow( matrix[i * size + j] , 2);
		}
	}
	resultado=sqrt( suma );

	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf",suma);
	printf("\n Result: %lf \n",resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

void printMessageFromCuBLASError(cublasStatus_t error_code) {
	switch(error_code) {
		case 0:		fprintf(stderr, "CUBLAS_STATUS_SUCCESS\n"); 			break;
		case 1:		fprintf(stderr, "CUBLAS_STATUS_NOT_INITIALIZED\n"); 	break;
		case 3:		fprintf(stderr, "CUBLAS_STATUS_ALLOC_FAILED\n"); 		break;
		case 7:		fprintf(stderr, "CUBLAS_STATUS_INVALID_VALUE\n"); 		break;
		case 8:		fprintf(stderr, "CUBLAS_STATUS_ARCH_MISMATCH\n"); 		break;
		case 11:	fprintf(stderr, "CUBLAS_STATUS_MAPPING_ERROR\n"); 		break;
		case 13:	fprintf(stderr, "CUBLAS_STATUS_EXECUTION_FAILED\n"); 	break;
		case 14:	fprintf(stderr, "CUBLAS_STATUS_INTERNAL_ERROR\n"); 		break;
		case 15:	fprintf(stderr, "CUBLAS_STATUS_NOT_SUPPORTED\n"); 		break;
		case 16:	fprintf(stderr, "CUBLAS_STATUS_LICENSE_ERROR\n"); 		break;
	}
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if ( argc != 3 ) {
		fprintf(stderr, "Usage: %s <matrixSize> <GPU>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size = atoi( argv[1] );
	int GPU = atoi( argv[2] );

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", size);
	printf("\n POLICY: Sync");
	struct cudaDeviceProp cu_dev_prop;
	CUDA_OP( cudaGetDeviceProperties(&cu_dev_prop, GPU) ); 
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
	CUDA_OP( cudaMallocHost((void**)&A, size * size * sizeof(float)) );
	CUDA_OP( cudaMallocHost((void**)&B, size * size * sizeof(float)) );

	srand( SEED );
	float alpha = 1.0;
	CUDA_OP( cudaSetDevice(GPU) );
	cublasHandle_t handle;
	CUBLAS_OP( cublasCreate(&handle) );

	CUDA_OP( cudaDeviceSynchronize() );

	// 3. Initialize host data structures	
	init_matrix(A, B, size);
	
	// 4. Alloc device data structures
	float *d_A, *d_B;
	CUDA_OP( cudaMalloc((void**)&d_A, size * size * sizeof(float)) );
	CUDA_OP( cudaMalloc((void**)&d_B, size * size * sizeof(float)) );
	
	// 5. Sync and start timer
	CUDA_OP( cudaDeviceSynchronize() );
	exec_clock = omp_get_wtime();

	// 6. Copy vectors from host memory to device memory
	CUBLAS_OP( cublasSetMatrix(size, size, sizeof(float), A, size, d_A, size) );
	CUBLAS_OP( cublasSetMatrix(size, size, sizeof(float), B, size, d_B, size) );

	// 7. Invoke kernel
	CUBLAS_OP( cublasSaxpy(handle, size * size, &alpha, d_A, 1, d_B, 1) );

	// 8. Sync and stop timer
	CUDA_OP( cudaDeviceSynchronize() );
	exec_clock = exec_clock - omp_get_wtime();
	
	// 9. Copy result from device memory to host memory
	CUBLAS_OP( cublasGetMatrix(size, size, sizeof(float), d_B, size, B, size) );

	// 10. Free device memory
	CUDA_OP( cudaFree(d_A) );
	CUDA_OP( cudaFree(d_B) );
	
	// 11. Calculate NORM
	norm_calc(B, size);

	// 12. Free host memory
	CUDA_OP( cudaFreeHost(A) );
	CUDA_OP( cudaFreeHost(B) );

	// 13. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	
	return 0;
}
