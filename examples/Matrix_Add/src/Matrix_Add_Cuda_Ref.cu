#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SEED 6834723

#define GRIDSIZE_0 ((SIZE + BLOCKSIZE_0 - 1) / BLOCKSIZE_0)
#define GRIDSIZE_1 ((SIZE + BLOCKSIZE_1 - 1) / BLOCKSIZE_1)

#ifdef _CTRL_EXAMPLES_CUDA_ERROR_CHECK_
	#define CUDA_CHECK()                                                           \
		{                                                                          \
			cudaError_t error;                                                     \
			if ((error = cudaGetLastError()) != cudaSuccess) {                     \
				fprintf(stdout, "%s::%d ERROR: %s: %s\n", __FILE__, __LINE__,      \
						cudaGetErrorName(error), cudaGetErrorString(error));       \
				exit(EXIT_FAILURE);                                                \
			}                                                                      \
		}
	#else
#define CUDA_CHECK()
#endif

double main_clock;
double exec_clock;

/* A. Kernel implementation */
__global__ void MatAdd(float* A, float* B, float* C, int size0, int size1, int NITER ) {
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	if ( i<size0 && j<size1 )
		for (int iter=0; iter<NITER; iter++)
			C[i*size1+j] = C[i*size1+j] + A[i*size1+j] + B[i*size1+j];
}

/* B. Initialize matrices */
void init_matrix(float* matrixA, float* matrixB, float* matrixC, int size){
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			matrixA[i * size + j]=1;
			matrixB[i * size + j]=2;
			matrixC[i * size + j]=0;
		}
	}
}

/* C. Calculate norm */
void norm_calc(float* matrix, int size){
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

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if ( argc != 4 ) {
		fprintf(stderr, "Usage: %s <matrixSize> <numIter> <GPU>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE = atoi( argv[1] );
	int NITER = atoi( argv[2] );
	int GPU = atoi( argv[3] );
	size_t MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", NITER);
	printf("\n POLICY: Sync");
	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, GPU); 
	printf("\n DEVICE: %s", cu_dev_prop.name);
	#ifdef _CTRL_QUEUE_
		printf("\n QUEUES: ON");
	#else
		printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	// 2. Alloc host data structures
	float* A = (float*)malloc(MATRIX_SIZE);
	float* B = (float*)malloc(MATRIX_SIZE);
	float* C = (float*)malloc(MATRIX_SIZE);

	if ( A == NULL || B == NULL || C == NULL ) {
		fprintf(stderr,"ERROR: Allocating host memory\n");
		exit ( EXIT_FAILURE );
	}
	
	srand( SEED );
	cudaSetDevice(GPU);
	cudaDeviceSynchronize();

	// 3. Initialize host data structures	
	init_matrix(A, B, C, SIZE);
	
	// 4. Alloc device data structures
	float* d_A;
	cudaMalloc(&d_A, MATRIX_SIZE);
	CUDA_CHECK();
	float* d_B;
	cudaMalloc(&d_B, MATRIX_SIZE);
	CUDA_CHECK();
	float* d_C;
	cudaMalloc(&d_C, MATRIX_SIZE);
	CUDA_CHECK();
	
	// 5. Sync and start timer
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	// 6. Copy vectors from host memory to device memory
	cudaMemcpy(d_A, A, MATRIX_SIZE, cudaMemcpyHostToDevice);
	CUDA_CHECK();
	cudaMemcpy(d_B, B, MATRIX_SIZE, cudaMemcpyHostToDevice);
	CUDA_CHECK();
	cudaMemcpy(d_C, C, MATRIX_SIZE, cudaMemcpyHostToDevice);
	CUDA_CHECK();

	// 7. Invoke kernel
	dim3 grid( GRIDSIZE_0, GRIDSIZE_1 );
	dim3 block( BLOCKSIZE_0, BLOCKSIZE_1 );
	MatAdd<<<grid, block>>>(d_A, d_B, d_C, SIZE, SIZE, NITER);
	CUDA_CHECK();
	
	// 8. Sync and stop timer
	cudaDeviceSynchronize();
	exec_clock = exec_clock - omp_get_wtime();
	
	// 9. Copy result from device memory to host memory
	cudaMemcpy(C, d_C, MATRIX_SIZE, cudaMemcpyDeviceToHost); 
	CUDA_CHECK();
	
	// 10. Free device memory
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
	CUDA_CHECK();
	
	// 11. Calculate NORM
	norm_calc(C, SIZE);

	// 12. Free host memory
	free(A);
	free(B);
	free(C);

	// 13. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	
	return 0;
}
