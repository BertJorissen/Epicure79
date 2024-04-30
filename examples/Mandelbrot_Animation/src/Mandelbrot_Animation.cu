/**
 * @file Mandelbrot_Animation.cu
 * @author Trasgo Group
 * @brief Mandelbrot: Ctrl CUDA version
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

#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"

#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

Ctrl_NewType(int);

double main_clock;
double exec_clock;

#define WRITE_OUTPUT

// FUNCTION: WRITE RESULTS IN OUTPUT FILES
void plot(HitTile_int mat) {
	#ifdef WRITE_OUTPUT
	static int iter = 0;
	FILE      *out;
	char       name[25];
	sprintf(name, "output.%d.0.dat", iter);
	out = fopen(name, "wb");
	if (out == NULL) {
		fprintf(stderr, "\nError opening file: %s for data output\n", name);
		exit(EXIT_FAILURE);
	}
	iter++;
	int size_x   = hit_tileDimCard(mat, 0);
	int size_y   = hit_tileDimCard(mat, 1);
	int ranges[] = {0, size_x, 0, size_y};
	// WRITE SIZES
	fwrite(&size_x, sizeof(int), 1, out);
	fwrite(&size_y, sizeof(int), 1, out);
	// WRITE SUBRANGE (SAME AS SIZE)
	fwrite(ranges, sizeof(int), 4, out);
	// WRITE DATA
	fwrite(&hit(mat, 0, 0), sizeof(int), size_x * size_y, out);
	fclose(out);

	#ifdef DEBUG
	if (iter == 1) {
		FILE *debug = stdout;
		for (int i = 0; i < hit_tileDimCard(mat, 0); i++) {
			for (int j = 0; j < hit_tileDimCard(mat, 1); j++)
				fprintf(debug, "%d %d %d\n", i, j, hit(mat, i, j));
			fprintf(debug, "\n");
		}
	}
	#endif
	#endif
}

CTRL_KERNEL_CHAR(Mandelbrot, MANUAL, BLOCKSIZE, BLOCKSIZE);

CTRL_KERNEL(Mandelbrot, GENERIC, DEFAULT, KHitTile_int mat, int threshold, float x1, float x2, float y1, float y2, {
	float x0 = x1 + (x2 - x1) / hit_tileDimCard(mat, 0) * thr_i;
	float y0 = y1 + (y2 - y1) / hit_tileDimCard(mat, 1) * thr_j;

	float x         = 0.0;
	float y         = 0.0;
	int   iteration = 0;
	while (x * x + y * y <= 2 * 2 && iteration < threshold) {
		float xtemp = x * x - y * y + x0;
		y           = 2 * x * y + y0;
		x           = xtemp;
		iteration++;
	}
	hit(mat, thr_i, thr_j) = iteration;
});

CTRL_HOST_TASK(Count_And_Paint, HitTile_int mat, int *result) {
	int count = 0;
	for (int i = 0; i < hit_tileDimCard(mat, 0); i++)
		for (int j = 0; j < hit_tileDimCard(mat, 1); j++)
			if (hit(mat, i, j) < 10) count++;
	*result = count;
	plot(mat);
}

CTRL_KERNEL_PROTO(Mandelbrot,
				  1, GENERIC, DEFAULT, 6,
				  OUT, HitTile_int, mat,
				  INVAL, int, threshold,
				  INVAL, float, x1,
				  INVAL, float, x2,
				  INVAL, float, y1,
				  INVAL, float, y2);

CTRL_HOST_TASK_PROTO(Count_And_Paint, 2,
					 IN, HitTile_int, mat,
					 INVAL, int *, result);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	Ctrl_Init(&argc, &argv);

	if (argc != 12) {
		fprintf(stderr, "\nUsage: %s <sizeX> <sizeY> <x1> <x2> <y1> <y2> <step> <threshold> <iters> <policy> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int         size_x     = atoi(argv[1]);
	int         size_y     = atoi(argv[2]);
	float       x1         = atof(argv[3]);
	float       x2         = atof(argv[4]);
	float       y1         = atof(argv[5]);
	float       y2         = atof(argv[6]);
	float       step       = atof(argv[7]);
	int         threshold  = atoi(argv[8]);
	int         iterations = atoi(argv[9]);
	Ctrl_Policy policy     = (Ctrl_Policy)atoi(argv[10]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[11];

	int *p_results = (int *)malloc(iterations * sizeof(int));

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, size_x, size_y);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		HitTile_int mat  = Ctrl_DomainAlloc(ctrl, int, hitShapeSize(size_x, size_y));
		HitTile_int mat2 = Ctrl_DomainAlloc(ctrl, int, hitShapeSize(size_x, size_y));
		HitTile_int tmp;

		// START TIME
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		float size_x = x2 - x1;
		float size_y = y2 - y1;
		for (int i = 0; i < iterations; i++) {
			// COMPUTE NEW IMAGE
			Ctrl_Launch(ctrl, Mandelbrot, threads, CTRL_THREAD_NULL, mat, threshold, x1, x2, y1, y2);

			// SWAP RESULT-COMPUTE MATRICES
			tmp  = mat2;
			mat2 = mat;
			mat  = tmp;

			// PROCESS RESULT IMAGE
			int *result = &p_results[i];
			Ctrl_HostTask(Count_And_Paint, mat2, result);
			x1 += step * size_x;
			x2 -= step * size_x;
			y1 += step * size_y;
			y2 -= step * size_y;
		}

		// STOP TIME
		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		/* PRINT RESULTS */
		#ifdef _CTRL_EXAMPLES_TEST_MODE_
		for (int i = 0; i < iterations; i++) {
			printf("%d(%d) ", i, p_results[i]);
		}
		fflush(stdout);
		#else
		printf("%d ", p_results[iterations - 1]);
		fflush(stdout);
		#endif

		Ctrl_Free(ctrl, mat, mat2);
		Ctrl_EndBlock();
	}

	free(p_results);

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
