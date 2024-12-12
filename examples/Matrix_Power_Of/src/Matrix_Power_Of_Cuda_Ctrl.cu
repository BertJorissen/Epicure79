/**
 * @file Matrix_Power_Of_Cuda_Ctrl.cu
 * @brief MatrixPow: Ctrl CUDA version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"
#include "../../examples/Utils/ctrl_print_info.h"

#define SEED    6834723
#define EPSILON 0.0001

int SIZE;

Ctrl_NewType(float);

double main_clock;
double exec_clock;

CTRL_KERNEL_CHAR(Mult, MANUAL, BLOCKSIZE, BLOCKSIZE);

#define mult_params      3, OUT, HitTile_float, matrix_result, IN, HitTile_float, matrix_a, IN, HitTile_float, matrix_b
#define init_params      3, OUT, HitTile_float, matrix_a, OUT, HitTile_float, matrix_b, OUT, HitTile_float, matrix_c
#define host_task_params 5, INVAL, int, ITER, INVAL, double *, p_sum, INVAL, double *, p_res, IN, HitTile_float, matrix, INVAL, HitTile_float, matrix_res

CTRL_KERNEL(Mult, CUDA, DEFAULT, CTRL_KPARAMS(mult_params), {
	__shared__ float tile_a[BLOCKSIZE][BLOCKSIZE];
	__shared__ float tile_b[BLOCKSIZE][BLOCKSIZE];

	float tmp = 0.0;
	int   idx;

	for (int sub = 0; sub < gridDim.x; ++sub) {
		idx                              = thr_i * hit_tileDimCard(matrix_a, 0) + sub * BLOCKSIZE + threadIdx.x;
		tile_a[threadIdx.y][threadIdx.x] = hit(matrix_a, idx);
		idx                              = (sub * BLOCKSIZE + threadIdx.y) * hit_tileDimCard(matrix_a, 0) + thr_j;
		tile_b[threadIdx.y][threadIdx.x] = hit(matrix_b, idx);
		__syncthreads();

		for (int k = 0; k < BLOCKSIZE; ++k) {
			tmp += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
		}
		__syncthreads();
	}

	hit(matrix_result, thr_i, thr_j) = tmp;
});

CTRL_HOST_TASK(Init_Tiles, CTRL_HPARAMS(init_params)) {
	srand(SEED);
	for (int j = 0; j < SIZE; j++) {
		float col_sum_a = 0;
		for (int i = 0; i < SIZE; i++) {
			// generate random floats in a way matrixes don't turn into NaN
			float min    = -(1 - col_sum_a) + EPSILON;
			float max    = 1 - col_sum_a - EPSILON;
			float random = ((float)rand()) / (float)RAND_MAX;
			float range  = max - min;
			float value  = (random * range) + min;

			hit(matrix_a, i, j) = value;
			hit(matrix_b, i, j) = value;
			hit(matrix_c, i, j) = 0;
			col_sum_a += fabsf(value);
		}
	}
}

CTRL_HOST_TASK(Host_Compute, CTRL_HPARAMS(host_task_params)) {
	double minimum = hit(matrix, 0);
	double maximum = hit(matrix, 0);

	for (int i = 0; i < SIZE * SIZE; i++) {
		if (minimum > hit(matrix, i)) {
			minimum = hit(matrix, i);
		}
		if (maximum < hit(matrix, i)) {
			maximum = hit(matrix, i);
		}
	}

	for (int i = 0; i < SIZE * SIZE; i++) {
		hit(matrix, i) -= minimum;
		hit(matrix, i) /= maximum;
	}

	p_sum[ITER] = 0;
	for (int i = 0; i < SIZE * SIZE; i++) {
		p_sum[ITER] += pow(hit(matrix, i), 2);
	}
	p_res[ITER] = sqrt(p_sum[ITER]);

	for (int i = 0; i < SIZE * SIZE; i++) {
		hit(matrix_res, i) = hit(matrix, i) / p_res[ITER];
	}
}

CTRL_KERNEL_PROTO(Mult, 1, CUDA, DEFAULT, mult_params);
CTRL_HOST_TASK_PROTO(Init_Tiles, init_params);
CTRL_HOST_TASK_PROTO(Host_Compute, host_task_params);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	if (argc != 5) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	SIZE               = atoi(argv[1]);
	int         N_ITER = atoi(argv[2]);
	Ctrl_Policy policy = (Ctrl_Policy)atoi(argv[3]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[4];

	double *p_res = (double *)malloc(N_ITER * sizeof(double));
	double *p_sum = (double *)malloc(N_ITER * sizeof(double));

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		// Extra information for collecting results
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		Ctrl_PrintInfo();
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		HitShape      shape    = hitShapeSize(SIZE, SIZE);
		HitTile_float matrix_a = Ctrl_DomainAlloc(ctrl, float, shape);
		HitTile_float matrix_b = Ctrl_DomainAlloc(ctrl, float, shape);
		HitTile_float matrix_c = Ctrl_DomainAlloc(ctrl, float, shape);

		HitTile_float matrix_tmp = hitTile(float, shape);

		Ctrl_HostTask(Init_Tiles, matrix_a, matrix_b, matrix_c);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		for (int i = 0; i < N_ITER; i++) {
			HitTile_float matrix1, matrix2;
			matrix1 = (i % 2 == 0) ? matrix_c : matrix_b;
			matrix2 = (i % 2 == 0) ? matrix_b : matrix_c;

			Ctrl_Launch(ctrl, Mult, threads, CTRL_THREAD_NULL, matrix1, matrix_a, matrix2);
			Ctrl_HostTask(Host_Compute, i, p_sum, p_res, matrix1, matrix_tmp);
		}

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		/* PRINT RESULTS */
		#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%lf, %lf, ", p_sum[N_ITER - 1], p_res[N_ITER - 1]);
		#else // _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- NORM ----------------------- \n\n");
		for (int i = 0; i < N_ITER; i++) {
			printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]);
		}
		printf("\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		Ctrl_Free(ctrl, matrix_a, matrix_b, matrix_c);
		hit_tileFree(matrix_tmp);

		Ctrl_EndBlock();
	}

	free(p_sum);
	free(p_res);

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_ // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Finalize();
	return EXIT_SUCCESS;
}
