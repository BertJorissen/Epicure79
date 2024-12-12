#ifndef _CTRL_CUDA_REQUEST_H_
#define _CTRL_CUDA_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cuda_Request.h
 * @brief Specific CUDA implementation for a request.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <cuda_runtime_api.h>

#ifdef _CTRL_CUBLAS_
#include <cublas_v2.h>
#endif // _CTRL_CUBLAS_

#ifdef _CTRL_MAGMA_
#include "magma_lapack.h"
#include "magma_v2.h"
#endif //_CTRL_MAGMA_

/**
 * \brief CUDA specific info needed to execute a kernel.
 */
typedef struct Ctrl_Cuda_Request {
	#ifdef _CTRL_CUBLAS_
	cublasHandle_t *p_cublas_handle; /**< CUBLAS handle for CUDALIB_CUBLAS type kernels */
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	magma_queue_t *p_magma_queue; /**< MAGMA queue for CUDALIB_MAGMA type kernels */
	#endif // _CTRL_MAGMA_

	cudaStream_t *p_stream; /**< CUDA stream to launch the kernels to */
} Ctrl_Cuda_Request;
///@endcond

#endif // _CTRL_CUDA_REQUEST_H_
