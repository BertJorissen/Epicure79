#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

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
	BYTE  **Data;
	FILE   *File;
	size_t *sizes;
} hostFuncData_t;

double main_clock;
double exec_clock;

void Sobel_Host_Init(BYTE *Input_Img[N_IMG], BYTE *Output_Img[N_IMG], size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		cudaMallocHost((void **)(&Input_Img[i]), (size_t)(sizes[i] * sizeof(BYTE)));
		cudaMallocHost((void **)(&Output_Img[i]), (size_t)(sizes[i] * sizeof(BYTE)));
	}
}

void Sobel_Device_Init(BYTE *Input_Img[N_IMG], BYTE *Output_Img[N_IMG], size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		cudaMalloc((void **)(&Input_Img[i]), (size_t)(sizes[i] * sizeof(BYTE)));
		cudaMalloc((void **)(&Output_Img[i]), (size_t)(sizes[i] * sizeof(BYTE)));
		cudaMemset(Output_Img[i], 0, (size_t)(sizes[i] * sizeof(BYTE)));
	}
}

void Load_Frame(void *data) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host load frame");
	#endif //_PROFILING_ENABLED_

	hostFuncData_t *tmp = (hostFuncData_t *)data;

	BYTE  **Input_Img   = tmp->Data;
	FILE   *File_reader = tmp->File;
	size_t *sizes       = tmp->sizes;

	for (int i = 0; i < N_IMG; i++) {
		fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader);
	}

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

void Save_Frame(void *data) {
	#ifdef _PROFILING_ENABLED_
	nvtxRangePushA("Host save frame");
	#endif //_PROFILING_ENABLED_

	hostFuncData_t *tmp = (hostFuncData_t *)data;

	BYTE  **Output_Img  = tmp->Data;
	FILE   *File_writer = tmp->File;
	size_t *sizes       = tmp->sizes;

	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}

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

	size_t sizes[N_IMG] = {(size_t)(Width[IMG_Y] * Height[IMG_Y]), (size_t)(Width[IMG_U] * Height[IMG_U]), (size_t)(Width[IMG_V] * Height[IMG_V])};

	char *Input_Filename  = argv[4];
	char *Output_Filename = argv[5];

	int DEVICE = atoi(argv[6]);

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

	cudaStream_t streams[N_STREAMS];
	cudaEvent_t  events[N_EVENTS];

	for (int i = 0; i < N_STREAMS; i++) {
		cudaStreamCreate(&(streams[i]));
	}
	for (int i = 0; i < N_EVENTS; i++) {
		cudaEventCreateWithFlags(&(events[i]), cudaEventDisableTiming);
	}

	hostFuncData_t *Load_Frame_data = (hostFuncData_t *)malloc(sizeof(hostFuncData_t) * Num_Frames);
	hostFuncData_t *Save_Frame_data = (hostFuncData_t *)malloc(sizeof(hostFuncData_t) * Num_Frames);

	dim3 dimBlock[N_IMG];
	dim3 dimGrid[N_IMG];

	for (int i = 0; i < N_IMG; i++) {
		dimBlock[i] = dim3(BLOCKSIZE_0, BLOCKSIZE_1);
		dimGrid[i]  = dim3((Width[i] + BLOCKSIZE_0 - 1) / BLOCKSIZE_0,
						   (Height[i] + BLOCKSIZE_1 - 1) / BLOCKSIZE_1);
	}

	if (!(File_reader = fopen(Input_Filename, "rb"))) {
		printf("\nError in opening input file: %s\n", Input_Filename);
		printf("Reading file error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (!(File_writer = fopen(Output_Filename, "wb+"))) {
		printf("\nError in opening output file: %s\n", Output_Filename);
		exit(EXIT_FAILURE);
	}

	Sobel_Host_Init(Host_Input_Img, Host_Output_Img, sizes);
	Sobel_Device_Init(Device_Input_Img, Device_Output_Img, sizes);

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	Load_Frame_data[0] = (hostFuncData_t){.Data  = Host_Input_Img,
										  .File  = File_reader,
										  .sizes = sizes};

	cudaLaunchHostFunc(streams[STREAM_HOST], Load_Frame, (void *)(&Load_Frame_data[0]));
	cudaEventRecord(events[EVENT_HOST_LOAD], streams[STREAM_HOST]);

	for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
		cudaStreamWaitEvent(streams[STREAM_HTD], events[EVENT_HOST_LOAD], 0);
		cudaStreamWaitEvent(streams[STREAM_HTD], events[EVENT_KERNEL], 0);
		cudaStreamWaitEvent(streams[STREAM_KERNEL], events[EVENT_DTH], 0);
		cudaStreamWaitEvent(streams[STREAM_DTH], events[EVENT_HOST_SAVE], 0);
		for (int i = 0; i < N_IMG; i++) {
			cudaMemcpyAsync(Device_Input_Img[i], Host_Input_Img[i], (size_t)(sizes[i] * sizeof(BYTE)), cudaMemcpyHostToDevice, streams[STREAM_HTD]);
			cudaEventRecord(events[EVENT_HTD], streams[STREAM_HTD]);
			cudaStreamWaitEvent(streams[STREAM_KERNEL], events[EVENT_HTD], 0);
			Sobel_Operation<<<dimGrid[i], dimBlock[i], 0, streams[STREAM_KERNEL]>>>(Device_Input_Img[i], Device_Output_Img[i], Width[i], Height[i]);
			cudaEventRecord(events[EVENT_KERNEL], streams[STREAM_KERNEL]);
			cudaStreamWaitEvent(streams[STREAM_DTH], events[EVENT_KERNEL], 0);
			cudaMemcpyAsync(Host_Output_Img[i], Device_Output_Img[i], (size_t)(sizes[i] * sizeof(BYTE)), cudaMemcpyDeviceToHost, streams[STREAM_DTH]);
		}
		cudaEventRecord(events[EVENT_DTH], streams[STREAM_DTH]);

		if (Frame_num + 1 < Num_Frames) {
			Load_Frame_data[Frame_num + 1] = (hostFuncData_t){.Data = Host_Input_Img, .File = File_reader, .sizes = sizes};
			cudaStreamWaitEvent(streams[STREAM_HOST], events[EVENT_HTD], 0);
			cudaLaunchHostFunc(streams[STREAM_HOST], Load_Frame, (void *)(&Load_Frame_data[Frame_num + 1]));
			cudaEventRecord(events[EVENT_HOST_LOAD], streams[STREAM_HOST]);
		}

		Save_Frame_data[Frame_num] = (hostFuncData_t){.Data = Host_Output_Img, .File = File_writer, .sizes = sizes};
		cudaStreamWaitEvent(streams[STREAM_HOST], events[EVENT_DTH], 0);
		cudaLaunchHostFunc(streams[STREAM_HOST], Save_Frame, (void *)(&Save_Frame_data[Frame_num]));
		cudaEventRecord(events[EVENT_HOST_SAVE], streams[STREAM_HOST]);
	}

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	for (int i = 0; i < N_IMG; i++) {
		cudaFreeHost(Host_Input_Img[i]);
		cudaFreeHost(Host_Output_Img[i]);

		cudaFree(Device_Input_Img[i]);
		cudaFree(Device_Output_Img[i]);
	}

	free(Load_Frame_data);
	free(Save_Frame_data);

	fclose(File_reader);
	fclose(File_writer);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
