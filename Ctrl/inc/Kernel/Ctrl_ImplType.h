#ifndef _CTRL_KERNEL_IMPLTYPE_H_
#define _CTRL_KERNEL_IMPLTYPE_H_
/**
 * @file Ctrl_ImplType.h
 * @author Trasgo Group
 * @brief Definitions for kernel implementations, and masks to identify the implementations
 * that fit in a device/launching requirement.
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
	CUDALIB_MAGMA = CTRL_IMPL_RANGE_SIZE * 4,
	CUDALIB_CUBLAS,
	OPENCLGPU_DEFAULT    = CTRL_IMPL_RANGE_SIZE * 5,
	OPENCLGPULIB_DEFAULT = CTRL_IMPL_RANGE_SIZE * 6,
	FPGA_DEFAULT         = CTRL_IMPL_RANGE_SIZE * 7,
	FPGALIB_DEFAULT      = CTRL_IMPL_RANGE_SIZE * 8,
} Ctrl_ImplType;

#endif // _CTRL_KERNEL_IMPLTYPE_H_
