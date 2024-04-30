#ifndef _CTRL_CORE_REQUEST_H_
#define _CTRL_CORE_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_Request.h
 * @author Trasgo Group
 * @brief Abstract Ctrl_Request definition.
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

#ifdef _CTRL_ARCH_CPU_
#include "Architectures/Cpu/Ctrl_Cpu_Request.h"
#endif // _CTRL_ARCH_CPU_

#ifdef _CTRL_ARCH_CUDA_
#include "Architectures/Cuda/Ctrl_Cuda_Request.h"
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_HIP_
#include "Architectures/Hip/Ctrl_Hip_Request.h"
#endif // _CTRL_ARCH_HIP_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include "Architectures/OpenCL/Ctrl_OpenCL_Request.h"
#endif // _CTRL_ARCH_OPENCL_GPU_

#ifdef _CTRL_ARCH_FPGA_
#include "Architectures/FPGA/Ctrl_FPGA_Request.h"
#endif // _CTRL_ARCH_FPGA_

/**
 * \brief Architecture specific info needed to execute a kernel
 */
typedef union {
	#ifdef _CTRL_ARCH_CPU_
	Ctrl_Cpu_Request cpu;
	#endif // _CTRL_ARCH_CPU_

	#ifdef _CTRL_ARCH_CUDA_
	Ctrl_Cuda_Request cuda;
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_HIP_
	Ctrl_Hip_Request hip;
	#endif // _CTRL_ARCH_HIP_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	Ctrl_OpenCL_Request opencl;
	#endif // _CTRL_ARCH_OPENCL_GPU_

	#ifdef _CTRL_ARCH_FPGA_
	Ctrl_FPGA_Request fpga;
	#endif // _CTRL_ARCH_FPGA_
} Ctrl_Request;
///@endcond
#endif // _CTRL_CORE_REQUEST_H_
