#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char BYTE;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2


double main_clock;
double exec_clock;

void Sobel_Host_Init(BYTE *Input_Img[N_IMG], BYTE *Output_Img[N_IMG], size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		Input_Img[i] = (BYTE*) malloc((size_t)(sizes[i] * sizeof(BYTE)));
		Output_Img[i] = (BYTE*) calloc((size_t)sizes[i], sizeof(BYTE));
	}
}

void Preload_Frame(BYTE *Input_Img[N_IMG], FILE *File_reader, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fread(Input_Img[i], sizeof(BYTE), sizes[i], File_reader);
	}
}

void Get_Frame(BYTE *buffer_read[N_IMG], BYTE *Input_Img[N_IMG], size_t *sizes) {
	for (int i = 0; i < N_IMG; i++)
		memcpy(Input_Img[i], buffer_read[i], sizeof(BYTE) * sizes[i]);
}

void Sobel_Operation(BYTE *Input, BYTE *Output, int Width, int Height, int *n_blocks, int threads) {
	#pragma omp parallel for num_threads(threads)
	for (int ti=0; ti < n_blocks[0]; ti++) {
		for (int tj = 0; tj < n_blocks[1]; tj++) {
			for (int i = 0; i < BLOCKSIZE_0; i++) {
				for (int j = 0; j < BLOCKSIZE_1; j++) {
					// Variable for Gradient in X and Y direction and Final one
					float Gradient_h, Gradient_v, Gradient_mod;

					// Calculating index id
					const unsigned int Col_Index = tj*BLOCKSIZE_1+j;
					const unsigned int Row_Index = ti*BLOCKSIZE_0+i;

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
						Output[Row_Index * Width + Col_Index] = ((int) Gradient_mod < 256) ? (BYTE) Gradient_mod : 255;
					}
				}
			}
		}
	}
}

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

void Put_Frame(BYTE *Output_Img[N_IMG], BYTE *buffer_write[N_IMG], size_t *sizes) {
	for (int i = 0; i < N_IMG; i++)
		memcpy(buffer_write[i], Output_Img[i], sizeof(BYTE) * sizes[i]);
}

int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	/************************************* Argument Parse *************************************/
	if (argc < 7) {
		printf("Usage: %s <width> <height> <num_frames> <input_yuv_file> <output_yuv_file> <n_threads>", argv[0]);
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
			(size_t)(Width[IMG_V] * Height[IMG_V])
	};

	char *Input_Filename = argv[4];
	char *Output_Filename = argv[5];

	int THREADS = atoi(argv[6]);

	#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n WIDTH: %d", Width[0]);
		printf("\n HEIGHT: %d", Height[0]);
		printf("\n NUM_FRAMES: %d", Num_Frames);
		printf("\n N_THREADS: %d", THREADS);
		printf("\n POLICY: Sync");
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	int Frame_num = 0;

	FILE *File_reader, *File_writer;

	BYTE *Host_Input_Img[N_IMG];
	BYTE *Host_Output_Img[N_IMG];

	BYTE ***buffer_read;
	BYTE ***buffer_write;

	buffer_read = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);
	buffer_write = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);

	for (int i = 0; i < Num_Frames; i++) {
		buffer_read[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);
		buffer_write[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);
		for (int j = 0; j < N_IMG; j++) {
			buffer_read[i][j] = (BYTE *)malloc(sizeof(BYTE) * sizes[j]);
			buffer_write[i][j] = (BYTE *)malloc(sizeof(BYTE) * sizes[j]);
		}
	}

	int n_blocks[N_IMG][2];
	
	for (int i = 0; i < N_IMG; i++) {
		n_blocks[i][0] = (Height[i] + BLOCKSIZE_0 - 1) / BLOCKSIZE_0;
		n_blocks[i][1] = (Width[i] + BLOCKSIZE_1 - 1) / BLOCKSIZE_1;
	}

	if (!(File_reader = fopen(Input_Filename, "rb"))) {
		printf("\nError in opening input file: %s\n", Input_Filename); fflush(stdout);
		exit(EXIT_FAILURE);
	}
	if (!(File_writer = fopen(Output_Filename, "wb+"))) {
		printf("\nError in opening output file: %s\n", Output_Filename); fflush(stdout);
		exit(EXIT_FAILURE);
	}

	Sobel_Host_Init(Host_Input_Img, Host_Output_Img, sizes);

	for (int i = 0; i < Num_Frames; i++) {
		Preload_Frame(buffer_read[i], File_reader, sizes);
	}

	exec_clock = omp_get_wtime();

	Get_Frame(buffer_read[0], Host_Input_Img, sizes);

	for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
		for (int i = 0; i < N_IMG; i++) {
			Sobel_Operation(Host_Input_Img[i], Host_Output_Img[i], Width[i], Height[i], n_blocks[i], THREADS);
		}

		if (Frame_num + 1 < Num_Frames) {
			Get_Frame(buffer_read[Frame_num + 1], Host_Input_Img, sizes);
		}

		Put_Frame(Host_Output_Img, buffer_write[Frame_num], sizes);
	}

	exec_clock = omp_get_wtime() - exec_clock;

	for (int i = 0; i < Num_Frames; i++) {
		Save_Frame(buffer_write[i], File_writer, sizes);
	}

	for (int i = 0; i < N_IMG; i++) {
		free(Host_Input_Img[i]);
		free(Host_Output_Img[i]);
	}
	
	fclose(File_reader);
	fclose(File_writer);
	
	for (int i = 0; i < Num_Frames; i++) {
		for (int j = 0; j < N_IMG; j++) {
			free(buffer_read[i][j]);
			free(buffer_write[i][j]);
		}
		free(buffer_read[i]);
		free(buffer_write[i]);
	}
	free(buffer_read);
	free(buffer_write);

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

	return EXIT_SUCCESS;
}
