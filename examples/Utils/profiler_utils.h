/**
 * @file profiler_utils.h
 * @brief Helper macros for profiling marks on host tasks
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */
// clang-format off
#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	#include "nvToolsExt.h"
	#endif // _CTRL_ARCH_CUDA_

	#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_HIP_)
	#include <roctracer/roctx.h>
	#endif // _CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_HIP_
#endif //_PROFILING_ENABLED_

#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
		#define PROF_NVTX_RANGEPUSH(name) nvtxRangePushA(name);
		#define PROF_NVTX_RANGEPOP()      nvtxRangePop();
	#else // _CTRL_ARCH_CUDA_
		#define PROF_NVTX_RANGEPUSH(name)
		#define PROF_NVTX_RANGEPOP()
	#endif // _CTRL_ARCH_CUDA_

	#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_HIP_)
		#define PROF_ROCTX_RANGEPUSH(name) roctxRangePush(name);
		#define PROF_ROCTX_RANGEPOP()      roctxRangePop();
	#else // _CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_HIP_
		#define PROF_ROCTX_RANGEPUSH(name)
		#define PROF_ROCTX_RANGEPOP()
	#endif // _CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_HIP_
#else // _PROFILING_ENABLED_
	#define PROF_NVTX_RANGEPUSH(name)
	#define PROF_ROCTX_RANGEPUSH(name)
	#define PROF_NVTX_RANGEPOP()
	#define PROF_ROCTX_RANGEPOP()
#endif // _PROFILING_ENABLED_


/**
 * Expands to range push operation for host marking libraries supported (nvtx and/or roctx) if compiled for profiling
 * @hideinitializer
 * @param name string the name of the range
 */
#define PROF_RANGEPUSH(name)   \
	PROF_NVTX_RANGEPUSH(name); \
	PROF_ROCTX_RANGEPUSH(name);

/**
 * Expands to range pop operation for host marking libraries supported (nvtx and/or roctx) if compiled for profiling
 * @hideinitializer
 */
#define PROF_RANGEPOP()   \
	PROF_NVTX_RANGEPOP(); \
	PROF_ROCTX_RANGEPOP();
