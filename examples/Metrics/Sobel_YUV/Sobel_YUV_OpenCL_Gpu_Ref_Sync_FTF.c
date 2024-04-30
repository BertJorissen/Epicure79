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

	size_t sizes[N_IMG] = {(size_t)(Width[IMG_Y] * Height[IMG_Y]), (size_t)(Width[IMG_U] * Height[IMG_U]), (size_t)(Width[IMG_V] * Height[IMG_V])};

	char *Input_Filename  = argv[4];
	char *Output_Filename = argv[5];

	int PLATFORM = atoi(argv[6]);

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

	cl_platform_id platform_id;
	cl_device_id   device_id;

	cl_context context;
	cl_program program;
	cl_kernel  kernel_sobel_operation;

	cl_command_queue_properties properties;
	cl_command_queue            queue;

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

	size_t platform_name_size;
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size);
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL);

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
	queue      = clCreateCommandQueue(context, device_id, properties, &err);

	for (int i = 0; i < N_IMG; i++) {
		mem_input_img[i]        = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		mem_pinned_input_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizes[i] * sizeof(BYTE), NULL, &err);
		p_pinned_input_img[i]   = (BYTE *)clEnqueueMapBuffer(queue, mem_pinned_input_img[i], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL, &err);
		mem_output_img[i]       = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		cl_uint pattern         = 0;
		clEnqueueFillBuffer(queue, mem_output_img[i], &pattern, sizeof(cl_uint), 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL);
		mem_pinned_output_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizes[i] * sizeof(BYTE), NULL, &err);
		p_pinned_output_img[i]   = (BYTE *)clEnqueueMapBuffer(queue, mem_pinned_output_img[i], CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL, &err);
	}

	cl_event aux;

	clFinish(queue);
	exec_clock = omp_get_wtime();

	Load_Frame(p_pinned_input_img, File_reader, sizes);

	for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
		for (int i = 0; i < N_IMG; i++) {
			clEnqueueWriteBuffer(queue, mem_input_img[i], CL_FALSE, 0, sizes[i] * sizeof(BYTE), (void *)p_pinned_input_img[i], 0, NULL, &aux);
			clFlush(queue);
			clWaitForEvents(1, &aux);

			clSetKernelArg(kernel_sobel_operation, 0, sizeof(cl_mem), &mem_input_img[i]);
			clSetKernelArg(kernel_sobel_operation, 1, sizeof(cl_mem), &mem_output_img[i]);
			clSetKernelArg(kernel_sobel_operation, 2, sizeof(cl_int), &Width[i]);
			clSetKernelArg(kernel_sobel_operation, 3, sizeof(cl_int), &Height[i]);
			clSetKernelArg(kernel_sobel_operation, 4, sizeof(cl_int), &Width[0]);
			clSetKernelArg(kernel_sobel_operation, 5, sizeof(cl_int), &Height[0]);

			clEnqueueNDRangeKernel(queue, kernel_sobel_operation, 2, NULL, global_sizes[i], local_size, 0, NULL, NULL);
			clFlush(queue);

			clEnqueueReadBuffer(queue, mem_output_img[i], CL_FALSE, 0, sizes[i] * sizeof(BYTE), (void *)p_pinned_output_img[i], 0, NULL, &aux);
			clFlush(queue);
			clWaitForEvents(1, &aux);
		}

		if (Frame_num + 1 < Num_Frames) {
			Load_Frame(p_pinned_input_img, File_reader, sizes);
		}

		Save_Frame(p_pinned_output_img, File_writer, sizes);
	}

	clFinish(queue);
	exec_clock = omp_get_wtime() - exec_clock;

	/* RELEASE ZONE */

	for (int i = 0; i < N_IMG; i++) {
		clEnqueueUnmapMemObject(queue, mem_pinned_input_img[i], p_pinned_input_img[i], 0, NULL, NULL);
		clEnqueueUnmapMemObject(queue, mem_pinned_output_img[i], p_pinned_output_img[i], 0, NULL, NULL);
		clFinish(queue);

		clReleaseMemObject(mem_pinned_input_img[i]);
		clReleaseMemObject(mem_input_img[i]);
		clReleaseMemObject(mem_pinned_output_img[i]);
		clReleaseMemObject(mem_output_img[i]);
	}

	clReleaseKernel(kernel_sobel_operation);
	clReleaseProgram(program);

	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	fclose(File_reader);
	fclose(File_writer);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
