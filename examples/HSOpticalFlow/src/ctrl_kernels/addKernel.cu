#include "kernels_params.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief add two vectors
///
/// CUDA kernel
/// \param[in]  op1   term one
/// \param[in]  op2   term two
/// \param[out] sum   result
///////////////////////////////////////////////////////////////////////////////
CTRL_KERNEL(Add, GENERIC, DEFAULT, CTRL_KPARAMS(add_params), {
	hit(sum, thr_i, thr_j) = hit(op1, thr_i, thr_j) + hit(op2, thr_i, thr_j);
})