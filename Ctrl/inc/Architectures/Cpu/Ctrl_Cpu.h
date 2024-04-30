#ifndef _CTRL_CPU_H_
#define _CTRL_CPU_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu.h
 * @author Trasgo Group
 * @brief Ctrl implementation for cpu devices.
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

#include <omp.h>

#include "hitmap2.h"
#include <hwloc.h>
#include <stdbool.h>
#include <stdio.h>

#include "Core/Ctrl_Info.h"
#include "Core/Ctrl_KHitTile.h"
#include "Core/Ctrl_Policy.h"
#include "Core/Ctrl_Request.h"
#include "Core/Ctrl_TaskQueue.h"
#include "Core/Ctrl_TexDesc.h"
#include "Core/Ctrl_Tile.h"
#include "Core/Ctrl_Type.h"

#include "Kernel/Ctrl_ImplType.h"
#include "Kernel/Ctrl_KernelProto.h"

#ifdef _CTRL_MKL_
#include "mkl.h"
#endif // _CTRL_MKL_

/**
 * Launch a kernel to the ctrl queue
 * @hideinitializer
 *
 * @param p_ctrl pointer to the ctrl to launch the kernel.
 * @param name name of the kernel to be launched.
 * @param threads thread block to launch the kernel with. (Ctrl_Thread).
 * @param group block sizes for this kernel execution.
 * 		Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param ... arguments passed to the kernel.
 *
 * @see Ctrl_Launch, Ctrl_Thread
 */
#define CTRL_CPU_LAUNCH(p_ctrl, name, threads, group, ...)                                                                                                \
	case CTRL_TYPE_CPU:                                                                                                                                   \
		if (group.dims == 0) {                                                                                                                            \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, blocksize_CPU_##name, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                          \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, group, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                \
		}                                                                                                                                                 \
		break;

/**
 * Launch a kernel to a specific stream of the ctrl queue. On CPU Ctrls \p stream is ignored as there is only one kernel queue.
 * @hideinitializer
 *
 * @param p_ctrl: pointer to the ctrl to launch the kernel
 * @param name: name of the kernel to be launched
 * @param threads: thread block to launch the kernel with. (Ctrl_Thread)
 * @param group block sizes for this kernel execution.
 *      Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param stream: stream to launch the kernel to. Ignored on CPU.
 * @param ...: arguments passed to the kernel.
 *
 * @see Ctrl_LaunchToStream, Ctrl_Thread
 */
#define CTRL_CPU_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, ...) CTRL_CPU_LAUNCH(p_ctrl, name, threads, group, __VA_ARGS__)

/**
 * CPU implementation of abstract ctrl
 */
typedef struct Ctrl_Cpu {
	int                    global_id;         /**< Id of this ctrl with respect to other ctrls */
	int                    n_cores;           /**< Number of threads to be used when executing kernels*/
	hwloc_topology_t       topo;              /**< Hwloc topology of the computer for binding threads and memory to specific cores and NUMA nodes */
	hwloc_cpuset_t         device_cpuset;     /**< Cpuset representing NUMA nodes used as a device by this ctrl */
	bool                   mem_moves;         /**< Flag to determine if memory transfers between host and device should be made or not */
	struct Ctrl_Tile_List *p_tile_list_head;  /**< Head of the list of tiles associate to this ctrl */
	struct Ctrl_Tile_List *p_tile_list_tail;  /**< Tail of the list of tiles associate to this ctrl */
	Ctrl_Policy            policy;            /**< Policy to be used by this ctrl (sync or async) */
	Ctrl_TaskQueue        *p_kernel_stream;   /**< CPU stream for kernel execution */
	Ctrl_TaskQueue        *p_moveTo_stream;   /**< CPU stream for host to device memory transfers */
	Ctrl_TaskQueue        *p_moveFrom_stream; /**< CPU stream for host to device memory transfers */
	Ctrl_GenericEvent      seq_event;         /**< Event used for sync policy */
	int                    dependance_mode;   /**< Dependance mode to be used by this ctrl */
} Ctrl_Cpu;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Create the controller and its corresponding variables.
 *
 * @param p_ctrl Controller to be created.
 * @param policy Policy for this ctrl to be used.
 * @param args Space separated string containing the params for this ctrl. Contains:
 * 		- n_cores: Number of cores to be used.
 * 		- numa_begin: Start of the range of numa nodes to be used as device. Inclusive.
 * 		- numa_end: End of the range of numa nodes to be used as device. Not inclusive.
 * 		- mem_moves: Flag to indicate if copies between device and host should be made or accesses should be zero copies.
 *
 * If range of numa nodes is empty or invalid (numa_start > numa_end), the entire machine is used.
 */
void Ctrl_Cpu_Create(Ctrl_Cpu *p_ctrl, Ctrl_Policy policy, char *args);

/**
 * Evaluate a task on a CPU ctrl.
 *
 * @param p_ctrl ctrl to execute the task.
 * @param p_task pointer to the task to be evaluated.
 */
void Ctrl_Cpu_EvalTask(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Get the number of extra threads needed by \p p_ctrl.
 *
 * @param p_ctrl Ctrl we want to get the number of extra threads of.
 *
 * @returns the number of extra threads needed by \p p_ctrl.
 */
int Ctrl_Cpu_GetNumThreads(Ctrl_Cpu *p_ctrl);

/**
 * Set affinity of threads used by \p p_ctrl and send them to therir tasks.
 * @param p_ctrl pointer to the ctrl.
 * @param topo topology of the computer.
 * @param node index of NUMA node used as host.
 */
void Ctrl_Cpu_ThreadInit(Ctrl_Cpu *p_ctrl, hwloc_topology_t topo, int node);

/**
 * Get the number of host queues used by \p p_ctrl.
 * On cpu is 0.
 *
 * @param p_ctrl Ctrl to get the number of queues from
 * @return number of host queues used by \p p_ctrl
 */
int Ctrl_Cpu_GetNumQueues(Ctrl_Cpu *p_ctrl);

/**
 * Get the pointers to the host queues used by \p p_ctrl on list \p pp_queues
 * On cpu does nothing.
 *
 * @param p_ctrl Ctrl to get the queues from.
 * @param pp_queues [out] Pointer to pointers to the host queues used by this ctrl
 * @return Pointer right after the queue pointers stored on \p pp_queues
 *
 * @pre \p pp_queues must have enough memory allocated to store all queues from this ctrl.
 * @see Ctrl_Cpu_GetNumQueues
 */
Ctrl_TaskQueue **Ctrl_Cpu_GetHostQueues(Ctrl_Cpu *p_ctrl, Ctrl_TaskQueue **pp_queues);

/**
 * Enqueue appropiate wait operations for a tile in a host task.
 *
 * @param p_tile tile of the host task
 * @param rol rol of \p p_tile
 * @param p_queue queue to send the wait to.
 */
void Ctrl_Cpu_HostTaskWait(Ctrl_Cpu_Tile *p_tile, char rol, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for seq event on \p p_queue
 *
 * @param p_ctrl ctrl containing the event.
 * @param p_queue queue to send the wait to.
 */
void Ctrl_Cpu_SyncWait(Ctrl_Cpu *p_ctrl, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for appropiate events from \p p_tile_data for a MoveTo operation in \p p_queue.
 *
 * @param p_tile_data metadata of tile to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_Cpu_MoveToWait(Ctrl_Cpu_Tile *p_tile_data, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for appropiate events from \p p_tile_data for a MoveFrom operation in \p p_queue.
 *
 * @param p_tile_data metadata of tile to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_Cpu_MoveFromWait(Ctrl_Cpu_Tile *p_tile_data, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue memory transfer from device to host task on host queue.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the tile to be moved.
 *
 * @see Ctrl_Cpu_EvalTaskMoveFrom
 */
void Ctrl_Cpu_EvalTaskMoveFromInner(Ctrl_Cpu *p_ctrl, HitTile *p_tile);

/**
 * Get information of the device asociated with \p p_ctrl.
 * @param p_ctrl ctrl to get the info from.
 * @param p_info struct to store the info into.
 */
void Ctrl_Cpu_GetInfo(Ctrl_Cpu *p_ctrl, Ctrl_Info *p_info);

/**
 * Create a new texture object asociated with \p p_tile and \p p_ctrl
 *
 * @param p_ctrl
 * @param p_tile
 * @param tex_desc Confguration for the texture object
 *
 * @note NOT IMPLEMENTED
 */
void Ctrl_Cpu_CreateTex(Ctrl_Cpu *p_ctrl, HitTile *p_tile, Ctrl_TexDesc tex_desc);
#ifdef __cplusplus
}
#endif
///@endcond

#endif /* _CTRL_CPU_H_ */
