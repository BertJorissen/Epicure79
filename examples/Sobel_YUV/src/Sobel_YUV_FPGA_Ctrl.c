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
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"


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

typedef unsigned char BYTE;

void Save_Frame(BYTE *Output_Img[N_IMG], FILE *File_writer, size_t sizes[N_IMG]) {
	for (int i = 0; i < N_IMG; i++) {
		fwrite(Output_Img[i], sizeof(BYTE), sizes[i], File_writer);
	}
}

Ctrl_NewType(BYTE);

CTRL_KERNEL_CHAR(Sobel, MANUAL, LOCAL_SIZE_0, LOCAL_SIZE_1);

CTRL_KERNEL_PROTO(Sobel, 1, FPGA, DEFAULT, 3, 
		IN, HitTile_BYTE, frame_in, 
		OUT, HitTile_BYTE, frame_out, 
		INVAL, int, iterations
);

CTRL_HOST_TASK(Load_Frame, HitTile_BYTE Image_Y, HitTile_BYTE Image_U, HitTile_BYTE Image_V, FILE *File_reader){
	if(!(fread(&(hit(Image_Y, 0)), sizeof(BYTE), hit_tileCard(Image_Y), File_reader))) {
		printf("Cannot read frame Y\n");
		exit(ERR_Y);
	}
	
	if(!(fread(&(hit(Image_U, 0)), sizeof(BYTE), hit_tileCard(Image_U), File_reader))) {
		printf("Cannot read frame U\n");
		exit(ERR_U);
	}
	
	if(!(fread(&(hit(Image_V, 0)), sizeof(BYTE), hit_tileCard(Image_V), File_reader))) {
		printf("Cannot read frame V\n");
		exit(ERR_V);
	}
}

CTRL_HOST_TASK_PROTO(Load_Frame, 4, 
		OUT, HitTile_BYTE, Image_Y,
		OUT, HitTile_BYTE, Image_U,
		OUT, HitTile_BYTE, Image_V,
		INVAL, FILE *, File_reader
);

CTRL_HOST_TASK(Put_Frame, HitTile_BYTE Image_Y, HitTile_BYTE Image_U, HitTile_BYTE Image_V, BYTE **buffer_write){
	memcpy(buffer_write[IMG_Y], &(hit(Image_Y, 0)), sizeof(BYTE) * hit_tileCard(Image_Y));
	memcpy(buffer_write[IMG_U], &(hit(Image_U, 0)), sizeof(BYTE) * hit_tileCard(Image_U));
	memcpy(buffer_write[IMG_V], &(hit(Image_V, 0)), sizeof(BYTE) * hit_tileCard(Image_V));
}

CTRL_HOST_TASK_PROTO(Put_Frame, 4,
		IN, HitTile_BYTE, Image_Y,
		IN, HitTile_BYTE, Image_U,
		IN, HitTile_BYTE, Image_V,
		INVAL, BYTE **, buffer_write
);

CTRL_HOST_TASK(Memset, HitTile_BYTE Image_Y, HitTile_BYTE Image_U, HitTile_BYTE Image_V){
	memset(&(hit(Image_Y, 0)), 0, hit_tileCard(Image_Y));
	memset(&(hit(Image_U, 0)), 0, hit_tileCard(Image_U));
	memset(&(hit(Image_V, 0)), 0, hit_tileCard(Image_V));
}

CTRL_HOST_TASK_PROTO(Memset, 3,
		OUT, HitTile_BYTE, Image_Y,
		OUT, HitTile_BYTE, Image_U,
		OUT, HitTile_BYTE, Image_V
);

int main(int argc, char *argv[]) {
	main_clock = omp_get_wtime();

	if (argc != 11) {
		printf("Usage: %s <width> <height> <num_frames> <input_yuv_file> "
			   "<output_yuv_file> <device> <platform> <exec_mode> <policy> <affinity>\n",
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
	
	char *Input_Filename = argv[4];
	char *Output_Filename = argv[5];
	int DEVICE = atoi(argv[6]);
	int PLATFORM = atoi(argv[7]);
	int EXEC_MODE = atoi(argv[8]);
	Ctrl_Policy POLICY = atoi(argv[9]);
	int host_aff = atoi(argv[10]);
	Ctrl_SetHostAffinity(host_aff);

	int Frame_num = 0; // loop variable

	size_t *sizes = (size_t *)malloc(sizeof(size_t) * N_IMG);
	sizes[IMG_Y] = (size_t)(Width[IMG_Y] * Height[IMG_Y]);
	sizes[IMG_U] = (size_t)(Width[IMG_U] * Height[IMG_U]);
	sizes[IMG_V] = (size_t)(Width[IMG_V] * Height[IMG_V]);

	BYTE ***buffer_write;
	
	buffer_write = (BYTE ***)malloc(sizeof(BYTE **) * Num_Frames);

	for (int i = 0; i < Num_Frames; i++) {
		buffer_write[i] = (BYTE **)malloc(sizeof(BYTE *) * N_IMG);

		for (int j = 0; j < N_IMG; j++) {
			buffer_write[i][j] = (BYTE*)malloc(sizes[j] * sizeof(BYTE));
		}
	}

	// File pointer for reading and writting
	FILE *File_writer, *File_reader;

	cl_platform_id *p_platforms = (cl_platform_id *)malloc((PLATFORM + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP( clGetPlatformIDs(PLATFORM + 1, p_platforms, NULL) );
	cl_platform_id platform_id = p_platforms[PLATFORM];
	free(p_platforms);

	size_t platform_name_size;
	OPENCL_ASSERT_OP( clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size) );
	char* platform_name = (char*)malloc( sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP( clGetPlatformInfo( platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL ) );

	cl_device_id *p_devices = (cl_device_id *)malloc((DEVICE + 1) * sizeof(cl_device_id));
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, DEVICE + 1, p_devices, NULL);
	cl_device_id device_id = p_devices[DEVICE];
	free(p_devices);
	
	size_t device_name_size;
	OPENCL_ASSERT_OP( clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size) );
	char* device_name = (char*) malloc( sizeof(char) * device_name_size );
	OPENCL_ASSERT_OP( clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL) );


	#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%s, %s, ", device_name, platform_name);
	#else
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n WIDTH: %d", Width[0]);
		printf("\n HEIGHT: %d", Height[0]);
		printf("\n NUM_FRAMES: %d", Num_Frames);
		printf("\n POLICY %s", POLICY ? "Async" : "Sync");
		printf("\n DEVICE: %s", device_name);
		printf("\n PLATFORM: %s", platform_name);
		printf("\n EXEC_MODE: %d", EXEC_MODE);
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
	
	Ctrl_Thread group;
	Ctrl_ThreadInit(group, 1);
	#ifdef _CTRL_QUEUE_
		__ctrl_block__(1,1)
	#else
		__ctrl_block__(1,0)
	#endif //_CTRL_QUEUE_
	{
		PCtrl ctrl = Ctrl_Create(CTRL_TYPE_FPGA, POLICY, DEVICE, PLATFORM, EXEC_MODE);

		HitTile_BYTE Input_Img[N_IMG];
		Input_Img[IMG_Y] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_Y], Width[IMG_Y]));
		Input_Img[IMG_U] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_U], Width[IMG_U]));
		Input_Img[IMG_V] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_V], Width[IMG_V]));

		HitTile_BYTE Output_Img[N_IMG];
		Output_Img[IMG_Y] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_Y], Width[IMG_Y]));
		Output_Img[IMG_U] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_U], Width[IMG_U]));
		Output_Img[IMG_V] = Ctrl_DomainAlloc(ctrl, BYTE, hitShapeSize(Height[IMG_V], Width[IMG_V]));
		
		int iterations[N_IMG];
		iterations[0] = Width[0] * Height[0];
		iterations[1] = Width[1] * Height[1];
		iterations[2] = Width[2] * Height[2];

		// init output to 0
		Ctrl_HostTask(ctrl, Memset, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);
		Ctrl_MoveTo(ctrl, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime();

		Ctrl_HostTask(ctrl, Load_Frame, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], File_reader);
		
		for (Frame_num = 0; Frame_num < Num_Frames; Frame_num++) {

			for (int i = 0; i < N_IMG; i++) {
				Ctrl_Launch(ctrl, Sobel, threads[i], group, Input_Img[i], Output_Img[i], iterations[i]);
			}
			if (Frame_num + 1 < Num_Frames) {
				Ctrl_HostTask(ctrl, Load_Frame, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], File_reader);
			}
			Ctrl_HostTask(ctrl, Put_Frame, Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V], buffer_write[Frame_num]);
		}

		Ctrl_GlobalSync(ctrl);
		exec_clock = omp_get_wtime() - exec_clock;

		for (int i = 0; i < Num_Frames; i++) {
			Save_Frame(buffer_write[i], File_writer, sizes);
		}

		Ctrl_Free(ctrl, Input_Img[IMG_Y], Input_Img[IMG_U], Input_Img[IMG_V], Output_Img[IMG_Y], Output_Img[IMG_U], Output_Img[IMG_V]);

		Ctrl_Destroy(ctrl);
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
		fflush(stdout);
	#else
		printf("\n ----------------------- TIME ----------------------- \n\n");
		printf(" Clock main: %lf\n", main_clock);
		printf(" Clock exec: %lf\n", exec_clock);
		printf("\n ---------------------------------------------------- \n");
	#endif

	return 0;
}
