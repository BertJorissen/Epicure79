/**
 * @file Chain_MatMult_Multidev_Ctrl.c
 * @author Trasgo Group
 * @brief Chain matrix multiplication: Ctrl multidevice version
 * @version 4.0
 * @date 2021-07-31
 *
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
 */

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
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Init tile rand");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Init tile rand");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			hit(matrix, i, j) = -1 + 2 * (float)rand() / (float)RAND_MAX;
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

CTRL_HOST_TASK(Init_Matrix_Diag, CTRL_HPARAMS(init)) {
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Init tile diag");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Init tile diag");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		hit(matrix, i, i) = -1 + 2 * (float)rand() / (float)RAND_MAX;
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

CTRL_HOST_TASK(Init_Matrix_Null, CTRL_HPARAMS(init)) {
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Init tile null");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Init tile null");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			hit(matrix, i, j) = 0;
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
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_

		// 3. Get controller objects and print info
		int   n_ctrls = Ctrl_GetNCtrls();
		PCtrl ctrls[n_ctrls];
		for (int i = 0; i < n_ctrls; i++) {
			ctrls[i] = Ctrl_Get(i);

			#ifndef _CTRL_EXAMPLES_EXP_MODE_
			Ctrl_Info info = Ctrl_GetInfo(ctrls[i]);
			printf("\n\n CTRL %d TYPE: %s", i, info.type);
			// TODO @seralpa switch to check for types?
			printf("\n PLATFORM: %s", info.platform_name);
			printf("\n DEVICE: %s", info.device_name);
			printf("\n N_THREADS: %d", info.n_threads);
			printf("\n MEM_MOVES: %s", info.mem_transfers ? "ON" : "OFF");
			printf("\n NUMA RANGE: %d-%d", info.numa_range_min, info.numa_range_max);
			#endif // _CTRL_EXAMPLES_EXP_MODE_
		}
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
		#endif // _CTRL_EXAMPLES_EXP_MODE_

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
	printf("\n ---------------------- TIMERS ---------------------- \n");
	printf("Clock main: %lf\n", main_clock);
	printf("Clock exec: %lf\n", exec_clock);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Finalize();
	return 0;
}
