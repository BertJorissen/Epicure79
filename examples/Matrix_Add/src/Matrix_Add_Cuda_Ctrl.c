#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "Ctrl.h"

#define SEED 6834723

double main_clock;
double exec_clock;

/* Declare type for tiles */
Ctrl_NewType( float );

/* A. Characterization for the kernel */
CTRL_KERNEL_CHAR(Add, MANUAL, BLOCKSIZE_0, BLOCKSIZE_1);

CTRL_KERNEL(Add, CUDALIB, MAGMA, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C,
{
	for (int k = 0; k < n_iter; k++) {
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, A.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, B.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
	}
});

CTRL_KERNEL(Add, CUDALIB, CUBLAS, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C,
{
	const float alpha = 1.0f;
	for (int k = 0; k < n_iter; k++) {
		cublasSaxpy(handle, hit_tileDimCard(A, 0)*hit_tileDimCard(A, 1), &alpha, A.data, 1, C.data, 1);
		cublasSaxpy(handle, hit_tileDimCard(A, 0)*hit_tileDimCard(A, 1), &alpha, B.data, 1, C.data, 1);
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
	IO, HitTile_float, C
);

/* D. Host task to initialize the matrices */
CTRL_HOST_TASK( Init_Tiles, HitTile_float matrixA, HitTile_float matrixB, HitTile_float matrixC)
{
	srand(SEED);
	for (int i = 0; i < hit_tileDimCard( matrixA, 0 ); i++) {
		for (int j = 0; j < hit_tileDimCard( matrixA, 1 ); j++) {
			hit(matrixA, i, j) = 1.0; // (-1 + (2 * (((float)rand())/RAND_MAX)));
			hit(matrixB, i, j) = 2.0; // (-1 + (2 * (((float)rand())/RAND_MAX)));
			hit(matrixC, i, j) = 0.0;
		}
	}
}

/* E. Host task to calculate and print the norm */
CTRL_HOST_TASK(Norm_calc, HitTile_float matrix)
{
	double resultado=0;
	double suma=0;
	for (int i=0; i<hit_tileDimCard(matrix, 0); i++ ) {
		for (int j=0; j<hit_tileDimCard(matrix, 1); j++ ) {
			suma += pow( hit(matrix, i, j) , 2);
		}
	}
	resultado=sqrt( suma );

	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf",suma);
	printf("\n Result: %lf \n",resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO( Init_Tiles, 3,
	OUT, HitTile_float, matrixA,
	OUT, HitTile_float, matrixB, 
	OUT, HitTile_float, matrixC
);

CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]){
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 6) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <device> <policy> <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE = atoi(argv[1]);
	int N_ITER = atoi(argv[2]);
	int DEVICE = atoi(argv[3]);
	Ctrl_Policy policy = (Ctrl_Policy) atoi( argv[4] );
	int host_aff =  atoi( argv[5] ) ;
	Ctrl_SetHostAffinity(host_aff);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n POLICY %s", policy ? "Async" : "Sync");
	printf("\n HOST AFFINITY: %d", host_aff);
	struct cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, DEVICE); 
	printf("\n DEVICE: %s", cu_dev_prop.name);
	#ifdef _CTRL_QUEUE_
		printf("\n QUEUES: ON");
	#else
		printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	
	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

#ifdef _CTRL_QUEUE_
	__ctrl_block__(1,1)
#else
	__ctrl_block__(1,0)
#endif
	{
		// 3. Create controller object
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CUDA, policy, DEVICE);
		
		// 4. Alloc data structures
		HitTile_float matrixA = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );
		HitTile_float matrixB = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );
		HitTile_float matrixC = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );
		
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
