#ifndef _CTRL_CUDA_H_
#define _CTRL_CUDA_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cuda.h
 * @brief Ctrl implementation for CUDA devices.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <omp.h>
#include <stdbool.h>

#include <cuda_runtime.h>

#ifdef _CTRL_CUBLAS_
#include <cublas_v2.h>
#endif // _CTRL_CUBLAS_

#ifdef _CTRL_MAGMA_
#include "magma_lapack.h"
#include "magma_v2.h"
#endif //_CTRL_MAGMA_

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

#include "Architectures/Cuda/Ctrl_Cuda_Helper.h"
#include "Architectures/Cuda/Ctrl_Cuda_Request.h"

// maximum number of ops queued to the same kernel stream before changing to another one
// to avoid hitting the cuda stream queue limit, if limit is still being hit due to operation complexity decrease this number
#ifndef MAX_STREAM_TASKS
#define MAX_STREAM_TASKS 1000
#endif // MAX_STREAM_TASKS

// number of extra kernel streams to be used in sequence, if all are filling up causing unwanted dependencies increase this number
#ifndef EXTRA_STREAMS
#define EXTRA_STREAMS 1
#endif // EXTRA_STREAMS

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
#define CTRL_CUDA_LAUNCH(p_ctrl, name, threads, group, ...)                                                                                                                                            \
	case CTRL_TYPE_CUDA:                                                                                                                                                                               \
		if (group.dims == 0) {                                                                                                                                                                         \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, CTRL_KERNEL_CUDA_CHAR_threads(name, CTRL_KERNEL_CUDA_ARCH_KEPLER), 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                                                       \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, group, 0, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                                                             \
		}                                                                                                                                                                                              \
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
#define CTRL_CUDA_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, ...)                                                                                                                                  \
	case CTRL_TYPE_CUDA:                                                                                                                                                                                    \
		if (group.dims == 0) {                                                                                                                                                                              \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, CTRL_KERNEL_CUDA_CHAR_threads(name, CTRL_KERNEL_CUDA_ARCH_KEPLER), stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__))); \
		} else {                                                                                                                                                                                            \
			Ctrl_LaunchKernel(p_ctrl, Ctrl_KernelTaskCreate_##name(p_ctrl, threads, group, stream, CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));                                                             \
		}                                                                                                                                                                                                   \
		break;

/**
 * CUDA implementation of abstract ctrl
 */
typedef struct Ctrl_Cuda {
	int                    global_id;        /**< Id of this ctrl with respect to other ctrls */
	int                    device;           /**< Index of the CUDA gpu device used by the ctrl */
	Ctrl_GenericEvent      host_seq_event;   /**< Host event used for sync policy */
	Ctrl_GenericEvent      dev_seq_event;    /**< Device event used for sync policy */
	struct Ctrl_Tile_List *p_tile_list_head; /**< Head of the list of tiles associated to this ctrl */
	struct Ctrl_Tile_List *p_tile_list_tail; /**< Tail of the list of tiles associated to this ctrl */

	#ifdef _CTRL_CUBLAS_
	cublasHandle_t cublas_handle; /**< Handle for cublas lib operations */
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	magma_queue_t magma_queue; /**< Queue for magma lib operations */
	#endif // _CTRL_MAGMA_

	Ctrl_Policy      policy;                  /**< Policy to be used by this ctrl (sync or async) */
	int              dependance_mode;         /**< Dependance mode to be used by this ctrl */
	int              n_kernel_streams;        /**< Number of CUDA streams for kernel launching available to this ctrl */
	int              default_alloc_mode;      /**< Default allocation mode. On CUDA it will be pinned */
	cudaStream_t    *p_kernel_driver_streams; /**< CUDA streams for kernels */
	Ctrl_TaskQueue **pp_kernel_host_streams;  /**< Host queues for kernels */
	cudaStream_t     htd_driver_stream;       /**< CUDA stream for HTD memory transfers */
	Ctrl_TaskQueue  *p_htd_host_stream;       /**< Host queue for HTD memory transfers */
	cudaStream_t     dth_driver_stream;       /**< CUDA stream for DTH memory transfers */
	Ctrl_TaskQueue  *p_dth_host_stream;       /**< Host queue for DTH memory transfers */
	int             *p_stream_op_count;       /**< Counter of the operations sent to each driver stream */
	int             *p_stream_indexes;        /**< Current stream indexes to send ops to */
} Ctrl_Cuda;

/**
 * Create the controller and its corresponding variables.
 *
 * @param p_ctrl Controller to be created.
 * @param policy Policy for this ctrl to be used.
 * @param args Space separated string containing the params for this ctrl. Contains:
 * 		- Device: index of the device to be used.
 * 		- [OPTIONAL] Streams: number of streams to use to execute kernels. Default 1.
 */
void Ctrl_Cuda_Create(Ctrl_Cuda *p_ctrl, Ctrl_Policy policy, char *args);

/**
 * Evaluate a task on a CUDA ctrl.
 *
 * @param p_ctrl Ctrl to execute the task.
 * @param p_task Pointer to the task to be evaluated.
 */
void Ctrl_Cuda_EvalTask(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * @brief Execute a cuda task.
 *
 * Enqueue a ready to execute operation to the appropiate CUDA stream
 *
 * @param p_task Task to execute
 * @param p_ctrl Ctrl responsible for the task
 */
void Ctrl_Cuda_ExecTask(Ctrl_Task *p_task, Ctrl_Cuda *p_ctrl);

/**
 * Get the number of host queues used by \p p_ctrl.
 *
 * @param p_ctrl Ctrl to get the number of queues from
 * @return number of host queues used by \p p_ctrl
 */
int Ctrl_Cuda_GetNumQueues(Ctrl_Cuda *p_ctrl);

/**
 * Get the pointers to the host queues used by \p p_ctrl on list \p pp_queues
 *
 * @param p_ctrl Ctrl to get the queues from.
 * @param pp_queues [out] Pointer to pointers to the host queues used by this ctrl
 * @return Pointer right after the queue pointers stored on \p pp_queues
 *
 * @pre \p pp_queues must have enough memory allocated to store all queues from this ctrl.
 * @see Ctrl_Cuda_GetNumQueues
 */
Ctrl_TaskQueue **Ctrl_Cuda_GetHostQueues(Ctrl_Cuda *p_ctrl, Ctrl_TaskQueue **pp_queues);

/**
 * Enqueue appropiate wait operations for a tile in a host task.
 *
 * @param p_tile tile of the host task
 * @param rol rol of \p p_tile
 * @param p_queue queue to send the wait to.
 */
void Ctrl_Cuda_HostTaskWait(Ctrl_Cuda_Tile *p_tile, char rol, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for seq event on \p p_queue
 *
 * @param p_ctrl ctrl containing the event.
 * @param p_queue queue to send the wait to.
 */
void Ctrl_Cuda_SyncWait(Ctrl_Cuda *p_ctrl, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for appropiate events from \p p_tile_data for a MoveTo operation in \p p_queue.
 *
 * @param p_tile_data metadata of tile to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_Cuda_MoveToWait(Ctrl_Cuda_Tile *p_tile_data, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue wait for appropiate events from \p p_tile_data for a MoveFrom operation in \p p_queue.
 *
 * @param p_tile_data metadata of tile to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_Cuda_MoveFromWait(Ctrl_Cuda_Tile *p_tile_data, Ctrl_TaskQueue *p_queue);

/**
 * Enqueue memory transfer from device to host task on host queue.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the tile to be moved.
 *
 * @see Ctrl_Cuda_EvalTaskMoveFrom
 */
void Ctrl_Cuda_EvalTaskMoveFromInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile);

/**
 * Get information of the device asociated with \p p_ctrl.
 * @param p_ctrl ctrl to get the info from.
 * @param p_info struct to store the info into.
 */
void Ctrl_Cuda_GetInfo(Ctrl_Cuda *p_ctrl, Ctrl_Info *p_info);

/**
 * Create a new texture object asociated with \p p_tile and \p p_ctrl
 *
 * @param p_ctrl
 * @param p_tile
 * @param tex_desc Confguration for the texture object
 *
 * @pre \p p_tile must have device memory allocated with \p p_ctrl with a suitable alignment
 * @see Ctrl_Alloc
 */
void Ctrl_Cuda_CreateTex(Ctrl_Cuda *p_ctrl, HitTile *p_tile, Ctrl_TexDesc tex_desc);

/**
 * Wrapper around cudaSetDevice(0), fixes performance issues from threads making wait and destroy operations
 * without explicitly setting a device.
 *
 * Issues likely have to do with CUDA runtime initialization stuff.
 */
void Ctrl_Cuda_SetDevice();
///@endcond
#endif /* _CTRL_CUDA_H_ */
