/**
 * @file Matrix_Power_Of_OpenCL_Gpu_Ref_Async.c
 * @brief MatrixPow: Asynchronous native OpenCLGPU version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _PROFILING_ENABLED_
#include <roctracer/roctx.h>
#endif //_PROFILING_ENABLED_

#define SEED 6834723

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK_
#include <assert.h>

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_DEBUG_
#define OPENCL_ASSERT_OP(operation) \
	err = operation;                \
	printf("error: %d\n", err);     \
	assert(err == CL_SUCCESS);

#define OPENCL_ASSERT_ERROR(err) \
	printf("error: %d\n", err);  \
	assert(err == CL_SUCCESS)

#else
#define OPENCL_ASSERT_OP(operation) \
	assert(operation == CL_SUCCESS);

#define OPENCL_ASSERT_ERROR(err) \
	assert(err == CL_SUCCESS)
#endif
#else
#define OPENCL_ASSERT_OP(operation) operation
#define OPENCL_ASSERT_ERROR(err)
#endif

#define SWAP(x, y, T)  \
	do {               \
		T SWAP = x;    \
		x      = y;    \
		y      = SWAP; \
	} while (0)

double main_clock;
double exec_clock;

#define MATMULT_KERNEL_NAME "Mult"

#define MATMULT_KERNEL \
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
			tile_a[local_y][local_x] = d_matrix_a[idx]; \
			idx = (sub * local_size + local_y) * SIZE + col; \
			tile_b[local_y][local_x] = d_matrix_b[idx]; \
			barrier(CLK_LOCAL_MEM_FENCE); \
			\
			for (int k = 0; k < local_size; ++k) { \
				tmp += tile_a[local_y][k] * tile_b[k][local_x]; \
			} \
			barrier(CLK_LOCAL_MEM_FENCE); \
		} \
		d_result[row * SIZE + col] = tmp; \
	} "

void norm_calc(float *p_matrix, int size, double *p_sum, double *p_res, int ITER) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("norm calc");
	#endif //_PROFILING_ENABLED_

	double suma = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			suma += p_matrix[i * size + j] * p_matrix[i * size + j];
		}
	}
	p_sum[ITER] = suma;
	p_res[ITER] = sqrt(suma);

	ITER++;

	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

void init_matrix_rand(float *p_matrix, int size) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Init tile rand");
	#endif //_PROFILING_ENABLED_
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			p_matrix[i * size + j] = -1 + 2 * (float)rand() / (float)RAND_MAX;
		}
	}

	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

void init_matrix_null(float *p_matrix, int size) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Init tile null");
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			p_matrix[i * size + j] = 0;
		}
	}
	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

void init_matrix_diag(float *p_matrix, int size) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Init tile diag");
	#endif //_PROFILING_ENABLED_
	for (int i = 0; i < size; i++) {
		p_matrix[i * size + i] = -1 + 2 * (float)rand() / (float)RAND_MAX;
	}
	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	/* ARGUMENTS */

	if (argc < 5) {
		fprintf(stderr, "Usage: %s <matrix_size> <n_iters> <device> <platform>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	cl_int SIZE     = atoi(argv[1]);
	cl_int N_ITER   = atoi(argv[2]);
	cl_int DEVICE   = atoi(argv[3]);
	cl_int PLATFORM = atoi(argv[4]);

	size_t MATRIX_SIZE = sizeof(cl_float) * SIZE * SIZE;

	/* VARIABLES */

	cl_int err;

	char *kernel_raw_mult = MATMULT_KERNEL;

	char *kernel_raw = (char *)malloc((strlen(kernel_raw_mult) + 300) * sizeof(char));
	sprintf(&kernel_raw[0], " #define LOCAL_SIZE %d \n", BLOCKSIZE);
	strcat(kernel_raw, kernel_raw_mult);

	size_t kernel_size = strlen(kernel_raw);

	size_t local_size[2];
	size_t global_size[2];

	cl_context context;
	cl_program program;
	cl_kernel  kernel_mult;

	cl_command_queue_properties properties;

	cl_command_queue kernel_command_queue;
	cl_command_queue htd_command_queue;
	cl_command_queue dth_command_queue;

	cl_mem    d_m1, d_m2, d_m3, d_m4, d_m5;
	cl_mem    h_in_pinned, h_out_pinned;
	cl_float *p_h_in, *p_h_out;
	#ifdef _2BUF
	cl_mem    d_m1_aux, d_m2_aux, d_m3_aux, d_m4_aux, d_m5_aux;
	cl_mem    h_in_pinned_aux, h_out_pinned_aux;
	cl_float *p_h_in_aux, *p_h_out_aux;
	#endif // _2BUF

	cl_mem    d_const1, d_const2, d_const3, d_const4;
	cl_float *p_h_const1, *p_h_const2, *p_h_const3, *p_h_const4;

	double *p_res = (double *)malloc(sizeof(double) * N_ITER);
	double *p_sum = (double *)malloc(sizeof(double) * N_ITER);

	cl_event event_base;
	cl_event event_init;
	cl_event event_htd;
	cl_event event_kernel;
	cl_event event_dth;
	cl_event event_norm;
	#ifdef _2BUF
	cl_event event_init_aux;
	cl_event event_htd_aux;
	cl_event event_kernel_aux;
	cl_event event_dth_aux;
	cl_event event_norm_aux;
	#endif // _2BUF

	cl_event event_wait_list[2];

	/* PLATFORMS & DEVICES */

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL));
	cl_platform_id platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL));
	cl_device_id device_id = p_devices[DEVICE];
	free(p_devices);

	// Extra information for collecting results
	size_t platform_name_size;
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size));
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL));

	size_t device_name_size;
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size));
	char *device_name = (char *)malloc(sizeof(char) * device_name_size);
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL));

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("OpenCL-GPU-%s-%s, ", device_name, platform_name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %d", SIZE);
	printf("\n N_ITER: %d", N_ITER);
	printf("\n PLATFORM: %s", platform_name);
	printf("\n DEVICE: %s", device_name);
	printf("\n POLICY ASYNC");
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);
	free(platform_name);
	free(device_name);

	/* SET UP, CONTEXT, QUEUES, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	omp_set_num_threads(3);
	#pragma omp parallel
	{
		#pragma omp master
		{
			properties = 0;

			kernel_command_queue = clCreateCommandQueue(context, device_id, properties, &err);
			OPENCL_ASSERT_ERROR(err);
			htd_command_queue = clCreateCommandQueue(context, device_id, properties, &err);
			OPENCL_ASSERT_ERROR(err);
			dth_command_queue = clCreateCommandQueue(context, device_id, properties, &err);
			OPENCL_ASSERT_ERROR(err);

			h_in_pinned = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			p_h_in = (float *)clEnqueueMapBuffer(kernel_command_queue, h_in_pinned, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
			OPENCL_ASSERT_ERROR(err);

			h_out_pinned = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			p_h_out = (float *)clEnqueueMapBuffer(kernel_command_queue, h_out_pinned, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
			OPENCL_ASSERT_ERROR(err);

			p_h_const1 = (float *)malloc(MATRIX_SIZE);
			p_h_const2 = (float *)malloc(MATRIX_SIZE);
			p_h_const3 = (float *)malloc(MATRIX_SIZE);
			p_h_const4 = (float *)malloc(MATRIX_SIZE);

			d_m1 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m2 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m3 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m4 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m5 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_const1 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_const2 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_const3 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_const4 = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);

			#ifdef _2BUF
			h_in_pinned_aux = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			p_h_in_aux = (float *)clEnqueueMapBuffer(kernel_command_queue, h_in_pinned_aux, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
			OPENCL_ASSERT_ERROR(err);

			h_out_pinned_aux = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			p_h_out_aux = (float *)clEnqueueMapBuffer(kernel_command_queue, h_out_pinned_aux, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
			OPENCL_ASSERT_ERROR(err);

			d_m1_aux = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m2_aux = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m3_aux = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m4_aux = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			d_m5_aux = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
			OPENCL_ASSERT_ERROR(err);
			#endif // _2BUF

			OPENCL_ASSERT_OP(clFinish(kernel_command_queue));

			program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw), (const size_t *)(&kernel_size), &err);
			OPENCL_ASSERT_ERROR(err);

			OPENCL_ASSERT_OP(clBuildProgram(program, 1, &device_id, NULL, NULL, NULL));

			kernel_mult = clCreateKernel(program, MATMULT_KERNEL_NAME, &err);
			OPENCL_ASSERT_ERROR(err);

			local_size[0] = BLOCKSIZE;
			local_size[1] = BLOCKSIZE;

			global_size[0] = global_size[1] = SIZE;

			if ((SIZE % local_size[0]) != 0) {
				global_size[0] += (local_size[0] - (SIZE % local_size[0]));
			}

			if ((SIZE % local_size[1]) != 0) {
				global_size[1] += (local_size[1] - (SIZE % local_size[1]));
			}

			event_base = clCreateUserEvent(context, &err);
			OPENCL_ASSERT_ERROR(err);

			OPENCL_ASSERT_OP(clSetUserEventStatus(event_base, CL_COMPLETE));

			event_init   = event_base;
			event_htd    = event_base;
			event_kernel = event_base;
			event_dth    = event_base;
			event_norm   = event_base;

			OPENCL_ASSERT_OP(clRetainEvent(event_init));
			OPENCL_ASSERT_OP(clRetainEvent(event_htd));
			OPENCL_ASSERT_OP(clRetainEvent(event_kernel));
			OPENCL_ASSERT_OP(clRetainEvent(event_dth));
			OPENCL_ASSERT_OP(clRetainEvent(event_norm));

			#ifdef _2BUF
			event_init_aux   = event_base;
			event_htd_aux    = event_base;
			event_kernel_aux = event_base;
			event_dth_aux    = event_base;
			event_norm_aux   = event_base;

			OPENCL_ASSERT_OP(clRetainEvent(event_init_aux));
			OPENCL_ASSERT_OP(clRetainEvent(event_htd_aux));
			OPENCL_ASSERT_OP(clRetainEvent(event_kernel_aux));
			OPENCL_ASSERT_OP(clRetainEvent(event_dth_aux));
			OPENCL_ASSERT_OP(clRetainEvent(event_norm_aux));
			#endif // _2BUF

			srand(SEED);
			init_matrix_null(p_h_in, SIZE);
			#ifdef _2BUF
			init_matrix_null(p_h_in_aux, SIZE);
			#endif // _2BUF
			init_matrix_rand(p_h_const1, SIZE);
			init_matrix_rand(p_h_const2, SIZE);
			init_matrix_rand(p_h_const3, SIZE);
			init_matrix_rand(p_h_const4, SIZE);
			OPENCL_ASSERT_OP(clEnqueueWriteBuffer(htd_command_queue, d_const1, CL_FALSE, 0, MATRIX_SIZE, (void *)p_h_const1, 0, NULL, NULL));
			OPENCL_ASSERT_OP(clEnqueueWriteBuffer(htd_command_queue, d_const2, CL_FALSE, 0, MATRIX_SIZE, (void *)p_h_const2, 0, NULL, NULL));
			OPENCL_ASSERT_OP(clEnqueueWriteBuffer(htd_command_queue, d_const3, CL_FALSE, 0, MATRIX_SIZE, (void *)p_h_const3, 0, NULL, NULL));
			OPENCL_ASSERT_OP(clEnqueueWriteBuffer(htd_command_queue, d_const4, CL_FALSE, 0, MATRIX_SIZE, (void *)p_h_const4, 0, NULL, NULL));

			OPENCL_ASSERT_OP(clFlush(kernel_command_queue));
			OPENCL_ASSERT_OP(clFlush(htd_command_queue));
			OPENCL_ASSERT_OP(clFlush(dth_command_queue));
			OPENCL_ASSERT_OP(clFinish(kernel_command_queue));
			OPENCL_ASSERT_OP(clFinish(htd_command_queue));
			OPENCL_ASSERT_OP(clFinish(dth_command_queue));
			exec_clock = omp_get_wtime();

			for (int i = 0; i < N_ITER; i++) {
				// init
				OPENCL_ASSERT_OP(clReleaseEvent(event_init));
				event_init = clCreateUserEvent(context, &err);
				OPENCL_ASSERT_OP(clRetainEvent(event_htd));
				OPENCL_ASSERT_OP(clRetainEvent(event_init));
				#pragma omp task depend(inout: p_h_in) firstprivate(event_init, event_htd, i, p_h_in)
				{
					OPENCL_ASSERT_OP(clWaitForEvents(1, &event_htd));
					OPENCL_ASSERT_OP(clReleaseEvent(event_htd));
					init_matrix_diag(p_h_in, SIZE);
					OPENCL_ASSERT_OP(clSetUserEventStatus(event_init, CL_COMPLETE));
					OPENCL_ASSERT_OP(clReleaseEvent(event_init));
				}
				// htd
				event_wait_list[0] = event_init;
				event_wait_list[1] = event_kernel;
				OPENCL_ASSERT_OP(clReleaseEvent(event_htd));
				OPENCL_ASSERT_OP(clEnqueueWriteBuffer(htd_command_queue, d_m1, CL_FALSE, 0, MATRIX_SIZE, (void *)p_h_in, 2, event_wait_list, &event_htd));
				OPENCL_ASSERT_OP(clFlush(htd_command_queue));
				// kernels
				// TODO should probably pre-create 4 kernels with args and use them here
				OPENCL_ASSERT_OP(clReleaseEvent(event_kernel));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 0, sizeof(cl_int), &SIZE));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), &d_m2));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), &d_m1));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 3, sizeof(cl_mem), &d_const1));
				OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(kernel_command_queue, kernel_mult, 2, NULL, global_size, local_size, 1, &event_htd, NULL));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 0, sizeof(cl_int), &SIZE));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), &d_m3));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), &d_m2));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 3, sizeof(cl_mem), &d_const2));
				OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(kernel_command_queue, kernel_mult, 2, NULL, global_size, local_size, 0, NULL, NULL));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 0, sizeof(cl_int), &SIZE));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), &d_m4));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), &d_m3));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 3, sizeof(cl_mem), &d_const3));
				OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(kernel_command_queue, kernel_mult, 2, NULL, global_size, local_size, 0, NULL, NULL));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 0, sizeof(cl_int), &SIZE));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 1, sizeof(cl_mem), &d_m5));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 2, sizeof(cl_mem), &d_m4));
				OPENCL_ASSERT_OP(clSetKernelArg(kernel_mult, 3, sizeof(cl_mem), &d_const4));
				OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(kernel_command_queue, kernel_mult, 2, NULL, global_size, local_size, 1, &event_dth, &event_kernel));
				OPENCL_ASSERT_OP(clFlush(kernel_command_queue));
				// dth
				event_wait_list[0] = event_kernel;
				event_wait_list[1] = event_norm;
				OPENCL_ASSERT_OP(clReleaseEvent(event_dth));
				OPENCL_ASSERT_OP(clEnqueueReadBuffer(dth_command_queue, d_m5, CL_FALSE, 0, MATRIX_SIZE, (void *)p_h_out, 2, event_wait_list, &event_dth));
				OPENCL_ASSERT_OP(clFlush(dth_command_queue));
				// norm
				OPENCL_ASSERT_OP(clReleaseEvent(event_norm));
				event_norm = clCreateUserEvent(context, &err);
				OPENCL_ASSERT_OP(clRetainEvent(event_dth));
				OPENCL_ASSERT_OP(clRetainEvent(event_norm));
				#pragma omp task depend(inout: p_h_out) firstprivate(event_norm, event_dth, i, p_h_out)
				{
					OPENCL_ASSERT_OP(clWaitForEvents(1, &event_dth));
					OPENCL_ASSERT_OP(clReleaseEvent(event_dth));
					norm_calc(p_h_out, SIZE, p_sum, p_res, i);
					OPENCL_ASSERT_OP(clSetUserEventStatus(event_norm, CL_COMPLETE));
					OPENCL_ASSERT_OP(clReleaseEvent(event_norm));
				}
				#ifdef _2BUF
				SWAP(p_h_in, p_h_in_aux, float *);
				SWAP(p_h_out, p_h_out_aux, float *);
				SWAP(d_m1, d_m1_aux, cl_mem);
				SWAP(d_m2, d_m2_aux, cl_mem);
				SWAP(d_m3, d_m3_aux, cl_mem);
				SWAP(d_m4, d_m4_aux, cl_mem);
				SWAP(d_m5, d_m5_aux, cl_mem);
				SWAP(event_init, event_init_aux, cl_event);
				SWAP(event_htd, event_htd_aux, cl_event);
				SWAP(event_kernel, event_kernel_aux, cl_event);
				SWAP(event_dth, event_dth_aux, cl_event);
				SWAP(event_norm, event_norm_aux, cl_event);
				#endif // _2BUF
			}

			OPENCL_ASSERT_OP(clFinish(kernel_command_queue));
			OPENCL_ASSERT_OP(clFinish(htd_command_queue));
			OPENCL_ASSERT_OP(clFinish(dth_command_queue));
			OPENCL_ASSERT_OP(clWaitForEvents(1, &event_norm));
			exec_clock = omp_get_wtime() - exec_clock;

		} // omp single
	} // omp parallel

	/* PRINT RESULTS */
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", p_sum[N_ITER - 1], p_res[N_ITER - 1]);
	#else
	printf("\n ---------------------- RESULT ---------------------- \n");
	for (int i = 0; i < N_ITER; i++) {
		printf("\n iter: %d, sum: %lf, res: %lf", i + 1, p_sum[i], p_res[i]);
	}
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	/* RELEASE ZONE */

	OPENCL_ASSERT_OP(clReleaseEvent(event_base));
	OPENCL_ASSERT_OP(clReleaseEvent(event_init));
	OPENCL_ASSERT_OP(clReleaseEvent(event_htd));
	OPENCL_ASSERT_OP(clReleaseEvent(event_kernel));
	OPENCL_ASSERT_OP(clReleaseEvent(event_dth));
	OPENCL_ASSERT_OP(clReleaseEvent(event_norm));

	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(kernel_command_queue, h_in_pinned, p_h_in, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(kernel_command_queue, h_out_pinned, p_h_out, 0, NULL, NULL));

	#ifdef _2BUF
	OPENCL_ASSERT_OP(clReleaseEvent(event_init_aux));
	OPENCL_ASSERT_OP(clReleaseEvent(event_htd_aux));
	OPENCL_ASSERT_OP(clReleaseEvent(event_kernel_aux));
	OPENCL_ASSERT_OP(clReleaseEvent(event_dth_aux));
	OPENCL_ASSERT_OP(clReleaseEvent(event_norm_aux));

	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(kernel_command_queue, h_in_pinned_aux, p_h_in_aux, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(kernel_command_queue, h_out_pinned_aux, p_h_out_aux, 0, NULL, NULL));
	#endif // _2BUF

	OPENCL_ASSERT_OP(clFinish(kernel_command_queue));

	OPENCL_ASSERT_OP(clReleaseMemObject(h_in_pinned));
	OPENCL_ASSERT_OP(clReleaseMemObject(h_out_pinned));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m1));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m2));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m3));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m4));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m5));

	#ifdef _2BUF
	OPENCL_ASSERT_OP(clReleaseMemObject(h_in_pinned_aux));
	OPENCL_ASSERT_OP(clReleaseMemObject(h_out_pinned_aux));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m1_aux));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m2_aux));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m3_aux));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m4_aux));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_m5_aux));
	#endif // _2BUF

	OPENCL_ASSERT_OP(clReleaseMemObject(d_const1));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_const2));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_const3));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_const4));

	free(p_h_const1);
	free(p_h_const2);
	free(p_h_const3);
	free(p_h_const4);

	OPENCL_ASSERT_OP(clReleaseKernel(kernel_mult));
	OPENCL_ASSERT_OP(clReleaseProgram(program));

	OPENCL_ASSERT_OP(clReleaseCommandQueue(kernel_command_queue));
	OPENCL_ASSERT_OP(clReleaseCommandQueue(htd_command_queue));
	OPENCL_ASSERT_OP(clReleaseCommandQueue(dth_command_queue));

	free(p_res);
	free(p_sum);

	OPENCL_ASSERT_OP(clReleaseContext(context));

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
