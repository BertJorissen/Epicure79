#ifndef _CTRL_HIP_UTILS_H_
#define _CTRL_HIP_UTILS_H_
///@cond INTERNAL
/**
 * @file hip_helper.h
 * @brief Macros for error checking on HIP operations when debugging.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifdef _CTRL_HIP_ERROR_CHECK_
#include <hip/hip_runtime_api.h>

#define HIP_ERROR()                                                                  \
	{                                                                                \
		hipError_t error;                                                            \
		if ((error = hipGetLastError()) != hipSuccess) {                             \
			fprintf(stderr, "\tHIP Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
					hipGetErrorName(error), hipGetErrorString(error));               \
			fflush(stderr);                                                          \
			exit(EXIT_FAILURE);                                                      \
		}                                                                            \
	}

#define HIP_OP(op)                                                                   \
	{                                                                                \
		hipError_t error = op;                                                       \
		if (error != hipSuccess) {                                                   \
			fprintf(stderr, "\tHIP Error at: %s::%d\n %s: %s\n", __FILE__, __LINE__, \
					hipGetErrorName(error), hipGetErrorString(error));               \
			fflush(stderr);                                                          \
			exit(EXIT_FAILURE);                                                      \
		}                                                                            \
	}
#else
#define HIP_ERROR()

#define HIP_OP(op) op
#endif
///@endcond
#endif // _CTRL_HIP_UTILS_H_
