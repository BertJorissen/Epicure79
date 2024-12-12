/**
 * @file Sobel_YUV_FPGA_Ctrl.c
 * @brief SobelYUV: Ctrl FPGA version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Sobel_YUV_Ctrl_ext_type.h"

#include "Ctrl.h"
#include "../../examples/Utils/ctrl_print_info.h"

#define ERR_Y -1
#define ERR_U -2
#define ERR_V -3

#define SEED 6834723

/*Tiempos*/
double main_clock;
double exec_clock;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

Ctrl_NewType(BYTE);

CTRL_KERNEL_CHAR(Sobel, MANUAL, LOCAL_SIZE_1, LOCAL_SIZE_0);

CTRL_KERNEL_PROTO(Sobel, 1, FPGA, DEFAULT, 3,
				  IN, HitTile_BYTE, frame_in,
				  OUT, HitTile_BYTE, frame_out,
				  INVAL, int, iterations);

#define load_params   4, OUT, HitTile_BYTE, Image_Y, OUT, HitTile_BYTE, Image_U, OUT, HitTile_BYTE, Image_V, INVAL, FILE *, File_reader
#define put_params    4, IN, HitTile_BYTE, Image_Y, IN, HitTile_BYTE, Image_U, IN, HitTile_BYTE, Image_V, INVAL, BYTE **, buffer_write
#define memset_params 3, OUT, HitTile_BYTE, Image_Y, OUT, HitTile_BYTE, Image_U, OUT, HitTile_BYTE, Image_V

CTRL_HOST_TASK(Load_Frame, CTRL_HPARAMS(load_params)) {
	if (!(fread(&(hit(Image_Y, 0)), sizeof(BYTE), hit_tileCard(Image_Y), File_reader))) {
		printf("Cannot read frame Y\n");
		exit(ERR_Y);
	}

	if (!(fread(&(hit(Image_U, 0)), sizeof(BYTE), hit_tileCard(Image_U), File_reader))) {
		printf("Cannot read frame U\n");
		exit(ERR_U);
	}

	if (!(fread(&(hit(Image_V, 0)), sizeof(BYTE), hit_tileCard(Image_V), File_reader))) {
		printf("Cannot read frame V\n");
		exit(ERR_V);
	}
}

CTRL_HOST_TASK(Put_Frame, CTRL_HPARAMS(put_params)) {
	memcpy(buffer_write[IMG_Y], &(hit(Image_Y, 0)), sizeof(BYTE) * hit_tileCard(Image_Y));
	memcpy(buffer_write[IMG_U], &(hit(Image_U, 0)), sizeof(BYTE) * hit_tileCard(Image_U));
	memcpy(buffer_write[IMG_V], &(hit(Image_V, 0)), sizeof(BYTE) * hit_tileCard(Image_V));
}

CTRL_HOST_TASK(Memset, CTRL_HPARAMS(memset_params)) {
	memset(&(hit(Image_Y, 0)), 0, hit_tileCard(Image_Y));
	memset(&(hit(Image_U, 0)), 0, hit_tileCard(Image_U));
	memset(&(hit(Image_V, 0)), 0, hit_tileCard(Image_V));
}

CTRL_HOST_TASK_PROTO(Load_Frame, load_params);
CTRL_HOST_TASK_PROTO(Put_Frame, put_params);
CTRL_HOST_TASK_PROTO(Memset, memset_params);

int main(int argc, char *argv[]) {
	Ctrl_Init(&argc, &argv);
	main_clock = omp_get_wtime();

	if (argc != 8) {
		printf("Usage: %s <width> <height> <num_frames> <input_yuv_file> "
			   "<output_yuv_file> <policy> <config_file>\n",
			   argv[0]);
		exit(EXIT_FAILURE);
	}

	int Width[3];
	Width[0] = atoi(argv[1]);
	Width[1] = Width[2] = Width[0] / 2;

	int Height[3];
	Height[0] = atoi(argv[2]);
	Height[1] = Height[2] = Height[0] / 2;

	int Num_Frames = atoi(argv[3]);

	char       *Input_Filename  = argv[4];
	char       *Output_Filename = argv[5];
	Ctrl_Policy policy          = atoi(argv[6]);
	Ctrl_SetPolicy(policy);
	char *ctrl_conf_file = argv[7];

	int Frame_num = 0; // loop variable

	size_t *sizes = (size_t *)malloc(sizeof(size_t) * N_IMG);
	sizes[IMG_Y]  = (size_t)(Width[IMG_Y] * Height[IMG_Y]);
	sizes[IMG_U]  = (size_t)(Width[IMG_U] * Height[IMG_U]);
	sizes[IMG_V]  = (size_t)(Width[IMG_V] * Height[IMG_V]);

	BYTE ***buffer_write;

	buffer_write = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);

	for (int i = 0; i < Num_Frames; i++) {
		buffer_write[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);

		for (int j = 0; j < N_IMG; j++) {
			buffer_write[i][j] = (BYTE *)malloc(sizes[j] * sizeof(BYTE));
		}
	}

	FILE *File_writer, *File_reader;

	Ctrl_Thread threads[N_IMG];
	for (int i = 0; i < N_IMG; i++) {
		Ctrl_ThreadInit(threads[i], Height[i], Width[i]);
	}

	if (!(File_reader = fopen(Input_Filename, "rb"))) {
		printf("\nError in opening input file: %s\n", Input_Filename);
		exit(EXIT_FAILURE);
	}
	if (!(File_writer = fopen(Output_Filename, "wb+"))) {
		printf("\nError in opening output file: %s\n", Output_Filename);
		exit(EXIT_FAILURE);
	}

	Ctrl_Thread group;
	Ctrl_ThreadInit(group, 1);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);

		// Extra information for collecting results
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n WIDTH: %d", Width[0]);
		printf("\n HEIGHT: %d", Height[0]);
		printf("\n NUM_FRAMES: %d", Num_Frames);
		printf("\n POLICY %s", policy ? "Async" : "Sync");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		Ctrl_PrintInfo();
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		HitTile_BYTE Input_Img[N_IMG];
		HitShape     sh1 = hitShapeSize(Height[IMG_Y], Width[IMG_Y]);
		HitShape     sh2 = hitShapeSize(Height[IMG_U], Width[IMG_U]);
		HitShape     sh3 = hitShapeSize(Height[IMG_V], Width[IMG_V]);

		Input_Img[IMG_Y] = Ctrl_DomainAlloc(ctrl, BYTE, sh1);
		Input_Img[IMG_U] = Ctrl_DomainAlloc(ctrl, BYTE, sh2);
		Input_Img[IMG_V] = Ctrl_DomainAlloc(ctrl, BYTE, sh3);

		HitTile_BYTE Output_Img[N_IMG];
		Output_Img[IMG_Y] = Ctrl_DomainAlloc(ctrl, BYTE, sh1);
		Output_Img[IMG_U] = Ctrl_DomainAlloc(ctrl, BYTE, sh2);
		Output_Img[IMG_V] = Ctrl_DomainAlloc(ctrl, BYTE, sh3);

		int iterations[N_IMG];
		iterations[0] = Width[0] * Height[0];
		iterations[1] = Width[1] * Height[1];
		iterations[2] = Width[2] * Height[2];

		// init output to 0
		Ctrl_HostTask(Memset, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);
		Ctrl_MoveTo(ctrl, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		Ctrl_HostTask(Load_Frame, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], File_reader);

		for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {

			for (int i = 0; i < N_IMG; i++) {
				Ctrl_Launch(ctrl, Sobel, threads[i], group, Input_Img[i], Output_Img[i], iterations[i]);
			}
			if (Frame_num + 1 < Num_Frames) {
				Ctrl_HostTask(Load_Frame, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], File_reader);
			}
			Ctrl_HostTask(Put_Frame, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V], buffer_write[Frame_num]);
		}

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		for (int i = 0; i < Num_Frames; i++) {
			Save_Frame(buffer_write[i], File_writer, sizes);
		}

		Ctrl_Free(ctrl, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);

		Ctrl_EndBlock();
	}

	free(sizes);
	fclose(File_reader);
	fclose(File_writer);

	for (int i = 0; i < Num_Frames; i++) {
		for (int j = 0; j < N_IMG; j++) {
			free(buffer_write[i][j]);
		}
		free(buffer_write[i]);
	}
	free(buffer_write);

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Finalize();
	return EXIT_SUCCESS;
}
