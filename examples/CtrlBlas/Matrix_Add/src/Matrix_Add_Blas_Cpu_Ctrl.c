/**
 * @file Matrix_Add_Blas_Cpu_Ctrl.c
 * @brief MatrixAdd: CtrlBlas CPU version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl_Blas.c"
#include "../../examples/Utils/ctrl_print_info.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SEED 6834723

double main_clock;
double exec_clock;

/* A. Host task to initialize the matrices */
CTRL_HOST_TASK(Init_Tiles, HitTile_float A, HitTile_float B) {
	srand(SEED);
	for (int i = 0; i < hit_tileDimCard(A, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(A, 1); j++) {
			hit(A, i, j) = 1.0; // (-1 + (2 * (((float)rand())/(float)RAND_MAX)));
			hit(B, i, j) = 2.0; // (-1 + (2 * (((float)rand())/(float)RAND_MAX)));
		}
	}
}

/* B. Host task to calculate and print the norm */
CTRL_HOST_TASK(Norm_calc, HitTile_float matrix) {
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

/* C. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Tiles, 2,
					 OUT, HitTile_float, A,
					 OUT, HitTile_float, B);

CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	// 1. Taking arguments
	if (argc != 4) {
		fprintf(stderr, "\nUsage: %s <numRows> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int         size     = atoi(argv[1]);
	int         mat_size = size * size;
	Ctrl_Policy policy   = atoi(argv[2]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[3];

	__ctrl_block__(ctrl_conf_file) {
		// 2. Create controller object
		PCtrl ctrl = Ctrl_Get(0);

		// Extra information for collecting results
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", size);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		Ctrl_PrintInfo();
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		// 3. Alloc data structures
		HitTile_float A = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size, size));
		HitTile_float B = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size, size));

		// 4. Initialize data structures
		Ctrl_HostTask(Init_Tiles, A, B);

		// 5. Sync and start timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		// 6. Launch the kernel, copy to device (if necessary) is implicit
		float alpha = 1.0;
		int   inc   = 1;
		Ctrl_Launch(ctrl, ctrl_saxpy, CTRL_THREAD_NULL, CTRL_THREAD_NULL, mat_size, alpha, A, inc, B, inc);

		// 7. Sync and stop timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		// 8. Calculate NORM, copy from device to host(if necessary) is implicit
		Ctrl_HostTask(Norm_calc, B);

		// 9. Free data structures
		Ctrl_Free(ctrl, A, B);

		// 10. Destroy the controller
		Ctrl_EndBlock();
	}

	Ctrl_Finalize();

	// 11. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
