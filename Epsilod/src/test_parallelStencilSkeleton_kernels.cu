/**
 * @file test_parallelStencilSkeleton_kernels.cu
 * @brief Epsilod: Example with several key stencils. Kernels code.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "epsilod_ext_type.h"

#include "Ctrl.h"

Ctrl_NewType(float);

/* A.1. 1D NON-COMPACT RADIUS 2 */
CTRL_KERNEL(updateCell_1dNC4, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;

	hit(matrix, x) = (0.5 * (hit(matrixCopy, x - 2) + hit(matrixCopy, x + 2)) +
					  hit(matrixCopy, x - 1) + hit(matrixCopy, x + 1)) /
					 3;
});

/* A.1. 1D COMPACT RADIUS 1 */
CTRL_KERNEL(updateCell_1dC2, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;

	hit(matrix, x) = (hit(matrixCopy, x - 1) + hit(matrixCopy, x + 1)) / 2;
});

/* B.1. 2D COMPACT, RADIUS 1: 4-POINT STAR, NO CORNERS */
CTRL_KERNEL(updateCell_4, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = (hit(matrixCopy, x - 1, y) +
						 hit(matrixCopy, x + 1, y) +
						 hit(matrixCopy, x, y - 1) +
						 hit(matrixCopy, x, y + 1)) /
						4;
});

/* B.2. 2D COMPACT, RADIUS 1: 9-POINT STAR, CORNERS INCLUDED */
CTRL_KERNEL(updateCell_9, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = (4 * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x + 1, y) + hit(matrixCopy, x, y - 1) + hit(matrixCopy, x, y + 1)) +
						 (hit(matrixCopy, x - 1, y - 1) + hit(matrixCopy, x + 1, y - 1) + hit(matrixCopy, x - 1, y + 1) + hit(matrixCopy, x + 1, y + 1))) /
						20;
});

/* B.3. 2D NON-COMPACT, RADIUS 2: 9-POINT STAR, NO CORNERS */
CTRL_KERNEL(updateCell_NC9, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = ((hit(matrixCopy, x - 2, y) + hit(matrixCopy, x + 2, y) + hit(matrixCopy, x, y - 2) + hit(matrixCopy, x, y + 2)) +
						 4 * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x + 1, y) + hit(matrixCopy, x, y - 1) + hit(matrixCopy, x, y + 1))) /
						20;
});

/* B.3. 2D NON-COMPACT, NON-SYMMETRIC. RADIUS 2: 5-POINT STAR
 * FORWARD-DOWN WITH ONE CORNER ELEMENT */
CTRL_KERNEL(updateCell_F5, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = (2.0f * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x, y - 1)) +
						 (hit(matrixCopy, x - 2, y) + hit(matrixCopy, x, y - 2)) +
						 .5f * hit(matrixCopy, x - 1, y - 1)) /
						6.5f;
});

/* C.1. 3D COMPACT. RADIUS 1: 27-POINT STAR */
CTRL_KERNEL(updateCell_3d27, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;
	int z = thr_k;

	hit(matrix, x, y, z) = (hit(matrixCopy, x - 1, y - 1, z - 1) +
							hit(matrixCopy, x - 1, y - 1, z) +
							hit(matrixCopy, x - 1, y - 1, z + 1) +
							hit(matrixCopy, x - 1, y, z - 1) +
							hit(matrixCopy, x - 1, y, z) +
							hit(matrixCopy, x - 1, y, z + 1) +
							hit(matrixCopy, x - 1, y + 1, z - 1) +
							hit(matrixCopy, x - 1, y + 1, z) +
							hit(matrixCopy, x - 1, y + 1, z + 1) +
							hit(matrixCopy, x, y - 1, z - 1) +
							hit(matrixCopy, x, y - 1, z) +
							hit(matrixCopy, x, y - 1, z + 1) +
							hit(matrixCopy, x, y, z - 1) +
							hit(matrixCopy, x, y, z) +
							hit(matrixCopy, x, y, z + 1) +
							hit(matrixCopy, x, y + 1, z - 1) +
							hit(matrixCopy, x, y + 1, z) +
							hit(matrixCopy, x, y + 1, z + 1) +
							hit(matrixCopy, x + 1, y - 1, z - 1) +
							hit(matrixCopy, x + 1, y - 1, z) +
							hit(matrixCopy, x + 1, y - 1, z + 1) +
							hit(matrixCopy, x + 1, y, z - 1) +
							hit(matrixCopy, x + 1, y, z) +
							hit(matrixCopy, x + 1, y, z + 1) +
							hit(matrixCopy, x + 1, y + 1, z - 1) +
							hit(matrixCopy, x + 1, y + 1, z) +
							hit(matrixCopy, x + 1, y + 1, z + 1)) /
						   27;
});
