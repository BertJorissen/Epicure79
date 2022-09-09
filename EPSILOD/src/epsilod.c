/*
 * epsilod.c
 * 	Ctrl example
 * 	Stencil code: Any dimensions, stencil as a pattern of weights.
 *
 * v1.1
 * (c) 2019-2022, Arturo Gonzalez-Escribano, Yuri Torres de la Sierra, Manuel de Castro Caballero
 */

/*
 * <license>
 *
 * Hitmap v1.3
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
 * Copyright (c) 2007-2021, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * More information on http://trasgo.infor.uva.es/
 *
 * </license>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef _CTRL_ARCH_CUDA_
#include <cuda_runtime_api.h>
#endif // _CTRL_ARCH_CUDA_
#include "Ctrl.h"

//#define WRITE_RESULT
//#define _PSS_TRASGO_CLUSTER_
//#define _PSS_DEV_INFO_
//#define _PSS_USE_STATIC_LOAD_BALANCING_
//#define _PSS_USE_1D_LAYOUT_

Ctrl_NewType(float);

/* Special functions definition. */
typedef void (*stencilFunction)(PCtrl, Ctrl_Thread, Ctrl_Thread, int, HitTile_float, HitTile_float, HitTile_float, float);
typedef void (*initDataFunction)(HitTile_float, int, int[], int[]);
typedef void (*outputDataFunction)(HitTile_float, int, int[], int[]);

/* Parallel stencil skeleton launcher prototype (public API). */
void stencilComputation(int sizes[], HitShape stencilShape, float stencilData[], float factor,
						int numIterations, stencilFunction f_updateCell,
						initDataFunction f_init, outputDataFunction f_output);

/* Specific stencil kernel prototypes and wrapper launcher */
#define REGISTER_STENCIL(stencilname) REGISTER_STENCIL_WITH_ARCH(stencilname, GENERIC, DEFAULT)

#define REGISTER_STENCIL_WITH_ARCH(stencilname, ...)                                                                                                                            \
	REGISTER_STENCIL_N(stencilname, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)                                                                                        \
	CTRL_KERNEL_CHAR(stencilname, MANUAL, 64, 8, 1);                                                                                                                  \
	void stencilname(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor) { \
		Ctrl_LaunchToStream(ctrl, stencilname, threads, blockSize, stream, mat, copy);                                                                                \
	}

#define REGISTER_STENCIL_N(stencilname, n_archs_times2, ...)  REGISTER_STENCIL_N2(stencilname, n_archs_times2, __VA_ARGS__)
#define REGISTER_STENCIL_N2(stencilname, n_archs_times2, ...) STENCIL_PROTO(stencilname, n_archs_times2, n_archs_##n_archs_times2, __VA_ARGS__)
#define STENCIL_PROTO(stencilname, n_archs_times2, n_archs, ...)                       \
	CTRL_KERNEL_PROTO(stencilname,                                                     \
					  n_archs, STENCIL_EXTRACT_ARCHS_##n_archs_times2(__VA_ARGS__), 2, \
					  OUT, HitTile_float, matrix,                                      \
					  IN, HitTile_float, matrixCopy);

#define STENCIL_EXTRACT_ARCHS_2(arch, subarch)       arch, subarch
#define STENCIL_EXTRACT_ARCHS_4(arch, subarch, ...)  arch, subarch, STENCIL_EXTRACT_ARCHS_2(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_6(arch, subarch, ...)  arch, subarch, STENCIL_EXTRACT_ARCHS_4(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_8(arch, subarch, ...)  arch, subarch, STENCIL_EXTRACT_ARCHS_6(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_10(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_8(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_12(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_10(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_14(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_12(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_16(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_14(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_18(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_16(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_20(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_18(__VA_ARGS__)

#define n_archs_2  1
#define n_archs_4  2
#define n_archs_6  3
#define n_archs_8  4
#define n_archs_10 5
#define n_archs_12 6
#define n_archs_14 7
#define n_archs_16 8
#define n_archs_18 9
#define n_archs_20 10

/* Generic kernel prototype and wrapper launchers */
CTRL_KERNEL_CHAR(updateCell_default_1D, MANUAL, 64, 8, 1);
CTRL_KERNEL_CHAR(updateCell_default_2D, MANUAL, 64, 8, 1);
CTRL_KERNEL_CHAR(updateCell_default_3D, MANUAL, 64, 8, 1);

CTRL_KERNEL_PROTO(updateCell_default_1D,
				  1, GENERIC, DEFAULT, 6,
				  OUT, HitTile_float, matrix,
				  IN, HitTile_float, matrixCopy,
				  IN, HitTile_float, weight,
				  INVAL, int, begin_x,
				  INVAL, int, end_x,
				  INVAL, float, factor);

void updateCell_default_1D(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor) {
	Ctrl_LaunchToStream(ctrl, updateCell_default_1D, threads, blockSize, stream, mat, copy, weight,
						hit_tileDimBegin(weight, 0), hit_tileDimEnd(weight, 0),
						factor);
}

CTRL_KERNEL_PROTO(updateCell_default_2D,
				  1, GENERIC, DEFAULT, 8,
				  OUT, HitTile_float, matrix,
				  IN, HitTile_float, matrixCopy,
				  IN, HitTile_float, weight,
				  INVAL, int, begin_x,
				  INVAL, int, begin_y,
				  INVAL, int, end_x,
				  INVAL, int, end_y,
				  INVAL, float, factor);

void updateCell_default_2D(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor) {
	Ctrl_LaunchToStream(ctrl, updateCell_default_2D, threads, blockSize, stream, mat, copy, weight,
						hit_tileDimBegin(weight, 0), hit_tileDimBegin(weight, 1),
						hit_tileDimEnd(weight, 0), hit_tileDimEnd(weight, 1),
						factor);
}

CTRL_KERNEL_PROTO(updateCell_default_3D,
				  1, GENERIC, DEFAULT, 10,
				  OUT, HitTile_float, matrix,
				  IN, HitTile_float, matrixCopy,
				  IN, HitTile_float, weight,
				  INVAL, int, begin_x,
				  INVAL, int, begin_y,
				  INVAL, int, begin_z,
				  INVAL, int, end_x,
				  INVAL, int, end_y,
				  INVAL, int, end_z,
				  INVAL, float, factor);

void updateCell_default_3D(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor) {
	Ctrl_LaunchToStream(ctrl, updateCell_default_3D, threads, blockSize, stream, mat, copy, weight,
						hit_tileDimBegin(weight, 0), hit_tileDimBegin(weight, 1), hit_tileDimBegin(weight, 2),
						hit_tileDimEnd(weight, 0), hit_tileDimEnd(weight, 1), hit_tileDimEnd(weight, 2),
						factor);
}

/* ARRAY INIT AND OUTPUT FUNCTIONS */
void initData(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);
void outputData(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);

/* PROTOTYPES OF INITIALIZATION FUNCTIONS */
void initData1D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);
void initData2D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);
void initData3D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);

/* EXPERIMENTATION: clocks */
HitClock mainClock;
HitClock initClock;
HitClock loopClock;
/// HitClock	redistributeClock;
HitClock commClock;

/* Kernel characterizations */
#define H_BORDER_CHAR ((Ctrl_Thread){.dims = 2, .x = 256, .y = 1, .z = 1})
#define V_BORDER_CHAR ((Ctrl_Thread){.dims = 2, .x = 1, .y = 256, .z = 1})
#define INNER_CHAR    ((Ctrl_Thread){.dims = 2, .x = 64, .y = 4, .z = 1})
#define CHAR_1D       ((Ctrl_Thread){.dims = 1, .x = 256, .y = 1, .z = 1})
#define CHAR_CPU      ((Ctrl_Thread){.dims = 2, .x = 16, .y = 16, .z = 1})

/* EXPERIMENTATION: Optimized stencil kernels registration */
REGISTER_STENCIL(updateCell_1dNC4);
REGISTER_STENCIL(updateCell_1dC2);
REGISTER_STENCIL(updateCell_4);
REGISTER_STENCIL(updateCell_9);
REGISTER_STENCIL(updateCell_NC9);
REGISTER_STENCIL(updateCell_F5);
REGISTER_STENCIL(updateCell_3d27);

/* B.0. INITIALIZE ARRAY: 1, 2 or 3 DIMENSIONS */
void initData(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]) {
	/* 0. OPTIONAL COMPILATION: READING THE INPUT MATRIX FROM A FILE */
	#ifdef READ_INPUT
	#ifdef READ_BIN
	hit_tileFileRead(&tileMat, "Matrix.in", HIT_FILE_ARRAY);
	#else
	hit_tileTextFileRead(&realTileA, "Matrix.in.dtxt", HIT_FILE_ARRAY, HIT_FILE_FLOAT, 14, 4);
	#endif
	#else
	/* 1. INIT Mat = 0 */
	float zero = 0;
	hit_tileFill(&tileMat, &zero);

	/* 2. INIT BORDERS */
	switch (dims) {
		case 1: initData1D(tileMat, dims, borderLow, borderHigh); break;
		case 2: initData2D(tileMat, dims, borderLow, borderHigh); break;
		case 3: initData3D(tileMat, dims, borderLow, borderHigh); break;
		default:
			fprintf(stderr, "Error: This init function only works for 1, 2, or 3 dimensions\n");
			hit_comFinalize();
			exit(EXIT_FAILURE);
	}
	#endif

	#ifdef WRITE_INPUT
	/* 4. WRITE MAT ON A FILE */
	#ifdef WRITE_BIN
	hit_tileFileWrite(&tileMat, "Matrix.out", HIT_FILE_ARRAY);
	#else
	#ifdef WRITE_TILE_FILES
	char name[8];
	sprintf(name, "Matrix.%d", hit_Rank);
	hit_tileTextFileWrite(&timeMat, name, HIT_FILE_TILE, HIT_FILE_FLOAT, 20, 4);
	#endif
	hit_tileTextFileWrite(&tileMat, "Matrix.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_FLOAT, 20, 4);
	#endif
	#endif
}

/* B.1. INITIALIZE ARRAY 1D */
void initData1D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]) {
	/* 2. INIT BORDERS  DOWN(i)=1, UP(i)=2. */
	HitTile root = *hit_tileRoot(&tileMat);
	int     i;

	/* 2.1. FIRST ELEMENTS ARE MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimBegin(root, 0)))
		for (i = 0; i < borderLow[0]; i++)
			hit_tileElemAt(tileMat, 1, i) = 1;

	/* 2.4. LAST ELEMENTS ARE MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimEnd(root, 0)))
		for (i = 0; i < borderHigh[0]; i++)
			hit_tileElemAt(tileMat, 1, hit_tileDimCard(tileMat, 0) - 1 - i) = 2;
}

/* B.2. INITIALIZE MATRIX 2D */
void initData2D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]) {
	/* 2. INIT BORDERS  UP(i)=1, DOWN(i)=2, LEFT(i)=3, RIGHT(i)=4 */
	HitTile root = *hit_tileRoot(&tileMat);
	int     i, j;

	/* 2.1. FIRST COLUMN IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 1), hit_tileDimBegin(root, 1)))
		for (j = 0; j < borderLow[1]; j++)
			hit_tileForDimDomain(tileMat, 0, i)
				hit_tileElemAt(tileMat, 2, i, j) = 3;

	/* 2.2. LAST COLUMN IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 1), hit_tileDimEnd(root, 1)))
		for (j = 0; j < borderHigh[1]; j++)
			hit_tileForDimDomain(tileMat, 0, i)
				hit_tileElemAt(tileMat, 2, i, hit_tileDimCard(tileMat, 1) - 1 - j) = 4;

	/* 2.3. FIRST ROW IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimBegin(root, 0)))
		for (i = 0; i < borderLow[0]; i++)
			hit_tileForDimDomain(tileMat, 1, j)
				hit_tileElemAt(tileMat, 2, i, j) = 1;

	/* 2.4. LAST ROW IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimEnd(root, 0)))
		for (i = 0; i < borderHigh[0]; i++)
			hit_tileForDimDomain(tileMat, 1, j)
				hit_tileElemAt(tileMat, 2, hit_tileDimCard(tileMat, 0) - 1 - i, j) = 2;
}

/* B.3. INITIALIZE MATRIX 3D */
void initData3D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]) {
	/* 2. INIT BORDERS */
	HitTile root = *hit_tileRoot(&tileMat);
	int     i, j, k;

	/* FIRST LAYER OF k IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 2), hit_tileDimBegin(root, 2)))
		hit_tileForDimDomain(tileMat, 0, i)
			hit_tileForDimDomain(tileMat, 1, j) for (k = 0; k < borderLow[2]; k++)
				hit_tileElemAt(tileMat, 3, i, j, k) = 5;

	/* LAST LAYER OF k IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 2), hit_tileDimEnd(root, 2)))
		hit_tileForDimDomain(tileMat, 0, i)
			hit_tileForDimDomain(tileMat, 1, j) for (k = 0; k < borderHigh[2]; k++)
				hit_tileElemAt(tileMat, 3, i, j, hit_tileDimCard(tileMat, 2) - 1 - k) = 6;

	/* FIRST LAYER OF j IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 1), hit_tileDimBegin(root, 1)))
		hit_tileForDimDomain(tileMat, 0, i) for (j = 0; j < borderLow[1]; j++)
			hit_tileForDimDomain(tileMat, 2, k)
				hit_tileElemAt(tileMat, 3, i, j, k) = 3;

	/* LAST LAYER OF j IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 1), hit_tileDimEnd(root, 1)))
		hit_tileForDimDomain(tileMat, 0, i) for (j = 0; j < borderHigh[1]; j++)
			hit_tileForDimDomain(tileMat, 2, k)
				hit_tileElemAt(tileMat, 3, i, hit_tileDimCard(tileMat, 1) - 1 - j, k) = 4;

	/* FIRST LAYER OF i IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimBegin(root, 0)))
		for (i = 0; i < borderLow[0]; i++)
			hit_tileForDimDomain(tileMat, 1, j)
				hit_tileForDimDomain(tileMat, 2, k)
					hit_tileElemAt(tileMat, 3, i, j, k) = 1;

	/* LAST LAYER OF i IS MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimEnd(root, 0)))
		for (i = 0; i < borderHigh[0]; i++)
			hit_tileForDimDomain(tileMat, 1, j)
				hit_tileForDimDomain(tileMat, 2, k)
					hit_tileElemAt(tileMat, 3, hit_tileDimCard(tileMat, 0) - 1 - i, j, k) = 2;
}

/* B.4. INITIALIZATION HOST-TASK */
CTRL_HOST_TASK(Ctrl_Init_Tiles, initDataFunction f_init, HitTile_float matrix, int dims, int *borderLow, int *borderHigh) {
	f_init(matrix, dims, borderLow, borderHigh);
}

CTRL_HOST_TASK_PROTO(Ctrl_Init_Tiles, 5,
					 INVAL, initDataFunction, f_init,
					 OUT, HitTile_float, matrix,
					 INVAL, int, dims,
					 INVAL, int *, borderLow,
					 INVAL, int *, borderHigh);

CTRL_HOST_TASK(print_matrix, HitTile_float mat) {
	printf("Matrix for [%d]:\n", hit_Rank);
	int i, j, k;
	switch (mat.shape.info.sig.numDims) {
		case 1:
			for (i = 0; i < hit_tileDimCard(mat, 0); i++) {
				printf("%g ", hit(mat, i));
			}
			break;
		case 2:
			for (i = 0; i < hit_tileDimCard(mat, 0); i++) {
				for (j = 0; j < hit_tileDimCard(mat, 1); j++) {
					printf("%g ", hit(mat, i, j));
				}
				printf("\n");
			}
			break;
		case 3:
			for (i = 0; i < hit_tileDimCard(mat, 0); i++) {
				for (j = 0; j < hit_tileDimCard(mat, 1); j++) {
					for (k = 0; k < hit_tileDimCard(mat, 2); k++) {
						printf("%g ", hit(mat, i, j, k));
					}
					printf("\n");
				}
				printf("\n");
			}
			break;
	}
	printf("\n");
	fflush(stdout);
}

CTRL_HOST_TASK_PROTO(print_matrix, 1,
					 IN, HitTile_float, mat);
/* C. WRITE RESULTS */
void outputData(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]) {

	#ifdef WRITE_RESULT
	HitTile       root = *hit_tileRoot(&tileMat);
	HitTile_float outputTile;
	HitShape      outputShape = hit_tileShape(tileMat);

	int i;
	for (i = 0; i < dims; i++) {
		/* FIRST LAYERS ARE NOT MINE */
		if (!hit_sigIn(hit_shapeSig(outputShape, i), hit_tileDimBegin(root, i)))
			outputShape = hit_shapeTransform(outputShape, i, HIT_SHAPE_BEGIN, +borderLow[i]);

		/* LAST LAYERS ARE NOT MINE */
		if (!hit_sigIn(hit_shapeSig(outputShape, i), hit_tileDimEnd(root, i)))
			outputShape = hit_shapeTransform(outputShape, i, HIT_SHAPE_END, -borderHigh[i]);
	}

	hit_tileSelectArrayCoords(&outputTile, &tileMat, outputShape);
	#ifdef WRITE_BIN
	hit_tileFileWrite(&outputTile, "Result.out", HIT_FILE_ARRAY);
	#else
	hit_tileTextFileWrite(&outputTile, "Result.out.dtxt", HIT_FILE_ARRAY, HIT_FILE_FLOAT, 42, 40);
	#endif
	#endif
}

/* C.1. OUTPUTP HOST-TASK */
CTRL_HOST_TASK(Ctrl_Write_Output, outputDataFunction f_output, HitTile_float matrix, int dims, int *borderLow, int *borderHigh) {
	f_output(matrix, dims, borderLow, borderHigh);
}

CTRL_HOST_TASK_PROTO(Ctrl_Write_Output, 5,
					 INVAL, outputDataFunction, f_output,
					 IN, HitTile_float, matrix,
					 INVAL, int, dims,
					 INVAL, int *, borderLow,
					 INVAL, int *, borderHigh);

/* D. STENCIL PATTERN TRANSFERENCE HOST-TASK */
CTRL_HOST_TASK(Ctrl_Copy_Stencil, HitTile_float stencil, float *stencil_data) {
	int i;
	int j;
	int k;
	int dataind = 0;
	int dims    = hit_tileDims(stencil);
	switch (dims) {
		case 1:
			for (i = 0; i < hit_tileDimCard(stencil, 0); i++)
				hit(stencil, i) = stencil_data[i];
			break;
		case 2:
			for (i = 0; i < hit_tileDimCard(stencil, 0); i++)
				for (j = 0; j < hit_tileDimCard(stencil, 1); j++)
					hit(stencil, i, j) = stencil_data[dataind++];

			break;
		case 3:
			for (i = 0; i < hit_tileDimCard(stencil, 0); i++)
				for (j = 0; j < hit_tileDimCard(stencil, 1); j++)
					for (k = 0; k < hit_tileDimCard(stencil, 2); k++)
						hit(stencil, i, j, k) = stencil_data[dataind++];
			break;
		default:
			fprintf(stderr, "[EPSILOD] %d dims are not supported in the stencil definition, max. 3 dims\n", dims);
			exit(EXIT_FAILURE);
	}
}

CTRL_HOST_TASK_PROTO(Ctrl_Copy_Stencil, 2,
					 OUT, HitTile_float, stencil,
					 INVAL, float *, stencil_data);

/* String hash function for hashing the node names: */
static inline unsigned int hash(char *restrict str) { // djb2 hash adapted to 32 bit ints.
	unsigned int hash = 5381;
	int          c;

	while ((c = *str++) != 0)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

/* HitPattern function for automatic redistribution: */

/* STENCIL COMPUTATION FUNCTION: */
void stencilComputation(int sizes[], HitShape stencilShape, float stencilData[], float factor,
						int numIterations, stencilFunction f_updateCell,
						initDataFunction f_init, outputDataFunction f_output) {
	int dims = stencilShape.info.sig.numDims;

	/* CHECK IF GENERIC KERNEL HAS BEEN CHOSEN */
	if (f_updateCell == NULL) {
		switch (dims) {
			case 1:
				f_updateCell = updateCell_default_1D;
				break;
			case 2:
				f_updateCell = updateCell_default_2D;
				break;
			case 3:
				f_updateCell = updateCell_default_3D;
				break;
			default:
				fprintf(stderr, "[EPSILOD ERROR] Invalidly dimensioned stencil. The skeleton only supports 1D, 2D or 3D stencils.\n");
				fflush(stderr);
				exit(EXIT_FAILURE);
		}
	}

	/* Create MPI intra-process communicator */
	char nodeName[MPI_MAX_PROCESSOR_NAME];
	int  nodeNameLen;
	MPI_Get_processor_name(nodeName, &nodeNameLen);

	MPI_Comm commNode;
	MPI_Comm_split(MPI_COMM_WORLD, hash(nodeName), hit_Rank, &commNode);

	int hit_NodeRank, hit_NodeProcs;
	MPI_Comm_rank(commNode, &hit_NodeRank);
	MPI_Comm_size(commNode, &hit_NodeProcs);

	/* Get number of devices available to this process */
	int nDevices = 0;

	#ifdef _CTRL_ARCH_CUDA_
	int cudaGPUs;
	cudaGetDeviceCount(&cudaGPUs);
	nDevices += cudaGPUs;
	#ifdef _PSS_DEV_INFO_
	if (!hit_NodeRank) {
		printf("Found %d CUDA devices in node %s\n", cudaGPUs, nodeName);
		fflush(stdout);
	}
	#endif // _PSS_DEV_INFO_
	#endif // _CTRL_ARCH_CUDA_
	#ifdef _CTRL_ARCH_OPENCL_GPU_
	cl_uint n_platforms;
	OPENCL_ASSERT_OP(clGetPlatformIDs(0, NULL, &n_platforms));
	cl_platform_id *p_platforms = (cl_platform_id *)malloc(n_platforms * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(n_platforms, p_platforms, NULL));

	size_t platform_name_size;
	OPENCL_ASSERT_OP(clGetPlatformInfo(p_platforms[0], CL_PLATFORM_NAME, 0, NULL, &platform_name_size));
	char *platform_name = (char *)malloc(platform_name_size * sizeof(char));

	int ocl_platform_index = -1;
	for (int i = 0; i < (int)n_platforms; i++) {
		clGetPlatformInfo(p_platforms[i], CL_PLATFORM_NAME, platform_name_size, platform_name, NULL);
		if (!strncmp(platform_name, "AMD", 3)) {
			ocl_platform_index = i;
			break;
		}
	}
	free(platform_name);

	cl_uint oclGPUs = 0;
	if (ocl_platform_index > -1) {
		OPENCL_ASSERT_OP(clGetDeviceIDs(p_platforms[ocl_platform_index], CL_DEVICE_TYPE_GPU, 0, NULL, &oclGPUs));
	}
	free(p_platforms);

	nDevices += oclGPUs;
	#ifdef _PSS_DEV_INFO_
	if (!hit_NodeRank) {
		printf("Found %d AMD devices in node %s\n", oclGPUs, nodeName);
		fflush(stdout);
	}
	#endif // _PSS_DEV_INFO_
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#ifdef _CTRL_ARCH_CPU_
	int cpuThreads = hit_max(omp_get_max_threads(), 2 * hit_NodeProcs - 2) - 2 * hit_NodeProcs - 2; // Subtract the ctrl threads used by other processes.
	#ifdef _PSS_TRASGO_CLUSTER_
	if (!strcmp(nodeName, "manticore"))
		cpuThreads = hit_min(cpuThreads, 16);
	else if (!strcmp(nodeName, "hydra"))
		cpuThreads = hit_min(cpuThreads, 4);
	else if (!strcmp(nodeName, "medusa"))
		cpuThreads = hit_min(cpuThreads, 6);
	#endif //_PSS_TRASGO_CLUSTER_
	nDevices += cpuThreads > 0 ? 1 : 0;
	#endif // _CTRL_ARCH_CPU_

	// Remove surplus processes.
	if (hit_NodeRank >= nDevices) {
		fprintf(stderr, "[GenericStencil warning] Found surplus process (%d, %d in node %s) with no associated device (max node devices: %d). "
						"This process should not be used.\n",
				hit_Rank, hit_NodeRank, nodeName, nDevices);
		fflush(stderr);
	}

	int device = hit_NodeRank + 0; // strcmp(nodeName, "gorgon.infor.uva.es") ? 1 : 0;

	int affinity = 0;
	#ifdef _PSS_TRASGO_CLUSTER_
	if (!strcmp(nodeName, "manticore"))
		affinity = hit_NodeRank < 2 ? 1 : 0;
	else if (!strcmp(nodeName, "hydra"))
		affinity = hit_NodeRank > 1 && hit_NodeRank < 4 ? 1 : 0;
	else if (!strcmp(nodeName, "medusa"))
		affinity = hit_NodeRank == 1 || hit_NodeRank == 3 ? 1 : 0;
	#endif //_PSS_TRASGO_CLUSTER_

	Ctrl_SetHostAffinity(affinity);

	__ctrl_block__(1, 1) {
		int i, j, k;

		/* INIT CLOCKS */
		hit_clockSynchronizeAll();
		hit_clockStart(mainClock);
		hit_clockStart(initClock);

		PCtrl comm = NULL;
		#ifdef _CTRL_ARCH_CUDA_
		if (comm == NULL && device < cudaGPUs) {
			comm = Ctrl_Create(CTRL_TYPE_CUDA, CTRL_POLICY_ASYNC, device, 2 * dims);
			#ifdef _PSS_DEV_INFO_
			printf("Process %d (%d in %s) using CUDA's GPU %d (numa node: %d)\n", hit_Rank, hit_NodeRank, nodeName, device, affinity);
			fflush(stdout);
			#endif // _PSS_DEV_INFO_
		}
		device -= cudaGPUs;
		#endif // _CTRL_ARCH_CUDA_
		#ifdef _CTRL_ARCH_OPENCL_GPU_
		if (comm == NULL && device < oclGPUs && ocl_platform_index >= 0) {
			#ifdef _PSS_DEV_INFO_
			printf("Process %d (%d in %s) using OpenCL's GPU %d (platform %d) (numa node: %d)\n", hit_Rank, hit_NodeRank, nodeName, device, ocl_platform_index, affinity);
			fflush(stdout);
			#endif // _PSS_DEV_INFO_
			comm = Ctrl_Create(CTRL_TYPE_OPENCL_GPU, CTRL_POLICY_ASYNC, device, ocl_platform_index, 2 * dims);
		}
		device -= oclGPUs;
		#endif // _CTRL_ARCH_OPENCL_GPU__
		#ifdef _CTRL_ARCH_CPU_
		if (comm == NULL && device >= 0) {
			int  p_numanodes[1] = {1};
			int  n_numanodes    = 1;
			bool mem_transfers  = false;

			#ifdef _PSS_DEV_INFO_
			printf("Process %d (%d in %s) using CPU with %d threads\n", hit_Rank, hit_NodeRank, nodeName, cpuThreads);
			fflush(stdout);
			#endif // _PSS_DEV_INFO_
			comm = Ctrl_Create(CTRL_TYPE_CPU, CTRL_POLICY_ASYNC, cpuThreads, p_numanodes, n_numanodes, mem_transfers);
		}
		#endif // _CTRL_ARCH_CPU_
		if (comm == NULL) {
			fprintf(stderr, "[GenericStencil ERROR]: Couldn't find a proper Ctrl architecture to execute the stencil on. Recompile the library and program with the proper architecture support.\n");
			fflush(stderr);
			MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
			exit(EXIT_FAILURE);
		}

		Ctrl_SetDependanceMode(comm, CTRL_MODE_EXPLICIT);

		/* 0. BUILD THE STENCIL TILE */
		HitTile_float stencil = Ctrl_Domain(comm, float, stencilShape);

		Ctrl_Alloc(comm, stencil, CTRL_MEM_ALLOC_BOTH);
		Ctrl_WaitTile(comm, stencil);

		if (f_updateCell == updateCell_default_1D ||
			f_updateCell == updateCell_default_2D ||
			f_updateCell == updateCell_default_3D) {
			Ctrl_HostTask(comm, Ctrl_Copy_Stencil, stencil, stencilData);
			Ctrl_MoveTo(comm, stencil);
		}

		/* 1. BUILD ORIGINAL SHAPE */
		HitShape shp = HIT_SHAPE_NULL;
		hit_shapeDimsSet(shp, dims);
		for (i = 0; i < dims; i++) {
			hit_shapeSig(shp, i) = hit_sig(0, sizes[i] - 1, 1);
		}

		/* 2. SHORTCUTS FOR BORDER SIZES */
		int *borderLow  = (int *)malloc(dims * sizeof(int));
		int *borderHigh = (int *)malloc(dims * sizeof(int));
		for (i = 0; i < dims; i++) {
			borderLow[i]  = -hit_tileDimBegin(stencil, i);
			borderHigh[i] = hit_tileDimEnd(stencil, i);
		}

		/* 3.1. SHAPE TO DISTRIBUTE COMPUTATION (WITHOUT BORDERS) */
		HitShape shpInner = shp;
		for (i = 0; i < dims; i++) {
			shpInner = hit_shapeTransform(shpInner, i, HIT_SHAPE_BEGIN, +borderLow[i]);
			shpInner = hit_shapeTransform(shpInner, i, HIT_SHAPE_END, -borderHigh[i]);
		}

		/* 3.2. BUILD DISTRIBUTED SHAPE */
		HitTopology topo;

		#ifdef _PSS_USE_STATIC_LOAD_BALANCING_
		#ifdef _PSS_TRASGO_CLUSTER_
		float GPUWeights[7] = {
			/* Manticore: Use 0.41 instead of 0.41 for 2dnc9 */
			1.0, 1.0, 0.46, 0.46, // < Manticore weights (V100s first)
			//0.46, 0.46, 1.0, 1.0, // < Manticore weights (Radeons first)
			0.25,                 // < Gorgon weights (1 GPU)
			//0.25, 0.25, 0.25,     // < Gorgon weights (4 GPUs)
			0.35,                 // < Medusa weights
			0.21                  // < Hydra weights
		};
		#else // !_PSS_TRASGO_CLUSTER_
		fprintf(stderr, "[EPSILOD ERROR] No weights provided for static load balancing. "
			"Edit the EPSILOD function accordingly.\n");
		exit(EXIT_FAILURE);
		#endif // _PSS_TRASGO_CLUSTER_
		topo = hit_topology(plug_topPlain);
		HitWeights weights = hitWeights(hit_NProcs, GPUWeights);
		HitLayout  lay     = hit_layout(plug_layDimWeighted_Blocks, topo, shpInner, 0, weights);
		#else // !_PSS_USE_STATIC_LOAD_BALANCING_
		#ifdef _PSS_USE_1D_LAYOUT_
		topo = hit_topology(plug_topPlain);
		HitLayout lay = hit_layout(plug_layDimBlocks, topo, shpInner, 0);
		#else // !_PSS_USE_1D_LAYOUT_
		if (dims == 3) {
			topo = hit_topology(plug_topArray3D);
		} else if (dims == 2) {
			topo = hit_topology(plug_topArray2DComplete);
		} else {
			topo = hit_topology(plug_topPlain);
		}
		HitLayout lay = hit_layout(plug_layBlocks, topo, shpInner);
		#endif // _PSS_USE_1D_LAYOUT_
		#endif // _PSS_USE_STATIC_LOAD_BALANCING_

		HitShape shpLayout = hit_layShape(lay);
		#ifdef _PSS_DEV_INFO_
		printf("[%d] shpLayout\t", hit_Rank);
		dumpShape(shpLayout);
		fflush(stdout);
		#endif // _PSS_DEV_INFO_

		/* 4. ACTIVE PROCESSES */
		if (hit_layImActive(lay)) {

			// 4.1. STOP IF THERE IS NOT ENOUGH INNER DATA FOR A BORDER
			for (i = 0; i < dims; i++) {
				if (hit_shapeSigCard(shpLayout, i) < borderLow[i] ||
					hit_shapeSigCard(shpLayout, i) < borderHigh[i]) {
					if (hit_Rank == 0) {
						fprintf(stderr, "\nError: Not enough data after partition, too many processes in a topology axis\n\n");
						MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TOPOLOGY);
						exit(EXIT_FAILURE);
					}
				}
			}

			/* 4.2. EXPANDED SHAPE */
			HitShape shpExpanded = shpLayout;
			for (i = 0; i < dims; i++) {
				shpExpanded = hit_shapeTransform(shpExpanded, i, HIT_SHAPE_BEGIN, -borderLow[i]);
				shpExpanded = hit_shapeTransform(shpExpanded, i, HIT_SHAPE_END, borderHigh[i]);
			}

			/* 4.3. BORDER SHAPES: IN/OUT */
			int      numBorders = (int)pow(3, dims);
			HitShape shpBorderIn[numBorders];
			HitShape shpBorderOut[numBorders];
			HitRanks shiftsIn[numBorders];
			HitRanks shiftsOut[numBorders];
			int      borderInActive[numBorders];
			for (i = 0; i < numBorders; i++)
				borderInActive[i] = 0;

			// TRAVERSE THE STENCIL TO DETECT ACTIVE AND INACTIVE BORDERS DUE TO WEIGHTS
			int indeces[dims];
			int displacement = 0;
			for (j = 0; j < dims; j++)
				indeces[j] = 0;
			int endAnalysis = 0;
			while (!endAnalysis) {
				// CHECK IF THERE IS A WEIGHT IN THE STENCIL POSITION
				if (stencilData[displacement] != 0) {
					// ACTIVE BORDER, COMPUTE ITS NUMBER TO RAISE THE FLAG
					int acum   = 1;
					int border = 0;
					for (j = dims - 1; j >= 0; j--) {
						if (indeces[j] > borderLow[j])
							border += 2 * acum;
						else if (indeces[j] == borderLow[j])
							border += acum;
						acum *= 3;
					}
					borderInActive[border] = 1;
				}
				// ADVANCE TO THE NEXT STENCIL POSITION
				displacement++;
				for (k = dims - 1; k >= 0; k--) {
					indeces[k]++;
					if (k == 0 && indeces[0] == hit_tileDimCard(stencil, 0)) endAnalysis = 1;
					if (indeces[k] >= hit_tileDimCard(stencil, k))
						indeces[k] = 0;
					else
						break;
				}
			}
			// ALWAYS SKIP FALSE BORDER: TILE INNER
			borderInActive[numBorders / 2] = 0;

			// BUILD BORDER SHAPES AND THE SHORTCUTS OF THE NEIGHBOR SHIFTS
			for (i = 0; i < numBorders; i++) {
				shiftsIn[i]  = HIT_RANKS_NULL;
				shiftsOut[i] = HIT_RANKS_NULL;

				// NON-ACTIVE BORDERS, NULL SHAPES, NULL RANKS
				if (!borderInActive[i]) {
					shpBorderIn[i]  = HIT_SHAPE_NULL;
					shpBorderOut[i] = HIT_SHAPE_NULL;
					continue;
				}
				shpBorderIn[i]  = shpLayout;
				shpBorderOut[i] = shpLayout;

				// EXTRACT RANKS FOR THIS BORDER
				int digits = i;
				for (j = 0; j < dims; j++) {
					shiftsIn[i].rank[j]  = digits % 3 - 1;
					shiftsOut[i].rank[j] = -shiftsIn[i].rank[j];
					digits /= 3;

					// SHAPE IN
					if (shiftsIn[i].rank[j] == -1) {
						shpBorderIn[i] = hit_shapeTransform(shpBorderIn[i], j, HIT_SHAPE_FIRST, borderLow[j]);
						shpBorderIn[i] = hit_shapeTransform(shpBorderIn[i], j, HIT_SHAPE_MOVE, -borderLow[j]);
					} else if (shiftsIn[i].rank[j] == 1) {
						shpBorderIn[i] = hit_shapeTransform(shpBorderIn[i], j, HIT_SHAPE_LAST, borderHigh[j]);
						shpBorderIn[i] = hit_shapeTransform(shpBorderIn[i], j, HIT_SHAPE_MOVE, borderHigh[j]);
					}

					// SHAPE OUT (REVERSERD TARGET)
					if (shiftsIn[i].rank[j] == -1) {
						shpBorderOut[i] = hit_shapeTransform(shpBorderOut[i], j, HIT_SHAPE_LAST, borderLow[j]);
					} else if (shiftsIn[i].rank[j] == 1) {
						shpBorderOut[i] = hit_shapeTransform(shpBorderOut[i], j, HIT_SHAPE_FIRST, borderHigh[j]);
					}
				}
			}

			// DEACTIVATE BORDER COMMs DUE TO LACK OF NEIGHBOR
			for (i = 0; i < numBorders; i++) {
				// SKIP EMPTY BORDERS
				if (!borderInActive[i]) continue;
				// LOCATE NEIGHBOR IN THE LAYOUT GRID
				HitRanks neighIn  = hit_layNeighborN(lay, shiftsIn[i]);
				HitRanks neighOut = hit_layNeighborN(lay, shiftsOut[i]);

				// IF NEIGHBORS DOES NOT EXIST, DEACTIVATE BORDER COMMs.
				if (neighIn.rank[0] == -1 && neighOut.rank[0] == -1) {
					borderInActive[i] = 0;
				}
			}

			/* 4.4. NON-OVERLAPED BORDERS IN THE INNER PART
				a) TO EXTRACT DATA FROM DEVICE WITHOUT REPLICATION
				b) TO DETERMINE THE THREADS-GRID CARDINALITIES FOR THE COMPUTING KERNELS
			*/
			HitShape shpInnerLocal = shpLayout;
			HitShape shpBorderOutDev[dims][2];
			for (j = 0; j < dims; j++) {
				shpInnerLocal = hit_shapeTransform(shpInnerLocal, j, HIT_SHAPE_BEGIN, borderHigh[j]);
				shpInnerLocal = hit_shapeTransform(shpInnerLocal, j, HIT_SHAPE_END, -borderLow[j]);

				shpBorderOutDev[j][0] = hit_shapeTransform(shpLayout, j, HIT_SHAPE_FIRST, borderHigh[j]);
				shpBorderOutDev[j][1] = hit_shapeTransform(shpLayout, j, HIT_SHAPE_LAST, borderLow[j]);
				// TAKE OUT THE PARTS WHICH ARE OVERLAPPED WITH PREVIOUS DIMS
				for (k = 0; k < j; k++) {
					shpBorderOutDev[j][0] = hit_shapeTransform(shpBorderOutDev[j][0], k, HIT_SHAPE_BEGIN, borderHigh[k]);
					shpBorderOutDev[j][0] = hit_shapeTransform(shpBorderOutDev[j][0], k, HIT_SHAPE_END, -borderLow[k]);
					shpBorderOutDev[j][1] = hit_shapeTransform(shpBorderOutDev[j][1], k, HIT_SHAPE_BEGIN, borderHigh[k]);
					shpBorderOutDev[j][1] = hit_shapeTransform(shpBorderOutDev[j][1], k, HIT_SHAPE_END, -borderLow[k]);
				}
				// NULL SIGNATURE
			}

			for (j = 0; j < dims; j++) {
				for (k = 0; k < dims; k++) {
					if (hit_shapeSig(shpBorderOutDev[j][0], k).begin > hit_shapeSig(shpBorderOutDev[j][0], k).end) {
						shpBorderOutDev[j][0] = HIT_SIG_SHAPE_NULL;
						break;
					}
				}
			}
			for (j = 0; j < dims; j++) {
				for (k = 0; k < dims; k++) {
					if (hit_shapeSig(shpBorderOutDev[j][1], k).begin > hit_shapeSig(shpBorderOutDev[j][1], k).end) {
						shpBorderOutDev[j][1] = HIT_SIG_SHAPE_NULL;
						break;
					}
				}
			}

			/* 4.5. BUILD TILES */
			int            num_tiles_total = 1 + 1 + 2 * numBorders + dims * 2;
			HitTile_float *tileList[num_tiles_total];
			HitTile_float *copyList[num_tiles_total];

			HitTile_float tileBorderIn[numBorders];
			HitTile_float tileCopyBorderIn[numBorders];
			HitTile_float tileBorderOut[numBorders];
			HitTile_float tileCopyBorderOut[numBorders];
			HitTile_float tileBorderOutDev[dims][2];
			HitTile_float tileCopyBorderOutDev[dims][2];

			HitTile_float globalMat = Ctrl_Domain(comm, float, shp);

			HitTile_float tileMat, tileCopy;

			tileMat  = Ctrl_Select(comm, float, globalMat, shpExpanded, CTRL_SELECT_ARR_COORD); //-------------//select tileMat
			tileCopy = Ctrl_Select(comm, float, globalMat, shpExpanded, CTRL_SELECT_ARR_COORD); //-------------//select tileCopy

			tileList[0] = &tileMat;
			copyList[0] = &tileCopy;

			Ctrl_Alloc(comm, tileMat, CTRL_MEM_ALLOC_BOTH);  //---------------------//alloc tileMat
			Ctrl_Alloc(comm, tileCopy, CTRL_MEM_ALLOC_BOTH); //---------------------//alloc tileCopy

			HitTile_float tileInnerLocal     = Ctrl_Select(comm, float, tileMat, shpInnerLocal, CTRL_SELECT_ARR_COORD);
			HitTile_float tileCopyInnerLocal = Ctrl_Select(comm, float, tileCopy, shpInnerLocal, CTRL_SELECT_ARR_COORD);

			tileList[1] = &tileInnerLocal;
			copyList[1] = &tileCopyInnerLocal;

			for (i = 0; i < numBorders; i++) {
				tileBorderIn[i]      = Ctrl_Select(comm, float, tileMat, shpBorderIn[i], CTRL_SELECT_ARR_COORD);   //-------------------//select
				tileCopyBorderIn[i]  = Ctrl_Select(comm, float, tileCopy, shpBorderIn[i], CTRL_SELECT_ARR_COORD);  //-------------------//select
				tileBorderOut[i]     = Ctrl_Select(comm, float, tileMat, shpBorderOut[i], CTRL_SELECT_ARR_COORD);  //-------------------//select
				tileCopyBorderOut[i] = Ctrl_Select(comm, float, tileCopy, shpBorderOut[i], CTRL_SELECT_ARR_COORD); //-------------------//select

				tileList[2 + 2 * i]     = &tileBorderIn[i];
				copyList[2 + 2 * i]     = &tileCopyBorderIn[i];
				tileList[2 + 2 * i + 1] = &tileBorderOut[i];
				copyList[2 + 2 * i + 1] = &tileCopyBorderOut[i];
			}

			// clang-format off
			
			#define validShape(s) (hit_shapeDims((s)) != (-1))
			// clang-format on

			for (i = 0; i < dims; i++) {
				if (validShape(shpBorderOutDev[i][0])) {
					tileBorderOutDev[i][0]     = Ctrl_Select(comm, float, tileMat, shpBorderOutDev[i][0], CTRL_SELECT_ARR_COORD);  //-------------------//select
					tileCopyBorderOutDev[i][0] = Ctrl_Select(comm, float, tileCopy, shpBorderOutDev[i][0], CTRL_SELECT_ARR_COORD); //-------------------//select
				} else {
					tileBorderOutDev[i][0]     = *(HitTile_float *)&HIT_TILE_NULL;
					tileCopyBorderOutDev[i][0] = *(HitTile_float *)&HIT_TILE_NULL;
				}
				if (validShape(shpBorderOutDev[i][1])) {
					tileBorderOutDev[i][1]     = Ctrl_Select(comm, float, tileMat, shpBorderOutDev[i][1], CTRL_SELECT_ARR_COORD);  //-------------------//select
					tileCopyBorderOutDev[i][1] = Ctrl_Select(comm, float, tileCopy, shpBorderOutDev[i][1], CTRL_SELECT_ARR_COORD); //-------------------//select
				} else {
					tileBorderOutDev[i][1]     = *(HitTile_float *)&HIT_TILE_NULL;
					tileCopyBorderOutDev[i][1] = *(HitTile_float *)&HIT_TILE_NULL;
				}

				tileList[2 + 2 * numBorders + 2 * i]     = &tileBorderOutDev[i][0];
				copyList[2 + 2 * numBorders + 2 * i]     = &tileCopyBorderOutDev[i][0];
				tileList[2 + 2 * numBorders + 2 * i + 1] = &tileBorderOutDev[i][1];
				copyList[2 + 2 * numBorders + 2 * i + 1] = &tileCopyBorderOutDev[i][1];
			}

			/* 4.6. BUILD DISTRIBUTED-MEMORY COMMUNICATION PATTERN */
			HitPattern neighSync = hit_pattern(HIT_PAT_UNORDERED);
			for (i = 0; i < numBorders; i++) {
				// SKIP EMPTY BORDERS
				if (!borderInActive[i]) continue;

				// LOCATE NEIGHBORS IN THE LAYOUT GRID

				HitRanks neighIn  = HIT_RANKS_NULL;
				HitRanks neighOut = HIT_RANKS_NULL;

				if (borderInActive[i]) {
					neighIn  = hit_layNeighborN(lay, shiftsIn[i]);
					neighOut = hit_layNeighborN(lay, shiftsOut[i]);
				}

				hit_patternAdd(&neighSync, hit_comSendRecv(lay,
														   neighOut, &tileBorderOut[i],
														   neighIn, &tileBorderIn[i],
														   HIT_FLOAT));
			}

			HitPattern neighSyncCopy = hit_pattern(HIT_PAT_UNORDERED);
			for (i = 0; i < numBorders; i++) {
				// SKIP EMPTY BORDERS
				if (!borderInActive[i]) continue;

				// LOCATE NEIGHBORS IN THE LAYOUT GRID

				HitRanks neighIn  = HIT_RANKS_NULL;
				HitRanks neighOut = HIT_RANKS_NULL;

				if (borderInActive[i]) {
					neighIn  = hit_layNeighborN(lay, shiftsIn[i]);
					neighOut = hit_layNeighborN(lay, shiftsOut[i]);
				}

				hit_patternAdd(&neighSyncCopy, hit_comSendRecv(lay,
															   neighOut, &tileCopyBorderOut[i],
															   neighIn, &tileCopyBorderIn[i],
															   HIT_FLOAT));
			}

			/* 4.7. INITIALIZE REDISTRIBUTION STRUCTURES */
			/// HitTile *redistributedTiles[2];
			/// HitPattern *redistributedPatterns[2];

			/// HitAvg avgs = hit_avgSimple(49); // TODO: change windows from 49 (50 - 1) to something that makes more sense.

			/* 4.8. INITIALIZE ARRAY */
			Ctrl_HostTask(comm, Ctrl_Init_Tiles, f_init, tileMat, dims, borderLow, borderHigh);
			Ctrl_HostTask(comm, Ctrl_Init_Tiles, f_init, tileCopy, dims, borderLow, borderHigh);
			Ctrl_WaitTile(comm, tileMat, tileCopy);

			/* Send tileMat to the device */
			Ctrl_MoveTo(comm, tileMat, tileCopy);
			Ctrl_WaitTile(comm, tileMat, tileCopy);
			// clang-format off
			
			#define copy2Threads( th, arr ) {th.x = arr[0]; th.y = arr[1]; th.z = arr[2];}
			// clang-format on
			Ctrl_Thread threadsInner;
			threadsInner.dims = dims;

			int threadsInnerCoords[3] = {1, 1, 1};

			for (i = 0; i < dims && i < 3; i++) {
				threadsInnerCoords[i] = hit_tileDimCard(tileInnerLocal, i);
			}
			copy2Threads(threadsInner, threadsInnerCoords);

			Ctrl_Thread thrBorderOutDev[dims][2];

			for (i = 0; i < dims; i++) {
				thrBorderOutDev[i][0].dims = dims;
				thrBorderOutDev[i][1].dims = dims;

				int coords[2][3] = {{1, 1, 1}, {1, 1, 1}};
				for (j = 0; j < dims && j < 3; j++) {
					coords[0][j] = hit_tileDimCard(tileBorderOutDev[i][0], j);
					coords[1][j] = hit_tileDimCard(tileBorderOutDev[i][1], j);
				}
				copy2Threads(thrBorderOutDev[i][0], coords[0]);
				copy2Threads(thrBorderOutDev[i][1], coords[1]);
				thrBorderOutDev[i][0].dims = dims;
				thrBorderOutDev[i][1].dims = dims;
			}

			/* Inner tile characterization */
			Ctrl_Thread inner_char;
			if (dims != 1) {
				inner_char = comm->type == CTRL_TYPE_CPU ? CHAR_CPU : INNER_CHAR;
			} else {
				inner_char = CHAR_1D;
			}
			hit_clockStop(initClock);

			hit_comBarrier(lay);

			/* 4.9. COMPUTATION LOOP */
			hit_clockStart(loopClock);

			// clang-format off
			
			#define swap(a, b, n)                      \
				for (int _i = 0; _i < n; _i++) {       \
					HitTile_float tmp = *a[_i];        \
					*a[_i]            = *b[_i];        \
					a[_i]->ref        = tmp.ref;       \
					tmp.ref           = b[_i]->ref;    \
					*b[_i]            = tmp;           \
				}                                      \
				HitPattern *sync     = &neighSync;     \
				HitPattern *syncCopy = &neighSyncCopy; \
				HitPattern  tmpSync  = neighSync;      \
				*sync                = neighSyncCopy;  \
				*syncCopy            = tmpSync;
			// clang-format on

			int loopIndex;
			for (loopIndex = 0; loopIndex < numIterations - 1; loopIndex++) {
				/* 4.8.1. UPDATE TILE COPY */
				if (validShape(tileCopy.shape) && validShape(tileMat.shape)) {
					swap(tileList, copyList, num_tiles_total);
				}

				/* 4.8.2. SEQUENTIALIZED LOOP */
				for (i = 0; i < dims; i++) {
					if (validShape(tileBorderOutDev[i][0].shape) && validShape(tileCopyBorderOutDev[i][0].shape)) {
						f_updateCell(comm, thrBorderOutDev[i][0], H_BORDER_CHAR, 2 * i, tileBorderOutDev[i][0], tileCopyBorderOutDev[i][0], stencil, factor);
					}
					if (validShape(tileBorderOutDev[i][1].shape) && validShape(tileCopyBorderOutDev[i][1].shape)) {
						f_updateCell(comm, thrBorderOutDev[i][1], V_BORDER_CHAR, 2 * i + 1, tileBorderOutDev[i][1], tileCopyBorderOutDev[i][1], stencil, factor);
					}
				}

				if (validShape(tileInnerLocal.shape) && validShape(tileCopyInnerLocal.shape)) {
					f_updateCell(comm, threadsInner, inner_char, 0, tileInnerLocal, tileCopyInnerLocal, stencil, factor);
				}

				for (i = 0; i < dims; i++) {
					for (j = 0; j < 2; j++) {
						if (validShape(tileBorderOutDev[i][j].shape)) {
							Ctrl_MoveFrom(comm, tileBorderOutDev[i][j]);
						}
					}
					break; // Only first dimension in topPlain topology.
				}
				for (i = 0; i < dims; i++) {
					for (j = 0; j < 2; j++) {
						if (validShape(tileBorderOutDev[i][j].shape)) {
							Ctrl_WaitTile(comm, tileBorderOutDev[i][j]);
						}
					}
				}
				/// redistributedTiles[0] = (HitTile *)&tileMat;
				/// redistributedTiles[1] = (HitTile *)&tileCopy;
				/// redistributedPatterns[0] = &neighSync;
				/// redistributedPatterns[1] = &neighSyncCopy;

				/// hit_clockStart( redistributeClock );
				/// hit_albWeightedRedistributeFixedInterval(redistributedTiles, 2, &lay, &avgs, redistributedPatterns, 2, ... );
				/// hit_clockStop( redistributeClock );

				hit_clockStart(commClock);
				hit_patternDo(neighSync);
				hit_clockStop(commClock);

				for (i = 0; i < numBorders; i++) {
					// SKIP EMPTY BORDERS
					if (!borderInActive[i]) continue;
					Ctrl_MoveTo(comm, tileBorderIn[i]);
				}
				for (i = 0; i < numBorders; i++) {
					// SKIP EMPTY BORDERS
					if (!borderInActive[i]) continue;
					Ctrl_WaitTile(comm, tileBorderIn[i]);
				}
				Ctrl_WaitTile(comm, tileInnerLocal);
			} // Computation loop

			/* 4.10. LAST ITERATION UPDATE: NO COMMUNICATION AFTER */
			if (validShape(tileCopy.shape) && validShape(tileMat.shape)) {
				swap(tileList, copyList, num_tiles_total);
			}

			for (i = 0; i < dims; i++) {
				if (validShape(tileBorderOutDev[i][0].shape) && validShape(tileCopyBorderOutDev[i][0].shape)) {
					f_updateCell(comm, thrBorderOutDev[i][0], H_BORDER_CHAR, 2 * i, tileBorderOutDev[i][0], tileCopyBorderOutDev[i][0], stencil, factor);
				}
				if (validShape(tileBorderOutDev[i][1].shape) && validShape(tileCopyBorderOutDev[i][1].shape)) {
					f_updateCell(comm, thrBorderOutDev[i][1], V_BORDER_CHAR, 2 * i + 1, tileBorderOutDev[i][1], tileCopyBorderOutDev[i][1], stencil, factor);
				}
			}
			if (validShape(tileInnerLocal.shape) && validShape(tileCopyInnerLocal.shape)) {
				f_updateCell(comm, threadsInner, inner_char, 0, tileInnerLocal, tileCopyInnerLocal, stencil, factor);
			}

			// Wait for kernels to finish:
			for (i = 0; i < dims; i++)
				for (j = 0; j < 2; j++)
					if (validShape(tileBorderOutDev[i][j].shape))
						Ctrl_WaitTile(comm, tileBorderOutDev[i][j]);
			Ctrl_WaitTile(comm, tileInnerLocal);

			// Move matrix to the host:
			Ctrl_MoveFrom(comm, tileMat);
			Ctrl_WaitTile(comm, tileMat);

			/* 4.11. CLOCK RESULTS */
			hit_clockStop(mainClock);
			hit_clockStop(loopClock);
			hit_clockReduce(lay, mainClock);
			hit_clockReduce(lay, initClock);
			hit_clockReduce(lay, loopClock);
			/// hit_clockReduce( lay, redistributeClock );
			hit_clockReduce(lay, commClock);
			hit_clockPrintMax(mainClock);
			hit_clockPrintMax(initClock);
			hit_clockPrintMax(loopClock);
			/// hit_clockPrintMax( redistributeClock );
			hit_clockPrintMax(commClock);

			/* 4.12. WRITE RESULT MATRIX */
			f_output(tileMat, dims, borderLow, borderHigh);
			hit_patternFree(&neighSync);
			hit_patternFree(&neighSyncCopy);
		} // layactive

		/* 5. INACTIVE PROCESSES: ONLY COLLECTIVE CLOCK OPERATIONS */
		else {
			hit_clockStop(initClock);
			hit_clockStop(mainClock);
			hit_clockStop(loopClock);
			hit_clockReduce(lay, mainClock);
			hit_clockReduce(lay, initClock);
			hit_clockReduce(lay, loopClock);
			/// hit_clockReduce( lay, redistributeClock );
			hit_clockReduce(lay, commClock);
			hit_clockPrintMax(mainClock);
			hit_clockPrintMax(initClock);
			hit_clockPrintMax(loopClock);
			/// hit_clockPrintMax( redistributeClock );
			hit_clockPrintMax(commClock);
		}

		/* 6. FREE OTHER RESOURCES */
		Ctrl_Destroy(comm);
		hit_layFree(lay);
		hit_topFree(topo);
		free(borderLow);
		free(borderHigh);
	}
}

/* HELP: PRINT ARGUMENT USAGE */
void printUsage(char *argv[]) {
	fprintf(stderr, "\n=== DISTRIBUTED MULTI-GPU STENCIL COMPUTATION EXAMPLE ===\n");
	fprintf(stderr, "\nUsage: %s <stencilId> <size0> [ <size1> [ <size2> ] ] <numIterations>\n", argv[0]);
	fprintf(stderr, "\t1dnc4\t1D Non-Compact, 4-points\n");
	fprintf(stderr, "\t1dc2\t1D Compact, 2-points\n");
	fprintf(stderr, "\t2d4\t2D Compact 4-star\n");
	fprintf(stderr, "\t2d9\t2D Compact 9-star\n");
	fprintf(stderr, "\t2dnc9\t2D Non-Compact, higher-order, 9-star\n");
	fprintf(stderr, "\t2df5\t2D Non-Compact, Forward (right-down), 5-points\n");
	fprintf(stderr, "\t3d27\t3D Compact, 27-star\n");
	fprintf(stderr, "\nAppend \"_\" before the stencilId (e.g: _2d4) to use an optimized implementation instead of a generic one.\n");
	fprintf(stderr, "\n");
}

/* MAIN: STENCIL PROGRAM, READ ARGUMENTS AND CALL THE PATTERN */
int main(int argc, char *argv[]) {
	if (argc < 4) {
		printUsage(argv);
		exit(EXIT_FAILURE);
	}

	float factor = 0;
	/* DATA OF DIFFERENT STENCIL COMPUTATIONS */
	HitShape shpSt_1dNC      = hitShape((-2, 2));
	HitShape shpSt_1dC       = hitShape((-1, 1));
	HitShape shpSt_2dCompact = hitShape((-1, 1), (-1, 1));
	HitShape shpSt_2dNC      = hitShape((-2, 2), (-2, 2));
	HitShape shpSt_2dF5      = hitShape((-2, 0), (-2, 0));
	HitShape shpSt_3dCompact = hitShape((-1, 1), (-1, 1), (-1, 1));

	float stencilData_1dNC4[] = {0.5, 1, 0, 1, 0.5};

	float stencilData_1dC2[] = {1, 0, 1};

	float stencilData_4[] = {
		0, 1, 0,
		1, 0, 1,
		0, 1, 0};

	float stencilData_9[] = {
		1, 4, 1,
		4, 0, 4,
		1, 4, 1};

	float stencilData_NC9[] = {
		0, 0, 1, 0, 0,
		0, 0, 4, 0, 0,
		1, 4, 0, 4, 1,
		0, 0, 4, 0, 0,
		0, 0, 1, 0, 0};

	float stencilData_F5[] = {
		0, 0, 1,
		0, 0.5, 2,
		1, 2, 0};

	// clang-format off
	float stencilData_27[] = {
		1, 1, 1,   1, 1, 1,   1, 1, 1,
		1, 1, 1,   1, 1, 1,   1, 1, 1,
		1, 1, 1,   1, 1, 1,   1, 1, 1};
	// clang-format on

	int d, dims, numIter;
	int sizes[3] = {0, 0, 0};
	dims         = argv[1][0] == '_' ? argv[1][1] - '0' : argv[1][0] - '0';

	// SELECT STENCIL
	char    *stencilType = argv[1][0] == '_' ? &argv[1][1] : argv[1];
	float   *stencilData;
	HitShape shpStencil;

	/* ARGUMENTS FOR EACH DIMENSION SIZE */
	for (d = 0; d < dims; d++)
		sizes[d] = atoi(argv[2 + d]);

	/* ARGUMENTS FOR ITERATIONS */
	numIter = atoi(argv[2 + dims]);

	/* Init communication system */
	hit_comInit(&argc, &argv);

	/* Stencil selection */
	stencilFunction f_stencil;
	if (!strcmp(stencilType, "1dnc4")) {
		shpStencil  = shpSt_1dNC;
		stencilData = stencilData_1dNC4;
		factor      = 3;
		f_stencil   = updateCell_1dNC4;
	} else if (!strcmp(stencilType, "1dc2")) {
		shpStencil  = shpSt_1dC;
		stencilData = stencilData_1dC2;
		factor      = 2;
		f_stencil   = updateCell_1dC2;
	} else if (!strcmp(stencilType, "2d4")) {
		shpStencil  = shpSt_2dCompact;
		stencilData = stencilData_4;
		factor      = 4;
		f_stencil   = updateCell_4;
	} else if (!strcmp(stencilType, "2d9")) {
		shpStencil  = shpSt_2dCompact;
		stencilData = stencilData_9;
		factor      = 20;
		f_stencil   = updateCell_9;
	} else if (!strcmp(stencilType, "2dnc9")) {
		shpStencil  = shpSt_2dNC;
		stencilData = stencilData_NC9;
		factor      = 20;
		f_stencil   = updateCell_NC9;
	} else if (!strcmp(stencilType, "2df5")) {
		shpStencil  = shpSt_2dF5;
		stencilData = stencilData_F5;
		factor      = 6.5;
		f_stencil   = updateCell_F5;
	} else if (!strcmp(stencilType, "3d27")) {
		shpStencil  = shpSt_3dCompact;
		stencilData = stencilData_27;
		factor      = 27;
		f_stencil   = updateCell_3d27;
	} else {
		fprintf(stderr, "\nError: Unknown stencil type %s\n", stencilType);
		printUsage(argv);
		exit(EXIT_FAILURE);
	}

	/* LAUNCH STENCIL COMPUTATION */
	if (argv[1][0] != '_') f_stencil = NULL;
	stencilComputation(sizes, shpStencil, stencilData, factor, numIter, f_stencil, initData, outputData);

	hit_comFinalize();

	return 0;
}
