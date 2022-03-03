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
	float x0 = x1 + (x2 - x1) / hit_tileDimCard(mat, 0) * thread_id_x;
	float y0 = y1 + (y2 - y1) / hit_tileDimCard(mat, 1) * thread_id_y;

	float x         = 0.0;
	float y         = 0.0;
	int   iteration = 0;
	while (x * x + y * y <= 2 * 2 && iteration < threshold) {
		float xtemp = x * x - y * y + x0;
		y           = 2 * x * y + y0;
		x           = xtemp;
		iteration++;
	}
	hit(mat, thread_id_x, thread_id_y) = iteration;
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

	if (argc != 13) {
		fprintf(stderr, "\nUsage: %s <sizeX> <sizeY> <x1> <x2> <y1> <y2> <step> <threshold> <iters> <device_id> <policy> <affinity>\n", argv[0]);
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
	int         device     = atoi(argv[10]);
	Ctrl_Policy policy     = (Ctrl_Policy)atoi(argv[11]);
	int         host_aff   = atoi(argv[12]);
	Ctrl_SetHostAffinity(host_aff);

	int *p_results = (int *)malloc(iterations * sizeof(int));

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, size_x, size_y);

	#ifdef _CTRL_QUEUE_
	__ctrl_block__(1, 1)
		#else
		__ctrl_block__(1, 0)
	#endif //_CTRL_QUEUE_
	{
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_CUDA, policy, device);

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
			Ctrl_HostTask(ctrl, Count_And_Paint, mat2, result);
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
		Ctrl_Destroy(ctrl);
	}

	free(p_results);

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
