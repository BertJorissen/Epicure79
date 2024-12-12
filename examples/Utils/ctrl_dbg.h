/**
 * @file ctrl_dbg.h
 * @brief Helper utilities for debugging purposes of Ctrl programs.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"

/**
 * @brief write a piece of a matrix to a file, for debugging purposes
 */
CTRL_HOST_TASK(dbg_write_mat, HitTile_float mat, char *path, Ctrl_Thread th) {
	FILE *file = fopen(path, "w");
	if (file == NULL) {
		perror("Error opening file");
		return;
	}
	// int w = hit_tileDimCard(mat, 1);
	// int h = hit_tileDimCard(mat, 0);
	int w = th.j;
	int h = th.i;

	// Write the matrix dimensions first
	fprintf(file, "%d %d\n", h, w);

	// Write the matrix data
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			fprintf(file, "%.2f ", hit(mat, i, j));
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

CTRL_HOST_TASK_PROTO(dbg_write_mat, 3, IN, HitTile_float, mat, INVAL, char *, path, INVAL, Ctrl_Thread, th);