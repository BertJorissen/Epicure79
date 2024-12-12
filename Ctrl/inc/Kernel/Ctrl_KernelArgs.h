#ifndef _CTRL_KERNEL_ARGS_H_
#define _CTRL_KERNEL_ARGS_H_
/**
 * @file Ctrl_KernelArgs.h
 * @brief Macros to process lists of arguments in kernels definitions and launches
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

// @sergioalo formatter breaks recursive macros into multiple lines and makes them very long
// clang-format off

#define CTRL_KERNEL_PATH_LENGTH 512

#ifdef _CTRL_ARCH_CPU_
	#include "Kernel/Architectures/Cpu/Ctrl_Cpu_KernelArgs.h"
	#include "Kernel/Architectures/Cpu/Ctrl_Cpu_KernelChar.h"
#else
	#define CTRL_KERNEL_CPU_KTILE_DEVICE_DATA(name)
	#define CTRL_KERNEL_CPU_KERNEL_CHAR(...)
#endif // _CTRL_ARCH_CPU_

#ifdef _CTRL_ARCH_CUDA_
	#include "Kernel/Architectures/Cuda/Ctrl_Cuda_KernelArgs.h"
	#include "Kernel/Architectures/Cuda/Ctrl_Cuda_KernelChar.h"
#else
	#define CTRL_KERNEL_CUDA_KTILE_DEVICE_DATA(...)
	#define CTRL_KERNEL_CUDA_KERNEL_CHAR(...)
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_HIP_
	#include "Kernel/Architectures/Hip/Ctrl_Hip_KernelArgs.h"
	#include "Kernel/Architectures/Hip/Ctrl_Hip_KernelChar.h"
#else
	#define CTRL_KERNEL_HIP_KTILE_DEVICE_DATA(...)
	#define CTRL_KERNEL_HIP_KERNEL_CHAR(...)
#endif // _CTRL_ARCH_HIP_

#ifdef _CTRL_ARCH_OPENCL_GPU_
	#include "Kernel/Architectures/OpenCL/Ctrl_OpenCL_KernelArgs.h"
	#include "Kernel/Architectures/OpenCL/Ctrl_OpenCL_Gpu_KernelChar.h"
#else
	#define CTRL_KERNEL_OPENCL_KTILE_DEVICE_DATA(...)
	#define CTRL_KERNEL_OPENCL_GPU_KERNEL_CHAR(...)
#endif // _CTRL_ARCH_OPENCL_GPU_

#ifdef _CTRL_ARCH_FPGA_
	#ifdef CTRL_HOST_COMPILE
		#include "Kernel/Architectures/FPGA/Ctrl_FPGA_KernelArgs.h"
		#include "Kernel/Architectures/FPGA/Ctrl_FPGA_KernelChar.h"
	#endif // CTRL_HOST_COMPILE
#else
	#define CTRL_KERNEL_FPGA_KTILE_DEVICE_DATA(...)
	#define CTRL_KERNEL_FPGA_KERNEL_CHAR(...)
#endif // _CTRL_ARCH_FPGA_

#define MANUAL 1
#define AUTO 2

/**
 * @brief Set kernel characterization.
 * @hideinitializer
 *
 * @param name name of the kernel this characterization is applied to.
 * @param type type of characterization, MANUAL or AUTO.
 * @param ... sizes of blocks, 1 value per dimension.
 */
#define CTRL_KERNEL_CHAR(name, type, ...) \
	CTRL_KERNEL_CHARN(name, type, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)

#define CTRL_KERNEL_CHARN(name, type, dims, ...) CTRL_KERNEL_CHARN2(name, type, dims, __VA_ARGS__)

#define CTRL_KERNEL_CHARN2(name, type, dims, ...)                      \
	CTRL_KERNEL_CPU_KERNEL_CHAR(name, type, dims, __VA_ARGS__);        \
	CTRL_KERNEL_CUDA_KERNEL_CHAR(name, type, dims, __VA_ARGS__);       \
	CTRL_KERNEL_HIP_KERNEL_CHAR(name, type, dims, __VA_ARGS__);        \
	CTRL_KERNEL_OPENCL_GPU_KERNEL_CHAR(name, type, dims, __VA_ARGS__); \
	CTRL_KERNEL_FPGA_KERNEL_CHAR(name, type, dims, __VA_ARGS__);

/*
 *******************************************************************************************
 *************************************  Basics  ********************************************
 *******************************************************************************************
 */

#ifdef __cplusplus
#define C_GUARD extern "C"
#else // __cplusplus
#define C_GUARD
#endif // __cplusplus

#define CTRL_MACRO_STRINGIFY(a)  CTRL_MACRO_STRINGIFY2(a)
#define CTRL_MACRO_STRINGIFY2(a) #a

#define CTRL_COUNTPARAM(...) CTRL_COUNTPARAM_N(__VA_ARGS__, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define CTRL_COUNTPARAM_N(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, num, ...) num

/* Copy parameter types and names */
#define CTRL_KERNEL_TYPED(list, numArgs, ...) CTRL_KERNEL_TYPED_##numArgs(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_1(role, type, name) type name
#define CTRL_KERNEL_TYPED_2(role, type, name, ...) type name, CTRL_KERNEL_TYPED_1(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_3(role, type, name, ...) type name, CTRL_KERNEL_TYPED_2(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_4(role, type, name, ...) type name, CTRL_KERNEL_TYPED_3(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_5(role, type, name, ...) type name, CTRL_KERNEL_TYPED_4(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_6(role, type, name, ...) type name, CTRL_KERNEL_TYPED_5(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_7(role, type, name, ...) type name, CTRL_KERNEL_TYPED_6(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_8(role, type, name, ...) type name, CTRL_KERNEL_TYPED_7(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_9(role, type, name, ...) type name, CTRL_KERNEL_TYPED_8(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_10(role, type, name, ...) type name, CTRL_KERNEL_TYPED_9(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_11(role, type, name, ...) type name, CTRL_KERNEL_TYPED_10(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_12(role, type, name, ...) type name, CTRL_KERNEL_TYPED_11(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_13(role, type, name, ...) type name, CTRL_KERNEL_TYPED_12(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_14(role, type, name, ...) type name, CTRL_KERNEL_TYPED_13(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_15(role, type, name, ...) type name, CTRL_KERNEL_TYPED_14(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_16(role, type, name, ...) type name, CTRL_KERNEL_TYPED_15(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_17(role, type, name, ...) type name, CTRL_KERNEL_TYPED_16(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_18(role, type, name, ...) type name, CTRL_KERNEL_TYPED_17(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_19(role, type, name, ...) type name, CTRL_KERNEL_TYPED_18(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_20(role, type, name, ...) type name, CTRL_KERNEL_TYPED_19(__VA_ARGS__)

/* Copy pointer parameter types and names */
#define CTRL_KERNEL_TYPED_POINTER(list, numArgs, ...) CTRL_KERNEL_TYPED_POINTER_##numArgs(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_1(role, type, name) type *name
#define CTRL_KERNEL_TYPED_POINTER_2(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_1(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_3(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_2(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_4(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_3(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_5(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_4(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_6(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_5(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_7(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_6(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_8(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_7(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_9(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_8(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_10(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_9(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_11(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_10(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_12(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_11(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_13(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_12(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_14(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_13(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_15(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_14(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_16(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_15(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_17(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_16(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_18(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_17(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_19(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_18(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_POINTER_20(role, type, name, ...) type *name, CTRL_KERNEL_TYPED_POINTER_19(__VA_ARGS__)

/*
 * Change list of arguments to list of pointers
 */
#define CTRL_KERNEL_ARGS_POINTERS_1(_1) &(_1)
#define CTRL_KERNEL_ARGS_POINTERS_2(_1, _2) &(_1), &(_2)
#define CTRL_KERNEL_ARGS_POINTERS_3(_1, _2, _3) &(_1), &(_2), &(_3)
#define CTRL_KERNEL_ARGS_POINTERS_4(_1, _2, _3, _4) &(_1), &(_2), &(_3), &(_4)
#define CTRL_KERNEL_ARGS_POINTERS_5(_1, _2, _3, _4, _5) &(_1), &(_2), &(_3), &(_4), &(_5)
#define CTRL_KERNEL_ARGS_POINTERS_6(_1, _2, _3, _4, _5, _6) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6)
#define CTRL_KERNEL_ARGS_POINTERS_7(_1, _2, _3, _4, _5, _6, _7) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7)
#define CTRL_KERNEL_ARGS_POINTERS_8(_1, _2, _3, _4, _5, _6, _7, _8) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8)
#define CTRL_KERNEL_ARGS_POINTERS_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9)
#define CTRL_KERNEL_ARGS_POINTERS_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10)
#define CTRL_KERNEL_ARGS_POINTERS_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11)
#define CTRL_KERNEL_ARGS_POINTERS_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12)
#define CTRL_KERNEL_ARGS_POINTERS_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13)
#define CTRL_KERNEL_ARGS_POINTERS_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14)
#define CTRL_KERNEL_ARGS_POINTERS_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14), &(_15)
#define CTRL_KERNEL_ARGS_POINTERS_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14), &(_15), &(_16)
#define CTRL_KERNEL_ARGS_POINTERS_17(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14), &(_15), &(_16), &(_17)
#define CTRL_KERNEL_ARGS_POINTERS_18(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14), &(_15), &(_16), &(_17), &(_18)
#define CTRL_KERNEL_ARGS_POINTERS_19(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14), &(_15), &(_16), &(_17), &(_18), &(_19)
#define CTRL_KERNEL_ARGS_POINTERS_20(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) &(_1), &(_2), &(_3), &(_4), &(_5), &(_6), &(_7), &(_8), &(_9), &(_10), &(_11), &(_12), &(_13), &(_14), &(_15), &(_16), &(_17), &(_18), &(_19), &(_20)

/*
 * Override CAL_ARGS_TO_POINTERS macro by number of variadic arguments
 */
#define CTRL_KERNEL_ARGS_POINTERS_N( _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, NAME, ... ) NAME
#define CTRL_KERNEL_ARGS_TO_POINTERS( ... ) CTRL_KERNEL_ARGS_POINTERS_N( __VA_ARGS__, \
			CTRL_KERNEL_ARGS_POINTERS_20,                                             \
			CTRL_KERNEL_ARGS_POINTERS_19,                                             \
			CTRL_KERNEL_ARGS_POINTERS_18,                                             \
			CTRL_KERNEL_ARGS_POINTERS_17,                                             \
			CTRL_KERNEL_ARGS_POINTERS_16,                                             \
			CTRL_KERNEL_ARGS_POINTERS_15,                                             \
			CTRL_KERNEL_ARGS_POINTERS_14,                                             \
			CTRL_KERNEL_ARGS_POINTERS_13,                                             \
			CTRL_KERNEL_ARGS_POINTERS_12,                                             \
			CTRL_KERNEL_ARGS_POINTERS_11,                                             \
			CTRL_KERNEL_ARGS_POINTERS_10,                                             \
			CTRL_KERNEL_ARGS_POINTERS_9,                                              \
			CTRL_KERNEL_ARGS_POINTERS_8,                                              \
			CTRL_KERNEL_ARGS_POINTERS_7,                                              \
			CTRL_KERNEL_ARGS_POINTERS_6,                                              \
			CTRL_KERNEL_ARGS_POINTERS_5,                                              \
			CTRL_KERNEL_ARGS_POINTERS_4,                                              \
			CTRL_KERNEL_ARGS_POINTERS_3,                                              \
			CTRL_KERNEL_ARGS_POINTERS_2,                                              \
			CTRL_KERNEL_ARGS_POINTERS_1,                                              \
		)(__VA_ARGS__)

/*
 * Arguments list size (sum of the type sizes of all the arguments)
 */
#define CTRL_KERNEL_ARG_LIST_SIZE(numArgs, ...) CTRL_KERNEL_ARG_LIST_SIZE_##numArgs(__VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_SIZE_1(role, type, name)       sizeof(type)
#define CTRL_KERNEL_ARG_LIST_SIZE_2(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_1(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_3(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_2(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_4(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_3(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_5(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_4(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_6(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_5(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_7(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_6(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_8(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_7(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_9(role, type, name, ...)  sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_8(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_10(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_9(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_11(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_10(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_12(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_11(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_13(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_12(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_14(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_13(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_15(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_14(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_16(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_15(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_17(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_16(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_18(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_17(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_19(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_18(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_20(role, type, name, ...) sizeof(type) + CTRL_KERNEL_ARG_LIST_SIZE_19(__VA_ARGS__)

// TODO @waxa los siguientes macrosw funcionan pero generean casteos anidados de mas, arreglar esto en un futuro
/*
 * List store: Store a copy of the values in a contiguos buffer
 */
#define CTRL_KERNEL_ARG_LIST_STORE(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_STORE_##numArgs(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_STORE_1(list, role, type, name)       *((type *)(list)) = name
#define CTRL_KERNEL_ARG_LIST_STORE_2(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_1((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_3(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_2((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_4(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_3((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_5(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_4((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_6(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_5((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_7(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_6((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_8(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_7((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_9(list, role, type, name, ...)  *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_8((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_10(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_9((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_11(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_10((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_12(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_11((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_13(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_12((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_14(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_13((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_15(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_14((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_16(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_15((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_17(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_16((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_18(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_17((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_19(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_18((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_20(list, role, type, name, ...) *((type *)(list)) = name; CTRL_KERNEL_ARG_LIST_STORE_19((uint8_t *)(list) + sizeof(type), __VA_ARGS__)

/*
 * Create list buffer: Create the buffer and invocates the store macro
 */
#define CTRL_KERNEL_ARG_LIST_CREATE(listArgs, numArgs, ...)             \
	listArgs = malloc(CTRL_KERNEL_ARG_LIST_SIZE(numArgs, __VA_ARGS__)); \
	if (listArgs == NULL) {                                             \
		fprintf(stderr, "CTRL_KERNEL Internal error: Args Malloc\n");   \
		exit(-1);                                                       \
	}                                                                   \
	CTRL_KERNEL_ARG_LIST_STORE(listArgs, numArgs, __VA_ARGS__)

/*
 * Access to list buffer values: Macro to generate the code to access a specific list
 * 		of arguments types in a function call
 */
#define CTRL_KERNEL_ARG_LIST_ACCESS(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_ACCESS_##numArgs(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_ACCESS_1(list, role, type, name)       *((type *)(list))
#define CTRL_KERNEL_ARG_LIST_ACCESS_2(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_1((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_3(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_2((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_4(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_3((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_5(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_4((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_6(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_5((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_7(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_6((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_8(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_7((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_9(list, role, type, name, ...)  *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_8((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_10(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_9((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_11(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_10((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_12(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_11((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_13(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_12((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_14(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_13((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_15(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_14((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_16(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_15((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_17(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_16((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_18(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_17((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_19(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_18((uint8_t *)(list) + sizeof(type), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_20(list, role, type, name, ...) *((type *)(list)), CTRL_KERNEL_ARG_LIST_ACCESS_19((uint8_t *)(list) + sizeof(type), __VA_ARGS__)

/*
 *******************************************************************************************
 **************************************  KTILES  *******************************************
 *******************************************************************************************
 */

/* Copy parameter types and names for GPU*/
#define CTRL_KTILE_TYPED_INVAL(type, name) type name
#define CTRL_KTILE_TYPED_IN(type, name) K##type name
#define CTRL_KTILE_TYPED_IO(type, name) K##type name
#define CTRL_KTILE_TYPED_OUT(type, name) K##type name

#define CTRL_TYPED_KTILE(role, type, name) CTRL_KTILE_TYPED_##role(type, name)

#define CTRL_KERNEL_TYPED_KTILE(list, numArgs, ...)       CTRL_KERNEL_TYPED_KTILE_##numArgs(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_1(role, type, name)       CTRL_TYPED_KTILE(role, type, name)
#define CTRL_KERNEL_TYPED_KTILE_2(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_1(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_3(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_2(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_4(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_3(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_5(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_4(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_6(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_5(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_7(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_6(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_8(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_7(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_9(role, type, name, ...)  CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_8(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_10(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_9(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_11(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_10(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_12(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_11(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_13(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_12(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_14(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_13(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_15(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_14(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_16(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_15(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_17(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_16(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_18(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_17(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_19(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_18(__VA_ARGS__)
#define CTRL_KERNEL_TYPED_KTILE_20(role, type, name, ...) CTRL_TYPED_KTILE(role, type, name), CTRL_KERNEL_TYPED_KTILE_19(__VA_ARGS__)

/*
 * Arguments list size for GPU (sum of the type sizes of all the arguments)
 */
#define CTRL_KERNEL_SIZE_KTILE_INVAL(type) sizeof(type)
#define CTRL_KERNEL_SIZE_KTILE_IN(type) sizeof(K##type)
#define CTRL_KERNEL_SIZE_KTILE_IO(type) sizeof(K##type)
#define CTRL_KERNEL_SIZE_KTILE_OUT(type) sizeof(K##type)

#define CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) CTRL_KERNEL_SIZE_KTILE_##role(type)

#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE(numArgs, ...) CTRL_KERNEL_ARG_LIST_SIZE_KTILE_##numArgs(__VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_1(role, type, name)       CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_2(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_1(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_3(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_2(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_4(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_3(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_5(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_4(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_6(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_5(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_7(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_6(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_8(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_7(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_9(role, type, name, ...)  CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_8(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_10(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_9(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_11(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_10(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_12(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_11(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_13(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_12(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_14(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_13(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_15(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_14(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_16(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_15(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_17(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_16(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_18(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_17(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_19(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_18(__VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_SIZE_KTILE_20(role, type, name, ...) CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name) + CTRL_KERNEL_ARG_LIST_SIZE_KTILE_19(__VA_ARGS__)

/*
 * Macros for casting the arguments for the GPU devices
 */
#define CTRL_KERNEL_KTILE_STORE_INVAL(list, type, name) \
	*((type *)(list)) = (*name);

#define CTRL_KERNEL_KTILE_STORE_IN(list, type, name) \
	CTRL_KERNEL_KTILE_STORE_NO_INVAL(list, type, name)

#define CTRL_KERNEL_KTILE_STORE_OUT(list, type, name) \
	CTRL_KERNEL_KTILE_STORE_NO_INVAL(list, type, name)

#define CTRL_KERNEL_KTILE_STORE_IO(list, type, name) \
	CTRL_KERNEL_KTILE_STORE_NO_INVAL(list, type, name)

// TODO @waxa las lineas de deviceType hay que moverlas a compilacion condicional
#define CTRL_KERNEL_KTILE_STORE_NO_INVAL(list, hit_type, name)                                                                              \
	KHitTile k_##name##_void;                                                                                                               \
	switch (p_ctrl->type) {                                                                                                                 \
		CTRL_KERNEL_CPU_KTILE_DEVICE_DATA(name)                                                                                             \
		CTRL_KERNEL_CUDA_KTILE_DEVICE_DATA(name)                                                                                            \
		CTRL_KERNEL_HIP_KTILE_DEVICE_DATA(name)                                                                                             \
		CTRL_KERNEL_OPENCL_KTILE_DEVICE_DATA(name)                                                                                          \
		CTRL_KERNEL_FPGA_KTILE_DEVICE_DATA(name)                                                                                            \
		default:                                                                                                                            \
			fprintf(stderr, "[Ctrl_KernelArgs] Unsupported Ctrl type: %d. Recompile the library with the proper support.\n", p_ctrl->type); \
			exit(EXIT_FAILURE);                                                                                                             \
			break;                                                                                                                          \
	}                                                                                                                                       \
	K##hit_type k_##name;                                                                                                                   \
	memcpy(&k_##name, &k_##name##_void, sizeof(KHitTile));                                                                                  \
	k_##name.origAcumCard[0] = name->origAcumCard[0];                                                                                       \
	k_##name.origAcumCard[1] = name->origAcumCard[1];                                                                                       \
	k_##name.origAcumCard[2] = name->origAcumCard[2];                                                                                       \
	k_##name.origAcumCard[3] = name->origAcumCard[3];                                                                                       \
	k_##name.card[0]         = hit_tileDimCard((*name), 0);                                                                                 \
	k_##name.card[1]         = hit_tileDimCard((*name), 1);                                                                                 \
	k_##name.card[2]         = hit_tileDimCard((*name), 2);                                                                                 \
	/* Offset for subselections. Most times will be 0. */                                                                                   \
	{                                                                                                                                       \
		hit_type *p_parent = name;                                                                                                          \
		while (p_parent->memStatus == HIT_MS_NOT_OWNER)                                                                                     \
			p_parent = p_parent->ref;                                                                                                       \
		k_##name.offset = ((size_t)name->data - (size_t)p_parent->data) / name->baseExtent;                                                 \
	}                                                                                                                                       \
	*((K##hit_type *)(list)) = k_##name;

/*
 * List store for Ktile types: Store a copy of the values in a contiguos buffer, also casting to kTiles and transferring memory
 */
#define CTRL_KERNEL_KTILE_STORE(list, role, type, name) CTRL_KERNEL_KTILE_STORE_##role(list, type, name)

#define CTRL_KERNEL_ARG_LIST_STORE_KTILE(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_STORE_KTILE_##numArgs(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_1(list, role, type, name)       CTRL_KERNEL_KTILE_STORE(list, role, type, name);
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_2(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_1((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_3(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_2((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_4(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_3((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_5(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_4((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_6(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_5((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_7(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_6((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_8(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_7((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_9(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_8((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_10(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_9((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_11(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_10((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_12(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_11((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_13(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_12((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_14(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_13((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_15(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_14((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_16(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_15((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_17(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_16((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_18(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_17((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_19(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_18((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_KTILE_20(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, role, type, name); CTRL_KERNEL_ARG_LIST_STORE_KTILE_19((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)

/*
 * Create list buffer: Create the buffer and invocates the store macro
 */
#define CTRL_KERNEL_ARG_LIST_CREATE_KTILE(listArgs, numArgs, ...)             \
	listArgs = malloc(CTRL_KERNEL_ARG_LIST_SIZE_KTILE(numArgs, __VA_ARGS__)); \
	if (listArgs == NULL) {                                                   \
		fprintf(stderr, "CTRL KERNEL Internal error: Args Malloc\n");         \
		exit(-1);                                                             \
	}                                                                         \
	CTRL_KERNEL_ARG_LIST_STORE_KTILE(listArgs, numArgs, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_STORE_TILE(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_STORE_TILE_##numArgs(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_STORE_TILE_1(list, role, type, name)       CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name);
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_2(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_1((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_3(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_2((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_4(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_3((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_5(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_4((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_6(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_5((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_7(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_6((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_8(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_7((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_9(list, role, type, name, ...)  CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_8((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_10(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_9((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_11(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_10((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_12(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_11((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_13(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_12((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_14(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_13((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_15(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_14((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_16(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_15((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_17(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_16((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_18(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_17((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_19(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_18((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_STORE_TILE_20(list, role, type, name, ...) CTRL_KERNEL_KTILE_STORE(list, INVAL, type, name); CTRL_KERNEL_ARG_LIST_STORE_TILE_19((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_CREATE_TILE(listArgs, numArgs, ...)        \
	listArgs = malloc(CTRL_KERNEL_ARG_LIST_SIZE(numArgs, __VA_ARGS__)); \
	if (listArgs == NULL) {                                             \
		fprintf(stderr, "CTRL KERNEL Internal error: Args Malloc\n");   \
		exit(-1);                                                       \
	}                                                                   \
	CTRL_KERNEL_ARG_LIST_STORE_TILE(listArgs, numArgs, __VA_ARGS__)

/*
 * Access to list buffer values: Macro to generate the code to access a specific list
 * 		of arguments types in a function call
 */
#define CTRL_KERNEL_KTILE_ACCESS_INVAL(list, type, name) *((type *)(list))
#define CTRL_KERNEL_KTILE_ACCESS_IN(list, type, name)    *((K##type *)(list))
#define CTRL_KERNEL_KTILE_ACCESS_IO(list, type, name)    *((K##type *)(list))
#define CTRL_KERNEL_KTILE_ACCESS_OUT(list, type, name)   *((K##type *)(list))

#define CTRL_KERNEL_KTILE_ACCESS(list, role, type, name) CTRL_KERNEL_KTILE_ACCESS_##role(list, type, name)

#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_##numArgs(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_1(list, role, type, name)       CTRL_KERNEL_KTILE_ACCESS(list, role, type, name)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_2(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_1((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_3(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_2((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_4(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_3((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_5(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_4((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_6(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_5((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_7(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_6((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_8(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_7((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_9(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_8((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_10(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_9((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_11(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_10((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_12(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_11((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_13(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_12((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_14(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_13((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_15(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_14((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_16(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_15((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_17(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_16((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_18(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_17((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_19(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_18((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_20(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_19((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)

#define CTRL_KERNEL_ARG_COUNT_LIST_ELEMENTS(_var, numArgs, ...) numArgs

#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_ACCESS_TILE_##numArgs(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_1(list, role, type, name)       CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_2(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_1((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_3(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_2((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_4(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_3((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_5(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_4((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_6(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_5((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_7(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_6((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_8(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_7((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_9(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_8((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_10(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_9((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_11(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_10((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_12(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_11((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_13(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_12((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_14(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_13((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_15(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_14((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_16(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_15((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_17(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_16((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_18(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_17((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_19(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_18((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_TILE_20(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS(list, INVAL, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_TILE_19((uint8_t *)(list) + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name), __VA_ARGS__)

/*
 * Access to list buffer references: Macro to generate the code to access a specific list
 *		of references to arguments in a function call
 */
#define CTRL_KERNEL_KTILE_ACCESS_INVAL_REF(list, type, name) ((type *)(list))
#define CTRL_KERNEL_KTILE_ACCESS_IN_REF(list, type, name)    ((K##type *)(list))
#define CTRL_KERNEL_KTILE_ACCESS_IO_REF(list, type, name)    ((K##type *)(list))
#define CTRL_KERNEL_KTILE_ACCESS_OUT_REF(list, type, name)   ((K##type *)(list))

#define CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name) CTRL_KERNEL_KTILE_ACCESS_##role##_REF(list, type, name)

#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_REF(list, numArgs, ...) CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_##numArgs##_REF(list, __VA_ARGS__)

#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_1_REF(list, role, type, name)       CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_2_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_1_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_3_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_2_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_4_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_3_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_5_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_4_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_6_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_5_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_7_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_6_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_8_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_7_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_9_REF(list, role, type, name, ...)  CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_8_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_10_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_9_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_11_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_10_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_12_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_11_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_13_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_12_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_14_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_13_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_15_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_14_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_16_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_15_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_17_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_16_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_18_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_17_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_19_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_18_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)
#define CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_20_REF(list, role, type, name, ...) CTRL_KERNEL_KTILE_ACCESS_REF(list, role, type, name), CTRL_KERNEL_ARG_LIST_ACCESS_KTILE_19_REF((uint8_t *)list + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name), __VA_ARGS__)

/*
 *******************************************************************************************
 *************************************  Common Macros  *************************************
 *******************************************************************************************
 */

/* Skip the implementation names, and apply another macro to the parameter list */
#define CTRL_KERNEL_SKIP_IMPL(action, list, numImpl, ...) CTRL_KERNEL_SKIP_IMPL_##numImpl(action, list, __VA_ARGS__)

#define CTRL_KERNEL_SKIP_IMPL_1(action, list, impl, subimpl, ...)  CTRL_KERNEL_##action(list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_2(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_1(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_3(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_2(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_4(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_3(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_5(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_4(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_6(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_5(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_7(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_6(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_8(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_7(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_9(action, list, impl, subimpl, ...)  CTRL_KERNEL_SKIP_IMPL_8(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_10(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_9(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_11(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_10(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_12(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_11(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_13(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_12(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_14(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_13(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_15(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_14(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_16(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_15(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_17(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_16(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_18(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_17(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_19(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_18(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_20(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_19(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_21(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_20(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_22(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_21(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_23(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_22(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_24(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_23(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_25(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_24(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_26(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_25(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_27(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_26(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_28(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_27(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_29(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_28(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_30(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_29(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_31(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_30(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_32(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_31(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_33(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_32(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_34(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_33(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_35(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_34(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_36(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_35(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_37(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_36(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_38(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_37(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_39(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_38(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_40(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_39(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_41(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_40(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_42(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_41(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_43(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_42(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_44(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_43(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_45(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_44(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_46(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_45(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_47(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_46(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_48(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_47(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_49(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_48(action, list, __VA_ARGS__)
#define CTRL_KERNEL_SKIP_IMPL_50(action, list, impl, subimpl, ...) CTRL_KERNEL_SKIP_IMPL_49(action, list, __VA_ARGS__)

/* Copy parameter roles, types and names */
#define CTRL_KERNEL_ROLED(list, numArgs, ...) numArgs, CTRL_KERNEL_ROLED_##numArgs(__VA_ARGS__)

#define CTRL_KERNEL_ROLED_1(role, type, name)       role, type, name
#define CTRL_KERNEL_ROLED_2(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_1(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_3(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_2(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_4(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_3(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_5(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_4(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_6(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_5(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_7(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_6(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_8(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_7(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_9(role, type, name, ...)  role, type, name, CTRL_KERNEL_ROLED_8(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_10(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_9(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_11(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_10(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_12(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_11(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_13(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_12(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_14(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_13(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_15(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_14(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_16(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_15(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_17(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_16(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_18(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_17(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_19(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_18(__VA_ARGS__)
#define CTRL_KERNEL_ROLED_20(role, type, name, ...) role, type, name, CTRL_KERNEL_ROLED_19(__VA_ARGS__)

/* Copy parameter names */
#define CTRL_KERNEL_VALUE(list, numArgs2, ...) CTRL_KERNEL_VALUE_##numArgs2(__VA_ARGS__)

#define CTRL_KERNEL_VALUE_1(role, type, name)       name
#define CTRL_KERNEL_VALUE_2(role, type, name, ...)  name, CTRL_KERNEL_VALUE_1(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_3(role, type, name, ...)  name, CTRL_KERNEL_VALUE_2(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_4(role, type, name, ...)  name, CTRL_KERNEL_VALUE_3(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_5(role, type, name, ...)  name, CTRL_KERNEL_VALUE_4(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_6(role, type, name, ...)  name, CTRL_KERNEL_VALUE_5(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_7(role, type, name, ...)  name, CTRL_KERNEL_VALUE_6(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_8(role, type, name, ...)  name, CTRL_KERNEL_VALUE_7(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_9(role, type, name, ...)  name, CTRL_KERNEL_VALUE_8(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_10(role, type, name, ...) name, CTRL_KERNEL_VALUE_9(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_11(role, type, name, ...) name, CTRL_KERNEL_VALUE_10(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_12(role, type, name, ...) name, CTRL_KERNEL_VALUE_11(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_13(role, type, name, ...) name, CTRL_KERNEL_VALUE_12(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_14(role, type, name, ...) name, CTRL_KERNEL_VALUE_13(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_15(role, type, name, ...) name, CTRL_KERNEL_VALUE_14(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_16(role, type, name, ...) name, CTRL_KERNEL_VALUE_15(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_17(role, type, name, ...) name, CTRL_KERNEL_VALUE_16(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_18(role, type, name, ...) name, CTRL_KERNEL_VALUE_17(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_19(role, type, name, ...) name, CTRL_KERNEL_VALUE_18(__VA_ARGS__)
#define CTRL_KERNEL_VALUE_20(role, type, name, ...) name, CTRL_KERNEL_VALUE_19(__VA_ARGS__)

/* Create list of roles */
#define KERNEL_IN    0
#define KERNEL_OUT   1
#define KERNEL_IO    2
#define KERNEL_INVAL 3

#define CTRL_KERNEL_ROLES(rolesList, numArgs, ...)                                                                  \
	rolesList = (char *)malloc(numArgs * sizeof(char));                                                             \
	if (rolesList == NULL) {                                                                                        \
		fprintf(stderr, "CTRL Internal error: Allocating task memory for roles list %s[%d]\n", __FILE__, __LINE__); \
		exit(12);                                                                                                   \
	}                                                                                                               \
	CTRL_KERNEL_ROLES_##numArgs(rolesList, numArgs, __VA_ARGS__)

#define CTRL_KERNEL_ROLES_1(rolesList, numArgs, role, type, name)       rolesList[numArgs - 1] = KERNEL_##role;
#define CTRL_KERNEL_ROLES_2(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 2] = KERNEL_##role; CTRL_KERNEL_ROLES_1(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_3(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 3] = KERNEL_##role; CTRL_KERNEL_ROLES_2(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_4(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 4] = KERNEL_##role; CTRL_KERNEL_ROLES_3(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_5(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 5] = KERNEL_##role; CTRL_KERNEL_ROLES_4(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_6(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 6] = KERNEL_##role; CTRL_KERNEL_ROLES_5(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_7(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 7] = KERNEL_##role; CTRL_KERNEL_ROLES_6(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_8(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 8] = KERNEL_##role; CTRL_KERNEL_ROLES_7(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_9(rolesList, numArgs, role, type, name, ...)  rolesList[numArgs - 9] = KERNEL_##role; CTRL_KERNEL_ROLES_8(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_10(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 10] = KERNEL_##role; CTRL_KERNEL_ROLES_9(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_11(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 11] = KERNEL_##role; CTRL_KERNEL_ROLES_10(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_12(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 12] = KERNEL_##role; CTRL_KERNEL_ROLES_11(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_13(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 13] = KERNEL_##role; CTRL_KERNEL_ROLES_12(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_14(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 14] = KERNEL_##role; CTRL_KERNEL_ROLES_13(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_15(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 15] = KERNEL_##role; CTRL_KERNEL_ROLES_14(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_16(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 16] = KERNEL_##role; CTRL_KERNEL_ROLES_15(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_17(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 17] = KERNEL_##role; CTRL_KERNEL_ROLES_16(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_18(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 18] = KERNEL_##role; CTRL_KERNEL_ROLES_17(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_19(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 19] = KERNEL_##role; CTRL_KERNEL_ROLES_18(rolesList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_ROLES_20(rolesList, numArgs, role, type, name, ...) rolesList[numArgs - 20] = KERNEL_##role; CTRL_KERNEL_ROLES_19(rolesList, numArgs, __VA_ARGS__)

/* Create list of pointers */
#define CTRL_KERNEL_POINTERS(pointersList, numArgs, ...)                                                               \
	pointersList = (void **)malloc(numArgs * sizeof(void *));                                                          \
	if (pointersList == NULL) {                                                                                        \
		fprintf(stderr, "CTRL Internal Error: Allocating task memory for pointers list %s[%d]\n", __FILE__, __LINE__); \
		exit(EXIT_FAILURE);                                                                                            \
	}                                                                                                                  \
	CTRL_KERNEL_POINTERS_##numArgs(pointersList, numArgs, __VA_ARGS__)

#define CTRL_KERNEL_POINTERS_1(pointersList, numArgs, role, type, name)       pointersList[numArgs - 1] = (void *)(name);
#define CTRL_KERNEL_POINTERS_2(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 2] = (void *)(name); CTRL_KERNEL_POINTERS_1(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_3(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 3] = (void *)(name); CTRL_KERNEL_POINTERS_2(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_4(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 4] = (void *)(name); CTRL_KERNEL_POINTERS_3(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_5(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 5] = (void *)(name); CTRL_KERNEL_POINTERS_4(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_6(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 6] = (void *)(name); CTRL_KERNEL_POINTERS_5(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_7(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 7] = (void *)(name); CTRL_KERNEL_POINTERS_6(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_8(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 8] = (void *)(name); CTRL_KERNEL_POINTERS_7(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_9(pointersList, numArgs, role, type, name, ...)  pointersList[numArgs - 9] = (void *)(name); CTRL_KERNEL_POINTERS_8(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_10(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 10] = (void *)(name); CTRL_KERNEL_POINTERS_9(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_11(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 11] = (void *)(name); CTRL_KERNEL_POINTERS_10(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_12(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 12] = (void *)(name); CTRL_KERNEL_POINTERS_11(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_13(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 13] = (void *)(name); CTRL_KERNEL_POINTERS_12(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_14(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 14] = (void *)(name); CTRL_KERNEL_POINTERS_13(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_15(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 15] = (void *)(name); CTRL_KERNEL_POINTERS_14(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_16(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 16] = (void *)(name); CTRL_KERNEL_POINTERS_15(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_17(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 17] = (void *)(name); CTRL_KERNEL_POINTERS_16(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_18(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 18] = (void *)(name); CTRL_KERNEL_POINTERS_17(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_19(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 19] = (void *)(name); CTRL_KERNEL_POINTERS_18(pointersList, numArgs, __VA_ARGS__)
#define CTRL_KERNEL_POINTERS_20(pointersList, numArgs, role, type, name, ...) pointersList[numArgs - 20] = (void *)(name); CTRL_KERNEL_POINTERS_19(pointersList, numArgs, __VA_ARGS__)

/* Create list of displacement */
#define CTRL_KERNEL_DISPLACEMENTS(displacementsList, numArgs, ...)                                                     \
	displacementsList = (uint16_t *)malloc((numArgs + 1) * sizeof(uint16_t));                                          \
	if (displacementsList == NULL) {                                                                                   \
		fprintf(stderr, "CTRL Internal Error: Allocating task memory for pointers list %s[%d]\n", __FILE__, __LINE__); \
		exit(EXIT_FAILURE);                                                                                            \
	}                                                                                                                  \
	displacementsList[0] = 0;                                                                                          \
	CTRL_KERNEL_DISPLACEMENTS_##numArgs(displacementsList, numArgs, __VA_ARGS__)

#define CTRL_KERNEL_DISPLACEMENTS_1(displacementsList, numArgs, role, type, name)       displacementsList[numArgs] = displacementsList[numArgs - 1] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name);
#define CTRL_KERNEL_DISPLACEMENTS_2(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 1] = displacementsList[numArgs - 2] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_1(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_3(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 2] = displacementsList[numArgs - 3] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_2(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_4(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 3] = displacementsList[numArgs - 4] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_3(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_5(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 4] = displacementsList[numArgs - 5] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_4(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_6(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 5] = displacementsList[numArgs - 6] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_5(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_7(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 6] = displacementsList[numArgs - 7] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_6(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_8(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 7] = displacementsList[numArgs - 8] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_7(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_9(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 8] = displacementsList[numArgs - 9] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_8(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_10(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 9] = displacementsList[numArgs - 10] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_9(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_11(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 10] = displacementsList[numArgs - 11] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_10(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_12(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 11] = displacementsList[numArgs - 12] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_11(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_13(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 12] = displacementsList[numArgs - 13] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_12(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_14(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 13] = displacementsList[numArgs - 14] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_13(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_15(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 14] = displacementsList[numArgs - 15] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_14(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_16(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 15] = displacementsList[numArgs - 16] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_15(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_17(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 16] = displacementsList[numArgs - 17] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_16(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_18(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 17] = displacementsList[numArgs - 18] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_17(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_19(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 18] = displacementsList[numArgs - 19] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_18(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_20(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 19] = displacementsList[numArgs - 20] + CTRL_KERNEL_LIST_SIZE_KTILE(role, type, name); CTRL_KERNEL_DISPLACEMENTS_19(displacementsList, numArgs, __VA_ARGS__);

#define CTRL_KERNEL_DISPLACEMENTS_TILES(displacementsList, numArgs, ...)                                               \
	displacementsList = (uint16_t *)malloc((numArgs + 1) * sizeof(uint16_t));                                          \
	if (displacementsList == NULL) {                                                                                   \
		fprintf(stderr, "CTRL Internal Error: Allocating task memory for pointers list %s[%d]\n", __FILE__, __LINE__); \
		exit(EXIT_FAILURE);                                                                                            \
	}                                                                                                                  \
	displacementsList[0] = 0;                                                                                          \
	CTRL_KERNEL_DISPLACEMENTS_TILES_##numArgs(displacementsList, numArgs, __VA_ARGS__)

#define CTRL_KERNEL_DISPLACEMENTS_TILES_1(displacementsList, numArgs, role, type, name)       displacementsList[numArgs] = displacementsList[numArgs - 1] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_2(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 1] = displacementsList[numArgs - 2] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_1(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_3(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 2] = displacementsList[numArgs - 3] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_2(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_4(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 3] = displacementsList[numArgs - 4] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_3(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_5(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 4] = displacementsList[numArgs - 5] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_4(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_6(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 5] = displacementsList[numArgs - 6] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_5(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_7(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 6] = displacementsList[numArgs - 7] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_6(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_8(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 7] = displacementsList[numArgs - 8] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_7(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_9(displacementsList, numArgs, role, type, name, ...)  displacementsList[numArgs - 8] = displacementsList[numArgs - 9] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_8(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_10(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 9] = displacementsList[numArgs - 10] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_9(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_11(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 10] = displacementsList[numArgs - 11] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_10(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_12(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 11] = displacementsList[numArgs - 12] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_11(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_13(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 12] = displacementsList[numArgs - 13] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_12(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_14(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 13] = displacementsList[numArgs - 14] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_13(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_15(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 14] = displacementsList[numArgs - 15] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_14(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_16(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 15] = displacementsList[numArgs - 16] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_15(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_17(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 16] = displacementsList[numArgs - 17] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_16(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_18(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 17] = displacementsList[numArgs - 18] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_17(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_19(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 18] = displacementsList[numArgs - 19] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_18(displacementsList, numArgs, __VA_ARGS__);
#define CTRL_KERNEL_DISPLACEMENTS_TILES_20(displacementsList, numArgs, role, type, name, ...) displacementsList[numArgs - 19] = displacementsList[numArgs - 20] + CTRL_KERNEL_LIST_SIZE_KTILE(INVAL, type, name); CTRL_KERNEL_DISPLACEMENTS_TILES_19(displacementsList, numArgs, __VA_ARGS__);

#define CTRL_KERNEL_EXTRACT_KERNEL_1(kernel)    CTRL_MACRO_STRINGIFY(kernel);
#define CTRL_KERNEL_EXTRACT_KERNEL_2(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_1(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_3(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_2(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_4(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_3(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_5(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_4(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_6(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_5(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_7(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_6(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_8(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_7(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_9(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_8(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_10(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_9(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_11(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_10(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_12(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_11(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_13(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_12(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_14(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_13(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_15(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_14(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_16(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_15(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_17(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_16(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_18(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_17(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_19(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_18(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_20(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_19(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_21(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_20(__VA_ARGS__)

#define CTRL_KERNEL_COUNT_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, N, ...) N
#define CTRL_KERNEL_EXTRACT_KERNEL(...)                                 \
	CTRL_KERNEL_COUNT_ARG_N(__VA_ARGS__,                                \
							CTRL_KERNEL_EXTRACT_KERNEL_21(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_20(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_19(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_18(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_17(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_16(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_15(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_14(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_13(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_12(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_11(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_10(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_9(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_8(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_7(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_6(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_5(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_4(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_3(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_2(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_1(__VA_ARGS__))

/* @author: Gabriel Rodriguez-Canal
   @brief There must not be a semicolon after the kernel attributes
*/
#ifndef CTRL_FPGA_KERNEL_FILE
	#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_1(kernel) kernel;
#else // CTRL_FPGA_KERNEL_FILE
	#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_1(kernel) kernel
#endif // CTRL_FPGA_KERNEL_FILE

#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_2(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_1(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_3(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_2(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_4(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_3(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_5(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_4(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_6(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_5(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_7(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_6(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_8(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_7(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_9(arg, ...)  CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_8(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_10(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_9(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_11(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_10(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_12(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_11(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_13(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_12(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_14(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_13(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_15(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_14(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_16(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_15(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_17(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_16(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_18(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_17(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_19(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_18(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_20(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_19(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_21(arg, ...) CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_20(__VA_ARGS__)

#define CTRL_KERNEL_EXTRACT_KERNEL_NO_STR(...)                                 \
	CTRL_KERNEL_COUNT_ARG_N(__VA_ARGS__,                                       \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_21(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_20(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_19(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_18(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_17(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_16(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_15(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_14(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_13(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_12(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_11(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_10(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_9(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_8(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_7(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_6(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_5(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_4(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_3(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_2(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_KERNEL_NO_STR_1(__VA_ARGS__))

#define CTRL_KERNEL_EXTRACT_ARG_1(kernel)
#define CTRL_KERNEL_EXTRACT_ARG_2(arg, ...)  arg
#define CTRL_KERNEL_EXTRACT_ARG_3(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_2(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_4(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_3(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_5(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_4(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_6(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_5(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_7(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_6(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_8(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_7(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_9(arg, ...)  arg, CTRL_KERNEL_EXTRACT_ARG_8(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_10(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_9(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_11(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_10(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_12(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_11(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_13(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_12(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_14(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_13(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_15(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_14(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_16(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_15(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_17(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_16(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_18(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_17(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_19(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_18(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_20(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_19(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_ARG_21(arg, ...) arg, CTRL_KERNEL_EXTRACT_ARG_20(__VA_ARGS__)

#define CTRL_KERNEL_EXTRACT_ARGS(...)                                \
	CTRL_KERNEL_COUNT_ARG_N(__VA_ARGS__,                             \
							CTRL_KERNEL_EXTRACT_ARG_21(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_20(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_19(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_18(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_17(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_16(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_15(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_14(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_13(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_12(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_11(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_10(__VA_ARGS__), \
							CTRL_KERNEL_EXTRACT_ARG_9(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_8(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_7(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_6(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_5(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_4(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_3(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_2(__VA_ARGS__),  \
							CTRL_KERNEL_EXTRACT_ARG_1(__VA_ARGS__))

/*
 * Extract the kernel args from CTRL_KERNEL_PROTO().
 * Used within CTRL_KERNEL_DECLARATION() macro, which is used to
 * separate the kernels from the host code.
 */
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_1(role, type, name)       CTRL_KTILE_TYPED_##role(type, name)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_2(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_1(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_3(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_2(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_4(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_3(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_5(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_4(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_6(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_5(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_7(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_6(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_8(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_7(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_9(role, type, name, ...)  CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_8(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_10(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_9(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_11(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_10(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_12(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_11(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_13(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_12(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_14(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_13(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_15(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_14(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_16(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_15(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_17(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_16(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_18(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_17(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_19(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_18(__VA_ARGS__)
#define CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_20(role, type, name, ...) CTRL_KTILE_TYPED_##role(type, name), CTRL_KERNEL_EXTRACT_DECLARATION_ARGS_19(__VA_ARGS__)

#endif // _CTRL_KERNEL_ARGS_H_
