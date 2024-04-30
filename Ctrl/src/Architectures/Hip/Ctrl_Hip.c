///@cond INTERNAL
/**
 * @file Ctrl_Hip.c
 * @author Trasgo Group
 * @brief Source code for HIP backend.
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

#include "Architectures/Hip/Ctrl_Hip.h"

/*************************************************************
 ******** Prototypes of private functions ********************
 *************************************************************/

/**
 * Wrapper to launch host tasks using \e hipLaunchHostFunc as a workaround to signal hip events when the host task is done
 *
 * This function is sent to the hip host tasks stream, then it pushes the task to the host task queue and waits until
 * execution of the host task is completed.
 *
 * @param p_task Task to be executed.
 */
void Ctrl_Hip_LaunchHost(void *p_task);

/**
 * Allocate memory for a new \e Ctrl_Hip_Tile.
 *
 * @param p_ctrl Pointer to the ctrl to be attached to the tile.
 * @param p_task Pointer to the task containing the HitTile that will contain the new \e Ctrl_Hip_Tile.
 *
 * @see Ctrl_Hip_EvalTaskAllocTile, Ctrl_Hip_Tile
 */
void Ctrl_Hip_CreateTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Initializate a \e Ctrl_Hip_Tile.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile and to be updated of the initialization.
 * @param p_task Pointer to the task containing the HitTile that contains the \e Ctrl_Hip_Tile to be.
 * initialized.
 */
void Ctrl_Hip_InitTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Perform memory transfer from host to device.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 *
 * @see Ctrl_Hip_EvalTaskMoveTo
 */
void Ctrl_Hip_EvalTaskMoveToInner(Ctrl_Hip *p_ctrl, HitTile *p_tile);

/**
 * Perform memory transfer from device to host.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 *
 * @see Ctrl_Hip_EvalTaskMoveFrom
 */
void Ctrl_Hip_EvalTaskMoveFromInner(Ctrl_Hip *p_ctrl, HitTile *p_tile);

/*************************************************************
 ******** Prototypes of tasks' evaluation functions **********
 *************************************************************/

/**
 * Destroy a HIP ctrl.
 * @param p_ctrl ctrl to be destroyed.
 */
void Ctrl_Hip_Destroy(Ctrl_Hip *p_ctrl);

/**
 * Evaluation of kernel launch.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Launch
 */
void Ctrl_Hip_EvalTaskKernelLaunch(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of host task launch.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_HostTask
 */
void Ctrl_Hip_EvalTaskHostTaskLaunch(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of global sync. Waits for all work related to any tile attached to this ctrl.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_GlobalSync
 */
void Ctrl_Hip_EvalTaskGlobalSync(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of creation of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Domain
 */
void Ctrl_Hip_EvalTaskDomainTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of allocation of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Alloc
 */
void Ctrl_Hip_EvalTaskAllocTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of subselecting tiles.
 *
 * @param p_ctrl: Ctrl in charge of task.
 * @param p_task: task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Select
 */
void Ctrl_Hip_EvalTaskSelectTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of freeing of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Free
 */
void Ctrl_Hip_EvalTaskFreeTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 *
 * This function checks state of tile and, if comunication is necesary calls to \e Ctrl_Hip_EvalTaskMoveToInner.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Hip_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_Hip_EvalTaskMoveTo(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 *
 * This function checks state of tile and, if comunication is necesary calls to \e Ctrl_Hip_EvalTaskMoveToInner.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_Hip_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_Hip_EvalTaskMoveFrom(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of wait. Waits for all the work related to the tile specified in \p p_task .
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Hip_EvalTask, Ctrl_WaitTile
 */
void Ctrl_Hip_EvalTaskWaitTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of change of dependance mode.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated. .
 */
void Ctrl_Hip_EvalTaskSetDependanceMode(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task);

/********************************************
 ******** HIP Controller functions **********
 ********************************************/

void Ctrl_Hip_Create(Ctrl_Hip *p_ctrl, Ctrl_Policy policy, char *args) {
	p_ctrl->policy           = policy;
	p_ctrl->device           = atoi(strtok(args, " "));
	char *streams            = strtok(NULL, "");
	p_ctrl->n_kernel_streams = streams == NULL ? 1 : atoi(streams);
	if (p_ctrl->n_kernel_streams <= 0) {
		fprintf(stderr, "[Ctrl_Hip] Warning: Tried to create Hip Ctrl with less than one stream; defaulting to 1.");
		fflush(stderr);
	}
	p_ctrl->kernel_streams     = (hipStream_t *)malloc(p_ctrl->n_kernel_streams * sizeof(hipStream_t));
	p_ctrl->dependance_mode    = CTRL_MODE_IMPLICIT;
	p_ctrl->default_alloc_mode = CTRL_MEM_PINNED;

	// set the main thread to use the desired Hip device.
	HIP_OP(hipSetDevice(p_ctrl->device));

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	// Init HIP streams
	for (int i = 0; i < p_ctrl->n_kernel_streams; i++) {
		HIP_OP(hipStreamCreateWithFlags(&(p_ctrl->kernel_streams[i]), hipStreamNonBlocking));
	}
	HIP_OP(hipStreamCreateWithFlags(&(p_ctrl->stream_host), hipStreamNonBlocking));

	HIP_OP(hipEventCreateWithFlags(&(p_ctrl->event_seq), hipEventDisableTiming));
}

void Ctrl_Hip_EvalTask(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_KERNEL:
			Ctrl_Hip_EvalTaskKernelLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_HOST:
			Ctrl_Hip_EvalTaskHostTaskLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_GLOBALSYNC:
			Ctrl_Hip_EvalTaskGlobalSync(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_ALLOCTILE:
			Ctrl_Hip_EvalTaskAllocTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DOMAINTILE:
			Ctrl_Hip_EvalTaskDomainTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_SELECTTILE:
			Ctrl_Hip_EvalTaskSelectTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_FREETILE:
			Ctrl_Hip_EvalTaskFreeTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVETO:
			Ctrl_Hip_EvalTaskMoveTo(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVEFROM:
			Ctrl_Hip_EvalTaskMoveFrom(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_WAITTILE:
			Ctrl_Hip_EvalTaskWaitTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DESTROYCTRL:
			Ctrl_Hip_Destroy(p_ctrl);
			break;
		case CTRL_TASK_TYPE_SETDEPENDANCEMODE:
			Ctrl_Hip_EvalTaskSetDependanceMode(p_ctrl, p_task);
			break;
		default:
			fprintf(stderr, "[Ctrl_Hip] Unsupported task type: %d.\n", p_task->task_type);
			fflush(stderr);
			exit(EXIT_FAILURE);
	}
}

void Ctrl_Hip_GetInfo(Ctrl_Hip *p_ctrl, Ctrl_Info *p_info) {
	hipDeviceProp_t hip_dev_prop;
	hipGetDeviceProperties(&hip_dev_prop, p_ctrl->device);
	p_info->type = "HIP";
	strncpy(p_info->device_name, hip_dev_prop.name, CTRL_MAX_DEV_NAME - 1);
	p_info->device_name[255] = '\0';
	p_info->n_kernel_queues  = p_ctrl->n_kernel_streams;
}

/*********************************
 ******* Private functions *******
 *********************************/

void Ctrl_Hip_LaunchHost(void *p_task) {
	// push task to host queue
	Ctrl_TaskQueue_Push(p_ctrl_host_stream, *(Ctrl_Task *)p_task);
	free(p_task);

	// wait for host task execution to finish
	Ctrl_CpuEvent e = Ctrl_CpuEvent_Create();
	Ctrl_CpuEvent_Record(&e, p_ctrl_host_stream);
	Ctrl_CpuEvent_Wait(e);
}

void Ctrl_Hip_CreateTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)malloc(sizeof(Ctrl_Hip_Tile));

	p_tile_data->p_ctrl = p_ctrl;

	p_tile_data->host_status   = CTRL_TILE_UNALLOC;
	p_tile_data->device_status = CTRL_TILE_UNALLOC;

	p_tile->ext = (void *)p_tile_data;

	p_tile_data->p_parent_ext = NULL;

	p_tile_data->is_initialized = false;
}

void Ctrl_Hip_InitTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	Ctrl_Hip_Tile_List *p_list_node = (Ctrl_Hip_Tile_List *)malloc(sizeof(Ctrl_Hip_Tile_List));

	p_list_node->p_prev      = NULL;
	p_list_node->p_next      = NULL;
	p_list_node->p_tile_ext  = p_tile_data;
	p_tile_data->p_tile_elem = p_list_node;

	if (p_ctrl->p_tile_list_tail != NULL) {
		p_ctrl->p_tile_list_tail->p_next = p_list_node;
		p_list_node->p_prev              = p_ctrl->p_tile_list_tail;
		p_ctrl->p_tile_list_tail         = p_list_node;
	} else {
		p_ctrl->p_tile_list_head = p_ctrl->p_tile_list_tail = p_list_node;
	}

	HIP_OP(hipEventCreateWithFlags(&(p_tile_data->kernel_last_read_event), hipEventDisableTiming));
	HIP_OP(hipEventCreateWithFlags(&(p_tile_data->kernel_last_write_event), hipEventDisableTiming));

	HIP_OP(hipEventCreateWithFlags(&(p_tile_data->offloading_last_read_event), hipEventDisableTiming));
	HIP_OP(hipEventCreateWithFlags(&(p_tile_data->offloading_last_write_event), hipEventDisableTiming));

	HIP_OP(hipEventCreateWithFlags(&(p_tile_data->host_last_read_event), hipEventDisableTiming));
	HIP_OP(hipEventCreateWithFlags(&(p_tile_data->host_last_write_event), hipEventDisableTiming));

	HIP_OP(hipStreamCreateWithFlags(&(p_tile_data->stream), hipStreamNonBlocking));

	p_tile_data->is_initialized = true;
}

void Ctrl_Hip_EvalTaskMoveToInner(Ctrl_Hip *p_ctrl, HitTile *p_tile) {
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	// Wait for appropiate events
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->kernel_last_read_event, 0));
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->kernel_last_write_event, 0));
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->offloading_last_read_event, 0));
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->host_last_write_event, 0));

	// Wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_ctrl->event_seq, 0));
	}

	/* @arturo TODO: STRIDED TILES */

	// Send memcpy to hip stream
	/* SINGLE CONTIGUOUS DATA TRANSFER:
	 *	- TILES WITH THEIR OWN MEMORY ALLOCATION
	 *	- OR CONTIGUOUS 1D TILES
	 *	- OR ROW BAND WITH FULL MINOR DIMENSIONS */
	if ((p_tile->memStatus == HIT_MS_OWNER) ||
		(hit_tileDims(*p_tile) == 1) ||
		(hit_tileDims(*p_tile) > 1 && p_tile->acumCard / p_tile->card[0] == p_tile->origAcumCard[2])) {
		HIP_OP(
			hipMemcpyAsync(p_tile_data->p_device_data,
						   p_tile->data,
						   ((size_t)(p_tile->acumCard)) * (p_tile->baseExtent),
						   hipMemcpyHostToDevice,
						   p_tile_data->stream));
	}
	/* 2D TILES */
	else if (p_tile->shape.info.sig.numDims == 2) {
		HIP_OP(
			hipMemcpy2DAsync(p_tile_data->p_device_data,
							 (p_tile->baseExtent) * p_tile->origAcumCard[1],
							 p_tile->data,
							 (p_tile->baseExtent) * p_tile->origAcumCard[1],
							 (p_tile->baseExtent) * p_tile->card[1],
							 p_tile->card[0],
							 hipMemcpyHostToDevice,
							 p_tile_data->stream));
	}
	/* 3D TILES */
	else if (p_tile->shape.info.sig.numDims == 3) {
		struct hipMemcpy3DParms params = {0};

		params.srcPtr = make_hipPitchedPtr(p_tile->data,
										   (p_tile->baseExtent) * p_tile->origAcumCard[2],
										   p_tile->origAcumCard[2],
										   p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.dstPtr = make_hipPitchedPtr(p_tile_data->p_device_data,
										   (p_tile->baseExtent) * p_tile->origAcumCard[2],
										   p_tile->origAcumCard[2],
										   p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.extent = make_hipExtent(p_tile->card[2] * p_tile->baseExtent, p_tile->card[1], p_tile->card[0]);
		params.kind   = hipMemcpyHostToDevice;

		HIP_OP(hipMemcpy3DAsync(&params, p_tile_data->stream));
	} else {
		fprintf(stderr, "Internal Error: Number of dimensions not supported for non-owner tile in MoveTo: %d\n",
				p_tile->shape.info.sig.numDims);
	}

	// Update events
	HIP_OP(hipEventRecord(p_tile_data->offloading_last_write_event, p_tile_data->stream));

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipEventRecord(p_ctrl->event_seq, p_tile_data->stream));
	}

	// Update state of the tile
	p_tile_data->host_status   = CTRL_TILE_VALID;
	p_tile_data->device_status = CTRL_TILE_VALID;
}

void Ctrl_Hip_EvalTaskMoveFromInner(Ctrl_Hip *p_ctrl, HitTile *p_tile) {
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	// Wait for appropiate events
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->kernel_last_write_event, 0));
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->offloading_last_write_event, 0));
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->host_last_read_event, 0));
	HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_tile_data->host_last_write_event, 0));

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipStreamWaitEvent(p_tile_data->stream, p_ctrl->event_seq, 0));
	}

	/* @arturo TODO: STRIDED TILES */

	// Send memcpy to hip stream
	/* SINGLE CONTIGUOUS DATA TRANSFER:
	 *	- TILES WITH THEIR OWN MEMORY ALLOCATION
	 *	- OR CONTIGUOUS 1D TILES
	 *	- OR ROW BAND WITH FULL MINOR DIMENSIONS */
	if ((p_tile->memStatus == HIT_MS_OWNER) ||
		(hit_tileDims(*p_tile) == 1) ||
		(hit_tileDims(*p_tile) > 1 && p_tile->acumCard / p_tile->card[0] == p_tile->origAcumCard[2])) {
		HIP_OP(
			hipMemcpyAsync(p_tile->data,
						   p_tile_data->p_device_data,
						   ((size_t)(p_tile->acumCard)) * (p_tile->baseExtent),
						   hipMemcpyDeviceToHost,
						   p_tile_data->stream));
	}
	/* CONTIGUOUS 2D TILES */
	else if (p_tile->shape.info.sig.numDims == 2) {
		HIP_OP(
			hipMemcpy2DAsync(p_tile->data,                                   // dst
							 (p_tile->baseExtent) * p_tile->origAcumCard[1], // dpitch
							 p_tile_data->p_device_data,                     // src
							 (p_tile->baseExtent) * p_tile->origAcumCard[1], // spitch
							 (p_tile->baseExtent) * p_tile->card[1],         // width
							 p_tile->card[0],                                // height
							 hipMemcpyDeviceToHost,
							 p_tile_data->stream));
	}
	/* CONTIGUOUS 3D TILES */
	else if (p_tile->shape.info.sig.numDims == 3) {
		struct hipMemcpy3DParms params = {0};

		params.srcPtr = make_hipPitchedPtr(p_tile_data->p_device_data,
										   (p_tile->baseExtent) * p_tile->origAcumCard[2],
										   p_tile->origAcumCard[2],
										   p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.dstPtr = make_hipPitchedPtr(p_tile->data,
										   (p_tile->baseExtent) * p_tile->origAcumCard[2],
										   p_tile->origAcumCard[2],
										   p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.extent = make_hipExtent(p_tile->card[2] * p_tile->baseExtent, p_tile->card[1], p_tile->card[0]);
		params.kind   = hipMemcpyDeviceToHost;

		HIP_OP(hipMemcpy3DAsync(&params, p_tile_data->stream));
	} else {
		fprintf(stderr, "Internal Error: Number of dimensions not supported for non-owner tile in MoveFrom: %d\n",
				p_tile->shape.info.sig.numDims);
	}

	// Update events
	HIP_OP(hipEventRecord(p_tile_data->offloading_last_read_event, p_tile_data->stream));

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipEventRecord(p_ctrl->event_seq, p_tile_data->stream));
	}

	// Update state of the tile
	p_tile_data->host_status   = CTRL_TILE_VALID;
	p_tile_data->device_status = CTRL_TILE_VALID;
}

/**********************************
 ** TASKS'S EVALUATION FUNCTIONS **
 **********************************/

void Ctrl_Hip_Destroy(Ctrl_Hip *p_ctrl) {
	if (p_ctrl->p_tile_list_head != NULL) {
		fprintf(stderr, "Warning: Tiles left attached to ctrl %d\n", p_ctrl->global_id);
		fflush(stderr);
	}

	HIP_OP(hipSetDevice(p_ctrl->device));

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	for (int i = 0; i < p_ctrl->n_kernel_streams; i++) {
		HIP_OP(hipStreamDestroy(p_ctrl->kernel_streams[i]));
	}
	HIP_OP(hipStreamDestroy(p_ctrl->stream_host));

	HIP_OP(hipEventDestroy(p_ctrl->event_seq));
	free(p_ctrl->kernel_streams);
}

void Ctrl_Hip_EvalTaskGlobalSync(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HIP_OP(hipSetDevice(p_ctrl->device));
	HIP_OP(hipDeviceSynchronize());
}

void Ctrl_Hip_EvalTaskKernelLaunch(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HIP_OP(hipSetDevice(p_ctrl->device));

	// Check if the specified stream exists:
	if (p_task->stream < 0 || p_task->stream >= p_ctrl->n_kernel_streams) {
		fprintf(stderr, "[Ctrl_Hip] Internal Error: Tried to execute a task on a nonexistent stream: %d", p_task->stream);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	hipStream_t stream_kernel = p_ctrl->kernel_streams[p_task->stream];

	// wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

			if (hit_tileIsNull(*p_tile)) {
				fprintf(stderr, "Warning: Launching task %s, skipping null tile on parameter %d (starting at 0)\n", p_task->p_func_name, i);
				fflush(stderr);
				continue;
			}

			if (p_tile_data->device_status == CTRL_TILE_UNALLOC) {
				fprintf(stderr, "[Ctrl_Hip] Internal Error: Launching kernel %s with a tile with no device memory as parameter %d (starting at 0)\n", p_task->p_func_name, i);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}

			if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data->device_status == CTRL_TILE_INVALID) {
				if (p_tile_data->host_status == CTRL_TILE_VALID && p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
					// if tile's role is IN or IO, is not updated on device and host has memory allocated transfer it
					Ctrl_Hip_EvalTaskMoveToInner(p_ctrl, p_tile);
				}
				if (p_tile_data->device_status == CTRL_TILE_INVALID) {
					fprintf(stderr, "[Ctrl_Hip] Warning: Tile with uninitialized data as input on kernel %s, parameter: %d (starting at 0)\n", p_task->p_func_name, i);
					fflush(stderr);
				}
			}

			if (p_ctrl->n_kernel_streams != 0) {
				HIP_OP(hipStreamWaitEvent(stream_kernel, p_tile_data->kernel_last_write_event, 0));
			}

			if (p_tile_data->host_status != CTRL_TILE_UNALLOC && !(p_tile_data->device_status == CTRL_TILE_VALID && p_tile_data->host_status == CTRL_TILE_INVALID)) {
				HIP_OP(hipStreamWaitEvent(stream_kernel, p_tile_data->offloading_last_write_event, 0));
			}

			if (p_task->p_roles[i] != KERNEL_IN) {
				if (p_ctrl->n_kernel_streams != 0) {
					HIP_OP(hipStreamWaitEvent(stream_kernel, p_tile_data->kernel_last_read_event, 0));
				}

				if (p_tile_data->host_status != CTRL_TILE_UNALLOC && !(p_tile_data->device_status == CTRL_TILE_VALID && p_tile_data->host_status == CTRL_TILE_INVALID)) {
					HIP_OP(hipStreamWaitEvent(stream_kernel, p_tile_data->offloading_last_read_event, 0));
				}
			}
		}
	}

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipStreamWaitEvent(stream_kernel, p_ctrl->event_seq, 0));
	}

	// create request with info for kernel execution
	Ctrl_Request request;
	request.hip.p_stream = &stream_kernel;

	// Launch kernel to HIP kernel stream
	p_task->pfn_kernel_wrapper(request, p_task->device_id, CTRL_TYPE_HIP, p_task->threads, p_task->blocksize, p_task->p_arguments);

	// update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

			if (p_task->p_roles[i] != KERNEL_IN) {
				if (p_tile_data->host_status == CTRL_TILE_VALID) p_tile_data->host_status = CTRL_TILE_INVALID;
				p_tile_data->device_status = CTRL_TILE_VALID;
				HIP_OP(hipEventRecord(p_tile_data->kernel_last_write_event, stream_kernel));
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				HIP_OP(hipEventRecord(p_tile_data->kernel_last_read_event, stream_kernel));
			}
		}
	}

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipEventRecord(p_ctrl->event_seq, stream_kernel));
	}
}

void Ctrl_Hip_EvalTaskHostTaskLaunch(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HIP_OP(hipSetDevice(p_ctrl->device));

	// wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

			if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
				fprintf(stderr, "[Ctrl_Hip] Internal Error: Launching host task with tile with no host memory as argument %d\n", i);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}

			// if tile's role is IN or IO and is not updated on host transfer it
			if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data->host_status == CTRL_TILE_INVALID) {
				if (p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT && p_tile_data->device_status == CTRL_TILE_VALID) {
					Ctrl_Hip_EvalTaskMoveFromInner(p_ctrl, p_tile);
				}
				if (p_tile_data->host_status == CTRL_TILE_INVALID) {
					fprintf(stderr, "[Ctrl_Hip] Warning: Tile with uninitialized data as input on host task %d\n", i);
					fflush(stderr);
				}
			}

			if (p_tile_data->device_status != CTRL_TILE_UNALLOC && !(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data->device_status == CTRL_TILE_INVALID)) {
				HIP_OP(hipStreamWaitEvent(p_ctrl->stream_host, p_tile_data->offloading_last_read_event, 0));
			}

			if (p_task->p_roles[i] != KERNEL_IN) {
				if (p_tile_data->device_status == CTRL_TILE_VALID) p_tile_data->device_status = CTRL_TILE_INVALID;
				p_tile_data->host_status = CTRL_TILE_VALID;

				if (p_tile_data->device_status != CTRL_TILE_UNALLOC && !(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data->device_status == CTRL_TILE_INVALID)) {
					HIP_OP(hipStreamWaitEvent(p_ctrl->stream_host, p_tile_data->offloading_last_write_event, 0));
				}
			}
		}
	}

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipStreamWaitEvent(p_ctrl->stream_host, p_ctrl->event_seq, 0));
	}

	// Allocate memory for task, needed because task wont be copied to host queue until hostfunc executes on hip steam
	Ctrl_Task *p_task_aux = (Ctrl_Task *)malloc(sizeof(Ctrl_Task));
	*p_task_aux           = *p_task;
	// Send task to HIP host stream, when this execs it will be pushed to host queue
	HIP_OP(hipLaunchHostFunc(p_ctrl->stream_host, Ctrl_Hip_LaunchHost, p_task_aux));

	// update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

			if (p_task->p_roles[i] != KERNEL_IN) {
				HIP_OP(hipEventRecord(p_tile_data->host_last_write_event, p_ctrl->stream_host));
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				HIP_OP(hipEventRecord(p_tile_data->host_last_read_event, p_ctrl->stream_host));
			}
		}
	}

	// wait for host task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipEventRecord(p_ctrl->event_seq, p_ctrl->stream_host));
		HIP_OP(hipEventSynchronize(p_ctrl->event_seq));
	}
}

void Ctrl_Hip_EvalTaskDomainTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_Hip_CreateTile(p_ctrl, p_task);
}

void Ctrl_Hip_EvalTaskAllocTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	HIP_OP(hipSetDevice(p_ctrl->device));

	if (!(p_tile_data->is_initialized))
		Ctrl_Hip_InitTile(p_ctrl, p_task);

	if (p_task->flags & (CTRL_MEM_ALLOC_BOTH | CTRL_MEM_ALLOC_HOST) || !(p_task->flags & CTRL_MEM_ALLOC_DEV)) {
		if (p_tile_data->host_status != CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Hip] Warning: Host memory already allocated for this tile, ignoring this call.\n");
			fflush(stderr);
			return;
		}

		if ((p_task->flags & CTRL_MEM_PINNED) || (!(p_task->flags & CTRL_MEM_NOPINNED) && p_ctrl->default_alloc_mode == CTRL_MEM_PINNED)) {
			// Allocate host "pinned" memory
			p_tile_data->is_pinned = true;
			HIP_OP(hipHostMalloc(&(p_tile->data), ((size_t)(p_tile->origAcumCard[0])) * (p_tile->baseExtent), hipHostMallocDefault));
		} else {
			// Allocate host memory the usual way (on HIP this is probably never a good idea to do, but the option is there)
			p_tile_data->is_pinned = false;
			p_tile->data           = (void *)malloc((size_t)p_tile->acumCard * p_tile->baseExtent);
		}
		p_tile_data->host_status = CTRL_TILE_INVALID;
		p_tile->memPtr           = p_tile->data;
	}

	if (p_task->flags & (CTRL_MEM_ALLOC_BOTH | CTRL_MEM_ALLOC_DEV) || !(p_task->flags & CTRL_MEM_ALLOC_HOST)) {
		if (p_tile_data->device_status != CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Hip] Warning: Device memory already allocated for this tile, ignoring this call.\n");
			fflush(stderr);
			return;
		}
		// Allocate device memory
		p_tile_data->device_status = CTRL_TILE_INVALID;
		HIP_OP(hipMalloc(&(p_tile_data->p_device_data), ((size_t)(p_tile->origAcumCard[0])) * (p_tile->baseExtent)));
	}

	// TODO Habria que poner qstride, origAcumCard, y memStatus en p_tile_data con los dev (al tener dos espacios de memoria)
}

void Ctrl_Hip_EvalTaskSelectTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_Hip_CreateTile(p_ctrl, p_task);

	HitTile *p_tile = (HitTile *)(p_task->p_tile);

	// @arturo: Bug, select tasks for NULL Tiles should not be introduced in the queue
	if (hit_tileIsNull(*p_tile)) {
		return;
	}

	HitTile       *p_parent      = p_tile->ref;
	Ctrl_Hip_Tile *p_tile_data   = (Ctrl_Hip_Tile *)(p_tile->ext);
	p_tile_data->p_parent_ext    = ((Ctrl_Hip_Tile *)(p_tile->ref->ext));
	Ctrl_Hip_Tile *p_parent_data = p_tile_data->p_parent_ext;

	if ((p_task->flags & CTRL_SELECT_INIT) == CTRL_SELECT_INIT) {
		HIP_OP(hipSetDevice(p_ctrl->device));
		Ctrl_Hip_InitTile(p_ctrl, p_task);
	}

	if (p_tile->memStatus == HIT_MS_NOT_OWNER) {
		p_tile_data->host_status   = p_tile_data->p_parent_ext->host_status;
		p_tile_data->device_status = p_tile_data->p_parent_ext->device_status;

		p_tile_data->p_device_data = p_parent_data->p_device_data + (p_tile->data - p_parent->data);
	}
}

void Ctrl_Hip_EvalTaskFreeTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	if (p_tile_data->is_initialized) {
		// Wait for all work related to this tile to finish
		HIP_OP(hipEventSynchronize(p_tile_data->kernel_last_read_event));
		HIP_OP(hipEventSynchronize(p_tile_data->kernel_last_write_event));

		HIP_OP(hipEventSynchronize(p_tile_data->offloading_last_read_event));
		HIP_OP(hipEventSynchronize(p_tile_data->offloading_last_write_event));

		HIP_OP(hipEventSynchronize(p_tile_data->host_last_read_event));
		HIP_OP(hipEventSynchronize(p_tile_data->host_last_write_event));

		// destroy hip streams and events inside the tile
		HIP_OP(hipEventDestroy(p_tile_data->kernel_last_read_event));
		HIP_OP(hipEventDestroy(p_tile_data->kernel_last_write_event));

		HIP_OP(hipEventDestroy(p_tile_data->offloading_last_read_event));
		HIP_OP(hipEventDestroy(p_tile_data->offloading_last_write_event));

		HIP_OP(hipEventDestroy(p_tile_data->host_last_read_event));
		HIP_OP(hipEventDestroy(p_tile_data->host_last_write_event));

		HIP_OP(hipStreamDestroy(p_tile_data->stream));

		// Remove tle from tile linked list
		if (p_tile_data->p_tile_elem->p_prev != NULL) {
			if (p_tile_data->p_tile_elem->p_next == NULL) {
				p_tile_data->p_tile_elem->p_prev->p_next = NULL;
				p_ctrl->p_tile_list_tail                 = p_tile_data->p_tile_elem->p_prev;
			} else {
				p_tile_data->p_tile_elem->p_prev->p_next = p_tile_data->p_tile_elem->p_next;
			}
		}

		if (p_tile_data->p_tile_elem->p_next != NULL) {
			if (p_tile_data->p_tile_elem->p_prev == NULL) {
				p_tile_data->p_tile_elem->p_next->p_prev = NULL;
				p_ctrl->p_tile_list_head                 = p_tile_data->p_tile_elem->p_next;
			} else {
				p_tile_data->p_tile_elem->p_next->p_prev = p_tile_data->p_tile_elem->p_prev;
			}
		}

		// Clear node fields
		p_tile_data->p_tile_elem->p_tile_ext = NULL;
		p_tile_data->p_tile_elem->p_next     = NULL;
		p_tile_data->p_tile_elem->p_prev     = NULL;

		// Free node
		free(p_tile_data->p_tile_elem);
	}

	// Free host image of the tile, equivalent to hit_tileFree(*p_tile);
	if (p_tile->memStatus == HIT_MS_OWNER) {
		if (p_tile_data->host_status != CTRL_TILE_UNALLOC) {
			// Free host image of the tile
			if (p_tile_data->is_pinned) {
				HIP_OP(hipHostFree(p_tile->data));
			} else {
				free(p_tile->data);
			}
		}

		if (p_tile_data->device_status != CTRL_TILE_UNALLOC) {
			// Free device image of the tile
			HIP_OP(hipFree(p_tile_data->p_device_data));
		}

		p_tile->memPtr    = NULL;
		p_tile->data      = NULL;
		p_tile->memStatus = HIT_MS_NOMEM;
	}

	// Clear tile fields
	p_tile->ext         = NULL;
	p_tile_data->p_ctrl = NULL;

	// Free tile
	free(p_tile_data);
}

void Ctrl_Hip_EvalTaskMoveTo(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Hip] Internal Error: Tryinng to move tile from host to device but host memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data->device_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Hip] Internal Error: Tryinng to move tile from host to device but device memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data->host_status == CTRL_TILE_INVALID) {
		fprintf(stderr, "[Ctrl_Hip] Warning: Moving a tile from host to device with invalid data on host memory\n");
		fflush(stderr);
	}

	// If tile is not updated perform the transfer
	if (!(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data->device_status == CTRL_TILE_VALID) || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_Hip_EvalTaskMoveToInner(p_ctrl, p_tile);
	}

	// If policy is sync wait for transfer to finish
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipEventSynchronize(p_ctrl->event_seq));
	}
}

void Ctrl_Hip_EvalTaskMoveFrom(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Hip] Internal Error: Trying to move tile from device to host but host memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data->device_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Hip] Internal Error: Trying to move tile from device to host but device memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data->device_status == CTRL_TILE_INVALID) {
		fprintf(stderr, "[Ctrl_Hip] Warning: Moving a tile from device to host with invalid data on device memory\n");
		fflush(stderr);
	}

	// If tile is not updated perform the transfer
	if (!(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data->device_status == CTRL_TILE_VALID) || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_Hip_EvalTaskMoveFromInner(p_ctrl, p_tile);
	}

	// If policy is sync wait for transfer to finish
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		HIP_OP(hipEventSynchronize(p_ctrl->event_seq));
	}
}

void Ctrl_Hip_EvalTaskWaitTile(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Hip_Tile *p_tile_data = (Ctrl_Hip_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	// Wait for all work related to this tile to finish
	HIP_OP(hipEventSynchronize(p_tile_data->kernel_last_read_event));
	HIP_OP(hipEventSynchronize(p_tile_data->kernel_last_write_event));

	HIP_OP(hipEventSynchronize(p_tile_data->offloading_last_read_event));
	HIP_OP(hipEventSynchronize(p_tile_data->offloading_last_write_event));

	HIP_OP(hipEventSynchronize(p_tile_data->host_last_read_event));
	HIP_OP(hipEventSynchronize(p_tile_data->host_last_write_event));
}

void Ctrl_Hip_EvalTaskSetDependanceMode(Ctrl_Hip *p_ctrl, Ctrl_Task *p_task) {
	p_ctrl->dependance_mode = p_task->flags;
}

///@endcond
