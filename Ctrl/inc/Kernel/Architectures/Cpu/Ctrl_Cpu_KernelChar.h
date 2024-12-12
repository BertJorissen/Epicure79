#ifndef _CTRL_KERNEL_CPU_CHAR_H_
#define _CTRL_KERNEL_CPU_CHAR_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_KernelChar.h
 * @brief Macros to set Cpu kernels characterization (tile size).
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <Kernel/Ctrl_KernelArgs.h>

/**
 * Create default characterization for a \e CPU kernel or a \e GENERIC kernel when using \e CPU type ctrl.
 *
 * This is called by \e CTRL_KERNEL_CHAR when \e CPU support is activated.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of characterization desired, \e MANUAL or \e AUTOMATIC.
 * @param dims Number of dimensions of the kernel.
 * @param ... If \p type is \e MANUAL block sizes for each dimension. If \p type is \e AUTOMATIC, properties of the kernel.
 *
 * @see CTRL_KERNEL_CHAR
 */
#define CTRL_KERNEL_CPU_KERNEL_CHAR(name, type, dims, ...) \
	CTRL_KERNEL_CPU_KERNEL_CHAR_##type(name, dims, __VA_ARGS__)

/**
 * Create default characterization for a \e CPU kernel or a \e GENERIC kernel when using \e CPU type ctrl, using \e MANUAL
 * characterization mode.
 *
 * This is called by \e CTRL_KERNEL_CPU_KERNEL_CHAR when mode is \e MANUAL.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param dims Number of dimensions of the kernel.
 * @param ... Block sizes for each dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR
 */
#define CTRL_KERNEL_CPU_KERNEL_CHAR_1(name, dims, ...) \
	CTRL_KERNEL_CPU_KERNEL_CHAR_1_##dims(name, __VA_ARGS__)

/**
 * Create default characterization for a \e CPU kernel or a \e GENERIC kernel when using \e CPU type ctrl, using \e MANUAL
 * characterization mode and a 1D block.
 *
 * This is called by \e CTRL_KERNEL_CPU_KERNEL_CHAR_1 when dims is 1.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_CPU_KERNEL_CHAR_1_1(name, X) \
	Ctrl_Thread blocksize_CPU_##name = {.dims = 1, .i = X, .j = 1, .k = 1};

/**
 * Create default characterization for a \e CPU kernel or a \e GENERIC kernel when using \e CPU type ctrl, using \e MANUAL
 * characterization mode and a 2D block.
 *
 * This is called by \e CTRL_KERNEL_CPU_KERNEL_CHAR_1 when dims is 2.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 * @param Y Block sizes for j dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_CPU_KERNEL_CHAR_1_2(name, X, Y) \
	Ctrl_Thread blocksize_CPU_##name = {.dims = 2, .i = X, .j = Y, .k = 1};

/**
 * Create default characterization for a \e CPU kernel or a \e GENERIC kernel when using \e CPU type ctrl, using \e MANUAL
 * characterization mode and a 3D block.
 *
 * This is called by \e CTRL_KERNEL_CPU_KERNEL_CHAR_1 when dims is 3.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 * @param Y Block sizes for j dimension.
 * @param Z Block sizes for k dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_CPU_KERNEL_CHAR_1_3(name, X, Y, Z) \
	Ctrl_Thread blocksize_CPU_##name = {.dims = 3, .i = X, .j = Y, .k = Z};

/**
 * Create default characterization for a \e CPU kernel or a \e GENERIC kernel when using \e CPU type ctrl, using \e AUTOMATIC
 * characterization mode.
 *
 * This is called by \e CTRL_KERNEL_CPU_KERNEL_CHAR when mode is \e AUTOMATIC.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param dims Number of dimensions of the kernel.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CPU_KERNEL_CHAR
 */
#define CTRL_KERNEL_CPU_KERNEL_CHAR_2(name, dims, ...) \
	Ctrl_Thread blocksize_CPU_##name = CTRL_KERNEL_CPU_CHAR_##dims##D

#define CTRL_KERNEL_CPU_CHAR_1D {.dims = 1, .i = 16, .j = 1, .k = 1};
#define CTRL_KERNEL_CPU_CHAR_2D {.dims = 2, .i = 16, .j = 16, .k = 1};
#define CTRL_KERNEL_CPU_CHAR_3D {.dims = 3, .i = 16, .j = 16, .k = 16};
///@endcond
#endif // _CTRL_KERNEL_CPU_CHAR_H_