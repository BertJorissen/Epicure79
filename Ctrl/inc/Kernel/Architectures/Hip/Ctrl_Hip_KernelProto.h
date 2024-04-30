#ifndef _CTRL_HIP_KERNELPROTO_H_
#define _CTRL_HIP_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_HIP_KernelProto.h
 * @author Trasgo Group
 * @brief Macros to generate the code and manage HIP kernels.
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

/**
 * Block of code that launches a \e HIP kernel, this calls to the kernel function defined on either \e CTRL_KERNEL_HIP or
 * \e CTRL_KERNEL_FUNCTION_CHIP with the appropiate characterization.
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
#define CTRL_KERNEL_WRAP_HIP(name, argsList, type, subtype, ...)                                                                                                                          \
	{                                                                                                                                                                                     \
		if (threads.dims != blocksize.dims) {                                                                                                                                             \
			fprintf(stderr, "[CTRL_KERNEL_CUDA_WRAP] WARNING: Thread space dims (%d) and blocksize dims (%d) don't match on launch of kernel %s\n", threads.dims, blocksize.dims, #name); \
			fflush(stderr);                                                                                                                                                               \
		}                                                                                                                                                                                 \
		unsigned int grid_x = 1, grid_y = 1, grid_z = 1;                                                                                                                                  \
		unsigned int block_x = 1, block_y = 1, block_z = 1;                                                                                                                               \
		switch (threads.dims) {                                                                                                                                                           \
			case 3:                                                                                                                                                                       \
				grid_x  = (threads.k + blocksize.k - 1) / blocksize.k;                                                                                                                    \
				grid_y  = (threads.j + blocksize.j - 1) / blocksize.j;                                                                                                                    \
				grid_z  = (threads.i + blocksize.i - 1) / blocksize.i;                                                                                                                    \
				block_x = blocksize.k;                                                                                                                                                    \
				block_y = blocksize.j;                                                                                                                                                    \
				block_z = blocksize.i;                                                                                                                                                    \
				break;                                                                                                                                                                    \
			case 2:                                                                                                                                                                       \
				grid_x  = (threads.j + blocksize.j - 1) / blocksize.j;                                                                                                                    \
				grid_y  = (threads.i + blocksize.i - 1) / blocksize.i;                                                                                                                    \
				block_x = blocksize.j;                                                                                                                                                    \
				block_y = blocksize.i;                                                                                                                                                    \
				break;                                                                                                                                                                    \
			case 1:                                                                                                                                                                       \
				grid_x  = (threads.i + blocksize.i - 1) / blocksize.i;                                                                                                                    \
				block_x = blocksize.i;                                                                                                                                                    \
				break;                                                                                                                                                                    \
			default:                                                                                                                                                                      \
				fprintf(stderr, "[CTRL_KERNEL_WRAP_CUDA] ERROR: Invalid number of dimensions for thread space on kernel %s: %d\n", #name, threads.dims);                                  \
				exit(EXIT_FAILURE);                                                                                                                                                       \
		}                                                                                                                                                                                 \
		dim3  grid   = {grid_x, grid_y, grid_z};                                                                                                                                          \
		dim3  block  = {block_x, block_y, block_z};                                                                                                                                       \
		void *args[] = {&threads, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_REF(argsList, __VA_ARGS__)};                                                                                          \
		hipLaunchKernel((void *)Ctrl_Kernel_Hip_##type##_##subtype##_##name, grid, block, args, (size_t)0, *(request.hip.p_stream));                                                      \
		HIP_ERROR();                                                                                                                                                                      \
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

///@endcond
#endif // _CTRL_HIP_KERNELPROTO_H_
