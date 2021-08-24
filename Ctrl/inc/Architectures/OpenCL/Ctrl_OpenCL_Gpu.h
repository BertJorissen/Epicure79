#ifndef _CTRL_OPENCL_GPU_H_
#define _CTRL_OPENCL_GPU_H_
///@cond INTERNAL
/**
 * @file Ctrl_OpenCL_Gpu.h
 * @author Trasgo Group
 * @brief Ctrl implementation for OpenCL GPU devices.
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

#ifdef _CTRL_OPENCL_GPU_DEBUG_
	#include <stdio.h>
#endif

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include "hitmap2.h"

#include "Core/Ctrl_Policy.h"
#include "Core/Ctrl_Type.h"
#include "Core/Ctrl_TaskQueue.h"
#include "Core/Ctrl_Tile.h"
#include "Core/Ctrl_KHitTile.h"
#include "Core/Ctrl_Request.h"

#include "Kernel/Ctrl_KernelProto.h"
#include "Kernel/Ctrl_ImplType.h"

#include "Architectures/OpenCL/Ctrl_OpenCL_Request.h"
#include "Architectures/OpenCL/Ctrl_OpenCL_Helper.h"
#include "Architectures/OpenCL/Ctrl_OpenCL_Tile.h"

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
#define CTRL_OPENCL_GPU_LAUNCH( p_ctrl, name, threads, group, ... ) \
	case CTRL_TYPE_OPENCL_GPU: \
		if(group.dims == 0){ \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name( CTRL_TYPE_OPENCL_GPU, threads, local_size_OPENCL_GPU_##name, 0, CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__ ) )); \
		}else{ \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name( CTRL_TYPE_OPENCL_GPU, threads, group, 0, CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__ ) )); \
		} \
		break;

/**
 * Launch a kernel to a specific stream of the ctrl queue
 * @hideinitializer
 * 
 * @param p_ctrl: pointer to the ctrl to launch the kernel
 * @param name: name of the kernel to be launched
 * @param threads: thread block to launch the kernel with. (Ctrl_Thread)
 * @param group block sizes for this kernel execution.
 *    Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param stream: stream to launch the kernel to.
 * @param ...: arguments passed to the kernel.
 * 
 * @see Ctrl_LaunchToStream, Ctrl_Thread
 */

#define CTRL_OPENCL_GPU_LAUNCH_STREAM( p_ctrl, name, threads, group, stream, ... ) \
  case CTRL_TYPE_OPENCL_GPU: \
    if(group.dims == 0){ \
      Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name( CTRL_TYPE_OPENCL_GPU, threads, local_size_OPENCL_GPU_##name, stream, CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__ ) )); \
    }else{ \
      Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name( CTRL_TYPE_OPENCL_GPU, threads, group, stream, CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__ ) )); \
    } \
    break;

/**
 * OpenCL Gpu implementation of abstract ctrl
 */
typedef struct Ctrl_OpenCLGpu {
	cl_platform_id						platform_id;			/**< Index of the OpenCl platform to be used to create the context */
	cl_device_id						device_id;				/**< Index of the OpenCl device to be used to create the context */
	int									default_alloc_mode;		/**< Default allocation mode depending on platform, on Nvidia platforms memory will be reserved as pinned, on AMD it will be reserved normally */

	cl_context							context;				/**< OpenCL context used to create and launch everything related to OpenCL*/

	cl_command_queue_properties			queue_properties;		/**< Properties to use when creating OpenCL queues */

	struct Ctrl_OpenCL_Tile_List		*p_tile_list_head;		/**< Head of the list of tiles associate to this ctrl */
	struct Ctrl_OpenCL_Tile_List		*p_tile_list_tail;		/**< Tail of the list of tiles associate to this ctrl */

	cl_event							last_kernel_event;		/**< Event that records previous kernel operation */
	cl_event							last_host_task_event;	/**< Event that records previous host task operation */
	cl_event							default_event;			/**< Event used to create all events initially */

	#ifdef _CTRL_QUEUE_
	omp_lock_t							*p_lock_first_host;		/**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t							*p_lock_first_ctrl;		/**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t							*p_lock_host;			/**< Lock used for sync between main thread and queue manager thread */
	omp_lock_t							*p_lock_ctrl;			/**< Lock used for sync between main thread and queue manager thread */
	#endif //_CTRL_QUEUE_

	Ctrl_Policy							policy;					/**< Policy to be used by this ctrl (sync or async) */

	int									dependance_mode;		/**< Dependance mode to be used by this ctrl */

	int												n_queues;				/**< Number of OpenCl queues for kernel launching available to this cltr */
	cl_command_queue					*queues;				/**< OpenCl queues to launch kernels */
	#ifdef _CTRL_OPENCL_GPU_PROFILING_
		int							platform;
		int							device;

		cl_profiling_info 			profiling_info_start;
		cl_profiling_info 			profiling_info_end;

		bool 						has_first_profiling_event;

		cl_event 					first_profiling_event;
		cl_event 					last_profiling_event;

		cl_event 					*profiling_read_events;
		cl_event 					*profiling_write_events;
		cl_event 					*profiling_kernel_events;

		int 						i_read_task;
		int 						i_write_task;
		int 						i_kernel_task;

		cl_ulong 					profiling_total;
		cl_ulong 					profiling_sum;
		cl_ulong 					profiling_offloading;
		cl_ulong 					profiling_read;
		cl_ulong 					profiling_write;
		cl_ulong 					profiling_kernel;

		cl_ulong 					profiling_start;
		cl_ulong 					profiling_end;
		
		#ifdef _CTRL_OPENCL_GPU_PROFILING_VERBOSE_
			visual_event 			*profiling_visual_events;
			int 					i_visual_task;
		#endif
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_
} Ctrl_OpenCLGpu;

#ifdef __cplusplus
	extern "C" {
#endif
/**
 * Create the controller and its corresponding variables.
 * 
 * @param p_ctrl Controller to be created.
 * @param policy Policy to be used by the contrller.
 * @param device index of the device to be used.
 * @param platform Index of the OpenCL platform to be used.
 */
void Ctrl_OpenCLGpu_Create(Ctrl_OpenCLGpu *p_ctrl, Ctrl_Policy policy, int device, int platform, int streams);

/**
 * Evaluate a task on a OpenCL ctrl.
 * 
 * @param p_ctrl ctrl to execute the task.
 * @param p_task pointer to the task to be evaluated.
 */
void Ctrl_OpenCLGpu_EvalTask(Ctrl_OpenCLGpu *p_ctrl, Ctrl_Task *p_task);

#ifdef __cplusplus
	}
#endif
///@endcond
#endif // _CTRL_OPENCL_GPU_H_
