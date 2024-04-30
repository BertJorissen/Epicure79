/**
 * @file gemver_Blas_Cuda_Ref_Magma.c
 * @author Trasgo Group
 * @brief gemver: Native CUDA Magma version
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

#include "magma_v2.h"
#include <cuda_runtime.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void gemver_cuda(float *A, float *u1, float *u2, float *v1, float *v2, float *w, float *y, float *z, const float alpha, const float beta, const int size, const int gpu) {
	float *d_A, *d_B;
	float *d_u1, *d_u2;
	float *d_v1, *d_v2;
	float *d_w, *d_x, *d_y, *d_z;

	magma_queue_t queue = NULL;
	magma_queue_create(gpu, &queue);

	MAGMA_OP(magma_smalloc(&d_A, size * size));
	MAGMA_OP(magma_smalloc(&d_B, size * size));
	MAGMA_OP(magma_smalloc(&d_u1, size));
	MAGMA_OP(magma_smalloc(&d_u2, size));
	MAGMA_OP(magma_smalloc(&d_v1, size));
	MAGMA_OP(magma_smalloc(&d_v2, size));
	MAGMA_OP(magma_smalloc(&d_w, size));
	MAGMA_OP(magma_smalloc(&d_x, size));
	MAGMA_OP(magma_smalloc(&d_y, size));
	MAGMA_OP(magma_smalloc(&d_z, size));

	exec_clock = omp_get_wtime();

	magma_ssetmatrix(size, size, A, size, d_A, size, queue);

	magma_ssetvector(size, u1, 1, d_u1, 1, queue);
	magma_ssetvector(size, u2, 1, d_u2, 1, queue);
	magma_ssetvector(size, v1, 1, d_v1, 1, queue);
	magma_ssetvector(size, v2, 1, d_v2, 1, queue);
	magma_ssetvector(size, y, 1, d_y, 1, queue);
	magma_ssetvector(size, z, 1, d_z, 1, queue);

	magma_scopy(size * size, d_A, 1, d_B, 1, queue);                                   // Copy A to B
	magma_sger(size, size, 1, d_u1, 1, d_v1, 1, d_B, size, queue);                     // B = u1*v1' + B
	magma_sger(size, size, 1, d_u2, 1, d_v2, 1, d_B, size, queue);                     // B = u2*v2' + B
	magma_scopy(size, d_z, 1, d_x, 1, queue);                                          // Copy z to x
	magma_sgemv(MagmaTrans, size, size, beta, d_B, size, d_y, 1, 1, d_x, 1, queue);    // x = βB'y + x (B' * y)
	magma_sgemv(MagmaNoTrans, size, size, alpha, d_B, size, d_x, 1, 0, d_w, 1, queue); // w = αBx (B * x)

	MAGMA_OP(magma_queue_sync(queue));
	exec_clock = omp_get_wtime() - exec_clock;

	magma_sgetvector(size, d_w, 1, w, 1, queue);

	magma_queue_destroy(queue);
	queue = NULL;

	MAGMA_OP(magma_free(d_A));
	MAGMA_OP(magma_free(d_B));
	MAGMA_OP(magma_free(d_u1));
	MAGMA_OP(magma_free(d_u2));
	MAGMA_OP(magma_free(d_v1));
	MAGMA_OP(magma_free(d_v2));
	MAGMA_OP(magma_free(d_w));
	MAGMA_OP(magma_free(d_x));
	MAGMA_OP(magma_free(d_y));
	MAGMA_OP(magma_free(d_z));
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

	/*1. Declaration of host matrices*/
	float *A;
	float *u1, *u2;
	float *v2, *v1;
	float *w, *y, *z;

	MAGMA_OP(magma_smalloc_pinned(&A, size * size));
	MAGMA_OP(magma_smalloc_pinned(&u1, size));
	MAGMA_OP(magma_smalloc_pinned(&u2, size));
	MAGMA_OP(magma_smalloc_pinned(&v2, size));
	MAGMA_OP(magma_smalloc_pinned(&v1, size));
	MAGMA_OP(magma_smalloc_pinned(&w, size));
	MAGMA_OP(magma_smalloc_pinned(&y, size));
	MAGMA_OP(magma_smalloc_pinned(&z, size));

	init_array(A, u1, u2, v1, v2, y, z, size);
	gemver_cuda(A, u1, u2, v1, v2, w, y, z, alpha, beta, size, GPU);
	norm_calc(w, size);

	MAGMA_OP(magma_free_pinned(A));
	MAGMA_OP(magma_free_pinned(u1));
	MAGMA_OP(magma_free_pinned(u2));
	MAGMA_OP(magma_free_pinned(v1));
	MAGMA_OP(magma_free_pinned(v1));
	MAGMA_OP(magma_free_pinned(w));
	MAGMA_OP(magma_free_pinned(y));
	MAGMA_OP(magma_free_pinned(z));

	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
