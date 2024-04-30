#include <CL/cl.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEED 6834723

#define STR_SIZE 256

#define DEVICE 0

/* maximum power density possible (say 300W for a 10mm x 10mm chip) */
#define MAX_PD (3.0e6)
/* required precision in degrees */
#define PRECISION    0.001
#define SPEC_HEAT_SI 1.75e6
#define K_SI         100
/* capacitance fitting factor */
#define FACTOR_CHIP 0.5

#define EXPAND_RATE 2 // add one iteration will extend the pyramid base by 2 per each borderline

#define IN_RANGE(x, min, max) ((x) >= (min) && (x) <= (max))
#define MIN(a, b)             ((a) <= (b) ? (a) : (b))

/* chip parameters	*/
const float t_chip      = 0.0005;
const float chip_height = 0.016;
const float chip_width  = 0.016;
/* ambient temperature, assuming no package at all	*/
const float amb_temp = 80.0;

#define HOTSPOT_KERNEL_NAME "Hotspot"

#define HOTSPOT_KERNEL "__kernel void Hotspot(const int iteration, __global const float *power, __global const float *temp_src, __global float *temp_dst, const int grid_cols, const int grid_rows, const int border_cols, const int border_rows, const float Cap, const float Rx, const float Ry, const float Rz, const float step) { \
		__local float temp_on_opencl[LOCAL_SIZE_1][LOCAL_SIZE_0]; \
		__local float power_on_opencl[LOCAL_SIZE_1][LOCAL_SIZE_0]; \
		__local float temp_t[LOCAL_SIZE_1][LOCAL_SIZE_0]; \
	 \
		float amb_temp = 80.0; \
		float step_div_Cap; \
		float Rx_1; \
		float Ry_1; \
		float Rz_1; \
 	 \
		int bx = get_group_id(0); \
		int by = get_group_id(1); \
 	 \
		int tx = get_local_id(0); \
		int ty = get_local_id(1); \
 	 \
		step_div_Cap = step / Cap; \
 	 \
		Rx_1 = 1 / Rx; \
		Ry_1 = 1 / Ry; \
		Rz_1 = 1 / Rz; \
 	 \
		int small_block_rows = LOCAL_SIZE_0 - iteration * 2; \
		int small_block_cols = LOCAL_SIZE_1 - iteration * 2; \
		 \
		int blkY = small_block_rows * by - border_rows; \
		int blkX = small_block_cols * bx - border_cols; \
		int blkYmax = blkY + LOCAL_SIZE_0 - 1; \
		int blkXmax = blkX + LOCAL_SIZE_1 - 1; \
		 \
		int yidx = blkY + ty; \
		int xidx = blkX + tx; \
		 \
		int loadYidx = yidx; \
		int loadXidx = xidx; \
		int index = grid_cols * loadYidx + loadXidx; \
		 \
		if (IN_RANGE(loadYidx, 0, grid_rows - 1) && IN_RANGE(loadXidx, 0, grid_cols - 1)) { \
			temp_on_opencl[ty][tx] = temp_src[index]; \
			power_on_opencl[ty][tx] = power[index]; \
		} \
		barrier(CLK_LOCAL_MEM_FENCE); \
		 \
		int validYmin = (blkY < 0) ? -blkY : 0; \
		int validYmax = (blkYmax > grid_rows - 1) ? LOCAL_SIZE_1 - 1 - (blkYmax - grid_rows + 1) : LOCAL_SIZE_1 - 1; \
		int validXmin = (blkX < 0) ? -blkX : 0; \
		int validXmax = (blkXmax > grid_cols - 1) ? LOCAL_SIZE_0 - 1 - (blkXmax - grid_cols + 1) : LOCAL_SIZE_0 - 1; \
		 \
		int N = ty - 1; \
		int S = ty + 1; \
		int W = tx - 1; \
		int E = tx + 1; \
		 \
		N = (N < validYmin) ? validYmin : N; \
		S = (S > validYmax) ? validYmax : S; \
		W = (W < validXmin) ? validXmin : W; \
		E = (E > validXmax) ? validXmax : E; \
		 \
		bool computed; \
		for (int i = 0; i < iteration; i++) { \
			computed = false; \
			if (IN_RANGE(tx, i + 1, LOCAL_SIZE_1 - i - 2) && IN_RANGE(ty, i + 1, LOCAL_SIZE_0 - i - 2) && IN_RANGE(tx, validXmin, validXmax) && IN_RANGE(ty, validYmin, validYmax)) { \
				computed = true; \
				temp_t[ty][tx] = temp_on_opencl[ty][tx] + step_div_Cap * (power_on_opencl[ty][tx] + (temp_on_opencl[S][tx] + temp_on_opencl[N][tx] - 2.0 * temp_on_opencl[ty][tx]) * Ry_1 + (temp_on_opencl[ty][E] + temp_on_opencl[ty][W] - 2.0 * temp_on_opencl[ty][tx]) * Rx_1 + (amb_temp - temp_on_opencl[ty][tx]) * Rz_1); \
			} \
			barrier(CLK_LOCAL_MEM_FENCE); \
			if (i == iteration - 1) { \
				break; \
			} \
			if (computed) { \
				temp_on_opencl[ty][tx] = temp_t[ty][tx]; \
			} \
			barrier(CLK_LOCAL_MEM_FENCE); \
		} \
		 \
		if (computed) { \
			temp_dst[index] = temp_t[ty][tx]; \
		} \
	}"

double main_clock;
double exec_clock;

void init_matrix(float *matrix_temp, float *matrix_power, int rows, int cols) {
	srand(SEED);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_temp[i * cols + j] = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_power[i * cols + j] = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}
}

void host_compute(float *dst, float *src, int rows, int cols) {
	for (int i = 0; i < rows * cols; i++) {
		dst[i] = src[i];
	}
}

int compute_tran_temp(cl_mem MatrixPower, cl_mem MatrixTemp[2], int col, int row, int total_iterations, int num_iterations, int blockCols, int blockRows, int borderCols, int borderRows, int iters_per_copy, float *FilesavingTemp[2], float *MatrixCopy, cl_kernel kernel_hotspot, cl_command_queue queue) {

	cl_event aux;

	size_t local_size[2];
	size_t global_size[2];

	local_size[0] = LOCAL_SIZE_0;
	local_size[1] = LOCAL_SIZE_1;

	global_size[0] = blockCols * local_size[0];
	global_size[1] = blockRows * local_size[1];

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

	clSetKernelArg(kernel_hotspot, 1, sizeof(cl_mem), &MatrixPower);
	clSetKernelArg(kernel_hotspot, 4, sizeof(cl_int), &col);
	clSetKernelArg(kernel_hotspot, 5, sizeof(cl_int), &row);
	clSetKernelArg(kernel_hotspot, 6, sizeof(cl_int), &borderCols);
	clSetKernelArg(kernel_hotspot, 7, sizeof(cl_int), &borderRows);
	clSetKernelArg(kernel_hotspot, 8, sizeof(cl_float), &Cap);
	clSetKernelArg(kernel_hotspot, 9, sizeof(cl_float), &Rx);
	clSetKernelArg(kernel_hotspot, 10, sizeof(cl_float), &Ry);
	clSetKernelArg(kernel_hotspot, 11, sizeof(cl_float), &Rz);
	clSetKernelArg(kernel_hotspot, 12, sizeof(cl_float), &step);

	for (int t = 0; t < total_iterations; t += num_iterations) {
		int temp = src;
		src      = dst;
		dst      = temp;

		int aux_iterations = MIN(num_iterations, total_iterations - t);

		clSetKernelArg(kernel_hotspot, 0, sizeof(cl_int), &aux_iterations);
		clSetKernelArg(kernel_hotspot, 2, sizeof(cl_mem), &MatrixTemp[src]);
		clSetKernelArg(kernel_hotspot, 3, sizeof(cl_mem), &MatrixTemp[dst]);

		clEnqueueNDRangeKernel(queue, kernel_hotspot, 2, NULL, global_size, local_size, 0, NULL, NULL);
		clFlush(queue);

		if ((real_iter % iters_per_copy) == 0) {
			clEnqueueReadBuffer(queue, MatrixTemp[dst], CL_FALSE, 0, sizeof(float) * row * col, (void *)FilesavingTemp[dst], 0, NULL, &aux);
			clFlush(queue);
			clWaitForEvents(1, &aux);
			host_compute(MatrixCopy, FilesavingTemp[dst], row, col);
		}
		real_iter++;
	}

	return dst;
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/* ARGUMENTS */

	if (argc != 6) {
		fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy> <device> <platform>\n", argv[0]);
		fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
		fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
		fprintf(stderr, "\t<sim_time>   - number of iterations\n");
		fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
		fprintf(stderr, "\t<platform> - OpenCL platform index\n");
		exit(EXIT_FAILURE);
	}
	int grid_rows        = atoi(argv[1]);
	int grid_cols        = atoi(argv[1]);
	int pyramid_height   = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy   = atoi(argv[4]);
	int PLATFORM         = atoi(argv[5]);

	/* --------------- pyramid parameters --------------- */

	int borderCols    = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows    = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = LOCAL_SIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = LOCAL_SIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols     = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows     = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	int size = grid_rows * grid_cols;

	cl_mem FilesavingTemp_mem[2];
	cl_mem FilesavingPower_mem;

	cl_mem MatrixTemp[2];
	cl_mem MatrixPower;

	float *FilesavingTemp[2];
	float *FilesavingPower;
	float *MatrixCopy;

	/* VARIABLES */

	cl_int err;

	char *kernel_raw_hotspot = HOTSPOT_KERNEL;

	char *kernel_raw = (char *)malloc((strlen(kernel_raw_hotspot) + 300) * sizeof(char));
	sprintf(&kernel_raw[0], " \n #define IN_RANGE(x, min, max) ((x) >= (min) && (x) <= (max)) \n #define LOCAL_SIZE_0  %d \n#define LOCAL_SIZE_1 %d \n", LOCAL_SIZE_0, LOCAL_SIZE_1);
	strcat(kernel_raw, kernel_raw_hotspot);

	size_t kernel_size = strlen(kernel_raw);

	cl_platform_id platform_id;
	cl_device_id   device_id;

	cl_context context;
	cl_program program;
	cl_kernel  kernel_hotspot;

	cl_command_queue_properties properties;

	cl_command_queue main_command_queue;

	/* PLATFORMS & DEVICES */
	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL);
	platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL);
	device_id = p_devices[DEVICE];
	free(p_devices);

	/* SET UP, CONTEXTO, COLAS, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);

	program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw), (const size_t *)(&kernel_size), &err);

	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel_hotspot = clCreateKernel(program, HOTSPOT_KERNEL_NAME, &err);

	properties = 0;

	main_command_queue = clCreateCommandQueue(context, device_id, properties, &err);

	FilesavingTemp_mem[0] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(float), NULL, &err);
	FilesavingTemp[0]     = (float *)clEnqueueMapBuffer(main_command_queue, FilesavingTemp_mem[0], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size * sizeof(float), 0, NULL, NULL, &err);

	FilesavingTemp_mem[1] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(float), NULL, &err);
	FilesavingTemp[1]     = (float *)clEnqueueMapBuffer(main_command_queue, FilesavingTemp_mem[1], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size * sizeof(float), 0, NULL, NULL, &err);

	FilesavingPower_mem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(float), NULL, &err);
	FilesavingPower     = (float *)clEnqueueMapBuffer(main_command_queue, FilesavingPower_mem, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size * sizeof(float), 0, NULL, NULL, &err);

	MatrixCopy = (float *)malloc(size * sizeof(float));

	clFinish(main_command_queue);

	if (!FilesavingPower || !FilesavingTemp[0] || !FilesavingTemp[1] || !MatrixCopy) {
		fprintf(stderr, "unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	init_matrix(FilesavingTemp[0], FilesavingPower, grid_rows, grid_cols);

	MatrixTemp[0] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	MatrixTemp[1] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	MatrixPower   = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);

	cl_event aux;

	clFinish(main_command_queue);
	exec_clock = omp_get_wtime();

	clEnqueueWriteBuffer(main_command_queue, MatrixTemp[0], CL_FALSE, 0, size * sizeof(float), (void *)FilesavingTemp[0], 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);
	clEnqueueWriteBuffer(main_command_queue, MatrixPower, CL_FALSE, 0, size * sizeof(float), (void *)FilesavingPower, 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);

	int ret = compute_tran_temp(MatrixPower, MatrixTemp, grid_cols, grid_rows, total_iterations, pyramid_height, blockCols, blockRows, borderCols, borderRows, iters_per_copy, FilesavingTemp, MatrixCopy, kernel_hotspot, main_command_queue);

	clEnqueueReadBuffer(main_command_queue, MatrixTemp[ret], CL_FALSE, 0, sizeof(float) * size, (void *)FilesavingTemp[ret], 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);

	host_compute(MatrixCopy, FilesavingTemp[ret], grid_rows, grid_cols);

	clFinish(main_command_queue);
	exec_clock = omp_get_wtime() - exec_clock;

	/* RELEASE ZONE */

	clReleaseMemObject(MatrixTemp[0]);
	clReleaseMemObject(MatrixTemp[1]);
	clReleaseMemObject(MatrixPower);
	clEnqueueUnmapMemObject(main_command_queue, FilesavingTemp_mem[0], FilesavingTemp[0], 0, NULL, NULL);
	clEnqueueUnmapMemObject(main_command_queue, FilesavingTemp_mem[1], FilesavingTemp[1], 0, NULL, NULL);
	clEnqueueUnmapMemObject(main_command_queue, FilesavingPower_mem, FilesavingPower, 0, NULL, NULL);

	clFinish(main_command_queue);

	clReleaseMemObject(FilesavingTemp_mem[0]);
	clReleaseMemObject(FilesavingTemp_mem[1]);
	clReleaseMemObject(FilesavingPower_mem);

	clReleaseKernel(kernel_hotspot);
	clReleaseProgram(program);

	clReleaseCommandQueue(main_command_queue);
	clReleaseContext(context);

	free(MatrixCopy);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
