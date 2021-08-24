#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "Ctrl.h"

#define SEED 6834723
#define EPSILON 0.0001

Ctrl_NewType( float );

double main_clock;
double exec_clock;

CTRL_KERNEL_CHAR(Mult, MANUAL, BLOCKSIZE, BLOCKSIZE, BLOCKSIZE);
CTRL_KERNEL_CHAR(Reset, MANUAL, BLOCKSIZE, BLOCKSIZE);

CTRL_KERNEL(Mult, CPU, DEFAULT, KHitTile_float matrix_result, KHitTile_float matrix_a, KHitTile_float matrix_b,
{
	// using the same size to address all matrixes for compiler optimization issues
	int size = hit_tileDimCard(matrix_result, 0);
	matrix_result.data[thread_id_x * size + thread_id_z] += matrix_a.data[thread_id_x * size + thread_id_y] * matrix_b.data[thread_id_y * size + thread_id_z];
});

CTRL_KERNEL(Reset, CPU, DEFAULT, KHitTile_float matrix,
{
	hit(matrix, thread_id_x, thread_id_y) = 0;
});

float RandomFloat(float min, float max) {
    assert(max > min); 
    float random = ((float) rand()) / (float) RAND_MAX;
    float range = max - min;  
    return (random*range) + min;
}

CTRL_HOST_TASK(Init_Tiles, HitTile_float matrix_a, HitTile_float matrix_b, HitTile_float matrix_c) {
	srand(SEED);
	for (int j = 0; j < hit_tileDimCard( matrix_a, 1 ); j++) {
		float col_sum_a=0;
		float col_sum_b=0;
		for (int i = 0; i < hit_tileDimCard( matrix_a, 0 ); i++) {
			float a=RandomFloat(-(1-col_sum_a)+EPSILON, 1-col_sum_a-EPSILON);
			float b=RandomFloat(-(1-col_sum_b)+EPSILON, 1-col_sum_b-EPSILON);
			hit(matrix_a, i, j) = a;
			hit(matrix_b, i, j) = b;
			hit(matrix_c, i, j) = 0;
			col_sum_a+=fabsf(a);
			col_sum_b+=fabsf(b);
		}
	}
}

CTRL_HOST_TASK(Host_Compute, int ITER, double *p_sum, double *p_res, HitTile_float matrix, HitTile_float matrix_res) {
	double minimum = hit(matrix, 0, 0);
	double maximum = hit(matrix, 0, 0);

	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			if (minimum > hit(matrix, j, k)) {
				minimum = hit(matrix, j, k);
			}
			if (maximum < hit(matrix, j, k)) {
				maximum = hit(matrix, j, k);
			}
		}
	}

	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			hit(matrix_res, j, k) = hit(matrix, j, k) - minimum;
			hit(matrix_res, j, k) = hit(matrix_res, j, k) / maximum; 
		}
	}

	p_sum[ITER] = 0;
	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			p_sum[ITER] += pow(hit(matrix_res, j, k), 2);
		}
	}
	p_res[ITER] = sqrt(p_sum[ITER]);

	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			hit(matrix_res, j, k) = hit(matrix_res, j, k) / p_res[ITER];
		}
	}				
}

CTRL_KERNEL_PROTO( Mult,
	1, CPU, DEFAULT, 3, 
	OUT, HitTile_float, matrix_result, 
	IN, HitTile_float, matrix_a, 
	IN, HitTile_float, matrix_b
);

CTRL_KERNEL_PROTO(Reset,
	1, CPU, DEFAULT, 1,
	OUT, HitTile_float, matrix);

CTRL_HOST_TASK_PROTO( Init_Tiles, 3,
	OUT, HitTile_float, matrix_a,
	OUT, HitTile_float, matrix_b, 
	OUT, HitTile_float, matrix_c
);

CTRL_HOST_TASK_PROTO( Host_Compute, 5,
	INVAL, int, ITER,
	INVAL, double *, p_sum,
	INVAL, double *, p_res, 
	IN, HitTile_float, matrix,
	OUT, HitTile_float, matrix_res
);

void usage(int argc, char **argv) {
	fprintf(stderr, "\nUsage: %s <size> <n_iter> <n_threads> <device> <mem_transfers> <policy> <host>\n", argv[0]);
	fprintf(stderr, "\t<size> - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<n_iter> - number of iterations\n");
	fprintf(stderr, "\t<n_threads> - number of threads\n");
	fprintf(stderr, "\t<device> - numa node for the device \n");
	fprintf(stderr, "\t<mem_transfers> - 0 for no mem transfers 1 for mem transfers\n");
	fprintf(stderr, "\t<policy> - 0 for sync or 1 for async\n");
	fprintf(stderr, "\t<host> - numa node for the host\n");
	fflush(stdout);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 8) {
		usage(argc, argv);
	}
	int SIZE = atoi(argv[1]);
	int N_ITER = atoi(argv[2]);
	int THREADS = atoi(argv[3]);
	int p_numanodes[1];
	int n_numanodes=1;
	p_numanodes[0]=atoi(argv[4]);
	bool mem_transfers=atoi(argv[5]);
	Ctrl_Policy policy=atoi(argv[6]);
	int host_aff = atoi(argv[7]);
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

	double *p_res = (double *)malloc(N_ITER * sizeof(double));
	double *p_sum = (double *)malloc(N_ITER * sizeof(double));

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE, SIZE);
	Ctrl_Thread threads2D;
	Ctrl_ThreadInit(threads2D, SIZE, SIZE);

	__ctrl_block__(1, 1)
	{
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CPU, policy, THREADS, p_numanodes, n_numanodes, mem_transfers);

		HitTile_float matrix_a = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrix_b = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrix_c = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrix_tmp = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		Ctrl_HostTask(ctrl, Init_Tiles, matrix_a, matrix_b, matrix_c);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		// Ctrl_MoveTo(ctrl, matrix_a, matrix_b, matrix_c);

		for (int i = 0; i < N_ITER; i++) {
			if ((i % 2) == 0) {
				Ctrl_Launch(ctrl, Reset, threads2D, CTRL_THREAD_NULL, matrix_c);
				Ctrl_Launch(ctrl, Mult, threads, CTRL_THREAD_NULL, matrix_c, matrix_a, matrix_b);
				// Ctrl_MoveFrom(ctrl, matrix_c);
				Ctrl_HostTask(ctrl, Host_Compute, i, p_sum, p_res, matrix_c, matrix_tmp);
			} else {
				Ctrl_Launch(ctrl, Reset, threads2D, CTRL_THREAD_NULL, matrix_b);
				Ctrl_Launch(ctrl, Mult, threads, CTRL_THREAD_NULL, matrix_b, matrix_a, matrix_c);
				// Ctrl_MoveFrom(ctrl, matrix_b);
				Ctrl_HostTask(ctrl, Host_Compute, i, p_sum, p_res, matrix_b, matrix_tmp);
			}
		}

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		/* PRINT RESULTS */
		#ifdef _CTRL_EXAMPLES_TEST_MODE_
			for (int i = 0; i < N_ITER; i++) {
				printf("%lf, %lf, ", p_sum[i], p_res[i]);
			}
			fflush(stdout);
		#elif _CTRL_EXAMPLES_EXP_MODE_
			printf("%lf, %lf, ", p_sum[N_ITER-1], p_res[N_ITER-1]);
			fflush(stdout);
		#else
			printf("\n ----------------------- NORM ----------------------- \n\n"); fflush(stdout);
			for (int i = 0; i < N_ITER; i++) {
				printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]); fflush(stdout);
			}
			printf("\n ---------------------------------------------------- \n"); fflush(stdout);
		#endif

		Ctrl_Free(ctrl, matrix_a, matrix_b, matrix_c, matrix_tmp);
		Ctrl_Destroy(ctrl);
	}

	free(p_sum);
	free(p_res);

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