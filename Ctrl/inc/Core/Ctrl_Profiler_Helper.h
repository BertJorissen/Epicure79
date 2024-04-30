#ifndef _CTRL_PROFILER_HELPER_H_
#define _CTRL_PROFILER_HELPER_H_
/**
 * @file Ctrl_Profiler_Helper.h
 * @author Trasgo Group
 * @brief Helper macros for profiling marks on host tasks
 * @version 4.0
 * @date 2021-07-31
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

	#ifdef _CTRL_ARCH_OPENCL_GPU_
		#include <roctx.h>
		#define CTRL_PROF_ROCTX_RANGEPUSH(name) roctxRangePush(name);
		#define CTRL_PROF_ROCTX_RANGEPOP()      roctxRangePop();
	#else // _CTRL_ARCH_OPENCL_GPU_
		#define CTRL_PROF_ROCTX_RANGEPUSH(name)
		#define CTRL_PROF_ROCTX_RANGEPOP()
	#endif // _CTRL_ARCH_OPENCL_GPU_
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
