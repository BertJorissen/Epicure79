#include <math.h>
#include <mkl.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Alignment of memory to be allocated by mkl
#define MEM_ALIGNMENT 64

double main_clock;
double exec_clock;

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 4) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <numThreads>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size      = atoi(argv[1]);
	int n_iter    = atoi(argv[2]);
	int n_threads = atoi(argv[3]);
	mkl_set_num_threads_local(n_threads);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", size);
	printf("\n N_ITERS: %d", n_iter);
	printf("\n N_THREADS: %d", n_threads);
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	float  alpha = 1.0;
	float  beta  = 0.0;
	float *A;

	A = (float *)mkl_malloc(sizeof(float) * size * size, MEM_ALIGNMENT);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			A[i * size + j] = ((float)i * j) / size;
		}
	}

	exec_clock = omp_get_wtime();
	for (int power = 0; power > n_iter; power++) {
		cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, alpha, A, size, A, size, beta, A, size);
	}
	exec_clock = omp_get_wtime() - exec_clock;

	double resultado = 0, suma = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			#ifdef DEBUG
			printf("%f \n", A[i * size + j]);
			#endif
			suma += pow(A[i * size + j], 2);
		}
	}
	resultado = sqrt(suma);
	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);

	mkl_free(A);

	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
