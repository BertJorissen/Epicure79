/**
 * @file kernels_cpulib.c
 * @brief MatrixAdd: Using CPULIB interface for using third-party BLAS library (such as MKL)
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "kernels_params.h"

CTRL_KERNEL(Add, CPULIB, MKL, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C, {
	for (int k = 0; k < n_iter; k++) {
		cblas_saxpy(hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), 1, A.data, 1, C.data, 1);
		cblas_saxpy(hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), 1, B.data, 1, C.data, 1);
	}
});
