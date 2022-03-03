#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"

#ifdef _PROFILING_ENABLED_
#include <roctx.h>
#endif //_PROFILING_ENABLED_

#define SEED 6834723

/*Tiempos*/
double main_clock;
double exec_clock;

typedef unsigned char BYTE;

#define N_IMG 3
#define IMG_Y 0
#define IMG_U 1
#define IMG_V 2

/*Tipo de datos*/
Ctrl_NewType(BYTE);

CTRL_KERNEL_CHAR(Sobel_Operation, MANUAL, LOCAL_SIZE_0, LOCAL_SIZE_1);

CTRL_KERNEL(Sobel_Operation, GENERIC, DEFAULT, KHitTile_BYTE Output, KHitTile_BYTE Input, {
	// Variable for Gradient in X and Y direction and Final one
	float Gradient_h;
	float Gradient_v;
	float Gradient_mod;

	// Calculating index id
	unsigned int Col_Index = thread_id_y;
	unsigned int Row_Index = thread_id_x;
	if ((Row_Index != 0) && (Col_Index != 0) && (Row_Index < hit_tileDimCard(Input, 0) - 1) && (Col_Index < hit_tileDimCard(Input, 1) - 1)) {
		Gradient_v =
			-(-hit(Input, (Row_Index - 1), (Col_Index - 1)) +
			  hit(Input, (Row_Index - 1), (Col_Index + 1)) -
			  2 * hit(Input, Row_Index, (Col_Index - 1)) +
			  2 * hit(Input, Row_Index, (Col_Index + 1)) -
			  hit(Input, (Row_Index + 1), (Col_Index - 1)) +
			  hit(Input, (Row_Index + 1), (Col_Index + 1)));

		Gradient_h =
			-(-hit(Input, (Row_Index - 1), (Col_Index - 1)) -
			  2 * hit(Input, (Row_Index - 1), Col_Index) -
			  hit(Input, (Row_Index - 1), (Col_Index + 1)) +
			  hit(Input, (Row_Index + 1), (Col_Index - 1)) +
			  2 * hit(Input, (Row_Index + 1), Col_Index) +
			  hit(Input, (Row_Index + 1), (Col_Index + 1)));

		Gradient_mod = sqrt(Gradient_h * Gradient_h + Gradient_v * Gradient_v);

		hit(Output, Row_Index, Col_Index) = ((int)Gradient_mod < 256) ? (BYTE)Gradient_mod : 255;
	}
});

CTRL_KERNEL_PROTO(Sobel_Operation, 1, GENERIC, DEFAULT, 2,
				  OUT, HitTile_BYTE, Output,
				  IN, HitTile_BYTE, Input);

CTRL_HOST_TASK(Load_Frame, HitTile_BYTE Image_Y, HitTile_BYTE Image_U, HitTile_BYTE Image_V, FILE *File_reader) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Host load frame");
	#endif //_PROFILING_ENABLED_

	fread(&(hit(Image_Y, 0)), sizeof(BYTE), hit_tileCard(Image_Y), File_reader);
	fread(&(hit(Image_U, 0)), sizeof(BYTE), hit_tileCard(Image_U), File_reader);
	fread(&(hit(Image_V, 0)), sizeof(BYTE), hit_tileCard(Image_V), File_reader);

	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

CTRL_HOST_TASK_PROTO(Load_Frame, 4,
					 OUT, HitTile_BYTE, Image_Y,
					 OUT, HitTile_BYTE, Image_U,
					 OUT, HitTile_BYTE, Image_V,
					 INVAL, FILE *, File_reader);

CTRL_HOST_TASK(Save_Frame, HitTile_BYTE Image_Y, HitTile_BYTE Image_U, HitTile_BYTE Image_V, FILE *File_writer) {
	#ifdef _PROFILING_ENABLED_
	roctxRangePush("Host save frame");
	#endif //_PROFILING_ENABLED_

	fwrite(&(hit(Image_Y, 0)), sizeof(BYTE), hit_tileCard(Image_Y), File_writer);
	fwrite(&(hit(Image_U, 0)), sizeof(BYTE), hit_tileCard(Image_U), File_writer);
	fwrite(&(hit(Image_V, 0)), sizeof(BYTE), hit_tileCard(Image_V), File_writer);

	#ifdef _PROFILING_ENABLED_
	roctxRangePop();
	#endif //_PROFILING_ENABLED_
}

CTRL_HOST_TASK_PROTO(Save_Frame, 4,
					 IN, HitTile_BYTE, Image_Y,
					 IN, HitTile_BYTE, Image_U,
					 IN, HitTile_BYTE, Image_V,
					 INVAL, FILE *, File_writer);

CTRL_HOST_TASK(Memset, HitTile_BYTE Image_Y, HitTile_BYTE Image_U, HitTile_BYTE Image_V) {
	memset(&(hit(Image_Y, 0)), 0, hit_tileCard(Image_Y));
	memset(&(hit(Image_U, 0)), 0, hit_tileCard(Image_U));
	memset(&(hit(Image_V, 0)), 0, hit_tileCard(Image_V));
}

CTRL_HOST_TASK_PROTO(Memset, 3,
					 OUT, HitTile_BYTE, Image_Y,
					 OUT, HitTile_BYTE, Image_U,
					 OUT, HitTile_BYTE, Image_V);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 10) {
		printf("Usage: %s <width> <height> <num_frames> <input_yuv_file> "
			   "<output_yuv_file> <device> <platform> <policy> <affinity>\n",
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
	int         DEVICE          = atoi(argv[6]);
	int         PLATFORM        = atoi(argv[7]);
	Ctrl_Policy policy          = atoi(argv[8]);
	int         host_aff        = atoi(argv[9]);
	Ctrl_SetHostAffinity(host_aff);

	int Frame_num = 0; // loop variable

	size_t *sizes = (size_t *)malloc(sizeof(size_t) * N_IMG);
	sizes[IMG_Y]  = (size_t)(Width[IMG_Y] * Height[IMG_Y]);
	sizes[IMG_U]  = (size_t)(Width[IMG_U] * Height[IMG_U]);
	sizes[IMG_V]  = (size_t)(Width[IMG_V] * Height[IMG_V]);

	// File pointer for reading and writting
	FILE *File_writer, *File_reader;

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL));
	cl_platform_id platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	size_t platform_name_size;
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size));
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL));

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICE + 1, p_devices, NULL));
	cl_device_id device_id = p_devices[DEVICE];
	free(p_devices);

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
	printf("\n POLICY %s", policy ? "Async" : "Sync");
	printf("\n DEVICE: %s", device_name);
	printf("\n PLATFORM: %s", platform_name);
	printf("\n HOST AFFINITY: %d", host_aff);
	#ifdef _CTRL_QUEUE_
	printf("\n QUEUES: ON");
	#else
	printf("\n QUEUES: OFF");
	#endif // _CTRL_QUEUE_
	printf("\n\n ---------------------------------------------------- \n");
	fflush(stdout);
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	free(platform_name);
	free(device_name);

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

	#ifdef _CTRL_QUEUE_
	__ctrl_block__(1, 1)
		#else
		__ctrl_block__(1, 0)
	#endif //_CTRL_QUEUE_
	{
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_OPENCL_GPU, policy, DEVICE, PLATFORM);

		HitTile_BYTE Input_Img[N_IMG];
		Input_Img[IMG_Y] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_Y], Width[IMG_Y]), CTRL_MEM_PINNED);
		Input_Img[IMG_U] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_U], Width[IMG_U]), CTRL_MEM_PINNED);
		Input_Img[IMG_V] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_V], Width[IMG_V]), CTRL_MEM_PINNED);

		HitTile_BYTE Output_Img[N_IMG];
		Output_Img[IMG_Y] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_Y], Width[IMG_Y]), CTRL_MEM_PINNED);
		Output_Img[IMG_U] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_U], Width[IMG_U]), CTRL_MEM_PINNED);
		Output_Img[IMG_V] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_V], Width[IMG_V]), CTRL_MEM_PINNED);

		// init output to 0
		Ctrl_HostTask(ctrl, Memset, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);
		Ctrl_MoveTo(ctrl, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		Ctrl_HostTask(ctrl, Load_Frame, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], File_reader);

		for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {
			for (int i = 0; i < N_IMG; i++) {
				Ctrl_Launch(ctrl, Sobel_Operation, threads[i], CTRL_THREAD_NULL, Output_Img[i], Input_Img[i]);
			}
			if (Frame_num + 1 < Num_Frames) {
				Ctrl_HostTask(ctrl, Load_Frame, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], File_reader);
			}
			Ctrl_HostTask(ctrl, Save_Frame, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V], File_writer);
		}

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		Ctrl_Free(ctrl, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);

		Ctrl_Destroy(ctrl);
	}

	free(sizes);
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
