/**
 * @file epsilod_kernels.cu
 * @brief Epsilod: Generic kernels for fully described 1D, 2D, 3D stencils.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "epsilod_ext_type.h"

#include "Ctrl.h"

Ctrl_NewType(float);

/* 1D CELL UPDATE DEFAULT STENCIL */
CTRL_KERNEL(updateCell_default_1D, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const KHitTile_float weight, const int begin_x, const int end_x, const float factor, const Epsilod_ext ext_params, {
	int x = thr_i;

	float sum = 0;

	int i;
	for (i = begin_x; i <= end_x; i++) {
		if (!hit(weight, i - begin_x)) continue;
		sum += hit(matrixCopy, x + i) * hit(weight, i - begin_x);
	}
	hit(matrix, x) = sum / factor;
});

/* 2D CELL UPDATE DEFAULT STENCIL */
CTRL_KERNEL(updateCell_default_2D, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const KHitTile_float weight, const int begin_x, const int begin_y, const int end_x, const int end_y, const float factor, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	float sum = 0;

	int i;
	int j;
	for (i = begin_x; i <= end_x; i++)
		for (j = begin_y; j <= end_y; j++) {
			if (!hit(weight, i - begin_x, j - begin_y)) continue;
			sum += hit(matrixCopy, x + i, y + j) * hit(weight, i - begin_x, j - begin_y);
		}
	hit(matrix, x, y) = sum / factor;
});

/* 3D CELL UPDATE DEFAULT STENCIL */
CTRL_KERNEL(updateCell_default_3D, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const KHitTile_float weight, const int begin_x, const int begin_y, const int begin_z, const int end_x, const int end_y, const int end_z, const float factor, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;
	int z = thr_k;

	float sum = 0;

	int i;
	int j;
	int k;
	for (i = begin_x; i <= end_x; i++)
		for (j = begin_y; j <= end_y; j++)
			for (k = begin_z; k <= end_z; k++) {
				if (!hit(weight, i - begin_x, j - begin_y, k - begin_z)) continue;
				sum += hit(matrixCopy, x + i, y + j, z + k) * hit(weight, i - begin_x, j - begin_y, k - begin_z);
			}
	hit(matrix, x, y, z) = sum / factor;
});
