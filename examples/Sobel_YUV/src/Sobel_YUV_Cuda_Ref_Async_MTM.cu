/**
 * @file Sobel_YUV_Cuda_Ref_Async_MTM.cu
 * @author Trasgo Group
 * @brief SobelYUV: Asyncronous native CUDA mem to mem version
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

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _PROFILING_ENABLED_
#include "nvToolsExt.h"
#endif //_PROFILING_ENABLED_

#ifndef _CTRL_EXAMPLES_CUDA_ERROR_CHECK_
#define CUDA_CHECK()
#else
#define CUDA_CHECK()                                                         \
	{                                                                        \
		cudaError_t error;                                                   \
		if ((error = cudaGetLastError()) != cudaSuccess) {                   \
			printf("\tCUDA Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
				   cudaGetErrorName(error), cudaGetErrorString(error));      \
			fflush(stdout);                                                  \
			fflush(stderr);                                                  \
			exit(EXIT_FAILURE);                                              \
		}                                                                    \
	}
#endif

#define CUDA_CALL(func) \
	func;               \
	CUDA_CHECK()

typedef unsigned char BYTE;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2

#define N_STREAMS     4
#define STREAM_KERNEL 0
#define STREAM_HOST   1
#define STREAM_HTD    2
#define STREAM_DTH    3

#define N_EVENTS        5
#define EVENT_KERNEL    0
#define EVENT_HOST_LOAD 1
#define EVENT_HOST_SAVE 2
#define EVENT_HTD       3
#define EVENT_DTH       4

typedef struct hostFuncData {
	BYTE  **buffer;
	BYTE  **Img;
	FILE   *File;
	size_t *sizes;
} hostFuncData_t;

double main_clock;
double exec_clock;

void Sobel_Host_Init(BYTE *Input_Img[N_IMG], BYTE *Output_Img[N_IMG], size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		CUDA_CALL(cudaMallocHost((void **)(&Input_Img[i]), (size_t)(sizes[i] * sizeof(BYTE))));
		CUDA_CALL(cudaMallocHost((void **)(&Output_Img[i]), (size_t)(sizes[i] * sizeof(BYTE))));
	}
}

void Sobel_Device_Init(BYTE *Input_Img[N_IMG], BYTE *Output_Img[N_IMG], size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		CUDA_CALL(cudaMalloc((void **)(&Input_Img[i]), (size_t)(sizes[i] * sizeof(BYTE))));
		CUDA_CALL(cudaMalloc((void **)(&Output_Img[i]), (size_t)(sizes[i] * sizeof(BYTE))));
		CUDA_CALL(cudaMemset(Output_Img[i], 0, (size_t)(sizes[i] * sizeof(BYTE))));
	}
}

void Preload_Frame(BYTE *Input_Img[N_IMG], FILE *File_reader, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader);
	}
}

void Get_Frame(void *data) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host get frame");
	#endif //_PROFILING_ENABLED_

	hostFuncData_t *tmp = (hostFuncData_t *)data;

	BYTE  **Input_Img   = tmp->Img;
	BYTE  **buffer_read = tmp->buffer;
	size_t *sizes       = tmp->sizes;

	for (int i = 0; i < N_IMG; i++)
		memcpy(Input_Img[i], buffer_read[i], sizeof(BYTE) * sizes[i]);

	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

__global__ void Sobel_Operation(BYTE *Input, BYTE *Output, int Width, int Height) {

	float Gradient_h, Gradient_v, Gradient_mod;

	// Calculating index id
	const unsigned int Col_Index = blockDim.x * blockIdx.x + threadIdx.x;
	const unsigned int Row_Index = blockDim.y * blockIdx.y + threadIdx.y;

	if ((Row_Index != 0) && (Col_Index != 0) && (Row_Index < Height - 1) && (Col_Index < Width - 1)) {
		Gradient_v = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] +
					   Input[(Row_Index - 1) * Width + (Col_Index + 1)] -
					   2 * Input[Row_Index * Width + (Col_Index - 1)] +
					   2 * Input[Row_Index * Width + (Col_Index + 1)] -
					   Input[(Row_Index + 1) * Width + (Col_Index - 1)] +
					   Input[(Row_Index + 1) * Width + (Col_Index + 1)]);

		Gradient_h = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] -
					   2 * Input[(Row_Index - 1) * Width + Col_Index] -
					   Input[(Row_Index - 1) * Width + (Col_Index + 1)] +
					   Input[(Row_Index + 1) * Width + (Col_Index - 1)] +
					   2 * Input[(Row_Index + 1) * Width + Col_Index] +
					   Input[(Row_Index + 1) * Width + (Col_Index + 1)]);

		Gradient_mod = sqrt(Gradient_h * Gradient_h + Gradient_v * Gradient_v);

		Output[Row_Index * Width + Col_Index] = ((int)Gradient_mod < 256) ? (BYTE)Gradient_mod : 255;
	}
}

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

void Put_Frame(void *data) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host put frame");
	#endif //_PROFILING_ENABLED_

	hostFuncData_t *tmp = (hostFuncData_t *)data;

	BYTE  **Output_Img   = tmp->Img;
	BYTE  **buffer_write = tmp->buffer;
	size_t *sizes        = tmp->sizes;

	for (int i = 0; i < N_IMG; i++)
		memcpy(buffer_write[i], Output_Img[i], sizeof(BYTE) * sizes[i]);

	#ifdef _PROFILING_ENABLED_
	nvtxRangePop();
	#endif //_PROFILING_ENABLED_
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/*********************************** Argument Parse *************************************/
	if (argc < 7) {
		printf("Usage: %s <width> <height> <num_frames> <input_yuv_file> <output_yuv_file> <device>\n", argv[0]);
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

	int DEVICE = atoi(argv[6]);

	// Extra information for collecting results
	cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, DEVICE);
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, ", cu_dev_prop.name);
	#else
	printf("\n ----------------------- ARGS ----------------------- \n");
	printf("\n WIDTH: %d", Width[0]);
	printf("\n HEIGHT: %d", Height[0]);
	printf("\n NUM_FRAMES: %d", Num_Frames);
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n POLICY ASYNC");
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	int Frame_num = 0;

	FILE *File_reader, *File_writer;

	BYTE *Host_Input_Img[N_IMG];
	BYTE *Host_Output_Img[N_IMG];

	BYTE *Device_Input_Img[N_IMG];
	BYTE *Device_Output_Img[N_IMG];

	BYTE ***buffer_read;
	BYTE ***buffer_write;

	buffer_read  = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);
	buffer_write = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);

	for (int i = 0; i < Num_Frames; i++) {
		buffer_read[i]  = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);
		buffer_write[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);
		for (int j = 0; j < N_IMG; j++) {
			CUDA_CALL(cudaMallocHost((void **)(&(buffer_read[i][j])), sizes[j] * sizeof(BYTE)));
			CUDA_CALL(cudaMallocHost((void **)(&(buffer_write[i][j])), sizes[j] * sizeof(BYTE)));
		}
	}

	cudaStream_t streams[N_STREAMS];
	cudaEvent_t  events[N_EVENTS];

	for (int i = 0; i < N_STREAMS; i++) {
		CUDA_CALL(cudaStreamCreate(&(streams[i])));
	}
	for (int i = 0; i < N_EVENTS; i++) {
		CUDA_CALL(cudaEventCreateWithFlags(&(events[i]), cudaEventDisableTiming));
	}

	hostFuncData_t *Load_Frame_data = (hostFuncData_t *)malloc(sizeof(hostFuncData_t) * Num_Frames);
	hostFuncData_t *Save_Frame_data = (hostFuncData_t *)malloc(sizeof(hostFuncData_t) * Num_Frames);

	dim3 dimBlock[N_IMG];
	dim3 dimGrid[N_IMG];

	for (int i = 0; i < N_IMG; i++) {
		dimBlock[i] = dim3(BLOCKSIZE_0, BLOCKSIZE_1);
		dimGrid[i]  = dim3(
            (Width[i] + BLOCKSIZE_0 - 1) / BLOCKSIZE_0,
            (Height[i] + BLOCKSIZE_1 - 1) / BLOCKSIZE_1);
	}

	if (!(File_reader = fopen(Input_Filename, "rb"))) {
		printf("\nError in opening input file: %s\n", Input_Filename);
		exit(EXIT_FAILURE);
	}
	if (!(File_writer = fopen(Output_Filename, "wb+"))) {
		printf("\nError in opening output file: %s\n", Output_Filename);
		exit(EXIT_FAILURE);
	}

	Sobel_Host_Init(Host_Input_Img, Host_Output_Img, sizes);
	Sobel_Device_Init(Device_Input_Img, Device_Output_Img, sizes);

	for (int i = 0; i < Num_Frames; i++) {
		Preload_Frame(buffer_read[i], File_reader, sizes);
	}

	CUDA_CALL(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime();

	Load_Frame_data[0] = (hostFuncData_t){.buffer = buffer_read[0],
										  .Img    = Host_Input_Img,
										  .File   = NULL,
										  .sizes  = sizes};

	CUDA_CALL(cudaLaunchHostFunc(streams[STREAM_HOST], Get_Frame, (void *)(&Load_Frame_data[0])));
	CUDA_CALL(cudaEventRecord(events[EVENT_HOST_LOAD], streams[STREAM_HOST]));

	for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
		CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_HTD], events[EVENT_HOST_LOAD], 0));
		CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_HTD], events[EVENT_KERNEL], 0));
		CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_KERNEL], events[EVENT_DTH], 0));
		CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_DTH], events[EVENT_HOST_SAVE], 0));
		for (int i = 0; i < N_IMG; i++) {
			CUDA_CALL(cudaMemcpyAsync(Device_Input_Img[i], Host_Input_Img[i], (size_t)(sizes[i] * sizeof(BYTE)), cudaMemcpyHostToDevice, streams[STREAM_HTD]));
			CUDA_CALL(cudaEventRecord(events[EVENT_HTD], streams[STREAM_HTD]));
			CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_KERNEL], events[EVENT_HTD], 0));
			CUDA_CALL((Sobel_Operation<<<dimGrid[i], dimBlock[i], 0, streams[STREAM_KERNEL]>>>(Device_Input_Img[i], Device_Output_Img[i], Width[i], Height[i])));
			CUDA_CALL(cudaEventRecord(events[EVENT_KERNEL], streams[STREAM_KERNEL]));
			CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_DTH], events[EVENT_KERNEL], 0));
			CUDA_CALL(cudaMemcpyAsync(Host_Output_Img[i], Device_Output_Img[i], (size_t)(sizes[i] * sizeof(BYTE)), cudaMemcpyDeviceToHost, streams[STREAM_DTH]));
		}
		CUDA_CALL(cudaEventRecord(events[EVENT_DTH], streams[STREAM_DTH]));

		if (Frame_num + 1 < Num_Frames) {
			Load_Frame_data[Frame_num + 1] = (hostFuncData_t){.buffer = buffer_read[Frame_num + 1], .Img = Host_Input_Img, .File = NULL, .sizes = sizes};
			CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_HOST], events[EVENT_HTD], 0));
			CUDA_CALL(cudaLaunchHostFunc(streams[STREAM_HOST], Get_Frame, (void *)(&Load_Frame_data[Frame_num + 1])));
			CUDA_CALL(cudaEventRecord(events[EVENT_HOST_LOAD], streams[STREAM_HOST]));
		}

		Save_Frame_data[Frame_num] = (hostFuncData_t){.buffer = buffer_write[Frame_num], .Img = Host_Output_Img, .File = NULL, .sizes = sizes};
		CUDA_CALL(cudaStreamWaitEvent(streams[STREAM_HOST], events[EVENT_DTH], 0));
		CUDA_CALL(cudaLaunchHostFunc(streams[STREAM_HOST], Put_Frame, (void *)(&Save_Frame_data[Frame_num])));
		CUDA_CALL(cudaEventRecord(events[EVENT_HOST_SAVE], streams[STREAM_HOST]));
	}

	CUDA_CALL(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime() - exec_clock;

	for (int i = 0; i < Num_Frames; i++) {
		Save_Frame(buffer_write[i], File_writer, sizes);
	}

	for (int i = 0; i < N_IMG; i++) {
		CUDA_CALL(cudaFreeHost(Host_Input_Img[i]));
		CUDA_CALL(cudaFreeHost(Host_Output_Img[i]));

		CUDA_CALL(cudaFree(Device_Input_Img[i]));
		CUDA_CALL(cudaFree(Device_Output_Img[i]));
	}

	free(Load_Frame_data);
	free(Save_Frame_data);

	fclose(File_reader);
	fclose(File_writer);

	for (int i = 0; i < Num_Frames; i++) {
		for (int j = 0; j < N_IMG; j++) {
			CUDA_CALL(cudaFreeHost(buffer_read[i][j]));
			CUDA_CALL(cudaFreeHost(buffer_write[i][j]));
		}
		free(buffer_read[i]);
		free(buffer_write[i]);
	}
	free(buffer_read);
	free(buffer_write);

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
