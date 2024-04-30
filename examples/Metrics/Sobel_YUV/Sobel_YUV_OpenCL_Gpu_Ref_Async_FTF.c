#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEED 6834723

#define DEVICE 0

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
			Gradient_v = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] + Input[(Row_Index - 1) * Width + (Col_Index + 1)] - 2 * Input[Row_Index * Width + (Col_Index - 1)] + 2 * Input[Row_Index * Width + (Col_Index + 1)] - Input[(Row_Index + 1) * Width + (Col_Index - 1)] + Input[(Row_Index + 1) * Width + (Col_Index + 1)]); \
			Gradient_h = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] - 2 * Input[(Row_Index - 1) * Width + Col_Index] - Input[(Row_Index - 1) * Width + (Col_Index + 1)] + Input[(Row_Index + 1) * Width + (Col_Index - 1)] + 2 * Input[(Row_Index + 1) * Width + Col_Index] + Input[(Row_Index + 1) * Width + (Col_Index + 1)]); \
			Gradient_mod = sqrt(Gradient_h * Gradient_h + Gradient_v * Gradient_v); \
			Output[Row_Index * Width + Col_Index] = ((int) Gradient_mod < 256) ? (BYTE) Gradient_mod : 255; \
		} \
	}"

double main_clock;
double exec_clock;

void Load_Frame(BYTE *Input_Img[N_IMG], FILE *File_reader, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader);
	}
}

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/* ARGUMENT PARSE */
	if (argc < 7) {
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

	int PLATFORM = atoi(argv[7]);

	/* VARIABLES */

	int Frame_num = 0; // loop variable

	FILE *File_reader, *File_writer;

	cl_int err;

	char *kernel_raw_sobel_operation = SOBEL_YUV_KERNEL_SOBEL_OPERATION;

	char *kernel_raw = (char *)malloc((strlen(kernel_raw_sobel_operation) + 300) * sizeof(char));

	sprintf(&kernel_raw[0], " #define LOCAL_SIZE_0  %d \n#define LOCAL_SIZE_1 %d \ntypedef unsigned char BYTE;\n", LOCAL_SIZE_0, LOCAL_SIZE_1);

	strcat(kernel_raw, kernel_raw_sobel_operation);

	size_t kernel_size = strlen(kernel_raw);

	size_t local_size[2];
	size_t global_sizes[N_IMG][2];

	size_t sizes[N_IMG] = {(size_t)(Width[IMG_Y] * Height[IMG_Y]), (size_t)(Width[IMG_U] * Height[IMG_U]), (size_t)(Width[IMG_V] * Height[IMG_V])};

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
	clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL);
	platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL);
	device_id = p_devices[DEVICE];
	free(p_devices);

	/* SET UP, CONTEXT, QUEUES, KERNELS, ETC */

	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	context                                    = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);

	program = clCreateProgramWithSource(context, 1, (const char **)(&kernel_raw), (const size_t *)(&kernel_size), &err);

	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel_sobel_operation = clCreateKernel(program, SOBEL_YUV_KERNEL_NAME_SOBEL_OPERATION, &err);

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
	}

	default_event = clCreateUserEvent(context, &err);

	clSetUserEventStatus(default_event, CL_COMPLETE);

	for (int i = 0; i < N_EVENTS; i++) {
		events[i] = default_event;
	}

	for (int i = 0; i < N_EVENTS_KERNEL; i++) {
		events_kernel[i] = default_event;
	}

	for (int i = 0; i < N_IMG; i++) {
		mem_input_img[i]        = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		mem_pinned_input_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizes[i] * sizeof(BYTE), NULL, &err);
		p_pinned_input_img[i]   = (BYTE *)clEnqueueMapBuffer(queues[0], mem_pinned_input_img[i], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL, &err);

		mem_output_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		cl_uint pattern   = 0;
		clEnqueueFillBuffer(queues[0], mem_output_img[i], &pattern, sizeof(cl_uint), 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL);
		mem_pinned_output_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizes[i] * sizeof(BYTE), NULL, &err);
		p_pinned_output_img[i]   = (BYTE *)clEnqueueMapBuffer(queues[0], mem_pinned_output_img[i], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL, &err);
	}

	omp_set_num_threads(2);
	#pragma omp parallel
	{
		#pragma omp master
		{
			for (int i = 0; i < N_QUEUES; i++) {
				clFlush(queues[i]);
			}

			for (int i = 0; i < N_QUEUES; i++) {
				clFinish(queues[i]);
			}

			exec_clock = omp_get_wtime();

			events[EVENT_HOST_LOAD] = clCreateUserEvent(context, &err);
			aux_event_host          = events[EVENT_HOST_LOAD];

			#pragma omp task depend(inout: sizes) firstprivate(aux_event_host)
			{
				Load_Frame(p_pinned_input_img, File_reader, sizes);
				clSetUserEventStatus(aux_event_host, CL_COMPLETE);
			}

			for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {

				for (int i = 0; i < N_IMG; i++) {
					wait_events[0] = events[EVENT_HOST_LOAD];
					wait_events[1] = events_kernel[i];
					clEnqueueWriteBuffer(queues[i], mem_input_img[i], CL_FALSE, 0, sizes[i] * sizeof(BYTE), (void *)p_pinned_input_img[i], 2, wait_events, &events[i]);
					clFlush(queues[i]);
					aux_host_wait_events[i] = events[i];

					clSetKernelArg(kernel_sobel_operation, 0, sizeof(cl_mem), &mem_input_img[i]);
					clSetKernelArg(kernel_sobel_operation, 1, sizeof(cl_mem), &mem_output_img[i]);
					clSetKernelArg(kernel_sobel_operation, 2, sizeof(cl_int), &Width[i]);
					clSetKernelArg(kernel_sobel_operation, 3, sizeof(cl_int), &Height[i]);
					clSetKernelArg(kernel_sobel_operation, 4, sizeof(cl_int), &Width[0]);
					clSetKernelArg(kernel_sobel_operation, 5, sizeof(cl_int), &Height[0]);

					clEnqueueNDRangeKernel(queues[QUEUE_KERNEL], kernel_sobel_operation, 2, NULL, global_sizes[i], local_size, 1, &events[i], &events_kernel[i]);
					clFlush(queues[QUEUE_KERNEL]);

					wait_events[0] = events[EVENT_HOST_SAVE];
					wait_events[1] = events_kernel[i];
					clEnqueueReadBuffer(queues[i], mem_output_img[i], CL_FALSE, 0, sizes[i] * sizeof(BYTE), (void *)p_pinned_output_img[i], 2, wait_events, &events[i]);
					clFlush(queues[i]);
				}

				if (Frame_num + 1 < Num_Frames) {
					events[EVENT_HOST_LOAD] = clCreateUserEvent(context, &err);
					aux_event_host          = events[EVENT_HOST_LOAD];
					host_wait_y             = aux_host_wait_events[IMG_Y];
					host_wait_u             = aux_host_wait_events[IMG_U];
					host_wait_v             = aux_host_wait_events[IMG_V];

					#pragma omp task depend(inout: sizes) firstprivate(aux_event_host, host_wait_y, host_wait_u, host_wait_v)
					{
						cl_event host_wait_list[3];
						host_wait_list[IMG_Y] = host_wait_y;
						host_wait_list[IMG_U] = host_wait_u;
						host_wait_list[IMG_V] = host_wait_v;

						clWaitForEvents(3, host_wait_list);
						clReleaseEvent(host_wait_y);
						clReleaseEvent(host_wait_u);
						clReleaseEvent(host_wait_v);

						Load_Frame(p_pinned_input_img, File_reader, sizes);
						clSetUserEventStatus(aux_event_host, CL_COMPLETE);
					}
				}

				events[EVENT_HOST_SAVE] = clCreateUserEvent(context, &err);
				aux_event_host          = events[EVENT_HOST_SAVE];
				host_wait_y             = events[IMG_Y];
				host_wait_u             = events[IMG_U];
				host_wait_v             = events[IMG_V];

				#pragma omp task depend(inout: sizes) firstprivate(aux_event_host, host_wait_y, host_wait_u, host_wait_v)
				{
					cl_event host_wait_list[3];
					host_wait_list[IMG_Y] = host_wait_y;
					host_wait_list[IMG_U] = host_wait_u;
					host_wait_list[IMG_V] = host_wait_v;
					// These calls are added to the code in order to help to synchronize the access of the event in the task with
					// the update of the event in the main thread. It solves deadk-lock problems found in some possibly non thread-safe implementations of OpenCL
					clGetEventInfo(host_wait_y, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), NULL, NULL);
					clGetEventInfo(host_wait_u, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), NULL, NULL);
					clGetEventInfo(host_wait_v, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), NULL, NULL);

					clWaitForEvents(3, host_wait_list);
					clReleaseEvent(host_wait_y);
					clReleaseEvent(host_wait_u);
					clReleaseEvent(host_wait_v);

					Save_Frame(p_pinned_output_img, File_writer, sizes);
					clSetUserEventStatus(aux_event_host, CL_COMPLETE);
				}
			}

			for (int i = 0; i < N_QUEUES; i++) {
				clFlush(queues[i]);
			}

			for (int i = 0; i < N_QUEUES; i++) {
				clFinish(queues[i]);
			}

			clWaitForEvents(1, &events[EVENT_HOST_SAVE]);

			exec_clock = omp_get_wtime() - exec_clock;
		}
	}

	/* RELEASE ZONE */

	for (int i = 0; i < N_IMG; i++) {
		clEnqueueUnmapMemObject(queues[0], mem_pinned_input_img[i], p_pinned_input_img[i], 0, NULL, NULL);
		clEnqueueUnmapMemObject(queues[0], mem_pinned_output_img[i], p_pinned_output_img[i], 0, NULL, NULL);
		clFinish(queues[0]);

		clReleaseMemObject(mem_pinned_input_img[i]);
		clReleaseMemObject(mem_input_img[i]);
		clReleaseMemObject(mem_pinned_output_img[i]);
		clReleaseMemObject(mem_output_img[i]);
	}

	clReleaseKernel(kernel_sobel_operation);
	clReleaseProgram(program);

	for (int i = 0; i < N_QUEUES; i++) {
		clReleaseCommandQueue(queues[i]);
	}

	clReleaseContext(context);

	fclose(File_reader);
	fclose(File_writer);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
