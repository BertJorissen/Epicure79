/**
 * @file kernels_generic (.c/.cu)
 * @brief MatrixAdd: Generic kernel for different architectures
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "kernels_params.h"

CTRL_KERNEL(Add, GENERIC, DEFAULT, CTRL_KPARAMS(madd_params), {
	for (int k = 0; k < n_iter; k++) {
		hit(C, thr_i, thr_j) =
			hit_as(A, C, thr_i, thr_j) +
			hit_as(B, C, thr_i, thr_j) +
			hit(C, thr_i, thr_j);
	}
});
