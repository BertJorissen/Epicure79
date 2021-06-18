#ifndef _CTRL_KERNEL_PROTO_H_
#define _CTRL_KERNEL_PROTO_H_
/**
 * @file Ctrl_KernelProto.h
 * @author Trasgo Group
 * Macros to generate the code, from the prototype declaration of a kernel,
 * of the enqueue/dequeue(launching) functions. The launching function is a
 * wrapper to invocate the actual kernel implementations
 * @version 2.1
 * @date 2021-04-26
 * @note Macros to support up to 7 different implementations, and up to 20 different parameters
 *		in a kernel call.
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

#ifdef CTRL_HOST_COMPILE
	#define MODEL_TASK	0
	#define MODEL_NDRANGE	1
#endif

#ifndef CTRL_FPGA_KERNEL_FILE
	#include "Kernel/Ctrl_ImplType.h"
	#include "Kernel/Ctrl_KernelArgs.h"

	#include "Core/Ctrl_TaskQueue.h"
#endif

#ifdef _CTRL_ARCH_CPU_
	#include "Kernel/Architectures/Cpu/Ctrl_Cpu_KernelProto.h"
#else
	#define CTRL_KERNEL_CPU( ... )
	#define CTRL_KERNEL_WRAP_CPU( ... )
	#define CTRL_KERNEL_CPU_LIB( ... )
	#define CTRL_KERNEL_WRAP_CPU_LIB( ... )
	#define CTRL_KERNEL_CPU_GENERIC( ... )
	#define CTRL_KERNEL_WRAP_CPU_GENERIC( ... )
	#define CTRL_KERNEL_LAUNCH_POINTERS_CPU( ... )
	#define CTRL_KERNEL_DECLARATION_CPU( ... )
#endif // _CTRL_ARCH_CPU_

#ifdef _CTRL_ARCH_CUDA_
	#include "Kernel/Architectures/Cuda/Ctrl_Cuda_KernelProto.h"
#else
	#define CTRL_KERNEL_CUDA( ... )
	#define CTRL_KERNEL_WRAP_CUDA( ... )
	#define CTRL_KERNEL_CUDA_LIB( ... )
	#define CTRL_KERNEL_WRAP_CUDA_LIB( ... )
	#define CTRL_KERNEL_CUDA_GENERIC( ... )
	#define CTRL_KERNEL_WRAP_CUDA_GENERIC( ... )
	#define CTRL_KERNEL_LAUNCH_POINTERS_CUDA( ... )
	#define CTRL_KERNEL_DECLARATION_CUDA( ... )
#endif

#ifdef _CTRL_ARCH_OPENCL_GPU_
	#include "Kernel/Architectures/OpenCL/Ctrl_OpenCL_KernelProto.h"
#else
	#define CTRL_KERNEL_OPENCLGPU( ... )
	#define CTRL_KERNEL_WRAP_OPENCLGPU( ... )
	#define CTRL_KERNEL_OPENCLGPULIB( ... )
	#define CTRL_KERNEL_WRAP_OPENCLGPULIB( ... )
	#define CTRL_KERNEL_OPENCLGPU_GENERIC( ... )
	#define CTRL_KERNEL_WRAP_OPENCLGPU_GENERIC( ... )
	#define CTRL_KERNEL_LAUNCH_POINTERS_OPENCLGPU( ... )
	#define CTRL_KERNEL_DECLARATION_OPENCLGPU( ... )
#endif // _CTRL_ARCH_OPENCL_GPU_

#ifdef _CTRL_ARCH_FPGA_
	#include "Kernel/Architectures/FPGA/Ctrl_FPGA_KernelProto.h"
#else
	#define CTRL_KERNEL_FPGA( ... )
	#define CTRL_KERNEL_WRAP_FPGA( ... )
	#define CTRL_KERNEL_FPGA_LIB( ... )
	#define CTRL_KERNEL_WRAP_FPGA_LIB( ... )
	#define CTRL_KERNEL_LAUNCH_POINTERS_FPGA( ... )
	#define CTRL_KERNEL_DECLARATION_FPGA( ... )
#endif // _CTRL_ARCH_FPGA_

/*
 *******************************************************************************************
 ****************************************  KERNEL  *****************************************
 *******************************************************************************************
 */

///@cond INTERNAL
/**
 * Expands to implementation for generic kernel definition for every architecture active. 
 * @hideinitializer
 * 
 * @see CTRL_KERNEL_CPU_GENERIC, CTRL_KERNEL_CUDA_GENERIC, CTRL_KERNEL_OPENCLGPU_GENERIC
 */
#define CTRL_KERNEL_GENERIC( ... ) \
		CTRL_KERNEL_CPU_GENERIC( __VA_ARGS__ ) \
		CTRL_KERNEL_CUDA_GENERIC( __VA_ARGS__ ) \
		CTRL_KERNEL_OPENCLGPU_GENERIC( __VA_ARGS__ )

/**
 * Expands to implementation for generic kernel wrapper for every architecture active. 
 * @hideinitializer
 * 
 * @see CTRL_KERNEL_WRAP_CPU_GENERIC, CTRL_KERNEL_WRAP_CUDA_GENERIC, CTRL_KERNEL_WRAP_OPENCLGPU_GENERIC
 */
#define CTRL_KERNEL_WRAP_GENERIC( ... ) \
		CTRL_KERNEL_WRAP_CPU_GENERIC( __VA_ARGS__ ) \
		CTRL_KERNEL_WRAP_CUDA_GENERIC( __VA_ARGS__ ) \
		CTRL_KERNEL_WRAP_OPENCLGPU_GENERIC( __VA_ARGS__ )

///@endcond

/**
 * Macro used to define a kernel. Expands to type (and architecture) specific kernel definition.
 * @hideinitializer
 * 
 * @param name Kernel's name.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param subtype Kernel subtype (Ctrl_ImplSubType).
 * @param ... Parameters to the kernel and kernel body.
 * 
 * @see Ctrl_ImplType, CTRL_KERNEL_PROTO
 * @if INTERNAL
 * @see CTRL_KERNEL_CPU_GENERIC, CTRL_KERNEL_CUDA_GENERIC, CTRL_KERNEL_OPENCLGPU_GENERIC,
 * CTRL_KERNEL_CPU, CTRL_KERNEL_CUDA, CTRL_KERNEL_OPENCLGPU,
 * CTRL_KERNEL_CPU_LIB, CTRL_KERNEL_CUDA_LIB, CTRL_KERNEL_OPENCLGPULIB
 * @endif
 */
#define CTRL_KERNEL( name, type, subtype, ... ) \
	CTRL_KERNEL_##type( name, type, subtype, __VA_ARGS__ )

/**
 * Macro used to define a kernel with function form (body outside the macro). 
 * Expands to type (and architecture) specific kernel definition.
 * 
 * This is used as an alternative way of defining a kernel that is more similar to regular C syntax, but it is not avaiable
 * for every ctrl type. Thus \e CTRL_KERNEL is more general and remains as the preferred method.
 * @hideinitializer
 * 
 * @param name Kernel's name.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param subtype Kernel subtype (Ctrl_ImplSubType).
 * @param ... Parameters to the kernel.
 * 
 * @pre \p type must be CPU, CUDA or FPGA other types must use \e CTRL_KERNEL instead.
 * @see Ctrl_ImplType, CTRL_KERNEL_PROTO
 * @if INTERNAL
 * @see CTRL_KERNEL_FUNCTION_CPU, CTRL_KERNEL_FUNCTION_CUDA, CTRL_KERNEL_FUNCTION_FPGA
 * @endif
 */
#define CTRL_KERNEL_FUNCTION( name, type, subtype, ... ) \
	CTRL_KERNEL_FUNCTION_##type( name, type, subtype, __VA_ARGS__ )

/**
 * Kernel end marker.
 * Used to mark the end of a kernel.
 * @hideinitializer
 * 
 * @param type type of the kernel.
 */
#define CTRL_KERNEL_END( type ) }

///@cond INTERNAL

/* Case structure to call/launch one of the kernel implementations */
#define CTRL_KERNEL_WRAP_LAUNCH_1( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, __VA_ARGS__ ) break; 
#define CTRL_KERNEL_WRAP_LAUNCH_2( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 1, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_1( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_3( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 2, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_2( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_4( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 3, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_3( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_5( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 4, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_4( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_6( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 5, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_5( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_7( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 6, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_6( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_8( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 7, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_7( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_9( name, argsList, type, subtype, ... )		case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 8, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_8( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_10( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 9, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_9( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_11( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 10, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_10( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_12( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 11, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_11( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_13( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 12, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_12( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_14( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 13, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_13( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_15( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 14, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_14( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_16( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 15, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_15( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_17( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 16, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_16( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_18( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 17, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_17( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_19( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 18, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_18( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_20( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 19, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_19( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_21( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 20, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_20( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_22( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 21, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_21( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_23( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 22, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_22( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_24( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 23, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_23( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_25( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 24, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_24( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_26( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 25, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_25( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_27( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 26, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_26( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_28( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 27, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_27( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_29( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 28, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_28( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_30( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 29, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_29( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_31( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 30, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_30( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_32( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 31, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_31( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_33( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 32, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_32( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_34( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 33, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_33( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_35( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 34, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_34( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_36( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 35, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_35( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_37( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 36, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_36( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_38( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 37, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_37( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_39( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 38, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_38( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_40( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 39, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_39( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_41( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 40, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_40( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_42( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 41, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_41( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_43( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 42, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_42( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_44( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 43, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_43( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_45( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 44, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_44( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_46( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 45, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_45( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_47( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 46, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_46( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_48( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 47, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_47( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_49( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 48, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_48( name, argsList, __VA_ARGS__ )
#define CTRL_KERNEL_WRAP_LAUNCH_50( name, argsList, type, subtype, ... )	case type##_##subtype: CTRL_KERNEL_WRAP_##type( name, argsList, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 49, __VA_ARGS__ ) ) break; CTRL_KERNEL_WRAP_LAUNCH_49( name, argsList, __VA_ARGS__ )

#ifdef CTRL_FPGA_KERNEL_FILE
/* Case structure to define one of the kernel implementations */
#define CTRL_KERNEL_WRAP_DEFINE( name, type, subtype, ... )	\
	CTRL_KERNEL_WRAP_##type( name, type, subtype, __VA_ARGS__ )
#endif

#define CTRL_KERNEL_LAUNCH_POINTERS( name, type, subtype, ... ) \
	CTRL_KERNEL_LAUNCH_POINTERS_##type( name, type, subtype, __VA_ARGS__ )

#define CTRL_KERNEL_LAUNCH_POINTERS_GENERIC( name, type, subtype, ... )

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

/**
 * Kernel declaration for host code or header files.
 * Used in Cuda implementations to declare the kernel prototype in included
 * header files, as the kernel defintions may be written in a separate file from the host code.
 * Also used for OpenCL GPU to define constructor function to preload kernels, and on FPGA to declare variables related to the kernel.
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION_##type( name, type, subtype, __VA_ARGS__ ) 

#define CTRL_KERNEL_DECLARATION_1( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_2( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 1, __VA_ARGS__ ) ) \
	CTRL_KERNEL_DECLARATION_1( name, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_3( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 2, __VA_ARGS__ ) ) \
	CTRL_KERNEL_DECLARATION_2( name, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_4( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 3, __VA_ARGS__ ) ) \
	CTRL_KERNEL_DECLARATION_3( name, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_5( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 4, __VA_ARGS__ ) ) \
	CTRL_KERNEL_DECLARATION_4( name, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_6( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 5, __VA_ARGS__ ) ) \
	CTRL_KERNEL_DECLARATION_5( name, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_7( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION( name, type, subtype, CTRL_KERNEL_SKIP_IMPL( ROLED, NULL, 6, __VA_ARGS__ ) ) \
	CTRL_KERNEL_DECLARATION_6( name, __VA_ARGS__ )

#define CTRL_KERNEL_DECLARATION_GENERIC( name, type, subtype, ... ) \
	CTRL_KERNEL_DECLARATION_FPGA( name, type, subtype, __VA_ARGS__ ) \
	CTRL_KERNEL_DECLARATION_CPU( name, type, subtype, __VA_ARGS__ ) \
	CTRL_KERNEL_DECLARATION_CUDA( name, type, subtype, __VA_ARGS__ ) \
	CTRL_KERNEL_DECLARATION_OPENCLGPU( name, type, subtype, __VA_ARGS__ )

/*
 *******************************************************************************************
 ***************************************  MASKs   ******************************************
 *******************************************************************************************
 */

#define CTRL_KERNEL_MASK_1(list, type, subtype, ...)	list[0 ] = type##_##subtype;
#define CTRL_KERNEL_MASK_2(list, type, subtype, ...)	list[1 ] = type##_##subtype; CTRL_KERNEL_MASK_1(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_3(list, type, subtype, ...)	list[2 ] = type##_##subtype; CTRL_KERNEL_MASK_2(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_4(list, type, subtype, ...)	list[3 ] = type##_##subtype; CTRL_KERNEL_MASK_3(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_5(list, type, subtype, ...)	list[4 ] = type##_##subtype; CTRL_KERNEL_MASK_4(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_6(list, type, subtype, ...)	list[5 ] = type##_##subtype; CTRL_KERNEL_MASK_5(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_7(list, type, subtype, ...)	list[6 ] = type##_##subtype; CTRL_KERNEL_MASK_6(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_8(list, type, subtype, ...)	list[7 ] = type##_##subtype; CTRL_KERNEL_MASK_7(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_9(list, type, subtype, ...)	list[8 ] = type##_##subtype; CTRL_KERNEL_MASK_8(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_10(list, type, subtype, ...)	list[9 ] = type##_##subtype; CTRL_KERNEL_MASK_9(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_11(list, type, subtype, ...)	list[10] = type##_##subtype; CTRL_KERNEL_MASK_10(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_12(list, type, subtype, ...)	list[11] = type##_##subtype; CTRL_KERNEL_MASK_11(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_13(list, type, subtype, ...)	list[12] = type##_##subtype; CTRL_KERNEL_MASK_12(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_14(list, type, subtype, ...)	list[13] = type##_##subtype; CTRL_KERNEL_MASK_13(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_15(list, type, subtype, ...)	list[14] = type##_##subtype; CTRL_KERNEL_MASK_14(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_16(list, type, subtype, ...)	list[15] = type##_##subtype; CTRL_KERNEL_MASK_15(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_17(list, type, subtype, ...)	list[16] = type##_##subtype; CTRL_KERNEL_MASK_16(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_18(list, type, subtype, ...)	list[17] = type##_##subtype; CTRL_KERNEL_MASK_17(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_19(list, type, subtype, ...)	list[18] = type##_##subtype; CTRL_KERNEL_MASK_18(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_20(list, type, subtype, ...)	list[19] = type##_##subtype; CTRL_KERNEL_MASK_19(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_21(list, type, subtype, ...)	list[20] = type##_##subtype; CTRL_KERNEL_MASK_20(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_22(list, type, subtype, ...)	list[21] = type##_##subtype; CTRL_KERNEL_MASK_21(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_23(list, type, subtype, ...)	list[22] = type##_##subtype; CTRL_KERNEL_MASK_22(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_24(list, type, subtype, ...)	list[23] = type##_##subtype; CTRL_KERNEL_MASK_23(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_25(list, type, subtype, ...)	list[24] = type##_##subtype; CTRL_KERNEL_MASK_24(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_26(list, type, subtype, ...)	list[25] = type##_##subtype; CTRL_KERNEL_MASK_25(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_27(list, type, subtype, ...)	list[26] = type##_##subtype; CTRL_KERNEL_MASK_26(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_28(list, type, subtype, ...)	list[27] = type##_##subtype; CTRL_KERNEL_MASK_27(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_29(list, type, subtype, ...)	list[28] = type##_##subtype; CTRL_KERNEL_MASK_28(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_30(list, type, subtype, ...)	list[29] = type##_##subtype; CTRL_KERNEL_MASK_29(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_31(list, type, subtype, ...)	list[30] = type##_##subtype; CTRL_KERNEL_MASK_30(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_32(list, type, subtype, ...)	list[31] = type##_##subtype; CTRL_KERNEL_MASK_31(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_33(list, type, subtype, ...)	list[32] = type##_##subtype; CTRL_KERNEL_MASK_32(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_34(list, type, subtype, ...)	list[33] = type##_##subtype; CTRL_KERNEL_MASK_33(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_35(list, type, subtype, ...)	list[34] = type##_##subtype; CTRL_KERNEL_MASK_34(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_36(list, type, subtype, ...)	list[35] = type##_##subtype; CTRL_KERNEL_MASK_35(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_37(list, type, subtype, ...)	list[36] = type##_##subtype; CTRL_KERNEL_MASK_36(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_38(list, type, subtype, ...)	list[37] = type##_##subtype; CTRL_KERNEL_MASK_37(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_39(list, type, subtype, ...)	list[38] = type##_##subtype; CTRL_KERNEL_MASK_38(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_40(list, type, subtype, ...)	list[39] = type##_##subtype; CTRL_KERNEL_MASK_39(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_41(list, type, subtype, ...)	list[40] = type##_##subtype; CTRL_KERNEL_MASK_40(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_42(list, type, subtype, ...)	list[41] = type##_##subtype; CTRL_KERNEL_MASK_41(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_43(list, type, subtype, ...)	list[42] = type##_##subtype; CTRL_KERNEL_MASK_42(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_44(list, type, subtype, ...)	list[43] = type##_##subtype; CTRL_KERNEL_MASK_43(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_45(list, type, subtype, ...)	list[44] = type##_##subtype; CTRL_KERNEL_MASK_44(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_46(list, type, subtype, ...)	list[45] = type##_##subtype; CTRL_KERNEL_MASK_45(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_47(list, type, subtype, ...)	list[46] = type##_##subtype; CTRL_KERNEL_MASK_46(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_48(list, type, subtype, ...)	list[47] = type##_##subtype; CTRL_KERNEL_MASK_47(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_49(list, type, subtype, ...)	list[48] = type##_##subtype; CTRL_KERNEL_MASK_48(list, __VA_ARGS__)
#define CTRL_KERNEL_MASK_50(list, type, subtype, ...)	list[49] = type##_##subtype; CTRL_KERNEL_MASK_49(list, __VA_ARGS__)

///@endcond
/*
 *******************************************************************************************
 ***************************************  PROTO  *******************************************
 *******************************************************************************************
 */

#ifndef CTRL_FPGA_KERNEL_FILE
	/** 
	 * Prototype and implementations declaration for a kernel. 
	 * @hideinitializer
	 * 
	 * @param name kernel's name.
	 * @param n_implementations number of different implementations for this kernel.
	 * @param ... types of implementations avaiable for the kernel, number of parameters for the kernel, list of parameters for .
	 * the kernel in the form (ROL, TYPE, NAME)
	 * 
	 * @see CTRL_KERNEL
	 * @if INTERNAL
	 * Define functions to create \e Ctrl_Task for this kernel and wrapper to launch the kernel.
	 * 
	 * 1st: expand launch macros for all implementations provided.
	 * 2nd: define kernel wrapper function. This is a switch block to expand to the appropiate implementation wrapper macro.
	 * 3rd: define task creation function. This creates and returns a \e Ctrl_Task to launch this kernel
	 * 
	 * @see CTRL_KERNEL_WRAP_GENERIC, CTRL_KERNEL_WRAP_CPU, CTRL_KERNEL_WRAP_CUDA, CTRL_KERNEL_WRAP_OPENCLGPU, Ctrl_Task, 
	 * CTRL_KERNEL_DECLARATION
	 * @endif
	 */ 
	#define CTRL_KERNEL_PROTO( name, n_implementations, ... ) \
		CTRL_KERNEL_DECLARATION_##n_implementations( name, __VA_ARGS__ ) \
		\
		void Ctrl_KernelWrapper_##name( Ctrl_Request request, int device_id, Ctrl_Thread threads, Ctrl_Thread blocksize, void * args_list ) { \
			switch ( device_id ) { \
				CTRL_KERNEL_WRAP_LAUNCH_##n_implementations( name, args_list, __VA_ARGS__ ) \
				default: \
				fprintf(stderr, "Ctrl Internal error: Wrong implementation type on launching wrapper: %s, %s[%d]\n", #name, __FILE__, __LINE__ ); \
			} \
		} \
		\
		Ctrl_Task Ctrl_KernelTaskCreate_##name( Ctrl_Type ctrl_type, Ctrl_Thread threads, Ctrl_Thread blocksize, CTRL_KERNEL_SKIP_IMPL( TYPED_POINTER, NULL, n_implementations, __VA_ARGS__ ) ) { \
			Ctrl_Task task = CTRL_TASK_NULL; \
			task.threads = threads; \
			task.blocksize = blocksize; \
			task.task_type = CTRL_TASK_TYPE_KERNEL; \
			task.n_arguments = CTRL_KERNEL_SKIP_IMPL( ARG_COUNT_LIST_ELEMENTS, NULL, n_implementations, __VA_ARGS__ ); \
			task.pfn_kernel_wrapper = Ctrl_KernelWrapper_##name; \
			CTRL_KERNEL_SKIP_IMPL( ARG_LIST_CREATE_KTILE, task.p_arguments, n_implementations, __VA_ARGS__); \
			CTRL_KERNEL_SKIP_IMPL( ROLES, task.p_roles, n_implementations, __VA_ARGS__); \
			CTRL_KERNEL_SKIP_IMPL( POINTERS, task.pp_pointers, n_implementations, __VA_ARGS__); \
			CTRL_KERNEL_SKIP_IMPL( DISPLACEMENTS, task.p_displacements, n_implementations, __VA_ARGS__); \
			int avail_impl[n_implementations]; \
			CTRL_KERNEL_MASK_##n_implementations(avail_impl, __VA_ARGS__); \
			int dev = Ctrl_Dev(ctrl_type, avail_impl, n_implementations); \
			task.device_id = dev; \
			return task; \
		}
#else
	#define CTRL_KERNEL_PROTO( name, n_implementations, ... ) \
		CTRL_KERNEL_WRAP_DEFINE( name, __VA_ARGS__ )
#endif

/*
 *******************************************************************************************
 ***************************************  HOST TASKS  **************************************
 *******************************************************************************************
 */

/**
 * Macro used to define a host task.
 * @hideinitializer
 * 
 * @param name host task's name.
 * @param ... Parameters to the host task.
 * 
 * @see CTRL_HOST_TASK_PROTO
 */
#define CTRL_HOST_TASK( name, ... ) \
	void Ctrl_Host_Task_##name(__VA_ARGS__) 
		
/** 
 * Prototype and implementations declaration for a host task. 
 * @hideinitializer
 * 
 * @param name host task's name.
 * @param n_args number of arguments for this host task.
 * @param ... list of parameters for the host task in the form (ROL, TYPE, NAME).
 * 
 * @see CTRL_HOST_TASK
 * @if INTERNAL
 * Define functions to create \e Ctrl_Task for this host task and wrapper to launch the host task.
 * 
 * 1st: define host task wrapper function. This calls to the function defined by \e CTRL_HOST_TASK.
 * 2nd: define task creation function. This creates and returns a \e Ctrl_Task to launch this host task
 * 
 * @see Ctrl_Task
 * @endif
 */ 
#define CTRL_HOST_TASK_PROTO( name, n_args, ... ) \
	void Ctrl_HostWrapper_##name( void * args_list ) { \
		Ctrl_Host_Task_##name( CTRL_KERNEL_ARG_LIST_ACCESS_TILE( args_list, n_args, __VA_ARGS__ ) ); \
	} \
	\
	Ctrl_Task Ctrl_HostTaskCreate_##name(CTRL_KERNEL_TYPED_POINTER( NULL, n_args, __VA_ARGS__ )) { \
		Ctrl_Task task = CTRL_TASK_NULL; \
		task.task_type = CTRL_TASK_TYPE_HOST; \
		task.n_arguments = n_args; \
		task.pfn_hostTask_wrapper = Ctrl_HostWrapper_##name; \
		CTRL_KERNEL_ARG_LIST_CREATE_TILE( task.p_arguments, n_args, __VA_ARGS__ ); \
		CTRL_KERNEL_ROLES( task.p_roles, n_args, __VA_ARGS__ ); \
		CTRL_KERNEL_POINTERS( task.pp_pointers, n_args, __VA_ARGS__ ); \
		CTRL_KERNEL_DISPLACEMENTS_TILES( task.p_displacements, n_args, __VA_ARGS__ ); \
		task.device_id = -1; \
		return task; \
	}
		
#endif // _CTRL_KERNEL_PROTO_H_
