#ifndef _CTRL_CORE_TASKQUEUE_H_
#define _CTRL_CORE_TASKQUEUE_H_
///@cond INTERNAL
/**
 * @file Ctrl_TaskQueue.h
 * @author Trasgo Group
 * @brief Ctrl queues, tasks and events.
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CTRL_TASKQUEUE_SIZE 500000

#include "Core/Ctrl_Request.h"
#include "hitmap2.h"

#include "Ctrl_Type.h"
#include "Kernel/Ctrl_Thread.h"

#ifdef _CTRL_ARCH_CUDA_
#include "Architectures/Cuda/Ctrl_Cuda_Helper.h"
#include <cuda_runtime_api.h>
#endif //_CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_HIP_
#include <hip/hip_runtime.h>
#endif //_CTRL_ARCH_HIP_

#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
#include <CL/cl.h>
#ifdef _CTRL_ARCH_OPENCL_GPU_
#include "Architectures/OpenCL/Ctrl_OpenCL_Helper.h"
#else // _CTRL_ARCH_OPENCL_GPU_
#include "Architectures/FPGA/Ctrl_FPGA_Helper.h"
#endif // _CTRL_ARCH_OPENCL_GPU_
#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
/**
 * Event for a cpu stream.
 * @see Ctrl_GenericEvent, Ctrl_CpuEvent_Create, Ctrl_CpuEvent_Destroy, Ctrl_CpuEvent_Wait, Ctrl_CpuEvent_Record
 */
typedef struct Ctrl_CpuEvent {
	struct Ctrl_TaskQueue *stream;
	int                    task;
} Ctrl_CpuEvent;

/**
 * User Event, similar to opencl user events.
 * @see Ctrl_GenericEvent, Ctrl_CpuUserEvent_Create, Ctrl_CpuUserEvent_Destroy, Ctrl_CpuUserEvent_Wait, Ctrl_CpuUserEvent_Signal
 */
typedef struct Ctrl_CpuUserEvent {
	bool *state;
} Ctrl_CpuUserEvent;

/**
 * Types of events ef a generic event
 */
typedef enum Ctrl_EventType {
	CTRL_EVENT_TYPE_NULL,
	CTRL_EVENT_TYPE_CUDA,
	CTRL_EVENT_TYPE_HIP,
	CTRL_EVENT_TYPE_OPENCL,
	CTRL_EVENT_TYPE_CPU,
	CTRL_EVENT_TYPE_USERCPU,
} Ctrl_EventType;

/**
 * Generic event that contains either \e CpuEvent, \e CpuUserEvent, \e cudaEvent_t, \e hipEvent_t or \e cl_event.
 * @see Ctrl_GenericEvent_StreamWait, Ctrl_GenericEvent_StreamSignal
 * @see Ctrl_GenericEvent_Wait, Ctrl_GenericEvent_Signal, Ctrl_GenericEvent_Release
 */
typedef struct Ctrl_GenericEvent {
	Ctrl_EventType event_type;
	int            ctrl_id; // to check OpenCL compatibility
	int           *p_ref_count;
	union event {
		Ctrl_CpuEvent     event_cpu;
		Ctrl_CpuUserEvent user_event_cpu;

		#ifdef _CTRL_ARCH_CUDA_
		cudaEvent_t event_cuda;
		#endif //_CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		hipEvent_t event_hip;
		#endif //_CTRL_ARCH_HIP_

		#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
		cl_event *p_event_cl;
		#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
	} event;
} Ctrl_GenericEvent;

/**
 * Types of tasks
 */
typedef enum Ctrl_TaskType {
	CTRL_TASK_TYPE_NULL,
	CTRL_TASK_TYPE_KERNEL,
	CTRL_TASK_TYPE_HOST,
	CTRL_TASK_TYPE_DESTROYCTRL,
	CTRL_TASK_TYPE_GLOBALSYNC,
	CTRL_TASK_TYPE_ALLOCTILE,
	CTRL_TASK_TYPE_SELECTTILE,
	CTRL_TASK_TYPE_FREETILE,
	CTRL_TASK_TYPE_MOVETO,
	CTRL_TASK_TYPE_MOVEFROM,
	CTRL_TASK_TYPE_WAITTILE,
	CTRL_TASK_TYPE_WAITEVENT,
	CTRL_TASK_TYPE_SIGNALEVENT,
	CTRL_TASK_TYPE_SETDEPENDANCEMODE,
} Ctrl_TaskType;

#define CTRL_MEM_ALLOC_HOST 1
#define CTRL_MEM_ALLOC_DEV  2

#define CTRL_MEM_PINNED   4
#define CTRL_MEM_NOPINNED 8

#define CTRL_MEM_ALIGNED 16

#define CTRL_MODE_IMPLICIT 0
#define CTRL_MODE_EXPLICIT 1

#define CTRL_SELECT_DEFAULT    0
#define CTRL_SELECT_TILE_COORD 0
#define CTRL_SELECT_ARR_COORD  1
#define CTRL_SELECT_BOUND      0
#define CTRL_SELECT_NO_BOUND   2

#define CTRL_TASK_NAME_MAX_LEN 32

/**
 * Tasks to send to \e Ctrl_TaskQueue
 */
typedef struct Ctrl_Task {
	const char *p_func_name; /**< Pointer to string with the kernel/host-task name */
	void (*pfn_kernel_wrapper)(
		Ctrl_Request request, int impl, Ctrl_Type ctrl_type,
		Ctrl_Thread threads, Ctrl_Thread blocksize,
		void *p_arguments);                          /**< Kernel launching wrapper pointer */
	void (*pfn_hostTask_wrapper)(void *p_arguments); /**< Host Task launching wrapper pointer */
	int               device_id;                     /**< The device id inside the Ctrl */
	Ctrl_TaskType     task_type;                     /**< A task label (Future optimization: Reuse predefined tasks) */
	int               n_arguments;                   /**< Number of arguments/roles/pointers */
	void             *p_arguments;                   /**< Packed list of arguments */
	char             *p_roles;                       /**< Input/Output roles, for memory optimizations */
	void            **pp_pointers;                   /**< Pointers to the original variables, for memory basic operations */
	uint16_t         *p_displacements;               /**< Displacement of parameter over arguments array, for memory basic operations */
	Ctrl_Thread       threads;                       /**< Index domain where the task is executed */
	Ctrl_Thread       blocksize;                     /**< Block size for this task */
	HitTile          *p_tile;                        /**< For 1 tile tasks */
	HitTile           tile;                          /**< For exec moveTo and moveFrom */
	Ctrl_GenericEvent event;                         /**< For event related tasks */
	int               flags;                         /**< For tile allocation and dependance mode */
	int               stream;                        /**< Stream to execute the task in (in kernel execution tasks) */
	Ctrl_Request      request;                       /**< For kernel execution tasks */
} Ctrl_Task;

/**
 * Queue for \e Ctrl_Task
 */
typedef struct Ctrl_TaskQueue {
	int       read;                        /**< Index of next task to execute */
	int       write;                       /**< Index of next free spot */
	int       last_finished;               /**< Index of last finished task */
	Ctrl_Task buffer[CTRL_TASKQUEUE_SIZE]; /**< Buffer of tasks */
} Ctrl_TaskQueue;

#define CTRL_DEVICE_ALL -1

/**
 * Null value for \e Ctrl_GenericEvent
 * @hideinitializer
 */
#define CTRL_GENERIC_EVENT_NULL \
	(Ctrl_GenericEvent) { .event_type = CTRL_EVENT_TYPE_NULL }

/**
 * Null value for \e Ctrl_Task
 * @hideinitializer
 */
#define CTRL_TASK_NULL                                   \
	{                                                    \
		.p_func_name          = NULL,                    \
		.pfn_kernel_wrapper   = NULL,                    \
		.pfn_hostTask_wrapper = NULL,                    \
		.device_id            = 0,                       \
		.task_type            = CTRL_TASK_TYPE_NULL,     \
		.n_arguments          = 0,                       \
		.p_arguments          = NULL,                    \
		.p_roles              = NULL,                    \
		.pp_pointers          = NULL,                    \
		.p_displacements      = NULL,                    \
		.threads              = CTRL_THREAD_NULL,        \
		.blocksize            = CTRL_THREAD_NULL,        \
		.p_tile               = NULL,                    \
		.tile                 = HIT_TILE_NULL_STATIC,    \
		.event                = CTRL_GENERIC_EVENT_NULL, \
		.flags                = 0,                       \
		.stream               = 0                        \
	}

/**
 * Free \p p_task.
 *
 * @param p_task task to be free'd.
 */
static inline void Ctrl_TaskQueue_FreeTask(Ctrl_Task *p_task) {
	p_task->device_id            = 0;
	p_task->pfn_kernel_wrapper   = NULL;
	p_task->task_type            = CTRL_TASK_TYPE_NULL;
	p_task->pfn_hostTask_wrapper = NULL;
	p_task->n_arguments          = 0;
	if ((p_task->p_arguments) != NULL) free(p_task->p_arguments);
	if ((p_task->p_roles) != NULL) free(p_task->p_roles);
	if ((p_task->pp_pointers) != NULL) free(p_task->pp_pointers);
	if ((p_task->p_displacements) != NULL) free(p_task->p_displacements);
	p_task->p_tile = NULL;
	p_task->tile   = (HitTile)HIT_TILE_NULL_STATIC;
	p_task->event  = CTRL_GENERIC_EVENT_NULL;
	p_task->flags  = 0;
	p_task->stream = 0;
}

/**
 * \brief Create new task queue
 *
 * Allocates memory and initizes queue's indexes to it's starting values.
 *
 * @return Pointer to the new queue.
 */
static inline Ctrl_TaskQueue *Ctrl_TaskQueue_Create() {
	Ctrl_TaskQueue *p_queue = (Ctrl_TaskQueue *)malloc(sizeof(Ctrl_TaskQueue));
	p_queue->read = p_queue->write = 0;
	p_queue->last_finished         = -1;
	return p_queue;
}

/**
 * Push \p task to \p p_queue.
 *
 * @param p_queue Queue to push into.
 * @param task Task to be pushed.
 *
 * @pre \p p_queue must have been initialized via Ctrl_TaskQueueInit
 * @note if multiple threads try to push tasks at the same time to the same queue race conditions might occur
 */
static inline void Ctrl_TaskQueue_Push(Ctrl_TaskQueue *p_queue, Ctrl_Task task) {
	// CHECK IF THE QUEUE IS EXHAUSTED
	if (p_queue->write >= CTRL_TASKQUEUE_SIZE) {
		fprintf(stderr, "CTRL Internal error: Task queue exhausted (see CTRL_TASKQUEUE_SIZE compilation parameter)\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	p_queue->buffer[p_queue->write] = task;

	#pragma omp atomic update
	p_queue->write++;
}

/**
 * Pops the next task in \p p_queue from the \p p_queue,
 * if no tasks are available, it polls the queue until a new task is ready.
 *
 * @param p_queue Queue to extract the task from.
 *
 * @returns a pointer to the task
 */
static inline Ctrl_Task *Ctrl_TaskQueue_Pop(Ctrl_TaskQueue *p_queue) {
	int write = 0;

	#pragma omp atomic read
	write = p_queue->write;

	while (write == p_queue->read) {
		#pragma omp atomic read
		write = p_queue->write;
	}

	p_queue->read++;
	return &(p_queue->buffer[(p_queue->read) - 1]);
}

/**
 * Returns the next task in \p p_queue from the \p p_queue,
 * if no tasks are available, returns null.
 *
 * @param p_queue Queue to extract the task from.
 *
 * @returns a pointer to the task
 */
static inline Ctrl_Task *Ctrl_TaskQueue_GetNext(Ctrl_TaskQueue *p_queue) {
	int write = 0;

	#pragma omp atomic read
	write = p_queue->write;

	if (write == p_queue->read) {
		return NULL;
	}

	return &(p_queue->buffer[p_queue->read]);
}

/**
 * Destroys \p p_queue by putting all indexes to 0 and freeing memory.
 *
 * @param p_queue Queue to be destroyed.
 */
static inline void Ctrl_TaskQueue_Destroy(Ctrl_TaskQueue *p_queue) {
	for (int i = 0; i < p_queue->read; i++) {
		if ((p_queue->buffer[i]).task_type == CTRL_TASK_TYPE_HOST || (p_queue->buffer[i]).task_type == CTRL_TASK_TYPE_KERNEL) {
			Ctrl_TaskQueue_FreeTask(&(p_queue->buffer[i]));
		}
	}
	p_queue->read = p_queue->write = p_queue->last_finished = 0;
	free(p_queue);
}

/**
 * @returns a new cpu event
 */
static inline Ctrl_CpuEvent Ctrl_CpuEvent_Create() {
	return (Ctrl_CpuEvent){.stream = NULL, .task = 0};
}

/**
 * @returns a new cpu user event
 */
static inline Ctrl_CpuUserEvent Ctrl_CpuUserEvent_Create() {
	bool *state = (bool *)malloc(sizeof(bool));
	*state      = 0;
	return (Ctrl_CpuUserEvent){.state = state};
}

/**
 * Destroys \p p_event.
 *
 * @param p_event Event to be destroyed.
 */
static inline void Ctrl_CpuEvent_Destroy(Ctrl_CpuEvent *p_event) {
	p_event->stream = NULL;
	p_event->task   = 0;
}

/**
 * Destroys \p p_event.
 *
 * @param p_event User event to be destroyed.
 */
static inline void Ctrl_CpuUserEvent_Destroy(Ctrl_CpuUserEvent *p_event) {
	free(p_event->state);
}

/**
 * Captures in \p p_event the contents of \p p_stream at the time of this call.
 *
 * @param p_event Event to record.
 * @param p_stream Stream in which to record event.
 */
static inline void Ctrl_CpuEvent_Record(Ctrl_CpuEvent *p_event, Ctrl_TaskQueue *p_stream) {
	p_event->stream = p_stream;
	int write       = 0;
	#pragma omp atomic read
	write         = p_stream->write;
	p_event->task = write - 1;
}

/**
 * Blocks execution by polling until the task recorded on \p event is executed.
 *
 * @param event Event to wait to.
 */
static inline void Ctrl_CpuUserEvent_Wait(Ctrl_CpuUserEvent event) {
	int state = 0;
	do {
		#pragma omp atomic read
		state = *event.state;
	} while (!state);
}

/**
 * Query the status of an event
 *
 * @param event event to query
 *
 * @return true if completed false otherwise
 */
static inline bool Ctrl_CpuUserEvent_Test(Ctrl_CpuUserEvent event) {
	int state = 0;
	#pragma omp atomic read
	state = *event.state;
	return state;
}

/**
 * Blocks execution by polling until \p event is signaled.
 *
 * @param event Event to wait to.
 *
 * @see Ctrl_CpuUserEvent_Signal
 */
static inline void Ctrl_CpuEvent_Wait(Ctrl_CpuEvent event) {
	if (event.stream == NULL)
		return;
	int last_finished = -1;
	do {
		#pragma omp atomic read
		last_finished = event.stream->last_finished;
	} while (last_finished < event.task);
}

/**
 * Query the status of an event
 *
 * @param event event to query
 *
 * @return true if the event is completed false otherwise
 */
static inline bool Ctrl_CpuEvent_Test(Ctrl_CpuEvent event) {
	if (event.stream == NULL)
		return true;
	int last_finished = -1;
	#pragma omp atomic read
	last_finished = event.stream->last_finished;
	return last_finished >= event.task;
}

/**
 * Signals \p p_event.
 *
 * @param p_event Event to signal.
 */
static inline void Ctrl_CpuUserEvent_Signal(Ctrl_CpuUserEvent *p_event) {
	#pragma omp atomic write
	*p_event->state = 1;
}

/**
 * Wait for all the work in \p p_queue to finish.
 *
 * @param p_queue Queue to wait to.
 */
static inline void Ctrl_TaskQueue_Synchronize(Ctrl_TaskQueue *p_queue) {
	Ctrl_CpuEvent event = Ctrl_CpuEvent_Create();
	Ctrl_CpuEvent_Record(&event, p_queue);
	Ctrl_CpuEvent_Wait(event);
}

/**
 * @brief Creates a new generic event.
 *
 * Ref count is initialized to 1.
 *
 * @param type Type of the new event to be created.
 * @param ctrl_id Id of ctrl responsible of creating this event. If no ctrl is responsible it should be -1.
 * @return Ctrl_GenericEvent Newly created generic event.
 */
static inline Ctrl_GenericEvent Ctrl_GenericEvent_Create(Ctrl_EventType type, int ctrl_id) {
	Ctrl_GenericEvent event;
	event.event_type   = type;
	event.ctrl_id      = ctrl_id;
	event.p_ref_count  = (int *)malloc(sizeof(int));
	*event.p_ref_count = 1;
	switch (type) {
		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_EVENT_TYPE_CUDA:
			CUDA_OP(cudaEventCreateWithFlags(&event.event.event_cuda, cudaEventDisableTiming));
			break;
		#endif //_CTRL_ARCH_CUDA_

		#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
		case CTRL_EVENT_TYPE_OPENCL:
			event.event.p_event_cl = (cl_event *)malloc(sizeof(cl_event));
			break;
		#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
		case CTRL_EVENT_TYPE_CPU:
			event.event.event_cpu = Ctrl_CpuEvent_Create();
			break;
		case CTRL_EVENT_TYPE_USERCPU:
			event.event.user_event_cpu = Ctrl_CpuUserEvent_Create();
			break;
		default:
			printf("unknown event type\n");
			exit(EXIT_FAILURE);
			break;
	}
	return event;
}

/**
 * @brief Atomically increment reference counter of \p event
 *
 * @param event
 * @see Ctrl_GenericEvent_Release
 */
static inline void Ctrl_GenericEvent_Retain(Ctrl_GenericEvent event) {
	#pragma omp atomic update
	event.p_ref_count[0]++;
}

/**
 * @brief Atomically decrement reference counter of \p event
 *
 * If the counter reaches 0 the event is destroyed.
 *
 * @param event
 * @see Ctrl_GenericEvent_Retain Ctrl_GenericEvent_Destroy
 */
static inline void Ctrl_GenericEvent_Release(Ctrl_GenericEvent event) {
	int ref_count;

	#pragma omp atomic capture
	ref_count = --event.p_ref_count[0];

	if (ref_count == 0) {
		free(event.p_ref_count);
		switch (event.event_type) {
			#ifdef _CTRL_ARCH_CUDA_
			case CTRL_EVENT_TYPE_CUDA:
				CUDA_OP(cudaEventDestroy(event.event.event_cuda));
				break;
			#endif //_CTRL_ARCH_CUDA_

			#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
			case CTRL_EVENT_TYPE_OPENCL:
				OPENCL_ASSERT_OP(clReleaseEvent(*event.event.p_event_cl));
				free(event.event.p_event_cl);
				break;
			#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
			case CTRL_EVENT_TYPE_CPU:
				Ctrl_CpuEvent_Destroy(&event.event.event_cpu);
				break;
			case CTRL_EVENT_TYPE_USERCPU:
				Ctrl_CpuUserEvent_Destroy(&event.event.user_event_cpu);
				break;
			default:
				printf("unknown event type\n");
				exit(EXIT_FAILURE);
				break;
		}
	}
}

/**
 * Waits on \p event, the type of wait depends on the type of the event.
 *
 * @param event Event to wait to.
 * @see Ctrl_GenericEvent_StreamWait, Ctrl_CpuUserEvent_Wait, Ctrl_CpuEvent_Wait
 */
static inline void Ctrl_GenericEvent_Wait(Ctrl_GenericEvent event) {
	switch (event.event_type) {
		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_EVENT_TYPE_CUDA:
			CUDA_OP(cudaEventSynchronize(event.event.event_cuda));
			break;
		#endif //_CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_EVENT_TYPE_HIP:
			hipEventSynchronize(event.event.event_hip);
			break;
		#endif //_CTRL_ARCH_HIP_

		#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
		case CTRL_EVENT_TYPE_OPENCL:
			OPENCL_ASSERT_OP(clWaitForEvents(1, event.event.p_event_cl));
			break;
		#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
		case CTRL_EVENT_TYPE_CPU:
			Ctrl_CpuEvent_Wait(event.event.event_cpu);
			break;
		case CTRL_EVENT_TYPE_USERCPU:
			Ctrl_CpuUserEvent_Wait(event.event.user_event_cpu);
			break;
		default:
			printf("unknown event type %d\n", event.event_type);
			exit(EXIT_FAILURE);
			break;
	}
}

/**
 * Signals \p event.
 *
 * @param event event to be signaled.
 *
 * @pre event must be of a type capable of being signaled
 * @see Ctrl_GenericEvent_StreamSignal, Ctrl_CpuUserEvent_Signal
 */
static inline void Ctrl_GenericEvent_Signal(Ctrl_GenericEvent event) {
	switch (event.event_type) {
		#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
		case CTRL_EVENT_TYPE_OPENCL:
			OPENCL_ASSERT_OP(clSetUserEventStatus(*event.event.p_event_cl, CL_COMPLETE));
			break;
		#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
		case CTRL_EVENT_TYPE_USERCPU:
			Ctrl_CpuUserEvent_Signal(&event.event.user_event_cpu);
			break;
		default:
			printf("unknown event type\n");
			exit(EXIT_FAILURE);
			break;
	}
}

/**
 * Query the status of an event
 *
 * @param event event to query
 *
 * @return true if event is completed false otherwise
 */
static inline bool Ctrl_GenericEvent_Test(Ctrl_GenericEvent event) {
	switch (event.event_type) {
		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_EVENT_TYPE_CUDA:
			return cudaEventQuery(event.event.event_cuda) == cudaSuccess;
		#endif //_CTRL_ARCH_CUDA_

		#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
		case CTRL_EVENT_TYPE_OPENCL: {
			cl_int status;
			OPENCL_ASSERT_OP(clGetEventInfo(*event.event.p_event_cl, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &status, NULL));
			if (status == CL_COMPLETE) {
				// Needed because clGetEventInfo is not a syncronization point according to OpenCL spec
				OPENCL_ASSERT_OP(clWaitForEvents(1, event.event.p_event_cl));
			}

			return status == CL_COMPLETE;
		}
		#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
		case CTRL_EVENT_TYPE_CPU:
			return Ctrl_CpuEvent_Test(event.event.event_cpu);
		case CTRL_EVENT_TYPE_USERCPU:
			return Ctrl_CpuUserEvent_Test(event.event.user_event_cpu);
		default:
			printf("[Ctrl GenericEvent Test] unknown event type\n");
			exit(EXIT_FAILURE);
	}
	printf("[Ctrl GenericEvent Test] Internal error\n");
	exit(EXIT_FAILURE);
}

/**
 * Check if \p event is compatible with driver queues of \p p_ctrl
 *
 * @param event Event to check
 * @param ctrl_type Type of ctrl recieving the event
 * @param ctrl_id Id of ctrl recieving the event
 * @return If the recieving controller driver queues are natively compatible with \p event and thus it's waiting can be meved to the driver
 */
static inline bool Ctrl_Event_CheckCompat(Ctrl_GenericEvent event, Ctrl_Type ctrl_type, int ctrl_id) {
	if (event.event_type == CTRL_EVENT_TYPE_CUDA && ctrl_type == CTRL_TYPE_CUDA)
		return true;
	if (event.event_type == CTRL_EVENT_TYPE_OPENCL && (ctrl_type == CTRL_TYPE_OPENCL_GPU || ctrl_type == CTRL_TYPE_FPGA))
		return event.ctrl_id == ctrl_id;
	return false;
}

/**
 * Enqueues a wait operation for \p event on \p p_stream.
 *
 * @param event Event to wait to.
 * @param p_stream Stream to enqueue the wait into.
 *
 * @see Ctrl_GenericEvent_Wait
 */
static inline void Ctrl_GenericEvent_StreamWait(Ctrl_GenericEvent event, Ctrl_TaskQueue *p_stream) {
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_WAITEVENT;
	task.event     = event;
	Ctrl_GenericEvent_Retain(event);

	Ctrl_TaskQueue_Push(p_stream, task);
}

/**
 * Enqueues a signal operation for \p event on \p p_stream.
 *
 * @param event Event to signal.
 * @param p_stream Stream to enqueue the operation into.
 *
 * @pre event must be of a type capable of being signaled
 * @see Ctrl_GenericEvent_Signal
 */
static inline void Ctrl_GenericEvent_StreamSignal(Ctrl_GenericEvent event, Ctrl_TaskQueue *p_stream) {
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_SIGNALEVENT;
	task.event     = event;

	Ctrl_TaskQueue_Push(p_stream, task);
}

#ifdef _CTRL_DEBUG_
static inline void Ctrl_TaskQueue_GetTypeName(Ctrl_Task *p_task, char *name) {
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_NULL:
			strcpy(name, "CTRL_TASK_TYPE_NULL");
			break;
		case CTRL_TASK_TYPE_KERNEL:
			strcpy(name, "CTRL_TASK_TYPE_KERNEL");
			break;
		case CTRL_TASK_TYPE_HOST:
			strcpy(name, "CTRL_TASK_TYPE_HOST");
			break;
		case CTRL_TASK_TYPE_DESTROYCTRL:
			strcpy(name, "CTRL_TASK_TYPE_DESTROYCTRL");
			break;
		case CTRL_TASK_TYPE_GLOBALSYNC:
			strcpy(name, "CTRL_TASK_TYPE_GLOBALSYNC");
			break;
		case CTRL_TASK_TYPE_ALLOCTILE:
			strcpy(name, "CTRL_TASK_TYPE_ALLOCTILE");
			break;
		case CTRL_TASK_TYPE_SELECTTILE:
			strcpy(name, "CTRL_TASK_TYPE_SELECTTILE");
			break;
		case CTRL_TASK_TYPE_FREETILE:
			strcpy(name, "CTRL_TASK_TYPE_FREETILE");
			break;
		case CTRL_TASK_TYPE_MOVETO:
			strcpy(name, "CTRL_TASK_TYPE_MOVETO");
			break;
		case CTRL_TASK_TYPE_MOVEFROM:
			strcpy(name, "CTRL_TASK_TYPE_MOVEFROM");
			break;
		case CTRL_TASK_TYPE_WAITTILE:
			strcpy(name, "CTRL_TASK_TYPE_WAITTILE");
			break;
		case CTRL_TASK_TYPE_WAITEVENT:
			strcpy(name, "CTRL_TASK_TYPE_WAITEVENT");
			break;
		case CTRL_TASK_TYPE_SIGNALEVENT:
			strcpy(name, "CTRL_TASK_TYPE_SIGNALEVENT");
			break;
		case CTRL_TASK_TYPE_SETDEPENDANCEMODE:
			strcpy(name, "CTRL_TASK_TYPE_SETDEPENDANCEMODE");
			break;
		default:
			strcpy(name, "Unknown task type");
			break;
	}
}
#endif // _CTRL_DEBUG_

///@endcond
#endif // _CTRL_CORE_TASKQUEUE_H_
