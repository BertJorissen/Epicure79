#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SEED 6834723
#define EPSILON 0.0001

double main_clock;
double exec_clock;

void Matrix_Mult(float *res, float *A, float *B, int size, int n_threads) {
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			res[i*size+j]=0;
	
	#pragma omp parallel for num_threads(n_threads)
	for (int ti=0; ti<size; ti+=BLOCKSIZE)
		for (int tj=0; tj<size; tj+=BLOCKSIZE)
			for (int tk=0; tk<size; tk+=BLOCKSIZE) {
				/* MULTIPLICACION DE BLOQUE x BLOQUE */
				int i_max = ((ti+BLOCKSIZE) > size) ? size : (ti+BLOCKSIZE);
				int j_max = ((tj+BLOCKSIZE) > size) ? size : (tj+BLOCKSIZE);
				int k_max = ((tk+BLOCKSIZE) > size) ? size : (tk+BLOCKSIZE);
				for (int i=ti; i<i_max; i++)
					for (int j=tj; j<j_max; j++) 
						for (int k=tk; k<k_max; k++) {
							/* NOTA: LOS INDICES tj,j Y tk,k ESTAN
								INVERTIDOS PARA SIMULAR LA REORDENACION
								DE LOS BUCLES tj <-> tk */
							res[i*size+k] += A[i*size+j] * B[j*size+k];
						}
			}
}

float RandomFloat(float min, float max) {
    assert(max > min); 
    float random = ((float) rand()) / (float) RAND_MAX;
    float range = max - min;  
    return (random*range) + min;
}

void Init_Tiles(float *matrix_a, float *matrix_b, float *matrix_c, int rows, int columns) {
	srand(SEED);
	for (int j = 0; j < columns; j++) {
		float col_sum_a=0;
		float col_sum_b=0;
		for (int i = 0; i < rows; i++) {
			float a=RandomFloat(-(1-col_sum_a)+EPSILON, 1-col_sum_a-EPSILON);
			float b=RandomFloat(-(1-col_sum_b)+EPSILON, 1-col_sum_b-EPSILON);
			matrix_a[i * columns + j] = a;
			matrix_b[i * columns + j] = b;
			matrix_c[i * columns + j] = 0;
			col_sum_a+=fabsf(a);
			col_sum_b+=fabsf(b);
		}
	}
}

void Host_Compute(int ITER, double *p_sum, double *p_res, float *matrix, float *matrix_res, int rows, int columns) {
	double minimum = matrix[0 * columns + 0];
	double maximum = matrix[0 * columns + 0];

	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			if (minimum > matrix[j * columns + k]) {
				minimum = matrix[j * columns + k];
			}
			if (maximum < matrix[j * columns + k]) {
				maximum = matrix[j * columns + k];
			}
		}
	}

	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			matrix_res[j * columns + k] = matrix[j * columns + k] - minimum;
			matrix_res[j * columns + k] = matrix_res[j * columns + k] / maximum;
		}
	}

	p_sum[ITER] = 0;
	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			p_sum[ITER] += pow(matrix_res[j * columns + k], 2);
		}
	}
	p_res[ITER] = sqrt(p_sum[ITER]);

	for (int j = 0; j < rows; j++) {
		for (int k = 0; k < columns; k++) {
			matrix_res[j * columns + k] = matrix_res[j * columns + k] / p_res[ITER];
		}
	}
}

int main(int argc, char const *argv[]) {
	main_clock = omp_get_wtime();

	// Simpler interface for square matrices (only one parameter)
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <matrixSize> <n_power> <n_threads>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE = atoi(argv[1]);
	int POWER = atoi(argv[2]);
	int THREADS = atoi(argv[3]);
	int MATRIX_SIZE = sizeof(float) * SIZE * SIZE;

	#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", POWER);
		printf("\n N_THREADS: %d", THREADS);
		printf("\n POLICY SYNC");
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	/*Variables*/
	double *p_res = (double *)malloc(POWER * sizeof(double));
	double *p_sum = (double *)malloc(POWER * sizeof(double));
	float *A = (float *)malloc(MATRIX_SIZE);
	float *B = (float *)malloc(MATRIX_SIZE);
	float *C = (float *)malloc(MATRIX_SIZE);
	float *matrix_tmp = (float *)malloc(MATRIX_SIZE);

	/*Init data*/
	Init_Tiles( A, B, C, SIZE, SIZE );
	
	/*Start timer*/
	exec_clock = omp_get_wtime();

	for (int i = 0; i < POWER; i++) {
		if ((i % 2) == 0) {
			Matrix_Mult(C, A, B, SIZE, THREADS);
			Host_Compute(i, p_sum, p_res, C, matrix_tmp, SIZE, SIZE);
		} else {
			Matrix_Mult(B, A, C, SIZE, THREADS);
			Host_Compute(i, p_sum, p_res, B, matrix_tmp, SIZE, SIZE);
		}
	}
	/* Stop timer */
	exec_clock = omp_get_wtime() - exec_clock;

	/* Calculate NORM */
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		for (int i = 0; i < POWER; i++) {
			printf("%lf, %lf, ", p_sum[i], p_res[i]);
		}
		fflush(stdout);
	#else	
		printf("\n ----------------------- NORM ----------------------- \n\n"); fflush(stdout);
		for (int i = 0; i < POWER; i++) {
			printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]); fflush(stdout);
		}
		printf("\n ---------------------------------------------------- \n"); fflush(stdout);
	#endif

	/* Free resources */
	free(p_res);
	free(p_sum);
	free(A);
	free(B);
	free(C);
	free(matrix_tmp);

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%lf, %lf\n", main_clock, exec_clock);
		fflush(stdout);
	#else
		printf("\n ----------------------- TIME ----------------------- \n\n");
		printf(" Clock main: %lf\n", main_clock);
		printf(" Clock exec: %lf\n", exec_clock);
		printf("\n ---------------------------------------------------- \n");
	#endif

	return 0;
}
