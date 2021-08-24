#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Ctrl_Blas.c"

#define SEED 6834723

double main_clock;
double exec_clock;

/* A. INITIALIZE MATRIX */
CTRL_HOST_TASK( init_tiles, HitTile_float u1, HitTile_float u2, HitTile_float v1, HitTile_float v2, HitTile_float y, HitTile_float z, HitTile_float A) {
	int size = hit_tileDimCard(A, 0);	// matrices are square
	for (int i = 0; i < size; i++) {
		hit(u1, i) = i;
		hit(u2, i) = ((i+1)/size)/2.0;
		hit(v1, i) = ((i+1)/size)/4.0;
		hit(v2, i) = ((i+1)/size)/6.0;
		hit(y, i)  = ((i+1)/size)/8.0;
		hit(z, i)  = ((i+1)/size)/9.0;
		for (int j = 0; j < size; j++) {
			hit(A, i, j) = ((float) i*j)/size;
		}
	}
}

CTRL_HOST_TASK_PROTO( init_tiles, 7, 
	OUT, HitTile_float, u1,
	OUT, HitTile_float, u2,
	OUT, HitTile_float, v1,
	OUT, HitTile_float, v2,
	OUT, HitTile_float, y,
	OUT, HitTile_float, z,
	OUT, HitTile_float, A
);

CTRL_HOST_TASK( norm_calc, HitTile_float w) {
	double resultado=0,suma=0;
	for (int i=0; i<hit_tileDimCard(w, 0); i++){
		#ifdef DEBUG
			printf("%lf \n", hit( w, i ));
		#endif
		suma += pow(  hit( w, i ) ,2);
	}
	resultado=sqrt( suma );
	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf",suma);
	printf("\n Result: %lf \n",resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

CTRL_HOST_TASK_PROTO(norm_calc, 1, IN, HitTile_float, w);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if ( argc != 7 ) {
		fprintf(stderr, "Usage: %s <matrixSize> <alpha> <beta> <GPU> <policy> <host>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int size = atoi( argv[1] );
	float alpha = atof( argv[2] ); 
	float beta = atof( argv[3] );
	int DEVICE = atoi( argv[4] );
	Ctrl_Policy policy = (Ctrl_Policy) atoi( argv[5] );
	int host_aff = atoi( argv[6] );
	Ctrl_SetHostAffinity(host_aff);

	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n SIZE: %d x %d", size, size);
	printf("\n ALPHA: %.2f", alpha);
	printf("\n BETA: %.2f", beta);
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

#ifdef _CTRL_QUEUE_
	__ctrl_block__(1, 1)
#else
	__ctrl_block__(1, 0)
#endif
	{
		// 2. Create controller object
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CUDA, policy, DEVICE);

		// 3. Alloc data structures
		HitTile_float A = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size, size) );
		HitTile_float B = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size, size) ); // host memory not needed but currently not implemented
		HitTile_float u1 = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );
		HitTile_float u2 = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );
		HitTile_float v1 = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );
		HitTile_float v2 = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );
		HitTile_float w = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );
		HitTile_float x = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) ); // host memory not needed but currently not implemented
		HitTile_float y = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );
		HitTile_float z = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(size) );

		// 4. Initialize data structures
		Ctrl_HostTask(ctrl, init_tiles, u1, u2, v1, v2, y, z, A);

		// 5. Sync and start timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		// 6. Invoke kernels
		// variables needed cause passing literals or expressions to Ctrl_Launch is not allowed
		int inc = 1;
		int mat_size = size*size;
		float one = 1.0;
		float zero = 0.0;
		Ctrl_Blas_Trans trans_T = CTRL_BLAS_TRANS;
		Ctrl_Blas_Trans trans_N = CTRL_BLAS_NOTRANS;

		Ctrl_Launch(ctrl, ctrl_scopy, CTRL_THREAD_NULL, CTRL_THREAD_NULL, mat_size, A, inc, B, inc);									// Copy A to B
		Ctrl_Launch(ctrl, ctrl_sger, CTRL_THREAD_NULL, CTRL_THREAD_NULL, size, size, one, u1, inc, v1, inc, B, size);				// B = u1*v1' + B
		Ctrl_Launch(ctrl, ctrl_sger, CTRL_THREAD_NULL, CTRL_THREAD_NULL, size, size, one, u2, inc, v2, inc, B, size);				// B = u2*v2' + B
		Ctrl_Launch(ctrl, ctrl_scopy, CTRL_THREAD_NULL, CTRL_THREAD_NULL, size, z, inc, x, inc);										// Copy z to x
		Ctrl_Launch(ctrl, ctrl_sgemv, CTRL_THREAD_NULL, CTRL_THREAD_NULL, trans_T, size, size, beta, B, size, y, inc, one, x, inc);	// x = βB'y + x (B' * y)
		Ctrl_Launch(ctrl, ctrl_sgemv, CTRL_THREAD_NULL, CTRL_THREAD_NULL, trans_N, size, size, alpha, B, size, x, inc, zero, w, inc);// w = αBx (B * x)

		// 7. Sync and stop timer
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		// 8. Calculate NORM
		Ctrl_HostTask(ctrl, norm_calc, w);

		// 9. Free data structures
		Ctrl_Free(ctrl, A, B, u1, u2, v1, v2, w, x, y, z);

		// 10. Destroy the controller
		Ctrl_Destroy(ctrl);
	}

	// 11. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return 0;
}
