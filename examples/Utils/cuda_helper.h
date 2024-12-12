#ifndef _CTRL_CUDA_UTILS_H_
#define _CTRL_CUDA_UTILS_H_
///@cond INTERNAL
/**
 * @file cuda_helper.h
 * @brief Macros for error checking on CUDA operations when debugging.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifdef _CTRL_CUDA_ERROR_CHECK_
#include <cuda_runtime_api.h>

#define CUDA_ERROR()                                                                  \
	{                                                                                 \
		cudaError_t error;                                                            \
		if ((error = cudaGetLastError()) != cudaSuccess) {                            \
			fprintf(stderr, "\tCUDA Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
					cudaGetErrorName(error), cudaGetErrorString(error));              \
			fflush(stderr);                                                           \
			exit(EXIT_FAILURE);                                                       \
		}                                                                             \
	}

#define CUDA_OP(op)                                                                   \
	{                                                                                 \
		cudaError_t error = op;                                                       \
		if (error != cudaSuccess) {                                                   \
			fprintf(stderr, "\tCUDA Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
					cudaGetErrorName(error), cudaGetErrorString(error));              \
			fflush(stderr);                                                           \
			exit(EXIT_FAILURE);                                                       \
		}                                                                             \
	}
#else
#define CUDA_ERROR()

#define CUDA_OP(op) op
#endif
///@endcond
#endif // _CTRL_CUDA_UTILS_H_
