/**
 * @file addKernel.h
 * @brief Controller add kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief add two vectors
///
/// GENERIC kernel
/// \param[in]  op1   term one
/// \param[in]  op2   term two
/// \param[out] sum   result
///////////////////////////////////////////////////////////////////////////////
CTRL_KERNEL(Add, GENERIC, DEFAULT, CTRL_KPARAMS(add_params), {
	hit(sum, thr_i, thr_j) = hit(op1, thr_i, thr_j) + hit(op2, thr_i, thr_j);
})