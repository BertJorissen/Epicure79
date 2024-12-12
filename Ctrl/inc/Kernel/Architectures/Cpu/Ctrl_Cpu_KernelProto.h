#ifndef _CTRL_CPU_KERNELPROTO_H_
#define _CTRL_CPU_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_KernelProto.h
 * @brief Macros to generate the code and manage Cpu kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <stdio.h>

#include "Kernel/Ctrl_KernelArgs.h"
#include "Kernel/Ctrl_Thread.h"

#include "Core/Ctrl_Profiler_Helper.h"

#define CTRL_KERNEL_CPU_3D(collapse_dims, ...)                                                       \
	if (blocksize.dims == 0) {                                                                       \
		/* Executing in parallel the 3D threads */                                                   \
		_Pragma(CTRL_MACRO_STRINGIFY(omp parallel for num_threads(n_cores) collapse(collapse_dims))) \
		for (int thr_i = 0; thr_i < threads_i; thr_i++) {                                            \
			for (int thr_j = 0; thr_j < threads_j; thr_j++) {                                        \
				for (int thr_k = 0; thr_k < threads_k; thr_k++) {                                    \
					CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                   \
				}                                                                                    \
			}                                                                                        \
		}                                                                                            \
	} else {                                                                                         \
		/* Executing in parallel the 3D threads */                                                   \
		_Pragma(CTRL_MACRO_STRINGIFY(omp parallel for num_threads(n_cores) collapse(collapse_dims))) \
		for (int i_outer = 0; i_outer < threads_i; i_outer += block_i) {                             \
			for (int j_outer = 0; j_outer < threads_j; j_outer += block_j) {                         \
				for (int k_outer = 0; k_outer < threads_k; k_outer += block_k) {                     \
					int i_max = ((i_outer + block_i) > threads_i) ? threads_i : (i_outer + block_i); \
					int j_max = ((j_outer + block_j) > threads_j) ? threads_j : (j_outer + block_j); \
					int k_max = ((k_outer + block_k) > threads_k) ? threads_k : (k_outer + block_k); \
					for (int thr_i = i_outer; thr_i < i_max; thr_i++) {                              \
						for (int thr_j = j_outer; thr_j < j_max; thr_j++) {                          \
							for (int thr_k = k_outer; thr_k < k_max; thr_k++) {                      \
								CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                       \
							}                                                                        \
						}                                                                            \
					}                                                                                \
				}                                                                                    \
			}                                                                                        \
		}                                                                                            \
	}

#define CTRL_KERNEL_CPU_2D(collapse_dims, ...)                                                                                                  \
	if (blocksize.dims == 0) {                                                                                                                  \
		/* Executing in parallel the 2D threads */                                                                                              \
		_Pragma(CTRL_MACRO_STRINGIFY(omp parallel for num_threads(n_cores) collapse(collapse_dims)))                                            \
		for (int thr_i = 0; thr_i < threads_i; thr_i++) {                                                                                       \
			for (int thr_j = 0; thr_j < threads_j; thr_j++) {                                                                                   \
				CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                                  \
			}                                                                                                                                   \
		}                                                                                                                                       \
	} else {                                                                                                                                    \
		_Pragma(CTRL_MACRO_STRINGIFY(omp parallel for num_threads(n_cores) collapse(collapse_dims))) /* Executing in parallel the 2D threads */ \
		for (int i_outer = 0; i_outer < threads_i; i_outer += block_i) {                                                                        \
			for (int j_outer = 0; j_outer < threads_j; j_outer += block_j) {                                                                    \
				int i_max = ((i_outer + block_i) > threads_i) ? threads_i : (i_outer + block_i);                                                \
				int j_max = ((j_outer + block_j) > threads_j) ? threads_j : (j_outer + block_j);                                                \
				for (int thr_i = i_outer; thr_i < i_max; thr_i++) {                                                                             \
					for (int thr_j = j_outer; thr_j < j_max; thr_j++) {                                                                         \
						CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                          \
					}                                                                                                                           \
				}                                                                                                                               \
			}                                                                                                                                   \
		}                                                                                                                                       \
	}

#define CTRL_KERNEL_CPU_1D(...)                                                              \
	if (blocksize.dims == 0) {                                                               \
		/* Executing in parallel the 2D threads */                                           \
		_Pragma("omp parallel for num_threads(n_cores)")                                     \
		for (int thr_i = 0; thr_i < threads_i; thr_i++) {                                    \
			CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                   \
		}                                                                                    \
	} else {                                                                                 \
		/* Executing in parallel the 1D threads */                                           \
		_Pragma("omp parallel for num_threads(n_cores)")                                     \
		for (int i_outer = 0; i_outer < threads_i; i_outer += block_i) {                     \
			int i_max = ((i_outer + block_i) > threads_i) ? threads_i : (i_outer + block_i); \
			for (int thr_i = i_outer; thr_i < i_max; thr_i++) {                              \
				CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                               \
			}                                                                                \
		}                                                                                    \
	}

/**
 * Defines the function containing the user provided code for a \e CPU type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and by kernel body.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPU
 */
#define CTRL_KERNEL_CPU(name, type, subtype, ...)                                                                                                           \
	C_GUARD                                                                                                                                                 \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name(Ctrl_Thread ctrl_threads, Ctrl_Thread blocksize, int n_cores, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		/* Index space */                                                                                                                                   \
		int threads_i = ctrl_threads.i;                                                                                                                     \
		int threads_j = ctrl_threads.j;                                                                                                                     \
		int threads_k = ctrl_threads.k;                                                                                                                     \
		/* Block size */                                                                                                                                    \
		int block_i = blocksize.i;                                                                                                                          \
		int block_j = blocksize.j;                                                                                                                          \
		int block_k = blocksize.k;                                                                                                                          \
		/* indexes */                                                                                                                                       \
		int thr_i __attribute__((unused)) = 0;                                                                                                              \
		int thr_j __attribute__((unused)) = 0;                                                                                                              \
		int thr_k __attribute__((unused)) = 0;                                                                                                              \
		/* parallel dims */                                                                                                                                 \
		int parallel_dims = atoi(CTRL_MACRO_STRINGIFY(CTRL_KERNEL_PARALLEL_DIMS_##name));                                                                   \
		switch (ctrl_threads.dims) {                                                                                                                        \
			case 3:                                                                                                                                         \
				switch (parallel_dims) {                                                                                                                    \
					case 0:                                                                                                                                 \
					case 3: CTRL_KERNEL_CPU_3D(3, __VA_ARGS__); break;                                                                                      \
					case 2: CTRL_KERNEL_CPU_3D(2, __VA_ARGS__); break;                                                                                      \
					case 1: CTRL_KERNEL_CPU_3D(1, __VA_ARGS__); break;                                                                                      \
					default:                                                                                                                                \
						fprintf(stderr, "[Ctrl_CPUKernel] Invalid value for dimension collapse %d\n", parallel_dims);                                       \
						exit(EXIT_FAILURE);                                                                                                                 \
				}                                                                                                                                           \
				break;                                                                                                                                      \
			case 2:                                                                                                                                         \
				switch (parallel_dims) {                                                                                                                    \
					case 0:                                                                                                                                 \
					case 3:                                                                                                                                 \
					case 2: CTRL_KERNEL_CPU_2D(2, __VA_ARGS__); break;                                                                                      \
					case 1: CTRL_KERNEL_CPU_2D(1, __VA_ARGS__); break;                                                                                      \
					default:                                                                                                                                \
						fprintf(stderr, "[Ctrl_CPUKernel] Invalid value for dimension collapse %d\n", parallel_dims);                                       \
						exit(EXIT_FAILURE);                                                                                                                 \
				}                                                                                                                                           \
				break;                                                                                                                                      \
			case 1:                                                                                                                                         \
				CTRL_KERNEL_CPU_1D(__VA_ARGS__);                                                                                                            \
				break;                                                                                                                                      \
			case 0:                                                                                                                                         \
				/* Executing in task mode */                                                                                                                \
				CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                                              \
				break;                                                                                                                                      \
			default:                                                                                                                                        \
				fprintf(stderr, "[Ctrl_CPUKernel] Unsupported number of dimensions: %d\n", ctrl_threads.dims);                                              \
				exit(EXIT_FAILURE);                                                                                                                         \
		}                                                                                                                                                   \
	}

/**
 * Defines the function containing the user provided code for a \e GENERIC type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPU_GENERIC
 */
#define CTRL_KERNEL_CPU_GENERIC(name, type, subtype, ...) \
	CTRL_KERNEL_CPU(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e CPULIB type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPULIB
 */
#define CTRL_KERNEL_CPULIB(name, type, subtype, ...) CTRL_KERNEL_CPULIB_##subtype(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e CPULIB type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPULIB
 */
#define CTRL_KERNEL_FUNCTION_CPULIB(name, type, subtype, ...) CTRL_KERNEL_FUNCTION_CPULIB_##subtype(name, type, subtype, __VA_ARGS__)

#ifdef _CTRL_MKL_
/**
 * Defines the function containing the user provided code for a \e CPULIB_MKL type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPULIB
 */
#define CTRL_KERNEL_CPULIB_MKL(name, type, subtype, ...)                                      \
	C_GUARD                                                                                   \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                        \
	}

/**
 * Defines the function containing the user provided code for a \e CPULIB_MKL type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_CPULIB
 */
#define CTRL_KERNEL_FUNCTION_CPULIB_MKL(name, type, subtype, ...) \
	C_GUARD                                                       \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name(__VA_ARGS__)
#else // _CTRL_MKL_
#define CTRL_KERNEL_CPULIB_MKL(...)
#define CTRL_KERNEL_FUNCTION_CPULIB_MKL(...)
#endif // _CTRL_MKL_

/**
 * Block of code that launches a \e CPU kernel, this calculates thread ids and calls to the function defined in either
 * \e CTRL_KERNEL_CPU.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CPU
 */
#define CTRL_KERNEL_WRAP_CPU(name, argsList, type, subtype, ...)                                                                                                                         \
	{                                                                                                                                                                                    \
		CTRL_PROF_RANGEPUSH(#name);                                                                                                                                                      \
		if (threads.dims != blocksize.dims && blocksize.dims != 0 && threads.dims != 0) {                                                                                                \
			fprintf(stderr, "[CTRL_KERNEL_CPU_WRAP] WARNING: Thread space dims (%d) and blocksize dims (%d) don't match on launch of kernel %s\n", threads.dims, blocksize.dims, #name); \
			fflush(stderr);                                                                                                                                                              \
		}                                                                                                                                                                                \
		Ctrl_Kernel_Cpu_##type##_##subtype##_##name(threads, blocksize, request.cpu.n_cores, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argsList, __VA_ARGS__));                                  \
		CTRL_PROF_RANGEPOP();                                                                                                                                                            \
	};

/**
 * Block of code that launches a \e GENERIC kernel on \e CPU architecture, this calculates thread ids and calls the
 * function defined in \e CTRL_KERNEL_CPU_GENERIC.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param argslist List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CPU_GENERIC
 */
#define CTRL_KERNEL_WRAP_CPU_GENERIC(name, argslist, type, subtype, ...) \
	CTRL_KERNEL_WRAP_CPU(name, argslist, type, subtype, __VA_ARGS__)

/**
 * Block of code that launches a \e CPULIB kernel, this calls the function defined in \e CTRL_KERNEL_CPULIB.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param argslist List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CPULIB
 */
#define CTRL_KERNEL_WRAP_CPULIB(name, argslist, type, subtype, ...) CTRL_KERNEL_WRAP_CPULIB_##subtype(name, argslist, type, subtype, __VA_ARGS__)

#ifdef _CTRL_MKL_
/**
 * Block of code that launches a \e CPULIB_MKL kernel, this calls the function defined in \e CTRL_KERNEL_CPULIB.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param argslist List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CPULIB
 */
#define CTRL_KERNEL_WRAP_CPULIB_MKL(name, argslist, type, subtype, ...)                                        \
	{                                                                                                          \
		mkl_set_num_threads_local(request.cpu.n_cores);                                                        \
		Ctrl_Kernel_Cpu_##type##_##subtype##_##name(CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argslist, __VA_ARGS__)); \
	};
#else // _CTRL_MKL_
#define CTRL_KERNEL_WRAP_CPULIB_MKL(...)
#endif // _CTRL_MKL_

/**
 * Kernel function prototype for \e CPU type kernels to allow moving kernel definitions to another file.
 *
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_CPU(name, type, subtype, n_params, ...) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name(Ctrl_Thread threads, Ctrl_Thread blocksize, int n_cores, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params(__VA_ARGS__));

/**
 * Kernel function prototype for \e CPULIB type kernels to allow moving kernel definitions to another file.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_CPULIB(name, type, subtype, n_params, ...) \
	CTRL_KERNEL_DECLARATION_CPULIB_##subtype(name, type, subtype, n_params, __VA_ARGS__)

#ifdef _CTRL_MKL_
/**
 * Kernel function prototype for \e CPULIB_MKL type kernels to allow moving kernel definitions to another file.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_CPULIB_MKL(name, type, subtype, n_params, ...) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params(__VA_ARGS__));
#else // _CTRL_MKL_
#define CTRL_KERNEL_DECLARATION_CPULIB_MKL(...)
#endif // _CTRL_MKL_

///@endcond
#endif //_CTRL_CPU_KERNELPROTO_H_
