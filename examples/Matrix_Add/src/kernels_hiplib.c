/**
 * @file kernels_hiplib.c
 * @brief MatrixAdd: Using HIPLIB interface for using third-party BLAS library: hipBLAS
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "kernels_params.h"

/* hipBLAS */
CTRL_KERNEL(Add, HIPLIB, HIPBLAS, CTRL_KPARAMS(madd_params), {
	const float alpha = 1.0f;
	for (int k = 0; k < n_iter; k++) {
		hipblasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, A.data, 1, C.data, 1);
		hipblasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, B.data, 1, C.data, 1);
	}
});
