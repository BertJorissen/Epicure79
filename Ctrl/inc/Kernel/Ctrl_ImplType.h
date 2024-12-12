#ifndef _CTRL_KERNEL_IMPLTYPE_H_
#define _CTRL_KERNEL_IMPLTYPE_H_
/**
 * @file Ctrl_ImplType.h
 * @brief Definitions for kernel implementations, and masks to identify the implementations
 * that fit in a device/launching requirement.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/**
 * Size of the range in between kernel implementation sizes.
 * This determines how many subimplementations of each type are allowed.
 */
#define CTRL_IMPL_RANGE_SIZE 100

/**
 * Helper macro to test if a value corresponds to a implementation type.
 * @hideinitializer
 *
 * @param val Value to check.
 * @param impl_type Type to check against. This corresponds to only the \e type portion of a \e Ctrl_ImplType member.
 *
 * @pre \p impl_type must have a default subtype.
 * @see Ctrl_ImplType
 */
#define CTRL_IMPL_IN_RANGE(val, impl_type) ((val) >= impl_type##_DEFAULT && (val) < (impl_type##_DEFAULT + CTRL_IMPL_RANGE_SIZE))

/**
 * Types of possible implementations for kernels.
 * Members are of the form \c \<type\>_\<subype\>.
 * Order of members in this enum is important as it determines implementation preference order.
 * the higher the number, the more precedence that implementation has.
 *
 * @see CTRL_KERNEL, CTRL_KERNEL_PROTO
 */
typedef enum {
	GENERIC_DEFAULT = CTRL_IMPL_RANGE_SIZE * 0,
	CPU_DEFAULT     = CTRL_IMPL_RANGE_SIZE * 1,
	CPULIB_MKL      = CTRL_IMPL_RANGE_SIZE * 2,
	CUDA_DEFAULT    = CTRL_IMPL_RANGE_SIZE * 3,
	CUDA_FERMI,
	CUDA_KEPLER,
	CUDA_MAXWELL,
	CUDA_PASCAL,
	CUDA_VOLTA,
	CUDA_TURING,
	CUDA_AMPERE,
	CUDALIB_DEFAULT = CTRL_IMPL_RANGE_SIZE * 4,
	CUDALIB_MAGMA,
	CUDALIB_CUBLAS,
	HIP_DEFAULT    = CTRL_IMPL_RANGE_SIZE * 5,
	HIPLIB_DEFAULT = CTRL_IMPL_RANGE_SIZE * 6,
	HIPLIB_HIPBLAS,
	OPENCLGPU_DEFAULT    = CTRL_IMPL_RANGE_SIZE * 7,
	OPENCLGPULIB_DEFAULT = CTRL_IMPL_RANGE_SIZE * 8,
	FPGA_DEFAULT         = CTRL_IMPL_RANGE_SIZE * 9,
	FPGALIB_DEFAULT      = CTRL_IMPL_RANGE_SIZE * 10,
} Ctrl_ImplType;

#endif // _CTRL_KERNEL_IMPLTYPE_H_
