#ifndef _CTRL_HIP_H_
#define _CTRL_HIP_H_
///@cond INTERNAL
/**
 * @file Ctrl_Hip.h
 * @author Trasgo Group
 * @brief Ctrl implementation for HIP devices.
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

#include <hip/hip_runtime.h>

#include "hitmap2.h"

#include "Core/Ctrl_Info.h"
#include "Core/Ctrl_KHitTile.h"
#include "Core/Ctrl_Policy.h"
#include "Core/Ctrl_Request.h"
#include "Core/Ctrl_TaskQueue.h"
#include "Core/Ctrl_Tile.h"
#include "Core/Ctrl_Type.h"

#include "Kernel/Ctrl_ImplType.h"
#include "Kernel/Ctrl_KernelProto.h"

#include "Architectures/Hip/Ctrl_Hip_Helper.h"
#include "Architectures/Hip/Ctrl_Hip_Request.h"
#include "Architectures/Hip/Ctrl_Hip_Tile.h"

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
#define CTRL_HIP_LAUNCH(p_ctrl, name, threads, group, ...)                                                                                                                                                  \
	case CTRL_TYPE_HIP:                                                                                                                                                                                     \
		if (group.dims == 0) {                                                                                                                                                                              \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_HIP, threads, CTRL_KERNEL_HIP_CHAR_threads(name, CTRL_KERNEL_HIP_ARCH_KEPLER), 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                                                            \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_HIP, threads, group, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                                                           \
		}                                                                                                                                                                                                   \
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
#define CTRL_HIP_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, ...)                                                                                                                                        \
	case CTRL_TYPE_HIP:                                                                                                                                                                                          \
		if (group.dims == 0) {                                                                                                                                                                                   \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_HIP, threads, CTRL_KERNEL_HIP_CHAR_threads(name, CTRL_KERNEL_HIP_ARCH_KEPLER), stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                                                                 \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(CTRL_TYPE_HIP, threads, group, stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                                                           \
		}                                                                                                                                                                                                        \
		break;

/**
 * HIP implementation of abstract ctrl
 */
typedef struct Ctrl_Hip {
	int                        device;             /**< Index of the HIP gpu device used by tehe ctrl */
	hipStream_t                stream_host;        /**< Stream to launch host tasks (needed to sync host queue with other hip streams) */
	hipEvent_t                 event_seq;          /**< Event used for sync policy */
	struct Ctrl_Hip_Tile_List *p_tile_list_head;   /**< Head of the list of tiles associate to this ctrl */
	struct Ctrl_Hip_Tile_List *p_tile_list_tail;   /**< Tail of the list of tiles associate to this ctrl */
	Ctrl_Policy                policy;             /**< Policy to be used by this ctrl (sync or async) */
	int                        dependance_mode;    /**< Dependance mode to be used by this ctrl */
	int                        n_kernel_streams;   /**< Number of HIP streams for kernel launching available to this ctrl */
	int                        default_alloc_mode; /**< Default allocation mode. On HIP it will be pinned */
	hipStream_t               *kernel_streams;     /**< Streams to launch kernels */
} Ctrl_Hip;

/**
 * Create the controller and its corresponding variables.
 *
 * @param p_ctrl Controller to be created.
 * @param policy Policy for this ctrl to be used.
 * @param args Space separated string containing the params for this ctrl. Contains:
 * 		- Device: index of the device to be used.
 * 		- [OPTIONAL] Streams: number of streams to use to execute kernels. Default 1.
 */
void Ctrl_Hip_Create(Ctrl_Hip *p_ctrl, Ctrl_Policy policy, char *args);

/**
 * Evaluate a task on a HIP ctrl.
 *
 * @param p_ctrl Ctrl to execute the task.
 * @param p_task Pointer to the task to be evaluated.
 */
void Ctrl_Hip_EvalTask(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Get information of the device asociated with \p p_ctrl.
 * @param p_ctrl ctrl to get the info from.
 * @param p_info struct to store the info into.
 */
void Ctrl_Hip_GetInfo(Ctrl_Hip *p_ctrl, Ctrl_Info *p_info);
///@endcond
#endif /* _CTRL_HIP_H_ */
