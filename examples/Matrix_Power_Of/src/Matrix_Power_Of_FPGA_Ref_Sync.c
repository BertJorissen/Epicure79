/**
 * @file Matrix_Power_Of_FPGA_Ref_Sync.c
 * @author Trasgo Group
 * @brief MatrixPow: Synchronous native CUDA version
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

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#define FPGA_EMULATION 1
#define FPGA_PROFILING 2

#define AOCL_ALIGNMENT 64

#include <CL/cl.h>
#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_NOT_FOUND -1
#define ERR_READ      -2

#define _STRINGIFY(x) #x
#define STRINGIFY(x)  _STRINGIFY(x)

#define SEED    6834723
#define EPSILON 0.0001

double main_clock;
double exec_clock;

#define CELL_AUTOM_KERNEL_NAME_MULT "Mult"

void computeNorm(float *p_matrix, float *p_matrix_res, int SIZE, double *p_sum, double *p_res, int i) {
	double minimo = p_matrix[0];
	double maximo = p_matrix[0];

	for (int j = 0; j < SIZE; j++) {
		for (int k = 0; k < SIZE; k++) {
			if (minimo > p_matrix[j * SIZE + k]) {
				minimo = p_matrix[j * SIZE + k];
			}
			if (maximo < p_matrix[j * SIZE + k]) {
				maximo = p_matrix[j * SIZE + k];
			}
		}
	}

	for (int j = 0; j < SIZE; j++) {
		for (int k = 0; k < SIZE; k++) {
			p_matrix[j * SIZE + k] = p_matrix[j * SIZE + k] - minimo;
			p_matrix[j * SIZE + k] = p_matrix[j * SIZE + k] / maximo;
		}
	}

	p_sum[i] = 0;
	for (int j = 0; j < SIZE; j++) {
		for (int k = 0; k < SIZE; k++) {
			p_sum[i] += pow(p_matrix[j * SIZE + k], 2);
		}
	}
	p_res[i] = sqrt(p_sum[i]);

	for (int j = 0; j < SIZE; j++) {
		for (int k = 0; k < SIZE; k++) {
			p_matrix_res[j * SIZE + k] = p_matrix[j * SIZE + k] / p_res[i];
		}
	}
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	/* ARGUMENTS */

	if (argc < 6) {
		fprintf(stderr, "Usage: %s <size> <numIter> <device> <platform> <exec_mode>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	cl_int SIZE      = atoi(argv[1]);
	cl_int N_ITER    = atoi(argv[2]);
	cl_int DEVICE    = atoi(argv[3]);
	cl_int PLATFORM  = atoi(argv[4]);
	cl_int EXEC_MODE = atoi(argv[5]);

	size_t MATRIX_SIZE = sizeof(cl_float) * SIZE * SIZE;

	/* VARIABLES */

	cl_int err;

	size_t local_size[2];
	size_t global_size[2];

	cl_platform_id platform_id;
	cl_device_id   device_id;

	cl_context context;
	cl_program program;
	cl_kernel  kernel_mult;

	cl_command_queue_properties properties;

	cl_command_queue main_command_queue;

	cl_mem mem_matrix[3];

	cl_float *p_pinned_matrix[3];

	double *p_res        = (double *)malloc(sizeof(double) * N_ITER);
	double *p_sum        = (double *)malloc(sizeof(double) * N_ITER);
	float  *p_matrix_tmp = (float *)malloc(MATRIX_SIZE);

	/* PLATFORMS & DEVICES */

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL);
	platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, DEVICE + 1, p_devices, NULL);
	device_id = p_devices[DEVICE];
	free(p_devices);

	size_t platform_name_size;
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size);
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL);

	size_t device_name_size;
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size);
	char *device_name = (char *)malloc(sizeof(char) * device_name_size);
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, %s, ", device_name, platform_name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n PLATFORM: %s", platform_name);
	printf("\n DEVICE: %s", device_name);
	printf("\n EXEC_MODE: %d", EXEC_MODE);
	printf("\n POLICY SYNC");
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	free(platform_name);
	free(device_name);

	/* SET UP, CONTEXTO, COLAS, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);

	properties         = 0;
	main_command_queue = clCreateCommandQueue(context, device_id, properties, &err);

	FILE *binary_file;
	char *kernel_path = (char *)malloc(500 * sizeof(char));
	kernel_path[0]    = '\0';
	strcat(kernel_path, STRINGIFY(REF_KERNEL_PATH));
	strcat(kernel_path, CELL_AUTOM_KERNEL_NAME_MULT);
	strcat(kernel_path, "/");
	strcat(kernel_path, CELL_AUTOM_KERNEL_NAME_MULT);
	if (EXEC_MODE == FPGA_EMULATION)
		strcat(kernel_path, "_emu");
	else if (EXEC_MODE == FPGA_PROFILING)
		strcat(kernel_path, "_profiling");
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

	kernel_mult = clCreateKernel(program, CELL_AUTOM_KERNEL_NAME_MULT, &err);

	local_size[0] = LOCAL_SIZE_0;
	local_size[1] = LOCAL_SIZE_1;

	global_size[0] = global_size[1] = SIZE;

	if ((SIZE % local_size[0]) != 0) {
		global_size[0] += (local_size[0] - (SIZE % local_size[0]));
	}

	if ((SIZE % local_size[1]) != 0) {
		global_size[1] += (local_size[1] - (SIZE % local_size[1]));
	}

	for (int i = 0; i < 3; i++) {
		posix_memalign((void **)&p_pinned_matrix[i], AOCL_ALIGNMENT, MATRIX_SIZE);
		mem_matrix[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
	}

	clFinish(main_command_queue);

	srand(SEED);
	for (int j = 0; j < SIZE; j++) {
		float col_sum_a = 0;
		for (int i = 0; i < SIZE; i++) {
			// generate random floats in a way matrixes don't turn into NaN
			float min    = -(1 - col_sum_a) + EPSILON;
			float max    = 1 - col_sum_a - EPSILON;
			float random = ((float)rand()) / (float)RAND_MAX;
			float range  = max - min;
			float value  = (random * range) + min;

			p_pinned_matrix[0][i * SIZE + j] = value;
			p_pinned_matrix[1][i * SIZE + j] = value;
			p_pinned_matrix[2][i * SIZE + j] = 0;
			col_sum_a += fabsf(value);
		}
	}

	cl_event aux;
	clFlush(main_command_queue);
	clFinish(main_command_queue);
	exec_clock = omp_get_wtime();

	for (int i = 0; i < 3; i++) {
		clEnqueueWriteBuffer(main_command_queue, mem_matrix[i], CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_matrix[i], 0, NULL, &aux);
		clFlush(main_command_queue);
		clWaitForEvents(1, &aux);
	}

	int src, dst, aux_idx;
	src = 1;
	dst = 2;
	for (int i = 0; i < N_ITER; i++) {
		err |= clSetKernelArg(kernel_mult, 0, sizeof(cl_mem), &mem_matrix[dst]);
		err |= clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), &mem_matrix[0]);
		err |= clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), &mem_matrix[src]);
		err |= clSetKernelArg(kernel_mult, 3, sizeof(cl_int), &SIZE);
		err |= clSetKernelArg(kernel_mult, 4, sizeof(cl_int), &SIZE);

		clEnqueueNDRangeKernel(main_command_queue, kernel_mult, 2, NULL, global_size, local_size, 0, NULL, NULL);
		clFlush(main_command_queue);

		clEnqueueReadBuffer(main_command_queue, mem_matrix[dst], CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_matrix[dst], 0, NULL, &aux);
		clFlush(main_command_queue);
		clWaitForEvents(1, &aux);

		computeNorm(p_pinned_matrix[dst], p_matrix_tmp, SIZE, p_sum, p_res, i);

		aux_idx = src;
		src     = dst;
		dst     = aux_idx;
	}
	clFlush(main_command_queue);
	clFinish(main_command_queue);
	exec_clock = omp_get_wtime() - exec_clock;

	/* PRINT RESULTS */
	#ifdef _CTRL_EXAMPLES_TEST_MODE_
	for (int i = 0; i < N_ITER; i++) {
		printf("%lf, %lf, ", p_sum[i], p_res[i]);
	}
	fflush(stdout);
	#elif _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", p_sum[N_ITER - 1], p_res[N_ITER - 1]);
	fflush(stdout);
	#else
	printf("\n ----------------------- NORM ----------------------- \n\n");
	fflush(stdout);
	for (int i = 0; i < N_ITER; i++) {
		printf(" iter: %d, sum: %lf, res: %lf\n", i + 1, p_sum[i], p_res[i]);
		fflush(stdout);
	}
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif

	/* RELEASE ZONE */

	clFinish(main_command_queue);

	for (int i = 0; i < 3; i++) {
		clReleaseMemObject(mem_matrix[i]);
	}

	clReleaseKernel(kernel_mult);
	clReleaseProgram(program);

	clReleaseCommandQueue(main_command_queue);
	clReleaseContext(context);

	free(p_res);
	free(p_sum);
	free(p_matrix_tmp);

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
