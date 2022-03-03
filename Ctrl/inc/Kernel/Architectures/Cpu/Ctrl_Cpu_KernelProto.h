#ifndef _CTRL_CPU_KERNELPROTO_H_
#define _CTRL_CPU_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_KernelProto.h
 * @author Trasgo Group
 * @brief Macros to generate the code and manage Cpu kernels.
 * @version 2.1
 * @date 2021-04-26
 *
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
 */

#include <stdio.h>

#include "Kernel/Ctrl_KernelArgs.h"
#include "Kernel/Ctrl_Thread.h"

// @seralpa turned off formater for this macro cause pragmas caused issues in kernel loops
// clang-format off

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
#define CTRL_KERNEL_CPU(name, type, subtype, ...)                                                                                                      \
	C_GUARD                                                                                                                                            \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name(Ctrl_Thread threads, Ctrl_Thread blocksize, int n_cores, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		/* Index space */                                                                                                                              \
		int threads_x = threads.x;                                                                                                                     \
		int threads_y = threads.y;                                                                                                                     \
		int threads_z = threads.z;                                                                                                                     \
		/* Block size */                                                                                                                               \
		int block_x = blocksize.x;                                                                                                                     \
		int block_y = blocksize.y;                                                                                                                     \
		int block_z = blocksize.z;                                                                                                                     \
		/* indexes */                                                                                                                                  \
		int thread_id_x __attribute__((unused)) = 0;                                                                                                   \
		int thread_id_y __attribute__((unused)) = 0;                                                                                                   \
		int thread_id_z __attribute__((unused)) = 0;                                                                                                   \
		if (threads.z > 1) {                                                                                                                           \
			_Pragma("omp parallel for num_threads(n_cores)") /* Executing in parallel the 3D threads */                                                \
			for (int i_outer = 0; i_outer < threads_x; i_outer += block_x) {                                                                           \
				for (int j_outer = 0; j_outer < threads_y; j_outer += block_y) {                                                                       \
					for (int k_outer = 0; k_outer < threads_z; k_outer += block_z) {                                                                   \
						int i_max = ((i_outer + block_x) > threads_x) ? threads_x : (i_outer + block_x);                                               \
						int j_max = ((j_outer + block_y) > threads_y) ? threads_y : (j_outer + block_y);                                               \
						int k_max = ((k_outer + block_z) > threads_z) ? threads_z : (k_outer + block_z);                                               \
						for (int thread_id_x = i_outer; thread_id_x < i_max; thread_id_x++) {                                                          \
							for (int thread_id_y = j_outer; thread_id_y < j_max; thread_id_y++) {                                                      \
								for (int thread_id_z = k_outer; thread_id_z < k_max; thread_id_z++) {                                                  \
									CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                     \
								}                                                                                                                      \
							}                                                                                                                          \
						}                                                                                                                              \
					}                                                                                                                                  \
				}                                                                                                                                      \
			}                                                                                                                                          \
		} else if (threads.y > 1) {                                                                                                                    \
			_Pragma("omp parallel for num_threads(n_cores)") /* Executing in parallel the 2D threads */                                                \
			for (int i_outer = 0; i_outer < threads_x; i_outer += block_x) {                                                                           \
				for (int j_outer = 0; j_outer < threads_y; j_outer += block_y) {                                                                       \
					int i_max = ((i_outer + block_x) > threads_x) ? threads_x : (i_outer + block_x);                                                   \
					int j_max = ((j_outer + block_y) > threads_y) ? threads_y : (j_outer + block_y);                                                   \
					for (int thread_id_x = i_outer; thread_id_x < i_max; thread_id_x++) {                                                              \
						for (int thread_id_y = j_outer; thread_id_y < j_max; thread_id_y++) {                                                          \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                             \
						}                                                                                                                              \
					}                                                                                                                                  \
				}                                                                                                                                      \
			}                                                                                                                                          \
		} else {                                                                                                                                       \
			_Pragma("omp parallel for num_threads(n_cores)") /* Executing in parallel the 1D threads */                                                \
			for (int i_outer = 0; i_outer < threads_x; i_outer += block_x) {                                                                           \
				int i_max = ((i_outer + block_x) > threads_x) ? threads_x : (i_outer + block_x);                                                       \
				for (int thread_id_x = i_outer; thread_id_x < i_max; thread_id_x++) {                                                                  \
					CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                                     \
				}                                                                                                                                      \
			}                                                                                                                                          \
		}                                                                                                                                              \
	}                                                                                                                                                  \
// clang-format on

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
#define CTRL_KERNEL_WRAP_CPU(name, argsList, type, subtype, ...)                                                                                        \
	{                                                                                                                                                   \
		Ctrl_Kernel_Cpu_##type##_##subtype##_##name(threads, blocksize, request.cpu.n_cores, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argsList, __VA_ARGS__)); \
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
 * @note Moving the kernel definition to a separate file is discouraged on \e CPU type kernels because of the inability of the
 * compiler to perform inlining on it, probably leading to poor performance.
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
