#ifndef _CTRL_CUDA_KERNELPROTO_H_
#define _CTRL_CUDA_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cuda_KernelProto.h
 * @author Trasgo Group
 * @brief Macros to generate the code and manage CUDA kernels.
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

#include "Kernel/Ctrl_KernelArgs.h"
#include "Kernel/Ctrl_Thread.h"

#include "Core/Ctrl_Request.h"

#include "Architectures/Cuda/Ctrl_Cuda_Helper.h"
#include "Architectures/Cuda/Ctrl_Cuda_Request.h"
#include "Architectures/Cuda/Ctrl_Cuda.h"

/**
 * Defines the function containing the user provided code for a \e CUDA type kernel
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CUDA
 */
#define CTRL_KERNEL_CUDA(name, type, subtype, ...) \
	extern "C" \
	__global__ void Ctrl_Kernel_Cuda_##type##_##subtype##_##name(Ctrl_Thread threads, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)){ \
		unsigned int thread_idx = blockIdx.x * blockDim.x + threadIdx.x; \
		unsigned int thread_idy = blockIdx.y * blockDim.y + threadIdx.y; \
		unsigned int thread_idz = blockIdx.z * blockDim.z + threadIdx.z; \
		if (threads.dims == 3) \
		{ \
			int thread_temp = thread_idx; \
			thread_idx = thread_idz; \
			thread_idz = thread_temp; \
		} \
		else if (threads.dims == 2) \
		{ \
			int thread_temp = thread_idx; \
			thread_idx = thread_idy; \
			thread_idy = thread_temp; \
		} \
		if (thread_idx >= threads.x || thread_idy >= threads.y  \
			|| thread_idz >= threads.z) \
			return; \
		Ctrl_Thread thread_id = { threads.dims, thread_idx,	thread_idy, thread_idz }; \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__) \
	}

/**
 * Defines the function containing the user provided code for a \e CUDA type kernel
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CUDA
 */
#define CTRL_KERNEL_FUNCTION_CUDA(name, type, subtype, ...) \
	extern "C" \
	__global__ void Ctrl_Kernel_Cuda_##type##_##subtype##_##name(Ctrl_Thread threads, __VA_ARGS__){ \
		unsigned int thread_idx = blockIdx.x * blockDim.x + threadIdx.x; \
		unsigned int thread_idy = blockIdx.y * blockDim.y + threadIdx.y; \
		unsigned int thread_idz = blockIdx.z * blockDim.z + threadIdx.z; \
		if (threads.dims == 3) \
		{ \
			int thread_temp = thread_idx; \
			thread_idx = thread_idz; \
			thread_idz = thread_temp; \
		} \
		else if (threads.dims == 2) \
		{ \
			int thread_temp = thread_idx; \
			thread_idx = thread_idy; \
			thread_idy = thread_temp; \
		} \
		if (thread_idx >= threads.x || thread_idy >= threads.y \
			|| thread_idz >= threads.z) \
			return; \
		Ctrl_Thread thread_id = { threads.dims, thread_idx,	thread_idy, thread_idz };

/**
 * Defines the function containing the user provided code for a \e GENERIC type kernel
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CUDA_GENERIC
 */
#define CTRL_KERNEL_CUDA_GENERIC(name, type, subtype, ...) \
	CTRL_KERNEL_CUDA(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e CUDA_LIB type kernel
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CUDA_LIB
 */
#define CTRL_KERNEL_CUDALIB(name, type, subtype, ...) \
	extern "C" \
	void Ctrl_Kernel_Cuda_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)){ \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__) \
	}

/**
 * Block of code that launches a \e CUDA kernel, this calls to the kernel function defined on either \e CTRL_KERNEL_CUDA or 
 * \e CTRL_KERNEL_FUNCTION_CUDA with the appropiate characterization.
 * @hideinitializer
 * 
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CUDA, CTRL_KERNEL_FUNCTION_CUDA
 */
#define CTRL_KERNEL_WRAP_CUDA(name, argsList, type, subtype, ...) \
	{ \
		dim3 grid = CTRL_KERNEL_CUDA_CHAR_grid(threads, blocksize); \
		dim3 block = {blocksize.x, blocksize.y, blocksize.z}; \
		void *args[] = { \
			&threads, \
			CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_REF(argsList, __VA_ARGS__) \
		}; \
		cudaLaunchKernel((void*)Ctrl_Kernel_Cuda_##type##_##subtype##_##name, grid, block, args, (size_t)0, *(request.cuda.stream)); \
		CUDA_ERROR(); \
	};

/**
 * Block of code that launches a \e GENERIC kernel on \e CUDA architecture, this calls to the kernel function defined on 
 * \e CTRL_KERNEL_CUDA_GENERIC with the appropiate characterization.
 * @hideinitializer
 * 
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CUDA_GENERIC
 */
#define CTRL_KERNEL_WRAP_CUDA_GENERIC(name, argsList, type, subtype, ...) \
	CTRL_KERNEL_WRAP_CUDA(name, argsList, type, subtype, __VA_ARGS__);

/**
 * Block of code that launches a \e CUDA_LIB kernel, this calls directly to the function defined in \e CTRL_KERNEL_CUDA_LIB.
 * @hideinitializer
 * 
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CUDA_LIB
 */
#define CTRL_KERNEL_WRAP_CUDALIB(name, argsList, type, subtype, ...) \
	{ \
		Ctrl_Kernel_Cuda_##type##_##subtype##_##name(CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argsList, __VA_ARGS__)); \
	};

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
#define CTRL_KERNEL_DECLARATION_CUDA( name, type, subtype, n_params, ... ) \
	__global__ void Ctrl_Kernel_Cuda_##type##_##subtype##_##name(Ctrl_Thread threads, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params( __VA_ARGS__ ));

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
#define CTRL_KERNEL_DECLARATION_CUDALIB( name, type, subtype, n_params, ... ) \
	void Ctrl_Kernel_Cuda_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params( __VA_ARGS__ ));
	
#define CTRL_KERNEL_LAUNCH_POINTERS_CUDA( name, type, subtype, ... ) \
	void Ctrl_Kernel_Cuda_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__));

///@endcond
#endif // _CTRL_CUDA_KERNELPROTO_H_