#ifndef _CTRL_OPENCL_KERNELPROTO_H_
#define _CTRL_OPENCL_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_OpenCL_KernelProto.h
 * @brief Macros to generate the code and manage OpenCL GPU kernels.
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

#include <string.h>

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include "Kernel/Ctrl_KernelArgs.h"
#include "Kernel/Ctrl_Thread.h"

#include "Core/Ctrl_Request.h"

#include "Architectures/OpenCL/Ctrl_OpenCL_Gpu.h"
#include "Architectures/OpenCL/Ctrl_OpenCL_Request.h"

#define CTRL_KERNEL_OPENCL_PARSE_ARGS_INVAL(type, name) " const ", CTRL_KERNEL_STRINGIFY(type), " ", CTRL_KERNEL_STRINGIFY(name)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_IN(type, name)    " const ", CTRL_KERNEL_STRINGIFY(K##type##_wrapper), " ", CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name), " , __global const ", raw_ktile_K##type, " ", CTRL_KERNEL_STRINGIFY(*ctrl_mem_wrapper_##name)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_OUT(type, name)   "  ", CTRL_KERNEL_STRINGIFY(K##type##_wrapper), " ", CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name), " , __global ", raw_ktile_K##type, " ", CTRL_KERNEL_STRINGIFY(*ctrl_mem_wrapper_##name)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_IO(type, name)    CTRL_KERNEL_OPENCL_PARSE_ARGS_OUT(type, name)

#define CTRL_KERNEL_OPENCL_PARSE_ARGS_1(role, type, name)       CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_2(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_1(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_3(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_2(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_4(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_3(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_5(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_4(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_6(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_5(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_7(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_6(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_8(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_7(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_9(role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_8(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_10(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_9(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_11(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_10(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_12(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_11(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_13(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_12(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_14(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_13(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_15(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_14(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_16(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_15(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_17(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_16(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_18(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_17(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_19(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_18(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_20(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_19(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_21(role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##role(type, name), CTRL_KERNEL_OPENCL_PARSE_ARGS_20(__VA_ARGS__)

#define CTRL_KERNEL_OPENCL_PARSE_ARGS(n, ...) CTRL_KERNEL_OPENCL_PARSE_ARGS_##n(__VA_ARGS__)

#define CTRL_KERNEL_OPENCL_RESET_TILES_INVAL(type, name)
#define CTRL_KERNEL_OPENCL_RESET_TILES_IN(type, name)  raw_added_ktile_K##type = false;
#define CTRL_KERNEL_OPENCL_RESET_TILES_OUT(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_IN(type, name)
#define CTRL_KERNEL_OPENCL_RESET_TILES_IO(type, name)  CTRL_KERNEL_OPENCL_RESET_TILES_OUT(type, name)

#define CTRL_KERNEL_OPENCL_RESET_TILES_1(role, type, name)       CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name)
#define CTRL_KERNEL_OPENCL_RESET_TILES_2(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_1(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_3(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_2(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_4(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_3(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_5(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_4(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_6(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_5(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_7(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_6(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_8(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_7(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_9(role, type, name, ...)  CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_8(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_10(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_9(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_11(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_10(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_12(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_11(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_13(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_12(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_14(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_13(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_15(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_14(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_16(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_15(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_17(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_16(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_18(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_17(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_19(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_18(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_20(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_19(__VA_ARGS__)
#define CTRL_KERNEL_OPENCL_RESET_TILES_21(role, type, name, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##role(type, name) CTRL_KERNEL_OPENCL_RESET_TILES_20(__VA_ARGS__)

#define CTRL_KERNEL_OPENCL_RESET_TILES(n, ...) CTRL_KERNEL_OPENCL_RESET_TILES_##n(__VA_ARGS__)

#define CTRL_KERNEL_OPENCL_PARSE_TILES_INVAL(string, type, name)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_IN(string, type, name) \
	if (!raw_added_ktile_K##type) {                           \
		strcat(string, " typedef unsigned char BYTE; ");      \
		strcat(string, raw_def_ktile_K##type);                \
		raw_added_ktile_K##type = true;                       \
	}
#define CTRL_KERNEL_OPENCL_PARSE_TILES_OUT(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_IN(string, type, name)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_IO(string, type, name)  CTRL_KERNEL_OPENCL_PARSE_TILES_OUT(string, type, name)

#define CTRL_KERNEL_OPENCL_PARSE_TILES_1(string, role, type, name)       CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_2(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_1(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_3(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_2(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_4(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_3(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_5(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_4(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_6(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_5(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_7(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_6(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_8(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_7(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_9(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_8(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_10(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_9(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_11(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_10(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_12(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_11(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_13(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_12(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_14(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_13(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_15(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_14(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_16(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_15(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_17(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_16(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_18(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_17(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_19(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_18(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_20(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_19(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_PARSE_TILES_21(string, role, type, name, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_PARSE_TILES_20(string, __VA_ARGS__)

#define CTRL_KERNEL_OPENCL_PARSE_TILES(string, n, ...) CTRL_KERNEL_OPENCL_PARSE_TILES_##n(string, __VA_ARGS__)

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_INVAL(string, type, name)

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_IN(string, type, name) \
	CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_GENERIC(string, type, name, read)

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_OUT(string, type, name) \
	CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_GENERIC(string, type, name, write)

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_GENERIC(string, type, name, IOtype) \
	strcat(string, CTRL_KERNEL_STRINGIFY(K##type##_##IOtype));                   \
	strcat(string, " ");                                                         \
	strcat(string, CTRL_KERNEL_STRINGIFY(name));                                 \
	strcat(string, " = { .data = ");                                             \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_mem_wrapper_##name));              \
	strcat(string, " + ");                                                       \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".offset, ");                                                 \
	strcat(string, ".origAcumCard = { ");                                        \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".origAcumCard[0], ");                                        \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".origAcumCard[1], ");                                        \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".origAcumCard[2], ");                                        \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".origAcumCard[3] } , .card = { ");                           \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".card[0], ");                                                \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".card[1], ");                                                \
	strcat(string, CTRL_KERNEL_STRINGIFY(ctrl_ktile_wrapper_##name));            \
	strcat(string, ".card[2] }}; ");

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_IO(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_OUT(string, type, name)

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_1(string, role, type, name)       CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_2(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_1(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_3(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_2(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_4(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_3(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_5(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_4(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_6(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_5(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_7(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_6(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_8(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_7(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_9(string, role, type, name, ...)  CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_8(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_10(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_9(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_11(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_10(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_12(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_11(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_13(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_12(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_14(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_13(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_15(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_14(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_16(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_15(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_17(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_16(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_18(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_17(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_19(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_18(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_20(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_19(string, __VA_ARGS__)
#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_21(string, role, type, name, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##role(string, type, name) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_20(string, __VA_ARGS__)

#define CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES(string, n, ...) CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES_##n(string, __VA_ARGS__)

#define CTRL_KERNEL_OPENCL_PARSE_THREADS " \
		int thread_id_x __attribute__((unused)) = get_global_id(0); \
		int thread_id_y __attribute__((unused)) = get_global_id(1); \
		int thread_id_z __attribute__((unused)) = get_global_id(2); \
		if ( get_work_dim() > 1 ) { \
			thread_id_x = get_global_id(1); \
			thread_id_y = get_global_id(0); \
		} \
		if ( thread_id_x >= ctrl_thread.x || thread_id_y >= ctrl_thread.y || thread_id_z >= ctrl_thread.z ) { \
			return ; \
		} "

/**
 * Defines strings and other variables needed for a \e OPENCLGPU type kernel launch from the information passed by the user
 * on the kernel definition.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_OPENCLGPU
 */
#define CTRL_KERNEL_OPENCLGPU(name, type, subtype, ...)                                                                            \
	cl_kernel  *p_kernel_##type##_##subtype##_##name             = NULL;                                                           \
	cl_program *p_program_##type##_##subtype##_##name            = NULL;                                                           \
	char       *p_kernel_raw_##type##_##subtype##_##name         = NULL;                                                           \
	const char *p_ctrl_kernel_name_##type##_##subtype##_##name   = CTRL_KERNEL_STRINGIFY(CTRL_KERNEL_##type##_##subtype##_##name); \
	const char *p_ctrl_kernel_string_##type##_##subtype##_##name = CTRL_KERNEL_EXTRACT_KERNEL(__VA_ARGS__);

/**
 * Defines strings and other variables needed for a \e GENERIC type kernel launch from the information passed by the user
 * on the kernel definition.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_OPENCLGPU_GENERIC
 */
#define CTRL_KERNEL_OPENCLGPU_GENERIC(name, type, subtype, ...) \
	CTRL_KERNEL_OPENCLGPU(name, type, subtype, __VA_ARGS__)

/**
 * Defines stuff needed for a \e OPENCLGPULIB type kernel launch from the information passed by the user on the kernel definition.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL, CTRL_KERNEL_WRAP_OPENCLGPU_LIB
 * @todo opencl lib kernels are not fully implemented
 */
#define CTRL_KERNEL_OPENCLGPULIB(name, type, subtype, ...)                                          \
	C_GUARD                                                                                         \
	void Ctrl_Kernel_OpenCLGPU_##type##_##subtype##_##name(CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                              \
	}

/**
 * Block of code that launches a \e OPENCLGPU kernel, this uses stuff defined on \e CTRL_KERNEL_OPENCLGPU.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param args_list List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see CTRL_KERNEL_OPENCLGPU
 */
#define CTRL_KERNEL_WRAP_OPENCLGPU(name, args_list, type, subtype, ...)                                                                              \
	{                                                                                                                                                \
		cl_int err     = 0;                                                                                                                          \
		int    arg_pos = 0;                                                                                                                          \
		for (int i = 0; i < request.opencl.n_arguments; i++) {                                                                                       \
			if (request.opencl.p_roles[i] == KERNEL_INVAL) {                                                                                         \
				err |= clSetKernelArg(*p_kernel_##type##_##subtype##_##name, arg_pos,                                                                \
									  request.opencl.p_displacements[i + 1] - request.opencl.p_displacements[i],                                     \
									  ((uint8_t *)args_list + request.opencl.p_displacements[i]));                                                   \
				arg_pos++;                                                                                                                           \
			} else {                                                                                                                                 \
				KHitTile *p_ktile = (KHitTile *)((uint8_t *)args_list + request.opencl.p_displacements[i]);                                          \
                                                                                                                                                     \
				KHitTile_opencl_wrapper ktile_opencl_wraper = {.origAcumCard = {p_ktile->origAcumCard[0],                                            \
																				p_ktile->origAcumCard[1],                                            \
																				p_ktile->origAcumCard[2],                                            \
																				p_ktile->origAcumCard[3]},                                           \
															   .card         = {p_ktile->card[0], p_ktile->card[1], p_ktile->card[2]},               \
															   .offset       = p_ktile->offset};                                                           \
                                                                                                                                                     \
				err |= clSetKernelArg(*p_kernel_##type##_##subtype##_##name, arg_pos, sizeof(KHitTile_opencl_wrapper), &ktile_opencl_wraper);        \
				arg_pos++;                                                                                                                           \
				err |= clSetKernelArg(*p_kernel_##type##_##subtype##_##name, arg_pos, sizeof(cl_mem), (cl_mem *)(p_ktile->data));                    \
				arg_pos++;                                                                                                                           \
			}                                                                                                                                        \
		}                                                                                                                                            \
		err |= clSetKernelArg(*p_kernel_##type##_##subtype##_##name, arg_pos, sizeof(Ctrl_Thread), &threads);                                        \
		arg_pos++;                                                                                                                                   \
		size_t global_size[3];                                                                                                                       \
		if (threads.dims == 1) {                                                                                                                     \
			global_size[0] = threads.x;                                                                                                              \
			global_size[1] = threads.y;                                                                                                              \
			global_size[2] = threads.z;                                                                                                              \
		} else {                                                                                                                                     \
			global_size[0] = threads.y;                                                                                                              \
			global_size[1] = threads.x;                                                                                                              \
			global_size[2] = threads.z;                                                                                                              \
		}                                                                                                                                            \
		size_t local_size[3];                                                                                                                        \
		local_size[0] = blocksize.x;                                                                                                                 \
		local_size[1] = blocksize.y;                                                                                                                 \
		local_size[2] = blocksize.z;                                                                                                                 \
		if (threads.dims >= 1) {                                                                                                                     \
			if ((global_size[0] % local_size[0]) != 0) {                                                                                             \
				global_size[0] += (local_size[0] - (global_size[0] % local_size[0]));                                                                \
			}                                                                                                                                        \
		}                                                                                                                                            \
		if (threads.dims >= 2) {                                                                                                                     \
			if ((global_size[1] % local_size[1]) != 0) {                                                                                             \
				global_size[1] += (local_size[1] - (global_size[1] % local_size[1]));                                                                \
			}                                                                                                                                        \
		}                                                                                                                                            \
		if (threads.dims == 3) {                                                                                                                     \
			if ((global_size[2] % local_size[2]) != 0) {                                                                                             \
				global_size[2] += (local_size[2] - (global_size[2] % local_size[2]));                                                                \
			}                                                                                                                                        \
		}                                                                                                                                            \
		OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(*(request.opencl.queue), *p_kernel_##type##_##subtype##_##name,                                      \
												threads.dims, 0, global_size, local_size,                                                            \
												request.opencl.n_event_wait, request.opencl.p_event_wait_list, request.opencl.p_last_kernel_event)); \
		OPENCL_ASSERT_OP(clFlush(*(request.opencl.queue)));                                                                                          \
	}

/**
 * Block of code that launches a \e GENERIC kernel, this uses stuff defined on \e CTRL_KERNEL_OPENCLGPU_GENERIC.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param args_list List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see CTRL_KERNEL_OPENCLGPU_GENERIC
 */
#define CTRL_KERNEL_WRAP_OPENCLGPU_GENERIC(name, args_list, type, subtype, ...) \
	CTRL_KERNEL_WRAP_OPENCLGPU(name, args_list, type, subtype, __VA_ARGS__)

/**
 * Block of code that launches a \e OPENCLGPU_LIB kernel, this uses stuff defined on \e CTRL_KERNEL_OPENCLGPU_LIB.
 * @hideinitializer
 *
 * @param name Kernel name.
 * @param args_list List of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel and kernel body.
 *
 * @see CTRL_KERNEL_OPENCLGPU_LIB
 */
#define CTRL_KERNEL_WRAP_OPENCLGPULIB(name, args_list, type, subtype, ...)                                                    \
	{                                                                                                                         \
		Ctrl_Kernel_OpenCLGPU_##type##_##subtype##_##name(CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(args_list, __VA_ARGS__));         \
		/* TODO @sergioalo: retain needed because rn this does not generate a new clevent, proper lib implementations will */ \
		clRetainEvent(*request.opencl.p_last_kernel_event);                                                                   \
	};

/**
 * Define constructor function to preload an OpenCL kernel.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_args Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 */
#define CTRL_KERNEL_DECLARATION_OPENCLGPU(name, type, subtype, n_args, ...)                                                                                                                           \
	extern cl_kernel  *p_kernel_##type##_##subtype##_##name;                                                                                                                                          \
	extern cl_program *p_program_##type##_##subtype##_##name;                                                                                                                                         \
	extern char       *p_kernel_raw_##type##_##subtype##_##name;                                                                                                                                      \
	extern const char *p_ctrl_kernel_name_##type##_##subtype##_##name;                                                                                                                                \
	extern const char *p_ctrl_kernel_string_##type##_##subtype##_##name;                                                                                                                              \
                                                                                                                                                                                                      \
	__attribute__((constructor)) static void Ctrl_InitKernel_##type##_##subtype##_##name() {                                                                                                          \
		p_kernel_##type##_##subtype##_##name  = (cl_kernel *)malloc(sizeof(cl_kernel));                                                                                                               \
		p_program_##type##_##subtype##_##name = (cl_program *)malloc(sizeof(cl_program));                                                                                                             \
                                                                                                                                                                                                      \
		/* kernel prototype, list of strings {"__kernel void ", <name>, "(", args[], */                                                                                                               \
		const char *pp_args_names[]                 = {" __kernel void ", CTRL_KERNEL_STRINGIFY(CTRL_KERNEL_##type##_##subtype##_##name), " ( ", CTRL_KERNEL_OPENCL_PARSE_ARGS(n_args, __VA_ARGS__)}; \
		p_kernel_raw_##type##_##subtype##_##name    = (char *)malloc(10000 * sizeof(char));                                                                                                           \
		p_kernel_raw_##type##_##subtype##_##name[0] = '\0';                                                                                                                                           \
		/* definitions for each type at the begining of the kernel. */                                                                                                                                \
		CTRL_KERNEL_OPENCL_RESET_TILES(n_args, __VA_ARGS__);                                                                                                                                          \
		CTRL_KERNEL_OPENCL_PARSE_TILES(p_kernel_raw_##type##_##subtype##_##name, n_args, __VA_ARGS__);                                                                                                \
		CTRL_KERNEL_OPENCL_RESET_TILES(n_args, __VA_ARGS__);                                                                                                                                          \
                                                                                                                                                                                                      \
		/* definition for Ctrl_thread */                                                                                                                                                              \
		strcat(p_kernel_raw_##type##_##subtype##_##name, CTRL_THREAD_STRINGIFY);                                                                                                                      \
                                                                                                                                                                                                      \
		/* concatenate __kernel void <name> ( args*/                                                                                                                                                  \
		strcat(&p_kernel_raw_##type##_##subtype##_##name[0], &pp_args_names[0][0]);                                                                                                                   \
		strcat(&p_kernel_raw_##type##_##subtype##_##name[0], &pp_args_names[1][0]);                                                                                                                   \
		strcat(&p_kernel_raw_##type##_##subtype##_##name[0], &pp_args_names[2][0]);                                                                                                                   \
		char *p_roles;                                                                                                                                                                                \
		CTRL_KERNEL_ROLES(p_roles, n_args, __VA_ARGS__);                                                                                                                                              \
		int index = 3;                                                                                                                                                                                \
		for (int i = 0; i < n_args; i++) {                                                                                                                                                            \
			if (i != 0) {                                                                                                                                                                             \
				strcat(p_kernel_raw_##type##_##subtype##_##name, " , ");                                                                                                                              \
			}                                                                                                                                                                                         \
			if (p_roles[i] == KERNEL_INVAL) {                                                                                                                                                         \
				for (int j = 0; j < 4; j++) {                                                                                                                                                         \
					strcat(p_kernel_raw_##type##_##subtype##_##name, pp_args_names[index++]);                                                                                                         \
				}                                                                                                                                                                                     \
			} else {                                                                                                                                                                                  \
				for (int j = 0; j < 8; j++) {                                                                                                                                                         \
					strcat(p_kernel_raw_##type##_##subtype##_##name, pp_args_names[index++]);                                                                                                         \
				}                                                                                                                                                                                     \
			}                                                                                                                                                                                         \
		}                                                                                                                                                                                             \
		/* locate first "{" on used defined kernel, this is needed to insert stuff at the begining of the kernel */                                                                                   \
		int init_kernel = 0;                                                                                                                                                                          \
		while (true) {                                                                                                                                                                                \
			if (p_ctrl_kernel_string_##type##_##subtype##_##name[init_kernel] == '{') {                                                                                                               \
				init_kernel++;                                                                                                                                                                        \
				break;                                                                                                                                                                                \
			}                                                                                                                                                                                         \
			init_kernel++;                                                                                                                                                                            \
		}                                                                                                                                                                                             \
		/* last parameter to kernel signature */                                                                                                                                                      \
		strcat(p_kernel_raw_##type##_##subtype##_##name, " , const Ctrl_Thread ctrl_thread ) { ");                                                                                                    \
		/* add stuff before user provided code */                                                                                                                                                     \
		CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES(p_kernel_raw_##type##_##subtype##_##name, n_args, __VA_ARGS__);                                                                                          \
		/* define thread_id */                                                                                                                                                                        \
		strcat(p_kernel_raw_##type##_##subtype##_##name, CTRL_KERNEL_OPENCL_PARSE_THREADS);                                                                                                           \
		/* add user kernel */                                                                                                                                                                         \
		strcat(p_kernel_raw_##type##_##subtype##_##name, &p_ctrl_kernel_string_##type##_##subtype##_##name[init_kernel]);                                                                             \
                                                                                                                                                                                                      \
		Ctrl_OpenCLGpu_KernelParams *curr_k_par = &OpenClGpu_initial_kp;                                                                                                                              \
		while (curr_k_par->p_next != NULL)                                                                                                                                                            \
			curr_k_par = curr_k_par->p_next;                                                                                                                                                          \
                                                                                                                                                                                                      \
		curr_k_par->p_kernel_raw  = p_kernel_raw_##type##_##subtype##_##name;                                                                                                                         \
		curr_k_par->p_kernel      = p_kernel_##type##_##subtype##_##name;                                                                                                                             \
		curr_k_par->p_program     = p_program_##type##_##subtype##_##name;                                                                                                                            \
		curr_k_par->p_kernel_name = p_ctrl_kernel_name_##type##_##subtype##_##name;                                                                                                                   \
                                                                                                                                                                                                      \
		static Ctrl_OpenCLGpu_KernelParams new_kernel_params = CTRL_OPENCLGPU_KERNELPARAMS_NULL;                                                                                                      \
		curr_k_par->p_next                                   = &new_kernel_params;                                                                                                                    \
	}

/**
 * Kernel function prototype for \e OPENCLGPULIB type kernels to allow moving kernel definitions to another file.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of this implementation.
 * @param subtype Subtype of this implementation.
 * @param n_args Number of arguments recieved by the kernel.
 * @param ... Arguments recieved by the kernel (with roles).
 *
 * @todo lib kernels not implemented in OpenCL
 */
#define CTRL_KERNEL_DECLARATION_OPENCLGPULIB(name, type, subtype, n_args, ...)

///@endcond
#endif //_CTRL_OPENCL_KERNELPROTO_H_
