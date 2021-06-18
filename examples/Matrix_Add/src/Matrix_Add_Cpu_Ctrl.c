#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Ctrl.h"

double main_clock;
double exec_clock;

/* Declare type for tiles */
Ctrl_NewType( float );

CTRL_KERNEL_CHAR(Add, MANUAL, 16, 16);

/* A. GENERIC Kernel implementation */
CTRL_KERNEL(Add, GENERIC, DEFAULT, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C,
{
	for (int k = 0; k < n_iter; k++) {
		hit(C, thread_id.x, thread_id.y) =
			hit(C, thread_id.x, thread_id.y) +
			hit(A, thread_id.x, thread_id.y) +
			hit(B, thread_id.x, thread_id.y);
	}
});

/* A. GENERIC Kernel implementation */
CTRL_KERNEL_FUNCTION(Add, CPU, DEFAULT, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C)
{
	for (int k = 0; k < n_iter; k++) {
		hit(C, thread_id.x, thread_id.y) =
			hit(C, thread_id.x, thread_id.y) +
			hit(A, thread_id.x, thread_id.y) +
			hit(B, thread_id.x, thread_id.y);
	}

	CTRL_KERNEL_END(CPU);
}

/* B. Defining kernel prototypes */ 
CTRL_KERNEL_PROTO( Add, 
		2, GENERIC, DEFAULT, CPU, DEFAULT, 
		4, 
		INVAL, int, n_iter,
		IN, HitTile_float, A, 
		IN, HitTile_float, B, 
		IO, HitTile_float, C);

/* C. Host task to initialize the matrices */
CTRL_HOST_TASK(Init_Matrix, HitTile_float matrixA, HitTile_float matrixB, HitTile_float matrixC)
{
	for (int i=0; i<hit_tileDimCard(matrixA, 0); i++){
		for (int j=0; j<hit_tileDimCard(matrixA, 1); j++){
			hit( matrixA, i, j ) = 1;//j+i*hit_tileDimCard(matrixA, 0);
			hit( matrixB, i, j ) = 2;//j+i*hit_tileDimCard(matrixB, 0);
			hit( matrixC, i, j ) = 0;
		}
	}
}

/* D. Host task to calculate and print the norm */
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

/* E. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Init_Matrix, 3,
		OUT, HitTile_float, A, 
		OUT, HitTile_float, B, 
		OUT, HitTile_float, C);

CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	// printf("", blocksize_CPU_Add);
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if ( argc != 8 ) {
		fprintf(stderr, "\nUsage: %s <numRows> <numIter> <numThreads> <device> <mem_transfers> <policy> <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE = atoi( argv[1] );
	int N_ITER = atoi( argv[2] );
	int N_THREADS = atoi( argv[3] );
	int p_numanodes[1];
	int n_numanodes=1;
	p_numanodes[0] = atoi( argv[4] );
	bool mem_transfers = atoi( argv[5] );
	Ctrl_Policy policy = atoi( argv[6] );
	int host_aff =  atoi( argv[7] ) ;
	Ctrl_SetHostAffinity(host_aff);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n N_THREADS: %d", N_THREADS);
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

	// 2. Create block of threads
	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	__ctrl_block__(1, 1) {
		// 3. Create controller object
		PCtrl ctrl=Ctrl_Create(CTRL_TYPE_CPU, policy, N_THREADS, p_numanodes, n_numanodes, mem_transfers);
		
		// 4. Alloc data structures 
		HitTile_float matrixA = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );
		HitTile_float matrixB = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );
		HitTile_float matrixC = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );

		// 5. Initialize data structures
		Ctrl_HostTask(ctrl, Init_Matrix, matrixA, matrixB, matrixC);

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
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
