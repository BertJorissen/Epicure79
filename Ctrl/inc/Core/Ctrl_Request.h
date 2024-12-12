#ifndef _CTRL_CORE_REQUEST_H_
#define _CTRL_CORE_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_Request.h
 * @brief Abstract Ctrl_Request definition.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
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
