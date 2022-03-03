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
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_NOT_FOUND -1
#define ERR_READ      -2
#define ERR_FRAME     -3

#define _STRINGIFY(x) #x
#define STRINGIFY(x)  _STRINGIFY(x)

#define SEED 6834723

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK_
#include <assert.h>

#ifdef _CTRL_EXAMPLES_OPENCL_GPU_DEBUG_
#define OPENCL_ASSERT_OP(operation)                       \
	err = operation;                                      \
	printf("error %s@%d: %d\n", __FILE__, __LINE__, err); \
	assert(err == CL_SUCCESS);

#define OPENCL_ASSERT_ERROR(err)                          \
	printf("error %s@%d: %d\n", __FILE__, __LINE__, err); \
	assert(err == CL_SUCCESS)

#else
#define OPENCL_ASSERT_OP(operation) assert(operation == CL_SUCCESS);
#define OPENCL_ASSERT_ERROR(err)    assert(err == CL_SUCCESS)
#endif
#else
#define OPENCL_ASSERT_OP(operation) operation
#define OPENCL_ASSERT_ERROR(err)
#endif

#define THRESH_HOLD_Y 20
#define THRESH_HOLD_U 1
#define THRESH_HOLD_V 1

typedef unsigned char BYTE;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2

#define SOBEL_YUV_KERNEL_NAME_SOBEL_OPERATION "Sobel_Operation"

double main_clock;
double exec_clock;

void Load_Frame(BYTE *Input_Img[N_IMG], FILE *File_reader, size_t sizes[N_IMG]) {
	// size_t read_size;
	for (int i = 0; i < N_IMG; i++) {
		if (!(fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader))) {
			printf("Cannot read frame\n");
			exit(ERR_FRAME);
		}
	}
}

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

void Put_Frame(BYTE *Output_Img[N_IMG], BYTE *buffer_write[N_IMG], size_t *sizes) {
	for (int i = 0; i < N_IMG; i++) {
		memcpy(buffer_write[i], Output_Img[i], sizeof(BYTE) * sizes[i]);
	}
}
int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/* ARGUMENT PARSE */
	if (argc < 9) {
		printf(
			"Usage: %s <width> <height> <num_frames> <input_yuv_file> "
			"<output_yuv_file> <device> <platform> <exec_mode>",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	int Width[N_IMG];
	Width[IMG_Y] = atoi(argv[1]);
	Width[IMG_U] = Width[IMG_V] = Width[IMG_Y] / 2;

	int Height[N_IMG];
	Height[IMG_Y] = atoi(argv[2]);
	Height[IMG_U] = Height[IMG_V] = Height[IMG_Y] / 2;

	int Num_Frames = atoi(argv[3]);

	size_t sizes[N_IMG] = {
		(size_t)(Width[IMG_Y] * Height[IMG_Y]),
		(size_t)(Width[IMG_U] * Height[IMG_U]),
		(size_t)(Width[IMG_V] * Height[IMG_V])};

	char *Input_Filename  = argv[4];
	char *Output_Filename = argv[5];

	int DEVICE    = atoi(argv[6]);
	int PLATFORM  = atoi(argv[7]);
	int EXEC_MODE = atoi(argv[8]);

	/* VARIABLES */

	int Frame_num = 0; // loop variable

	// File pointer for reading and writting
	FILE *File_reader, *File_writer;

	cl_int err;

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
	BYTE  *p_pinned_input_img[N_IMG];

	cl_mem mem_output_img[N_IMG];
	BYTE  *p_pinned_output_img[N_IMG];

	BYTE ***buffer_write;

	buffer_write = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);

	for (int i = 0; i < Num_Frames; i++) {
		buffer_write[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);
		for (int j = 0; j < N_IMG; j++) {
			buffer_write[i][j] = (BYTE *)malloc(sizes[j] * sizeof(BYTE));
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
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, DEVICE + 1, p_devices, NULL);
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

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, %s, ", device_name, platform_name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n WIDTH: %d", Width[0]);
	printf("\n HEIGHT: %d", Height[0]);
	printf("\n NUM_FRAMES: %d", Num_Frames);
	printf("\n DEVICE: %s", device_name);
	printf("\n PLATFORM: %s", platform_name);
	printf("\n EXEC_MODE: %d", EXEC_MODE);
	printf("\n POLICY SYNC");
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	free(platform_name);
	free(device_name);

	/* SET UP, CONTEXTO, COLAS, KERNELS, ETC */

	cl_context_properties context_properties[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platform_id,
		0};
	context = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	FILE *binary_file;
	char *kernel_path = (char *)malloc(500 * sizeof(char));
	kernel_path[0]    = '\0';
	strcat(kernel_path, STRINGIFY(REF_KERNEL_PATH));
	strcat(kernel_path, SOBEL_YUV_KERNEL_NAME_SOBEL_OPERATION);
	strcat(kernel_path, "/");
	strcat(kernel_path, SOBEL_YUV_KERNEL_NAME_SOBEL_OPERATION);
	if (EXEC_MODE == FPGA_PROFILING)
		strcat(kernel_path, "_profiling");
	else if (EXEC_MODE == FPGA_EMULATION)
		strcat(kernel_path, "_emu");
	strcat(kernel_path, "_Ref.aocx");
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
	queue      = clCreateCommandQueue(context, device_id, properties, &err);
	OPENCL_ASSERT_ERROR(err);

	for (int i = 0; i < N_IMG; i++) {
		mem_input_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		OPENCL_ASSERT_ERROR(err);
		posix_memalign((void **)&p_pinned_input_img[i], AOCL_ALIGNMENT, sizes[i] * sizeof(BYTE));

		mem_output_img[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizes[i] * sizeof(BYTE), NULL, &err);
		OPENCL_ASSERT_ERROR(err);
		cl_uint pattern = 0;
		OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, mem_output_img[i], &pattern, sizeof(cl_uint), 0, sizes[i] * sizeof(BYTE), 0, NULL, NULL));
		posix_memalign((void **)&p_pinned_output_img[i], AOCL_ALIGNMENT, sizes[i] * sizeof(BYTE));
	}

	cl_event aux;

	OPENCL_ASSERT_OP(clFinish(queue));

	cl_int iterations[N_IMG];
	iterations[0] = Width[0] * Height[0];
	iterations[1] = Width[1] * Height[1];
	iterations[2] = Width[2] * Height[2];

	exec_clock = omp_get_wtime();

	Load_Frame(p_pinned_input_img, File_reader, sizes);

	for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
		for (int i = 0; i < N_IMG; i++) {
			OPENCL_ASSERT_OP(clEnqueueWriteBuffer(queue, mem_input_img[i], CL_FALSE,
												  0, sizes[i] * sizeof(BYTE), (void *)p_pinned_input_img[i],
												  0, NULL, &aux));
			OPENCL_ASSERT_OP(clFlush(queue));
			OPENCL_ASSERT_OP(clWaitForEvents(1, &aux));

			err = clSetKernelArg(kernel_sobel_operation, 0, sizeof(cl_mem), &mem_input_img[i]);
			err |= clSetKernelArg(kernel_sobel_operation, 1, sizeof(cl_mem), &mem_output_img[i]);
			err |= clSetKernelArg(kernel_sobel_operation, 2, sizeof(cl_int), &iterations[i]);
			OPENCL_ASSERT_ERROR(err);

			OPENCL_ASSERT_OP(clEnqueueTask(queue, kernel_sobel_operation, 0, NULL, NULL));
			OPENCL_ASSERT_OP(clFlush(queue));

			OPENCL_ASSERT_OP(clEnqueueReadBuffer(queue, mem_output_img[i], CL_FALSE,
												 0, sizes[i] * sizeof(BYTE), (void *)p_pinned_output_img[i],
												 0, NULL, &aux));
			OPENCL_ASSERT_OP(clFlush(queue));
			OPENCL_ASSERT_OP(clWaitForEvents(1, &aux));
		}

		if (Frame_num + 1 < Num_Frames) {
			Load_Frame(p_pinned_input_img, File_reader, sizes);
		}
		Put_Frame(p_pinned_output_img, buffer_write[Frame_num], sizes);
	}

	OPENCL_ASSERT_OP(clFinish(queue));
	exec_clock = omp_get_wtime() - exec_clock;

	for (int i = 0; i < Num_Frames; i++) {
		Save_Frame(buffer_write[i], File_writer, sizes);
	}

	/* RELEASE ZONE */

	for (int i = 0; i < N_IMG; i++) {
		OPENCL_ASSERT_OP(clReleaseMemObject(mem_input_img[i]));
		OPENCL_ASSERT_OP(clReleaseMemObject(mem_output_img[i]));
	}

	for (int i = 0; i < Num_Frames; i++) {
		for (int j = 0; j < N_IMG; j++) {
			free(buffer_write[i][j]);
		}
		free(buffer_write[i]);
	}
	free(buffer_write);

	OPENCL_ASSERT_OP(clReleaseKernel(kernel_sobel_operation));
	OPENCL_ASSERT_OP(clReleaseProgram(program));

	OPENCL_ASSERT_OP(clReleaseCommandQueue(queue));
	OPENCL_ASSERT_OP(clReleaseContext(context));

	fclose(File_reader);
	fclose(File_writer);

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
	return 0;
}
