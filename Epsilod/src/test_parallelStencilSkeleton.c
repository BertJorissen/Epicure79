/**
 * @file test_parallelStencilSkeleton.c
 * @brief Epsilod: Example with several key stencils. Data type: float
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <epsilod.h>

// GLOBAL VARIABLES FOR INPUT/OUPUT FILE OPTIONS
int io_read_input   = 0;
int io_write_input  = 0;
int io_write_output = 0;

/* A. PROTOYPES */
/* A.1. ARRAY INIT AND OUTPUT FUNCTIONS */
void initData(HitTile_float io_tile, int dims, int borderLow[], int borderHigh[], Epsilod_ext ext_params);
void outputData(HitTile_float io_tile, Epsilod_ext ext_params);

/* A.2. INITIALIZATION FUNCTIONS */
void initData1D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);
void initData2D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);
void initData3D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]);

/* B. INITIALIZE ARRAYs */
/* B.1. INIT ARRAY: FILL WITH 0s, CALL FUNCTION TO INIT BORDERS 1, 2 or 3 DIMENSIONS */
void initData(HitTile_float io_tile, int dims, int borderLow[], int borderHigh[], Epsilod_ext ext_params) {
	/* 0. OPTIONAL COMPILATION: READING THE INPUT MATRIX FROM A FILE */
	if (io_read_input) {
		hit_tileFileReadOptions(&io_tile, "Matrix.in", NULL, HIT_FILE_RUNTIME, HIT_FILE_RUNTIME, io_read_input - 1, HIT_FILE_RUNTIME, HIT_FILE_FLOAT, HIT_FILE_RUNTIME, HIT_FILE_RUNTIME);
	} else {
		HitTile_float tileMat = *(HitTile_float *)hit_tileMemoryAncestor(&io_tile);

		char *omp_env     = getenv("OMP_NUM_THREADS");
		int   omp_threads = (omp_env != NULL) ? atoi(omp_env) : 1;
		#pragma omp parallel for num_threads(omp_threads)
		for (int i = 0; i < tileMat.acumCard; i++) {
			hit(tileMat, i) = 0;
		}

		/* 2. INIT BORDERS */
		switch (dims) {
			case 1: initData1D(tileMat, dims, borderLow, borderHigh); break;
			case 2: initData2D(tileMat, dims, borderLow, borderHigh); break;
			case 3: initData3D(tileMat, dims, borderLow, borderHigh); break;
			default:
				fprintf(stderr, "Error: This init function only works for 1, 2, or 3 dimensions\n");
				Ctrl_Finalize();
				exit(EXIT_FAILURE);
		}
	}

	/* 4. WRITE MAT TO A FILE (DEBUGGING) */
	if (io_write_input) {
		hit_tileFileWriteOptions(&io_tile, "Matrix.copy", NULL, HIT_FILE_RUNTIME, HIT_FILE_RUNTIME, io_write_input - 1, HIT_FILE_RUNTIME, HIT_FILE_FLOAT, HIT_FILE_RUNTIME, HIT_FILE_RUNTIME);
	}
}

/* B.2. INITIALIZE BORDERS ARRAY 1D */
void initData1D(HitTile_float tileMat, int dims, int borderLow[], int borderHigh[]) {
	/* 2. INIT BORDERS  DOWN(i)=1, UP(i)=2. */
	HitTile root = *hit_tileRoot(&tileMat);
	int     i;

	/* 2.1. FIRST ELEMENTS ARE MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimBegin(root, 0))) {
		for (i = 0; i < borderLow[0]; i++) {
			hit_tileElemAt(tileMat, 1, i) = 1;
		}
	}

	/* 2.4. LAST ELEMENTS ARE MINE */
	if (hit_sigIn(hit_tileDimSig(tileMat, 0), hit_tileDimEnd(root, 0))) {
		for (i = 0; i < borderHigh[0]; i++) {
			hit_tileElemAt(tileMat, 1, hit_tileDimCard(tileMat, 0) - 1 - i) = 2;
		}
	}
}

/* B.3. INITIALIZE BORDERS MATRIX 2D */
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

/* B.4. INITIALIZE BORDERS MATRIX 3D */
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

/* C. WRITE RESULTS */
void outputData(HitTile_float io_tile, Epsilod_ext ext_params) {
	if (io_write_output) {
		hit_tileFileWriteOptions(&io_tile, "Matrix.out", NULL, HIT_FILE_RUNTIME, HIT_FILE_RUNTIME, io_write_output - 1, HIT_FILE_RUNTIME, HIT_FILE_FLOAT, HIT_FILE_RUNTIME, HIT_FILE_RUNTIME);
	}
}

/* D. DECLARATIONS OF OPTIMIZED STENCIL KERNELS:
 * SEE test_parallelStencilSkeleton_kernels.c FILE */
REGISTER_STENCIL(updateCell_1dNC4, GENERIC, DEFAULT);
REGISTER_STENCIL(updateCell_1dC2, GENERIC, DEFAULT);
REGISTER_STENCIL(updateCell_4, GENERIC, DEFAULT);
REGISTER_STENCIL(updateCell_9, GENERIC, DEFAULT);
REGISTER_STENCIL(updateCell_NC9, GENERIC, DEFAULT);
REGISTER_STENCIL(updateCell_F5, GENERIC, DEFAULT);
REGISTER_STENCIL(updateCell_3d27, GENERIC, DEFAULT);

/* HELP: PRINT ARGUMENT USAGE */
void print_usage(char *argv[]) {
	if (hit_Rank == 0) {
		fprintf(stderr, "\n=== DISTRIBUTED MULTI-GPU STENCIL COMPUTATION EXAMPLE ===\n");
		fprintf(stderr, "\nUsage: %s <stencilId> <size0> [ <size1> [ <size2> ] ] <numIterations> <device_selection_file>\n", argv[0]);
		fprintf(stderr, "\t1dnc4\t1D Non-Compact, 4-points\n");
		fprintf(stderr, "\t1dc2\t1D Compact, 2-points\n");
		fprintf(stderr, "\t2d4\t2D Compact 4-star\n");
		fprintf(stderr, "\t2d9\t2D Compact 9-star\n");
		fprintf(stderr, "\t2dnc9\t2D Non-Compact, higher-order, 9-star\n");
		fprintf(stderr, "\t2df5\t2D Non-Compact, Forward (right-down), 5-points\n");
		fprintf(stderr, "\t3d27\t3D Compact, 27-star\n");
		fprintf(stderr, "\tAppend \"_\" before the stencilId (e.g: _2d4) to use an optimized implementation instead of a generic one.\n");
		fprintf(stderr, "\nEnvironment variable:\n");
		fprintf(stderr, "\tTEST_EPSILOD_READ_INPUT Read input from file Matrix.in\n");
		fprintf(stderr, "\tTEST_EPSILOD_WRITE_OUTPUT Write output to file Matrix.out\n");
		fprintf(stderr, "\tTEST_EPSILOD_WRITE_INPUT Wite input to file Matrix.copy\n");
		epsilod_print_usage();
		hit_filePrintUsage();
		fprintf(stderr, "\n");
	}
}

/* MAIN: STENCIL PROGRAM, READ ARGUMENTS AND CALL THE PATTERN */
int main(int argc, char *argv[]) {

	/* EXAMPLE STENCIL DECLARATIONS */
	/* RADIUS OF THE CHOSEN STENCIL EXAMPLES */
	HitShape shpSt_1dNC      = hitShape((-2, 2));
	HitShape shpSt_1dC       = hitShape((-1, 1));
	HitShape shpSt_2dCompact = hitShape((-1, 1), (-1, 1));
	HitShape shpSt_2dNC      = hitShape((-2, 2), (-2, 2));
	HitShape shpSt_2dF5      = hitShape((-2, 0), (-2, 0));
	HitShape shpSt_3dCompact = hitShape((-1, 1), (-1, 1), (-1, 1));

	/* WEIGHTS OF THE CHOSEN STENCIL EXAMPLES */
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

	/* Init communication system */
	Ctrl_Init(&argc, &argv);

	/* Check program arguments number */
	if (argc < 5 || argc > 7) {
		print_usage(argv);
		exit(EXIT_FAILURE);
	}

	/* READ ARGUMENTS */
	int d, dims, numIter;
	int sizes[3] = {0, 0, 0};
	dims         = argv[1][0] == '_' ? argv[1][1] - '0' : argv[1][0] - '0';
	if (dims < 0 || dims > 3) {
		fprintf(stderr, "Error: Non-supported number of dimensions: %d, should be in the range [1:3]\n\n", dims);
		Ctrl_Finalize();
		exit(EXIT_FAILURE);
	}

	// ARGUMENT TO SELECT STENCIL EXAMPLE
	char *stencilType = argv[1][0] == '_' ? &argv[1][1] : argv[1];

	/* ARGUMENTS FOR EACH DIMENSION SIZE */
	for (d = 0; d < dims; d++)
		sizes[d] = atoi(argv[2 + d]);

	/* ARGUMENT FOR ITERATIONS */
	numIter = atoi(argv[2 + dims]);

	/* ARGUMENT FOR CONFIG FILE */
	char *device_selection_file = argv[3 + dims];

	/* STENCIL SELECTION */
	HitShape        shpStencil;
	float          *stencilData;
	float           factor = 0;
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
		print_usage(argv);
		Ctrl_Finalize();
		exit(EXIT_FAILURE);
	}

	/* SKIP SPECIFIC KERNEL WHEN THE USER REQUIRES THE USE OF GENERIC KERNEL */
	if (argv[1][0] != '_') f_stencil = NULL;

	/* READ ENV VARIABLES */
	const char *io_options[] = {"none", "array", "tile", NULL};
	io_read_input            = hit_envOptions("TEST_EPSILOD_READ_INPUT", io_options);
	io_write_output          = hit_envOptions("TEST_EPSILOD_WRITE_OUTPUT", io_options);
	io_write_input           = hit_envOptions("TEST_EPSILOD_WRITE_INPUT", io_options);

	/* LAUNCH STENCIL COMPUTATION */
	stencilComputation(sizes, shpStencil, stencilData, factor, numIter, f_stencil, initData, outputData, NULL, device_selection_file);

	/* END */
	Ctrl_Finalize();
	return 0;
}
