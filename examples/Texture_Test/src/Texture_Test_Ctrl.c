#include "Ctrl.h"
#include "Texture_Test_Ctrl.h"
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
CTRL_KERNEL_CHAR(TexTest, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);

/* C. Defining kernel prototypes */
CTRL_KERNEL_PROTO(TexTest, 2, CUDA, DEFAULT, OPENCLGPU, DEFAULT, textest_params);

/* D. Host task to initialize the matrix */
CTRL_HOST_TASK(Init_Matrix, HitTile_float matrix) {
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Init tile");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Init tile");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			hit(matrix, i, j) = i * hit_tileDimCard(matrix, 1) + j;
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

CTRL_HOST_TASK(Print_matrix, HitTile_float matrix) {
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			printf("%.2f ", hit(matrix, i, j));
		}
		printf("\n");
	}
	fflush(stdout);
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Matrix, 1, OUT, HitTile_float, matrix);
CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);
CTRL_HOST_TASK_PROTO(Print_matrix, 1, IN, HitTile_float, matrix);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	// 1. Taking arguments
	if (argc != 4) {
		fprintf(stderr, "\nUsage: %s <size> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int         SIZE   = atoi(argv[1]);
	Ctrl_Policy policy = (Ctrl_Policy)atoi(argv[2]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[3];

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(ctrl_conf_file) {
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n POLICY %s", policy ? "Async" : "Sync");

		// 3. Get controller object and print info
		PCtrl ctrl = Ctrl_Get(0);

		Ctrl_Info info = Ctrl_GetInfo(ctrl);
		printf("\n\n CTRL TYPE: %s", info.type);
		printf("\n PLATFORM: %s", info.platform_name);
		printf("\n DEVICE: %s", info.device_name);
		printf("\n N_THREADS: %d", info.n_threads);
		printf("\n MEM_MOVES: %s", info.mem_transfers ? "ON" : "OFF");
		printf("\n NUMA RANGE: %d-%d", info.numa_range_min, info.numa_range_max);
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);

		// 4. Alloc data structures
		HitShape      shape = hitShapeSize(SIZE, SIZE);
		HitTile_float A     = Ctrl_DomainAlloc(ctrl, float, shape, CTRL_MEM_ALIGNED);
		HitTile_float B     = Ctrl_DomainAlloc(ctrl, float, shape, CTRL_MEM_ALIGNED);

		Ctrl_TexDesc tex_desc      = {0};
		tex_desc.normalized_coords = false;
		tex_desc.read_mode         = CTRL_TEX_READMODE_ELEMTYPE;
		tex_desc.addr_mode[0]      = CTRL_TEX_ADDRMODE_CLAMP;

		Ctrl_CreateTex(ctrl, A, tex_desc);

		// 6. Sync and start timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		// 7. Launch the operations, copies (if necessary) are implicit
		Ctrl_HostTask(Init_Matrix, A);
		Ctrl_Launch(ctrl, TexTest, threads, CTRL_THREAD_NULL, A, B);
		Ctrl_HostTask(Print_matrix, B);
		Ctrl_HostTask(Norm_calc, B);

		// 8. Sync and stop timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;

		// This should not be neccessary but there are weird issues with the freeing of shared tiles in opencl ctrls
		Ctrl_Synchronize();

		// 10. Free data structures
		Ctrl_Free(ctrl, A, B);

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
