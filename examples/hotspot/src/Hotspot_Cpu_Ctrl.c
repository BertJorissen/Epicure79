/*
LICENSE TERMS

Copyright (c)2008-2014 University of Virginia
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted without royalty fees or other restrictions, provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the University of Virginia, the Dept. of Computer Science, nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF VIRGINIA OR THE SOFTWARE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include "Ctrl.h"
#include "Hotspot_Constants.h"

double main_clock;
double exec_clock;

Ctrl_NewType( float );

CTRL_KERNEL_CHAR(Hotspot, MANUAL, BLOCKSIZE_0, BLOCKSIZE_1);

CTRL_KERNEL(Hotspot, CPU, DEFAULT, KHitTile_float power, KHitTile_float temp_src, KHitTile_float temp_dst,
		float Cap_1, float Rx_1, float Ry_1, float Rz_1, float step,
{
	int N = thread_id.x - 1;
	int S = thread_id.x + 1;
	int W = thread_id.y - 1;
	int E = thread_id.y + 1;

	N = (N < 0)								? 0								: N;
	S = (S > hit_tileDimCard(power, 0) - 1)	? hit_tileDimCard(power, 0) - 1	: S;
	W = (W < 0)								? 0								: W;
	E = (E > hit_tileDimCard(power, 1) - 1)	? hit_tileDimCard(power, 1) - 1	: E;

	hit(temp_dst, thread_id.x, thread_id.y) =hit(temp_src, thread_id.x, thread_id.y)+ 
					(Cap_1 * (hit(power, thread_id.x, thread_id.y) + 
					(hit(temp_src, S, thread_id.y) + hit(temp_src, N, thread_id.y) - 2.f*hit(temp_src, thread_id.x, thread_id.y)) * Ry_1 + 
					(hit(temp_src, thread_id.x, E) + hit(temp_src, thread_id.x, W) - 2.f*hit(temp_src, thread_id.x, thread_id.y)) * Rx_1 + 
					(amb_temp - hit(temp_src, thread_id.x, thread_id.y)) * Rz_1));
});

CTRL_HOST_TASK(Init_Tiles, HitTile_float matrix_temp, HitTile_float matrix_power) {
	srand(SEED);
	for (int i = 0; i < hit_tileDimCard( matrix_temp, 0 ); i++) {
		for (int j = 0; j < hit_tileDimCard( matrix_temp, 1 ); j++) {
			hit(matrix_temp, i, j) = (-1 + (2 * (((float)rand())/RAND_MAX)));
		}
	}
	for (int i = 0; i < hit_tileDimCard( matrix_power, 0 ); i++) {
		for (int j = 0; j < hit_tileDimCard( matrix_power, 1 ); j++) {
			hit(matrix_power, i, j) = (-1 + (2 * (((float)rand())/RAND_MAX)));
		}
	}
}

CTRL_HOST_TASK( Host_Compute, HitTile_float matrix_dst, HitTile_float matrix_src) {
	for (int i = 0; i < hit_tileDimCard(matrix_src, 0); i++){
		for (int j = 0; j < hit_tileDimCard(matrix_src, 1); j++){
			hit(matrix_dst, i, j) = hit(matrix_src, i, j);
		}		
	}
}

CTRL_HOST_TASK( Norm_Calc, HitTile_float matrix) {
	double resultado = 0;
	double suma = 0;
	
	for (int i = 0; i < hit_tileDimCard( matrix, 0 ); i++) {
		for (int j = 0; j < hit_tileDimCard( matrix, 1 ); j++) {
			suma += pow(hit(matrix, i, j), 2);
		}
	}

	resultado = sqrt(suma);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%lf, %lf, ", suma, resultado);
		fflush(stdout);
	#else
		printf("\n ----------------------- NORM ----------------------- \n\n");
		printf(" Sum: %lf \n", suma);
		printf(" Result: %lf \n", resultado);
		printf("\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif
}

CTRL_KERNEL_PROTO( Hotspot,
	1, CPU, DEFAULT, 8, 
	IN, HitTile_float, power,
	IN, HitTile_float, temp_src,
	OUT, HitTile_float, temp_dst, 
	INVAL, float, Cap_1,
	INVAL, float, Rx_1,
	INVAL, float, Ry_1,
	INVAL, float, Rz_1,
	INVAL, float, step
);

CTRL_HOST_TASK_PROTO( Init_Tiles, 2, 
	OUT, HitTile_float, matrix_temp, 
	OUT, HitTile_float, matrix_power
);

CTRL_HOST_TASK_PROTO( Host_Compute, 2,
	INVAL, HitTile_float, matrix_dst,
	IN, HitTile_float, matrix_src 
);

CTRL_HOST_TASK_PROTO( Norm_Calc, 1,
	INVAL, HitTile_float, matrix
);

void usage(int argc, char **argv){
	fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <sim_time> <iters_per_copy> <n_threads> <device> <mem_transfers> <policy> <host>\n", argv[0]);
	fprintf(stderr, "\t<grid_rows/grid_cols> - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<sim_time> - number of iterations\n");	
	fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
	fprintf(stderr, "\t<no. of threads> - number of threads\n");
	fprintf(stderr, "\t<device> - numa node for the device \n");
	fprintf(stderr, "\t<mem_transfers> - 0 for no mem transfers 1 for mem transfers\n");
	fprintf(stderr, "\t<policy> - 0 for sync or 1 for async\n");
	fprintf(stderr, "\t<host> - numa node for the host\n");
	fflush(stdout);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	main_clock = omp_get_wtime();
	
	if (argc != 9) {
		usage(argc, argv);
	}

	int grid_rows = atoi(argv[1]);
	int grid_cols = atoi(argv[1]);
	int sim_time = atoi(argv[2]);
	int iters_per_copy = atoi(argv[3]);
	int n_threads = atoi(argv[4]);
	int p_numanodes[1];
	int n_numanodes=1;
	p_numanodes[0]=atoi(argv[5]);
	bool mem_transfers=atoi(argv[6]);
	Ctrl_Policy policy=atoi(argv[7]);
	int host_aff = atoi(argv[8]);
	Ctrl_SetHostAffinity(host_aff);
	

	float grid_height = chip_height / grid_rows;
	float grid_width = chip_width / grid_cols;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step = PRECISION / max_slope / 1000.0;

	float Rx_1=1.f/Rx;
	float Ry_1=1.f/Ry;
	float Rz_1=1.f/Rz;
	float Cap_1 = step/Cap;

	#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
		printf("\n N_ITER: %d", sim_time);
		printf("\n N_THREADS: %d", n_threads);
		printf("\n ITERS_PER_COPY: %d", iters_per_copy);
		
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		printf("\n MEM_TRANSFERS: %s", mem_transfers ? "ON" : "OFF");
		printf("\n HOST AFFINITY: %d", host_aff);
		printf("\n DEVICE: %s", n_numanodes != 0 ? argv[5] : "NULL");
		#ifdef _CTRL_QUEUE_
			printf("\n QUEUES: ON");
		#else
			printf("\n QUEUES: OFF");
		#endif // _CTRL_QUEUE_
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, grid_rows, grid_cols);

	__ctrl_block__(1,1)
	{
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CPU, policy, n_threads, p_numanodes, n_numanodes, mem_transfers);

		/* allocate memory for the temperature and power arrays	*/
		HitTile_float MatrixTemp[2], MatrixPower;
		MatrixTemp[0] = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(grid_rows, grid_cols));
		MatrixTemp[1] = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(grid_rows, grid_cols));
		MatrixPower = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(grid_rows, grid_cols));
		HitTile_float MatrixCopy = hitTile(float, hitShapeSize(grid_rows, grid_cols));

		/* read initial temperatures and input power */
		Ctrl_HostTask(ctrl, Init_Tiles, MatrixTemp[0], MatrixPower);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		
		int src = 1;
		int dst = 0;
		for (int i = 0; i < sim_time ; i++) {
			int temp = src;
			src = dst;
			dst = temp;
			Ctrl_Launch(ctrl, Hotspot, threads, CTRL_THREAD_NULL, MatrixPower, MatrixTemp[src], MatrixTemp[dst],
						 Cap_1, Rx_1, Ry_1, Rz_1, step);
			if(i%iters_per_copy==0){
				Ctrl_HostTask(ctrl, Host_Compute, MatrixCopy, MatrixTemp[dst]);
			}
			
		}

		Ctrl_HostTask(ctrl, Host_Compute, MatrixCopy, MatrixTemp[dst]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		Ctrl_HostTask(ctrl, Norm_Calc, MatrixCopy);

		Ctrl_Sycnhronize();
		
		Ctrl_Free(ctrl, MatrixTemp[0], MatrixTemp[1], MatrixPower);
		hit_tileFree(MatrixCopy);
		Ctrl_Destroy(ctrl);
	}
	
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