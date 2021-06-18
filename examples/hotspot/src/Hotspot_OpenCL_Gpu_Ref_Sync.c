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
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "Hotspot_Constants.h"

#ifdef _PROFILING_ENABLED_
	#include <roctx.h>
#endif //_PROFILING_ENABLED_

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_DEBUG_
	#define OPENCL_ASSERT_OP( operation ) \
		err = operation; \
		fprintf(stderr, "error: %d\n", err); \
		exit(EXIT_FAILURE);

	#define OPENCL_ASSERT_ERROR( err ) \
		fprintf(stderr, "error: %d\n", err); \
		exit(EXIT_FAILURE);
#else
	#define OPENCL_ASSERT_OP( operation ) operation
	#define OPENCL_ASSERT_ERROR(err)
#endif

#define HOTSPOT_KERNEL_NAME "Hotspot"

#define HOTSPOT_KERNEL \
	"__kernel void Hotspot(const int iteration, __global const float *power, __global const float *temp_src, \
						__global float *temp_dst, const int grid_cols, const int grid_rows, const int border_cols, \
						const int border_rows, const float Cap, const float Rx, const float Ry, const float Rz, const float step) \
	{ \
		__local float temp_on_opencl[LOCAL_SIZE_0][LOCAL_SIZE_1]; \
		__local float power_on_opencl[LOCAL_SIZE_0][LOCAL_SIZE_1]; \
		__local float temp_t[LOCAL_SIZE_0][LOCAL_SIZE_1]; \
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
			if (IN_RANGE(tx, i + 1, LOCAL_SIZE_1 - i - 2) && IN_RANGE(ty, i + 1, LOCAL_SIZE_0 - i - 2) && \
				IN_RANGE(tx, validXmin, validXmax) && IN_RANGE(ty, validYmin, validYmax)) { \
				computed = true; \
				temp_t[ty][tx] = temp_on_opencl[ty][tx] + step_div_Cap * (power_on_opencl[ty][tx] + \
									(temp_on_opencl[S][tx] + temp_on_opencl[N][tx] - 2.0 * temp_on_opencl[ty][tx]) * Ry_1 + \
									(temp_on_opencl[ty][E] + temp_on_opencl[ty][W] - 2.0 * temp_on_opencl[ty][tx]) * Rx_1 + \
									(amb_temp - temp_on_opencl[ty][tx]) * Rz_1); \
			} \
			barrier(CLK_LOCAL_MEM_FENCE); \
			if (i == iteration - 1) { \
				break; \
			} \
			if (computed) {  \
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
			matrix_temp[i * cols + j] = (-1 + (2 * (((float)rand()) / RAND_MAX)));
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_power[i * cols + j] = (-1 + (2 * (((float)rand()) / RAND_MAX)));
		}
	}
}

void host_compute(float *dst, float *src, int rows, int cols) {
	#ifdef _PROFILING_ENABLED_
		roctxRangePush("Host task");
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			dst[i * cols + j] = src[i * cols + j];
		}
	}

	#ifdef _PROFILING_ENABLED_
		roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

void calc_norm(float *matrix, int rows, int cols) {
	double resultado = 0;
	double suma = 0;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			suma += pow(matrix[i * cols + j], 2);
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

int compute_tran_temp(cl_mem MatrixPower, cl_mem MatrixTemp[2], int col,
					  int row, int total_iterations, int num_iterations,
					  int blockCols, int blockRows, int borderCols,
					  int borderRows, int iters_per_copy, float *FilesavingTemp[2],
					  float *MatrixCopy, cl_kernel kernel_hotspot, cl_command_queue queue) {
	cl_int err;

	cl_event aux;

	size_t local_size[2];
	size_t global_size[2];

	local_size[0] = LOCAL_SIZE_0;
	local_size[1] = LOCAL_SIZE_1;

	global_size[0] = blockCols * local_size[0];
	global_size[1] = blockRows * local_size[1];

	float grid_height = chip_height / row;
	float grid_width = chip_width / col;

	float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	float Rx = grid_width / (2.0 * K_SI * t_chip * grid_height);
	float Ry = grid_height / (2.0 * K_SI * t_chip * grid_width);
	float Rz = t_chip / (K_SI * grid_height * grid_width);

	float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	float step = PRECISION / max_slope;

	int real_iter = 1;
	int src = 1;
	int dst = 0;

	
	err = clSetKernelArg(kernel_hotspot, 1, sizeof(cl_mem), &MatrixPower);
	err |= clSetKernelArg(kernel_hotspot, 4, sizeof(cl_int), &col);
	err |= clSetKernelArg(kernel_hotspot, 5, sizeof(cl_int), &row);
	err |= clSetKernelArg(kernel_hotspot, 6, sizeof(cl_int), &borderCols);
	err |= clSetKernelArg(kernel_hotspot, 7, sizeof(cl_int), &borderRows);
	err |= clSetKernelArg(kernel_hotspot, 8, sizeof(cl_float), &Cap);
	err |= clSetKernelArg(kernel_hotspot, 9, sizeof(cl_float), &Rx);
	err |= clSetKernelArg(kernel_hotspot, 10, sizeof(cl_float), &Ry);
	err |= clSetKernelArg(kernel_hotspot, 11, sizeof(cl_float), &Rz);
	err |= clSetKernelArg(kernel_hotspot, 12, sizeof(cl_float), &step);
	OPENCL_ASSERT_ERROR( err );

	for (int t = 0; t < total_iterations; t += num_iterations) {
		int temp = src;
		src = dst;
		dst = temp;
		
		int aux_iterations = MIN(num_iterations, total_iterations - t);

		err = clSetKernelArg(kernel_hotspot, 0, sizeof(cl_int), &aux_iterations);
		err |= clSetKernelArg(kernel_hotspot, 2, sizeof(cl_mem), &MatrixTemp[src]);
		err |= clSetKernelArg(kernel_hotspot, 3, sizeof(cl_mem), &MatrixTemp[dst]);
		OPENCL_ASSERT_ERROR( err );

		OPENCL_ASSERT_OP( clEnqueueNDRangeKernel(queue, kernel_hotspot, 2, NULL, global_size, local_size, 0, NULL, NULL) );
		OPENCL_ASSERT_OP( clFlush(queue) );

		if ((real_iter % iters_per_copy) == 0) {
			OPENCL_ASSERT_OP( clEnqueueReadBuffer(queue, MatrixTemp[dst], CL_FALSE, 0, sizeof(float) * row * col, (void *)FilesavingTemp[dst], 0, NULL, &aux) );
			OPENCL_ASSERT_OP( clFlush(queue) );
			OPENCL_ASSERT_OP( clWaitForEvents(1, &aux) );
			host_compute(MatrixCopy, FilesavingTemp[dst], row, col);
		}
		real_iter++;
	}

	return dst;
}

void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy> <device> <platform>\n", argv[0]);
	fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
	fprintf(stderr, "\t<sim_time>   - number of iterations\n");
	fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
	fprintf(stderr, "\t<device> - GPU index\n");
	fprintf(stderr, "\t<platform> - OpenCL platform index\n");
	exit(EXIT_FAILURE);
}

void run (int argc, char *argv[]) {

	/* ARGUMENTS */

	if (argc != 7) {
		usage(argc, argv);
	}
	int grid_rows = atoi(argv[1]);
	int grid_cols = atoi(argv[1]);
	int pyramid_height = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy = atoi(argv[4]);
	int DEVICE = atoi(argv[5]);
	int PLATFORM = atoi(argv[6]);

	/* --------------- pyramid parameters --------------- */

	int borderCols = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = LOCAL_SIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = LOCAL_SIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

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
	cl_device_id device_id;

	cl_context context;
	cl_program program;
	cl_kernel kernel_hotspot;

	cl_command_queue_properties properties;
	
	cl_command_queue main_command_queue;


	/* PLATFORMS & DEVICES */

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP( clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL) );
	platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP( clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL) );
	device_id = p_devices[DEVICE];
	free(p_devices);
	
	/* SET UP, CONTEXTO, COLAS, KERNELS, ETC */

	cl_context_properties context_properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0 };
	context = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	
	OPENCL_ASSERT_ERROR( err );

	program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw), (const size_t *)(&kernel_size), &err);
	OPENCL_ASSERT_ERROR( err );
	
	OPENCL_ASSERT_OP( clBuildProgram(program, 1, &device_id, NULL, NULL, NULL) );

	kernel_hotspot = clCreateKernel(program, HOTSPOT_KERNEL_NAME, &err);
	OPENCL_ASSERT_ERROR( err );

	properties = 0;
	
	main_command_queue = clCreateCommandQueue(context, device_id, properties, &err);

	FilesavingTemp_mem[0] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(float), NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	FilesavingTemp[0] = (float *)clEnqueueMapBuffer(main_command_queue, FilesavingTemp_mem[0], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size * sizeof(float), 0, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	
	FilesavingTemp_mem[1] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(float), NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	FilesavingTemp[1] = (float *)clEnqueueMapBuffer(main_command_queue, FilesavingTemp_mem[1], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size * sizeof(float), 0, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR( err );

	FilesavingPower_mem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(float), NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	FilesavingPower = (float *)clEnqueueMapBuffer(main_command_queue, FilesavingPower_mem, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size * sizeof(float), 0, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR( err );

	MatrixCopy = (float *)malloc(size * sizeof(float));

	OPENCL_ASSERT_OP( clFinish(main_command_queue) );

	if (!FilesavingPower || !FilesavingTemp[0] || !FilesavingTemp[1] || !MatrixCopy) {
		fprintf(stderr, "unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	size_t platform_name_size;
	OPENCL_ASSERT_OP( clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size) );
	char* platform_name = (char*)malloc( sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP( clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL ) );
	
	size_t device_name_size;
	OPENCL_ASSERT_OP( clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size) );
	char* device_name = (char*) malloc( sizeof(char) * device_name_size );
	OPENCL_ASSERT_OP( clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL) );

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%s, %s, ", device_name, platform_name);
	#else
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE (SIZE x SIZE): %d, %d, %d", grid_rows * grid_cols, grid_rows, grid_cols);
		printf("\n PYRAMID HEIGHT: %d", pyramid_height);
		printf("\n N_ITER: %d", total_iterations);
		printf("\n ITERS_PER_COPY: %d", iters_per_copy);
		printf("\n PLATFORM: %s", platform_name);
		printf("\n DEVICE: %s", device_name);
		printf("\n POLICY SYNC");
		printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	free(platform_name);
	free(device_name);

	init_matrix(FilesavingTemp[0], FilesavingPower, grid_rows, grid_cols);

	MatrixTemp[0] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	MatrixTemp[1] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	MatrixPower = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	OPENCL_ASSERT_ERROR( err );

	cl_event aux;

	OPENCL_ASSERT_OP( clFinish(main_command_queue) );
	exec_clock = omp_get_wtime();

	OPENCL_ASSERT_OP( clEnqueueWriteBuffer(main_command_queue, MatrixTemp[0], CL_FALSE, 0, size * sizeof(float),(void *)FilesavingTemp[0], 0, NULL, &aux) );
	OPENCL_ASSERT_OP( clFlush(main_command_queue) );
	OPENCL_ASSERT_OP( clWaitForEvents(1, &aux) );
	OPENCL_ASSERT_OP( clEnqueueWriteBuffer(main_command_queue, MatrixPower, CL_FALSE, 0, size * sizeof(float),(void *)FilesavingPower, 0, NULL, &aux) );
	OPENCL_ASSERT_OP( clFlush(main_command_queue) );
	OPENCL_ASSERT_OP( clWaitForEvents(1, &aux) );

	int ret = compute_tran_temp(MatrixPower, MatrixTemp, grid_cols, grid_rows,
								total_iterations, pyramid_height, blockCols,
								blockRows, borderCols, borderRows, iters_per_copy,
								FilesavingTemp, MatrixCopy, kernel_hotspot, main_command_queue);

	OPENCL_ASSERT_OP( clEnqueueReadBuffer(main_command_queue, MatrixTemp[ret], CL_FALSE, 0, sizeof(float) * size, (void *)FilesavingTemp[ret], 0, NULL, &aux) );
	OPENCL_ASSERT_OP( clFlush(main_command_queue) );
	OPENCL_ASSERT_OP( clWaitForEvents(1, &aux) );
	
	host_compute(MatrixCopy, FilesavingTemp[ret], grid_rows, grid_cols);

	OPENCL_ASSERT_OP( clFinish(main_command_queue) );
	exec_clock = omp_get_wtime() - exec_clock ;

	/* CALCULATION OF RESULTS */

	calc_norm(MatrixCopy, grid_rows, grid_cols);
	
	/* RELEASE ZONE */
	
	OPENCL_ASSERT_OP( clReleaseMemObject(MatrixTemp[0]) );
	OPENCL_ASSERT_OP( clReleaseMemObject(MatrixTemp[1]) );
	OPENCL_ASSERT_OP( clReleaseMemObject(MatrixPower) );
	OPENCL_ASSERT_OP( clEnqueueUnmapMemObject(main_command_queue, FilesavingTemp_mem[0], FilesavingTemp[0], 0, NULL, NULL) );
	OPENCL_ASSERT_OP( clEnqueueUnmapMemObject(main_command_queue, FilesavingTemp_mem[1], FilesavingTemp[1], 0, NULL, NULL) );
	OPENCL_ASSERT_OP( clEnqueueUnmapMemObject(main_command_queue, FilesavingPower_mem, FilesavingPower, 0, NULL, NULL) );
	
	OPENCL_ASSERT_OP( clFinish(main_command_queue) );

	OPENCL_ASSERT_OP( clReleaseMemObject(FilesavingTemp_mem[0]) );
	OPENCL_ASSERT_OP( clReleaseMemObject(FilesavingTemp_mem[1]) );
	OPENCL_ASSERT_OP( clReleaseMemObject(FilesavingPower_mem) );

	OPENCL_ASSERT_OP( clReleaseKernel(kernel_hotspot) );
	OPENCL_ASSERT_OP( clReleaseProgram(program) );
	
	OPENCL_ASSERT_OP( clReleaseCommandQueue(main_command_queue) );
	OPENCL_ASSERT_OP( clReleaseContext(context) );

	free(MatrixCopy);
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	run(argc, argv);

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

	return EXIT_SUCCESS;
}