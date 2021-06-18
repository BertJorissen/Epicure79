///@cond INTERNAL
/**
 * @file Ctrl_Cuda.c
 * @author Trasgo Group
 * @brief Source code for CUDA backend.
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

#include "Architectures/Cuda/Ctrl_Cuda.h"

/*************************************************************
 ******** Prototypes of private functions ********************
 *************************************************************/

/**
 * Sync with main thread if queues are enabled, if queues are not enabled this function does nothing.
 * 
 * @param p_ctrl Pointer to the ctrl to perform the syncronization.
 */
void Ctrl_Cuda_Sync(Ctrl_Cuda *p_ctrl);

/**
 * Wrapper to launch host tasks using \e cudaLaunchHostFunc as a workaround to signal cuda events when the host task is done
 * 
 * This function is sent to the cuda host tasks stream, then it pushes the task to the host task queue and waits until 
 * execution of the host task is completed.
 * 
 * @param p_task Task to be executed.
 */
void Ctrl_Cuda_LaunchHost(void* p_task);

/**
 * Allocate memory for a new \e Ctrl_Cuda_Tile.
 * 
 * @param p_ctrl Pointer to the ctrl to be attached to the tile.
 * @param p_task Pointer to the task containing the HitTile that will contain the new \e Ctrl_Cuda_Tile.
 * 
 * @see Ctrl_Cuda_EvalTaskAllocTile, Ctrl_Cuda_Tile
 */
void Ctrl_Cuda_CreateTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Initializate a \e Ctrl_Cuda_Tile.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile and to be updated of the initialization.
 * @param p_task Pointer to the task containing the HitTile that contains the \e Ctrl_Cuda_Tile to be.
 * initialized.
 */
void Ctrl_Cuda_InitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Perform memory transfer from host to device.
 * 
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 * 
 * @see Ctrl_Cuda_EvalTaskMoveTo
 */
void Ctrl_Cuda_EvalTaskMoveToInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile);

/**
 * Perform memory transfer from device to host.
 * 
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 * 
 * @see Ctrl_Cuda_EvalTaskMoveFrom
 */
void Ctrl_Cuda_EvalTaskMoveFromInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile);

/*************************************************************
 ******** Prototypes of tasks' evaluation functions **********
 *************************************************************/

/**
 * Destroy a CPU ctrl.
 * @param p_ctrl ctrl to be destroyed.
 */
void Ctrl_Cuda_Destroy(Ctrl_Cuda *p_ctrl);

/**
 * Evaluation of kernel launch.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_Launch
 */
void Ctrl_Cuda_EvalTaskKernelLaunch(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of host task launch.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_HostTask
 */
void Ctrl_Cuda_EvalTaskHostTaskLaunch(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of global sync. Waits for all work related to any tile attached to this ctrl.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_GlobalSync
 */
void Ctrl_Cuda_EvalTaskGlobalSync(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of creation of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cuda_EvalTask, Ctrl_Domain
 */ 
void Ctrl_Cuda_EvalTaskDomainTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of allocation of tiles.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_Alloc
 */
void Ctrl_Cuda_EvalTaskAllocTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of freeing of tiles.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_Free
 */
void Ctrl_Cuda_EvalTaskFreeTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 * 
 * This function checks state of tile and, if comunication is necesary calls to \e Ctrl_Cuda_EvalTaskMoveToInner.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_Cuda_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_Cuda_EvalTaskMoveTo(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 * 
 * This function checks state of tile and, if comunication is necesary calls to \e Ctrl_Cuda_EvalTaskMoveToInner.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_Cuda_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_Cuda_EvalTaskMoveFrom(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of wait. Waits for all the work related to the tile specified in \p p_task .
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_Cuda_EvalTask, Ctrl_WaitTile
 */
void Ctrl_Cuda_EvalTaskWaitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of change of dependance mode.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated. .
 */
void Ctrl_Cuda_EvalTaskSetDependanceMode(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/********************************************
 ******** CUDA Controller functions *********
 ********************************************/

void Ctrl_Cuda_NewThreadSetup(Ctrl_Cuda *p_ctrl) {
    CUDA_OP( cudaSetDevice(p_ctrl->device) );
}

void Ctrl_Cuda_Create(Ctrl_Cuda *p_ctrl, Ctrl_Policy policy, int device) {
	p_ctrl->policy = policy;
    p_ctrl->device = device;
	p_ctrl->dependance_mode = CTRL_MODE_IMPLICIT;
	p_ctrl->default_alloc_mode = CTRL_MEM_PINNED;

	//set the main thread to use the desired cuda device.
	CUDA_OP( cudaSetDevice(p_ctrl->device) );

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

    CUDA_OP( cudaStreamCreateWithFlags(&(p_ctrl->stream_kernel), cudaStreamNonBlocking) );
    CUDA_OP( cudaStreamCreateWithFlags(&(p_ctrl->stream_host), cudaStreamNonBlocking) );

	CUDA_OP( cudaEventCreateWithFlags(&(p_ctrl->event_seq), cudaEventDisableTiming) );
}

void Ctrl_Cuda_EvalTask(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	switch ( p_task->task_type ) {
		case CTRL_TASK_TYPE_KERNEL:
			Ctrl_Cuda_EvalTaskKernelLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_HOST:
			Ctrl_Cuda_EvalTaskHostTaskLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_GLOBALSYNC:
			Ctrl_Cuda_EvalTaskGlobalSync(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_ALLOCTILE:
			Ctrl_Cuda_EvalTaskAllocTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DOMAINTILE:
			Ctrl_Cuda_EvalTaskDomainTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_FREETILE:
			Ctrl_Cuda_EvalTaskFreeTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVETO:
			Ctrl_Cuda_EvalTaskMoveTo(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVEFROM:
			Ctrl_Cuda_EvalTaskMoveFrom(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_WAITTILE:
			Ctrl_Cuda_EvalTaskWaitTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DESTROYCNTRL:
			Ctrl_Cuda_Destroy(p_ctrl);
			break;
		case CTRL_TASK_TYPE_SETDEPENDANCEMODE:
			Ctrl_Cuda_EvalTaskSetDependanceMode(p_ctrl, p_task);
			break;
		default:
			fprintf(stderr, "[Ctrl_Cuda] Unsupported task type:%d.\n", p_task->task_type);
			exit(EXIT_FAILURE);
	}
}

/*********************************
 ******* Private functions *******
 *********************************/

void Ctrl_Cuda_LaunchHost(void* p_task){
	//push task to host queue
	Ctrl_TaskQueue_Push(p_ctrl_host_stream, *(Ctrl_Task*)p_task);
	free(p_task);

	// wait for host task execution to finish
	Ctrl_CpuEvent e=Ctrl_CpuEvent_Create();
	Ctrl_CpuEvent_Record(&e, p_ctrl_host_stream);
	Ctrl_CpuEvent_Wait(e);
}

void Ctrl_Cuda_Sync(Ctrl_Cuda *p_ctrl) {
	#ifdef _CTRL_QUEUE_
	omp_set_lock(p_ctrl->p_lock_first_ctrl);
	omp_unset_lock(p_ctrl->p_lock_ctrl);
	omp_set_lock(p_ctrl->p_lock_host);
	omp_unset_lock(p_ctrl->p_lock_first_ctrl);

	omp_set_lock(p_ctrl->p_lock_first_host);
	omp_unset_lock(p_ctrl->p_lock_host);
	omp_set_lock(p_ctrl->p_lock_ctrl);
	omp_unset_lock(p_ctrl->p_lock_first_host);
	#endif //_CTRL_QUEUE_
}

void Ctrl_Cuda_CreateTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)malloc(sizeof(Ctrl_Cuda_Tile));

	p_tile_data->p_ctrl = p_ctrl;

	p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST_DEV;
	if (p_tile->ref != NULL) ((Ctrl_Cuda_Tile *)(p_tile->ref->ext))->last_update = CTRL_TILE_LAST_UPDATE_DEV;

	p_tile->ext = (void *)p_tile_data;

	p_tile_data->is_initialized = false;
}

void Ctrl_Cuda_InitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	Ctrl_Cuda_Tile_List *p_list_node = (Ctrl_Cuda_Tile_List *)malloc(sizeof(Ctrl_Cuda_Tile_List));

	p_list_node->p_prev = NULL;
	p_list_node->p_next = NULL;
	p_list_node->p_tile_ext = p_tile_data;
	p_tile_data->p_tile_elem = p_list_node;

	if (p_ctrl->p_tile_list_tail != NULL) {
		p_ctrl->p_tile_list_tail->p_next = p_list_node;
		p_list_node->p_prev = p_ctrl->p_tile_list_tail;
		p_ctrl->p_tile_list_tail = p_list_node;
	} else {
		p_ctrl->p_tile_list_head = p_ctrl->p_tile_list_tail = p_list_node;
	}

	CUDA_OP( cudaEventCreateWithFlags(&(p_tile_data->kernel_last_read_event), cudaEventDisableTiming) );
	CUDA_OP( cudaEventCreateWithFlags(&(p_tile_data->kernel_last_write_event), cudaEventDisableTiming) );

	CUDA_OP( cudaEventCreateWithFlags(&(p_tile_data->offloading_last_read_event), cudaEventDisableTiming) );
	CUDA_OP( cudaEventCreateWithFlags(&(p_tile_data->offloading_last_write_event), cudaEventDisableTiming) );

	CUDA_OP( cudaEventCreateWithFlags(&(p_tile_data->host_last_read_event), cudaEventDisableTiming) );
	CUDA_OP( cudaEventCreateWithFlags(&(p_tile_data->host_last_write_event), cudaEventDisableTiming) );

	CUDA_OP( cudaStreamCreateWithFlags(&(p_tile_data->stream), cudaStreamNonBlocking) );

	p_tile_data->is_initialized = true;
}

void Ctrl_Cuda_EvalTaskMoveToInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile) {
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	//Wait for appropiate events
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->kernel_last_read_event, 0) );
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->kernel_last_write_event, 0) );
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->offloading_last_read_event, 0) );
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->host_last_write_event, 0) );

	//Wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_ctrl->event_seq, 0) );
	}

	/* @arturo TODO: STRIDED TILES */

	//Send memcpy to cuda stream
	/* TILES WITH THEIR OWN MEMORY ALLOCATION, OR CONTIGUOUS 1D TILES NEED ONLY ONE CONTIGUOUS COPY */
	if ((p_tile->memStatus == HIT_MS_OWNER) || (p_tile->shape.info.sig.numDims == 1)) {
		CUDA_OP( 
			cudaMemcpyAsync(
				p_tile_data->p_device_data, 
				p_tile->data, 
				((size_t)(p_tile->acumCard)) * (p_tile->baseExtent), 
				cudaMemcpyHostToDevice, 
				p_tile_data->stream
			)	
		);
	}
	/* CONTIGUOUS 2D TILES */
	else if (p_tile->shape.info.sig.numDims == 2) {
		CUDA_OP(
			cudaMemcpy2DAsync(
				p_tile_data->p_device_data,
				(p_tile->baseExtent) * p_tile->origAcumCard[1],
				p_tile->data,
				(p_tile->baseExtent) * p_tile->origAcumCard[1],
				(p_tile->baseExtent) * p_tile->card[1],
				p_tile->card[0],
				cudaMemcpyHostToDevice,
				p_tile_data->stream
			)
		);
	}
	/* CONTIGUOUS 3D TILES */
	else if (p_tile->shape.info.sig.numDims == 3) {
		struct cudaMemcpy3DParms params = { 0 };
		params.srcPtr = make_cudaPitchedPtr( 
				p_tile->data, 
				(p_tile->baseExtent) * p_tile->origAcumCard[2],
				p_tile->origAcumCard[2],
				p_tile->origAcumCard[1] / p_tile->origAcumCard[2]
				);
		params.dstPtr = make_cudaPitchedPtr( 
				p_tile_data->p_device_data,
				(p_tile->baseExtent) * p_tile->origAcumCard[2],
				p_tile->origAcumCard[2],
				p_tile->origAcumCard[1] / p_tile->origAcumCard[2]
				);
		params.extent = make_cudaExtent( p_tile->card[2] * p_tile->baseExtent, p_tile->card[1], p_tile->card[0] );
		params.kind = cudaMemcpyHostToDevice;

		CUDA_OP(
			cudaMemcpy3DAsync( &params, p_tile_data->stream )
		);
	}
	else {
		fprintf(stderr, "Internal Error: Number of dimensions not supported for non-owner tile in MoveTo: %d\n",
			p_tile->shape.info.sig.numDims);
	}

	//Update events
	CUDA_OP( cudaEventRecord(p_tile_data->offloading_last_write_event, p_tile_data->stream) );
	
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaEventRecord(p_ctrl->event_seq, p_tile_data->stream) );
	}

	//Update state of the tile
	p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST_DEV;
}

void Ctrl_Cuda_EvalTaskMoveFromInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile) {
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	//Wait for appropiate events
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->kernel_last_write_event, 0) );
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->offloading_last_write_event, 0) );
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->host_last_read_event, 0) );
	CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_tile_data->host_last_write_event, 0) );

	//wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaStreamWaitEvent(p_tile_data->stream, p_ctrl->event_seq, 0) );
	}

	/* @arturo TODO: STRIDED TILES */

	//Send memcpy to cuda stream
	/* TILES WITH THEIR OWN MEMORY ALLOCATION, OR CONTIGUOUS 1D TILES NEED ONLY ONE CONTIGUOUS COPY */
	if ((p_tile->memStatus == HIT_MS_OWNER) || (p_tile->shape.info.sig.numDims == 1)) {
		CUDA_OP( 
			cudaMemcpyAsync(
				p_tile->data, 
				p_tile_data->p_device_data, 
				((size_t)(p_tile->acumCard)) * (p_tile->baseExtent),
				cudaMemcpyDeviceToHost, 
				p_tile_data->stream
			)	
		);
	}
	/* CONTIGUOUS 2D TILES */
	else if (p_tile->shape.info.sig.numDims == 2) {
		CUDA_OP(
			cudaMemcpy2DAsync(
				p_tile->data,	// dst
				(p_tile->baseExtent) * p_tile->origAcumCard[1],	// dpitch
				p_tile_data->p_device_data,	// src
				(p_tile->baseExtent) * p_tile->origAcumCard[1],	// spitch
				(p_tile->baseExtent) * p_tile->card[1],	// width
				p_tile->card[0],	// height
				cudaMemcpyDeviceToHost,
				p_tile_data->stream
			)
		);
}
	/* CONTIGUOUS 3D TILES */
	else if (p_tile->shape.info.sig.numDims == 3) {		
		struct cudaMemcpy3DParms params = { 0 };
		params.srcPtr = make_cudaPitchedPtr( 
				p_tile_data->p_device_data,
				(p_tile->baseExtent) * p_tile->origAcumCard[2],
				p_tile->origAcumCard[2],
				p_tile->origAcumCard[1] / p_tile->origAcumCard[2]
				);
		params.dstPtr = make_cudaPitchedPtr( 
				p_tile->data, 
				(p_tile->baseExtent) * p_tile->origAcumCard[2],
				p_tile->origAcumCard[2],
				p_tile->origAcumCard[1] / p_tile->origAcumCard[2]
				);
		params.extent = make_cudaExtent( p_tile->card[2] * p_tile->baseExtent, p_tile->card[1], p_tile->card[0] );
		params.kind = cudaMemcpyDeviceToHost;

		CUDA_OP(
			cudaMemcpy3DAsync( &params, p_tile_data->stream )
		);
	}
	else {
		fprintf(stderr, "Internal Error: Number of dimensions not supported for non-owner tile in MoveFrom: %d\n",
			p_tile->shape.info.sig.numDims);
	}

	//Update events
	CUDA_OP( cudaEventRecord(p_tile_data->offloading_last_read_event, p_tile_data->stream) );

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaEventRecord(p_ctrl->event_seq, p_tile_data->stream) );
	}

	//Update state of the tile
	p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST_DEV;
}

/**********************************
 ** TASKS'S EVALUATION FUNCTIONS **
 **********************************/

void Ctrl_Cuda_Destroy(Ctrl_Cuda *p_ctrl) {
	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	CUDA_OP( cudaStreamDestroy(p_ctrl->stream_kernel) );
    CUDA_OP( cudaStreamDestroy(p_ctrl->stream_host) );

	CUDA_OP( cudaEventDestroy(p_ctrl->event_seq) );

	//send destroy task to host task stream
	Ctrl_Task task=CTRL_TASK_NULL;
	task.task_type=CTRL_TASK_TYPE_DESTROYCNTRL;
	Ctrl_TaskQueue_Push(p_ctrl_host_stream,task);
}

void Ctrl_Cuda_EvalTaskGlobalSync(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	CUDA_OP( cudaDeviceSynchronize() );
	Ctrl_Cuda_Sync(p_ctrl);
}

void Ctrl_Cuda_EvalTaskKernelLaunch(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	//wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			KHitTile *p_ktile = (KHitTile *)((uint8_t *)p_task->p_arguments + p_task->p_displacements[i]);
			Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

			if (hit_tileIsNull(*p_tile)) {
				fprintf(stderr, "Warning: Launching task, skipping null tile on parameter %d\n", i); fflush(stderr);
				continue;
			}

			//if tile's role is IN or IO and is not updated on device transfer it
			if (p_task->p_roles[i] != KERNEL_OUT && p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
				if (p_tile_data->last_update == CTRL_TILE_LAST_UPDATE_HOST) {
					Ctrl_Cuda_EvalTaskMoveToInner(p_ctrl, p_tile);
				}
			}

			p_ktile->data = p_tile_data->p_device_data;

			CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_kernel, p_tile_data->kernel_last_write_event, 0) );
			CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_kernel, p_tile_data->offloading_last_write_event, 0) );

			if (p_task->p_roles[i] != KERNEL_IN) {
				CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_kernel, p_tile_data->kernel_last_read_event, 0) );
				CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_kernel, p_tile_data->offloading_last_read_event, 0) );
			}
		}
	}

	//wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_kernel, p_ctrl->event_seq, 0) );
	}

	//create request with info for kernel execution
	Ctrl_Request request;
	request.cuda.stream = &(p_ctrl->stream_kernel);
	
	//Launch kernel to CUDA kernel stream
	p_task->pfn_kernel_wrapper(request, p_task->device_id, p_task->threads, p_task->blocksize, p_task->p_arguments);

	//update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

			if (p_task->p_roles[i] != KERNEL_IN) {
				p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_DEV;
				CUDA_OP( cudaEventRecord(p_tile_data->kernel_last_write_event, p_ctrl->stream_kernel) );
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				CUDA_OP( cudaEventRecord(p_tile_data->kernel_last_read_event, p_ctrl->stream_kernel) );
			}
		}
	}

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaEventRecord(p_ctrl->event_seq, p_ctrl->stream_kernel) );
	}
}

void Ctrl_Cuda_EvalTaskHostTaskLaunch(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	//wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

			//if tile's role is IN or IO and is not updated on host transfer it
			if (p_task->p_roles[i] != KERNEL_OUT && p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
				if (p_tile_data->last_update == CTRL_TILE_LAST_UPDATE_DEV) {
					Ctrl_Cuda_EvalTaskMoveFromInner(p_ctrl, p_tile);
				}
			}

			CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_host, p_tile_data->offloading_last_read_event, 0) );
			CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_host, p_tile_data->host_last_write_event, 0) );

			if (p_task->p_roles[i] != KERNEL_IN) {
				p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST;

				CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_host, p_tile_data->offloading_last_write_event, 0) );
				CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_host, p_tile_data->host_last_read_event, 0) );
			}

		}
	}

	//wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaStreamWaitEvent(p_ctrl->stream_host, p_ctrl->event_seq, 0) );
	}

	//Allocate memory for task, needed because task wont be copied to host queue until hostfunc executes on cuda steam
	Ctrl_Task *p_task_aux=(Ctrl_Task*)malloc(sizeof(Ctrl_Task));
	*p_task_aux=*p_task;
	//Send task to CUDA host stream, when this execs it will be pushed to host queue
	CUDA_OP( cudaLaunchHostFunc(p_ctrl->stream_host, Ctrl_Cuda_LaunchHost , p_task_aux) );

	//update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

			if (p_task->p_roles[i] != KERNEL_IN) {
				CUDA_OP( cudaEventRecord(p_tile_data->host_last_write_event, p_ctrl->stream_host) );
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				CUDA_OP( cudaEventRecord(p_tile_data->host_last_read_event, p_ctrl->stream_host) );
			}
		}
	}

	//wait for host task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaEventRecord(p_ctrl->event_seq, p_ctrl->stream_host) );
		CUDA_OP( cudaEventSynchronize(p_ctrl->event_seq) );
		Ctrl_Cuda_Sync(p_ctrl);
	}
}

void Ctrl_Cuda_EvalTaskDomainTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_Cuda_CreateTile(p_ctrl, p_task);
	Ctrl_Cuda_Sync(p_ctrl);
}

void Ctrl_Cuda_EvalTaskAllocTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	if ( ! (p_tile_data->is_initialized) ) {
		Ctrl_Cuda_InitTile(p_ctrl, p_task);
	}

	p_tile_data->mem_flags = p_task->flags;

	if ( (p_tile_data->mem_flags & CTRL_MEM_ALLOC_BOTH) || !((p_tile_data->mem_flags & CTRL_MEM_ALLOC_HOST) || (p_tile_data->mem_flags & CTRL_MEM_ALLOC_DEV) )) {
		// Allocate memory in both host and device
		CUDA_OP( cudaMalloc(&(p_tile_data->p_device_data), ((size_t)(p_tile->origAcumCard[0])) * (p_tile->baseExtent)) );
		
		if ( (p_tile_data->mem_flags & CTRL_MEM_PINNED) || (!(p_tile_data->mem_flags & CTRL_MEM_NOPINNED) && p_ctrl->default_alloc_mode == CTRL_MEM_PINNED) ) { 
			//Allocate host "pinned" memory
			CUDA_OP( cudaMallocHost(&(p_tile->data), ((size_t)(p_tile->origAcumCard[0])) * (p_tile->baseExtent)) );
		}else {
			//Allocate host memory the usual way (on CUDA this is probably never a good idea to do, but the option is there)
			p_tile->data = (void*)malloc((size_t)p_tile->acumCard * p_tile->baseExtent);
		}
		
		p_tile->memPtr = p_tile->data;
	} else if (p_tile_data->mem_flags & CTRL_MEM_ALLOC_HOST) { // Allocate memory only in host
		fprintf(stderr, "Internal Error: Not supported flag in Alloc: CTRL_MEM_ALLOC_HOST\n"); fflush(stderr); exit(EXIT_FAILURE);
	} else if (p_tile_data->mem_flags & CTRL_MEM_ALLOC_DEV) { // Allocate memory only in device
		fprintf(stderr, "Internal Error: Not supported flag in Alloc: CTRL_MEM_ALLOC_DEV\n"); fflush(stderr); exit(EXIT_FAILURE);
	}
	
	// TODO Habria que poner qstride, origAcumCard, y memStatus en p_tile_data con los dev (al tener dos espacios de memoria)

	Ctrl_Cuda_Sync(p_ctrl);
}

void Ctrl_Cuda_EvalTaskFreeTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	if (p_tile_data->is_initialized) {
		//Wait for all work related to this tile to finish 
		CUDA_OP( cudaEventSynchronize(p_tile_data->kernel_last_read_event) );
		CUDA_OP( cudaEventSynchronize(p_tile_data->kernel_last_write_event) );

		CUDA_OP( cudaEventSynchronize(p_tile_data->offloading_last_read_event) );
		CUDA_OP( cudaEventSynchronize(p_tile_data->offloading_last_write_event) );

		CUDA_OP( cudaEventSynchronize(p_tile_data->host_last_read_event) );
		CUDA_OP( cudaEventSynchronize(p_tile_data->host_last_write_event) );

		//destroy cuda streams and events inside the tile
		CUDA_OP( cudaEventDestroy(p_tile_data->kernel_last_read_event) );
		CUDA_OP( cudaEventDestroy(p_tile_data->kernel_last_write_event) );

		CUDA_OP( cudaEventDestroy(p_tile_data->offloading_last_read_event) );
		CUDA_OP( cudaEventDestroy(p_tile_data->offloading_last_write_event) );

		CUDA_OP( cudaEventDestroy(p_tile_data->host_last_read_event) );
		CUDA_OP( cudaEventDestroy(p_tile_data->host_last_write_event) );

		CUDA_OP( cudaStreamDestroy(p_tile_data->stream) );

		//Remove tle from tile linked list 
		if (p_tile_data->p_tile_elem->p_prev != NULL) {
			if (p_tile_data->p_tile_elem->p_next == NULL) {
				p_tile_data->p_tile_elem->p_prev->p_next = NULL;
				p_ctrl->p_tile_list_tail = p_tile_data->p_tile_elem->p_prev;
			} else {
				p_tile_data->p_tile_elem->p_prev->p_next = p_tile_data->p_tile_elem->p_next;
			}
		}

		if (p_tile_data->p_tile_elem->p_next != NULL) {
			if (p_tile_data->p_tile_elem->p_prev == NULL) {
				p_tile_data->p_tile_elem->p_next->p_prev = NULL;
				p_ctrl->p_tile_list_head = p_tile_data->p_tile_elem->p_next;
			} else {
				p_tile_data->p_tile_elem->p_next->p_prev = p_tile_data->p_tile_elem->p_prev;
			}
		}

		//Clear node fields
		p_tile_data->p_tile_elem->p_tile_ext = NULL;
		p_tile_data->p_tile_elem->p_next = NULL;
		p_tile_data->p_tile_elem->p_prev = NULL;

		//Free node
		free(p_tile_data->p_tile_elem);
	}

	if (p_tile->memStatus == HIT_MS_OWNER) {
		if ( (p_tile_data->mem_flags & CTRL_MEM_PINNED) || (!(p_tile_data->mem_flags & CTRL_MEM_NOPINNED) && p_ctrl->default_alloc_mode == CTRL_MEM_PINNED) ) { 
			//Free host image of the tile
			CUDA_OP( cudaFreeHost(p_tile->data) );
		} else {
			free(p_tile->data);
		}
		
		//Free device image of the tile 
		CUDA_OP( cudaFree(p_tile_data->p_device_data) );
		
		p_tile->memPtr = NULL;
		p_tile->data = NULL;
		p_tile->memStatus = HIT_MS_NOMEM;
	}

	//Clear tile fields
	p_tile->ext = NULL;
	p_tile_data->p_ctrl = NULL;

	// Free tile
	free(p_tile_data);

	Ctrl_Cuda_Sync(p_ctrl);
}

void Ctrl_Cuda_EvalTaskMoveTo(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	//If tile is not updated perform the transfer
	if (p_tile_data->last_update != CTRL_TILE_LAST_UPDATE_HOST_DEV || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_Cuda_EvalTaskMoveToInner(p_ctrl, p_tile);
	}

	//If policy is sync wait for transfer to finish
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaEventSynchronize(p_ctrl->event_seq) );
		Ctrl_Cuda_Sync(p_ctrl);
	}
}

void Ctrl_Cuda_EvalTaskMoveFrom(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	//If tile is not updated perform the transfer
	if (p_tile_data->last_update != CTRL_TILE_LAST_UPDATE_HOST_DEV || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_Cuda_EvalTaskMoveFromInner(p_ctrl, p_tile);
	}

	//If policy is sync wait for transfer to finish
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		CUDA_OP( cudaEventSynchronize(p_ctrl->event_seq) );
		Ctrl_Cuda_Sync(p_ctrl);
	}
}

void Ctrl_Cuda_EvalTaskWaitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_Cuda_Tile *p_tile_data = (Ctrl_Cuda_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	//Wait for all work related to this tile to finish 
	CUDA_OP( cudaEventSynchronize(p_tile_data->kernel_last_read_event) );
	CUDA_OP( cudaEventSynchronize(p_tile_data->kernel_last_write_event) );

	CUDA_OP( cudaEventSynchronize(p_tile_data->offloading_last_read_event) );
	CUDA_OP( cudaEventSynchronize(p_tile_data->offloading_last_write_event) );

	CUDA_OP( cudaEventSynchronize(p_tile_data->host_last_read_event) );
	CUDA_OP( cudaEventSynchronize(p_tile_data->host_last_write_event) );

	Ctrl_Cuda_Sync(p_ctrl);
}

void Ctrl_Cuda_EvalTaskSetDependanceMode(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	p_ctrl->dependance_mode = p_task->flags;
	if (p_ctrl->policy == CTRL_POLICY_SYNC){
		Ctrl_Cuda_Sync(p_ctrl);
	}
}
///@endcond