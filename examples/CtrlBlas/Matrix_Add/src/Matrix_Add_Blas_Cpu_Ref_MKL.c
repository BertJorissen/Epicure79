/**
 * @file Matrix_Add_Blas_Cpu_Ref_MKL.c
 * @author Trasgo Group
 * @brief MatrixAdd: Native MKL version
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

// Alignment of memory to be allocated by mkl
#define MEM_ALIGNMENT 64

double main_clock;
double exec_clock;

/* B. Initialize matrices */
void init_matrix(float *A, float *B, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			A[i * size + j] = 1.0;
			B[i * size + j] = 2.0;
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

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 3) {
		fprintf(stderr, "\nUsage: %s <numRows> <numThreads>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size      = atoi(argv[1]);
	int n_threads = atoi(argv[2]);
	mkl_set_num_threads_local(n_threads);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", size);
	printf("\n N_THREADS: %d", n_threads);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	// 2. Allocate data structures
	float *A = (float *)mkl_malloc(size * size * sizeof(float), MEM_ALIGNMENT);
	float *B = (float *)mkl_malloc(size * size * sizeof(float), MEM_ALIGNMENT);

	// 3. Initialize data structures
	init_matrix(A, B, size);

	// 4. Start timer
	exec_clock = omp_get_wtime();

	// 5. Perform the adition
	cblas_saxpy(size * size, 1, A, 1, B, 1);

	// 6. Stop the timer
	exec_clock = omp_get_wtime() - exec_clock;

	// 7. Calculate the norm
	norm_calc(B, size);

	// 8. Free data structures
	mkl_free(A);
	mkl_free(B);

	// 9. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
