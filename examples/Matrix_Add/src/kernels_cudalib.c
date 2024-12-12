/**
 * @file kernels_cudalib.c
 * @brief MatrixAdd: Using CUDALIB interface for using third-party BLAS library: cuBLAS
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "kernels_params.h"

/* MAGMA */
CTRL_KERNEL(Add, CUDALIB, MAGMA, CTRL_KPARAMS(madd_params), {
	for (int k = 0; k < n_iter; k++) {
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, A.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, B.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
	}
});

/* cuBLAS */
CTRL_KERNEL(Add, CUDALIB, CUBLAS, CTRL_KPARAMS(madd_params), {
	const float alpha = 1.0f;
	for (int k = 0; k < n_iter; k++) {
		cublasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, A.data, 1, C.data, 1);
		cublasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, B.data, 1, C.data, 1);
	}
});
