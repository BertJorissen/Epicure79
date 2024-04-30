/**
 * @file Hotspot_FPGA_Ctrl.c
 * @author Trasgo Group
 * @brief Hotspot: Ctrl FPGA version
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
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>

#include "Ctrl.h"
#include "Hotspot_Constants.h"

#define SSIZE 8

// input shift register parameters
#define IN_SR_BASE 2 * BSIZE          // this shows the point to write into the shift register; one row for top neighbors and one row for center
#define IN_SR_SIZE IN_SR_BASE + SSIZE // SSIZE indexes are enough for the bottommost row

// offsets for reading from the input shift register
#define SR_OFF_C BSIZE            // center
#define SR_OFF_N SR_OFF_C + BSIZE // north  (+y direction)
#define SR_OFF_S SR_OFF_C - BSIZE // south  (-y direction)
#define SR_OFF_E SR_OFF_C + 1     // east   (-x direction)
#define SR_OFF_W SR_OFF_C - 1     // west   (+x direction)

double main_clock;
double exec_clock;

int grid_rows;
int grid_cols;

Ctrl_NewType(float);

CTRL_KERNEL_CHAR(Hotspot, MANUAL, LOCAL_SIZE_1, LOCAL_SIZE_0);

CTRL_KERNEL_PROTO(Hotspot,
				  1, FPGA, DEFAULT, 10,
				  IN, HitTile_float, pwr_in,
				  IN, HitTile_float, src,
				  OUT, HitTile_float, dst,
				  INVAL, int, grid_cols,
				  INVAL, int, grid_rows,
				  INVAL, float, sdc,
				  INVAL, float, Rx_1,
				  INVAL, float, Ry_1,
				  INVAL, float, Rz_1,
				  INVAL, int, comp_exit);

#define init_params    2, OUT, HitTile_float, matrix_temp, OUT, HitTile_float, matrix_power
#define compute_params 2, INVAL, HitTile_float, matrix_dst, IN, HitTile_float, matrix_src

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

CTRL_HOST_TASK_PROTO(Init_Tiles, init_params);
CTRL_HOST_TASK_PROTO(Host_Compute, compute_params);

// Extra HostTask added for error checking
#define norm_params 1, INVAL, HitTile_float, matrix
CTRL_HOST_TASK(Norm_Calc, CTRL_HPARAMS(norm_params)) {
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
	fflush(stdout);
	#else
	printf("\n ----------------------- NORM ----------------------- \n\n");
	printf(" Sum: %lf \n", suma);
	printf(" Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif
}

CTRL_HOST_TASK_PROTO(Norm_Calc, norm_params);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	if (argc != 7) {
		fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy> <device> <platform> <exec_mode> <policy> <affinity>\n", argv[0]);
		fprintf(stderr, "\t<grid_rows/grid_cols> - number of rows/cols in the grid (positive integer)\n");
		fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
		fprintf(stderr, "\t<sim_time> - number of iterations\n");
		fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
		fprintf(stderr, "\t<policy> - 0 for sync or 1 for async\n");
		fprintf(stderr, "\t<config_file> - path to ctrl config file\n");
		exit(EXIT_FAILURE);
	}

	grid_rows                    = atoi(argv[1]);
	grid_cols                    = atoi(argv[1]);
	int         pyramid_height   = atoi(argv[2]);
	int         total_iterations = atoi(argv[3]);
	int         iters_per_copy   = atoi(argv[4]);
	Ctrl_Policy policy           = atoi(argv[5]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[6];

	int smallBlockCol = LOCAL_SIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = LOCAL_SIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols     = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows     = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	float grid_height = chip_height / grid_rows;
	float grid_width  = chip_width / grid_cols;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx  = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry  = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz  = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step      = PRECISION / max_slope / 1000.0;

	float step_div_Cap = step / Cap;
	float Rx_1         = 1 / Rx;
	float Ry_1         = 1 / Ry;
	float Rz_1         = 1 / Rz;

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, LOCAL_SIZE_1 * blockRows, LOCAL_SIZE_0 * blockCols);

	Ctrl_Thread group;
	Ctrl_ThreadInit(group, 1);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		// Extra information for collecting results
		Ctrl_Info info = Ctrl_GetInfo(ctrl);
		#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%s, %s, ", info.device_name, info.platform_name);
		#else // _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
		printf("\n PYRAMID HEIGHT: %d", pyramid_height);
		printf("\n N_ITER: %d", total_iterations);
		printf("\n ITERS_PER_COPY: %d", iters_per_copy);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		printf("\n DEVICE: %s", info.device_name);
		printf("\n PLATFORM: %s", info.platform_name);
		printf("\n EXEC_MODE: %s", info.exec_mode);
		printf("\n HOST AFFINITY: %d", info.host_affinity);
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		HitTile_float MatrixTemp[2], MatrixPower;
		HitShape      shape      = hitShapeSize(grid_rows, grid_cols);
		MatrixTemp[0]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixTemp[1]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixPower              = Ctrl_DomainAlloc(ctrl, float, shape);
		HitTile_float MatrixCopy = hitTile(float, shape);

		int comp_bsize = LOCAL_SIZE_0 - 2;
		int last_col   = (grid_cols % comp_bsize == 0) ? grid_cols + 0 : grid_cols + comp_bsize - grid_cols % comp_bsize;
		int col_blocks = last_col / comp_bsize;
		int comp_exit  = LOCAL_SIZE_0 * col_blocks * (grid_rows + 1) / SSIZE;

		Ctrl_HostTask(Init_Tiles, MatrixTemp[0], MatrixPower);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		int real_iter = 1;
		int src       = 1;
		int dst       = 0;

		for (int i = 0; i < total_iterations; i += 1) {
			int temp = src;
			src      = dst;
			dst      = temp;

			Ctrl_Launch(ctrl, Hotspot, threads, group, MatrixPower, MatrixTemp[src], MatrixTemp[dst], grid_cols,
						grid_rows, step_div_Cap, Rx_1, Ry_1, Rz_1, comp_exit);

			if ((real_iter % iters_per_copy) == 0) {
				Ctrl_HostTask(Host_Compute, MatrixCopy, MatrixTemp[dst]);
			}
			real_iter++;
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
