#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl_Blas.c"

double main_clock;
double exec_clock;

// A. INITIALIZE MATRIX
CTRL_HOST_TASK(init_tiles, HitTile_float A) {
	for (int i = 0; i < hit_tileDimCard(A, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(A, 1); j++) {
			hit(A, i, j) = ((float)i * j) / hit_tileDimCard(A, 0);
		}
	}
}

// B. Calculate NORM
CTRL_HOST_TASK(norm_calc, HitTile_float A) {
	double resultado = 0, suma = 0;
	for (int i = 0; i < hit_tileDimCard(A, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(A, 1); j++) {
			#ifdef DEBUG
			printf("%f \n", hit(A, i, j));
			#endif
			suma += pow(hit(A, i, j), 2);
		}
	}
	resultado = sqrt(suma);
	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

CTRL_HOST_TASK_PROTO(init_tiles, 1, OUT, HitTile_float, A);

CTRL_HOST_TASK_PROTO(norm_calc, 1, IN, HitTile_float, A);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 8) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <n_threads> <device> <mem_transfers> <policy> <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE    = atoi(argv[1]);
	int N_ITER  = atoi(argv[2]);
	int THREADS = atoi(argv[3]);
	int p_numanodes[1];
	int n_numanodes           = 1;
	p_numanodes[0]            = atoi(argv[4]);
	bool        mem_transfers = atoi(argv[5]);
	Ctrl_Policy policy        = atoi(argv[6]);
	int         host_aff      = atoi(argv[7]);
	Ctrl_SetHostAffinity(host_aff);

	#ifndef _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n N_THREADS: %d", THREADS);
	printf("\n POLICY %s", policy ? "Async" : "Sync");
	printf("\n MEM_TRANSFERS: %s", mem_transfers ? "ON" : "OFF");
	printf("\n HOST AFFINITY: %d", host_aff);
	printf("\n DEVICE: %s", n_numanodes != 0 ? argv[4] : "NULL");
	#ifdef _CTRL_QUEUE_
	printf("\n QUEUES: ON");
	#else
	printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	__ctrl_block__(1, 1) {
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CPU, policy, THREADS, p_numanodes, n_numanodes, mem_transfers);

		HitTile_float matrix_a = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		Ctrl_HostTask(ctrl, init_tiles, matrix_a);

		Ctrl_Blas_Trans trans_N = CTRL_BLAS_NOTRANS;
		float           alpha   = 1.0;
		float           beta    = 0.0;

		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		for (int power = 0; power > N_ITER; power++) {
			Ctrl_Launch(ctrl, ctrl_sgemm, CTRL_THREAD_NULL, CTRL_THREAD_NULL, trans_N, trans_N, SIZE, SIZE, SIZE, alpha,
						matrix_a, SIZE, matrix_a, SIZE, beta, matrix_a, SIZE); // C = αAB + βC
		}

		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;
		Ctrl_HostTask(ctrl, norm_calc, matrix_a);

		Ctrl_Free(ctrl, matrix_a);

		Ctrl_Destroy(ctrl);
	}

	// Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
