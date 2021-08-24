#ifndef _CTRL_CORE_TASKQUEUE_H_
#define _CTRL_CORE_TASKQUEUE_H_
///@cond INTERNAL
/**
 * @file Ctrl_TaskQueue.h
 * @author Trasgo Group
 * @brief Ctrl queues, tasks and events.
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define	CTRL_TASKQUEUE_SIZE	250000

#include "Core/Ctrl_Request.h"

#include "Ctrl_Type.h"
#include "Kernel/Ctrl_Thread.h"

#ifdef _CTRL_ARCH_CUDA_
#include <cuda_runtime_api.h>
#endif //_CTRL_ARCH_CUDA_

#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
#include <CL/cl.h>
#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
/**
 * Event for a cpu stream.
 * @see Ctrl_GenericEvent, Ctrl_CpuEvent_Create, Ctrl_CpuEvent_Destroy, Ctrl_CpuEvent_Wait, Ctrl_CpuEvent_Record
 */
typedef struct Ctrl_CpuEvent{
	struct Ctrl_TaskQueue *stream;
	int task;
}Ctrl_CpuEvent;

/**
 * User Event, similar to opencl user events.
 * @see Ctrl_GenericEvent, Ctrl_CpuUserEvent_Create, Ctrl_CpuUserEvent_Destroy, Ctrl_CpuUserEvent_Wait, Ctrl_CpuUserEvent_Signal
 */
typedef struct Ctrl_CpuUserEvent{
	bool *state;
}Ctrl_CpuUserEvent;

/**
 * Types of events ef a generic event
 */
typedef enum Ctrl_EventType{
	CTRL_EVENT_TYPE_NULL,
	CTRL_EVENT_TYPE_CUDA,
	CTRL_EVENT_TYPE_OPENCL,
	CTRL_EVENT_TYPE_CPU,
	CTRL_EVENT_TYPE_USERCPU,
} Ctrl_EventType;

/**
 * Generic event that contains either \e CpuEvent, \e CpuUserEvent, \e cudaEvent_t or \e cl_event.
 * @see Ctrl_GenericEvent_StreamWait, Ctrl_GenericEvent_StreamSignal, Ctrl_GenericEvent_StreamRelease
 * @see Ctrl_GenericEvent_Wait, Ctrl_GenericEvent_Signal, Ctrl_GenericEvent_Release
 */
typedef struct Ctrl_GenericEvent{
    Ctrl_EventType event_type;
    union event {
        Ctrl_CpuEvent event_cpu;
        Ctrl_CpuUserEvent user_event_cpu;
        #ifdef _CTRL_ARCH_CUDA_
        cudaEvent_t event_cuda;
        #endif //_CTRL_ARCH_CUDA_

        #if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
        cl_event event_cl;
        #endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
   }event;
} Ctrl_GenericEvent;

/**
 * Types of tasks
 */
typedef enum Ctrl_TaskType{
	CTRL_TASK_TYPE_NULL,
	CTRL_TASK_TYPE_KERNEL,
	CTRL_TASK_TYPE_HOST,
	CTRL_TASK_TYPE_DESTROYCNTRL,
	CTRL_TASK_TYPE_GLOBALSYNC,
	CTRL_TASK_TYPE_ALLOCTILE,
	CTRL_TASK_TYPE_SELECTTILE,
	CTRL_TASK_TYPE_DOMAINTILE,
	CTRL_TASK_TYPE_FREETILE,
	CTRL_TASK_TYPE_MOVETO,
	CTRL_TASK_TYPE_MOVEFROM,
	CTRL_TASK_TYPE_WAITTILE,
	CTRL_TASK_TYPE_WAITEVENT,
	CTRL_TASK_TYPE_SIGNALEVENT,
	CTRL_TASK_TYPE_RELEASEEVENT,
	CTRL_TASK_TYPE_SETDEPENDANCEMODE,
} Ctrl_TaskType;

#define CTRL_MEM_ALLOC_BOTH 1
#define CTRL_MEM_ALLOC_HOST 2
#define CTRL_MEM_ALLOC_DEV 4

#define CTRL_MEM_PINNED 8
#define CTRL_MEM_NOPINNED 16

#define CTRL_MODE_IMPLICIT 0
#define CTRL_MODE_EXPLICIT 1

#define CTRL_SELECT_DEFAULT 0 
#define CTRL_SELECT_TILE_COORD 0
#define CTRL_SELECT_ARR_COORD 1
#define CTRL_SELECT_BOUND 0
#define CTRL_SELECT_NO_BOUND 2
#define CTRL_SELECT_INIT 0
#define CTRL_SELECT_NO_INIT 4

#define CTRL_TASK_NAME_MAX_LEN 29

/**
 * Tasks to send to \e Ctrl_TaskQueue
 */
typedef struct Ctrl_Task {
    int device_id;										/**< The device id inside the Ctrl */
    Ctrl_TaskType task_type;							/**< A task label (Future optimization: Reuse predefined tasks) */
    void (*pfn_kernel_wrapper)
			(
				Ctrl_Request request, int impl, Ctrl_Type ctrl_type,
				Ctrl_Thread threads, Ctrl_Thread blocksize,
				void *p_arguments
			);                                          /**< Kernel launching wrapper pointer */
	void (*pfn_hostTask_wrapper)(void *p_arguments);    /**< Host Task launching wrapper pointer */
	int n_arguments;                                    /**< Number of arguments/roles/pointers */
	void *p_arguments;                                  /**< Packed list of arguments */
	char *p_roles;                                      /**< Input/Output roles, for memory optimizations */
	void **pp_pointers;                                 /**< Pointers to the original variables, for memory basic operations */
	uint16_t *p_displacements;                          /**< Displacement of parameter over arguments array, for memory basic operations */
	struct Ctrl_Task *p_next;                           /**< Next task in the queue */
	Ctrl_Thread threads;                                /**< Index domain where the task is executed */
	Ctrl_Thread blocksize;                              /**< Block size for this task */
	HitTile *p_tile;                                    /**< For 1 tile tasks */
	Ctrl_GenericEvent event;                            /**< For event related tasks */
	int flags;                                          /**< For tile allocation and dependance mode */
	int stream;                                         /**< Stream to execute the task in (in kernel execution tasks) */
	Ctrl_Request request;                               /**< For kernel execution tasks */
	Ctrl_Type ctrl_type;                                /**< For kernel execution tasks, the type of the ctrl launching the kernel */
} Ctrl_Task;

/**
 * Queue for \e Ctrl_Task
 */
typedef struct Ctrl_TaskQueue {
	int read;				/**< Index of next task to execute */
	int write;				/**< Index of next free spot */
	int last_finished;		/**< Index of last finished task */
	Ctrl_Task buffer[CTRL_TASKQUEUE_SIZE];	/**< Buffer of tasks */
} Ctrl_TaskQueue;

#define CTRL_DEVICE_ALL -1

/**
 * Null value for \e Ctrl_GenericEvent
 * @hideinitializer
 */
#define CTRL_GENERIC_EVENT_NULL (Ctrl_GenericEvent){.event_type=CTRL_EVENT_TYPE_NULL}

/**
 * Null value for \e Ctrl_task
 * @hideinitializer
 */
#define CTRL_TASK_NULL                                                  \
	{                                                                   \
		.device_id = 0, .task_type = CTRL_TASK_TYPE_NULL,               \
		.pfn_kernel_wrapper = NULL, .pfn_hostTask_wrapper = NULL,       \
		.n_arguments = 0, .p_arguments = NULL, .p_roles = NULL,         \
		.pp_pointers = NULL, .p_displacements = NULL, .p_next = NULL,   \
		.threads = CTRL_THREAD_NULL, .blocksize = CTRL_THREAD_NULL,     \
		.p_tile = NULL, .event=CTRL_GENERIC_EVENT_NULL, .flags = 0,     \
		.stream = 0                                                     \
	}

/**
 * Free \p p_task.
 * 
 * @param p_task task to be free'd.
 */
static inline void Ctrl_TaskQueue_FreeTask(Ctrl_Task *p_task) {
    p_task->device_id = 0;
    p_task->pfn_kernel_wrapper = NULL;
    p_task->task_type = CTRL_TASK_TYPE_NULL;
    p_task->pfn_hostTask_wrapper = NULL;
    p_task->n_arguments = 0;
    if ((p_task->p_arguments) != NULL) free(p_task->p_arguments);
    if ((p_task->p_roles) != NULL) free(p_task->p_roles);
    if ((p_task->pp_pointers) != NULL) free(p_task->pp_pointers);
    if ((p_task->p_displacements) != NULL) free(p_task->p_displacements);
    p_task->p_next = NULL;
    p_task->p_tile = NULL;
    p_task->event = CTRL_GENERIC_EVENT_NULL;
    p_task->flags = 0;
    p_task->stream = 0;
}

/**
 * Initize queue's indexes to it's starting values.
 * 
 * @param p_queue pointer to queue to initialize.
 */
static inline void Ctrl_TaskQueue_Init(Ctrl_TaskQueue *p_queue) {
	p_queue->read = p_queue->write = 0;
	p_queue->last_finished=-1;
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
	if ( p_queue->write >= CTRL_TASKQUEUE_SIZE ) {
		fprintf(stderr, "CTRL Internal error: Task queue exhausted (see CTRL_TASKQUEUE_SIZE compilation parameter)\n"); fflush(stderr);
		exit( EXIT_FAILURE );
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

	while(write == p_queue->read) {
		#pragma omp atomic read
		write = p_queue->write;
	}

	p_queue->read++;
	return &(p_queue->buffer[(p_queue->read) - 1]);
}

/**
 * Destroys \p p_queue by putting all indexes to 0 and cleaning all remaining tasks and freeing memory.
 * 
 * @param p_queue Queue to be destroyed.
 */
static inline void Ctrl_TaskQueue_Destroy(Ctrl_TaskQueue *p_queue) {
	for (int i = 0; i < p_queue->read; i++){	
		if ((p_queue->buffer[i]).task_type == CTRL_TASK_TYPE_HOST || (p_queue->buffer[i]).task_type == CTRL_TASK_TYPE_KERNEL) {
			Ctrl_TaskQueue_FreeTask(&(p_queue->buffer[i]));
		}
	}
	p_queue->read = p_queue->write = p_queue->last_finished=0;
}

/**
 * @returns a new cpu event
 */
static inline Ctrl_CpuEvent Ctrl_CpuEvent_Create(){
	return (Ctrl_CpuEvent) {.stream=NULL, .task=0};
}

/**
 * @returns a new cpu user event
 */
static inline Ctrl_CpuUserEvent Ctrl_CpuUserEvent_Create(){
	bool *state=(bool*)malloc(sizeof(bool));
	*state=0;
	return (Ctrl_CpuUserEvent) {.state=state};
}

/**
 * Destroys \p p_event.
 * 
 * @param p_event Event to be destroyed.
 */
static inline void Ctrl_CpuEvent_Destroy(Ctrl_CpuEvent *p_event){
	p_event->stream=NULL;
	p_event->task=0;
}

/**
 * Destroys \p p_event.
 * 
 * @param p_event User event to be destroyed.
 */
static inline void Ctrl_CpuUserEvent_Destroy(Ctrl_CpuUserEvent *p_event){
	free(p_event->state);
}

/**
 * Captures in \p p_event the contents of \p p_stream at the time of this call.
 * 
 * @param p_event Event to record.
 * @param p_stream Stream in which to record event.
 */
static inline void Ctrl_CpuEvent_Record(Ctrl_CpuEvent *p_event,Ctrl_TaskQueue *p_stream ){
	p_event->stream=p_stream;
	int write=0;
	#pragma omp atomic read
		write=p_stream->write;
	p_event->task=write-1;
}

/**
 * Blocks execution by polling until the task recorded on \p event is executed.
 * 
 * @param event Event to wait to.
 */
static inline void Ctrl_CpuUserEvent_Wait(Ctrl_CpuUserEvent event){
	int state=0;
	do{
	#pragma omp atomic read
		state=*event.state;
	} while (!state);
}

/**
 * Blocks execution by polling until \p event is signaled.
 * 
 * @param event Event to wait to.
 * 
 * @see Ctrl_CpuUserEvent_Signal
 */
static inline void Ctrl_CpuEvent_Wait(Ctrl_CpuEvent event){
	if(event.stream==NULL)
		return;
	int last_finished=-1;
	do{
		#pragma omp atomic read
			last_finished=event.stream->last_finished;
	} while (last_finished<event.task);
}

/**
 * Signals \p p_event.
 * 
 * @param p_event Event to signal.
 */
static inline void Ctrl_CpuUserEvent_Signal(Ctrl_CpuUserEvent *p_event){
	#pragma omp atomic write
		*p_event->state=1;
}

/**
 * Wait for all the work in \p p_queue to finish.
 * 
 * @param p_queue Queue to wait to.
 */
static inline void Ctrl_TaskQueue_Synchronize(Ctrl_TaskQueue *p_queue){
	Ctrl_CpuEvent event=Ctrl_CpuEvent_Create();
	Ctrl_CpuEvent_Record(&event, p_queue);
	Ctrl_CpuEvent_Wait(event);
}

/**
 * Enqueues a wait operation for \p event on \p p_stream.
 * 
 * @param event Event to wait to.
 * @param p_stream Stream to enqueue the wait into.
 * 
 * @see Ctrl_GenericEvent_Wait
 */
static inline void Ctrl_GenericEvent_StreamWait(Ctrl_GenericEvent event, Ctrl_TaskQueue *p_stream){
	Ctrl_Task task=CTRL_TASK_NULL;
	task.task_type=CTRL_TASK_TYPE_WAITEVENT;
	task.event=event;

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
static inline void Ctrl_GenericEvent_StreamSignal(Ctrl_GenericEvent event, Ctrl_TaskQueue *p_stream){
	Ctrl_Task task=CTRL_TASK_NULL;
	task.task_type=CTRL_TASK_TYPE_SIGNALEVENT;
	task.event=event;

	Ctrl_TaskQueue_Push(p_stream, task);
}

/**
 * Enqueues a release operation for \p event on \p p_stream.
 * 
 * @param event Event to be released.
 * @param p_stream Stream to enqueue the operation into.
 * 
 * @pre event must be of a type capable of being released
 * @see Ctrl_GenericEvent_Release
 */
static inline void Ctrl_GenericEvent_StreamRelease(Ctrl_GenericEvent event, Ctrl_TaskQueue *p_stream){
	Ctrl_Task task=CTRL_TASK_NULL;
	task.task_type=CTRL_TASK_TYPE_RELEASEEVENT;
	task.event=event;

	Ctrl_TaskQueue_Push(p_stream, task);
}

/**
 * Waits on \p event, the type of wait depends on the type of the event.
 * 
 * @param event Event to wait to.
 * @see Ctrl_GenericEvent_StreamWait, Ctrl_CpuUserEvent_Wait, Ctrl_CpuEvent_Wait
 */
static inline void Ctrl_GenericEvent_Wait(Ctrl_GenericEvent event){
    switch (event.event_type){
        case CTRL_EVENT_TYPE_CPU:
            Ctrl_CpuEvent_Wait(event.event.event_cpu);
            break;

        #ifdef _CTRL_ARCH_CUDA_
        case CTRL_EVENT_TYPE_CUDA:
            cudaEventSynchronize(event.event.event_cuda);
            break;
        #endif //_CTRL_ARCH_CUDA_

        #if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
        case CTRL_EVENT_TYPE_OPENCL:
            clWaitForEvents(1,&event.event.event_cl);
            break;
        #endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_

        case CTRL_EVENT_TYPE_USERCPU:
            Ctrl_CpuUserEvent_Wait(event.event.user_event_cpu);
            break;
        default:
            printf("unknown event type\n");
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
static inline void Ctrl_GenericEvent_Signal(Ctrl_GenericEvent event){
	switch (event.event_type){
		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_EVENT_TYPE_OPENCL:
			clSetUserEventStatus(event.event.event_cl,CL_COMPLETE);
			break;
		#endif //_CTRL_ARCH_OPENCL_GPU_

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
 * Release \p event.
 * 
 * @param event Event to be released.
 * 
 * @pre event must be of a type capable of being released
 * @see Ctrl_GenericEvent_StreamRelease
 */
static inline void Ctrl_GenericEvent_Release(Ctrl_GenericEvent event){
    switch (event.event_type){
        #if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
        case CTRL_EVENT_TYPE_OPENCL:
            clReleaseEvent(event.event.event_cl);
            break;
        #endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_

        default:
            printf("unknown event type\n");
            exit(EXIT_FAILURE);
            break;
    }
}

/**
 * Pointer to stream for host task execution
 */
extern Ctrl_TaskQueue *p_ctrl_host_stream;

#ifdef _CTRL_DEBUG_
	static inline void Ctrl_TaskQueue_GetTypeName(Ctrl_Task *p_task, char *name) {
		switch(p_task->task_type){
			case CTRL_TASK_TYPE_NULL:
				strcpy(name, "CTRL_TASK_TYPE_NULL");
				break;
			case CTRL_TASK_TYPE_KERNEL:
				strcpy(name, "CTRL_TASK_TYPE_KERNEL");
				break;
			case CTRL_TASK_TYPE_HOST:
				strcpy(name, "CTRL_TASK_TYPE_HOST");
				break;
			case CTRL_TASK_TYPE_DESTROYCNTRL:
				strcpy(name, "CTRL_TASK_TYPE_DESTROYCNTRL");
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
			case CTRL_TASK_TYPE_DOMAINTILE:
				strcpy(name, "CTRL_TASK_TYPE_DOMAINTILE");
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
