/**
 * @file gemver_Blas_Cpu_Ref_MKL.c
 * @author Trasgo Group
 * @brief gemver: MKL native version
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

#include <math.h>
#include <mkl.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Alignment of memory to be allocated by mkl
#define MEM_ALIGNMENT 64

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

void gemver_cuda(float *A, float *u1, float *u2, float *v1, float *v2, float *w, float *y, float *z, const float alpha, const float beta, const int size, const int n_threads) {
	mkl_set_num_threads_local(n_threads);

	float *B = (float *)mkl_malloc(size * size * sizeof(float), MEM_ALIGNMENT);
	float *x = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);

	exec_clock = omp_get_wtime();

	cblas_scopy(size * size, A, 1, B, 1);                                                // Copy A to B
	cblas_sger(CblasRowMajor, size, size, 1, u1, 1, v1, 1, B, size);                     // B = u1*v1' + B
	cblas_sger(CblasRowMajor, size, size, 1, u2, 1, v2, 1, B, size);                     // B = u2*v2' + B
	cblas_scopy(size, z, 1, x, 1);                                                       // Copy z to x
	cblas_sgemv(CblasRowMajor, CblasTrans, size, size, beta, B, size, y, 1, 1, x, 1);    // x = βB'y + x (B' * y)
	cblas_sgemv(CblasRowMajor, CblasNoTrans, size, size, alpha, B, size, x, 1, 0, w, 1); // w = αBx (B * x)

	exec_clock = omp_get_wtime() - exec_clock;

	mkl_free(B);
	mkl_free(x);
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 5) {
		fprintf(stderr, "Usage: %s <matrixSize> <alpha> <beta> <n_threads>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int   size      = atoi(argv[1]);
	float alpha     = atof(argv[2]);
	float beta      = atof(argv[3]);
	int   n_threads = atoi(argv[4]);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d x %d", size, size);
	printf("\n ALPHA: %.2f", alpha);
	printf("\n BETA: %.2f", beta);
	printf("\n POLICY Sync");
	printf("\n N_THREADS: %d", n_threads);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	/*1. Declaration of host matrices*/
	float *A  = (float *)mkl_malloc(size * size * sizeof(float), MEM_ALIGNMENT);
	float *u1 = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);
	float *u2 = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);
	float *v2 = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);
	float *v1 = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);
	float *w  = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);
	float *y  = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);
	float *z  = (float *)mkl_malloc(size * sizeof(float), MEM_ALIGNMENT);

	init_array(A, u1, u2, v1, v2, y, z, size);
	gemver_cuda(A, u1, u2, v1, v2, w, y, z, alpha, beta, size, n_threads);
	norm_calc(w, size);

	mkl_free(A);
	mkl_free(u1);
	mkl_free(u2);
	mkl_free(v1);
	mkl_free(v2);
	mkl_free(w);
	mkl_free(y);
	mkl_free(z);

	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
