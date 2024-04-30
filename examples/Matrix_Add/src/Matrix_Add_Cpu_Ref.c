/**
 * @file Matrix_Add_Cpu_Ref.c
 * @author Trasgo Group
 * @brief MatrixAdd: Native CPU version
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
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

double main_clock;
double exec_clock;

/* A. Perform the adition */
void add(float *matrixA, float *matrixB, float *matrixC, int size, int n_threads, int n_iter) {
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int iter = 0; iter < n_iter; iter++) {
				matrixC[i * size + j] =
					matrixA[i * size + j] +
					matrixB[i * size + j] +
					matrixC[i * size + j];
			}
		}
	}
}

/* B. Initialize matrices */
void init_matrix(float *matrixA, float *matrixB, float *matrixC, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrixA[i * size + j] = 1;
			matrixB[i * size + j] = 2;
			matrixC[i * size + j] = 0;
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
	if (argc != 4) {
		fprintf(stderr, "\nUsage: %s <numRows> <numIter> <numThreads>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE      = atoi(argv[1]);
	int N_ITER    = atoi(argv[2]);
	int N_THREADS = atoi(argv[3]);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n N_THREADS: %d", N_THREADS);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	// 2. Allocate data structures
	float *matrixA = (float *)malloc(SIZE * SIZE * sizeof(float));
	float *matrixB = (float *)malloc(SIZE * SIZE * sizeof(float));
	float *matrixC = (float *)malloc(SIZE * SIZE * sizeof(float));

	// 3. Initialize data structures
	init_matrix(matrixA, matrixB, matrixC, SIZE);

	// 4. Start timer
	exec_clock = omp_get_wtime();

	// 5. Perform the adition
	add(matrixA, matrixB, matrixC, SIZE, N_THREADS, N_ITER);

	// 6. Stop the timer
	exec_clock = omp_get_wtime() - exec_clock;

	// 7. Calculate the norm
	norm_calc(matrixC, SIZE);

	// 8. Free data structures
	free(matrixA);
	free(matrixB);
	free(matrixC);

	// 9. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
