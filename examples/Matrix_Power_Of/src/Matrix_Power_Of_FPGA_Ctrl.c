/*
 * <license>
 * 
 * Controller v2.1
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <assert.h>
#include "Ctrl.h"

#define SEED 6834723
#define EPSILON 0.0001

double main_clock;
double exec_clock;

float RandomFloat(float min, float max) {
    assert(max > min); 
    float random = ((float) rand()) / (float) RAND_MAX;
    float range = max - min;  
    return (random*range) + min;
}

Ctrl_NewType( float );

CTRL_KERNEL_CHAR(Mult, MANUAL, LOCAL_SIZE_0, LOCAL_SIZE_1);

CTRL_KERNEL_PROTO( Mult,
	1, FPGA, DEFAULT, 5, 
	OUT, HitTile_float, C, 
	IN, HitTile_float, A, 
	IN, HitTile_float, B,
	INVAL, int, A_width,
	INVAL, int, B_width
);

CTRL_HOST_TASK( Init_Tiles, HitTile_float matrix_a, HitTile_float matrix_b, HitTile_float matrix_c) {
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

CTRL_HOST_TASK( Host_Compute, int ITER, double *p_sum, double *p_res, HitTile_float matrix, HitTile_float matrix_res) {
	double minimum = hit(matrix, 0, 0);
	double maximum = hit(matrix, 0, 0);

	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			if (minimum > hit(matrix, j, k)){
				minimum = hit(matrix, j, k);
			}
			if (maximum < hit(matrix, j, k)){
				maximum = hit(matrix, j, k);
			}
		}
	}

	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			hit(matrix, j, k) = hit(matrix, j, k) - minimum;
			hit(matrix, j, k) = hit(matrix, j, k) / maximum; 
		}
	}

	p_sum[ITER] = 0;
	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			p_sum[ITER] += pow(hit(matrix, j, k), 2);
		}
	}
	p_res[ITER] = sqrt(p_sum[ITER]);

	for (int j = 0; j < hit_tileDimCard( matrix, 0 ); j++) {
		for (int k = 0; k < hit_tileDimCard( matrix, 1 ); k++) {
			hit(matrix_res, j, k) = hit(matrix, j, k) / p_res[ITER];
		}
	}
}

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
	INVAL, HitTile_float, matrix_res
);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 8) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <device> <platform> <exec_mode> <policy> <affinity>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int SIZE = atoi(argv[1]);
	int N_ITER = atoi(argv[2]);
	int DEVICE = atoi(argv[3]);
	int PLATFORM = atoi(argv[4]);
	int EXEC_MODE = atoi(argv[5]);
	Ctrl_Policy POLICY = atoi(argv[6]);
	int host_aff = atoi(argv[7]);
	Ctrl_SetHostAffinity(host_aff);

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP( clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL) );
	cl_platform_id platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	size_t platform_name_size;
	OPENCL_ASSERT_OP( clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size) );
	char* platform_name = (char*)malloc( sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP( clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL ) );

    cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, DEVICE + 1, p_devices, NULL);
    cl_device_id device_id = p_devices[DEVICE];
    free(p_devices);
	
	size_t device_name_size;
	OPENCL_ASSERT_OP( clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size) );
	char* device_name = (char*) malloc( sizeof(char) * device_name_size );
	OPENCL_ASSERT_OP( clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL) );

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%s, %s, ", device_name, platform_name);
	#else	
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n POLICY %s", POLICY ? "Async" : "Sync");
		printf("\n DEVICE: %s", device_name);
		printf("\n PLATFORM: %s", platform_name);
		printf("\n EXEC_MODE: %d", EXEC_MODE);
		printf("\n HOST AFFINITY: %d", host_aff);
		#ifdef _CTRL_QUEUE_
			printf("\n QUEUES: ON");
		#else
			printf("\n QUEUES: OFF");
		#endif // _CTRL_QUEUE_
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	free(platform_name);
	free(device_name);

	double *p_res = (double *)malloc(N_ITER * sizeof(double));
	double *p_sum = (double *)malloc(N_ITER * sizeof(double));

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, SIZE, SIZE);

	Ctrl_Thread group;
	Ctrl_ThreadInit(group, LOCAL_SIZE_0, LOCAL_SIZE_1);
	#ifdef _CTRL_QUEUE_
		__ctrl_block__(1,1)
	#else
		__ctrl_block__(1,0)
	#endif //_CTRL_QUEUE_
	{
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_FPGA, POLICY, DEVICE, PLATFORM, EXEC_MODE);

		HitTile_float matrix[3];
		for(int i = 0; i < 3; i++) {
			matrix[i] = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE) );
		}

		HitTile_float matrix_tmp = hitTile(float, hitShapeSize(SIZE, SIZE));

		Ctrl_HostTask(ctrl, Init_Tiles, matrix[0], matrix[1], matrix[2]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		Ctrl_MoveTo(ctrl, matrix[0], matrix[1], matrix[2]);

		int src, dst, aux;
		src = 1;
		dst = 2;
        for (int i = 0; i < N_ITER; i++) {
			Ctrl_Launch(ctrl, Mult, threads, group, matrix[dst], matrix[0], matrix[src], SIZE, SIZE);
			Ctrl_HostTask(ctrl, Host_Compute, i, p_sum, p_res, matrix[dst], matrix_tmp);

			aux = src;
			src = dst;
			dst = aux;		
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

		Ctrl_Free(ctrl, matrix[0], matrix[1], matrix[2]);
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
