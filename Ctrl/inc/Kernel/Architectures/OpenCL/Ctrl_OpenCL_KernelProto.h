#ifndef _CTRL_OPENCL_KERNELPROTO_H_
#define _CTRL_OPENCL_KERNELPROTO_H_
///@cond INTERNAL
/**
 * @file Ctrl_OpenCL_KernelProto.h
 * @brief Macros to generate the code and manage OpenCL GPU kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#define CTRL_KERNEL_OPENCL_MAX_CODE_SIZE 16384

#include <string.h>

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include "Core/Ctrl_Request.h"
#include "Kernel/Ctrl_KernelArgs.h"
#include "Kernel/Ctrl_Thread.h"

// TODO @sergioalo OUT tiles should not have sampler (cant be used for write image functions), IO tiles should not have texture nor sampler since we don't support rw textures. (if this changes remember changing the constructor parser)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_INVAL(type, name) " const ", CTRL_MACRO_STRINGIFY(type), " ", CTRL_MACRO_STRINGIFY(name)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_IN(type, name)    " const ", CTRL_MACRO_STRINGIFY(K##type##_wrapper), " ", CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name), " , __global const ", raw_ktile_K##type, " ", CTRL_MACRO_STRINGIFY(*ctrl_mem_wrapper_##name), " , __read_only image2d_t ", CTRL_MACRO_STRINGIFY(name##_img), ", const sampler_t ", CTRL_MACRO_STRINGIFY(name##_smp)
#define CTRL_KERNEL_OPENCL_PARSE_ARGS_OUT(type, name)   "  ", CTRL_MACRO_STRINGIFY(K##type##_wrapper), " ", CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name), " , __global ", raw_ktile_K##type, " ", CTRL_MACRO_STRINGIFY(*ctrl_mem_wrapper_##name), " , __write_only image2d_t ", CTRL_MACRO_STRINGIFY(name##_img), ", const sampler_t ", CTRL_MACRO_STRINGIFY(name##_smp)
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
	strcat(string, CTRL_MACRO_STRINGIFY(K##type##_##IOtype));                    \
	strcat(string, " ");                                                         \
	strcat(string, CTRL_MACRO_STRINGIFY(name));                                  \
	strcat(string, " = { .data = ");                                             \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_mem_wrapper_##name));               \
	strcat(string, " + ");                                                       \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".offset, ");                                                 \
	strcat(string, ".origAcumCard = { ");                                        \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".origAcumCard[0], ");                                        \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".origAcumCard[1], ");                                        \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".origAcumCard[2], ");                                        \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".origAcumCard[3] } , .card = { ");                           \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".card[0], ");                                                \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
	strcat(string, ".card[1], ");                                                \
	strcat(string, CTRL_MACRO_STRINGIFY(ctrl_ktile_wrapper_##name));             \
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

#define CTRL_KERNEL_OPENCL_PARSE_THREADS "                                                 \
		unsigned int thr_i = 0;                                                            \
		unsigned int thr_j = 0;                                                            \
		unsigned int thr_k = 0;                                                            \
		if (ctrl_threads.dims == 3) {                                                      \
			thr_k = get_global_id(0);                                                      \
			thr_j = get_global_id(1);                                                      \
			thr_i = get_global_id(2);                                                      \
		} else if (ctrl_threads.dims == 2) {                                               \
			thr_j = get_global_id(0);                                                      \
			thr_i = get_global_id(1);                                                      \
		} else {                                                                           \
			thr_i = get_global_id(0);                                                      \
		}                                                                                  \
		if (thr_i >= ctrl_threads.i || thr_j >= ctrl_threads.j || thr_k >= ctrl_threads.k) \
			return; "

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
#define CTRL_KERNEL_OPENCLGPU(name, type, subtype, ...)                                                                      \
	const char                 *p_ctrl_kernel_string_##type##_##subtype##_##name  = CTRL_KERNEL_EXTRACT_KERNEL(__VA_ARGS__); \
	Ctrl_OpenCLGpu_KernelParams ctrl_kernel_openclgpu_##type##_##subtype##_##name = (Ctrl_OpenCLGpu_KernelParams){           \
		.p_kernel      = NULL,                                                                                               \
		.p_program     = NULL,                                                                                               \
		.p_kernel_name = CTRL_MACRO_STRINGIFY(ctrl_kernel_openclgpu_##type##_##subtype##_##name),                            \
		.p_kernel_raw  = NULL,                                                                                               \
		.p_next        = NULL};

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
 * Defines the function containing the user provided code for a \e OPENCLGPULIB type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_OPENCLGPULIB
 */
#define CTRL_KERNEL_FUNCTION_OPENCLGPULIB(name, type, subtype, ...) CTRL_KERNEL_FUNCTION_OPENCLGPULIB_##subtype(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e OPENCLGPULIB type kernel
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @see Ctrl_ImplType, CTRL_KERNEL_FUNCTION, CTRL_KERNEL_WRAP_OPENCLGPULIB
 */
#define CTRL_KERNEL_OPENCLGPULIB(name, type, subtype, ...) CTRL_KERNEL_OPENCLGPULIB_##subtype(name, type, subtype, __VA_ARGS__)

/**
 * Defines the function containing the user provided code for a \e OPENCLGPULIB_DEFAULT type kernel
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
#define CTRL_KERNEL_FUNCTION_OPENCLGPULIB_DEFAULT(name, type, subtype, ...) \
	C_GUARD                                                                 \
	void Ctrl_Kernel_OpenCLGPU_##type##_##subtype##_##name(cl_command_queue queue, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__))

/**
 * Defines the function containing the user provided code for a \e OPENCLGPULIB_DEFAULT type kernel
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
#define CTRL_KERNEL_OPENCLGPULIB_DEFAULT(name, type, subtype, ...)                                                          \
	C_GUARD                                                                                                                 \
	void Ctrl_Kernel_OpenCLGPU_##type##_##subtype##_##name(cl_command_queue queue, CTRL_KERNEL_EXTRACT_ARGS(__VA_ARGS__)) { \
		CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(__VA_ARGS__)                                                                      \
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
#define CTRL_KERNEL_WRAP_OPENCLGPU(name, args_list, type, subtype, ...)                                                                                                                               \
	{                                                                                                                                                                                                 \
		int arg_pos = 0;                                                                                                                                                                              \
		for (int i = 0; i < request.opencl.n_arguments; i++) {                                                                                                                                        \
			if (request.opencl.p_roles[i] == KERNEL_INVAL) {                                                                                                                                          \
				OPENCL_ASSERT_OP(clSetKernelArg(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id], arg_pos,                                                          \
												request.opencl.p_displacements[i + 1] - request.opencl.p_displacements[i],                                                                            \
												((uint8_t *)args_list + request.opencl.p_displacements[i])));                                                                                         \
				arg_pos++;                                                                                                                                                                            \
			} else {                                                                                                                                                                                  \
				KHitTile *p_ktile = (KHitTile *)((uint8_t *)args_list + request.opencl.p_displacements[i]);                                                                                           \
                                                                                                                                                                                                      \
				KHitTile_opencl_wrapper ktile_opencl_wraper = {.origAcumCard = {p_ktile->origAcumCard[0],                                                                                             \
																				p_ktile->origAcumCard[1],                                                                                             \
																				p_ktile->origAcumCard[2],                                                                                             \
																				p_ktile->origAcumCard[3]},                                                                                            \
															   .card         = {p_ktile->card[0], p_ktile->card[1], p_ktile->card[2]},                                                                \
															   .offset       = p_ktile->offset};                                                                                                            \
                                                                                                                                                                                                      \
				OPENCL_ASSERT_OP(clSetKernelArg(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id], arg_pos, sizeof(KHitTile_opencl_wrapper), &ktile_opencl_wraper)); \
				arg_pos++;                                                                                                                                                                            \
				OPENCL_ASSERT_OP(clSetKernelArg(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id], arg_pos, sizeof(cl_mem), (cl_mem *)(p_ktile->data)));             \
				arg_pos++;                                                                                                                                                                            \
				OPENCL_ASSERT_OP(clSetKernelArg(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id], arg_pos, sizeof(cl_mem), &p_ktile->ext.ocl.tex));                 \
				arg_pos++;                                                                                                                                                                            \
				OPENCL_ASSERT_OP(clSetKernelArg(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id], arg_pos, sizeof(cl_sampler), &p_ktile->ext.ocl.smp));             \
				arg_pos++;                                                                                                                                                                            \
			}                                                                                                                                                                                         \
		}                                                                                                                                                                                             \
		OPENCL_ASSERT_OP(clSetKernelArg(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id], arg_pos, sizeof(Ctrl_Thread), &threads));                                 \
		arg_pos++;                                                                                                                                                                                    \
		if (threads.dims != blocksize.dims) {                                                                                                                                                         \
			fprintf(stderr, "[CTRL_KERNEL_WRAP_OPENCLGPU] WARNING: Thread space dims (%d) and blocksize dims (%d) don't match on launch of kernel %s\n", threads.dims, blocksize.dims, #name);        \
			fflush(stderr);                                                                                                                                                                           \
		}                                                                                                                                                                                             \
		size_t global_size[3] = {1, 1, 1};                                                                                                                                                            \
		size_t local_size[3]  = {1, 1, 1};                                                                                                                                                            \
		switch (threads.dims) {                                                                                                                                                                       \
			case 3:                                                                                                                                                                                   \
				global_size[0] = ((threads.k + blocksize.k - 1) / blocksize.k) * blocksize.k;                                                                                                         \
				global_size[1] = ((threads.j + blocksize.j - 1) / blocksize.j) * blocksize.j;                                                                                                         \
				global_size[2] = ((threads.i + blocksize.i - 1) / blocksize.i) * blocksize.i;                                                                                                         \
				local_size[0]  = blocksize.k;                                                                                                                                                         \
				local_size[1]  = blocksize.j;                                                                                                                                                         \
				local_size[2]  = blocksize.i;                                                                                                                                                         \
				break;                                                                                                                                                                                \
			case 2:                                                                                                                                                                                   \
				global_size[0] = ((threads.j + blocksize.j - 1) / blocksize.j) * blocksize.j;                                                                                                         \
				global_size[1] = ((threads.i + blocksize.i - 1) / blocksize.i) * blocksize.i;                                                                                                         \
				local_size[0]  = blocksize.j;                                                                                                                                                         \
				local_size[1]  = blocksize.i;                                                                                                                                                         \
				break;                                                                                                                                                                                \
			case 1:                                                                                                                                                                                   \
				global_size[0] = ((threads.i + blocksize.i - 1) / blocksize.i) * blocksize.i;                                                                                                         \
				local_size[0]  = blocksize.i;                                                                                                                                                         \
				break;                                                                                                                                                                                \
			default:                                                                                                                                                                                  \
				fprintf(stderr, "[CTRL_KERNEL_WRAP_OPENCLGPU] ERROR: Invalid number of dimensions for thread space on kernel %s: %d\n", #name, threads.dims);                                         \
				exit(EXIT_FAILURE);                                                                                                                                                                   \
		}                                                                                                                                                                                             \
		OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(*(request.opencl.queue), ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel[request.opencl.type_id],                                          \
												threads.dims, NULL, global_size, local_size,                                                                                                          \
												0, NULL, request.opencl.p_last_kernel_event));                                                                                                        \
		OPENCL_ASSERT_OP(clFlush(*(request.opencl.queue)));                                                                                                                                           \
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
 * Block of code that launches a \e OPENCLGPULIB kernel, this calls the macro for the appropiate subtype wrapper.
 * @hideinitializer
 *
 * @param name kernel name.
 * @param argsList list of arguments passed inside task when launching a kernel.
 * @param type Type of the kernel.
 * @param subtype Subtype of the kernel.
 * @param ... Parameters to the kernel.
 *
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL or \e CTRL_KERNEL_FUNCTION.
 * @see CTRL_KERNEL_OPENCLGPULIB
 */
#define CTRL_KERNEL_WRAP_OPENCLGPULIB(name, argsList, type, subtype, ...) CTRL_KERNEL_WRAP_OPENCLGPULIB_##subtype(name, argsList, type, subtype, __VA_ARGS__)

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
 * @pre A kernel of type \p type and subtype \p subtype must have been defined via \e CTRL_KERNEL or \e CTRL_KERNEL_FUNCTION.
 * @see CTRL_KERNEL_OPENCLGPULIB
 */
#define CTRL_KERNEL_WRAP_OPENCLGPULIB_DEFAULT(name, args_list, type, subtype, ...)                                                                                        \
	{                                                                                                                                                                     \
		Ctrl_Kernel_OpenCLGPU_##type##_##subtype##_##name(*(request.opencl.context), *(request.opencl.queue), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(args_list, __VA_ARGS__)); \
		OPENCL_ASSERT_OP(clEnqueueBarrierWithWaitList((*request.opencl.queue), 0, NULL, request.opencl.p_last_kernel_event);                                              \
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
#define CTRL_KERNEL_DECLARATION_OPENCLGPU(name, type, subtype, n_args, ...)                                                                                                                                                          \
	extern const char                 *p_ctrl_kernel_string_##type##_##subtype##_##name;                                                                                                                                             \
	extern Ctrl_OpenCLGpu_KernelParams ctrl_kernel_openclgpu_##type##_##subtype##_##name;                                                                                                                                            \
                                                                                                                                                                                                                                     \
	__attribute__((constructor)) static void Ctrl_InitKernel_##type##_##subtype##_##name() {                                                                                                                                         \
		/* kernel prototype, list of strings {"__kernel void ", <name>, "(", args[], */                                                                                                                                              \
		const char *pp_args_names[]                                       = {" __kernel void ", CTRL_MACRO_STRINGIFY(ctrl_kernel_openclgpu_##type##_##subtype##_##name), " ( ", CTRL_KERNEL_OPENCL_PARSE_ARGS(n_args, __VA_ARGS__)}; \
		ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw    = (char *)malloc(CTRL_KERNEL_OPENCL_MAX_CODE_SIZE * sizeof(char));                                                                                         \
		ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw[0] = '\0';                                                                                                                                                    \
		/* definitions for each type at the begining of the kernel. */                                                                                                                                                               \
		/* Copy new INVAL type definitions */                                                                                                                                                                                        \
		strcpy(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, CTRL_MACRO_STRINGIFY(CTRL_USER_TYPES));                                                                                                               \
                                                                                                                                                                                                                                     \
		CTRL_KERNEL_OPENCL_RESET_TILES(n_args, __VA_ARGS__);                                                                                                                                                                         \
		CTRL_KERNEL_OPENCL_PARSE_TILES(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, n_args, __VA_ARGS__);                                                                                                         \
		CTRL_KERNEL_OPENCL_RESET_TILES(n_args, __VA_ARGS__);                                                                                                                                                                         \
                                                                                                                                                                                                                                     \
		/* definition for Ctrl_thread */                                                                                                                                                                                             \
		strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, CTRL_THREAD_STRINGIFY);                                                                                                                               \
                                                                                                                                                                                                                                     \
		/* concatenate __kernel void <name> ( args*/                                                                                                                                                                                 \
		strcat(&ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw[0], &pp_args_names[0][0]);                                                                                                                            \
		strcat(&ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw[0], &pp_args_names[1][0]);                                                                                                                            \
		strcat(&ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw[0], &pp_args_names[2][0]);                                                                                                                            \
		char *p_roles;                                                                                                                                                                                                               \
		CTRL_KERNEL_ROLES(p_roles, n_args, __VA_ARGS__);                                                                                                                                                                             \
		int index = 3;                                                                                                                                                                                                               \
		for (int i = 0; i < n_args; i++) {                                                                                                                                                                                           \
			if (i != 0) {                                                                                                                                                                                                            \
				strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, " , ");                                                                                                                                       \
			}                                                                                                                                                                                                                        \
			if (p_roles[i] == KERNEL_INVAL) {                                                                                                                                                                                        \
				for (int j = 0; j < 4; j++) {                                                                                                                                                                                        \
					strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, pp_args_names[index++]);                                                                                                                  \
				}                                                                                                                                                                                                                    \
			} else {                                                                                                                                                                                                                 \
				for (int j = 0; j < 12; j++) {                                                                                                                                                                                       \
					strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, pp_args_names[index++]);                                                                                                                  \
				}                                                                                                                                                                                                                    \
			}                                                                                                                                                                                                                        \
		}                                                                                                                                                                                                                            \
		/* locate first "{" on used defined kernel, this is needed to insert stuff at the begining of the kernel */                                                                                                                  \
		int init_kernel = 0;                                                                                                                                                                                                         \
		while (true) {                                                                                                                                                                                                               \
			if (p_ctrl_kernel_string_##type##_##subtype##_##name[init_kernel] == '{') {                                                                                                                                              \
				init_kernel++;                                                                                                                                                                                                       \
				break;                                                                                                                                                                                                               \
			}                                                                                                                                                                                                                        \
			init_kernel++;                                                                                                                                                                                                           \
		}                                                                                                                                                                                                                            \
		/* last parameter to kernel signature */                                                                                                                                                                                     \
		strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, " , const Ctrl_Thread ctrl_threads ) { ");                                                                                                            \
		/* add stuff before user provided code */                                                                                                                                                                                    \
		CTRL_KERNEL_OPENCL_MOUNT_INNER_TILES(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, n_args, __VA_ARGS__);                                                                                                   \
		/* define thread_id */                                                                                                                                                                                                       \
		strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, CTRL_KERNEL_OPENCL_PARSE_THREADS);                                                                                                                    \
		/* add user kernel */                                                                                                                                                                                                        \
		strcat(ctrl_kernel_openclgpu_##type##_##subtype##_##name.p_kernel_raw, &p_ctrl_kernel_string_##type##_##subtype##_##name[init_kernel]);                                                                                      \
                                                                                                                                                                                                                                     \
		Ctrl_OpenCLGpu_KernelParams *curr_k_par = &OpenCLGpu_initial_kp;                                                                                                                                                             \
		while (curr_k_par->p_next != NULL)                                                                                                                                                                                           \
			curr_k_par = curr_k_par->p_next;                                                                                                                                                                                         \
                                                                                                                                                                                                                                     \
		curr_k_par->p_next = &ctrl_kernel_openclgpu_##type##_##subtype##_##name;                                                                                                                                                     \
	}

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
#define CTRL_KERNEL_DECLARATION_OPENCLGPULIB(name, type, subtype, n_params, ...) CTRL_KERNEL_DECLARATION_OPENCLGPULIB_##subtype(name, type, subtype, n_params, __VA_ARGS__)

/**
 * Kernel declaration for host code or header files for \e OPENCLGPULIB_DEFAULT type kernels.
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
#define CTRL_KERNEL_DECLARATION_OPENCLGPULIB_DEFAULT(name, type, subtype, n_params, ...) \
	void CTRL_KERNEL_OPENCLGPU_##type##_##subtype##_##name(cl_command_queue queue, CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_##n_params(__VA_ARGS__));

///@endcond
#endif //_CTRL_OPENCL_KERNELPROTO_H_
