#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2

#define DEVICE 0

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

void Load_Frame(BYTE *Input_Img[N_IMG], FILE *File_reader, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader);
	}
}

__global__ void Sobel_Operation(BYTE *Input, BYTE *Output, int Width, int Height) {
	float Gradient_h, Gradient_v, Gradient_mod;

	// Calculating index id
	const unsigned int Col_Index = blockDim.x * blockIdx.x + threadIdx.x;
	const unsigned int Row_Index = blockDim.y * blockIdx.y + threadIdx.y;

	if ((Row_Index != 0) && (Col_Index != 0) && (Row_Index < Height - 1) && (Col_Index < Width - 1)) {
		Gradient_v = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] + Input[(Row_Index - 1) * Width + (Col_Index + 1)] - 2 * Input[Row_Index * Width + (Col_Index - 1)] + 2 * Input[Row_Index * Width + (Col_Index + 1)] - Input[(Row_Index + 1) * Width + (Col_Index - 1)] + Input[(Row_Index + 1) * Width + (Col_Index + 1)]);

		Gradient_h = -(-Input[(Row_Index - 1) * Width + (Col_Index - 1)] - 2 * Input[(Row_Index - 1) * Width + Col_Index] - Input[(Row_Index - 1) * Width + (Col_Index + 1)] + Input[(Row_Index + 1) * Width + (Col_Index - 1)] + 2 * Input[(Row_Index + 1) * Width + Col_Index] + Input[(Row_Index + 1) * Width + (Col_Index + 1)]);

		Gradient_mod = sqrt(Gradient_h * Gradient_h + Gradient_v * Gradient_v);

		Output[Row_Index * Width + Col_Index] = ((int)Gradient_mod < 256) ? (BYTE)Gradient_mod : 255;
	}
}

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/*********************************** Argument Parse *************************************/
	if (argc < 6) {
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

	int Frame_num = 0;

	FILE *File_reader, *File_writer;

	BYTE *Host_Input_Img[N_IMG];
	BYTE *Host_Output_Img[N_IMG];

	BYTE *Device_Input_Img[N_IMG];
	BYTE *Device_Output_Img[N_IMG];

	dim3 dimBlock[N_IMG];
	dim3 dimGrid[N_IMG];

	for (int i = 0; i < N_IMG; i++) {
		dimBlock[i] = dim3(BLOCKSIZE_0, BLOCKSIZE_1);
		dimGrid[i]  = dim3((Width[i] + BLOCKSIZE_0 - 1) / BLOCKSIZE_0, (Height[i] + BLOCKSIZE_1 - 1) / BLOCKSIZE_1);
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

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime();

	Load_Frame(Host_Input_Img, File_reader, sizes);

	for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
		for (int i = 0; i < N_IMG; i++) {
			cudaMemcpy(Device_Input_Img[i], Host_Input_Img[i], (size_t)(sizes[i] * sizeof(BYTE)), cudaMemcpyHostToDevice);
			Sobel_Operation<<<dimGrid[i], dimBlock[i]>>>(Device_Input_Img[i], Device_Output_Img[i], Width[i], Height[i]);
			cudaMemcpy(Host_Output_Img[i], Device_Output_Img[i], (size_t)(sizes[i] * sizeof(BYTE)), cudaMemcpyDeviceToHost);
		}

		if (Frame_num + 1 < Num_Frames) {
			Load_Frame(Host_Input_Img, File_reader, sizes);
		}

		Save_Frame(Host_Output_Img, File_writer, sizes);
	}

	cudaDeviceSynchronize();
	exec_clock = omp_get_wtime() - exec_clock;

	for (int i = 0; i < N_IMG; i++) {
		cudaFreeHost(Host_Input_Img[i]);
		cudaFreeHost(Host_Output_Img[i]);

		cudaFree(Device_Input_Img[i]);
		cudaFree(Device_Output_Img[i]);
	}

	fclose(File_reader);
	fclose(File_writer);

	main_clock = omp_get_wtime() - main_clock;

	printf("%lf, %lf\n", main_clock, exec_clock);
	return EXIT_SUCCESS;
}
