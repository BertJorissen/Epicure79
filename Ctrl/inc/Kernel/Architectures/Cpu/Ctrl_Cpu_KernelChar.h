#ifndef _CTRL_KERNEL_CPU_CHAR_H_
#define _CTRL_KERNEL_CPU_CHAR_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_KernelChar.h
 * @author Trasgo Group
 * @brief Macros to set Cpu kernels characterization (tile size).
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