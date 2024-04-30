#ifndef _CTRL_FPGA_KERNELPROTO_H_
#define _CTRL_FPGA_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_KernelProto.h
 * @author Gabriel Rodriguez-Canal
 * @brief Macros to generate the code and manage FPGA kernels.
 * @version 4.0
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

// @sergioalo formatter does not respect ifdef indentation so leaving it off in this file
// clang-format off
#ifdef CTRL_HOST_COMPILE
	#include <string.h>
#endif

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#define ERR_NOT_FOUND -1
#define ERR_READ -2

#ifdef CTRL_HOST_COMPILE
	#include <CL/cl.h>
#endif

#include "Kernel/Ctrl_KernelArgs.h"

#ifndef CTRL_FPGA_KERNEL_FILE
	#include "Kernel/Ctrl_Thread.h"

	#include "Core/Ctrl_Request.h"

	#include "Architectures/FPGA/Ctrl_FPGA.h"
	#include "Architectures/FPGA/Ctrl_FPGA_Request.h"
#endif

#define CTRL_EXTRACT_PIPELINE(...) __VA_ARGS__

#define CTRL_EXTRACT_PIPELINE_N(n_args, ...) \
	_CTRL_EXTRACT_PIPELINE_N(n_args, __VA_ARGS__)

#define _CTRL_EXTRACT_PIPELINE_N(n_args, ...) \
	CTRL_EXTRACT_PIPELINE_##n_args(__VA_ARGS__)

#define CTRL_EXTRACT_PIPELINE_1(arg)       CTRL_EXTRACT_##arg
#define CTRL_EXTRACT_PIPELINE_2(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_1(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_3(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_2(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_4(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_3(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_5(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_4(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_6(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_5(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_7(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_6(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_8(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_7(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_9(arg, ...)  CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_8(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_10(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_9(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_11(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_10(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_12(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_11(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_13(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_12(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_14(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_13(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_15(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_14(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_16(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_15(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_17(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_16(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_18(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_17(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_19(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_18(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_20(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_19(__VA_ARGS__)
#define CTRL_EXTRACT_PIPELINE_21(arg, ...) CTRL_EXTRACT_##arg CTRL_EXTRACT_PIPELINE_20(__VA_ARGS__)

#ifdef CTRL_FPGA_KERNEL_FILE
	#ifndef CTRL_HOST_COMPILE
		/*
		* CTRL_FPGA_BIN_NAME is used to extract the number of the FPGA bitstream, composed of the kernel name and its parameters (SIMD, CU, etc.)
		*/
		#ifdef CTRL_FPGA_BIN_NAME
			#define CPARAMS_BUFF 512

			char *cpar_kname = NULL;
			char *cpar_simd  = NULL;
			char *cpar_cu    = NULL;

			#define CTRL_EXTRACT_CU(cu)                     \
				__attribute((constructor)) void Init_cu() { \
					strcat(cpar_cu, "_CU-");                \
					strcat(cpar_cu, #cu);                   \
				}

			#define CTRL_EXTRACT_TASK

			#define CTRL_EXTRACT_SIMD(simd, wg_x, wg_y, wg_z)                \
				__attribute((constructor)) void Init_simd() {                \
					cpar_simd = (char *)malloc(CPARAMS_BUFF * sizeof(char)); \
					strcpy(cpar_simd, "_SIMD-");                             \
					strcat(cpar_simd, #simd);                                \
					strcat(cpar_simd, "-");                                  \
					strcat(cpar_simd, #wg_x);                                \
					strcat(cpar_simd, "x");                                  \
					strcat(cpar_simd, #wg_y);                                \
					strcat(cpar_simd, "x");                                  \
					strcat(cpar_simd, #wg_z);                                \
				}

			#define CTRL_KERNEL_FUNCTION_FPGA(name, type, subtype, pipeline, roles, ...) \
				__attribute((constructor)) void Init_name() {                            \
					cpar_kname = #name;                                                  \
				}                                                                        \
				CTRL_EXTRACT_PIPELINE_N(CTRL_COUNTPARAM(CTRL_EXTRACT_##pipeline), CTRL_EXTRACT_##pipeline)
		#else // CTRL_FPGA_BIN_NAME
			#define CTRL_EXTRACT_CU(cu) \
						__attribute__((num_compute_units(cu))

			#define CTRL_EXTRACT_TASK \
				__attribute__((max_global_work_dim(0)))

			#define CTRL_EXTRACT_SIMD(simd, wg_x, wg_y, wg_z)           \
				__attribute__((reqd_work_group_size(wg_x, wg_y, wg_z))) \
				__attribute__((num_simd_work_items(simd)))

			#define CTRL_KERNEL_FUNCTION_FPGA(name, type, subtype, pipeline, roles, ...)                   \
				CTRL_EXTRACT_PIPELINE_N(CTRL_COUNTPARAM(CTRL_EXTRACT_##pipeline), CTRL_EXTRACT_##pipeline) \
				__kernel void ctrl_kernel_fpga_##type##_##subtype##_##name(CTRL_KERNEL_SIGNATURE(CTRL_COUNTPARAM(__VA_ARGS__), EXTRACT_##roles, __VA_ARGS__))
		#endif // CTRL_FPGA_BIN_NAME

		#define CTRL_KERNEL_FPGA_PARSE_ARGS_1(role, type, name)       CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_2(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_1(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_3(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_2(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_4(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_3(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_5(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_4(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_6(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_5(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_7(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_6(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_8(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_7(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_9(role, type, name, ...)  CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_8(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_10(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_9(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_11(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_10(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_12(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_11(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_13(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_12(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_14(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_13(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_15(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_14(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_16(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_15(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_17(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_16(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_18(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_17(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_19(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_18(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_20(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_19(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_21(role, type, name, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role(type, name), CTRL_KERNEL_FPGA_PARSE_ARGS_20(__VA_ARGS__)

		#define _CTRL_KERNEL_FPGA_PARSE_ARGS(n, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##n(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_PARSE_ARGS(n, ...)  _CTRL_KERNEL_FPGA_PARSE_ARGS(n, __VA_ARGS__)

		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_1(arg)       arg
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_2(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_1(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_3(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_2(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_4(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_3(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_5(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_4(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_6(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_5(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_7(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_6(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_8(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_7(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_9(arg, ...)  arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_8(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_10(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_9(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_11(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_10(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_12(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_11(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_13(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_12(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_14(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_13(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_15(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_14(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_16(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_15(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_17(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_16(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_18(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_17(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_19(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_18(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_20(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_19(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS_21(arg, ...) arg, CTRL_KERNEL_FPGA_EXTRACT_ARGS_20(__VA_ARGS__)

		#define _CTRL_KERNEL_FPGA_EXTRACT_ARGS(n, ...) CTRL_KERNEL_FPGA_EXTRACT_ARGS_##n(__VA_ARGS__)
		#define CTRL_KERNEL_FPGA_EXTRACT_ARGS(n, ...)  _CTRL_KERNEL_FPGA_EXTRACT_ARGS(n, __VA_ARGS__)

		#define KTILE(name, type)                                                   \
			KHitTile_##type name;                                                   \
			name.data                                             = name##_data;    \
			*((fpga_wrapper_KHitTile_##type *)&name.origAcumCard) = name##_wrapper; \
			name.data += name.offset;

		#define CTRL_EXTRACT_NDRANGE

		/* @author: Gabriel Rodriguez-Canal
		@brief: KHitTiles are unfolded into a KHitTile wrapper and a data pointer (structures with pointers cannot be passed as kernel arguments) */
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_INVAL(arg) const arg
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_IN(arg)    fpga_wrapper_##arg##_wrapper, data_##arg##_data
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_OUT(arg)   fpga_wrapper_##arg##_wrapper, data_##arg##_data
		#define CTRL_KERNEL_FPGA_PARSE_ARGS_IO(arg)    fpga_wrapper_##arg##_wrapper, data_##arg##_data

		#define EXTRACT_PARAMS(...) __VA_ARGS__

		#define CTRL_KERNEL_SIGNATURE(n_args, ...)  _CTRL_KERNEL_SIGNATURE(n_args, __VA_ARGS__)
		#define _CTRL_KERNEL_SIGNATURE(n_args, ...) CTRL_KERNEL_SIGNATURE_##n_args(__VA_ARGS__)

		#define CTRL_KERNEL_SIGNATURE_1(role1, arg1, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role1(arg1)
		#define CTRL_KERNEL_SIGNATURE_2(role2, role1, arg2, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role2(arg2), CTRL_KERNEL_SIGNATURE_1(role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_3(role3, role2, role1, arg3, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role3(arg3), CTRL_KERNEL_SIGNATURE_2(role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_4(role4, role3, role2, role1, arg4, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role4(arg4), CTRL_KERNEL_SIGNATURE_3(role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_5(role5, role4, role3, role2, role1, arg5, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role5(arg5), CTRL_KERNEL_SIGNATURE_4(role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_6(role6, role5, role4, role3, role2, role1, arg6, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role6(arg6), CTRL_KERNEL_SIGNATURE_5(role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_7(role7, role6, role5, role4, role3, role2, role1, arg7, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role7(arg7), CTRL_KERNEL_SIGNATURE_6(role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_8(role8, role7, role6, role5, role4, role3, role2, role1, arg8, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role8(arg8), CTRL_KERNEL_SIGNATURE_7(role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_9(role9, role8, role7, role6, role5, role4, role3, role2, role1, arg9, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role9(arg9), CTRL_KERNEL_SIGNATURE_8(role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_10(role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg10, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role10(arg10), CTRL_KERNEL_SIGNATURE_9(role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_11(role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg11, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role11(arg11), CTRL_KERNEL_SIGNATURE_10(role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_12(role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg12, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role12(arg12), CTRL_KERNEL_SIGNATURE_11(role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_13(role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg13, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role13(arg13), CTRL_KERNEL_SIGNATURE_12(role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_14(role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg14, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role14(arg14), CTRL_KERNEL_SIGNATURE_13(role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_15(role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg15, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role15(arg15), CTRL_KERNEL_SIGNATURE_14(role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_16(role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg16, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role16(arg16), CTRL_KERNEL_SIGNATURE_15(role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_17(role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg17, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role17(arg17), CTRL_KERNEL_SIGNATURE_16(role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_18(role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg18, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role18(arg18), CTRL_KERNEL_SIGNATURE_17(role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_19(role19, role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg19, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role19(arg19), CTRL_KERNEL_SIGNATURE_18(role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_20(role20, role19, role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg20, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role20(arg20), CTRL_KERNEL_SIGNATURE_19(role19, role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)
		#define CTRL_KERNEL_SIGNATURE_21(role21, role20, role19, role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, arg21, ...) CTRL_KERNEL_FPGA_PARSE_ARGS_##role21(arg21), CTRL_KERNEL_SIGNATURE_20(role20, role19, role18, role17, role16, role15, role14, role13, role12, role11, role10, role9, role8, role7, role6, role5, role4, role3, role2, role1, __VA_ARGS__)

	#else // CTRL_HOST_COMPILE
		int kernel_model;
		#define CPARAMS_BUFF 512
		char *cpar_kname = "";
		char *cpar_simd  = "";
		char *cpar_cu    = "";

		#define CTRL_EXTRACT_NDRANGE                             \
			__attribute__((constructor)) void setKernelModel() { \
				kernel_model = MODEL_NDRANGE;                    \
			}

		#define CTRL_EXTRACT_TASK                                \
			__attribute__((constructor)) void setKernelModel() { \
				kernel_model = MODEL_TASK;                       \
			}

		#define CTRL_EXTRACT_SIMD(simd, wg_x, wg_y, wg_z)                \
			__attribute((constructor)) void Init_simd() {                \
				cpar_simd = (char *)malloc(CPARAMS_BUFF * sizeof(char)); \
				strcpy(cpar_simd, "_SIMD-");                             \
				strcat(cpar_simd, #simd);                                \
				strcat(cpar_simd, "-");                                  \
				strcat(cpar_simd, #wg_x);                                \
				strcat(cpar_simd, "x");                                  \
				strcat(cpar_simd, #wg_y);                                \
				strcat(cpar_simd, "x");                                  \
				strcat(cpar_simd, #wg_z);                                \
			}

		#define CTRL_EXTRACT_CU(cu)                     \
			__attribute((constructor)) void Init_cu() { \
				strcpy(cpar_cu, "_CU-");                \
				strcat(cpar_cu, #cu);                   \
			}

		#ifdef FPGA_PROFILING
		#define CTRL_KERNEL_MODE "_profiling"
		#elif FPGA_EMULATION
		#define CTRL_KERNEL_MODE "_emu"
		#else
		#define CTRL_KERNEL_MODE ""
		#endif

		#define CTRL_KERNEL_FUNCTION_FPGA(name, type, subtype, pipeline, ...)                          \
			CTRL_EXTRACT_PIPELINE_N(CTRL_COUNTPARAM(CTRL_EXTRACT_##pipeline), CTRL_EXTRACT_##pipeline) \
			Ctrl_FPGA_KernelParams ctrl_kernel_fpga_##type##_##subtype##_##name = (Ctrl_FPGA_KernelParams){ \
				 .p_kernel      = NULL,                                                                \
				 .p_program     = NULL,                                                                \
				 .p_kernel_name = CTRL_MACRO_STRINGIFY(ctrl_kernel_fpga_##type##_##subtype##_##name),  \
				 .p_binary_name = NULL,                                                                \
				 .p_next        = NULL};                                                               \
																									   \
			__attribute__((constructor)) static void Ctrl_InitKernel_##type##_##subtype##_##name() {   \
				/* FILE *binary_file; */                                                               \
				char *kernel_path = (char *)malloc(CTRL_KERNEL_PATH_LENGTH * sizeof(char));;           \
				kernel_path[0]    = '\0';                                                              \
				strcat(kernel_path, CTRL_MACRO_STRINGIFY(CTRL_KERNEL_PATH));                           \
				strcat(kernel_path, CTRL_MACRO_STRINGIFY(name));                                       \
				strcat(kernel_path, "/");                                                              \
				strcat(kernel_path, #name);                                                            \
				strcat(kernel_path, cpar_simd);                                                        \
																									   \
				Ctrl_FPGA_KernelParams *curr_k_par = &FPGA_initial_kp;                                 \
				while (curr_k_par->p_next != NULL)                                                     \
					curr_k_par = curr_k_par->p_next;                                                   \
																									   \
				ctrl_kernel_fpga_##type##_##subtype##_##name.p_binary_name = kernel_path;              \
				curr_k_par->p_next = &ctrl_kernel_fpga_##type##_##subtype##_##name;                    \
			}
	#endif // CTRL_HOST_COMPILE
#else // CTRL_FPGA_KERNEL_FILE
	#define CTRL_EXTRACT_NDRANGE

	#define CTRL_EXTRACT_CU(cu)                     \
		__attribute((constructor)) void Init_cu() { \
			strcat(cpar_cu, "_CU-");                \
			strcat(cpar_cu, #cu);                   \
		}

	#define CTRL_EXTRACT_TASK \
		__attribute__((max_global_work_dim(0)))

	#define CTRL_EXTRACT_SIMD(simd, wg_x, wg_y, wg_z)                \
		__attribute((constructor)) void Init_simd() {                \
			cpar_simd = (char *)malloc(CPARAMS_BUFF * sizeof(char)); \
			strcpy(cpar_simd, "_SIMD-");                             \
			strcat(cpar_simd, #simd);                                \
			strcat(cpar_simd, "-");                                  \
			strcat(cpar_simd, #wg_x);                                \
			strcat(cpar_simd, "x");                                  \
			strcat(cpar_simd, #wg_y);                                \
			strcat(cpar_simd, "x");                                  \
			strcat(cpar_simd, #wg_z);                                \
		}

	/* @author: Gabriel Rodriguez-Canal \
	@brief: CTRL_PARSE_ARGS generates the arguments passed to the kernel. Recall they must be passed by value in the case of INVAL while \
	IN and OUT must be split in a wrapper and a pointer to data. The wrapper will be of the form fpga_wrapper_KHitTile_<type> <name>_wrapper. \
	This macro is leveraged to have both the type of the wrapper and the index of the displacement at the moment of the creation of the arguments. \
	Take into account the macro arguments type and name are referring to the type and the name of the kernel argument and not those of the kernel \
	as in the case of CTRL_KERNEL_WRAP_FPGA. For that reason, the kernel name must also be passed. \
	*/
	#define CTRL_PARSE_ARGS(kernel, n_args, ...) CTRL_PARSE_ARGS_##n_args(kernel, 0, __VA_ARGS__)

	#define CTRL_PARSE_ARGS_1(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name)
	#define CTRL_PARSE_ARGS_2(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_1(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_3(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_2(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_4(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_3(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_5(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_4(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_6(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_5(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_7(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_6(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_8(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_7(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_9(kernel, arg_idx, role, type, name, ...)  CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_8(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_10(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_9(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_11(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_10(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_12(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_11(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_13(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_12(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_14(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_13(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_15(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_14(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_16(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_15(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_17(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_16(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_18(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_17(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_19(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_18(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_20(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_19(kernel, arg_idx + 1, __VA_ARGS__)
	#define CTRL_PARSE_ARGS_21(kernel, arg_idx, role, type, name, ...) CTRL_PARSE_ARGS_##role(kernel, arg_idx, type, name) CTRL_PARSE_ARGS_20(kernel, arg_idx + 1, __VA_ARGS__)

	#define CTRL_PARSE_ARGS_IN(kernel, arg_idx, type, name)                                                                                               \
		KHitTile            *p_ktile_##type##_##subtype##_##name            = (KHitTile *)((uint8_t *)args_list + request.fpga.p_displacements[arg_idx]); \
		fpga_wrapper_K##type ktile_fpga_wrapper_##type##_##subtype##_##name = {                                                                           \
			.origAcumCard = {p_ktile_##type##_##subtype##_##name->origAcumCard[0],                                                                        \
							p_ktile_##type##_##subtype##_##name->origAcumCard[1],                                                                         \
							p_ktile_##type##_##subtype##_##name->origAcumCard[2],                                                                         \
							p_ktile_##type##_##subtype##_##name->origAcumCard[3]},                                                                        \
			.card         = {p_ktile_##type##_##subtype##_##name->card[0],                                                                                \
							p_ktile_##type##_##subtype##_##name->card[1],                                                                                 \
							p_ktile_##type##_##subtype##_##name->card[2]},                                                                                \
			.offset       = p_ktile_##type##_##subtype##_##name->offset};                                                                                 \
		err |= clSetKernelArg(kernel, arg_pos, sizeof(fpga_wrapper_K##type), &ktile_fpga_wrapper_##type##_##subtype##_##name);                            \
		arg_pos++;                                                                                                                                        \
		err |= clSetKernelArg(kernel, arg_pos, sizeof(cl_mem), (cl_mem *)(p_ktile_##type##_##subtype##_##name->data));                                    \
		arg_pos++;

	#define CTRL_PARSE_ARGS_OUT(kernel, arg_idx, type, name) \
		CTRL_PARSE_ARGS_IN(kernel, arg_idx, type, name)

	#define CTRL_PARSE_ARGS_IO(kernel, arg_idx, type, name) \
		CTRL_PARSE_ARGS_IN(kernel, arg_idx, type, name)

	#define CTRL_PARSE_ARGS_INVAL(kernel, arg_idx, type, name)                                                 \
		err |= clSetKernelArg(kernel, arg_pos,                                                                 \
							request.fpga.p_displacements[arg_idx + 1] - request.fpga.p_displacements[arg_idx], \
							((uint8_t *)args_list + request.fpga.p_displacements[arg_idx]));                   \
		arg_pos++;
#endif // CTRL_FPGA_KERNEL_FILE

/**
 * Defines stuff needed for a \e FPGALIB type kernel launch from the information passed by the user on the kernel definition.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_FPGALIB
 * @todo FPGA lib kernels are not fully implemented
 */
#define CTRL_KERNEL_FPGALIB(name, type, subtype, ...)                                          \
	void Ctrl_Kernel_FPGA_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                         \
	}

#ifndef CTRL_FPGA_KERNEL_FILE
#define CTRL_KERNEL_WRAP_FPGA(name, args_list, type, subtype, n_args, ...)                                                          \
	{                                                                                                                               \
		cl_int err     = 0;                                                                                                         \
		int    arg_pos = 0;                                                                                                         \
		CTRL_PARSE_ARGS(ctrl_kernel_fpga_##type##_##subtype##_##name.p_kernel[request.fpga.type_id], n_args, __VA_ARGS__)                \
		if (kernel_model == MODEL_NDRANGE) {                                                                                        \
			size_t global_size[3];                                                                                                  \
			if (threads.dims == 1) {                                                                                                \
				global_size[0] = threads.x;                                                                                         \
				global_size[1] = threads.y;                                                                                         \
				global_size[2] = threads.z;                                                                                         \
			} else {                                                                                                                \
				global_size[0] = threads.y;                                                                                         \
				global_size[1] = threads.x;                                                                                         \
				global_size[2] = threads.z;                                                                                         \
			}                                                                                                                       \
			size_t local_size[3];                                                                                                   \
			local_size[0] = blocksize.x;                                                                                            \
			local_size[1] = blocksize.y;                                                                                            \
			local_size[2] = blocksize.z;                                                                                            \
			if (threads.dims >= 1) {                                                                                                \
				if ((global_size[0] % local_size[0]) != 0) {                                                                        \
					global_size[0] += (local_size[0] - (global_size[0] % local_size[0]));                                           \
				}                                                                                                                   \
			}                                                                                                                       \
			if (threads.dims >= 2) {                                                                                                \
				if ((global_size[1] % local_size[1]) != 0) {                                                                        \
					global_size[1] += (local_size[1] - (global_size[1] % local_size[1]));                                           \
				}                                                                                                                   \
			}                                                                                                                       \
			if (threads.dims == 3) {                                                                                                \
				if ((global_size[2] % local_size[2]) != 0) {                                                                        \
					global_size[2] += (local_size[2] - (global_size[2] % local_size[2]));                                           \
				}                                                                                                                   \
			}                                                                                                                       \
			clEnqueueNDRangeKernel(*(request.fpga.queue), ctrl_kernel_fpga_##type##_##subtype##_##name.p_kernel[request.fpga.type_id],   \
								   threads.dims, 0, global_size, local_size,                                                        \
								   0, NULL, request.fpga.p_last_kernel_event);    \
			OPENCL_ASSERT_OP(clFlush(*(request.fpga.queue)));                                                                       \
		} else if (kernel_model == MODEL_TASK) {                                                                                    \
			clEnqueueTask(*(request.fpga.queue), ctrl_kernel_fpga_##type##_##subtype##_##name.p_kernel[request.fpga.type_id],            \
						  0, NULL, request.fpga.p_last_kernel_event);             \
		}                                                                                                                           \
	}
#else
	#define CTRL_KERNEL_WRAP_FPGA(name, type, subtype, n_args, ...)
#endif

/**
 * Block of code that launches a \e FPGALIB kernel, this uses stuff defined on \e CTRL_KERNEL_FPGALIB.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param args_list List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see CTRL_KERNEL_FPGALIB
 */
#define CTRL_KERNEL_WRAP_FPGALIB(name, args_list, type, subtype, ...)                                                         \
	{                                                                                                                         \
		Ctrl_Kernel_FPGA_##type##_##subtype##_##name(CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(args_list, __VA_ARGS__));              \
		/* TODO @sergioalo: retain needed because rn this does not generate a new clevent, proper lib implementations will */ \
		clRetainEvent(*request.fpga.p_last_kernel_event);                                                                     \
	};

/**
 * Declares variables related to this kernel's info.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_FPGA(name, type, subtype, ...)             \
	extern Ctrl_FPGA_KernelParams ctrl_kernel_fpga_##type##_##subtype##_##name; \
	extern int                    kernel_model;                            \

/**
 * Kernel function prototype for \e FPGALIB type kernels to allow moving kernel definitions to another file.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_params Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_FPGALIB(name, type, subtype, n_params...)

///@endcond
#endif //_CTRL_FPGA_KERNELPROTO_H_
