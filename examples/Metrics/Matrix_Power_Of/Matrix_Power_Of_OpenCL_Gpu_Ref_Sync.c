#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEED    6834723
#define EPSILON 0.0001

double main_clock;
double exec_clock;

cl_int DEVICE = 0;

#define CELL_AUTOM_KERNEL_NAME_MULT "Mult"

#define CELL_AUTOM_KERNEL_MULT \
	" __kernel void Mult(const int SIZE, __global float *d_result, __global const float *d_matrix_a, __global const float *d_matrix_b ) { \
		__local float tile_a[LOCAL_SIZE][LOCAL_SIZE]; \
		__local float tile_b[LOCAL_SIZE][LOCAL_SIZE]; \
		int row = get_global_id(1); \
		int col = get_global_id(0); \
		\
		int n_groups_x = get_num_groups(0); \
		\
		int local_size = get_local_size(0); \
		\
		int local_x = get_local_id(0); \
		int local_y = get_local_id(1); \
		\
		float tmp = 0.0; \
		int idx; \
		\
		for (int sub = 0; sub < n_groups_x; ++sub) { \
			idx = row * SIZE + sub * local_size + local_x; \
			if (idx >= SIZE * SIZE) { \
				tile_a[local_y][local_x] = 0; \
			} else { \
				tile_a[local_y][local_x] = d_matrix_a[idx]; \
			} \
			idx = (sub * local_size + local_y) * SIZE + col; \
			if (idx >= SIZE * SIZE) { \
				tile_b[local_y][local_x] = 0; \
			} else { \
				tile_b[local_y][local_x] = d_matrix_b[idx]; \
			} \
			barrier(CLK_LOCAL_MEM_FENCE); \
			\
			for (int k = 0; k < local_size; ++k) { \
				tmp += tile_a[local_y][k] * tile_b[k][local_x]; \
			} \
			barrier(CLK_LOCAL_MEM_FENCE); \
		} \
		if (row < SIZE && col < SIZE) { \
			d_result[row * SIZE + col] = tmp; \
		} \
	} "

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

void Init_Tiles(cl_float *matrix_a, cl_float *matrix_b, cl_float *matrix_c, int rows, int columns) {
	srand(SEED);
	for (int j = 0; j < columns; j++) {
		float col_sum_a = 0;
		for (int i = 0; i < rows; i++) {
			float min    = -(1 - col_sum_a) + EPSILON;
			float max    = 1 - col_sum_a - EPSILON;
			float random = ((float)rand()) / (float)RAND_MAX;
			float range  = max - min;
			float value  = (random * range) + min;

			matrix_a[i * columns + j] = value;
			matrix_b[i * columns + j] = value;
			matrix_c[i * columns + j] = 0;
			col_sum_a += fabsf(value);
		}
	}
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	/* ARGUMENTS */

	if (argc < 4) {
		fprintf(stderr, "Usage: %s <matrix_size> <n_power> <device> <platform> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	cl_int SIZE     = atoi(argv[1]);
	cl_int N_ITER   = atoi(argv[2]);
	cl_int PLATFORM = atoi(argv[3]);

	size_t MATRIX_SIZE = sizeof(cl_float) * SIZE * SIZE;

	/* VARIABLES */

	cl_int err;

	char *kernel_raw_mult = CELL_AUTOM_KERNEL_MULT;

	char *kernel_raw = (char *)malloc((strlen(kernel_raw_mult) + 300) * sizeof(char));
	sprintf(&kernel_raw[0], " #define LOCAL_SIZE %d \n", LOCAL_SIZE);
	strcat(kernel_raw, kernel_raw_mult);

	size_t kernel_size = strlen(kernel_raw);

	size_t local_size[2];
	size_t global_size[2];

	cl_context context;
	cl_program program;
	cl_kernel  kernel_mult;

	cl_command_queue_properties properties;

	cl_command_queue main_command_queue;

	cl_mem mem_matrix_a;
	cl_mem mem_pinned_matrix_a;

	cl_mem mem_matrix_b;
	cl_mem mem_pinned_matrix_b;

	cl_mem mem_matrix_c;
	cl_mem mem_pinned_matrix_c;

	cl_float *p_pinned_matrix_a;
	cl_float *p_pinned_matrix_b;
	cl_float *p_pinned_matrix_c;

	double *p_res        = (double *)malloc(sizeof(double) * N_ITER);
	double *p_sum        = (double *)malloc(sizeof(double) * N_ITER);
	float  *p_matrix_tmp = (float *)malloc(MATRIX_SIZE);

	/* PLATFORMS & DEVICES */

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL);
	cl_platform_id platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL);
	cl_device_id device_id = p_devices[DEVICE];
	free(p_devices);

	/* SET UP, CONTEXT, QUEUES, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);

	properties         = 0;
	main_command_queue = clCreateCommandQueue(context, device_id, properties, &err);

	program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw), (const size_t *)(&kernel_size), &err);

	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel_mult = clCreateKernel(program, CELL_AUTOM_KERNEL_NAME_MULT, &err);

	local_size[0] = LOCAL_SIZE;
	local_size[1] = LOCAL_SIZE;

	global_size[0] = global_size[1] = SIZE;

	if ((SIZE % local_size[0]) != 0) {
		global_size[0] += (local_size[0] - (SIZE % local_size[0]));
	}

	if ((SIZE % local_size[1]) != 0) {
		global_size[1] += (local_size[1] - (SIZE % local_size[1]));
	}

	mem_pinned_matrix_a = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
	p_pinned_matrix_a   = (float *)clEnqueueMapBuffer(main_command_queue, mem_pinned_matrix_a, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
	mem_matrix_a        = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);

	mem_pinned_matrix_b = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
	p_pinned_matrix_b   = (float *)clEnqueueMapBuffer(main_command_queue, mem_pinned_matrix_b, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
	mem_matrix_b        = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);

	mem_pinned_matrix_c = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
	p_pinned_matrix_c   = (float *)clEnqueueMapBuffer(main_command_queue, mem_pinned_matrix_c, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
	mem_matrix_c        = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);

	clFinish(main_command_queue);

	Init_Tiles(p_pinned_matrix_a, p_pinned_matrix_b, p_pinned_matrix_c, SIZE, SIZE);

	cl_event aux;
	clFlush(main_command_queue);
	clFinish(main_command_queue);
	exec_clock = omp_get_wtime();

	clEnqueueWriteBuffer(main_command_queue, mem_matrix_a, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_matrix_a, 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);
	clEnqueueWriteBuffer(main_command_queue, mem_matrix_b, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_matrix_b, 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);
	clEnqueueWriteBuffer(main_command_queue, mem_matrix_c, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_matrix_c, 0, NULL, &aux);
	clFlush(main_command_queue);
	clWaitForEvents(1, &aux);

	for (int i = 0; i < N_ITER; i++) {
		cl_float *matrix1;
		cl_mem    dmatrix1;
		cl_mem    dmatrix2;
		if ((i % 2) == 0) {
			matrix1  = p_pinned_matrix_c;
			dmatrix1 = mem_matrix_c;
			dmatrix2 = mem_matrix_b;
		} else {
			matrix1  = p_pinned_matrix_b;
			dmatrix1 = mem_matrix_b;
			dmatrix2 = mem_matrix_c;
		}
		clSetKernelArg(kernel_mult, 0, sizeof(cl_int), &SIZE);
		clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), &dmatrix1);
		clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), &mem_matrix_a);
		clSetKernelArg(kernel_mult, 3, sizeof(cl_mem), &dmatrix2);
		clEnqueueNDRangeKernel(main_command_queue, kernel_mult, 2, NULL, global_size, local_size, 0, NULL, NULL);
		clFlush(main_command_queue);

		clEnqueueReadBuffer(main_command_queue, dmatrix1, CL_FALSE, 0, MATRIX_SIZE, (void *)matrix1, 0, NULL, &aux);
		clFlush(main_command_queue);
		clWaitForEvents(1, &aux);

		computeNorm(matrix1, p_matrix_tmp, SIZE, p_sum, p_res, i);
	}

	clFlush(main_command_queue);
	clFinish(main_command_queue);
	exec_clock = omp_get_wtime() - exec_clock;

	/* RELEASE ZONE */

	clEnqueueUnmapMemObject(main_command_queue, mem_pinned_matrix_a, p_pinned_matrix_a, 0, NULL, NULL);
	clEnqueueUnmapMemObject(main_command_queue, mem_pinned_matrix_b, p_pinned_matrix_b, 0, NULL, NULL);
	clEnqueueUnmapMemObject(main_command_queue, mem_pinned_matrix_c, p_pinned_matrix_c, 0, NULL, NULL);

	clFinish(main_command_queue);

	clReleaseMemObject(mem_pinned_matrix_a);
	clReleaseMemObject(mem_pinned_matrix_b);
	clReleaseMemObject(mem_pinned_matrix_c);
	clReleaseMemObject(mem_matrix_a);
	clReleaseMemObject(mem_matrix_b);
	clReleaseMemObject(mem_matrix_c);

	clReleaseKernel(kernel_mult);
	clReleaseProgram(program);

	clReleaseCommandQueue(main_command_queue);
	clReleaseContext(context);

	free(p_res);
	free(p_sum);
	free(p_matrix_tmp);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
