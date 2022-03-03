#ifndef _CTRL_CUDA_H_
#define _CTRL_CUDA_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cuda.h
 * @author Trasgo Group
 * @brief Ctrl implementation for CUDA devices.
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

#include <omp.h>
#include <stdbool.h>

#ifdef _CTRL_CUDA_DEBUG_
#include <stdio.h>
#endif

#include <cuda_runtime.h>

#ifdef _CTRL_CUBLAS
#include <cublas_v2.h>
#endif // _CTRL_CUBLAS

#ifdef _CTRL_MAGMA_
#include "magma_lapack.h"
#include "magma_v2.h"
#endif //_CTRL_MAGMA_

#include "hitmap2.h"

#include "Core/Ctrl_KHitTile.h"
#include "Core/Ctrl_Policy.h"
#include "Core/Ctrl_Request.h"
#include "Core/Ctrl_TaskQueue.h"
#include "Core/Ctrl_Tile.h"
#include "Core/Ctrl_Type.h"

#include "Kernel/Ctrl_ImplType.h"
#include "Kernel/Ctrl_KernelProto.h"

#include "Architectures/Cuda/Ctrl_Cuda_Helper.h"
#include "Architectures/Cuda/Ctrl_Cuda_Request.h"
#include "Architectures/Cuda/Ctrl_Cuda_Tile.h"

/**
 * Launch a kernel to the ctrl queue
 * @hideinitializer
 *
 * @param p_ctrl pointer to the ctrl to launch the kernel.
 * @param name name of the kernel to be launched.
 * @param threads thread block to launch the kernel with. (Ctrl_Thread).
 * @param group block sizes for this kernel execution.
 *      Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param ... arguments passed to the kernel.
 *
 * @see Ctrl_Launch, Ctrl_Thread
 */
#define CTRL_CUDA_LAUNCH(p_ctrl, name, threads, group, ...)                                                                                                                                                    \
	case CTRL_TYPE_CUDA:                                                                                                                                                                                       \
		if (group.dims == 0) {                                                                                                                                                                                 \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_CUDA, threads, CTRL_KERNEL_CUDA_CHAR_threads(name, CTRL_KERNEL_CUDA_ARCH_KEPLER), 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                                                               \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_CUDA, threads, group, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                                                             \
		}                                                                                                                                                                                                      \
		break;

/**
 * Launch a kernel to a specific stream of the ctrl queue
 * @hideinitializer
 *
 * @param p_ctrl: pointer to the ctrl to launch the kernel
 * @param name: name of the kernel to be launched
 * @param threads: thread block to launch the kernel with. (Ctrl_Thread)
 * @param group block sizes for this kernel execution.
 *      Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param stream: stream to launch the kernel to.
 * @param ...: arguments passed to the kernel.
 *
 * @see Ctrl_LaunchToStream, Ctrl_Thread
 */
#define CTRL_CUDA_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, ...)                                                                                                                                          \
	case CTRL_TYPE_CUDA:                                                                                                                                                                                            \
		if (group.dims == 0) {                                                                                                                                                                                      \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_CUDA, threads, CTRL_KERNEL_CUDA_CHAR_threads(name, CTRL_KERNEL_CUDA_ARCH_KEPLER), stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                                                                    \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_CUDA, threads, group, stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                                                             \
		}                                                                                                                                                                                                           \
		break;

/**
 * CUDA implementation of abstract ctrl
 */
typedef struct Ctrl_Cuda {
	int                         device;           /**< Index of the CUDA gpu device used by tehe ctrl */
	cudaStream_t                stream_host;      /**< Stream to launch host tasks (needed to sync host queue with other cuda streams) */
	cudaEvent_t                 event_seq;        /**< Event used for sync policy */
	struct Ctrl_Cuda_Tile_List *p_tile_list_head; /**< Head of the list of tiles associate to this ctrl */
	struct Ctrl_Cuda_Tile_List *p_tile_list_tail; /**< Tail of the list of tiles associate to this ctrl */

	#ifdef _CTRL_QUEUE_
	omp_lock_t *p_lock_first_host; /**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t *p_lock_first_ctrl; /**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t *p_lock_host;       /**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t *p_lock_ctrl;       /**< Lock used for sync between main thread and queue manager thread */
	#endif //_CTRL_QUEUE_

	#ifdef _CTRL_CUBLAS_
	cublasHandle_t cublas_handle; /**< Handle for cublas lib operations */
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	magma_queue_t magma_queue; /**< Queue for magma lib operations */
	#endif // _CTRL_MAGMA_

	Ctrl_Policy   policy;             /**< Policy to be used by this ctrl (sync or async) */
	int           dependance_mode;    /**< Dependance mode to be used by this ctrl */
	int           n_kernel_streams;   /**< Number of CUDA streams for kernel launching available to this ctrl */
	int           default_alloc_mode; /**< Default allocation mode. On CUDA it will be pinned */
	cudaStream_t *kernel_streams;     /**< Streams to launch kernels */
} Ctrl_Cuda;

/**
 * Create the controller and its corresponding variables.
 *
 * @param p_ctrl Controller to be created.
 * @param policy Policy to be used by the contrller.
 * @param device Index of the device to be used.
 */
void Ctrl_Cuda_Create(Ctrl_Cuda *p_ctrl, Ctrl_Policy policy, int device, int streams);

/**
 * Evaluate a task on a CPU ctrl.
 *
 * @param p_ctrl Ctrl to execute the task.
 * @param p_task Pointer to the task to be evaluated.
 */
void Ctrl_Cuda_EvalTask(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Set cuda to use the device in \e p_ctrl. Calls to \e cudaSetDevice.
 *
 * @param p_ctrl Ctrl we want to set device of.
 */
void Ctrl_Cuda_NewThreadSetup(Ctrl_Cuda *p_ctrl);
///@endcond
#endif /* _CTRL_CUDA_H_ */
