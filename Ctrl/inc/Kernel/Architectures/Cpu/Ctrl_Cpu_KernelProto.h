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
#define CTRL_KERNEL_CPU( name, type, subtype, ... ) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name ( Ctrl_Thread thread_id, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__) ){ \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__) \
	}

/**
 * Defines the function containing the user provided code for a \e CPU type kernel
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPU
 */
#define CTRL_KERNEL_FUNCTION_CPU( name, type, subtype, ... ) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name ( Ctrl_Thread thread_id, __VA_ARGS__ ) \
	{

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
 * Defines the function containing the user provided code for a \e CPU_LIB type kernel
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_CPU_LIB
 */
#define CTRL_KERNEL_CPULIB( name, type, subtype, ... ) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name (CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__) ){ \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__) \
	}

/**
 * Block of code that launches a \e CPU kernel, this calculates thread ids and calls to the function defined in either
 * \e CTRL_KERNEL_CPU or \e CTRL_KERNEL_FUNCTION_CPU.
 * @hideinitializer
 * 
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CPU, CTRL_KERNEL_FUNCTION_CPU
 */
#define CTRL_KERNEL_WRAP_CPU( name, argsList, type, subtype, ... ) \
	{ \
		if (threads.z > 1) {  /* Executing in parallel the 3D threads */ \
			_Pragma("omp parallel for num_threads(request.cpu.n_cores)")  \
			for (unsigned int i_outer = 0; i_outer < threads.x; i_outer+=blocksize.x) { \
				for (unsigned int j_outer = 0; j_outer < threads.y; j_outer+=blocksize.y) { \
					for (unsigned int k_outer = 0; k_outer < threads.z; k_outer+=blocksize.z) { \
						unsigned int i_max = ((i_outer+blocksize.x) > threads.x) ? threads.x : (i_outer+blocksize.x); \
						unsigned int j_max = ((j_outer+blocksize.y) > threads.y) ? threads.y : (j_outer+blocksize.y); \
						unsigned int k_max = ((k_outer+blocksize.z) > threads.z) ? threads.z : (k_outer+blocksize.z); \
						for (unsigned int i_inner = i_outer; i_inner < i_max; i_inner++){ \
							for (unsigned int j_inner = j_outer; j_inner < j_max; j_inner++){ \
								for (unsigned int k_inner = k_outer; k_inner < k_max; k_inner++){ \
									Ctrl_Thread thread_id = CTRL_THREAD_NULL; \
									thread_id.x = i_inner; \
									thread_id.y = j_inner; \
									thread_id.z = k_inner; \
									Ctrl_Kernel_Cpu_##type##_##subtype##_##name( thread_id, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE( argsList, __VA_ARGS__ ) ); \
								} \
							} \
						} \
					} \
				} \
			} \
		} else if (threads.y > 1) { /* Executing in parallel the 2D threads */ \
			_Pragma("omp parallel for num_threads(request.cpu.n_cores)")  \
			for (unsigned int i_outer = 0; i_outer < threads.x; i_outer+=blocksize.x) { \
				for (unsigned int j_outer = 0; j_outer < threads.y; j_outer+=blocksize.y) { \
					unsigned int i_max = ((i_outer+blocksize.x) > threads.x) ? threads.x : (i_outer+blocksize.x); \
					unsigned int j_max = ((j_outer+blocksize.y) > threads.y) ? threads.y : (j_outer+blocksize.y); \
					for (unsigned int i_inner = i_outer; i_inner < i_max; i_inner++){ \
						for (unsigned int j_inner = j_outer; j_inner < j_max; j_inner++){ \
							Ctrl_Thread thread_id = CTRL_THREAD_NULL; \
							thread_id.x = i_inner; \
							thread_id.y = j_inner; \
							Ctrl_Kernel_Cpu_##type##_##subtype##_##name( thread_id, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE( argsList, __VA_ARGS__ ) ); \
						} \
					} \
				} \
			} \
		} else { /* Executing in parallel the 1D threads */ \
			_Pragma("omp parallel for num_threads(request.cpu.n_cores)") \
			for (unsigned int i_outer = 0; i_outer < threads.x; i_outer+=blocksize.x) { \
			unsigned int i_max = ((i_outer+blocksize.x) > threads.x) ? threads.x : (i_outer+blocksize.x); \
				for (unsigned int i_inner = i_outer; i_inner < i_max; i_inner++){ \
					Ctrl_Thread thread_id = CTRL_THREAD_NULL; \
					thread_id.x = i_inner; \
					Ctrl_Kernel_Cpu_##type##_##subtype##_##name( thread_id, CTRL_KERNEL_ARG_LIST_ACCESS_KTILE( argsList, __VA_ARGS__ ) );\
				}\
			} \
		} \
	};

/**
 * Block of code that launches a \e GENERIC kernel on \e CPU architecture, this calculates thread ids and calls directly to the 
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
 * Block of code that launches a \e CPU_LIB kernel, this calls directly to the function defined in \e CTRL_KERNEL_CPU_LIB.
 * @hideinitializer
 * 
 * @param name Kernel name.
 * @param argslist List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 * 
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL.
 * @see CTRL_KERNEL_CPU_LIB
 */
#define CTRL_KERNEL_WRAP_CPULIB(name, argslist, type, subtype, ...) \
	{ \
		Ctrl_Kernel_Cpu_##type##_##subtype##_##name(CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(argslist, __VA_ARGS__)); \
	};

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
#define CTRL_KERNEL_DECLARATION_CPU( name, type, subtype, n_params, ... ) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name( Ctrl_Thread thread_id, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params( __VA_ARGS__ ) ); 

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
#define CTRL_KERNEL_DECLARATION_CPULIB( name, type, subtype, n_params, ... ) \
	void Ctrl_Kernel_Cpu_##type##_##subtype##_##name( CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params( __VA_ARGS__ ) ); 

#define CTRL_KERNEL_LAUNCH_POINTERS_CPU( name, type, subtype, ... )

///@endcond
#endif //_CTRL_CPU_KERNELPROTO_H_
