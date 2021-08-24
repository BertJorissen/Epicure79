#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Ctrl_Blas.c"

#define SEED 6834723

double main_clock;
double exec_clock;

/* A. Host task to initialize the matrices */
CTRL_HOST_TASK( Init_Tiles, HitTile_float A, HitTile_float B) {
	srand(SEED);
	for (int i = 0; i < hit_tileDimCard( A, 0 ); i++) {
		for (int j = 0; j < hit_tileDimCard( A, 1 ); j++) {
			hit(A, i, j) = 1.0; // (-1 + (2 * (((float)rand())/RAND_MAX)));
			hit(B, i, j) = 2.0; // (-1 + (2 * (((float)rand())/RAND_MAX)));
		}
	}
}

/* B. Host task to calculate and print the norm */
CTRL_HOST_TASK(Norm_calc, HitTile_float matrix) {
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

/* C. Defining host task prototypes */
CTRL_HOST_TASK_PROTO( Init_Tiles, 2,
	OUT, HitTile_float, A,
	OUT, HitTile_float, B
);

CTRL_HOST_TASK_PROTO(Norm_calc, 1, IN, HitTile_float, matrix);

/*
 * Main program to perform matrix addition
 */
int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if ( argc != 7 ) {
		fprintf(stderr, "\nUsage: %s <numRows> <numThreads> <device> <mem_transfers> <policy> <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size = atoi( argv[1] );
	int mat_size = size * size;
	int n_threads = atoi( argv[2] );
	int p_numanodes[1];
	int n_numanodes=1;
	p_numanodes[0] = atoi( argv[3] );
	bool mem_transfers = atoi( argv[4] );
	Ctrl_Policy policy = atoi( argv[5] );
	int host_aff =  atoi( argv[6] ) ;
	Ctrl_SetHostAffinity(host_aff);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d", size);
	printf("\n N_THREADS: %d", n_threads);
	printf("\n POLICY %s", policy ? "Async" : "Sync");
	printf("\n MEM_TRANSFERS: %s", mem_transfers ? "ON" : "OFF");
	printf("\n HOST AFFINITY: %d", host_aff);
	printf("\n DEVICE: %s", n_numanodes != 0 ? argv[3] : "NULL");
	#ifdef _CTRL_QUEUE_
		printf("\n QUEUES: ON");
	#else
		printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);

	__ctrl_block__(1, 1) {
		// 2. Create controller object
		PCtrl ctrl=Ctrl_Create(CTRL_TYPE_CPU, policy, n_threads, p_numanodes, n_numanodes, mem_transfers);
		
		// 3. Alloc data structures 
		HitTile_float A = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size, size) );
		HitTile_float B = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size, size) );

		// 4. Initialize data structures
		Ctrl_HostTask(ctrl, Init_Tiles, A, B);

		// 5. Sync and start timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		// 6. Launch the kernel, copy to device (if necessary) is implicit
		float alpha = 1.0;
		int inc = 1;
		Ctrl_Launch(ctrl, ctrl_saxpy, CTRL_THREAD_NULL, CTRL_THREAD_NULL, mat_size, alpha, A, inc, B, inc);

		// 7. Sync and stop timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		// 8. Calculate NORM, copy from device to host(if necessary) is implicit
		Ctrl_HostTask(ctrl, Norm_calc, B);

		// 9. Free data structures
		Ctrl_Free(ctrl, A, B);

		// 10. Destroy the controller
		Ctrl_Destroy(ctrl);
	}

	// 11. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
