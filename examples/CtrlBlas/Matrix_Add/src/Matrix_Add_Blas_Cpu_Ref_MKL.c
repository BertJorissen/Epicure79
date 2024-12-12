/**
 * @file Matrix_Add_Blas_Cpu_Ref_MKL.c
 * @brief MatrixAdd: Native MKL version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
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
