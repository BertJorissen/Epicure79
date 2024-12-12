/**
 * @file Hotspot_FPGA_Ref_Sync.c
 * @brief Hotspot: Synchronous native FPGA version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 *
 * @copyright This file is part of a modified version of a Rodinia benchmark. Thus the following applies:
 * @copyright Copyright (c)2008-2014 University of Virginia. All rights reserved.
 *
 * @copyright Redistribution and use in source and binary forms, with or without modification, are permitted
 * without royalty fees or other restrictions, provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Virginia, the Dept. of Computer Science,
 *    nor the names of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE UNIVERSITY OF VIRGINIA OR THE SOFTWARE AUTHORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#define FPGA_EMULATION 1
#define FPGA_PROFILING 2

#define AOCL_ALIGNMENT 64

#include "Hotspot_Constants.h"
#include <CL/cl.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_NOT_FOUND -1
#define ERR_READ      -2

#define _STRINGIFY(x) #x
#define STRINGIFY(x)  _STRINGIFY(x)

#define SSIZE 8

#define HOTSPOT_KERNEL_NAME "Hotspot"

double main_clock;
double exec_clock;

void init_matrix(float *matrix_temp, float *matrix_power, int rows, int cols) {
	srand(SEED);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_temp[i * cols + j] = (-1 + (2 * (((float)rand()) / (float)RAND_MAX)));
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix_power[i * cols + j] = (-1 + (2 * (((float)rand()) / (float)RAND_MAX)));
		}
	}
}

void host_compute(float *dst, float *src, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			dst[i * cols + j] = src[i * cols + j];
		}
	}
}

void calc_norm(float *matrix, int rows, int cols) {
	double resultado = 0;
	double suma      = 0;

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

	#ifndef _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ------------------ WORK-GROUP-INFO ----------------- \n");
	printf("\n Local Size: %lu x %lu", local_size[0], local_size[1]);
	printf("\n Global Size: %lu x %lu", global_size[0], global_size[1]);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

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

	int BLOCK_X = LOCAL_SIZE_0;
	// Exit condition should be a multiple of comp_bsize
	int comp_bsize = BLOCK_X - 2;
	int last_col   = (col % comp_bsize == 0) ? col + 0 : col + comp_bsize - col % comp_bsize;
	int col_blocks = last_col / comp_bsize;
	int comp_exit  = BLOCK_X * col_blocks * (row + 1) / SSIZE;

	float step_div_Cap = step / Cap;
	float Rx_1         = 1 / Rx;
	float Ry_1         = 1 / Ry;
	float Rz_1         = 1 / Rz;

	err = clSetKernelArg(kernel_hotspot, 0, sizeof(cl_mem), &MatrixPower);
	err |= clSetKernelArg(kernel_hotspot, 3, sizeof(cl_int), &col);
	err |= clSetKernelArg(kernel_hotspot, 4, sizeof(cl_int), &row);
	err |= clSetKernelArg(kernel_hotspot, 5, sizeof(cl_float), &step_div_Cap);
	err |= clSetKernelArg(kernel_hotspot, 6, sizeof(cl_float), &Rx_1);
	err |= clSetKernelArg(kernel_hotspot, 7, sizeof(cl_float), &Ry_1);
	err |= clSetKernelArg(kernel_hotspot, 8, sizeof(cl_float), &Rz_1);
	err |= clSetKernelArg(kernel_hotspot, 9, sizeof(cl_int), &comp_exit);

	for (int t = 0; t < total_iterations; t += 1) {
		int temp = src;
		src      = dst;
		dst      = temp;

		err |= clSetKernelArg(kernel_hotspot, 1, sizeof(cl_mem), &MatrixTemp[src]);
		err |= clSetKernelArg(kernel_hotspot, 2, sizeof(cl_mem), &MatrixTemp[dst]);

		clEnqueueTask(queue, kernel_hotspot, 0, NULL, NULL);
		clFlush(queue);

		if ((real_iter % iters_per_copy) == 0) {
			clEnqueueReadBuffer(queue, MatrixTemp[1], CL_FALSE, 0, sizeof(float) * row * col, (void *)FilesavingTemp[1], 0, NULL, &aux);
			clFlush(queue);
			clWaitForEvents(1, &aux);
			host_compute(MatrixCopy, FilesavingTemp[dst], row, col);
		}
		real_iter++;
	}

	return dst;
}

void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <iters_per_copy> <device> <platform> <exec_mode>\n", argv[0]);
	fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
	fprintf(stderr, "\t<sim_time>   - number of iterations\n");
	fprintf(stderr, "\t<iters_per_copy> - nº of iter between each copy back\n");
	fprintf(stderr, "\t<device> - GPU index\n");
	fprintf(stderr, "\t<platform> - OpenCL platform index\n");
	fprintf(stderr, "\t<exec_mode> - FPGA execution mode (0 - default, 1 - emulation, 2 - profiling)\n");
	exit(EXIT_FAILURE);
}

void run(int argc, char *argv[]) {

	/* ARGUMENTS */

	if (argc != 8) {
		usage(argc, argv);
	}
	int grid_rows        = atoi(argv[1]);
	int grid_cols        = atoi(argv[1]);
	int pyramid_height   = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy   = atoi(argv[4]);
	int DEVICE           = atoi(argv[5]);
	int PLATFORM         = atoi(argv[6]);
	int EXEC_MODE        = atoi(argv[7]);

	/* --------------- pyramid parameters --------------- */

	int borderCols    = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows    = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = LOCAL_SIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = LOCAL_SIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols     = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows     = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	int size = grid_rows * grid_cols;

	cl_mem MatrixTemp[2];
	cl_mem MatrixPower;

	float *FilesavingTemp[2];
	float *FilesavingPower;
	float *MatrixCopy;

	/* VARIABLES */

	cl_int err;

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
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, DEVICE + 1, p_devices, NULL);
	device_id = p_devices[DEVICE];
	free(p_devices);

	/* SET UP, CONTEXTO, COLAS, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);

	FILE *binary_file;
	char *kernel_path = (char *)malloc(500 * sizeof(char));
	kernel_path[0]    = '\0';
	strcat(kernel_path, STRINGIFY(REF_KERNEL_PATH));
	strcat(kernel_path, HOTSPOT_KERNEL_NAME);
	strcat(kernel_path, "/");
	strcat(kernel_path, HOTSPOT_KERNEL_NAME);
	if (EXEC_MODE == FPGA_PROFILING)
		strcat(kernel_path, "_profiling");
	else if (EXEC_MODE == FPGA_EMULATION)
		strcat(kernel_path, "_emu");
	#ifdef _INTEL_KERNELS
	strcat(kernel_path, "_Ref.aocx");
	#elif _XILINX_KERNELS
	strcat(kernel_path, "_Ref.xclbin");
	#endif
	if (!(binary_file = fopen(kernel_path, "rb"))) {
		printf("Kernel file not found.\n");
		exit(ERR_NOT_FOUND);
	}
	fseek(binary_file, 0, SEEK_END);
	size_t         binary_length = ftell(binary_file);
	unsigned char *binary_str    = (unsigned char *)malloc(binary_length * sizeof(unsigned char));
	rewind(binary_file);
	if (!(fread(binary_str, binary_length, 1, binary_file))) {
		printf("Error reading kernel file\n");
		exit(ERR_READ);
	}

	program = clCreateProgramWithBinary(context, 1, &device_id, (const size_t *)&binary_length, (const unsigned char **)&binary_str, NULL, &err);

	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel_hotspot = clCreateKernel(program, HOTSPOT_KERNEL_NAME, &err);

	properties = 0;

	main_command_queue = clCreateCommandQueue(context, device_id, properties, &err);

	posix_memalign((void **)&FilesavingTemp[0], AOCL_ALIGNMENT, size * sizeof(float));

	posix_memalign((void **)&FilesavingTemp[1], AOCL_ALIGNMENT, size * sizeof(float));

	posix_memalign((void **)&FilesavingPower, AOCL_ALIGNMENT, size * sizeof(float));

	MatrixCopy = (float *)malloc(size * sizeof(float));

	clFinish(main_command_queue);

	if (!FilesavingPower || !FilesavingTemp[0] || !FilesavingTemp[1] || !MatrixCopy) {
		if (!FilesavingPower)
			printf("FilesavingPower\n");
		if (!FilesavingTemp[0])
			printf("FilesavingTemp[0]\n");
		if (!FilesavingTemp[1])
			printf("FilesavingTemp[1]\n");
		if (!MatrixCopy)
			printf("Matrixcopy\n");
		fprintf(stderr, "unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	size_t platform_name_size;
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size);
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL);

	size_t device_name_size;
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size);
	char *device_name = (char *)malloc(sizeof(char) * device_name_size);
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("FPGA-%s-%s, ", info.device_name, info.platform_name);
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

	MatrixTemp[1] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);

	MatrixPower = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);

	cl_event aux;

	clFinish(main_command_queue);
	exec_clock = omp_get_wtime();

	clEnqueueWriteBuffer(main_command_queue, MatrixTemp[0], CL_FALSE, 0, size * sizeof(float), (void *)FilesavingTemp[0], 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);
	clEnqueueWriteBuffer(main_command_queue, MatrixPower, CL_FALSE, 0, size * sizeof(float), (void *)FilesavingPower, 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);

	int ret = compute_tran_temp(MatrixPower, MatrixTemp, grid_cols, grid_rows,
								total_iterations, pyramid_height, blockCols,
								blockRows, borderCols, borderRows, iters_per_copy,
								FilesavingTemp, MatrixCopy, kernel_hotspot, main_command_queue);

	clEnqueueReadBuffer(main_command_queue, MatrixTemp[ret], CL_FALSE, 0, sizeof(float) * size, (void *)FilesavingTemp[ret], 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);

	host_compute(MatrixCopy, FilesavingTemp[ret], grid_rows, grid_cols);

	clFinish(main_command_queue);
	exec_clock = omp_get_wtime() - exec_clock;

	/* CALCULO RESULTADOS */

	calc_norm(MatrixCopy, grid_rows, grid_cols);

	/* RELEASE ZONE */

	clReleaseMemObject(MatrixTemp[0]);
	clReleaseMemObject(MatrixTemp[1]);
	clReleaseMemObject(MatrixPower);

	clFinish(main_command_queue);

	clReleaseKernel(kernel_hotspot);
	clReleaseProgram(program);

	clReleaseCommandQueue(main_command_queue);
	clReleaseContext(context);

	free(MatrixCopy);
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	run(argc, argv);

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	return EXIT_SUCCESS;
}
