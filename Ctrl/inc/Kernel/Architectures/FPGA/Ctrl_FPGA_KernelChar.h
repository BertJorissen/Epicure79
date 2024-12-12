#ifndef _CTRL_KERNEL_FPGA_CHAR_H_
#define _CTRL_KERNEL_FPGA_CHAR_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_KernelChar.h
 * @brief Macros to set FPGA kernels characterization.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include <Kernel/Ctrl_KernelArgs.h>

/**
 * Create default characterization for a \e FPGA kernel.
 *
 * This is called by \e CTRL_KERNEL_CHAR when \e FPGA support is activated.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of characterization desired, \e MANUAL or \e AUTOMATIC.
 * @param dims Number of dimensions of the kernel.
 * @param ... If \p type is \e MANUAL block sizes for each dimension. If \p type is \e AUTOMATIC, properties of the kernel.
 *
 * @see CTRL_KERNEL_CHAR
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR(name, type, dims, ...) \
	CTRL_KERNEL_FPGA_KERNEL_CHAR_##type(name, dims, __VA_ARGS__)

/* Task kernel model */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_0(name, ...) \
	Ctrl_Thread local_size_FPGA_##name = CTRL_THREAD_NULL;

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode.
 *
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR when mode is \e MANUAL.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param dims Number of dimensions of the kernel.
 * @param ... Block sizes for each dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1(name, dims, ...) \
	CTRL_KERNEL_FPGA_KERNEL_CHAR_1_##dims(name, __VA_ARGS__)

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode and a 1D block.
 *
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR_1 when dims is 1.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1_1(name, X) \
	Ctrl_Thread local_size_FPGA_##name = {.dims = 1, .i = X, .j = 0, .k = 0};

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode and a 2D block.
 *
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR_1 when dims is 2.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 * @param Y Block sizes for j dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1_2(name, X, Y) \
	Ctrl_Thread local_size_FPGA_##name = {.dims = 2, .i = X, .j = Y, .k = 0};

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode and a 3D block.
 *
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR_1 when dims is 3.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 * @param Y Block sizes for j dimension.
 * @param Z Block sizes for k dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1_3(name, X, Y, Z) \
	Ctrl_Thread local_size_FPGA_##name = {.dims = 3, .i = X, .j = Y, .k = Z};

/**
 * Create default characterization for a \e FPGA kernel, using \e AUTOMATIC characterization mode.
 *
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR when mode is \e AUTOMATIC.
 * @hideinitializer
 *
 * @param name name of the kernel.
 * @param dims number of dimensions of the kernel.
 * @param A First property of the kernel.
 * @param B Second property of the kernel.
 * @param C Third property of the kernel.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_2(name, dims, A, B, C) \
	Ctrl_Thread local_size_FPGA_##name = CTRL_KERNEL_FPGA_CHAR_##dims##A##B##C

#define CTRL_KERNEL_FPGA_CHAR_1defdefdef          {.dims = 1, .i = 256, .j = 0, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2defdefdef          {.dims = 2, .i = 256, .j = 1, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_1mediumlowlow       {.dims = 1, .i = 128, .j = 0, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_1fulllowlow         {.dims = 1, .i = 256, .j = 0, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fulllowlow         {.dims = 2, .i = 128, .j = 2, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2square32lowlow     {.dims = 2, .i = 16, .j = 16, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fullmediummedium   {.dims = 2, .i = 128, .j = 2, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fulllowhigh        {.dims = 2, .i = 64, .j = 3, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2mediummediummedium {.dims = 2, .i = 128, .j = 2, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare32      {.dims = 2, .i = 16, .j = 16, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare16      {.dims = 2, .i = 16, .j = 16, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare4       {.dims = 2, .i = 4, .j = 4, .k = 0};
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare2       {.dims = 2, .i = 2, .j = 2, .k = 0};

///@endcond
#endif // _CTRL_KERNEL_FPGA_CHAR_H_