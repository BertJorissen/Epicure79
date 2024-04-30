/**
 * @file Sobel_YUV_OpenCL_Gpu_Ref_Async_MTF.c
 * @author Trasgo Group
 * @brief SobelYUV: Asynchronous native OpenCLGPU mem to file version
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

#include <CL/cl.h>
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _PROFILING_ENABLED_
#include <roctx.h>
#endif //_PROFILING_ENABLED_

#define SEED 6834723

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_DEBUG_
#define OPENCL_ASSERT_OP(operation)      \
	err = operation;                     \
	fprintf(stderr, "error: %d\n", err); \
	exit(EXIT_FAILURE);

#define OPENCL_ASSERT_ERROR(err)         \
	fprintf(stderr, "error: %d\n", err); \
	exit(EXIT_FAILURE);
#else
#define OPENCL_ASSERT_OP(operation) operation
#define OPENCL_ASSERT_ERROR(err)
#endif

typedef unsigned char BYTE;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2

#define N_QUEUES     4
#define QUEUE_Y      0
#define QUEUE_U      1
#define QUEUE_V      2
#define QUEUE_KERNEL 3

#define N_EVENTS        5
#define EVENT_Y         0
#define EVENT_U         1
#define EVENT_V         2
#define EVENT_HOST_LOAD 3
#define EVENT_HOST_SAVE 4

#define N_EVENTS_KERNEL 3
#define EVENT_KERNEL_Y  0
#define EVENT_KERNEL_U  1
#define EVENT_KERNEL_V  2

#define SOBEL_YUV_KERNEL_NAME_SOBEL_OPERATION "Sobel_Operation"

#define SOBEL_YUV_KERNEL_SOBEL_OPERATION \
	" __kernel void Sobel_Operation( __global const BYTE *Input, __global BYTE *Output, const int Width, const int Height) { \
		float Gradient_h, Gradient_v, Gradient_mod; \
		\
		unsigned int Col_Index = get_global_id(0); \
		unsigned int Row_Index = get_global_id(1); \
		\
		if ((Row_Index != 0) && (Col_Index != 0) && (Row_Index < Height - 1) && (Col_Index < Width - 1)) { \
			Gradient_v = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] + \
						Input[(Row_Index - 1) * Width + (Col_Index + 1)] - \
						2 * Input[Row_Index * Width + (Col_Index - 1)] + \
						2 * Input[Row_Index * Width + (Col_Index + 1)] - \
						Input[(Row_Index + 1) * Width + (Col_Index - 1)] + \
						Input[(Row_Index + 1) * Width + (Col_Index + 1)]); \
			Gradient_h = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] - \
						2 * Input[(Row_Index - 1) * Width + Col_Index] - \
						Input[(Row_Index - 1) * Width + (Col_Index + 1)] + \
						Input[(Row_Index + 1) * Width + (Col_Index - 1)] + \
						2 * Input[(Row_Index + 1) * Width + Col_Index] + \
						Input[(Row_Index + 1) * Width + (Col_Index + 1)]); \
			Gradient_mod = sqrt(Gradient_h * Gradient_h + Gradient_v * Gradient_v); \
			Output[Row_Index * Width + Col_Index] = ((int) Gradient_mod < 256) ? (BYTE) Gradient_mod : 255; \
		} \
	}"

double main_clock;
double exec_clock;

void Preload_Frame(BYTE *Input_Img[N_IMG], FILE *File_reader, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader);
	}
}

void Get_Frame(BYTE *buffer_read[N_IMG], BYTE *Input_Img[N_IMG], size_t *sizes) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Host get frame");
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < N_IMG; i++)
		memcpy(Input_Img[i], buffer_read[i], sizeof(BYTE) * sizes[i]);

	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Host save frame");
	#endif //_PROFILING_ENABLED_

	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}

	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/* ARGUMENT PARSE */
	if (argc < 8) {
		printf("Usage: %s <width> <height> <num_frames> <input_yuv_file> <output_yuv_file> <device> <platform>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int Width[N_IMG];
	Width[IMG_Y] = atoi(argv[1]);
	Width[IMG_U] = Width[IMG_V] = Width[IMG_Y] / 2;

	int Height[N_IMG];
	Height[IMG_Y] = atoi(argv[2]);
	Height[IMG_U] = Height[IMG_V] = Height[IMG_Y] / 2;

	int Num_Frames = atoi(argv[3]);

	char *Input_Filename  = argv[4];
	char *Output_Filename = argv[5];

	int DEVICE   = atoi(argv[6]);
	int PLATFORM = atoi(argv[7]);

	/* VARIABLES */

	int Frame_num = 0; // loop variable

	FILE *File_reader, *File_writer;

	cl_int err;

	char *kernel_raw_sobel_operation = SOBEL_YUV_KERNEL_SOBEL_OPERATION;

	char *kernel_raw = (char *)malloc((strlen(kernel_raw_sobel_operation) + 300) * sizeof(char));

	sprintf(&kernel_raw[0],
			" #define LOCAL_SIZE_0  %d \n#define LOCAL_SIZE_1 %d \ntypedef unsigned char BYTE;\n",
			LOCAL_SIZE_0, LOCAL_SIZE_1);

	strcat(kernel_raw, kernel_raw_sobel_operation);

	size_t kernel_size = strlen(kernel_raw);

	size_t local_size[2];
	size_t global_sizes[N_IMG][2];

	size_t sizes[N_IMG] = {
		(size_t)(Width[IMG_Y] * Height[IMG_Y]),
		(size_t)(Width[IMG_U] * Height[IMG_U]),
		(size_t)(Width[IMG_V] * Height[IMG_V])};

	cl_platform_id platform_id;
	cl_device_id   device_id;

	cl_context context;
	cl_program program;
	cl_kernel  kernel_sobel_operation;

	cl_command_queue_properties properties;
	cl_command_queue            queues[N_QUEUES];

	cl_event default_event;
	cl_event events[N_EVENTS];
	cl_event events_kernel[N_EVENTS_KERNEL];
	cl_event wait_events[5];
	cl_event aux_event_host;
	cl_event aux_host_wait_events[3];
	cl_event host_wait_y;
	cl_event host_wait_u;
	cl_event host_wait_v;

	cl_mem mem_input_img[N_IMG];
	cl_mem mem_pinned_input_img[N_IMG];
	BYTE  *p_pinned_input_img[N_IMG];

	cl_mem mem_output_img[N_IMG];
	cl_mem mem_pinned_output_img[N_IMG];
	BYTE  *p_pinned_output_img[N_IMG];

	BYTE ***buffer_read;

	buffer_read = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);

	for (int i = 0; i < Num_Frames; i++) {
		buffer_read[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);

		for (int j = 0; j < N_IMG; j++) {
			buffer_read[i][j] = (BYTE *)malloc(sizeof(BYTE) * sizes[j]);
		}
	}

	/* OPEN AND CLOSE FILE OPERATION */

	if (!(File_reader = fopen(Input_Filename, "rb"))) {
		printf("\nError in opening input file: %s\n", Input_Filename);
		exit(EXIT_FAILURE);
	}
	if (!(File_writer = fopen(Output_Filename, "wb+"))) {
		printf("\nError in opening output file: %s\n", Output_Filename);
		exit(EXIT_FAILURE);
	}

	/* PLATFORMS & DEVICES */

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL));
	platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL));
	device_id = p_devices[DEVICE];
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
	printf("%s, %s, ", device_name, platform_name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n WIDTH: %d", Width[0]);
	printf("\n HEIGHT: %d", Height[0]);
	printf("\n NUM_FRAMES: %d", Num_Frames);
	printf("\n DEVICE: %s", device_name);
	printf("\n PLATFORM: %s", platform_name);
	printf("\n POLICY ASYNC");
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);
	free(platform_name);
	free(device_name);

	/* SET UP, CONTEXT, QUEUES, KERNELS, ETC */

	cl_context_properties context_properties[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platform_id,
		0};
	context = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw),
										(const size_t *)(&kernel_size), &err);
	OPENCL_ASSERT_ERROR(err);

	err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE) {
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *)malloc(log_size);
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("%s\n", log);
		free(log);
	}
	OPENCL_ASSERT_ERROR(err);

	kernel_sobel_operation = clCreateKernel(program, SOBEL_YUV_KERNEL_NAME_SOBEL_OPERATION, &err);
	OPENCL_ASSERT_ERROR(err);

	local_size[0] = LOCAL_SIZE_0;
	local_size[1] = LOCAL_SIZE_1;

	for (int i = 0; i < N_IMG; i++) {
		global_sizes[i][0] = Width[i];
		global_sizes[i][1] = Height[i];

		if ((Width[i] % local_size[0]) != 0) {
			global_sizes[i][0] += (local_size[0] - (Width[i] % local_size[0]));
		}

		if ((Height[i] % local_size[1]) != 0) {
			global_sizes[i][1] += (local_size[1] - (Height[i] % local_size[1]));
		}
	}

	properties = 0;
	for (int i = 0; i < N_QUEUES; i++) {
		queues[i] = clCreateCommandQueue(context, device_id, properties, &err);
		OPENCL_ASSERT_ERROR(err);
	}

	default_event = clCreateUserEvent(context, &err);
	OPENCL_ASSERT_ERROR(err);

	OPENCL_ASSERT_OP(clSetUserEventStatus(default_event, CL_COMPLETE));

	for (int i = 0; i < N_EVENTS; i++) {
		events[i] = default_event;
	}

	for (int i = 0; i < N_EVENTS_KERNEL; i++) {
		events_kernel[i] = default_event;
	}

	for (int i = 0; i < N_IMG; i++) {
		mem_input_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		OPENCL_ASSERT_ERROR(err);
		mem_pinned_input_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizes[i] * sizeof(BYTE), NULL, &err);
		OPENCL_ASSERT_ERROR(err);
		p_pinned_input_img[i] = (BYTE *)clEnqueueMapBuffer(queues[0], mem_pinned_input_img[i], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL, &err);
		OPENCL_ASSERT_ERROR(err);

		mem_output_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		OPENCL_ASSERT_ERROR(err);
		cl_uint pattern = 0;
		OPENCL_ASSERT_OP(clEnqueueFillBuffer(queues[0], mem_output_img[i], &pattern, sizeof(cl_uint), 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL));
		mem_pinned_output_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizes[i] * sizeof(BYTE), NULL, &err);
		OPENCL_ASSERT_ERROR(err);
		p_pinned_output_img[i] = (BYTE *)clEnqueueMapBuffer(queues[0], mem_pinned_output_img[i], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL, &err);
		OPENCL_ASSERT_ERROR(err);
	}

	for (int i = 0; i < Num_Frames; i++) {
		Preload_Frame(buffer_read[i], File_reader, sizes);
	}

	omp_set_num_threads(2);
	#pragma omp parallel
	{
		#pragma omp master
		{
			for (int i = 0; i < N_QUEUES; i++) {
				OPENCL_ASSERT_OP(clFlush(queues[i]));
			}

			for (int i = 0; i < N_QUEUES; i++) {
				OPENCL_ASSERT_OP(clFinish(queues[i]));
			}

			exec_clock = omp_get_wtime();

			events[EVENT_HOST_LOAD] = clCreateUserEvent(context, &err);
			OPENCL_ASSERT_ERROR(err);
			aux_event_host = events[EVENT_HOST_LOAD];

			#pragma omp task depend(inout: sizes) firstprivate(aux_event_host)
			{
				Get_Frame(buffer_read[0], p_pinned_input_img, sizes);
				OPENCL_ASSERT_OP(clSetUserEventStatus(aux_event_host, CL_COMPLETE));
			}

			for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {

				for (int i = 0; i < N_IMG; i++) {
					wait_events[0] = events[EVENT_HOST_LOAD];
					wait_events[1] = events_kernel[i];
					OPENCL_ASSERT_OP(clEnqueueWriteBuffer(queues[i], mem_input_img[i],
														  CL_FALSE,
														  0, sizes[i] * sizeof(BYTE), (void *)p_pinned_input_img[i],
														  2, wait_events,
														  &events[i]));
					OPENCL_ASSERT_OP(clFlush(queues[i]));
					aux_host_wait_events[i] = events[i];

					OPENCL_ASSERT_OP(clSetKernelArg(kernel_sobel_operation, 0, sizeof(cl_mem), &mem_input_img[i]));
					OPENCL_ASSERT_OP(clSetKernelArg(kernel_sobel_operation, 1, sizeof(cl_mem), &mem_output_img[i]));
					OPENCL_ASSERT_OP(clSetKernelArg(kernel_sobel_operation, 2, sizeof(cl_int), &Width[i]));
					OPENCL_ASSERT_OP(clSetKernelArg(kernel_sobel_operation, 3, sizeof(cl_int), &Height[i]));
					OPENCL_ASSERT_OP(clSetKernelArg(kernel_sobel_operation, 4, sizeof(cl_int), &Width[0]));
					OPENCL_ASSERT_OP(clSetKernelArg(kernel_sobel_operation, 5, sizeof(cl_int), &Height[0]));

					OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queues[QUEUE_KERNEL], kernel_sobel_operation,
															2, NULL, global_sizes[i], local_size,
															1, &events[i],
															&events_kernel[i]));
					OPENCL_ASSERT_OP(clFlush(queues[QUEUE_KERNEL]));

					wait_events[0] = events[EVENT_HOST_SAVE];
					wait_events[1] = events_kernel[i];
					OPENCL_ASSERT_OP(clEnqueueReadBuffer(queues[i], mem_output_img[i],
														 CL_FALSE,
														 0, sizes[i] * sizeof(BYTE), (void *)p_pinned_output_img[i],
														 2, wait_events,
														 &events[i]));
					OPENCL_ASSERT_OP(clFlush(queues[i]));
				}

				if (Frame_num + 1 < Num_Frames) {
					events[EVENT_HOST_LOAD] = clCreateUserEvent(context, &err);
					OPENCL_ASSERT_ERROR(err);
					aux_event_host = events[EVENT_HOST_LOAD];
					host_wait_y    = aux_host_wait_events[IMG_Y];
					host_wait_u    = aux_host_wait_events[IMG_U];
					host_wait_v    = aux_host_wait_events[IMG_V];

					#pragma omp task depend(inout: sizes) firstprivate(aux_event_host, host_wait_y, host_wait_u, host_wait_v, Frame_num)
					{
						cl_event host_wait_list[3];
						host_wait_list[IMG_Y] = host_wait_y;
						host_wait_list[IMG_U] = host_wait_u;
						host_wait_list[IMG_V] = host_wait_v;

						OPENCL_ASSERT_OP(clWaitForEvents(3, host_wait_list));
						OPENCL_ASSERT_OP(clReleaseEvent(host_wait_y));
						OPENCL_ASSERT_OP(clReleaseEvent(host_wait_u));
						OPENCL_ASSERT_OP(clReleaseEvent(host_wait_v));

						Get_Frame(buffer_read[Frame_num + 1], p_pinned_input_img, sizes);
						OPENCL_ASSERT_OP(clSetUserEventStatus(aux_event_host, CL_COMPLETE));
					}
				}

				events[EVENT_HOST_SAVE] = clCreateUserEvent(context, &err);
				OPENCL_ASSERT_ERROR(err);
				aux_event_host = events[EVENT_HOST_SAVE];
				host_wait_y    = events[IMG_Y];
				host_wait_u    = events[IMG_U];
				host_wait_v    = events[IMG_V];

				#pragma omp task depend(inout: sizes) firstprivate(aux_event_host, host_wait_y, host_wait_u, host_wait_v)
				{
					cl_event host_wait_list[3];
					host_wait_list[IMG_Y] = host_wait_y;
					host_wait_list[IMG_U] = host_wait_u;
					host_wait_list[IMG_V] = host_wait_v;
					// These calls are added to the code in order to help to synchronize the access of the event in the task with
					// the update of the event in the main thread. It solves deadk-lock problems found in some possibly non thread-safe implementations of OpenCL
					OPENCL_ASSERT_OP(clGetEventInfo(host_wait_y, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), NULL, NULL));
					OPENCL_ASSERT_OP(clGetEventInfo(host_wait_u, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), NULL, NULL));
					OPENCL_ASSERT_OP(clGetEventInfo(host_wait_v, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), NULL, NULL));

					OPENCL_ASSERT_OP(clWaitForEvents(3, host_wait_list));
					OPENCL_ASSERT_OP(clReleaseEvent(host_wait_y));
					OPENCL_ASSERT_OP(clReleaseEvent(host_wait_u));
					OPENCL_ASSERT_OP(clReleaseEvent(host_wait_v));

					Save_Frame(p_pinned_output_img, File_writer, sizes);
					OPENCL_ASSERT_OP(clSetUserEventStatus(aux_event_host, CL_COMPLETE));
				}
			}

			for (int i = 0; i < N_QUEUES; i++) {
				OPENCL_ASSERT_OP(clFlush(queues[i]));
			}

			for (int i = 0; i < N_QUEUES; i++) {
				OPENCL_ASSERT_OP(clFinish(queues[i]));
			}

			OPENCL_ASSERT_OP(clWaitForEvents(1, &events[EVENT_HOST_SAVE]));

			exec_clock = omp_get_wtime() - exec_clock;
		}
	}

	/* RELEASE ZONE */

	for (int i = 0; i < N_IMG; i++) {
		OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(queues[0], mem_pinned_input_img[i], p_pinned_input_img[i], 0, NULL, NULL));
		OPENCL_ASSERT_OP(clEnqueueUnmapMemObject(queues[0], mem_pinned_output_img[i], p_pinned_output_img[i], 0, NULL, NULL));
		OPENCL_ASSERT_OP(clFinish(queues[0]));

		OPENCL_ASSERT_OP(clReleaseMemObject(mem_pinned_input_img[i]));
		OPENCL_ASSERT_OP(clReleaseMemObject(mem_input_img[i]));
		OPENCL_ASSERT_OP(clReleaseMemObject(mem_pinned_output_img[i]));
		OPENCL_ASSERT_OP(clReleaseMemObject(mem_output_img[i]));
	}

	for (int i = 0; i < Num_Frames; i++) {
		for (int j = 0; j < N_IMG; j++) {
			free(buffer_read[i][j]);
		}
		free(buffer_read[i]);
	}
	free(buffer_read);

	OPENCL_ASSERT_OP(clReleaseKernel(kernel_sobel_operation));
	OPENCL_ASSERT_OP(clReleaseProgram(program));

	for (int i = 0; i < N_QUEUES; i++) {
		OPENCL_ASSERT_OP(clReleaseCommandQueue(queues[i]));
	}

	OPENCL_ASSERT_OP(clReleaseContext(context));

	fclose(File_reader);
	fclose(File_writer);

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
