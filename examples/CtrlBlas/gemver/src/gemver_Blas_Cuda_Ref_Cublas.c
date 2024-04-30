/**
 * @file gemver_Blas_Cuda_Ref_Cublas.c
 * @author Trasgo Group
 * @brief gemver: Native CUBLAS version
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

#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define CUBLAS_OP(op) op
#define CUDA_OP(op)   op
#endif

double main_clock;
double exec_clock;

void init_array(float *A, float *u1, float *u2, float *v1, float *v2, float *y, float *z, int size) {
	for (int i = 0; i < size; i++) {
		u1[i] = i;
		u2[i] = (i + 1) / size / 2.0;
		v1[i] = (i + 1) / size / 4.0;
		v2[i] = (i + 1) / size / 6.0;
		y[i]  = (i + 1) / size / 8.0;
		z[i]  = (i + 1) / size / 9.0;
		for (int j = 0; j < size; j++) {
			A[(i * size) + j] = ((float)i * j) / size;
		}
	}
}

void norm_calc(float *w, int size) {
	double suma = 0, resultado = 0;
	for (int i = 0; i < size; i++) {
		suma += pow(w[i], 2);
	}
	resultado = sqrt(suma);
	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

void printMessageFromCuBLASError(cublasStatus_t error_code) {
	switch (error_code) {
		case 0: fprintf(stderr, "CUBLAS_STATUS_SUCCESS\n"); break;
		case 1: fprintf(stderr, "CUBLAS_STATUS_NOT_INITIALIZED\n"); break;
		case 3: fprintf(stderr, "CUBLAS_STATUS_ALLOC_FAILED\n"); break;
		case 7: fprintf(stderr, "CUBLAS_STATUS_INVALID_VALUE\n"); break;
		case 8: fprintf(stderr, "CUBLAS_STATUS_ARCH_MISMATCH\n"); break;
		case 11: fprintf(stderr, "CUBLAS_STATUS_MAPPING_ERROR\n"); break;
		case 13: fprintf(stderr, "CUBLAS_STATUS_EXECUTION_FAILED\n"); break;
		case 14: fprintf(stderr, "CUBLAS_STATUS_INTERNAL_ERROR\n"); break;
		case 15: fprintf(stderr, "CUBLAS_STATUS_NOT_SUPPORTED\n"); break;
		case 16: fprintf(stderr, "CUBLAS_STATUS_LICENSE_ERROR\n"); break;
	}
}

void gemver_cuda(float *A, float *u1, float *u2, float *v1, float *v2, float *w, float *y, float *z, const float alpha, const float beta, const int size, const int gpu) {
	float  one  = 1.0;
	float  zero = 0.0;
	float *d_A, *d_B;
	float *d_u1, *d_u2;
	float *d_v1, *d_v2;
	float *d_w, *d_x, *d_y, *d_z;

	CUDA_OP(cudaSetDevice(gpu));
	cublasHandle_t handle;
	CUBLAS_OP(cublasCreate(&handle));

	CUDA_OP(cudaMalloc((void **)&d_A, sizeof(float) * size * size));
	CUDA_OP(cudaMalloc((void **)&d_B, sizeof(float) * size * size));
	CUDA_OP(cudaMalloc((void **)&d_u1, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_u2, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_v1, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_v2, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_w, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_x, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_y, sizeof(float) * size));
	CUDA_OP(cudaMalloc((void **)&d_z, sizeof(float) * size));

	exec_clock = omp_get_wtime();

	CUBLAS_OP(cublasSetMatrix(size, size, sizeof(float), A, size, d_A, size));
	CUBLAS_OP(cublasSetVector(size, sizeof(float), u1, 1, d_u1, 1));
	CUBLAS_OP(cublasSetVector(size, sizeof(float), u2, 1, d_u2, 1));
	CUBLAS_OP(cublasSetVector(size, sizeof(float), v1, 1, d_v1, 1));
	CUBLAS_OP(cublasSetVector(size, sizeof(float), v2, 1, d_v2, 1));
	CUBLAS_OP(cublasSetVector(size, sizeof(float), y, 1, d_y, 1));
	CUBLAS_OP(cublasSetVector(size, sizeof(float), z, 1, d_z, 1));

	CUBLAS_OP(cublasScopy(handle, size * size, d_A, 1, d_B, 1));                                       // Copy A to B
	CUBLAS_OP(cublasSger(handle, size, size, &one, d_u1, 1, d_v1, 1, d_B, size));                      // B = u1*v1' + B
	CUBLAS_OP(cublasSger(handle, size, size, &one, d_u2, 1, d_v2, 1, d_B, size));                      // B = u2*v2' + B
	CUBLAS_OP(cublasScopy(handle, size, d_z, 1, d_x, 1));                                              // Copy z to x
	CUBLAS_OP(cublasSgemv(handle, CUBLAS_OP_T, size, size, &beta, d_B, size, d_y, 1, &one, d_x, 1));   // x = βB'y + x (B' * y)
	CUBLAS_OP(cublasSgemv(handle, CUBLAS_OP_N, size, size, &alpha, d_B, size, d_x, 1, &zero, d_w, 1)); // w = αBx (B * x)

	CUDA_OP(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime() - exec_clock;

	CUBLAS_OP(cublasGetVector(size, sizeof(float), d_w, 1, w, 1));

	cublasDestroy(handle);

	CUDA_OP(cudaFree(d_A));
	CUDA_OP(cudaFree(d_B));
	CUDA_OP(cudaFree(d_u1));
	CUDA_OP(cudaFree(d_u2));
	CUDA_OP(cudaFree(d_v1));
	CUDA_OP(cudaFree(d_v2));
	CUDA_OP(cudaFree(d_w));
	CUDA_OP(cudaFree(d_x));
	CUDA_OP(cudaFree(d_y));
	CUDA_OP(cudaFree(d_z));
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 5) {
		fprintf(stderr, "Usage: %s <matrixSize> <alpha> <beta> <GPU>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int   size  = atoi(argv[1]);
	float alpha = atof(argv[2]);
	float beta  = atof(argv[3]);
	int   GPU   = atoi(argv[4]);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d x %d", size, size);
	printf("\n ALPHA: %.2f", alpha);
	printf("\n BETA: %.2f", beta);
	printf("\n POLICY Sync");
	struct cudaDeviceProp cu_dev_prop;
	CUDA_OP(cudaGetDeviceProperties(&cu_dev_prop, GPU));
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	float *A;
	float *u1, *u2;
	float *v2, *v1;
	float *w, *y, *z;

	CUDA_OP(cudaMallocHost((void **)&A, size * size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&u1, size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&u2, size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&v2, size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&v1, size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&w, size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&y, size * sizeof(float)));
	CUDA_OP(cudaMallocHost((void **)&z, size * sizeof(float)));

	init_array(A, u1, u2, v1, v2, y, z, size);
	gemver_cuda(A, u1, u2, v1, v2, w, y, z, alpha, beta, size, GPU);
	norm_calc(w, size);

	CUDA_OP(cudaFreeHost(A));
	CUDA_OP(cudaFreeHost(u1));
	CUDA_OP(cudaFreeHost(u2));
	CUDA_OP(cudaFreeHost(v1));
	CUDA_OP(cudaFreeHost(v1));
	CUDA_OP(cudaFreeHost(w));
	CUDA_OP(cudaFreeHost(y));
	CUDA_OP(cudaFreeHost(z));

	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
