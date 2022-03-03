#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h> //For exit()
#include <string.h>

#define SEED 6834723

#ifndef LOCAL_SIZE_0
#define LOCAL_SIZE_0 8
#endif

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

#define OPENCL_PROFILE_OP_READ   1
#define OPENCL_PROFILE_OP_WRITE  2
#define OPENCL_PROFILE_OP_KERNEL 3

#define OPENCL_PROFILE_QUEUE_MAIN  0
#define OPENCL_PROFILE_QUEUE_READ  1
#define OPENCL_PROFILE_QUEUE_WRITE 2

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_
#define OPENCL_PROFILE_READ(event)                                       \
	profiling_read_events[i_read_task] = event;                          \
	OPENCL_ASSERT_OP(clRetainEvent(profiling_read_events[i_read_task])); \
	i_read_task++;

#define OPENCL_PROFILE_WRITE(event)                                        \
	profiling_write_events[i_write_task] = event;                          \
	OPENCL_ASSERT_OP(clRetainEvent(profiling_write_events[i_write_task])); \
	i_write_task++;

#define OPENCL_PROFILE_KERNEL(event)                                         \
	profiling_kernel_events[i_kernel_task] = event;                          \
	OPENCL_ASSERT_OP(clRetainEvent(profiling_kernel_events[i_kernel_task])); \
	i_kernel_task++;

#define OPENCL_PROFILE_FIRST(event) \
	first_profiling_event = event;  \
	OPENCL_ASSERT_OP(clRetainEvent(first_profiling_event));

#define OPENCL_PROFILE_LAST(event) \
	last_profiling_event = event;  \
	OPENCL_ASSERT_OP(clRetainEvent(last_profiling_event));

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_
typedef struct {
	cl_event event;
	int      queue;
	int      op;
} visual_event;

#define OPENCL_PROFILE_VISUAL(command_queue, visual_event, operation)              \
	profiling_visual_events[i_visual_task].event = visual_event;                   \
	profiling_visual_events[i_visual_task].queue = command_queue;                  \
	profiling_visual_events[i_visual_task].op    = operation;                      \
	OPENCL_ASSERT_OP(clRetainEvent(profiling_visual_events[i_visual_task].event)); \
	i_visual_task++;
#else
#define OPENCL_PROFILE_VISUAL(queue, event, op)
#endif
#else
#define OPENCL_PROFILE_READ(event)
#define OPENCL_PROFILE_WRITE(event)
#define OPENCL_PROFILE_KERNEL(event)
#define OPENCL_PROFILE_FIRST(event)
#define OPENCL_PROFILE_LAST(event)
#define OPENCL_PROFILE_VISUAL(queue, event, op)
#endif

#define MATRIX_ADD_KERNEL_NAME "Add"

#define MATRIX_ADD_KERNEL \
	"__kernel void Add(const unsigned long SIZE_0, const long N_ITER, __global const float *d_a, __global const float *d_b, __global float *d_c) { \
		if (get_global_id(0) >= SIZE_0 * SIZE_0) { \
			return; \
		} \
		int thread_x = get_global_id(0); \
		int i; \
		\
		for (i = 0; i < N_ITER; i++) { \
			d_c[thread_x] = d_c[thread_x] + d_a[thread_x] + d_b[thread_x]; \
		} \
	}"

double main_clock;
double exec_clock;

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();
	/* ARGUMENTS */

	if (argc < 5) {
		fprintf(stderr, "Usage: %s <size> <numIter> <platform> <device>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	cl_long SIZE     = atoi(argv[1]);
	cl_long N_ITER   = atoi(argv[2]);
	cl_int  PLATFORM = atoi(argv[3]);
	cl_int  DEVICE   = atoi(argv[4]);

	size_t MATRIX_SIZE = sizeof(cl_float) * SIZE * SIZE;

	cl_int n_read_taks   = 1;
	cl_int n_write_taks  = 3;
	cl_int n_kernel_taks = 1;

	/* VARIABLES */

	int    i;
	cl_int err;

	char *kernel_raw = MATRIX_ADD_KERNEL;

	size_t kernel_size = strlen(kernel_raw);

	size_t local_size[1];
	size_t global_size[1];

	double result;
	double sum;

	cl_platform_id platform_id;
	cl_device_id   device_id;

	cl_context                  context;
	cl_program                  program;
	cl_kernel                   kernel;
	cl_command_queue_properties properties;

	cl_command_queue main_command_queue;
	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	cl_command_queue write_command_queue;
	cl_command_queue read_command_queue;
	#endif
	cl_mem mem_a;
	cl_mem mem_pinned_a;

	cl_mem mem_b;
	cl_mem mem_pinned_b;

	cl_mem mem_c;
	cl_mem mem_pinned_c;

	cl_float *p_pinned_a;
	cl_float *p_pinned_b;
	cl_float *p_pinned_c;

	cl_event event_write_a;

	cl_event event_write_b;

	cl_event event_write_c;
	cl_event event_read_c;

	cl_event event_kernel;

	cl_event event_wait_list[3];

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_
	cl_profiling_info profiling_info_start = CL_PROFILING_COMMAND_START;
	cl_profiling_info profiling_info_end   = CL_PROFILING_COMMAND_END;

	cl_event first_profiling_event;
	cl_event last_profiling_event;

	cl_event *profiling_read_events   = (cl_event *)malloc(n_read_taks * sizeof(cl_event));
	cl_event *profiling_write_events  = (cl_event *)malloc(n_write_taks * sizeof(cl_event));
	cl_event *profiling_kernel_events = (cl_event *)malloc(n_kernel_taks * sizeof(cl_event));

	int i_read_task   = 0;
	int i_write_task  = 0;
	int i_kernel_task = 0;

	cl_ulong profiling_total      = 0;
	cl_ulong profiling_sum        = 0;
	cl_ulong profiling_offloading = 0;
	cl_ulong profiling_read       = 0;
	cl_ulong profiling_write      = 0;
	cl_ulong profiling_kernel     = 0;

	cl_ulong profiling_start = 0;
	cl_ulong profiling_end   = 0;

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_
	int n_visual_task = n_update_taks + n_copy_taks + n_read_taks + n_write_taks;
	int i_visual_task = 0;

	visual_event *profiling_visual_events = (visual_event *)malloc(n_visual_task * sizeof(visual_event));
	#endif
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_

	/* PLATFORMS & DEVICES */
	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL));
	platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL));
	device_id = p_devices[DEVICE];
	free(p_devices);

	size_t platform_name_size;
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size));
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL));

	size_t device_name_size;
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size));
	char *device_name = (char *)malloc(sizeof(char) * device_name_size);
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL));

	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n SIZE: %ld", SIZE);
	printf("\n N_ITER: %ld", N_ITER);
	printf("\n PLATFORM: %s", platform_name);
	printf("\n DEVICE: %s", device_name);
	printf("\n TOTAL TASKS: %d", n_kernel_taks + n_read_taks + n_write_taks);
	printf("\n KERNEL TASKS: %d", n_kernel_taks);
	printf("\n OFFLOADING TASKS: %d", n_read_taks + n_write_taks);
	printf("\n READ TASKS: %d", n_read_taks);
	printf("\n WRITE TASKS: %d", n_write_taks);
	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	printf("\n POLICY ASYNC");
	#else // _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	printf("\n POLICY SYNC");
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	printf("\n\n ---------------------------------------------------- \n");
	free(platform_name);
	free(device_name);

	/* SET UP, CONTEXTO, COLAS, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	properties = 0;

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_
	properties |= CL_QUEUE_PROFILING_ENABLE;
	#endif

	main_command_queue = clCreateCommandQueue(context, device_id, properties, &err);
	OPENCL_ASSERT_ERROR(err);

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	read_command_queue = clCreateCommandQueue(context, device_id, properties, &err);
	OPENCL_ASSERT_ERROR(err);
	write_command_queue = clCreateCommandQueue(context, device_id, properties, &err);
	OPENCL_ASSERT_ERROR(err);
	#endif

	mem_pinned_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	p_pinned_a = (float *)clEnqueueMapBuffer(main_command_queue, mem_pinned_a, CL_FALSE, CL_MAP_WRITE, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	mem_a = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	mem_pinned_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	p_pinned_b = (float *)clEnqueueMapBuffer(main_command_queue, mem_pinned_b, CL_FALSE, CL_MAP_WRITE, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	mem_b = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	mem_pinned_c = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, MATRIX_SIZE, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	p_pinned_c = (float *)clEnqueueMapBuffer(main_command_queue, mem_pinned_c, CL_FALSE, CL_MAP_READ | CL_MAP_WRITE, 0, MATRIX_SIZE, 0, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	mem_c = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw), (const size_t *)(&kernel_size), &err);
	OPENCL_ASSERT_ERROR(err);

	OPENCL_ASSERT_OP(clBuildProgram(program, 1, &device_id, NULL, NULL, NULL));

	kernel = clCreateKernel(program, MATRIX_ADD_KERNEL_NAME, &err);
	OPENCL_ASSERT_ERROR(err);

	err = clSetKernelArg(kernel, 0, sizeof(cl_long), &SIZE);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_long), &N_ITER);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &mem_a);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &mem_b);
	err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &mem_c);
	OPENCL_ASSERT_ERROR(err);

	local_size[0] = LOCAL_SIZE_0;

	global_size[0] = SIZE * SIZE;

	if ((global_size[0] % local_size[0]) != 0) {
		global_size[0] += (local_size[0] - (global_size[0] % local_size[0]));
	}

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_
	printf("%lu %lu\n", local_size[0], global_size[0]);
	#else
	printf("\n ------------------ WORK-GROUP-INFO ----------------- \n");
	printf("\n Local Size: %lu", local_size[0]);
	printf("\n Global Size: %lu", global_size[0]);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_

	srand(SEED);

	for (i = 0; i < SIZE * SIZE; i++) {
		#ifdef _CTRL_EXAMPLES_OPENCL_GPU_DEBUG_
		p_pinned_a[i] = 1;
		p_pinned_b[i] = 1;
		#else
		p_pinned_a[i] = (-1 + (2 * (((float)rand()) / RAND_MAX)));
		p_pinned_b[i] = (-1 + (2 * (((float)rand()) / RAND_MAX)));
		#endif // _CTRL_EXAMPLES_OPENCL_GPU_DEBUG_
		p_pinned_c[i] = 0;
	}

	OPENCL_ASSERT_OP(clFlush(main_command_queue));
	OPENCL_ASSERT_OP(clFinish(main_command_queue));

	exec_clock = omp_get_wtime();

	/* COPIA HOST TO DEVICE */

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(write_command_queue, mem_a, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_a, 0, NULL, &event_write_a));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_WRITE, event_write, OPENCL_PROFILE_OP_WRITE);
	#else
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(main_command_queue, mem_a, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_a, 0, NULL, &event_write_a));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_MAIN, event_write_a, OPENCL_PROFILE_OP_WRITE);
	#endif
	OPENCL_PROFILE_WRITE(event_write_a);
	OPENCL_PROFILE_FIRST(event_write_a);

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(write_command_queue, mem_b, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_b, 0, NULL, &event_write_b));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_WRITE, event_write_b, OPENCL_PROFILE_OP_WRITE);
	#else
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(main_command_queue, mem_b, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_b, 0, NULL, &event_write_b));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_MAIN, event_write_b, OPENCL_PROFILE_OP_WRITE);
	#endif
	OPENCL_PROFILE_WRITE(event_write_b);

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(write_command_queue, mem_c, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_c, 0, NULL, &event_write_c));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_WRITE, event_write_c, OPENCL_PROFILE_OP_WRITE);
	#else
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(main_command_queue, mem_c, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_c, 0, NULL, &event_write_c));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_MAIN, event_write_c, OPENCL_PROFILE_OP_WRITE);
	#endif
	OPENCL_PROFILE_WRITE(event_write_c);

	event_wait_list[0] = event_write_a;
	event_wait_list[1] = event_write_b;
	event_wait_list[2] = event_write_c;

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(main_command_queue, kernel, 1, NULL, global_size, local_size, 3, event_wait_list, &event_kernel));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_MAIN, event_kernel, OPENCL_PROFILE_OP_KERNEL);
	OPENCL_PROFILE_KERNEL(event_kernel);

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	OPENCL_ASSERT_OP(clEnqueueReadBuffer(read_command_queue, mem_c, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_c, 1, &event_kernel, &event_read_c));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_READ, event_read_c, OPENCL_PROFILE_OP_READ);
	#else
	OPENCL_ASSERT_OP(clEnqueueReadBuffer(main_command_queue, mem_c, CL_FALSE, 0, MATRIX_SIZE, (void *)p_pinned_c, 1, &event_kernel, &event_read_c));
	OPENCL_PROFILE_VISUAL(OPENCL_PROFILE_QUEUE_MAIN, event_read, OPENCL_PROFILE_OP_READ);
	#endif
	OPENCL_PROFILE_READ(event_read_c);
	OPENCL_PROFILE_LAST(event_read_c);

	OPENCL_ASSERT_OP(clFlush(main_command_queue));
	OPENCL_ASSERT_OP(clFinish(main_command_queue));

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	OPENCL_ASSERT_OP(clFlush(read_command_queue));
	OPENCL_ASSERT_OP(clFinish(read_command_queue));

	OPENCL_ASSERT_OP(clFlush(write_command_queue));
	OPENCL_ASSERT_OP(clFinish(write_command_queue));
	#endif

	exec_clock = omp_get_wtime() - exec_clock;

	/* CALCULO RESULTADOS */

	sum = result = 0;
	for (i = 0; i < SIZE * SIZE; i++) {
		sum += pow(p_pinned_c[i], 2);
	}
	result = sqrt(sum);

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_
	printf("%lf %lf\n", sum, result);
	#else
	printf("\n ---------------------- RESULT ---------------------- \n");
	printf("\n Sum: %lf", sum);
	printf("\n Result: %lf", result);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_
	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_
	FILE *f = fopen("visual_profiler_info.txt", "w");

	for (i = 0; i < n_visual_task; i++) {
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_visual_events[i].event, profiling_info_start, sizeof(cl_ulong), (void *)(&profiling_start), NULL));
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_visual_events[i].event, profiling_info_end, sizeof(cl_ulong), (void *)(&profiling_end), NULL));

		fprintf(f, "%d; %d; %lu; %lu\n", profiling_visual_events[i].queue, profiling_visual_events[i].op, profiling_start, profiling_end);
		OPENCL_ASSERT_OP(clReleaseEvent(profiling_visual_events[i].event));
	}

	fclose(f);
	free(profiling_visual_events);
	#endif
	for (i = 0; i < n_read_taks; i++) {
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_read_events[i], profiling_info_start, sizeof(cl_ulong), (void *)(&profiling_start), NULL));
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_read_events[i], profiling_info_end, sizeof(cl_ulong), (void *)(&profiling_end), NULL));

		profiling_sum += profiling_end - profiling_start;
		profiling_read += profiling_end - profiling_start;
		profiling_offloading += profiling_end - profiling_start;

		OPENCL_ASSERT_OP(clReleaseEvent(profiling_read_events[i]));
	}
	free(profiling_read_events);

	for (i = 0; i < n_write_taks; i++) {
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_write_events[i], profiling_info_start, sizeof(cl_ulong), (void *)(&profiling_start), NULL));
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_write_events[i], profiling_info_end, sizeof(cl_ulong), (void *)(&profiling_end), NULL));

		profiling_sum += profiling_end - profiling_start;
		profiling_write += profiling_end - profiling_start;
		profiling_offloading += profiling_end - profiling_start;

		OPENCL_ASSERT_OP(clReleaseEvent(profiling_write_events[i]));
	}
	free(profiling_write_events);

	for (i = 0; i < n_kernel_taks; i++) {
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_kernel_events[i], profiling_info_start, sizeof(cl_ulong), (void *)(&profiling_start), NULL));
		OPENCL_ASSERT_OP(clGetEventProfilingInfo(profiling_kernel_events[i], profiling_info_end, sizeof(cl_ulong), (void *)(&profiling_end), NULL));

		profiling_sum += profiling_end - profiling_start;
		profiling_kernel += profiling_end - profiling_start;

		OPENCL_ASSERT_OP(clReleaseEvent(profiling_kernel_events[i]));
	}
	free(profiling_kernel_events);

	OPENCL_ASSERT_OP(clGetEventProfilingInfo(first_profiling_event, profiling_info_start, sizeof(cl_ulong), (void *)(&profiling_start), NULL));
	OPENCL_ASSERT_OP(clGetEventProfilingInfo(last_profiling_event, profiling_info_end, sizeof(cl_ulong), (void *)(&profiling_end), NULL));

	profiling_total = profiling_end - profiling_start;

	OPENCL_ASSERT_OP(clReleaseEvent(first_profiling_event));
	OPENCL_ASSERT_OP(clReleaseEvent(last_profiling_event));

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_
	printf("%.3lf %.3lf %.3lf %.3lf %.3lf\n", profiling_total, profiling_sum, profiling_kernel, profiling_offloading, profiling_read, profiling_write);
	#else
	printf("\n -------------------- PROFILING --------------------- \n");
	printf("\n Total Time: %.3lf", profiling_total / 1000000.0);
	printf("\n Acumulate Sum Time: %.3lf", profiling_sum / 1000000.0);
	printf("\n Kernel Time: %.3lf", profiling_kernel / 1000000.0);
	printf("\n Offloading Time: %.3lf", profiling_offloading / 1000000.0);
	printf("\n Read Time: %.3lf", profiling_read / 1000000.0);
	printf("\n Write Time: %.3lf", profiling_write / 1000000.0);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_

	/* RELEASE ZONE */

	OPENCL_ASSERT_OP(clReleaseEvent(event_write_a));
	OPENCL_ASSERT_OP(clReleaseEvent(event_write_b));
	OPENCL_ASSERT_OP(clReleaseEvent(event_read_c));
	OPENCL_ASSERT_OP(clReleaseEvent(event_write_c));
	OPENCL_ASSERT_OP(clReleaseEvent(event_kernel));

	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(main_command_queue, mem_pinned_a, p_pinned_a, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(main_command_queue, mem_pinned_b, p_pinned_b, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(main_command_queue, mem_pinned_c, p_pinned_c, 0, NULL, NULL));

	OPENCL_ASSERT_OP(clFlush(main_command_queue));
	OPENCL_ASSERT_OP(clFinish(main_command_queue));

	OPENCL_ASSERT_OP(clReleaseMemObject(mem_pinned_a));
	OPENCL_ASSERT_OP(clReleaseMemObject(mem_a));

	OPENCL_ASSERT_OP(clReleaseMemObject(mem_pinned_b));
	OPENCL_ASSERT_OP(clReleaseMemObject(mem_b));

	OPENCL_ASSERT_OP(clReleaseMemObject(mem_pinned_c));
	OPENCL_ASSERT_OP(clReleaseMemObject(mem_c));

	OPENCL_ASSERT_OP(clReleaseKernel(kernel));
	OPENCL_ASSERT_OP(clReleaseProgram(program));

	OPENCL_ASSERT_OP(clReleaseCommandQueue(main_command_queue));
	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_POLICY_ASYNC_
	OPENCL_ASSERT_OP(clReleaseCommandQueue(read_command_queue));
	OPENCL_ASSERT_OP(clReleaseCommandQueue(write_command_queue));
	#endif

	OPENCL_ASSERT_OP(clReleaseContext(context));

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_
	printf("%lf %lf\n", main_clock, exec_clock);
	#else
	printf("\n ---------------------- TIMERS ---------------------- \n");
	printf("Clock main: %lf\n", main_clock);
	printf("Clock exec: %lf\n", exec_clock);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_TEST_MODE_

	return 0;
}
