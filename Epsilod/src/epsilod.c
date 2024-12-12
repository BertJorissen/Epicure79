/**
 * @file epsilod.c
 * @brief Epsilod: Stencil code: Any dimensions, stencil as a pattern of weights. Data type: float
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <epsilod.h>

#define _PSS_SHP_INFO_
#define _PSS_TOPO_INFO_
#define _PSS_WEIGHTS_INFO_

/* A. KERNEL CHARACTERIZATIONS */
Ctrl_Thread INNER_CHAR[3] = {
	{.dims = 1, .i = 256, .j = 1, .k = 1},
	{.dims = 2, .i = 4, .j = 64, .k = 1},
	{.dims = 3, .i = 1, .j = 4, .k = 64}};

Ctrl_Thread CPU_INNER_CHAR[3] = {
	{.dims = 1, .i = 1024, .j = 1, .k = 1},
	{.dims = 2, .i = 4, .j = 1024, .k = 1},
	{.dims = 3, .i = 2, .j = 2, .k = 1024}};

Ctrl_Thread BORDER_CHAR[3][3] = {
	// 1D
	{{.dims = 1, .i = 256, .j = 1, .k = 1},
	 CTRL_THREAD_NULL,
	 CTRL_THREAD_NULL},
	// 2D
	{{.dims = 2, .i = 1, .j = 256, .k = 1},
	 {.dims = 2, .i = 256, .j = 1, .k = 1},
	 CTRL_THREAD_NULL},
	// 3D
	{{.dims = 3, .i = 1, .j = 4, .k = 64},
	 {.dims = 3, .i = 4, .j = 1, .k = 64},
	 {.dims = 3, .i = 4, .j = 64, .k = 1}}};

Ctrl_Thread CPU_BORDER_CHAR[3][3] = {
	// 1D
	{{.dims = 1, .i = 1024, .j = 1, .k = 1},
	 CTRL_THREAD_NULL,
	 CTRL_THREAD_NULL},
	// 2D
	{{.dims = 2, .i = 1, .j = 1024, .k = 1},
	 {.dims = 2, .i = 1024, .j = 1, .k = 1},
	 CTRL_THREAD_NULL},
	// 3D
	{{.dims = 3, .i = 1, .j = 1, .k = 1024},
	 {.dims = 3, .i = 1, .j = 1, .k = 1024},
	 {.dims = 3, .i = 1, .j = 1024, .k = 1}}};

/* B. GENERIC KERNEL PROTOTYPE AND WRAPPER LAUNCHERS */
CTRL_KERNEL_CHAR(updateCell_default_1D, MANUAL, 0, 0, 0);
CTRL_KERNEL_CHAR(updateCell_default_2D, MANUAL, 0, 0, 0);
CTRL_KERNEL_CHAR(updateCell_default_3D, MANUAL, 0, 0, 0);

CTRL_KERNEL_PROTO(updateCell_default_1D,
				  1, GENERIC, DEFAULT, 7,
				  OUT, HitTile_float, matrix,
				  IN, HitTile_float, matrixCopy,
				  IN, HitTile_float, weight,
				  INVAL, int, begin_x,
				  INVAL, int, end_x,
				  INVAL, float, factor,
				  INVAL, Epsilod_ext, ext_params);

void updateCell_default_1D(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor, Epsilod_ext *ext_params) {
	Ctrl_LaunchToStream(ctrl, updateCell_default_1D, threads, blockSize, stream, mat, copy, weight,
						hit_tileDimBegin(weight, 0), hit_tileDimEnd(weight, 0),
						factor, *ext_params);
}

CTRL_KERNEL_PROTO(updateCell_default_2D,
				  1, GENERIC, DEFAULT, 9,
				  OUT, HitTile_float, matrix,
				  IN, HitTile_float, matrixCopy,
				  IN, HitTile_float, weight,
				  INVAL, int, begin_x,
				  INVAL, int, begin_y,
				  INVAL, int, end_x,
				  INVAL, int, end_y,
				  INVAL, float, factor,
				  INVAL, Epsilod_ext, ext_params);

void updateCell_default_2D(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor, Epsilod_ext *ext_params) {
	Ctrl_LaunchToStream(ctrl, updateCell_default_2D, threads, blockSize, stream, mat, copy, weight,
						hit_tileDimBegin(weight, 0), hit_tileDimBegin(weight, 1),
						hit_tileDimEnd(weight, 0), hit_tileDimEnd(weight, 1),
						factor, *ext_params);
}

CTRL_KERNEL_PROTO(updateCell_default_3D,
				  1, GENERIC, DEFAULT, 11,
				  OUT, HitTile_float, matrix,
				  IN, HitTile_float, matrixCopy,
				  IN, HitTile_float, weight,
				  INVAL, int, begin_x,
				  INVAL, int, begin_y,
				  INVAL, int, begin_z,
				  INVAL, int, end_x,
				  INVAL, int, end_y,
				  INVAL, int, end_z,
				  INVAL, float, factor,
				  INVAL, Epsilod_ext, ext_params);

void updateCell_default_3D(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor, Epsilod_ext *ext_params) {
	Ctrl_LaunchToStream(ctrl, updateCell_default_3D, threads, blockSize, stream, mat, copy, weight,
						hit_tileDimBegin(weight, 0), hit_tileDimBegin(weight, 1), hit_tileDimBegin(weight, 2),
						hit_tileDimEnd(weight, 0), hit_tileDimEnd(weight, 1), hit_tileDimEnd(weight, 2),
						factor, *ext_params);
}

/* D. FALSE INITIALIZATION OF SELECTIONS TO AVOID NON-INITIALIZED WARNINGS */
CTRL_HOST_TASK(Ctrl_Sub_Select_Init, HitTile_float matrix) { return; }
CTRL_HOST_TASK_PROTO(Ctrl_Sub_Select_Init, 1, OUT, HitTile_float, matrix);

/* F. DEBUG: WRITE OUTPUT ON STDOUT */
CTRL_HOST_TASK(print_matrix, HitTile_float mat, Epsilod_ext *ext_params) {
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

CTRL_HOST_TASK_PROTO(print_matrix, 2, IN, HitTile_float, mat, INVAL, Epsilod_ext *, ext_params);

/* G. STENCIL PATTERN TRANSFERENCE HOST-TASK */
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
			fprintf(stderr, "[Parallel Stencil Skeleton] %d dims are not supported in the stencil definition, max. 3 dims\n", dims);
			exit(EXIT_FAILURE);
	}
}

CTRL_HOST_TASK_PROTO(Ctrl_Copy_Stencil, 2,
					 OUT, HitTile_float, stencil,
					 INVAL, float *, stencil_data);

/* H. EXPERIMENTATION: GLOBAL clocks */
HitClock mainClock;
HitClock initClock;
HitClock loopClock;
/// HitClock	redistributeClock;
HitClock commClock;

/* HELP. PRINT EXTRA OPTIONS OF EPSILOD USING ENVIRONMENT VARIABLES */
void epsilod_print_usage() {
	fprintf(stderr, "\nEPSILOD environment variable:\n");
	fprintf(stderr, "\tEPSILOD_PARTITION=r       Regular blocks of similar sizes on a multidimensional grid topology\n");
	fprintf(stderr, "\tEPSILOD_PARTITION=r<dim>  Regular blocks of similar sizes on a single dimension topology\n");
	fprintf(stderr, "\tEPSILOD_PARTITION=w<dim>  Weigthed block distribution in the single choosen dimension. Processes weigths are specified in the device selection configuratuion file.\n");
	fprintf(stderr, "\n");
}

/* I. STENCIL COMPUTATION FUNCTION: */
void stencilComputation(
	int                sizes[],
	HitShape           stencilShape,
	float              stencilData[],
	float              factor,
	int                numIterations,
	stencilFunction    f_updateCell,
	initDataFunction   f_init,
	outputDataFunction f_output,
	Epsilod_ext       *ext_params_arg,
	char              *device_selection_file) {

	/* EXTERNAL/EXTRA PARAMETERS: USE FOO VARIABLE FOR NULL ARGUMENT */
	Epsilod_ext  foo;
	Epsilod_ext *ext_params = (ext_params_arg == NULL) ? &foo : ext_params_arg;

	int dims = hit_shapeDims(stencilShape);

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
				fprintf(stderr, "[Parallel Stencil Skeleton ERROR] Stencil with invalid number of dimensions. The skeleton only supports 1D, 2D or 3D stencils.\n");
				fflush(stderr);
				exit(EXIT_FAILURE);
		}
	}

	/* INITIALIZE DEVICE Controllers */
	__ctrl_block__(device_selection_file) {
		int i, j, k;

		/* INIT CLOCKS */
		hit_clockSynchronizeAll();
		hit_clockStart(mainClock);
		hit_clockStart(initClock);

		PCtrl comm = Ctrl_Get(0);

		#ifndef _CTRL_EXP_MODE_
		Ctrl_Info info = Ctrl_GetInfo(comm);
		printf("MPI_Rank[%d] DEVICE: %s", hit_Rank, info.device_name);
		fflush(stdout);
		#endif // _CTRL_EXAMPLES_EXP_MODE_

		Ctrl_SetDependanceMode(comm, CTRL_MODE_EXPLICIT);

		/* 0. BUILD THE STENCIL TILE */
		HitTile_float stencil = Ctrl_Domain(float, stencilShape);

		Ctrl_Alloc(comm, stencil);
		Ctrl_WaitTile(comm, stencil);

		if (f_updateCell == updateCell_default_1D ||
			f_updateCell == updateCell_default_2D ||
			f_updateCell == updateCell_default_3D) {
			Ctrl_HostTask(Ctrl_Copy_Stencil, stencil, stencilData);
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

		/* 3.2. SELECT AND BUILD PARTITION/DISTRIBUTION */
		int   partition_dim = 0;
		bool  use_weights   = false;
		char *partition_str = getenv("EPSILOD_PARTITION");
		if (partition_str != NULL) {
			if (strlen(partition_str) > 2) {
				fprintf(stderr, "\nError in EPSILOD_PARTITION enviroment string: More than two characters. String: %s\n\n", partition_str);
				MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
				exit(EXIT_FAILURE);
			}
			switch (partition_str[0]) {
				case 'r':
				case 'R':
					break;
				case 'w':
				case 'W':
					use_weights = true;
					break;
				default:
					fprintf(stderr, "\nError in EPSILOD_PARTITION enviroment string: Unknown partition type. String: %s\n\n", partition_str);
					MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
					exit(EXIT_FAILURE);
			}
			switch (partition_str[1]) {
				case '\0':
					partition_dim = -1;
					break;
				case '0':
				case '1':
				case '2':
					partition_dim = partition_str[1] - '0';
					break;
				default:
					fprintf(stderr, "\nError in EPSILOD_PARTITION enviroment string: Optional dimension should be in the range [0:2]. String: %s \n\n", partition_str);
					MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
					exit(EXIT_FAILURE);
			}
		}
		if (partition_dim >= dims) {
			fprintf(stderr, "\nError in EPSILOD_PARTITION environment string: Dimension %d, should be in the range of [0:stencil_dimensions-1]. String: %s\n\n", partition_dim, partition_str);
			MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
			exit(EXIT_FAILURE);
		}

		HitTopology topo;
		HitLayout   lay;
		HitWeights  weights = Ctrl_ConfigWeights();

		// TOPOLOGY
		if (partition_dim != -1)
			topo = hit_topology(plug_topPlain);
		else {
			switch (dims) {
				case 1:
					topo = hit_topology(plug_topPlain);
					break;
				case 2:
					topo = hit_topology(plug_topArray2DComplete);
					break;
				case 3:
					topo = hit_topology(plug_topArray3D);
					break;
				default:
					fprintf(stderr, "\nError EPSILOD: Number of dimensions for processes topology should be in the range [1:3]. Current value:%d \n\n", dims);
					MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
					exit(EXIT_FAILURE);
			}
		}

		// WEIGHTED DISTRIBUTION
		if (use_weights) {
			lay = hit_layout(plug_layDimWeighted_Blocks, topo, shpInner, partition_dim, weights);
		}
		// REGULAR DISTRIBUTION
		else {
			lay = hit_layout(plug_layBlocks, topo, shpInner);
		}

		HitShape shpLayout = hit_layShape(lay);
		#ifdef _PSS_SHP_INFO_
		printf(", shpLayout: ");
		dumpShape(shpLayout);
		fflush(stdout);
		#endif // _SHP_DEV_INFO_

		#ifdef _PSS_TOPO_INFO_
		if (hit_Rank == 0) {
			printf("\nTOPOLOGY: topoDims(%d,%d,%d)\n",
				   hit_topDimCard(topo, 0),
				   hit_topDimCard(topo, 1),
				   hit_topDimCard(topo, 2));
			fflush(stdout);
		}
		#endif // _SHP_DEV_INFO_

		#ifdef _PSS_WEIGHTS_INFO_
		if (hit_Rank == 0) {
			printf("\nPartition weights = {");
			for (int i = 0; i < weights.num_procs; i++)
				printf(" %f,", weights.ratios[i]);
			printf("\b }\n");
			fflush(stdout);
		}
		#endif

		/* 4. ACTIVE PROCESSES */
		if (hit_layImActive(lay)) {

			// 4.1. STOP IF THERE IS NOT ENOUGH INNER DATA FOR A BORDER
			for (i = 0; i < dims; i++) {
				if (hit_shapeSigCard(shpLayout, i) < borderLow[i] ||
					hit_shapeSigCard(shpLayout, i) < borderHigh[i]) {
					if (hit_Rank == 0) {
						fprintf(stderr, "\nError: Not enough data after partition, too many processes in a topology axis\n\n");
						MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
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

			HitTile_float globalMat = Ctrl_Domain(float, shp);

			HitTile_float tileMat, tileCopy;

			tileMat  = Ctrl_Select(float, globalMat, shpExpanded, CTRL_SELECT_ARR_COORD); //-------------//select tileMat
			tileCopy = Ctrl_Select(float, globalMat, shpExpanded, CTRL_SELECT_ARR_COORD); //-------------//select tileCopy

			tileList[0] = &tileMat;
			copyList[0] = &tileCopy;

			Ctrl_Alloc(comm, tileMat);  //---------------------//alloc tileMat
			Ctrl_Alloc(comm, tileCopy); //---------------------//alloc tileCopy

			HitTile_float tileInnerLocal     = Ctrl_Select(float, tileMat, shpInnerLocal, CTRL_SELECT_ARR_COORD);
			HitTile_float tileCopyInnerLocal = Ctrl_Select(float, tileCopy, shpInnerLocal, CTRL_SELECT_ARR_COORD);

			tileList[1] = &tileInnerLocal;
			copyList[1] = &tileCopyInnerLocal;

			for (i = 0; i < numBorders; i++) {
				tileBorderIn[i]      = Ctrl_Select(float, tileMat, shpBorderIn[i], CTRL_SELECT_ARR_COORD);   //-------------------//select
				tileCopyBorderIn[i]  = Ctrl_Select(float, tileCopy, shpBorderIn[i], CTRL_SELECT_ARR_COORD);  //-------------------//select
				tileBorderOut[i]     = Ctrl_Select(float, tileMat, shpBorderOut[i], CTRL_SELECT_ARR_COORD);  //-------------------//select
				tileCopyBorderOut[i] = Ctrl_Select(float, tileCopy, shpBorderOut[i], CTRL_SELECT_ARR_COORD); //-------------------//select

				tileList[2 + 2 * i]     = &tileBorderIn[i];
				copyList[2 + 2 * i]     = &tileCopyBorderIn[i];
				tileList[2 + 2 * i + 1] = &tileBorderOut[i];
				copyList[2 + 2 * i + 1] = &tileCopyBorderOut[i];
			}

			// clang-format off
			
			#define validShape(s) (hit_shapeDims((s)) != (-1))
			// clang-format on

			/* ELIMINATE BORDERS EXCEPT IF THEY ARE GLOBAL FOR IO SELECTION */
			HitTile_float io_tile, io_copy;
			HitShape      io_shape = hit_tileShape(tileMat);
			for (i = 0; i < dims; i++) {
				/* DIM i FIRST BORDER IS NOT MINE */
				if (!hit_sigIn(hit_shapeSig(io_shape, i), hit_tileDimBegin(globalMat, i))) {
					io_shape = hit_shapeTransform(io_shape, i, HIT_SHAPE_BEGIN, borderLow[i]);
				}
				/* DIM i LAST BORDER IS NOT MINE */
				if (!hit_sigIn(hit_shapeSig(io_shape, i), hit_tileDimEnd(globalMat, i))) {
					io_shape = hit_shapeTransform(io_shape, i, HIT_SHAPE_END, -borderHigh[i]);
				}
			}
			hit_tileSelectArrayCoords(&io_tile, &tileMat, io_shape);
			hit_tileSelectArrayCoords(&io_copy, &tileCopy, io_shape);

			for (i = 0; i < dims; i++) {
				if (validShape(shpBorderOutDev[i][0])) {
					tileBorderOutDev[i][0]     = Ctrl_Select(float, tileMat, shpBorderOutDev[i][0], CTRL_SELECT_ARR_COORD);  //-------------------//select
					tileCopyBorderOutDev[i][0] = Ctrl_Select(float, tileCopy, shpBorderOutDev[i][0], CTRL_SELECT_ARR_COORD); //-------------------//select
				} else {
					tileBorderOutDev[i][0]     = *(HitTile_float *)&HIT_TILE_NULL;
					tileCopyBorderOutDev[i][0] = *(HitTile_float *)&HIT_TILE_NULL;
				}
				if (validShape(shpBorderOutDev[i][1])) {
					tileBorderOutDev[i][1]     = Ctrl_Select(float, tileMat, shpBorderOutDev[i][1], CTRL_SELECT_ARR_COORD);  //-------------------//select
					tileCopyBorderOutDev[i][1] = Ctrl_Select(float, tileCopy, shpBorderOutDev[i][1], CTRL_SELECT_ARR_COORD); //-------------------//select
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

				printf("[%d] Border (%d) active: %d\n", hit_Rank, i, borderInActive[i]);
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
			if (hit_Rank == 0) {
				printf("Init stage\n");
				fflush(stdout);
			}

			f_init(io_tile, dims, borderLow, borderHigh, *ext_params);

			char *omp_env     = getenv("OMP_NUM_THREADS");
			int   omp_threads = (omp_env != NULL) ? atoi(omp_env) : 1;
			#pragma omp parallel for num_threads(omp_threads)
			for (int i = 0; i < tileMat.acumCard; i++)
				hit(tileCopy, i) = hit(tileMat, i);

			/* 4.8.b. FALSE INITIALIZATION OF SUBSELECTIONS AND COPY, ELIMINATE WARNINGS */
			Ctrl_HostTask(Ctrl_Sub_Select_Init, tileMat);
			Ctrl_HostTask(Ctrl_Sub_Select_Init, tileCopy);
			for (i = 0; i < dims; i++) {
				if (validShape(tileBorderOutDev[i][0].shape)) {
					Ctrl_HostTask(Ctrl_Sub_Select_Init, tileBorderOutDev[i][0]);
				}
				if (validShape(tileBorderOutDev[i][1].shape)) {
					Ctrl_HostTask(Ctrl_Sub_Select_Init, tileBorderOutDev[i][1]);
				}
			}
			if (validShape(tileInnerLocal.shape)) {
				Ctrl_HostTask(Ctrl_Sub_Select_Init, tileInnerLocal);
			}
			for (i = 0; i < numBorders; i++) {
				// SKIP EMPTY BORDERS
				if (!borderInActive[i]) continue;
				Ctrl_HostTask(Ctrl_Sub_Select_Init, tileBorderIn[i]);
				Ctrl_HostTask(Ctrl_Sub_Select_Init, tileCopyBorderIn[i]);
			}

			/* Send tileMat to the device */
			Ctrl_MoveTo(comm, tileMat, tileCopy);
			Ctrl_WaitTile(comm, tileMat, tileCopy);
			// clang-format off
			
			#define copy2Threads( th, arr ) {th.i = arr[0]; th.j = arr[1]; th.k = arr[2];}
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
			Ctrl_Thread inner_char = comm->type == CTRL_TYPE_CPU ? CPU_INNER_CHAR[dims - 1] : INNER_CHAR[dims - 1];

			/* Borders characterization for CPUs */
			Ctrl_Thread *border_char = comm->type == CTRL_TYPE_CPU ? &CPU_BORDER_CHAR[dims - 1][0] : &BORDER_CHAR[dims - 1][0];

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

			if (hit_Rank == 0) {
				printf("Computation stage\n");
				fflush(stdout);
			}

			int loopIndex;
			for (loopIndex = 0; loopIndex < numIterations - 1; loopIndex++) {
				/* 4.8.1. UPDATE TILE COPY */
				if (validShape(tileCopy.shape) && validShape(tileMat.shape)) {
					swap(tileList, copyList, num_tiles_total);
				}
				/* 4.8.2. COMPUTE BORDERS */
				for (i = 0; i < dims; i++) {
					if (validShape(tileBorderOutDev[i][0].shape) && validShape(tileCopyBorderOutDev[i][0].shape)) {
						f_updateCell(comm, thrBorderOutDev[i][0], border_char[i], 2 * i, tileBorderOutDev[i][0], tileCopyBorderOutDev[i][0], stencil, factor, ext_params);
					}
					if (validShape(tileBorderOutDev[i][1].shape) && validShape(tileCopyBorderOutDev[i][1].shape)) {
						f_updateCell(comm, thrBorderOutDev[i][1], border_char[i], 2 * i + 1, tileBorderOutDev[i][1], tileCopyBorderOutDev[i][1], stencil, factor, ext_params);
					}
				}

				if (validShape(tileInnerLocal.shape) && validShape(tileCopyInnerLocal.shape)) {
					f_updateCell(comm, threadsInner, inner_char, 0, tileInnerLocal, tileCopyInnerLocal, stencil, factor, ext_params);
				}

				for (i = 0; i < dims; i++) {
					for (j = 0; j < 2; j++) {
						if (validShape(tileBorderOutDev[i][j].shape)) {
							Ctrl_MoveFrom(comm, tileBorderOutDev[i][j]);
						}
					}
					if (topo.type == HIT_TOPOLOGY_PLAIN) break;
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
					f_updateCell(comm, thrBorderOutDev[i][0], border_char[i], 2 * i, tileBorderOutDev[i][0], tileCopyBorderOutDev[i][0], stencil, factor, ext_params);
				}
				if (validShape(tileBorderOutDev[i][1].shape) && validShape(tileCopyBorderOutDev[i][1].shape)) {
					f_updateCell(comm, thrBorderOutDev[i][1], border_char[i], 2 * i + 1, tileBorderOutDev[i][1], tileCopyBorderOutDev[i][1], stencil, factor, ext_params);
				}
			}
			if (validShape(tileInnerLocal.shape) && validShape(tileCopyInnerLocal.shape)) {
				f_updateCell(comm, threadsInner, inner_char, 0, tileInnerLocal, tileCopyInnerLocal, stencil, factor, ext_params);
			}

			// Wait for kernels to finish:
			for (i = 0; i < dims; i++)
				for (j = 0; j < 2; j++)
					if (validShape(tileBorderOutDev[i][j].shape))
						Ctrl_WaitTile(comm, tileBorderOutDev[i][j]);
			Ctrl_WaitTile(comm, tileInnerLocal);

			if (hit_Rank == 0) {
				printf(" End\n");
				fflush(stdout);
			}

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
			fflush(stdout);

			/* 4.12. WRITE RESULT MATRIX */
			f_output(numIterations % 2 == 0 ? io_tile : io_copy, *ext_params);
			hit_patternFree(&neighSync);
			hit_patternFree(&neighSyncCopy);

			if (hit_Rank == 0) {
				printf("Output finished\n");
				fflush(stdout);
			}
		} // layactive

		/* 5. INACTIVE PROCESSES: ONLY COLLECTIVE CLOCK OPERATIONS */
		else {
			printf("[%d] Warning, process not active\n", hit_Rank);
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
		if (hit_Rank == 0) {
			printf("Free data structures\n");
			fflush(stdout);
		}
		hit_layFree(lay);
		hit_topFree(topo);
		free(borderLow);
		free(borderHigh);

		if (hit_Rank == 0) {
			printf("Stop distributed Controllers\n");
			fflush(stdout);
		}
		Ctrl_EndBlock();

		if (hit_Rank == 0) {
			printf("Epsilod End\n");
			fflush(stdout);
		}
	}
}
