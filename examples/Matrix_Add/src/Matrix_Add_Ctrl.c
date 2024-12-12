/**
 * @file Matrix_Add_Cuda_Ctrl.c
 * @brief MatrixAdd: Ctrl version with generic kernel for all architectures and some lib-kernels
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "../../examples/Utils/ctrl_print_info.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEED 6834723

double main_clock;
double exec_clock;

/* C. Defining kernel prototypes */
#ifdef _CTRL_MATRIX_ADD_GENERIC_KERNEL_
#warning "Generic kernel"
#include "proto_generic.h"
#else
#warning "Lib kernels"
#include "proto_libs.h"
#endif
CTRL_KERNEL_CHAR(Add, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);

/* D. Host task to initialize the matrices */
#define init_params 3, OUT, HitTile_float, matrixA, OUT, HitTile_float, matrixB, OUT, HitTile_float, matrixC

CTRL_HOST_TASK(Init_Tiles, CTRL_HPARAMS(init_params)) {
	srand(SEED);
	for (int i = 0; i < hit_tileDimCard(matrixA, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrixA, 1); j++) {
			hit(matrixA, i, j) = 1.0; // (-1 + (2 * (((float)rand())/(float)RAND_MAX)));
			hit(matrixB, i, j) = 2.0; // (-1 + (2 * (((float)rand())/(float)RAND_MAX)));
			hit(matrixC, i, j) = 0.0;
		}
	}
}

/* E. Host task to calculate and print the norm */
#define norm_params 1, IN, HitTile_float, matrix

CTRL_HOST_TASK(Norm_calc, CTRL_HPARAMS(norm_params)) {
	double resultado = 0;
	double suma      = 0;
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			suma += pow(hit(matrix, i, j), 2);
		}
	}
	resultado = sqrt(suma);

	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Tiles, init_params);
CTRL_HOST_TASK_PROTO(Norm_calc, norm_params);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	Ctrl_Init(&argc, &argv);

	// 1. Taking arguments
	if (argc != 5) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int         SIZE   = atoi(argv[1]);
	int         N_ITER = atoi(argv[2]);
	Ctrl_Policy policy = (Ctrl_Policy)atoi(argv[3]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[4];

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(ctrl_conf_file) {
		// 3. Create controller object
		PCtrl ctrl = Ctrl_Get(0);

		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		Ctrl_PrintInfo();
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		// 4. Alloc data structures
		HitTile_float matrixA = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		// 5. Initialize data structures
		Ctrl_HostTask(Init_Tiles, matrixA, matrixB, matrixC);

		// 6. Sync and start timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		// 7. Launch the kernel, copy to device (if necessary) is implicit
		Ctrl_Launch(ctrl, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA, matrixB, matrixC);

		// 8. Sync and stop timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		// 9. Calculate NORM, copy from device to host(if necessary) is implicit
		Ctrl_HostTask(Norm_calc, matrixC);

		// 10. Free data structures
		Ctrl_Free(ctrl, matrixA, matrixB, matrixC);

		// 11. Destroy the controller
		Ctrl_EndBlock();
	}

	Ctrl_Finalize();

	// 12. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
