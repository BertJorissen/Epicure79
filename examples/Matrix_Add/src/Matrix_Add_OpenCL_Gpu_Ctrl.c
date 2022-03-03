#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"

#define SEED 6834723

/* Declare type for tiles */
Ctrl_NewType(float);

double main_clock;
double exec_clock;

/* A. Characterization for the kernel */
CTRL_KERNEL_CHAR(Add, MANUAL, LOCAL_SIZE_0, LOCAL_SIZE_1);

/* B. GENERIC Kernel implementation */
CTRL_KERNEL(Add, GENERIC, DEFAULT, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C, {
	for (int k = 0; k < n_iter; k++) {
		hit(C, thread_id_x, thread_id_y) =
			hit(C, thread_id_x, thread_id_y) +
			hit(A, thread_id_x, thread_id_y) +
			hit(B, thread_id_x, thread_id_y);
	}
});

/* C. Defining kernel prototypes */
CTRL_KERNEL_PROTO(Add,
				  1, GENERIC, DEFAULT, 4,
				  INVAL, int, n_iter,
				  IN, HitTile_float, A,
				  IN, HitTile_float, B,
				  OUT, HitTile_float, C);

/* D. Host task to initialize the matrices */
CTRL_HOST_TASK(Init_Tiles, HitTile_float matrixA, HitTile_float matrixB, HitTile_float matrixC) {
	srand(SEED);
	for (int i = 0; i < hit_tileDimCard(matrixA, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrixA, 1); j++) {
			hit(matrixA, i, j) = 1.0; //(-1 + (2 * (((float)rand())/RAND_MAX)))
			hit(matrixB, i, j) = 1.0; //(-1 + (2 * (((float)rand())/RAND_MAX)))
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
	if (argc != 7) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <device> <platform> <policy> <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int         SIZE     = atoi(argv[1]);
	int         N_ITER   = atoi(argv[2]);
	int         DEVICE   = atoi(argv[3]);
	int         PLATFORM = atoi(argv[4]);
	Ctrl_Policy policy   = atoi(argv[5]);
	int         host_aff = atoi(argv[6]);
	Ctrl_SetHostAffinity(host_aff);

	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n POLICY %s", policy ? "Async" : "Sync");

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL));
	cl_platform_id platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	size_t platform_name_size;
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size));
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL));
	printf("\n PLATFORM: %s", platform_name);
	fflush(stdout);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL));
	cl_device_id device_id = p_devices[DEVICE];
	free(p_devices);

	size_t device_name_size;
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size));
	char *device_name = (char *)malloc(sizeof(char) * device_name_size);
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL));
	printf("\n DEVICE: %s", device_name);

	printf("\n HOST AFFINITY: %d", host_aff);
	#ifdef _CTRL_QUEUE_
	printf("\n QUEUES: ON");
	#else
	printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	free(platform_name);
	free(device_name);

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	#ifdef _CTRL_QUEUE_
	__ctrl_block__(1, 1)
		#else
		__ctrl_block__(1, 0)
	#endif
	{
		// 3. Create controller object
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_OPENCL_GPU, policy, DEVICE, PLATFORM);

		// 4. Alloc data structures
		HitTile_float matrixA = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixB = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));
		HitTile_float matrixC = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		// 5. Initialize data structures
		Ctrl_HostTask(ctrl, Init_Tiles, matrixA, matrixB, matrixC);

		// 6. Sync and start timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		// 7. Launch the kernel, copy to device (if necessary) is implicit
		Ctrl_Launch(ctrl, Add, threads, CTRL_THREAD_NULL, N_ITER, matrixA, matrixB, matrixC);

		// 8. Sync and stop timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		// 9. Calculate NORM, copy from device to host(if necessary) is implicit
		Ctrl_HostTask(ctrl, Norm_calc, matrixC);

		// 10. Free data structures
		Ctrl_Free(ctrl, matrixA, matrixB, matrixC);

		// 11. Destroy the controller
		Ctrl_Destroy(ctrl);
	}

	// 12. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
