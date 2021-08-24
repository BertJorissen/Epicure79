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
/*
 * <license>
 * 
 * Controller v2.1
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#define FPGA_EMULATION 1
#define FPGA_PROFILING 2

#define AOCL_ALIGNMENT 64

#include <CL/cl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include "Hotspot_Constants.h"

#define ERR_NOT_FOUND -1
#define ERR_READ -2

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#define SSIZE 8

#define HOTSPOT_KERNEL_NAME "Hotspot"

#define STREAM_CPY_0 0
#define STREAM_CPY_1 1
#define STREAM_KERNEL 2

/* define timer macros */
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
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			dst[i * cols + j] = src[i * cols + j];
		}
	}
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
					  float *MatrixCopy, cl_kernel kernel_hotspot, cl_command_queue queues[3],
					  cl_event MatrixTemp_event[2], cl_event FilesavingTemp_event[2], cl_context context) {
	cl_int err;

	cl_event wait_event[2];
	cl_event aux_event;
	cl_event host_event;

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
	int aux_index;

    int BLOCK_X = LOCAL_SIZE_0;
    // Exit condition should be a multiple of comp_bsize
    int comp_bsize = BLOCK_X - 2;
    int last_col   = (col % comp_bsize == 0) ? col + 0 : col + comp_bsize - col % comp_bsize;
    int col_blocks = last_col / comp_bsize;
    int comp_exit  = BLOCK_X * col_blocks * (row + 1) / SSIZE;

    float step_div_Cap = step/Cap;
    float Rx_1=1/Rx;
    float Ry_1=1/Ry;
    float Rz_1=1/Rz;

	err = clSetKernelArg(kernel_hotspot, 0, sizeof(cl_mem), &MatrixPower);
	err |= clSetKernelArg(kernel_hotspot, 3, sizeof(cl_int), &col);
	err |= clSetKernelArg(kernel_hotspot, 4, sizeof(cl_int), &row);
	err |= clSetKernelArg(kernel_hotspot, 5, sizeof(cl_float), &step_div_Cap);
	err |= clSetKernelArg(kernel_hotspot, 6, sizeof(cl_float), &Rx_1);
	err |= clSetKernelArg(kernel_hotspot, 7, sizeof(cl_float), &Ry_1);
	err |= clSetKernelArg(kernel_hotspot, 8, sizeof(cl_float), &Rz_1);
	err |= clSetKernelArg(kernel_hotspot, 9, sizeof(cl_int), &comp_exit);
	
		
	cl_mem MatrixTemp_aux;
	float *FilesavingTemp_aux;
	cl_command_queue queue_aux;

	
	for (int t = 0; t < total_iterations; t += 1) {
		int temp = src;
		src = dst;
		dst = temp;

		err |= clSetKernelArg(kernel_hotspot, 1, sizeof(cl_mem), &MatrixTemp[src]);
		err |= clSetKernelArg(kernel_hotspot, 2, sizeof(cl_mem), &MatrixTemp[dst]);
		
		aux_event = MatrixTemp_event[dst];
		clEnqueueTask(queues[STREAM_KERNEL], kernel_hotspot, 1, &aux_event, &MatrixTemp_event[dst]);
		clFlush(queues[STREAM_KERNEL]);
		clReleaseEvent(aux_event);

		if ((real_iter % iters_per_copy) == 0) {
			
			wait_event[0] = MatrixTemp_event[dst];
			wait_event[1] = FilesavingTemp_event[dst];
			
			clEnqueueReadBuffer(queues[dst], MatrixTemp[dst], CL_FALSE, 0, sizeof(float) * row * col, (void *)FilesavingTemp[dst], 2, wait_event, &FilesavingTemp_event[dst]);
			clFlush(queues[dst]);

			MatrixTemp_event[dst] = FilesavingTemp_event[dst];
			clRetainEvent(MatrixTemp_event[dst]);
			
			clReleaseEvent(wait_event[0]);
			clReleaseEvent(wait_event[1]);

			aux_event = FilesavingTemp_event[dst];
			FilesavingTemp_event[dst] = clCreateUserEvent(context, &err);
			host_event = FilesavingTemp_event[dst];

			aux_index = dst;

			#pragma omp task depend(inout: MatrixPower) firstprivate(aux_event, host_event, aux_index)
			{
				clWaitForEvents(1, &aux_event);
				clReleaseEvent(aux_event);
				
				host_compute(MatrixCopy, FilesavingTemp[aux_index], row, col);

				clSetUserEventStatus(host_event, CL_COMPLETE);
			}
			
		}
		real_iter++;

		MatrixTemp_aux = MatrixTemp[0];
		MatrixTemp[0] = MatrixTemp[1];
		MatrixTemp[1] = MatrixTemp_aux;

		FilesavingTemp_aux = FilesavingTemp[0];
		FilesavingTemp[0] = FilesavingTemp[1];
		FilesavingTemp[1] = FilesavingTemp_aux;

		queue_aux = queues[0];
		queues[0] = queues[1];
		queues[1] = queue_aux;
	}

	return 1;
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

void run (int argc, char *argv[]) {

	/* ARGUMENTS */

	if (argc != 8) {
		usage(argc, argv);
	}
	int grid_rows = atoi(argv[1]);
	int grid_cols = atoi(argv[1]);
	int pyramid_height = atoi(argv[2]);
	int total_iterations = atoi(argv[3]);
	int iters_per_copy = atoi(argv[4]);
	int DEVICE = atoi(argv[5]);
	int PLATFORM = atoi(argv[6]);
	int EXEC_MODE = atoi(argv[7]);

	/* --------------- pyramid parameters --------------- */

	int borderCols = (pyramid_height)*EXPAND_RATE / 2;
	int borderRows = (pyramid_height)*EXPAND_RATE / 2;
	int smallBlockCol = LOCAL_SIZE_0 - (pyramid_height)*EXPAND_RATE;
	int smallBlockRow = LOCAL_SIZE_1 - (pyramid_height)*EXPAND_RATE;
	int blockCols = grid_cols / smallBlockCol + ((grid_cols % smallBlockCol == 0) ? 0 : 1);
	int blockRows = grid_rows / smallBlockRow + ((grid_rows % smallBlockRow == 0) ? 0 : 1);

	int size = grid_rows * grid_cols;

	cl_mem MatrixTemp[2];
	cl_mem MatrixPower;

	float *FilesavingTemp[2];
	float *FilesavingPower;
	float *MatrixCopy;

	/* VARIABLES */

	cl_int err;

	cl_platform_id platform_id;
	cl_device_id device_id;

	cl_context context;
	cl_program program;
	cl_kernel kernel_hotspot;

	cl_command_queue_properties properties;
	
	cl_command_queue queues[3];

	cl_event base_event;
	cl_event aux_event;
	cl_event host_event;

	cl_event wait_event[2];

	cl_event MatrixTemp_event[2];
	cl_event FilesavingTemp_event[2];

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

	cl_context_properties context_properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0 };
	context = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
	
	
    FILE *binary_file;
	char *kernel_path = (char *)malloc(500 * sizeof(char));
	kernel_path[0] = '\0';
	strcat(kernel_path, STRINGIFY(REF_KERNEL_PATH));
	strcat(kernel_path, HOTSPOT_KERNEL_NAME);
	strcat(kernel_path, "/");
	strcat(kernel_path, HOTSPOT_KERNEL_NAME);
	if(EXEC_MODE == FPGA_PROFILING)
		strcat(kernel_path, "_profiling");
	else if(EXEC_MODE == FPGA_EMULATION)
		strcat(kernel_path, "_emu");
	#ifdef _INTEL_KERNELS
		strcat(kernel_path, "_Ref.aocx");
	#elif _XILINX_KERNELS
		strcat(kernel_path, "_Ref.xclbin");
	#endif
	if(!(binary_file = fopen(kernel_path, "rb"))) {
		printf("Kernel file not found.\n");
		exit(ERR_NOT_FOUND);
	}
    fseek(binary_file, 0, SEEK_END);
    size_t binary_length = ftell(binary_file);
    unsigned char *binary_str = (unsigned char*)malloc(binary_length * sizeof(unsigned char));
    rewind(binary_file);
    if(!(fread(binary_str, binary_length, 1, binary_file))) {
		printf("Error reading kernel file\n");
		exit(ERR_READ);
	}

    program = clCreateProgramWithBinary(context, 1, &device_id, (const size_t *)&binary_length, (const unsigned char**)&binary_str, NULL, &err);
    
	
	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel_hotspot = clCreateKernel(program, HOTSPOT_KERNEL_NAME, &err);
	

	properties = 0;
	
	queues[STREAM_KERNEL] = clCreateCommandQueue(context, device_id, properties, &err);
	
	queues[STREAM_CPY_0] = clCreateCommandQueue(context, device_id, properties, &err);
	
	queues[STREAM_CPY_1] = clCreateCommandQueue(context, device_id, properties, &err);
	

	base_event = clCreateUserEvent(context, &err);
	

	clSetUserEventStatus(base_event, CL_COMPLETE);

	MatrixTemp_event[0] = base_event;
	MatrixTemp_event[1] = base_event;
	FilesavingTemp_event[0] = base_event;
	FilesavingTemp_event[1] = base_event;

	clRetainEvent(MatrixTemp_event[0]);
	clRetainEvent(MatrixTemp_event[1]);
	clRetainEvent(FilesavingTemp_event[0]);
	clRetainEvent(FilesavingTemp_event[1]);

	posix_memalign((void**)&FilesavingTemp[0], AOCL_ALIGNMENT, size * sizeof(float));
	
	posix_memalign((void**)&FilesavingTemp[1], AOCL_ALIGNMENT, size * sizeof(float));

	posix_memalign((void**)&FilesavingPower, AOCL_ALIGNMENT, size * sizeof(float));

	MatrixCopy = (float *)malloc(size * sizeof(float));

	clFinish(queues[0]);

	if (!FilesavingPower || !FilesavingTemp[0] || !FilesavingTemp[1] || !MatrixCopy) {
		fprintf(stderr, "unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	size_t platform_name_size;
	clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size);
	char* platform_name = (char*)malloc( sizeof(char) * platform_name_size);
	clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL );
	
	size_t device_name_size;
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size);
	char* device_name = (char*) malloc( sizeof(char) * device_name_size );
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL);

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
		printf("\n EXEC_MODE: %d", EXEC_MODE);
		printf("\n POLICY ASYNC");
		printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);
	free(platform_name);
	free(device_name);

	init_matrix(FilesavingTemp[0], FilesavingPower, grid_rows, grid_cols);

	MatrixTemp[0] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	
	MatrixTemp[1] = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);
	
	MatrixPower = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(float), NULL, &err);

	omp_set_num_threads(2);
	#pragma omp parallel 
	{
		#pragma omp master
		{
			clFlush(queues[0]);
			clFlush(queues[1]);
			clFlush(queues[2]);
			clFinish(queues[0]);
			clFinish(queues[1]);
			clFinish(queues[2]);

			exec_clock = omp_get_wtime();
			clEnqueueWriteBuffer(queues[STREAM_KERNEL], MatrixTemp[0], CL_FALSE, 0, size * sizeof(float), (void *)FilesavingTemp[0], 0, NULL, NULL);
			clFlush(queues[STREAM_KERNEL]);
			clEnqueueWriteBuffer(queues[STREAM_KERNEL], MatrixPower, CL_FALSE, 0, size * sizeof(float), (void *)FilesavingPower, 0, NULL, NULL);
			clFlush(queues[STREAM_KERNEL]);
			

			int ret = compute_tran_temp(MatrixPower, MatrixTemp, grid_cols, grid_rows,
										total_iterations, pyramid_height, blockCols,
										blockRows, borderCols, borderRows, iters_per_copy,
										FilesavingTemp, MatrixCopy, kernel_hotspot, queues, 
										MatrixTemp_event, FilesavingTemp_event, context);

			wait_event[0] = MatrixTemp_event[ret];
			wait_event[1] = FilesavingTemp_event[ret];
			
			clEnqueueReadBuffer(queues[ret], MatrixTemp[ret], CL_TRUE, 0, size * sizeof(float), (void *)FilesavingTemp[ret], 2, wait_event, &FilesavingTemp_event[ret]);
			clFlush(queues[ret]);
			MatrixTemp_event[ret] = FilesavingTemp_event[ret];
			clRetainEvent(MatrixTemp_event[ret]);
			
			clReleaseEvent(wait_event[0]);
			clReleaseEvent(wait_event[1]);

			aux_event = FilesavingTemp_event[ret];
			FilesavingTemp_event[ret] = clCreateUserEvent(context, &err);
			host_event = FilesavingTemp_event[ret];

			#pragma omp task depend(inout: MatrixPower) firstprivate(aux_event, host_event, ret)
			{
				clWaitForEvents(1, &aux_event);
				clReleaseEvent(aux_event);
				
				host_compute(MatrixCopy, FilesavingTemp[ret], grid_rows, grid_cols);

				clSetUserEventStatus(host_event, CL_COMPLETE);
			}

			clFlush(queues[0]);
			clFlush(queues[1]);
			clFlush(queues[2]);
			clFinish(queues[0]);
			clFinish(queues[1]);
			clFinish(queues[2]);
			clWaitForEvents(1, &FilesavingTemp_event[ret]);
			exec_clock = omp_get_wtime() - exec_clock ;
		}
	}


	/* CALCULO RESULTADOS */

	calc_norm(MatrixCopy, grid_rows, grid_cols);
	
	/* RELEASE ZONE */

	clReleaseEvent(base_event);
	clReleaseEvent(MatrixTemp_event[0]);
	clReleaseEvent(MatrixTemp_event[1]);
	clReleaseEvent(FilesavingTemp_event[0]);
	clReleaseEvent(FilesavingTemp_event[1]);
	
	clReleaseMemObject(MatrixTemp[0]);
	clReleaseMemObject(MatrixTemp[1]);
	clReleaseMemObject(MatrixPower);
	
	clFinish(queues[0]);

	clReleaseKernel(kernel_hotspot);
	clReleaseProgram(program);
	
	clReleaseCommandQueue(queues[0]);
	clReleaseCommandQueue(queues[1]);
	clReleaseCommandQueue(queues[2]);
	clReleaseContext(context);

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



