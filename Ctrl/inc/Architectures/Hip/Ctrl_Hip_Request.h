#ifndef _CTRL_HIP_REQUEST_H_
#define _CTRL_HIP_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_Hip_Request.h
 * @brief Specific HIP implementation for a request.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <hip/hip_runtime_api.h>

#ifdef _CTRL_HIPBLAS_
#include <hipblas/hipblas.h>
#endif // _CTRL_HIPBLAS_

/**
 * \brief HIP specific info needed to execute a kernel.
 */
typedef struct Ctrl_Hip_Request {
	#ifdef _CTRL_HIPBLAS_
	hipblasHandle_t *p_hipblas_handle; /**< HIPBLAS handle for HIPLIB_HIPBLAS type kernels */
	#endif // _CTRL_HIPBLAS_

	hipStream_t *p_stream; /**< HIP stream to launch the kernels to */
} Ctrl_Hip_Request;
///@endcond

#endif // _CTRL_HIP_REQUEST_H_
