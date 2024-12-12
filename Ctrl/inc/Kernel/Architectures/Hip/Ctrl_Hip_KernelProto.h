#ifndef _CTRL_HIP_KERNELPROTO_H_
#define _CTRL_HIP_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_Hip_KernelProto.h
 * @brief Macros to generate the code and manage HIP kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Architectures/Hip/Ctrl_Hip_Helper.h"
#include "Core/Ctrl_Request.h"
#include "Kernel/Ctrl_KernelArgs.h"
#include "Kernel/Ctrl_Thread.h"

/**
 * Defines the function containing the user provided code for a \e HIP type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_HIP
 */
#define CTRL_KERNEL_HIP(name, type, subtype, ...)                                                                                  \
	C_GUARD                                                                                                                        \
	__global__ void Ctrl_Kernel_Hip_##type##_##subtype##_##name(Ctrl_Thread ctrl_threads, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		unsigned int thr_i = 0;                                                                                                    \
		unsigned int thr_j = 0;                                                                                                    \
		unsigned int thr_k = 0;                                                                                                    \
		if (ctrl_threads.dims == 3) {                                                                                              \
			thr_k = blockIdx.x * blockDim.x + threadIdx.x;                                                                         \
			thr_j = blockIdx.y * blockDim.y + threadIdx.y;                                                                         \
			thr_i = blockIdx.z * blockDim.z + threadIdx.z;                                                                         \
		} else if (ctrl_threads.dims == 2) {                                                                                       \
			thr_j = blockIdx.x * blockDim.x + threadIdx.x;                                                                         \
			thr_i = blockIdx.y * blockDim.y + threadIdx.y;                                                                         \
		} else {                                                                                                                   \
			thr_i = blockIdx.x * blockDim.x + threadIdx.x;                                                                         \
		}                                                                                                                          \
		if (thr_i >= ctrl_threads.i || thr_j >= ctrl_threads.j || thr_k >= ctrl_threads.k)                                         \
			return;                                                                                                                \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                             \
	}

/**
 * Defines the function containing the user provided code for a \e HIP type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_HIP
 */
#define CTRL_KERNEL_FUNCTION_HIP(name, type, subtype, ...)                                               \
	C_GUARD                                                                                              \
	__global__ void Ctrl_Kernel_Hip_##type##_##subtype##_##name(Ctrl_Thread ctrl_threads, __VA_ARGS__) { \
		unsigned int thr_i = 0;                                                                          \
		unsigned int thr_j = 0;                                                                          \
		unsigned int thr_k = 0;                                                                          \
		if (ctrl_threads.dims == 3) {                                                                    \
			thr_k = blockIdx.x * blockDim.x + threadIdx.x;                                               \
			thr_j = blockIdx.y * blockDim.y + threadIdx.y;                                               \
			thr_i = blockIdx.z * blockDim.z + threadIdx.z;                                               \
		} else if (ctrl_threads.dims == 2) {                                                             \
			thr_j = blockIdx.x * blockDim.x + threadIdx.x;                                               \
			thr_i = blockIdx.y * blockDim.y + threadIdx.y;                                               \
		} else {                                                                                         \
			thr_i = blockIdx.x * blockDim.x + threadIdx.x;                                               \
		}                                                                                                \
		if (thr_i >= ctrl_threads.i || thr_j >= ctrl_threads.j || thr_k >= ctrl_threads.k)               \
			return;

#ifdef _CTRL_DEBUG_
#define HIP_GENERIC_KERNEL_WARN _Pragma("message(\"[Ctrl Kernels] HIP support is enabled, but hipcc isn't being used to compile the file containing the kernel (most likely because it is a .c or .cu file). \
This will cause an execution error if a HIP Controller is created and used to execute a GENERIC kernel. \
Change the file extension to .cpp to avoid the error and this warning. \
If the program doesn't use HIP Controllers, you can ignore this message.\")")
#else // _CTRL_DEBUG_
#define HIP_GENERIC_KERNEL_WARN
#endif // _CTRL_DEBUG_

// If not using nvcc use generic kernel dummy to prevent compilation error
#ifdef __HIPCC__
/**
 * Defines the function containing the user provided code for a \e GENERIC type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_HIP_GENERIC
 */
#define CTRL_KERNEL_HIP_GENERIC(name, type, subtype, ...) \
	CTRL_KERNEL_HIP(name, type, subtype, __VA_ARGS__)
#else // !__HIPCC__
/**
 * Defines the function containing the user provided code for a \e GENERIC type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_HIP_GENERIC
 */
#define CTRL_KERNEL_HIP_GENERIC(name, type, subtype, ...)                                                                     \
	HIP_GENERIC_KERNEL_WARN                                                                                                   \
	C_GUARD                                                                                                                   \
	__global__ void Ctrl_Kernel_Hip_##type##_##subtype##_##name(Ctrl_Thread threads, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
	}
#endif // __HIPCC__

/**
 * Defines the function containing the user provided code for a \e HIPLIB type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_HIPLIB
 */
#define CTRL_KERNEL_HIPLIB(name, type, subtype, ...) CTRL_KERNEL_HIPLIB_##subtype(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e HIPLIB type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_HIPLIB
 */
#define CTRL_KERNEL_FUNCTION_HIPLIB(name, type, subtype, ...) CTRL_KERNEL_FUNCTION_HIPLIB_##subtype(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e HIPLIB_DEFAULT type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_HIPLIB, CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_HIPLIB_DEFAULT(name, type, subtype, ...)                                                      \
	C_GUARD                                                                                                       \
	void Ctrl_Kernel_Hip_##type##_##subtype##_##name(hipStream_t stream, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                            \
	}

/**
 * Defines the function containing the user provided code for a \e HIPLIB_DEFAULT type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_HIPLIB, CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_FUNCTION_HIPLIB_DEFAULT(name, type, subtype, ...) \
	C_GUARD                                                           \
	void Ctrl_Kernel_Hip_##type##_##subtype##_##name(hipStream_t stream, __VA_ARGS__)

#ifdef _CTRL_HIPBLAS_
/**
 * Defines the function containing the user provided code for a \e HIPLIB_HIPBLAS type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_HIPLIB, CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_HIPLIB_HIPBLAS(name, type, subtype, ...)                                                          \
	C_GUARD                                                                                                           \
	void Ctrl_Kernel_Hip_##type##_##subtype##_##name(hipblasHandle_t handle, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                \
	}

/**
 * Defines the function containing the user provided code for a \e HIPLIB_HIPBLAS type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_HIPLIB, CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_FUNCTION_HIPLIB_HIPBLAS(name, type, subtype, ...) \
	C_GUARD                                                           \
	void Ctrl_Kernel_Hip_##type##_##subtype##_##name(hipblasHandle_t handle, __VA_ARGS__)
#else // _CTRL_HIPBLAS_
#define CTRL_KERNEL_HIPLIB_HIPBLAS(...)
#define CTRL_KERNEL_FUNCTION_HIPLIB_HIPBLAS(...)
#endif // _CTRL_HIPBLAS_

/**
 * Block of code that launches a \e HIP kernel, this calls to the kernel function defined on either \e CTRL_KERNEL_HIP or
 * \e CTRL_KERNEL_FUNCTION_HIP with the appropiate characterization.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_HIP, CTRL_KERNEL_FUNCTION_HIP
 */
#define CTRL_KERNEL_WRAP_HIP(name, argsList, type, subtype, ...)                                                                                                                         \
	{                                                                                                                                                                                    \
		if (threads.dims != blocksize.dims) {                                                                                                                                            \
			fprintf(stderr, "[CTRL_KERNEL_HIP_WRAP] WARNING: Thread space dims (%d) and blocksize dims (%d) don't match on launch of kernel %s\n", threads.dims, blocksize.dims, #name); \
			fflush(stderr);                                                                                                                                                              \
		}                                                                                                                                                                                \
		unsigned int grid_x = 1, grid_y = 1, grid_z = 1;                                                                                                                                 \
		unsigned int block_x = 1, block_y = 1, block_z = 1;                                                                                                                              \
		switch (threads.dims) {                                                                                                                                                          \
			case 3:                                                                                                                                                                      \
				grid_x  = (threads.k + blocksize.k - 1) / blocksize.k;                                                                                                                   \
				grid_y  = (threads.j + blocksize.j - 1) / blocksize.j;                                                                                                                   \
				grid_z  = (threads.i + blocksize.i - 1) / blocksize.i;                                                                                                                   \
				block_x = blocksize.k;                                                                                                                                                   \
				block_y = blocksize.j;                                                                                                                                                   \
				block_z = blocksize.i;                                                                                                                                                   \
				break;                                                                                                                                                                   \
			case 2:                                                                                                                                                                      \
				grid_x  = (threads.j + blocksize.j - 1) / blocksize.j;                                                                                                                   \
				grid_y  = (threads.i + blocksize.i - 1) / blocksize.i;                                                                                                                   \
				block_x = blocksize.j;                                                                                                                                                   \
				block_y = blocksize.i;                                                                                                                                                   \
				break;                                                                                                                                                                   \
			case 1:                                                                                                                                                                      \
				grid_x  = (threads.i + blocksize.i - 1) / blocksize.i;                                                                                                                   \
				block_x = blocksize.i;                                                                                                                                                   \
				break;                                                                                                                                                                   \
			default:                                                                                                                                                                     \
				fprintf(stderr, "[CTRL_KERNEL_WRAP_HIP] ERROR: Invalid number of dimensions for thread space on kernel %s: %d\n", #name, threads.dims);                                  \
				exit(EXIT_FAILURE);                                                                                                                                                      \
		}                                                                                                                                                                                \
		dim3  grid   = {grid_x, grid_y, grid_z};                                                                                                                                         \
		dim3  block  = {block_x, block_y, block_z};                                                                                                                                      \
		void *args[] = {&threads, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_REF(argsList, __VA_ARGS__)};                                                                                         \
		HIP_OP(hipLaunchKernel((void *)Ctrl_Kernel_Hip_##type##_##subtype##_##name, grid, block, args, (size_t)0, *(request.hip.p_stream)));                                             \
	};

/**
 * Block of code that launches a \e GENERIC kernel on \e HIP architecture, this calls to the kernel function defined on
 * \e CTRL_KERNEL_HIP_GENERIC with the appropiate characterization.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_HIP_GENERIC
 */
#define CTRL_KERNEL_WRAP_HIP_GENERIC(name, argsList, type, subtype, ...) \
	CTRL_KERNEL_WRAP_HIP(name, argsList, type, subtype, __VA_ARGS__);

/**
 * Block of code that launches a \e HIPLIB kernel, this calls the macro for the appropiate subtype wrapper.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_WRAP_HIPLIB(name, argsList, type, subtype, ...) CTRL_KERNEL_WRAP_HIPLIB_##subtype(name, argsList, type, subtype, __VA_ARGS__)

/**
 * Block of code that launches a \e HIPLIB_DEFAULT kernel, this calls the function defined in \e CTRL_KERNEL_HIPLIB.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL or \e CTRL_KERNEL_FUNCTION.
 * @see CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_WRAP_HIPLIB_DEFAULT(name, argsList, type, subtype, ...)                                                             \
	{                                                                                                                                   \
		Ctrl_Kernel_Hip_##type##_##subtype##_##name(*(request.hip.p_stream), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argsList, __VA_ARGS__)); \
	};

#ifdef _CTRL_HIPBLAS_
/**
 * Block of code that launches a \e HIPLIB_HIPBLAS kernel, this calls the function defined in \e CTRL_KERNEL_HIPLIB.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_HIPLIB
 */
#define CTRL_KERNEL_WRAP_HIPLIB_HIPBLAS(name, argsList, type, subtype, ...)                                                                     \
	{                                                                                                                                           \
		Ctrl_Kernel_Hip_##type##_##subtype##_##name(*(request.hip.p_hipblas_handle), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argsList, __VA_ARGS__)); \
	};
#else // _CTRL_HIPBLAS_
#define CTRL_KERNEL_WRAP_HIPLIB_HIPBLAS(...)
#endif // _CTRL_HIPBLAS_

/**
 * Kernel declaration for host code or header files.
 * Used to declare the kernel prototype in included header files, as the kernel defintions may be written in a separate file
 * from the host code.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_HIP(name, type, subtype, n_params, ...) \
	__global__ void Ctrl_Kernel_Hip_##type##_##subtype##_##name(Ctrl_Thread threads, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params(__VA_ARGS__));

/**
 * Kernel declaration for host code or header files.
 * Used to declare the kernel prototype in included header files, as the kernel defintions may be written in a separate file
 * from the host code.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_HIPLIB(name, type, subtype, n_params, ...) CTRL_KERNEL_DECLARATION_HIPLIB_##subtype(name, type, subtype, n_params, __VA_ARGS__)

/**
 * Kernel declaration for host code or header files for \e HIPLIB_DEFAULT type kernels.
 * Used to declare the kernel prototype in included header files, as the kernel defintions may be written in a separate file
 * from the host code.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_HIPLIB_DEFAULT(name, type, subtype, n_params, ...) \
	void Ctrl_Kernel_Hip_##type##_##subtype##_##name(hipStream_t stream, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params(__VA_ARGS__));

#ifdef _CTRL_HIPBLAS_
/**
 * Kernel declaration for host code or header files for \e HIPLIB_HIPBLAS type kernels.
 * Used to declare the kernel prototype in included header files, as the kernel defintions may be written in a separate file
 * from the host code.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_HIPLIB_HIPBLAS(name, type, subtype, n_params, ...) \
	void Ctrl_Kernel_Hip_##type##_##subtype##_##name(hipblasHandle_t handle, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params(__VA_ARGS__));
#else // _CTRL_HIPBLAS_
#define CTRL_KERNEL_DECLARATION_HIPLIB_HIPBLAS(...)
#endif // _CTRL_HIPBLAS_

///@endcond
#endif // _CTRL_HIP_KERNELPROTO_H_
