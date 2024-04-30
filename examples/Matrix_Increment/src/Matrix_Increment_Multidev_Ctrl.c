#include "Ctrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _PROFILING_ENABLED_
#ifdef _CTRL_ARCH_CUDA_
#include "nvToolsExt.h"
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include <roctx.h>
#endif // _CTRL_ARCH_OPENCL_GPU_
#endif //_PROFILING_ENABLED_

#define SEED 6834723

#define hit_tileSwap(a, b)               \
	{                                    \
		HitTile tmp    = *(HitTile *)&a; \
		a              = b;              \
		*(HitTile *)&b = tmp;            \
	}

double main_clock;
double exec_clock;

/* Declare type for tiles */
Ctrl_NewType(float);

/* A. Characterization for the kernel */
CTRL_KERNEL_CHAR(Increment, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);
CTRL_KERNEL_CHAR(Norm_calc, MANUAL, 0);

/* C. Defining kernel prototypes */
CTRL_KERNEL_PROTO(Increment,
				  1, GENERIC, DEFAULT,
				  3,
				  INVAL, int, n,
				  IN, HitTile_float, input,
				  OUT, HitTile_float, output);

CTRL_KERNEL_PROTO(Norm_calc, 1, CPU, DEFAULT, 1, IN, HitTile_float, matrix);

/* D. Host task to initialize the matrix */
CTRL_HOST_TASK(Init_Matrix, HitTile_float matrix, int n) {
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Init tile");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Init tile");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

	srand(SEED);
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			hit(matrix, i, j) = n;
		}
	}

	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePop();
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePop();
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_
}

/* E. Host task to calculate and print the norm */
CTRL_HOST_TASK(Norm_calc, HitTile_float matrix) {
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Norm calc");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Norm calc");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

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

	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePop();
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePop();
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Matrix, 2,
					 OUT, HitTile_float, matrix,
					 INVAL, int, n);

CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);

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

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(ctrl_conf_file) {
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", policy ? "Async" : "Sync");

		// 3. Get controller objects and print info
		int   n_ctrls = Ctrl_GetNCtrls();
		PCtrl ctrls[n_ctrls];
		for (int i = 0; i < n_ctrls; i++) {
			ctrls[i] = Ctrl_Get(i);

			Ctrl_Info info = Ctrl_GetInfo(ctrls[i]);
			printf("\n\n CTRL %d TYPE: %s", i, info.type);
			// TODO @seralpa switch to check for types?
			printf("\n PLATFORM: %s", info.platform_name);
			printf("\n DEVICE: %s", info.device_name);
			printf("\n N_THREADS: %d", info.n_threads);
			printf("\n MEM_MOVES: %s", info.mem_transfers ? "ON" : "OFF");
			printf("\n NUMA RANGE: %d-%d", info.numa_range_min, info.numa_range_max);
		}
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);

		// 4. Alloc data structures
		// list of tiles for the pipeline
		HitTile_float *mats     = (HitTile_float *)malloc(n_ctrls * sizeof(HitTile_float));
		HitTile_float *mats_aux = (HitTile_float *)malloc(n_ctrls * sizeof(HitTile_float));

		// what each device adds to the matrix
		int nums[n_ctrls - 1];

		mats[0]     = Ctrl_DomainAlloc(ctrls[0], float, hitShapeSize(SIZE, SIZE));
		mats_aux[0] = Ctrl_DomainAlloc(ctrls[0], float, hitShapeSize(SIZE, SIZE));
		for (int i = 0; i < n_ctrls - 1; i++) {
			mats[i + 1] = Ctrl_DomainAlloc(ctrls[i], float, hitShapeSize(SIZE, SIZE));
			Ctrl_Alloc(ctrls[i + 1], mats[i + 1]);

			mats_aux[i + 1] = Ctrl_DomainAlloc(ctrls[i], float, hitShapeSize(SIZE, SIZE));
			Ctrl_Alloc(ctrls[i + 1], mats_aux[i + 1]);

			nums[i] = i + 1;
		}

		// 6. Sync and start timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		// 7. Launch the operations, copies (if necessary) are implicit
		for (int i = 0; i < N_ITER; i++) {
			Ctrl_HostTask(Init_Matrix, mats[0], i);
			for (int j = 0; j < n_ctrls - 1; j++) {
				Ctrl_Launch(ctrls[j], Increment, threads, CTRL_THREAD_NULL, nums[j], mats[j], mats[j + 1]);
			}
			Ctrl_Launch(ctrls[n_ctrls - 1], Norm_calc, CTRL_THREAD_NULL, CTRL_THREAD_NULL, mats[n_ctrls - 1]);
			for (int j = 0; j < n_ctrls; j++) {
				hit_tileSwap(mats[j], mats_aux[j]);
			}
		}

		// 8. Sync and stop timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;

		// TODO @sergioalo norm info should be printed here ouside of exec clock

		// This should not be neccessary but there are weird issues with the freeing of shared tiles in opencl ctrls
		Ctrl_Synchronize();

		// 10. Free data structures
		for (int i = 0; i < n_ctrls - 1; i++) {
			Ctrl_Free(ctrls[i], mats[i], mats[i + 1]);
			Ctrl_Free(ctrls[i], mats_aux[i], mats_aux[i + 1]);
		}
		Ctrl_Free(ctrls[n_ctrls - 1], mats[n_ctrls - 1]);
		Ctrl_Free(ctrls[n_ctrls - 1], mats_aux[n_ctrls - 1]);

		free(mats);

		// 11. Destroy the controller
		Ctrl_EndBlock();
	}

	// 12. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	Ctrl_Finalize();
	return 0;
}
