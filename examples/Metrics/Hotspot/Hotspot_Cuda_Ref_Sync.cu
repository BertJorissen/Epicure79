#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <omp.h>
#include <stdbool.h>

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

double main_clock;
double exec_clock;

void init_matrix(float *matrix_temp, float *matrix_power, int grid_rows, int grid_cols) {
	srand(SEED);
	for (int i = 0; i < grid_rows; i++) {
		for (int j = 0; j < grid_cols; j++) {
			matrix_temp[i * grid_cols + j] = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
	for (int i = 0; i < grid_rows; i++) {
		for (int j = 0; j < grid_cols; j++) {
			matrix_power[i * grid_cols + j] = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
}

void host_compute(float *dst, float *src, int grid_rows, int grid_cols) {
	for (int i = 0; i < grid_rows * grid_cols; i++) {
		dst[i] = src[i];
	}
}

__global__ void calculate_temp(int iteration, float *power, float *temp_src, float *temp_dst, int grid_cols, int grid_rows, int border_cols, int border_rows, float Cap, float Rx, float Ry, float Rz, float step) {
	__shared__ float temp_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float power_on_cuda[BLOCKSIZE_1][BLOCKSIZE_0];
	__shared__ float temp_t[BLOCKSIZE_1][BLOCKSIZE_0]; // saving temparary temperature result

	const float amb_temp = 80.0;
	float       step_div_Cap;
	float       Rx_1, Ry_1, Rz_1;

	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	step_div_Cap = step / Cap;

	Rx_1 = 1 / Rx;
	Ry_1 = 1 / Ry;
	Rz_1 = 1 / Rz;

	int small_block_rows = BLOCKSIZE_0 - iteration * 2; // EXPAND_RATE
	int small_block_cols = BLOCKSIZE_1 - iteration * 2; // EXPAND_RATE

	int blkY    = small_block_rows * by - border_rows;
	int blkX    = small_block_cols * bx - border_cols;
	int blkYmax = blkY + BLOCKSIZE_0 - 1;
	int blkXmax = blkX + BLOCKSIZE_1 - 1;

	int yidx = blkY + ty;
	int xidx = blkX + tx;

	int loadYidx = yidx, loadXidx = xidx;
	int index = grid_cols * loadYidx + loadXidx;

	if (IN_RANGE(loadYidx, 0, grid_rows - 1) && IN_RANGE(loadXidx, 0, grid_cols - 1)) {
		temp_on_cuda[ty][tx]  = temp_src[index]; // Load the temperature data from global memory to shared memory
		power_on_cuda[ty][tx] = power[index];    // Load the power data from global memory to shared memory
	}
	__syncthreads();

	int validYmin = (blkY < 0) ? -blkY : 0;
	int validYmax = (blkYmax > grid_rows - 1) ? BLOCKSIZE_1 - 1 - (blkYmax - grid_rows + 1) : BLOCKSIZE_1 - 1;
	int validXmin = (blkX < 0) ? -blkX : 0;
	int validXmax = (blkXmax > grid_cols - 1) ? BLOCKSIZE_0 - 1 - (blkXmax - grid_cols + 1) : BLOCKSIZE_0 - 1;

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
		if (IN_RANGE(tx, i + 1, BLOCKSIZE_1 - i - 2) && IN_RANGE(ty, i + 1, BLOCKSIZE_0 - i - 2) && IN_RANGE(tx, validXmin, validXmax) && IN_RANGE(ty, validYmin, validYmax)) {
			computed       = true;
			temp_t[ty][tx] = temp_on_cuda[ty][tx] + step_div_Cap * (power_on_cuda[ty][tx] + (temp_on_cuda[S][tx] + temp_on_cuda[N][tx] - 2.0 * temp_on_cuda[ty][tx]) * Ry_1 + (temp_on_cuda[ty][E] + temp_on_cuda[ty][W] - 2.0 * temp_on_cuda[ty][tx]) * Rx_1 + (amb_temp - temp_on_cuda[ty][tx]) * Rz_1);
		}
		__syncthreads();
		if (i == iteration - 1) {
			break;
		}
		if (computed) { // Assign the computation range
			temp_on_cuda[ty][tx] = temp_t[ty][tx];
		}
		__syncthreads();
	}

	if (computed) {
		temp_dst[index] = temp_t[ty][tx];
	}
}

int compute_tran_temp(float *MatrixPower, float *MatrixTemp[2], int col, int row, int total_iterations, int num_iterations, int blockCols, int blockRows, int borderCols, int borderRows, int iters_per_copy, float *FilesavingTemp[2], float *MatrixCopy) {
	dim3 dimBlock(BLOCKSIZE_0, BLOCKSIZE_1);
	dim3 dimGrid(blockCols, blockRows);

	float grid_height = chip_height / row;
	float grid_width  = chip_width / col;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx  = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry  = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz  = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step      = PRECISION / max_slope / 1000.0;

	int real_iter = 1;
	int src       = 1;
	int dst       = 0;

	for (int t = 0; t < total_iterations; t += num_iterations) {
		int temp = src;
		src      = dst;
		dst      = temp;
		calculate_temp<<<dimGrid, dimBlock>>>(MIN(num_iterations, total_iterations - t), MatrixPower, MatrixTemp[src], MatrixTemp[dst], col, row, borderCols, borderRows, Cap, Rx, Ry, Rz, step);

		if ((real_iter % iters_per_copy) == 0) {
			cudaMemcpy(FilesavingTemp[dst], MatrixTemp[dst], sizeof(float) * row * col, cudaMemcpyDeviceToHost);
			host_compute(MatrixCopy, FilesavingTemp[dst], row, col);
		}
		real_iter++;
	}

	return dst;
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	if (argc != 5) {
		fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy>\n", argv[0]);
		fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
		fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
		fprintf(stderr, "\t<sim_time>   - number of iterations\n");
		fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
		exit(EXIT_FAILURE);
	}
	int grid_rows        = atoi(argv[1]);
	int grid_cols        = atoi(argv[1]);
	int pyramid_height   = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy   = atoi(argv[4]);

	/* --------------- pyramid parameters --------------- */

	int borderCols    = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows    = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = BLOCKSIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = BLOCKSIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols     = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows     = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	int size = grid_rows * grid_cols;

	float *FilesavingTemp[2];
	float *FilesavingPower;
	float *MatrixCopy;

	cudaMallocHost((void **)&FilesavingTemp[0], size * sizeof(float));
	cudaMallocHost((void **)&FilesavingTemp[1], size * sizeof(float));
	cudaMallocHost((void **)&FilesavingPower, size * sizeof(float));
	MatrixCopy = (float *)malloc(size * sizeof(float));

	init_matrix(FilesavingTemp[0], FilesavingPower, grid_rows, grid_cols);

	float *MatrixTemp[2];
	float *MatrixPower;
	cudaMalloc((void **)&MatrixTemp[0], sizeof(float) * size);
	cudaMalloc((void **)&MatrixTemp[1], sizeof(float) * size);
	cudaMalloc((void **)&MatrixPower, sizeof(float) * size);

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	cudaMemcpy(MatrixTemp[0], FilesavingTemp[0], sizeof(float) * size, cudaMemcpyHostToDevice);
	cudaMemcpy(MatrixPower, FilesavingPower, sizeof(float) * size, cudaMemcpyHostToDevice);

	int ret = compute_tran_temp(MatrixPower, MatrixTemp, grid_cols, grid_rows, total_iterations, pyramid_height, blockCols, blockRows, borderCols, borderRows, iters_per_copy, FilesavingTemp, MatrixCopy);
	cudaMemcpy(FilesavingTemp[ret], MatrixTemp[ret], sizeof(float) * size, cudaMemcpyDeviceToHost);
	host_compute(MatrixCopy, FilesavingTemp[ret], grid_rows, grid_cols);

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	cudaFree(MatrixPower);
	cudaFree(MatrixTemp[0]);
	cudaFree(MatrixTemp[1]);
	cudaFreeHost(FilesavingTemp[0]);
	cudaFreeHost(FilesavingTemp[1]);
	cudaFreeHost(FilesavingPower);
	free(MatrixCopy);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
