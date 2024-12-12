/**
 * @file Chain_MatMult_Multidev_Ctrl.c
 * @brief Chain matrix multiplication: Ctrl multidevice version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../examples/Utils/ctrl_print_info.h"

#define SEED 6834723

#define mult_params 3, OUT, HitTile_float, matrix_result, IN, HitTile_float, matrix_a, IN, HitTile_float, matrix_b
#define init        1, OUT, HitTile_float, matrix
#define norm_calc   4, INVAL, int, ITER, INVAL, double *, p_sum, INVAL, double *, p_res, IN, HitTile_float, matrix

#define SWAP(x, y, T)  \
	do {               \
		T SWAP = x;    \
		x      = y;    \
		y      = SWAP; \
	} while (0)

double main_clock;
double exec_clock;

/* Declare type for tiles */
Ctrl_NewType(float);

/* A. Characterization for the kernel */
CTRL_KERNEL_CHAR(Mult, MANUAL, BLOCKSIZE, BLOCKSIZE);
CTRL_KERNEL_CHAR(Norm_calc, MANUAL, 0);

/* C. Defining kernel prototypes */
CTRL_KERNEL_PROTO(Mult, 3, CUDA, DEFAULT, OPENCLGPU, DEFAULT, CPU, DEFAULT, mult_params);

CTRL_KERNEL_PROTO(Norm_calc, 1, CPU, DEFAULT, norm_calc);

/* D. Host task to initialize the matrix */
CTRL_HOST_TASK(Init_Matrix_Rand, CTRL_HPARAMS(init)) {
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			hit(matrix, i, j) = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
}

CTRL_HOST_TASK(Init_Matrix_Diag, CTRL_HPARAMS(init)) {
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		hit(matrix, i, i) = -1 + 2 * (float)rand() / (float)RAND_MAX;
	}
}

CTRL_HOST_TASK(Init_Matrix_Null, CTRL_HPARAMS(init)) {
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			hit(matrix, i, j) = 0;
		}
	}
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Matrix_Rand, init);
CTRL_HOST_TASK_PROTO(Init_Matrix_Null, init);
CTRL_HOST_TASK_PROTO(Init_Matrix_Diag, init);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	// 1. Taking arguments
	if (argc != 5) {
		fprintf(stderr, "\nUsage: %s <size> <n_iters> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int         SIZE   = atoi(argv[1]);
	int         N_ITER = atoi(argv[2]);
	Ctrl_Policy policy = (Ctrl_Policy)atoi(argv[3]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[4];

	double *p_res = (double *)malloc(N_ITER * sizeof(double));
	double *p_sum = (double *)malloc(N_ITER * sizeof(double));

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(ctrl_conf_file) {
		// 3. Get controller objects and print info
		int   n_ctrls = Ctrl_GetNCtrls();
		PCtrl ctrls[n_ctrls];
		for (int i = 0; i < n_ctrls; i++) {
			ctrls[i] = Ctrl_Get(i);
		}

		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_

		// Device info
		Ctrl_PrintInfo();

		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		// 4. Alloc data structures
		// list of tiles for the pipeline
		HitTile_float *mats       = (HitTile_float *)malloc(n_ctrls * sizeof(HitTile_float));
		HitTile_float *mats_const = (HitTile_float *)malloc((n_ctrls - 1) * sizeof(HitTile_float));
		#ifdef _2BUF
		HitTile_float *mats_aux = (HitTile_float *)malloc(n_ctrls * sizeof(HitTile_float));
		#endif // _2BUF

		srand(SEED);

		mats[0] = Ctrl_DomainAlloc(ctrls[0], float, hitShapeSize(SIZE, SIZE));
		Ctrl_HostTask(Init_Matrix_Null, mats[0]);

		#ifdef _2BUF
		mats_aux[0] = Ctrl_DomainAlloc(ctrls[0], float, hitShapeSize(SIZE, SIZE));
		Ctrl_HostTask(Init_Matrix_Null, mats_aux[0]);
		#endif // _2BUF
		for (int i = 1; i < n_ctrls; i++) {
			mats[i] = Ctrl_DomainAlloc(ctrls[i - 1], float, hitShapeSize(SIZE, SIZE));
			Ctrl_Alloc(ctrls[i], mats[i]);

			#ifdef _2BUF
			mats_aux[i] = Ctrl_DomainAlloc(ctrls[i - 1], float, hitShapeSize(SIZE, SIZE));
			Ctrl_Alloc(ctrls[i], mats_aux[i]);
			#endif // _2BUF

			mats_const[i - 1] = Ctrl_DomainAlloc(ctrls[i - 1], float, hitShapeSize(SIZE, SIZE));
			Ctrl_HostTask(Init_Matrix_Rand, mats_const[i - 1]);
			Ctrl_MoveTo(ctrls[i - 1], mats_const[i - 1]);
		}

		// 6. Sync and start timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		// 7. Launch the operations, copies (if necessary) are implicit
		for (int i = 0; i < N_ITER; i++) {
			Ctrl_HostTask(Init_Matrix_Diag, mats[0]);
			for (int j = 0; j < n_ctrls - 1; j++) {
				Ctrl_Launch(ctrls[j], Mult, threads, CTRL_THREAD_NULL, mats[j + 1], mats[j], mats_const[j]);
			}
			Ctrl_Launch(ctrls[n_ctrls - 1], Norm_calc, CTRL_THREAD_NULL, CTRL_THREAD_NULL, i, p_sum, p_res, mats[n_ctrls - 1]);
			#ifdef _2BUF
			SWAP(mats, mats_aux, HitTile_float *);
			#endif // _2BUF
		}

		// 8. Sync and stop timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;

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

		// This should not be neccessary but there are weird issues with the freeing of shared tiles in opencl ctrls
		Ctrl_Synchronize();

		// 10. Free data structures
		for (int i = 0; i < n_ctrls - 1; i++) {
			Ctrl_Free(ctrls[i], mats[i], mats[i + 1]);
			#ifdef _2BUF
			Ctrl_Free(ctrls[i], mats_aux[i], mats_aux[i + 1]);
			#endif // _2BUF
		}
		Ctrl_Free(ctrls[n_ctrls - 1], mats[n_ctrls - 1]);
		#ifdef _2BUF
		Ctrl_Free(ctrls[n_ctrls - 1], mats_aux[n_ctrls - 1]);
		#endif // _2BUF

		free(mats);

		// 11. Destroy the controller
		Ctrl_EndBlock();
	}

	// 12. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Finalize();
	return 0;
}
