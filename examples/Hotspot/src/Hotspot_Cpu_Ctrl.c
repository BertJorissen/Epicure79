/**
 * @file Hotspot_Cpu_Ctrl.c
 * @author Trasgo Group
 * @brief Hotspot: Ctrl CPU version
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
#include "Hotspot_Constants.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double main_clock;
double exec_clock;

int grid_rows;
int grid_cols;

Ctrl_NewType(float);

CTRL_KERNEL_CHAR(Hotspot, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);

#define hotspot_params 8, IN, HitTile_float, power, IN, HitTile_float, temp_src, OUT, HitTile_float, temp_dst, INVAL, float, Cap_1, INVAL, float, Rx_1, INVAL, float, Ry_1, INVAL, float, Rz_1, INVAL, float, step
#define init_params    2, OUT, HitTile_float, matrix_temp, OUT, HitTile_float, matrix_power
#define compute_params 2, INVAL, HitTile_float, matrix_dst, IN, HitTile_float, matrix_src

CTRL_KERNEL(Hotspot, CPU, DEFAULT, CTRL_KPARAMS(hotspot_params), {
	const float amb_temp = 80.0;

	int N = thr_i - 1;
	int S = thr_i + 1;
	int W = thr_j - 1;
	int E = thr_j + 1;

	N = (N < 0) ? 0 : N;
	S = (S > hit_tileDimCard(power, 0) - 1) ? hit_tileDimCard(power, 0) - 1 : S;
	W = (W < 0) ? 0 : W;
	E = (E > hit_tileDimCard(power, 1) - 1) ? hit_tileDimCard(power, 1) - 1 : E;

	hit(temp_dst, thr_i, thr_j) = hit(temp_src, thr_i, thr_j) +
								  (Cap_1 * (hit(power, thr_i, thr_j) +
											(hit(temp_src, S, thr_j) + hit(temp_src, N, thr_j) - 2.f * hit(temp_src, thr_i, thr_j)) * Ry_1 +
											(hit(temp_src, thr_i, E) + hit(temp_src, thr_i, W) - 2.f * hit(temp_src, thr_i, thr_j)) * Rx_1 +
											(amb_temp - hit(temp_src, thr_i, thr_j)) * Rz_1));
});

CTRL_HOST_TASK(Init_Tiles, CTRL_HPARAMS(init_params)) {
	srand(SEED);
	for (int i = 0; i < grid_rows; i++) {
		for (int j = 0; j < grid_cols; j++) {
			hit(matrix_temp, i, j) = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
	for (int i = 0; i < grid_rows; i++) {
		for (int j = 0; j < grid_cols; j++) {
			hit(matrix_power, i, j) = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
}

CTRL_HOST_TASK(Host_Compute, CTRL_HPARAMS(compute_params)) {
	for (int i = 0; i < grid_rows * grid_cols; i++) {
		hit(matrix_dst, i) = hit_as(matrix_src, matrix_dst, i);
	}
}

CTRL_KERNEL_PROTO(Hotspot, 1, CPU, DEFAULT, hotspot_params);
CTRL_HOST_TASK_PROTO(Init_Tiles, init_params);
CTRL_HOST_TASK_PROTO(Host_Compute, compute_params);

// Extra HostTask added for error checking
#define norm_params 1, INVAL, HitTile_float, matrix
CTRL_HOST_TASK(Norm_Calc, HitTile_float matrix) {
	double resultado = 0;
	double suma      = 0;

	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			suma += pow(hit(matrix, i, j), 2);
		}
	}

	resultado = sqrt(suma);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", suma, resultado);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- NORM ----------------------- \n\n");
	printf(" Sum: %lf \n", suma);
	printf(" Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);
}
CTRL_HOST_TASK_PROTO(Norm_Calc, norm_params);

void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <sim_time> <iters_per_copy> <n_threads> <device> <mem_transfers> <policy> <host>\n", argv[0]);
	fprintf(stderr, "\t<grid_rows/grid_cols> - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<sim_time> - number of iterations\n");
	fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
	fprintf(stderr, "\t<policy> - 0 for sync or 1 for async\n");
	fprintf(stderr, "\t<config_file> - path to ctrl config file\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	if (argc != 6) {
		usage(argc, argv);
	}

	grid_rows                  = atoi(argv[1]);
	grid_cols                  = atoi(argv[1]);
	int         sim_time       = atoi(argv[2]);
	int         iters_per_copy = atoi(argv[3]);
	Ctrl_Policy policy         = atoi(argv[4]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[5];

	float grid_height = chip_height / grid_rows;
	float grid_width  = chip_width / grid_cols;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx  = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry  = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz  = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step      = PRECISION / max_slope / 1000.0;

	float Rx_1  = 1.f / Rx;
	float Ry_1  = 1.f / Ry;
	float Rz_1  = 1.f / Rz;
	float Cap_1 = step / Cap;

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, grid_rows, grid_cols);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		// Extra information for collecting results
		Ctrl_Info info = Ctrl_GetInfo(ctrl);
		#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%d, %d-%d, %s, ", info.n_threads, info.numa_range_min, info.numa_range_max, info.mem_transfers ? "ON" : "OFF");
		#else
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
		printf("\n N_ITER: %d", sim_time);
		printf("\n N_THREADS: %d", info.n_threads);
		printf("\n ITERS_PER_COPY: %d", iters_per_copy);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		printf("\n MEM_TRANSFERS: %s", info.mem_transfers ? "ON" : "OFF");
		printf("\n HOST AFFINITY: %d", info.host_affinity);
		printf("\n DEVICE: %d-%d", info.numa_range_min, info.numa_range_max);
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
		#endif // _CTRL_EXAMPLES_EXP_MODE_

		/* allocate memory for the temperature and power arrays	*/
		HitTile_float MatrixTemp[2], MatrixPower;
		HitShape      shape      = hitShapeSize(grid_rows, grid_cols);
		MatrixTemp[0]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixTemp[1]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixPower              = Ctrl_DomainAlloc(ctrl, float, shape);
		HitTile_float MatrixCopy = hitTile(float, shape);

		/* read initial temperatures and input power */
		Ctrl_HostTask(Init_Tiles, MatrixTemp[0], MatrixPower);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		int src = 1;
		int dst = 0;
		for (int i = 0; i < sim_time; i++) {
			int temp = src;
			src      = dst;
			dst      = temp;
			Ctrl_Launch(ctrl, Hotspot, threads, CTRL_THREAD_NULL, MatrixPower, MatrixTemp[src], MatrixTemp[dst],
						Cap_1, Rx_1, Ry_1, Rz_1, step);
			if (i % iters_per_copy == 0) {
				Ctrl_HostTask(Host_Compute, MatrixCopy, MatrixTemp[dst]);
			}
		}

		Ctrl_HostTask(Host_Compute, MatrixCopy, MatrixTemp[dst]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		Ctrl_HostTask(Norm_Calc, MatrixCopy);

		Ctrl_Synchronize();

		Ctrl_Free(ctrl, MatrixTemp[0], MatrixTemp[1], MatrixPower);
		hit_tileFree(MatrixCopy);
		Ctrl_EndBlock();
	}

	Ctrl_Finalize();
	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n");
	printf("Clock main: %lf\n", main_clock);
	printf("Clock exec: %lf\n", exec_clock);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	return EXIT_SUCCESS;
}
