#ifndef _CTRL_FPGA_H_
#define _CTRL_FPGA_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA.h
 * @brief Ctrl implementation for FPGA devices.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <omp.h>
#include <stdbool.h>

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include "hitmap2.h"

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

#include "Architectures/FPGA/Ctrl_FPGA_Helper.h"
#include "Architectures/FPGA/Ctrl_FPGA_Request.h"

#define FPGA_EMULATION 1
#define FPGA_PROFILING 2

#define AOCL_ALIGNMENT 64

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
#define CTRL_FPGA_LAUNCH(p_ctrl, name, threads, group, ...)                                                                                                 \
	case CTRL_TYPE_FPGA:                                                                                                                                    \
		if (group.dims == 0) {                                                                                                                              \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, local_size_FPGA_##name, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                            \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, group, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                  \
		}                                                                                                                                                   \
		break;

/**
 * Launch a kernel to a specific stream of the ctrl queue
 * @hideinitializer
 *
 * @param p_ctrl: pointer to the ctrl to launch the kernel
 * @param name: name of the kernel to be launched
 * @param threads: thread block to launch the kernel with. (Ctrl_Thread)
 * @param group block sizes for this kernel execution
 * 		Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param stream: stream to launch the kernel to.
 * @param ...: arguments passed to the kernel.
 *
 * @see Ctrl_Launch, Ctrl_Thread
 */
#define CTRL_FPGA_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, ...)                                                                                       \
	case CTRL_TYPE_FPGA:                                                                                                                                         \
		if (group.dims == 0) {                                                                                                                                   \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, local_size_FPGA_##name, stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                 \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, group, stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                  \
		}                                                                                                                                                        \
		break;

/**
 * FPGA implementation of abstract ctrl
 */
typedef struct Ctrl_FPGA {
	int                         global_id;               /**< Id of this ctrl with respect to other ctrls */
	int                         type_id;                 /**< Id of this ctrl with respect to other OpenCLGPU ctrls */
	cl_platform_id              platform_id;             /**< Index of the OpenCL platform to be used to create the context */
	cl_device_id                device_id;               /**< Index of the OpenCL device to be used to create the context */
	Ctrl_GenericEvent           host_seq_event;          /**< Host event used for sync policy */
	Ctrl_GenericEvent           dev_seq_event;           /**< Device event used for sync policy */
	cl_event                    default_event;           /**< Event used to create all events initially */
	cl_context                  context;                 /**< OpenCL context used to create and launch everything related to OpenCL*/
	cl_command_queue_properties queue_properties;        /**< Properties to use when creating OpenCL queues */
	struct Ctrl_Tile_List      *p_tile_list_head;        /**< Head of the list of tiles associate to this ctrl */
	struct Ctrl_Tile_List      *p_tile_list_tail;        /**< Tail of the list of tiles associate to this ctrl */
	Ctrl_Policy                 policy;                  /**< Policy to be used by this ctrl (sync or async) */
	int                         dependance_mode;         /**< Dependance mode to be used by this ctrl */
	int                         n_kernel_streams;        /**< Number of OpenCL queues for kernel launching available to this cltr */
	cl_command_queue           *p_kernel_driver_streams; /**< OpenCL queues to launch kernels */
	Ctrl_TaskQueue            **pp_kernel_host_streams;  /**< Host queues for kernels */
	cl_command_queue            htd_driver_stream;       /**< OpenCL queue for HTD memory transfers */
	Ctrl_TaskQueue             *p_htd_host_stream;       /**< Host queue for HTD memory transfers */
	cl_command_queue            dth_driver_stream;       /**< OpenCL queue for DTH memory transfers */
	Ctrl_TaskQueue             *p_dth_host_stream;       /**< Host queue for DTH memory transfers */
	int                         exec_mode;               /**< Execution mode */

	#ifdef _CTRL_OPENCL_GPU_PROFILING_
	int platform;
	int device;

	cl_profiling_info profiling_info_start;
	cl_profiling_info profiling_info_end;

	bool has_first_profiling_event;

	cl_event first_profiling_event;
	cl_event last_profiling_event;

	cl_event *profiling_read_events;
	cl_event *profiling_write_events;
	cl_event *profiling_kernel_events;

	int i_read_task;
	int i_write_task;
	int i_kernel_task;

	cl_ulong profiling_total;
	cl_ulong profiling_sum;
	cl_ulong profiling_offloading;
	cl_ulong profiling_read;
	cl_ulong profiling_write;
	cl_ulong profiling_kernel;

	cl_ulong profiling_start;
	cl_ulong profiling_end;

	#ifdef _CTRL_OPENCL_GPU_PROFILING_VERBOSE_
	visual_event *profiling_visual_events;
	int           i_visual_task;
	#endif // _CTRL_OPENCL_GPU_PROFILING_VERBOSE_
	#endif // _CTRL_EXAMPLES_OPENCL_GPU_PROFILING_
} Ctrl_FPGA;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Create the controller and its corresponding variables.
 *
 * @param p_ctrl Controller to be created.
 * @param policy Policy for this ctrl to be used.
 * @param args Space separated string containing the params for this ctrl. Contains:
 * 		- Platform: index of the OpenCL platform to be used.
 * 		- Device: index of the device to be used.
 * 		- Exec mode: execution mode.
 * 		- [OPTIONAL] Streams: number of OpenCL queues to use to execute kernels. Default 1.
 */
void Ctrl_FPGA_Create(Ctrl_FPGA *p_ctrl, Ctrl_Policy policy, char *args);

/**
 * Evaluate a task on a FPGA ctrl.
 *
 * @param p_ctrl ctrl to execute the task.
 * @param p_task pointer to the task to be evaluated.
 */
void Ctrl_FPGA_EvalTask(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Allocate memory for \e cl_program and \e cl_kernel objects for all kernels defined for FPGA.
 *
 * @param n_fpga_ctrls Number of FPGA ctrls to be created.
 */
void Ctrl_FPGA_AllocKernel(int n_fpga_ctrls);

/**
 * @brief Execute a FPGA task.
 *
 * Enqueue a ready to execute operation to the appropiate OpenCL queue
 *
 * @param p_task Task to execute
 * @param p_ctrl Ctrl responsible for the task
 */
void Ctrl_FPGA_ExecTask(Ctrl_Task *p_task, Ctrl_FPGA *p_ctrl);

/**
 * Get the number of host queues used by \p p_ctrl.
 *
 * @param p_ctrl Ctrl to get the number of queues from
 * @return number of host queues used by \p p_ctrl
 */
int Ctrl_FPGA_GetNumQueues(Ctrl_FPGA *p_ctrl);

/**
 * Get the pointers to the host queues used by \p p_ctrl on list \p pp_queues
 *
 * @param p_ctrl Ctrl to get the queues from.
 * @param pp_queues [out] Pointer to pointers to the host queues used by this ctrl
 * @return Pointer right after the queue pointers stored on \p pp_queues
 *
 * @pre \p pp_queues must have enough memory allocated to store all queues from this ctrl.
 * @see Ctrl_FPGA_GetNumQueues
 */
Ctrl_TaskQueue **Ctrl_FPGA_GetHostQueues(Ctrl_FPGA *p_ctrl, Ctrl_TaskQueue **pp_queues);

/**
 * Enqueue appropiate wait operations for a tile in a host task.
 *
 * @param p_tile tile of the host task
 * @param rol rol of \p p_tile
 * @param p_queue queue to send the wait to.
 */
void Ctrl_FPGA_HostTaskWait(Ctrl_FPGA_Tile *p_tile, char rol, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for seq event on \p p_queue
 *
 * @param p_ctrl ctrl containing the event.
 * @param p_queue queue to send the wait to.
 */
void Ctrl_FPGA_SyncWait(Ctrl_FPGA *p_ctrl, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for appropiate events from \p p_tile_data for a MoveTo operation in \p p_queue.
 *
 * @param p_tile_data metadata of tile to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_FPGA_MoveToWait(Ctrl_FPGA_Tile *p_tile_data, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for appropiate events from \p p_tile_data for a MoveFrom operation in \p p_queue.
 *
 * @param p_tile_data metadata of tile to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_FPGA_MoveFromWait(Ctrl_FPGA_Tile *p_tile_data, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue memory transfer from device to host task on host queue.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the tile to be moved.
 *
 * @see Ctrl_FPGA_EvalTaskMoveFrom
 */
void Ctrl_FPGA_EvalTaskMoveFromInner(Ctrl_FPGA *p_ctrl, HitTile *p_tile);

/**
 * Get information of the device asociated with \p p_ctrl.
 * @param p_ctrl ctrl to get the info from.
 * @param p_info struct to store the info into.
 */
void Ctrl_FPGA_GetInfo(Ctrl_FPGA *p_ctrl, Ctrl_Info *p_info);

/**
 * Create a new texture object asociated with \p p_tile and \p p_ctrl
 *
 * @param p_ctrl
 * @param p_tile
 * @param tex_desc Confguration for the texture object
 *
 * @note NOT IMPLEMENTED
 */
void Ctrl_FPGA_CreateTex(Ctrl_FPGA *p_ctrl, HitTile *p_tile, Ctrl_TexDesc tex_desc);

#ifdef __cplusplus
}
#endif
///@endcond
#endif // _CTRL_FPGA_H_
