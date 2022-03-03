#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"

#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

#define SEED    6834723
#define EPSILON 0.0001

Ctrl_NewType(float);

double main_clock;
double exec_clock;

CTRL_KERNEL_CHAR(Mult, MANUAL, BLOCKSIZE, BLOCKSIZE);

CTRL_KERNEL(Mult, CUDA, DEFAULT, KHitTile_float matrix_result, KHitTile_float matrix_a, KHitTile_float matrix_b, {
	__shared__ float tile_a[BLOCKSIZE][BLOCKSIZE];
	__shared__ float tile_b[BLOCKSIZE][BLOCKSIZE];

	float tmp = 0.0;
	int   idx;

	for (int sub = 0; sub < gridDim.x; ++sub) {
		idx = thread_id_x * hit_tileDimCard(matrix_a, 0) + sub * BLOCKSIZE + threadIdx.x;
		if (idx >= hit_tileDimCard(matrix_a, 0) * hit_tileDimCard(matrix_a, 1)) {
			tile_a[threadIdx.y][threadIdx.x] = 0;
		} else {
			tile_a[threadIdx.y][threadIdx.x] = hit(matrix_a, idx);
		}
		idx = (sub * BLOCKSIZE + threadIdx.y) * hit_tileDimCard(matrix_a, 0) + thread_id_y;
		if (idx >= hit_tileDimCard(matrix_a, 0) * hit_tileDimCard(matrix_a, 1)) {
			tile_b[threadIdx.y][threadIdx.x] = 0;
		} else {
			tile_b[threadIdx.y][threadIdx.x] = hit(matrix_b, idx);
		}
		__syncthreads();

		for (int k = 0; k < BLOCKSIZE; ++k) {
			tmp += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
		}
		__syncthreads();
	}

	if (thread_id_x < hit_tileDimCard(matrix_a, 0) && thread_id_y < hit_tileDimCard(matrix_a, 1)) {
		hit(matrix_result, thread_id_x, thread_id_y) = tmp;
	}
});

float RandomFloat(float min, float max) {
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;
	float range  = max - min;
	return (random * range) + min;
}

CTRL_HOST_TASK(Init_Tiles, HitTile_float matrix_a, HitTile_float matrix_b, HitTile_float matrix_c) {
	srand(SEED);
	for (int j = 0; j < hit_tileDimCard(matrix_a, 1); j++) {
		float col_sum_a = 0;
		float col_sum_b = 0;
		for (int i = 0; i < hit_tileDimCard(matrix_a, 0); i++) {
			float a = RandomFloat(-(1 - col_sum_a) + EPSILON, 1 - col_sum_a - EPSILON);
			float b = RandomFloat(-(1 - col_sum_b) + EPSILON, 1 - col_sum_b - EPSILON);

			hit(matrix_a, i, j) = a;
			hit(matrix_b, i, j) = b;
			hit(matrix_c, i, j) = 0;
			col_sum_a += fabsf(a);
			col_sum_b += fabsf(b);
		}
	}
}

CTRL_HOST_TASK(Host_Compute, int ITER, double *p_sum, double *p_res, HitTile_float matrix, HitTile_float matrix_res) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host task");
	#endif //_PROFILING_ENABLED_

	double minimum = hit(matrix, 0, 0);
	double maximum = hit(matrix, 0, 0);

	for (int j = 0; j < hit_tileDimCard(matrix, 0); j++) {
		for (int k = 0; k < hit_tileDimCard(matrix, 1); k++) {
			if (minimum > hit(matrix, j, k)) {
				minimum = hit(matrix, j, k);
			}
			if (maximum < hit(matrix, j, k)) {
				maximum = hit(matrix, j, k);
			}
		}
	}

	for (int j = 0; j < hit_tileDimCard(matrix, 0); j++) {
		for (int k = 0; k < hit_tileDimCard(matrix, 1); k++) {
			hit(matrix, j, k) = hit(matrix, j, k) - minimum;
			hit(matrix, j, k) = hit(matrix, j, k) / maximum;
		}
	}

	p_sum[ITER] = 0;
	for (int j = 0; j < hit_tileDimCard(matrix, 0); j++) {
		for (int k = 0; k < hit_tileDimCard(matrix, 1); k++) {
			p_sum[ITER] += pow(hit(matrix, j, k), 2);
		}
	}
	p_res[ITER] = sqrt(p_sum[ITER]);

	for (int j = 0; j < hit_tileDimCard(matrix, 0); j++) {
		for (int k = 0; k < hit_tileDimCard(matrix, 1); k++) {
			hit(matrix_res, j, k) = hit(matrix, j, k) / p_res[ITER];
		}
	}

	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

CTRL_KERNEL_PROTO(Mult,
				  1, CUDA, DEFAULT, 3,
				  OUT, HitTile_float, matrix_result,
				  IN, HitTile_float, matrix_a,
				  IN, HitTile_float, matrix_b);

CTRL_HOST_TASK_PROTO(Init_Tiles, 3,
					 OUT, HitTile_float, matrix_a,
					 OUT, HitTile_float, matrix_b,
					 OUT, HitTile_float, matrix_c);

CTRL_HOST_TASK_PROTO(Host_Compute, 5,
					 INVAL, int, ITER,
					 INVAL, double *, p_sum,
					 INVAL, double *, p_res,
					 IN, HitTile_float, matrix,
					 INVAL, HitTile_float, matrix_res);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 6) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <device> <policy> <affinity>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int         SIZE     = atoi(argv[1]);
	int         N_ITER   = atoi(argv[2]);
	int         DEVICE   = atoi(argv[3]);
	Ctrl_Policy policy   = (Ctrl_Policy)atoi(argv[4]);
	int         host_aff = atoi(argv[5]);
	Ctrl_SetHostAffinity(host_aff);

	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, DEVICE);
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, ", cu_dev_prop.name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n POLICY %s", policy ? "Async" : "Sync");
	printf("\n HOST AFFINITY: %d", host_aff);
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
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	#ifdef _CTRL_QUEUE_
	__ctrl_block__(1, 1)
		#else
		__ctrl_block__(1, 0)
	#endif //_CTRL_QUEUE_
	{

		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CUDA, policy, DEVICE);

		HitTile_float matrix_a = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrix_b = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrix_c = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		HitTile_float matrix_tmp = hitTile(float, hitShapeSize(SIZE, SIZE));

		Ctrl_HostTask(ctrl, Init_Tiles, matrix_a, matrix_b, matrix_c);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		for (int i = 0; i < N_ITER; i++) {
			if ((i % 2) == 0) {
				Ctrl_Launch(ctrl, Mult, threads, CTRL_THREAD_NULL, matrix_c, matrix_a, matrix_b);
				Ctrl_HostTask(ctrl, Host_Compute, i, p_sum, p_res, matrix_c, matrix_tmp);
			} else {
				Ctrl_Launch(ctrl, Mult, threads, CTRL_THREAD_NULL, matrix_b, matrix_a, matrix_c);
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
		printf("%lf, %lf, ", p_sum[N_ITER - 1], p_res[N_ITER - 1]);
		fflush(stdout);
		#else
		printf("\n ----------------------- NORM ----------------------- \n\n");
		fflush(stdout);
		for (int i = 0; i < N_ITER; i++) {
			printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]);
			fflush(stdout);
		}
		printf("\n ---------------------------------------------------- \n");
		fflush(stdout);
		#endif

		Ctrl_Free(ctrl, matrix_a, matrix_b, matrix_c);
		hit_tileFree(matrix_tmp);

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
