#ifndef _CTRL_KERNEL_FPGA_CHAR_H_
#define _CTRL_KERNEL_FPGA_CHAR_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_KernelChar.h
 * @author Gabriel Rodriguez-Canal
 * @brief Macros to set FPGA kernels characterization.
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
#define CTRL_KERNEL_FPGA_KERNEL_CHAR( name, type, dims, ... ) \
		CTRL_KERNEL_FPGA_KERNEL_CHAR_##type( name, dims, __VA_ARGS__ )

/* Task kernel model */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_0( name, ... ) \
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
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1( name, dims, ... ) \
		CTRL_KERNEL_FPGA_KERNEL_CHAR_1_##dims( name, __VA_ARGS__ )

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode and a 1D block.
 * 
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR_1 when dims is 1.
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param X Block sizes for x dimension.
 * 
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1_1( name, X ) \
		Ctrl_Thread local_size_FPGA_##name = {.dims=1, .x=X, .y=0, .z=0};

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode and a 2D block.
 * 
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR_1 when dims is 2.
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param X Block sizes for x dimension.
 * @param Y Block sizes for y dimension.
 * 
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1_2( name, X, Y ) \
		Ctrl_Thread local_size_FPGA_##name = {.dims=2, .x=X, .y=Y, .z=0};

/**
 * Create default characterization for a \e FPGA kernel, using \e MANUAL characterization mode and a 3D block.
 * 
 * This is called by \e CTRL_KERNEL_FPGA_KERNEL_CHAR_1 when dims is 3.
 * @hideinitializer
 * 
 * @param name Name of the kernel.
 * @param X Block sizes for x dimension.
 * @param Y Block sizes for y dimension.
 * @param Z Block sizes for z dimension.
 * 
 * @see CTRL_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR, CTRL_KERNEL_FPGA_KERNEL_CHAR_1
 */
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_1_3( name, X, Y, Z ) \
		Ctrl_Thread local_size_FPGA_##name = {.dims=3, .x=X, .y=Y, .z=Z};

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
#define CTRL_KERNEL_FPGA_KERNEL_CHAR_2( name, dims, A, B, C ) \
		Ctrl_Thread local_size_FPGA_##name = CTRL_KERNEL_FPGA_CHAR_##dims##A##B##C;

#define CTRL_KERNEL_FPGA_CHAR_1defdefdef				{.dims=1, .x=256, .y=0, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2defdefdef				{.dims=2, .x=256, .y=1, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_1mediumlowlow				{.dims=1, .x=128, .y=0, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_1fulllowlow				{.dims=1, .x=256, .y=0, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fulllowlow				{.dims=2, .x=128, .y=2, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2square32lowlow			{.dims=2, .x=16, .y=16, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fullmediummedium			{.dims=2, .x=128, .y=2, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fulllowhigh				{.dims=2, .x=64, .y=3, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2mediummediummedium		{.dims=2, .x=128, .y=2, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare32			{.dims=2, .x=16, .y=16, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare16			{.dims=2, .x=16, .y=16, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare4				{.dims=2, .x=4, .y=4, .z=0}
#define CTRL_KERNEL_FPGA_CHAR_2fixedsquare2				{.dims=2, .x=2, .y=2, .z=0}

///@endcond 
#endif // _CTRL_KERNEL_FPGA_CHAR_H_