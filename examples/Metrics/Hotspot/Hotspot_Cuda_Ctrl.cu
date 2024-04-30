#include "Ctrl.h"
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>

#define POLICY (Ctrl_Policy)1
#define DEVICE 0

#define SEED 6834723

#define STR_SIZE 256

#define MAX_PD       (3.0e6)
#define PRECISION    0.001
#define SPEC_HEAT_SI 1.75e6
#define K_SI         100
#define FACTOR_CHIP  0.5

#define EXPAND_RATE 2

#define IN_RANGE(x, min, max) ((x) >= (min) && (x) <= (max))
#define MIN(a, b)             ((a) <= (b) ? (a) : (b))

const float t_chip      = 0.0005;
const float chip_height = 0.016;
const float chip_width  = 0.016;
const float amb_temp    = 80.0;
double      main_clock;
double      exec_clock;

int grid_rows;
int grid_cols;

Ctrl_NewType(float);

CTRL_KERNEL_CHAR(Hotspot, MANUAL, BLOCKSIZE_1, BLOCKSIZE_0);

#define hotspot_params 11, INVAL, int, iteration, IN, HitTile_float, power, IN, HitTile_float, temp_src, OUT, HitTile_float, temp_dst, INVAL, int, border_cols, INVAL, int, border_rows, INVAL, float, Cap, INVAL, float, Rx, INVAL, float, Ry, INVAL, float, Rz, INVAL, float, step

#define init_params 2, OUT, HitTile_float, matrix_temp, OUT, HitTile_float, matrix_power

#define compute_params 2, INVAL, HitTile_float, matrix_dst, IN, HitTile_float, matrix_src

CTRL_KERNEL(Hotspot, CUDA, DEFAULT, CTRL_KPARAMS(hotspot_params), {
	__shared__ float temp_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float power_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float temp_t[BLOCKSIZE_1][BLOCKSIZE_0];

	float amb_temp = 80.0;
	float step_div_Cap;
	float Rx_1;
	float Ry_1;
	float Rz_1;

	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	step_div_Cap = step / Cap;

	Rx_1 = 1 / Rx;
	Ry_1 = 1 / Ry;
	Rz_1 = 1 / Rz;

	int small_block_rows = BLOCKSIZE_0 - iteration * 2;
	int small_block_cols = BLOCKSIZE_1 - iteration * 2;

	int blkY    = small_block_rows * by - border_rows;
	int blkX    = small_block_cols * bx - border_cols;
	int blkYmax = blkY + BLOCKSIZE_0 - 1;
	int blkXmax = blkX + BLOCKSIZE_1 - 1;

	int yidx = blkY + ty;
	int xidx = blkX + tx;

	int loadYidx = yidx;
	int loadXidx = xidx;
	int index    = hit_tileDimCard(power, 1) * loadYidx + loadXidx;

	if (IN_RANGE(loadYidx, 0, hit_tileDimCard(power, 0) - 1) && IN_RANGE(loadXidx, 0, hit_tileDimCard(power, 1) - 1)) {
		temp_on_cuda[ty][tx]  = hit(temp_src, index);
		power_on_cuda[ty][tx] = hit(power, index);
	}
	__syncthreads();

	int validYmin = (blkY < 0) ? -blkY : 0;
	int validYmax = (blkYmax > hit_tileDimCard(power, 0) - 1) ? BLOCKSIZE_1 - 1 - (blkYmax - hit_tileDimCard(power, 0) + 1) : BLOCKSIZE_1 - 1;
	int validXmin = (blkX < 0) ? -blkX : 0;
	int validXmax = (blkXmax > hit_tileDimCard(power, 1) - 1) ? BLOCKSIZE_0 - 1 - (blkXmax - hit_tileDimCard(power, 1) + 1) : BLOCKSIZE_0 - 1;

	int N = ty - 1;
	int S = ty + 1;
	int W = tx - 1;
	int E = tx + 1;

	N = (N < validYmin) ? validYmin : N;
	S = (S > validYmax) ? validYmax : S;
	W = (W < validXmin) ? validXmin : W;
	E = (E > validXmax) ? validXmax : E;

	bool computed;
	for (int i = 0; i < iteration; i++) {
		computed = false;
		if (IN_RANGE(tx, i + 1, BLOCKSIZE_1 - i - 2) && IN_RANGE(ty, i + 1, BLOCKSIZE_0 - i - 2) &&
			IN_RANGE(tx, validXmin, validXmax) && IN_RANGE(ty, validYmin, validYmax)) {
			computed       = true;
			temp_t[ty][tx] = temp_on_cuda[ty][tx] + step_div_Cap * (power_on_cuda[ty][tx] + (temp_on_cuda[S][tx] + temp_on_cuda[N][tx] - 2.0 * temp_on_cuda[ty][tx]) * Ry_1 + (temp_on_cuda[ty][E] + temp_on_cuda[ty][W] - 2.0 * temp_on_cuda[ty][tx]) * Rx_1 + (amb_temp - temp_on_cuda[ty][tx]) * Rz_1);
		}
		__syncthreads();
		if (i == iteration - 1) {
			break;
		}
		if (computed) {
			temp_on_cuda[ty][tx] = temp_t[ty][tx];
		}
		__syncthreads();
	}

	if (computed) {
		hit(temp_dst, index) = temp_t[ty][tx];
	}
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

CTRL_KERNEL_PROTO(Hotspot, 1, CUDA, DEFAULT, hotspot_params);
CTRL_HOST_TASK_PROTO(Init_Tiles, init_params);
CTRL_HOST_TASK_PROTO(Host_Compute, compute_params);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 5) {
		fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time>\n", argv[0]);
		fprintf(stderr, "\t<grid_rows/grid_cols> - number of rows/cols in the grid (positive integer)\n");
		fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
		fprintf(stderr, "\t<sim_time> - number of iterations\n");
		fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
		exit(EXIT_FAILURE);
	}

	grid_rows            = atoi(argv[1]);
	grid_cols            = atoi(argv[1]);
	int pyramid_height   = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy   = atoi(argv[4]);

	int borderCols    = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows    = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = BLOCKSIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = BLOCKSIZE_1 - (pyramid_height)*EXPAND_RATE;
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

	Ctrl_Thread threads;
	Ctrl_ThreadInit(threads, BLOCKSIZE_1 * blockRows, BLOCKSIZE_0 * blockCols);

	__ctrl_block__(ctrl_conf_file) {

		PCtrl ctrl = Ctrl_Get(0);

		HitTile_float MatrixTemp[2], MatrixPower;
		HitShape      shape      = hitShapeSize(grid_rows, grid_cols);
		MatrixTemp[0]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixTemp[1]            = Ctrl_DomainAlloc(ctrl, float, shape);
		MatrixPower              = Ctrl_DomainAlloc(ctrl, float, shape);
		HitTile_float MatrixCopy = hitTile(float, shape);

		Ctrl_HostTask(Init_Tiles, MatrixTemp[0], MatrixPower);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		int real_iter = 1;
		int src       = 1;
		int dst       = 0;

		for (int i = 0; i < total_iterations; i += pyramid_height) {
			int temp = src;
			src      = dst;
			dst      = temp;

			int aux_iterations = MIN(pyramid_height, total_iterations - i);
			Ctrl_Launch(ctrl, Hotspot, threads, CTRL_THREAD_NULL, aux_iterations, MatrixPower, MatrixTemp[src], MatrixTemp[dst], borderCols, borderRows, Cap, Rx, Ry, Rz, step);

			if ((real_iter % iters_per_copy) == 0) {
				Ctrl_HostTask(Host_Compute, MatrixCopy, MatrixTemp[dst]);
			}
			real_iter++;
		}

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		Ctrl_Free(ctrl, MatrixTemp[0], MatrixTemp[1], MatrixPower);
		hit_tileFree(MatrixCopy);
		Ctrl_EndBlock();
	}

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
