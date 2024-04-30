#ifndef _CTRL_KERNEL_CUDA_CHAR_H_
#define _CTRL_KERNEL_CUDA_CHAR_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cuda_KernelChar.h
 * @author Trasgo Group
 * @brief Macros to set CUDA kernels characterization.
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

#include <cuda_runtime_api.h>

#define CTRL_KERNEL_CUDA_NARCHS 4 // Número de arquitecturas que existen

/* Arquitecturas soportadas por la biblioteca */
#define CTRL_KERNEL_CUDA_ARCH_FERMI  2
#define CTRL_KERNEL_CUDA_ARCH_KEPLER 3

#define CTRL_KERNEL_CUDA_AUTOMATIC 1
#define CTRL_KERNEL_CUDA_MANUAL    2

/**
 * Expands to \e Ctrl_Thread with appropiate block sizes according to default characterization for this kernel.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param arch Architecture of the device, this is only used if default characterization mode is \e AUTOMATIC.
 *
 * @see Ctrl_Thread, CTRL_KERNEL_CHAR
 */
#define CTRL_KERNEL_CUDA_CHAR_threads(name, arch) \
	blocksize_CUDA_##name[CTRL_KERNEL_CUDA_CHAR_type_##name == CTRL_KERNEL_CUDA_AUTOMATIC ? arch : 0]

/**
 * Create default characterization for a \e CUDA kernel or a \e GENERIC kernel when using \e CUDA type ctrl.
 *
 * This is called by \e CTRL_KERNEL_CHAR when \e CUDA support is activated.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param type Type of characterization desired, \e MANUAL or \e AUTOMATIC.
 * @param dims Number of dimensions of the kernel.
 * @param ... If \p type is \e MANUAL block sizes for each dimension. If \p type is \e AUTOMATIC, properties of the kernel.
 *
 * @see CTRL_KERNEL_CHAR
 */
#define CTRL_KERNEL_CUDA_KERNEL_CHAR(name, type, dims, ...) \
	CTRL_KERNEL_CUDA_KERNEL_CHAR_##type(name, dims, __VA_ARGS__);

/**
 * Create default characterization for a \e CUDA kernel or a \e GENERIC kernel when using \e CUDA type ctrl, using \e MANUAL
 * characterization mode.
 *
 * This is called by \e CTRL_KERNEL_CUDA_KERNEL_CHAR when mode is \e MANUAL.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param dims Number of dimensions of the kernel.
 * @param ... Block sizes for each dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR
 */
#define CTRL_KERNEL_CUDA_KERNEL_CHAR_1(name, dims, ...)              \
	int CTRL_KERNEL_CUDA_CHAR_type_##name = CTRL_KERNEL_CUDA_MANUAL; \
	CTRL_KERNEL_CUDA_KERNEL_CHAR_1_##dims(name, __VA_ARGS__)

/**
 * Create default characterization for a \e CUDA kernel or a \e GENERIC kernel when using \e CUDA type ctrl, using \e MANUAL
 * characterization mode and a 1D block.
 *
 * This is called by \e CTRL_KERNEL_CUDA_KERNEL_CHAR_1 when dims is 1.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_CUDA_KERNEL_CHAR_1_1(name, X) \
	Ctrl_Thread blocksize_CUDA_##name[] = {{.dims = 1, .i = X, .j = 1, .k = 1}};

/**
 * Create default characterization for a \e CUDA kernel or a \e GENERIC kernel when using \e CUDA type ctrl, using \e MANUAL
 * characterization mode and a 2D block.
 *
 * This is called by \e CTRL_KERNEL_CUDA_KERNEL_CHAR_1 when dims is 2.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 * @param Y Block sizes for j dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_CUDA_KERNEL_CHAR_1_2(name, X, Y) \
	Ctrl_Thread blocksize_CUDA_##name[] = {{.dims = 2, .i = X, .j = Y, .k = 1}};

/**
 * Create default characterization for a \e CUDA kernel or a \e GENERIC kernel when using \e CUDA type ctrl, using \e MANUAL
 * characterization mode and a 3D block.
 *
 * This is called by \e CTRL_KERNEL_CUDA_KERNEL_CHAR_1 when dims is 3.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param X Block sizes for i dimension.
 * @param Y Block sizes for j dimension.
 * @param Z Block sizes for k dimension.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_CUDA_KERNEL_CHAR_1_3(name, X, Y, Z) \
	Ctrl_Thread blocksize_CUDA_##name[] = {{.dims = 3, .i = X, .j = Y, .k = Z}};

/**
 * Create default characterization for a \e CUDA kernel or a \e GENERIC kernel when using \e CUDA type ctrl, using
 * \e AUTOMATIC characterization mode.
 *
 * This is called by \e CTRL_KERNEL_CUDA_KERNEL_CHAR when mode is \e AUTOMATIC.
 * @hideinitializer
 *
 * @param name Name of the kernel.
 * @param dims Number of dimensions of the kernel.
 * @param A First property of the kernel.
 * @param B Second property of the kernel.
 * @param C Third property of the kernel.
 *
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_CUDA_KERNEL_CHAR
 */
#define CTRL_KERNEL_CUDA_KERNEL_CHAR_2(name, dims, A, B, C)                                  \
	int         CTRL_KERNEL_CUDA_CHAR_type_##name              = CTRL_KERNEL_CUDA_AUTOMATIC; \
	Ctrl_Thread blocksize_CUDA_##name[CTRL_KERNEL_CUDA_NARCHS] = CTRL_KERNEL_CUDA_CHAR_1##A##B##C

/*
 * Hilos por bloque para cada una de los tipos de kernels.
 * Cada uno del los valores corresponde con una arquitectura.
 */
#define CTRL_KERNEL_CUDA_CHAR_1defdefdef { \
	{.dims = 1, .i = 1, .j = 1, .k = 1},   \
	{.dims = 1, .i = 1, .j = 1, .k = 1},   \
	{.dims = 1, .i = 256, .j = 1, .k = 1}, \
	{.dims = 1, .i = 256, .j = 1, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2defdefdef { \
	{.dims = 2, .i = 1, .j = 1, .k = 1},   \
	{.dims = 2, .i = 1, .j = 1, .k = 1},   \
	{.dims = 2, .i = 1, .j = 256, .k = 1}, \
	{.dims = 2, .i = 1, .j = 256, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_1mediumlowlow { \
	{.dims = 1, .i = 128, .j = 1, .k = 1},    \
	{.dims = 1, .i = 128, .j = 1, .k = 1},    \
	{.dims = 1, .i = 128, .j = 1, .k = 1},    \
	{.dims = 1, .i = 128, .j = 1, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_1fulllowlow { \
	{.dims = 1, .i = 256, .j = 1, .k = 1},  \
	{.dims = 1, .i = 256, .j = 1, .k = 1},  \
	{.dims = 1, .i = 256, .j = 1, .k = 1},  \
	{.dims = 1, .i = 256, .j = 1, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fulllowlow { \
	{.dims = 2, .i = 2, .j = 128, .k = 1},  \
	{.dims = 2, .i = 2, .j = 128, .k = 1},  \
	{.dims = 2, .i = 2, .j = 128, .k = 1},  \
	{.dims = 2, .i = 2, .j = 128, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2square32lowlow { \
	{.dims = 2, .i = 32, .j = 32, .k = 1},      \
	{.dims = 2, .i = 32, .j = 32, .k = 1},      \
	{.dims = 2, .i = 32, .j = 32, .k = 1},      \
	{.dims = 2, .i = 32, .j = 32, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fullmediummedium { \
	{.dims = 2, .i = 2, .j = 128, .k = 1},        \
	{.dims = 2, .i = 2, .j = 128, .k = 1},        \
	{.dims = 2, .i = 2, .j = 128, .k = 1},        \
	{.dims = 2, .i = 2, .j = 128, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fulllowhigh { \
	{.dims = 2, .i = 1, .j = 1, .k = 1},     \
	{.dims = 2, .i = 1, .j = 1, .k = 1},     \
	{.dims = 2, .i = 3, .j = 64, .k = 1},    \
	{.dims = 2, .i = 4, .j = 32, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2mediummediummedium { \
	{.dims = 2, .i = 4, .j = 32, .k = 1},           \
	{.dims = 2, .i = 4, .j = 32, .k = 1},           \
	{.dims = 2, .i = 2, .j = 128, .k = 1},          \
	{.dims = 2, .i = 2, .j = 128, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fixedsquare32 { \
	{.dims = 2, .i = 16, .j = 16, .k = 1},     \
	{.dims = 2, .i = 16, .j = 16, .k = 1},     \
	{.dims = 2, .i = 32, .j = 32, .k = 1},     \
	{.dims = 2, .i = 32, .j = 32, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fixedsquare16 { \
	{.dims = 2, .i = 16, .j = 16, .k = 1},     \
	{.dims = 2, .i = 16, .j = 16, .k = 1},     \
	{.dims = 2, .i = 16, .j = 16, .k = 1},     \
	{.dims = 2, .i = 16, .j = 16, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fixedsquare4 { \
	{.dims = 2, .i = 4, .j = 4, .k = 1},      \
	{.dims = 2, .i = 4, .j = 4, .k = 1},      \
	{.dims = 2, .i = 4, .j = 4, .k = 1},      \
	{.dims = 2, .i = 4, .j = 4, .k = 1}};
#define CTRL_KERNEL_CUDA_CHAR_2fixedsquare2 { \
	{.dims = 2, .i = 2, .j = 2, .k = 1},      \
	{.dims = 2, .i = 2, .j = 2, .k = 1},      \
	{.dims = 2, .i = 2, .j = 2, .k = 1},      \
	{.dims = 2, .i = 2, .j = 2, .k = 1}};

///@endcond
#endif // _CTRL_KERNEL_CUDA_CHAR_H_
