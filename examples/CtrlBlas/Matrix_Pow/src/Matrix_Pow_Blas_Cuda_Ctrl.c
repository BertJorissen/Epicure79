/**
 * @file Matrix_Pow_Blas_Cuda_Ctrl.c
 * @author Trasgo Group
 * @brief MatrixPow: CtrlBlas CUDA version
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl_Blas.c"

double main_clock;
double exec_clock;

// A. INITIALIZE MATRIX
CTRL_HOST_TASK(init_tiles, HitTile_float A) {
	for (int i = 0; i < hit_tileDimCard(A, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(A, 1); j++) {
			hit(A, i, j) = ((float)i * j) / hit_tileDimCard(A, 0);
		}
	}
}

// B. Calculate NORM
CTRL_HOST_TASK(norm_calc, HitTile_float A) {
	double resultado = 0, suma = 0;
	for (int i = 0; i < hit_tileDimCard(A, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(A, 1); j++) {
			#ifdef DEBUG
			printf("%f \n", hit(A, i, j));
			#endif
			suma += pow(hit(A, i, j), 2);
		}
	}
	resultado = sqrt(suma);
	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
}

CTRL_HOST_TASK_PROTO(init_tiles, 1, OUT, HitTile_float, A);

CTRL_HOST_TASK_PROTO(norm_calc, 1, IN, HitTile_float, A);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	if (argc != 5) {
		fprintf(stderr, "\nUsage: %s <size> <n_iter> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int         SIZE   = atoi(argv[1]);
	int         N_ITER = atoi(argv[2]);
	Ctrl_Policy policy = (Ctrl_Policy)atoi(argv[3]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[4];

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		Ctrl_Info info = Ctrl_GetInfo(ctrl);
		#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%s, ", info.device_name);
		#else
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE: %d", SIZE);
		printf("\n N_ITER: %d", N_ITER);
		printf("\n DEVICE: %s", info.device_name);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		printf("\n HOST AFFINITY: %d", info.host_affinity);
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
		#endif // _CTRL_EXAMPLES_EXP_MODE_

		HitTile_float matrix_a = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(SIZE, SIZE));

		Ctrl_HostTask(init_tiles, matrix_a);

		Ctrl_Blas_Trans trans_N = CTRL_BLAS_NOTRANS;
		float           alpha   = 1.0;
		float           beta    = 0.0;

		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		for (int power = 0; power < N_ITER; power++) {
			Ctrl_Launch(ctrl, ctrl_sgemm, CTRL_THREAD_NULL, CTRL_THREAD_NULL, trans_N, trans_N, SIZE, SIZE, SIZE, alpha,
						matrix_a, SIZE, matrix_a, SIZE, beta, matrix_a, SIZE); // C = αAB + βC
		}

		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;
		Ctrl_HostTask(norm_calc, matrix_a);

		Ctrl_Free(ctrl, matrix_a);

		Ctrl_EndBlock();
	}

	Ctrl_Finalize();

	// Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;
	printf("\n ----------------------- TIME ----------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec : %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");

	return EXIT_SUCCESS;
}
