#ifndef _CTRL_PROFILER_HELPER_H_
#define _CTRL_PROFILER_HELPER_H_
/**
 * @file Ctrl_Profiler_Helper.h
 * @brief Helper macros for profiling marks on host tasks
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */
// clang-format off
#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
		#include "nvToolsExt.h"
		#define CTRL_PROF_NVTX_RANGEPUSH(name) nvtxRangePushA(name);
		#define CTRL_PROF_NVTX_RANGEPOP()      nvtxRangePop();
	#else // _CTRL_ARCH_CUDA_
		#define CTRL_PROF_NVTX_RANGEPUSH(name)
		#define CTRL_PROF_NVTX_RANGEPOP()
	#endif // _CTRL_ARCH_CUDA_

	#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_HIP_)
		#include <roctracer/roctx.h>
		#define CTRL_PROF_ROCTX_RANGEPUSH(name) roctxRangePush(name);
		#define CTRL_PROF_ROCTX_RANGEPOP()      roctxRangePop();
	#else // _CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_HIP_
		#define CTRL_PROF_ROCTX_RANGEPUSH(name)
		#define CTRL_PROF_ROCTX_RANGEPOP()
	#endif // _CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_HIP_
#else // _PROFILING_ENABLED_
	#define CTRL_PROF_NVTX_RANGEPUSH(name)
	#define CTRL_PROF_ROCTX_RANGEPUSH(name)
	#define CTRL_PROF_NVTX_RANGEPOP()
	#define CTRL_PROF_ROCTX_RANGEPOP()
#endif // _PROFILING_ENABLED_


/**
 * Expands to range push operation for host marking libraries supported (nvtx and/or roctx) if compiled for profiling
 * @hideinitializer
 * @param name string the name of the range
 */
#define CTRL_PROF_RANGEPUSH(name)   \
	CTRL_PROF_NVTX_RANGEPUSH(name); \
	CTRL_PROF_ROCTX_RANGEPUSH(name);

/**
 * Expands to range pop operation for host marking libraries supported (nvtx and/or roctx) if compiled for profiling
 * @hideinitializer
 */
#define CTRL_PROF_RANGEPOP()   \
	CTRL_PROF_NVTX_RANGEPOP(); \
	CTRL_PROF_ROCTX_RANGEPOP();

#endif // _CTRL_PROFILER_HELPER_H_
