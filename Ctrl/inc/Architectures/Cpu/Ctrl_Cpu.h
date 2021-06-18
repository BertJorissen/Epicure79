#ifndef _CTRL_CPU_H_
#define _CTRL_CPU_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu.h
 * @author Trasgo Group
 * @brief Ctrl implementation for cpu devices.
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

#include <hwloc.h>
#include <stdbool.h>
#include <stdio.h>
#include "hitmap2.h"

#include "Core/Ctrl_Policy.h"
#include "Core/Ctrl_Type.h"
#include "Core/Ctrl_TaskQueue.h"
#include "Core/Ctrl_Tile.h"
#include "Core/Ctrl_KHitTile.h"
#include "Core/Ctrl_Request.h"

#include "Kernel/Ctrl_KernelProto.h"
#include "Kernel/Ctrl_ImplType.h"

#include "Architectures/Cpu/Ctrl_Cpu_Tile.h"

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
#define CTRL_CPU_LAUNCH( p_ctrl, name, threads, group, ... ) \
	case CTRL_TYPE_CPU: \
		if(group.dims == 0){ \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name( CTRL_TYPE_CPU, threads, blocksize_CPU_##name, CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__) )); \
		}else{ \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name( CTRL_TYPE_CPU, threads, group, CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__) )); \
		} \
		break;

/**
 * CPU implementation of abstract ctrl
 */
typedef struct Ctrl_Cpu{
	int								n_cores;			/**< Number of threads to be used when executing kernels*/
	hwloc_topology_t				topo;				/**< Hwloc topology of the computer for binding threads and memory to specific cores and NUMA nodes */
	hwloc_cpuset_t					device_cpuset;		/**< Cpuset representing NUMA nodes used as a device by this ctrl */

	bool							mem_moves;			/**< Flag to determine if memory transfers between host ad device should be made or not */

	struct Ctrl_Cpu_Tile_List		*p_tile_list_head;	/**< Head of the list of tiles associate to this ctrl */
	struct Ctrl_Cpu_Tile_List		*p_tile_list_tail;	/**< Tail of the list of tiles associate to this ctrl */

	#ifdef _CTRL_QUEUE_
	omp_lock_t						*p_lock_first_host;	/**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t						*p_lock_first_ctrl;	/**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t						*p_lock_host;		/**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t						*p_lock_ctrl;		/**< Lock used for sync between main thread and queue manager thread */
	#endif //_CTRL_QUEUE_

	Ctrl_Policy						policy;				/**< Policy to be used by this ctrl (sync or async) */

	Ctrl_TaskQueue					*p_kernel_stream;	/**< CPU stream for kernel execution */
	Ctrl_TaskQueue					*p_moveTo_stream;	/**< CPU stream for host to device memory transfers */
	Ctrl_TaskQueue					*p_moveFrom_stream;	/**< CPU stream for host to device memory transfers */
	Ctrl_CpuEvent					event_seq;			/**< Event used for sync policy */

	int								dependance_mode;	/**< Dependance mode to be used by this ctrl */
} Ctrl_Cpu;


#ifdef __cplusplus
	extern "C" {
#endif
/**
 * Create the controller and its corresponding variables.
 * 
 * @param p_ctrl Controller to be created.
 * @param policy Policy to be used by the contrller.
 * @param n_cores Number of cores to be used.
 * @param p_numa_nodes Pointer to the list of NUMA nodes to be used for kernel affinity.
 * @param n_numa_nodes Number of NUMA nodes passed in \p p_numa_nodes.
 * @param mem_moves Flag to indicate if copies between device and host should be made or accesses should be zero copies.
 * 
 * @pre if \p p_numa_nodes is NULL then \p n_numa_nodes must be 0
 */
void Ctrl_Cpu_Create(Ctrl_Cpu *p_ctrl, Ctrl_Policy policy, int n_cores, int *p_numa_nodes, int n_numa_nodes, bool mem_moves);

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
#ifdef __cplusplus
	}
#endif
///@endcond

#endif /* _CTRL_CPU_H_ */
