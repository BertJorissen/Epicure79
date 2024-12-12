/**
 * @file SubselecTest_Ctrl.c
 * @brief SubselecTest: Ctrl version
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"
#include "../../examples/Utils/ctrl_print_info.h"
#include "kernels_params.h"

Ctrl_NewType(long);

// #define STRIDE

CTRL_KERNEL_CHAR(some_kernel, MANUAL, 16, 16);
CTRL_KERNEL_CHAR(big_kernel, MANUAL, 16, 16);

CTRL_KERNEL(some_kernel, GENERIC, DEFAULT, CTRL_KPARAMS(some_kernel_params), {
	hit(tile, thr_i, thr_j) *= hit(tile, thr_i, thr_j);
	// hitStrided(tile, thr_i, thr_j) *= hitStrided(tile, thr_i, thr_j);
});

CTRL_KERNEL(big_kernel, GENERIC, DEFAULT, CTRL_KPARAMS(big_kernel_params), {
	hit(tile1, thr_i, thr_j) *= hit(tile1, thr_i, thr_j);
	hit(tile2, thr_j, thr_i) = -hit(tile2, thr_j, thr_i);
	hit(tile3, thr_i, thr_j) = 0;
	hit(tile4, thr_j, thr_i) %= 2;
	hit(tile5, thr_i, thr_j) = hit(tile6, thr_j, thr_i);
});

CTRL_HOST_TASK(init_tile, HitTile_long tile) {
	for (int i = 0; i < hit_tileDimCard(tile, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(tile, 1); j++) {
			hit(tile, i, j) = i * hit_tileDimCard(tile, 1) + j;
		}
	}
}

CTRL_HOST_TASK(print_matrix, HitTile_long tile) {
	for (int i = 0; i < hit_tileDimCard(tile, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(tile, 1); j++) {
			printf("%5ld ", hit(tile, i, j));
		}
		printf("\n");
	}
	printf("\n");
	fflush(stdout);
}

CTRL_KERNEL_PROTO(some_kernel, 2, GENERIC, DEFAULT, FPGA, DEFAULT, some_kernel_params);
CTRL_KERNEL_PROTO(big_kernel, 2, GENERIC, DEFAULT, FPGA, DEFAULT, big_kernel_params);

CTRL_HOST_TASK_PROTO(init_tile, 1, OUT, HitTile_long, tile);
CTRL_HOST_TASK_PROTO(print_matrix, 1, IN, HitTile_long, tile);

int main(int argc, char *argv[]) {
	Ctrl_Init(&argc, &argv);

	if (argc != 3) {
		fprintf(stderr, "\nUsage: %s <size> <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int   SIZE           = atoi(argv[1]);
	char *ctrl_conf_file = argv[2];

	Ctrl_Thread subselec_whole_threads, subselec_rows_threads, subselec_cols_threads, subselec_submatrix_threads;
	Ctrl_ThreadInit(subselec_whole_threads, SIZE, SIZE);
	Ctrl_ThreadInit(subselec_rows_threads, SIZE / 4, SIZE);
	Ctrl_ThreadInit(subselec_cols_threads, SIZE, SIZE / 4);
	Ctrl_ThreadInit(subselec_submatrix_threads, SIZE / 2, SIZE / 2);

	#ifdef STRIDE
	Ctrl_Thread subselec_stride_threads, subselec_corners_threads, subselec_null_threads;
	Ctrl_ThreadInit(subselec_stride_threads, SIZE / 2, SIZE / 2);
	Ctrl_ThreadInit(subselec_corners_threads, 2, 2);
	Ctrl_ThreadInit(subselec_null_threads, hit_shapeSigCard(HIT_SHAPE_NULL, 0), hit_shapeSigCard(HIT_SHAPE_NULL, 1));
	#endif // STRIDE

	Ctrl_Thread subselec_phantom_matrix_threads1, subselec_phantom_matrix_threads2;
	Ctrl_ThreadInit(subselec_phantom_matrix_threads1, SIZE / 2, SIZE);
	Ctrl_ThreadInit(subselec_phantom_matrix_threads2, SIZE, SIZE / 2);

	__ctrl_block__(ctrl_conf_file) {
		PCtrl ctrl = Ctrl_Get(0);
		printf("\n ----------------------- ARGS ----------------------- \n");
		printf("\n SIZE: %d", SIZE);
		Ctrl_PrintInfo();
		printf("\n\n ---------------------------------------------------- \n");
		fflush(stdout);

		Ctrl_SetDependanceMode(ctrl, CTRL_MODE_EXPLICIT);

		HitTile_long matrix, subselec_whole, subselec_rows, subselec_cols, subselec_submatrix;
		HitTile_long phantom_matrix, subselec_phantom_matrix1, subselec_phantom_matrix2;

		HitShape matrix_shape             = hitShapeSize(SIZE, SIZE);
		HitShape subselec_whole_shape     = HIT_SHAPE_WHOLE;
		HitShape subselec_rows_shape      = hitShapeSize(SIZE / 4, SIZE);
		HitShape subselec_cols_shape      = hitShapeSize(SIZE, SIZE / 4);
		HitShape subselec_submatrix_shape = hitShape((SIZE / 4, 3 * SIZE / 4 - 1, 1), (SIZE / 4, 3 * SIZE / 4 - 1, 1));

		#ifdef STRIDE
		HitShape subselec_stride_shape  = hitShape((0, SIZE - 1, 2), (0, SIZE - 1, 2));
		HitShape subselec_corners_shape = hitShape((0, SIZE - 1, SIZE - 1), (0, SIZE - 1, SIZE - 1));
		HitShape subselec_null_shape    = HIT_SHAPE_NULL;
		#endif // STRIDE

		HitShape subselec_phantom_matrix1_shape = hitShapeSize(SIZE / 2, SIZE);
		HitShape subselec_phantom_matrix2_shape = hitShapeSize(SIZE, SIZE / 2);

		matrix = Ctrl_Domain(long, matrix_shape);
		Ctrl_Alloc(ctrl, matrix);

		subselec_whole     = Ctrl_Select(long, matrix, subselec_whole_shape, CTRL_SELECT_DEFAULT);
		subselec_rows      = Ctrl_Select(long, matrix, subselec_rows_shape, CTRL_SELECT_DEFAULT);
		subselec_cols      = Ctrl_Select(long, matrix, subselec_cols_shape, CTRL_SELECT_DEFAULT);
		subselec_submatrix = Ctrl_Select(long, matrix, subselec_submatrix_shape, CTRL_SELECT_DEFAULT);

		#ifdef STRIDE
		HitTile_long subselec_stride, subselec_corners, subselec_null;
		subselec_stride  = Ctrl_Select(long, matrix, subselec_stride_shape, CTRL_SELECT_DEFAULT);
		subselec_corners = Ctrl_Select(long, matrix, subselec_corners_shape, CTRL_SELECT_DEFAULT);
		subselec_null    = Ctrl_Select(long, matrix, subselec_null_shape, CTRL_SELECT_DEFAULT);
		#endif // STRIDE

		phantom_matrix = Ctrl_Domain(long, matrix_shape);

		subselec_phantom_matrix1 = Ctrl_Select(long, phantom_matrix, subselec_phantom_matrix1_shape, CTRL_SELECT_DEFAULT);
		subselec_phantom_matrix2 = Ctrl_Select(long, phantom_matrix, subselec_phantom_matrix2_shape, CTRL_SELECT_DEFAULT);

		Ctrl_Alloc(ctrl, subselec_phantom_matrix1);
		Ctrl_Alloc(ctrl, subselec_phantom_matrix2);

		Ctrl_HostTask(init_tile, matrix);

		printf("=== ORIGINAL MATRIX ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_whole_threads, CTRL_THREAD_NULL, subselec_whole);
		Ctrl_MoveFrom(ctrl, subselec_whole);
		Ctrl_WaitTile(ctrl, subselec_whole);

		printf("=== WHOLE SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_HostTask(init_tile, matrix);
		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_rows_threads, CTRL_THREAD_NULL, subselec_rows);
		Ctrl_MoveFrom(ctrl, subselec_rows);
		Ctrl_WaitTile(ctrl, subselec_rows);

		printf("=== ROWS SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_HostTask(init_tile, matrix);
		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_cols_threads, CTRL_THREAD_NULL, subselec_cols);
		Ctrl_MoveFrom(ctrl, subselec_cols);
		Ctrl_WaitTile(ctrl, subselec_cols);

		printf("=== COLUMNS SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_HostTask(init_tile, matrix);
		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_submatrix_threads, CTRL_THREAD_NULL, subselec_submatrix);
		Ctrl_MoveFrom(ctrl, subselec_submatrix);
		Ctrl_WaitTile(ctrl, subselec_submatrix);

		printf("=== SUBMATRIX SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		#ifdef STRIDE
		Ctrl_HostTask(init_tile, matrix);
		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_stride_threads, CTRL_THREAD_NULL, subselec_stride);
		Ctrl_MoveFrom(ctrl, subselec_stride);
		Ctrl_WaitTile(ctrl, subselec_stride);

		printf("=== STRIDED SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_HostTask(init_tile, matrix);
		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_corners_threads, CTRL_THREAD_NULL, subselec_corners);
		Ctrl_MoveFrom(ctrl, subselec_corners);
		Ctrl_WaitTile(ctrl, subselec_corners);

		printf("=== CORNERS SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_HostTask(init_tile, matrix);
		Ctrl_MoveTo(ctrl, matrix);
		Ctrl_WaitTile(ctrl, matrix);

		Ctrl_Launch(ctrl, some_kernel, subselec_null_threads, CTRL_THREAD_NULL, subselec_null);
		Ctrl_MoveFrom(ctrl, subselec_null);
		Ctrl_WaitTile(ctrl, subselec_null);

		printf("=== NULL SUBSELECTION MODIFIED ===\n\n");
		Ctrl_HostTask(print_matrix, matrix);
		Ctrl_GlobalSync(ctrl);

		Ctrl_Free(ctrl, subselec_stride, subselec_corners, subselec_null);
		#endif // STRIDE

		Ctrl_HostTask(init_tile, subselec_phantom_matrix1);
		Ctrl_HostTask(init_tile, subselec_phantom_matrix2);

		Ctrl_MoveTo(ctrl, subselec_phantom_matrix1);
		Ctrl_Launch(ctrl, some_kernel, subselec_phantom_matrix_threads1, CTRL_THREAD_NULL, subselec_phantom_matrix1);
		Ctrl_MoveFrom(ctrl, subselec_phantom_matrix1);
		Ctrl_MoveTo(ctrl, subselec_phantom_matrix2);
		Ctrl_Launch(ctrl, some_kernel, subselec_phantom_matrix_threads2, CTRL_THREAD_NULL, subselec_phantom_matrix2);
		Ctrl_MoveFrom(ctrl, subselec_phantom_matrix2);

		printf("=== GHOST MATRIX ===\n\n");
		Ctrl_HostTask(print_matrix, subselec_phantom_matrix1);
		Ctrl_HostTask(print_matrix, subselec_phantom_matrix2);
		Ctrl_GlobalSync(ctrl);

		Ctrl_HostTask(init_tile, subselec_phantom_matrix1);
		Ctrl_HostTask(init_tile, subselec_phantom_matrix2);

		HitTile_long subselec_phantom_matrix11, subselec_phantom_matrix12, subselec_phantom_matrix21, subselec_phantom_matrix22;

		subselec_phantom_matrix11 = Ctrl_Select(long, subselec_phantom_matrix1, hitShapeSize(1, SIZE), CTRL_SELECT_DEFAULT);
		subselec_phantom_matrix12 = Ctrl_Select(long, subselec_phantom_matrix1, hitShapeSize(SIZE / 2, 1), CTRL_SELECT_DEFAULT);
		subselec_phantom_matrix21 = Ctrl_Select(long, subselec_phantom_matrix2, hitShapeSize(1, SIZE / 2), CTRL_SELECT_DEFAULT);
		subselec_phantom_matrix22 = Ctrl_Select(long, subselec_phantom_matrix2, hitShapeSize(SIZE, 1), CTRL_SELECT_DEFAULT);

		Ctrl_MoveTo(ctrl, subselec_phantom_matrix1);
		Ctrl_MoveTo(ctrl, subselec_phantom_matrix2);
		Ctrl_Launch(ctrl, big_kernel, subselec_phantom_matrix_threads1, CTRL_THREAD_NULL,
					subselec_phantom_matrix1, subselec_phantom_matrix2, subselec_phantom_matrix11,
					subselec_phantom_matrix21, subselec_phantom_matrix12, subselec_phantom_matrix22,
					subselec_phantom_matrix1, subselec_phantom_matrix2, subselec_phantom_matrix1,
					subselec_phantom_matrix2, subselec_phantom_matrix1, subselec_phantom_matrix2);
		Ctrl_MoveFrom(ctrl, subselec_phantom_matrix1);
		Ctrl_MoveFrom(ctrl, subselec_phantom_matrix2);

		printf("=== BIG KERNEL ===\n\n");
		Ctrl_HostTask(print_matrix, subselec_phantom_matrix1);
		Ctrl_HostTask(print_matrix, subselec_phantom_matrix2);
		Ctrl_GlobalSync(ctrl);

		printf("Phantom matrix: %p\n First half: %p (%p)\n Second half %p (%p)\n",
			   phantom_matrix.data,
			   subselec_phantom_matrix1.data, subselec_phantom_matrix1.memPtr,
			   subselec_phantom_matrix2.data, subselec_phantom_matrix2.memPtr);

		Ctrl_Free(NULL, matrix, subselec_whole, subselec_rows, subselec_cols, subselec_submatrix);
		Ctrl_Free(NULL, phantom_matrix, subselec_phantom_matrix1, subselec_phantom_matrix2);
		Ctrl_EndBlock();
	}
	Ctrl_Finalize();

	return EXIT_SUCCESS;
}
