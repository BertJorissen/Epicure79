/**
 * @file Texture_Test_Cuda_Ref.cu
 * @brief Texture Test: Native CUDA version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SEED 6834723

#define GRIDSIZE_0 ((SIZE + BLOCKSIZE_0 - 1) / BLOCKSIZE_0)
#define GRIDSIZE_1 ((SIZE + BLOCKSIZE_1 - 1) / BLOCKSIZE_1)

#ifdef _CTRL_EXAMPLES_CUDA_ERROR_CHECK_
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

/* A. Kernel implementation */
__global__ void TexTest(cudaTextureObject_t tex, float *B, int size) {
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < size && j < size)
		B[i * size + j] = tex2D<float>(tex, j, i);
}

/* B. Initialize matrices */
void init_matrix(float *matrixA, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrixA[i * size + j] = i * size + j;
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

void print_matrix(float *matrix, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%.2f ", matrix[i * size + j]);
		}
		printf("\n");
	}
	fflush(stdout);
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <matrixSize> <GPU>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int    SIZE        = atoi(argv[1]);
	int    GPU         = atoi(argv[2]);
	size_t MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n POLICY: Sync");
	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, GPU);
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, 0);
	printf("texturePitchAlignment: %lu\n", prop.texturePitchAlignment);
	cudaTextureObject_t tex;

	// 2. Alloc host data structures
	float *A = (float *)malloc(MATRIX_SIZE);
	float *B = (float *)malloc(MATRIX_SIZE);

	if (A == NULL || B == NULL) {
		fprintf(stderr, "ERROR: Allocating host memory\n");
		exit(EXIT_FAILURE);
	}
	// srand(SEED);
	cudaSetDevice(GPU);
	cudaDeviceSynchronize();

	// 3. Initialize host data structures
	init_matrix(A, SIZE);

	// 4. Alloc device data structures
	float *d_A;
	size_t pitch;
	cudaMallocPitch(&d_A, &pitch, SIZE * sizeof(float), SIZE);
	CUDA_CHECK();
	float *d_B;
	cudaMalloc(&d_B, MATRIX_SIZE);
	CUDA_CHECK();

	// 5. Sync and start timer
	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	// Create texture object
	struct cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));
	resDesc.resType                  = cudaResourceTypePitch2D;
	resDesc.res.pitch2D.devPtr       = d_A;
	resDesc.res.pitch2D.width        = SIZE;
	resDesc.res.pitch2D.height       = SIZE;
	resDesc.res.pitch2D.desc         = cudaCreateChannelDesc<float>();
	resDesc.res.pitch2D.pitchInBytes = pitch;
	struct cudaTextureDesc texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	cudaCreateTextureObject(&tex, &resDesc, &texDesc, NULL);

	// 6. Copy vectors from host memory to device memory
	cudaMemcpy2D(d_A, pitch, A, SIZE * sizeof(float), SIZE * sizeof(float), SIZE, cudaMemcpyHostToDevice);
	CUDA_CHECK();

	// 7. Invoke kernel
	dim3 grid(GRIDSIZE_0, GRIDSIZE_1);
	dim3 block(BLOCKSIZE_0, BLOCKSIZE_1);
	TexTest<<<grid, block>>>(tex, d_B, SIZE);
	CUDA_CHECK();

	// 8. Sync and stop timer
	cudaDeviceSynchronize();
	exec_clock = exec_clock - omp_get_wtime();

	// 9. Copy result from device memory to host memory
	cudaMemcpy(B, d_B, MATRIX_SIZE, cudaMemcpyDeviceToHost);
	CUDA_CHECK();

	// destroy texture object
	cudaDestroyTextureObject(tex);

	// 10. Free device memory
	cudaFree(d_A);
	cudaFree(d_B);
	CUDA_CHECK();

	// 11. Calculate NORM
	print_matrix(B, SIZE);
	norm_calc(B, SIZE);

	// 12. Free host memory
	free(A);
	free(B);

	// 13. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
