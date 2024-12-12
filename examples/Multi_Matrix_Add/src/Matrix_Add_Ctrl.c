/**
 * @file Matrix_Add_Ctrl.c
 * @brief Multi Matrix Add: Ctrl multi device version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEED 6834723

double main_clock;
double exec_clock;

/* Declare type for tiles */
Ctrl_NewType(float);

/* A. Characterization for the kernel */
CTRL_KERNEL_CHAR(Add, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);

CTRL_KERNEL(Add, CUDALIB, MAGMA, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C, {
	for (int k = 0; k < n_iter; k++) {
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, A.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, B.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
	}
});

CTRL_KERNEL(Add, CUDALIB, CUBLAS, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C, {
	const float alpha = 1.0f;
	for (int k = 0; k < n_iter; k++) {
		cublasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, A.data, 1, C.data, 1);
		cublasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, B.data, 1, C.data, 1);
	}
});

/* C. Defining kernel prototypes */
CTRL_KERNEL_PROTO(Add,
				  3, GENERIC, DEFAULT,
				  CUDALIB, MAGMA,
				  CUDALIB, CUBLAS,
				  4,
				  INVAL, int, n_iter,
				  IN, HitTile_float, A,
				  IN, HitTile_float, B,
				  IO, HitTile_float, C);

/* D. Host task to initialize the matrices */
CTRL_HOST_TASK(Init_Tiles, HitTile_float matrixA, HitTile_float matrixB, HitTile_float matrixC) {
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

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Tiles, 3,
					 OUT, HitTile_float, matrixA,
					 OUT, HitTile_float, matrixB,
					 OUT, HitTile_float, matrixC);

CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 5) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int         SIZE   = atoi(argv[1]);
	int         N_ITER = atoi(argv[2]);
	Ctrl_Policy policy = (Ctrl_Policy)atoi(argv[3]);
	Ctrl_SetPolicy(policy);
	char *ctrl_config_file = argv[4];

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(ctrl_config_file) {
		// 3. Create controller object
		PCtrl ctrl_cu0  = Ctrl_Get(0);
		PCtrl ctrl_cu1  = Ctrl_Get(1);
		PCtrl ctrl_ocl0 = Ctrl_Get(2);
		PCtrl ctrl_ocl1 = Ctrl_Get(3);
		PCtrl ctrl_cpu0 = Ctrl_Get(4);
		PCtrl ctrl_cpu1 = Ctrl_Get(5);

		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		// printf("\n CU_DEV 0: %s", cu_dev_prop.name);
		// printf("\n CU_DEV 1: %s", cu_dev_prop.name);
		// printf("\n OCL_PLATFORM: %s", platform_name);
		// printf("\n OCL_DEV 0: %s", device_name);
		// printf("\n OCL_DEV 1: %s", device_name);
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);

		// 4. Alloc data structures
		HitTile_float matrixA_cu0 = Ctrl_DomainAlloc(ctrl_cu0, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB_cu0 = Ctrl_DomainAlloc(ctrl_cu0, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC_cu0 = Ctrl_DomainAlloc(ctrl_cu0, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrixA_cu1 = Ctrl_DomainAlloc(ctrl_cu1, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB_cu1 = Ctrl_DomainAlloc(ctrl_cu1, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC_cu1 = Ctrl_DomainAlloc(ctrl_cu1, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrixA_cpu = Ctrl_DomainAlloc(ctrl_cpu0, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB_cpu = Ctrl_DomainAlloc(ctrl_cpu0, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC_cpu = Ctrl_DomainAlloc(ctrl_cpu0, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrixA_cpu1 = Ctrl_DomainAlloc(ctrl_cpu1, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB_cpu1 = Ctrl_DomainAlloc(ctrl_cpu1, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC_cpu1 = Ctrl_DomainAlloc(ctrl_cpu1, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrixA_ocl0 = Ctrl_DomainAlloc(ctrl_ocl0, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB_ocl0 = Ctrl_DomainAlloc(ctrl_ocl0, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC_ocl0 = Ctrl_DomainAlloc(ctrl_ocl0, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrixA_ocl1 = Ctrl_DomainAlloc(ctrl_ocl1, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB_ocl1 = Ctrl_DomainAlloc(ctrl_ocl1, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC_ocl1 = Ctrl_DomainAlloc(ctrl_ocl1, float, hitShapeSize(SIZE, SIZE));

		// 5. Initialize data structures
		Ctrl_HostTask(Init_Tiles, matrixA_cu0, matrixB_cu0, matrixC_cu0);
		Ctrl_HostTask(Init_Tiles, matrixA_cu1, matrixB_cu1, matrixC_cu1);
		Ctrl_HostTask(Init_Tiles, matrixA_cpu, matrixB_cpu, matrixC_cpu);
		Ctrl_HostTask(Init_Tiles, matrixA_cpu1, matrixB_cpu1, matrixC_cpu1);
		Ctrl_HostTask(Init_Tiles, matrixA_ocl0, matrixB_ocl0, matrixC_ocl0);
		Ctrl_HostTask(Init_Tiles, matrixA_ocl1, matrixB_ocl1, matrixC_ocl1);

		// 6. Sync and start timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		// 7. Launch the kernel, copy to device (if necessary) is implicit
		Ctrl_Launch(ctrl_cu0, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA_cu0, matrixB_cu0, matrixC_cu0);
		Ctrl_Launch(ctrl_cu1, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA_cu1, matrixB_cu1, matrixC_cu1);
		Ctrl_Synchronize();
		Ctrl_Launch(ctrl_cpu0, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA_cpu, matrixB_cpu, matrixC_cpu);
		Ctrl_Launch(ctrl_cpu1, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA_cpu1, matrixB_cpu1, matrixC_cpu1);
		Ctrl_Launch(ctrl_ocl0, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA_ocl0, matrixB_ocl0, matrixC_ocl0);
		Ctrl_Launch(ctrl_ocl1, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA_ocl1, matrixB_ocl1, matrixC_ocl1);

		// 8. Sync and stop timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;

		// 9. Calculate NORM, copy from device to host(if necessary) is implicit
		Ctrl_HostTask(Norm_calc, matrixC_cu0);
		Ctrl_HostTask(Norm_calc, matrixC_cu1);
		Ctrl_Synchronize();
		Ctrl_HostTask(Norm_calc, matrixC_cpu);
		Ctrl_HostTask(Norm_calc, matrixC_cpu1);
		Ctrl_HostTask(Norm_calc, matrixC_ocl0);
		Ctrl_HostTask(Norm_calc, matrixC_ocl1);

		// 10. Free data structures
		Ctrl_Free(ctrl_cu0, matrixA_cu0, matrixB_cu0, matrixC_cu0);
		Ctrl_Free(ctrl_cu1, matrixA_cu1, matrixB_cu1, matrixC_cu1);
		Ctrl_Free(ctrl_cpu0, matrixA_cpu, matrixB_cpu, matrixC_cpu);
		Ctrl_Free(ctrl_cpu1, matrixA_cpu1, matrixB_cpu1, matrixC_cpu1);
		Ctrl_Free(ctrl_ocl0, matrixA_ocl0, matrixB_ocl0, matrixC_ocl0);
		Ctrl_Free(ctrl_ocl1, matrixA_ocl1, matrixB_ocl1, matrixC_ocl1);

		// 11. Destroy the controller
		Ctrl_Synchronize();
		Ctrl_EndBlock();
	}

	// 12. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
