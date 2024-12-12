///@cond INTERNAL
/**
 * @file Ctrl_Cuda.c
 * @brief Source code for CUDA backend.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Architectures/Cuda/Ctrl_Cuda.h"
#include "Core/Ctrl_Core.h"

/*************************************************************
 ******** Prototypes of private functions ********************
 *************************************************************/

/**
 * Initialize a \e Ctrl_Cuda_Tile.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile and to be updated of the initialization.
 * @param p_task Pointer to the task containing the HitTile that contains the \e Ctrl_Cuda_Tile to be.
 * initialized.
 */
void Ctrl_Cuda_InitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

/**
 * Enqueue memory transfer from host to device task on host queue.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the tile to be moved.
 *
 * @see Ctrl_Cuda_EvalTaskMoveTo
 */
void Ctrl_Cuda_EvalTaskMoveToInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile);

/**
 * Perform memory transfer from host to device.
 *
 * @param p_task Task containing the tile to be moved.
 * @param p_ctrl Ctrl responsible for this task
 *
 * @see Ctrl_Cuda_EvalTaskMoveTo
 */
void Ctrl_Cuda_ExecTaskMoveTo(Ctrl_Task *p_task, Ctrl_Cuda *p_ctrl);

/**
 * Perform memory transfer from device to host.
 *
 * @param p_task Task containing the tile to be moved.
 * @param p_ctrl Ctrl responsible for this task
 *
 * @see Ctrl_Cuda_EvalTaskMoveFrom
 */
void Ctrl_Cuda_ExecTaskMoveFrom(Ctrl_Task *p_task, Ctrl_Cuda *p_ctrl);

/**
 * Get \p qid th driver queue used by \p p_ctrl
 * Order of qid is htd, dth, kernels.
 *
 * @param p_ctrl Ctrl to get the queues from.
 * @param qid Pointer to the driver queues used by this ctrl
 * @return Stream \p qid of ctrl \p p_ctrl
 *
 * @pre \p qid must be between 0 and the amount of queues of \p p_ctrl
 * @see Ctrl_Cuda_GetNumQueues
 */
cudaStream_t Ctrl_Cuda_GetStreamById(Ctrl_Cuda *p_ctrl, int qid);

/*************************************************************
 ******** Prototypes of tasks' evaluation functions **********
 *************************************************************/

/**
 * Destroy a CUDA ctrl.
 * @param p_ctrl ctrl to be destroyed.
 * @param p_task Task with info for cleanup
 */
void Ctrl_Cuda_Destroy(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

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
 * Evaluation of global sync. Waits for all work related to any tile attached to this ctrl.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cuda_EvalTask, Ctrl_GlobalSync
 */
void Ctrl_Cuda_EvalTaskGlobalSync(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

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
 * Evaluation of subselecting tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cuda_EvalTask, Ctrl_Select
 */
void Ctrl_Cuda_EvalTaskSelectTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task);

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

void Ctrl_Cuda_Create(Ctrl_Cuda *p_ctrl, Ctrl_Policy policy, char *args) {
	p_ctrl->policy           = policy;
	p_ctrl->device           = atoi(strtok(args, " "));
	char *streams            = strtok(NULL, "");
	p_ctrl->n_kernel_streams = streams == NULL ? 1 : atoi(streams);
	if (p_ctrl->n_kernel_streams <= 0) {
		p_ctrl->n_kernel_streams = 1;
		fprintf(stderr, "[Ctrl_Cuda] Warning: Tried to create Cuda Ctrl with less than one stream; defaulting to 1.");
		fflush(stderr);
	}
	p_ctrl->p_kernel_driver_streams = (cudaStream_t *)malloc(p_ctrl->n_kernel_streams * EXTRA_STREAMS * sizeof(cudaStream_t));
	p_ctrl->pp_kernel_host_streams  = (Ctrl_TaskQueue **)malloc(p_ctrl->n_kernel_streams * sizeof(Ctrl_TaskQueue *));
	p_ctrl->p_stream_op_count       = (int *)calloc(p_ctrl->n_kernel_streams, sizeof(int));
	p_ctrl->p_stream_indexes        = (int *)calloc(p_ctrl->n_kernel_streams, sizeof(int));
	p_ctrl->dependance_mode         = CTRL_MODE_IMPLICIT;
	p_ctrl->default_alloc_mode      = CTRL_MEM_PINNED;

	// set the main thread to use the desired cuda device.
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	// Init streams
	CUDA_OP(cudaStreamCreateWithFlags(&(p_ctrl->htd_driver_stream), cudaStreamNonBlocking));
	CUDA_OP(cudaStreamCreateWithFlags(&(p_ctrl->dth_driver_stream), cudaStreamNonBlocking));
	p_ctrl->p_htd_host_stream = Ctrl_TaskQueue_Create();
	p_ctrl->p_dth_host_stream = Ctrl_TaskQueue_Create();
	for (int i = 0; i < p_ctrl->n_kernel_streams; i++) {
		for (int j = 0; j < EXTRA_STREAMS; j++) {
			CUDA_OP(cudaStreamCreateWithFlags(&(p_ctrl->p_kernel_driver_streams[i * EXTRA_STREAMS + j]), cudaStreamNonBlocking));
		}
		p_ctrl->pp_kernel_host_streams[i] = Ctrl_TaskQueue_Create();
	}

	p_ctrl->host_seq_event = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, p_ctrl->global_id);
	p_ctrl->dev_seq_event  = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);

	#ifdef _CTRL_CUBLAS_
	cublasCreate(&(p_ctrl->cublas_handle));
	cublasSetStream(p_ctrl->cublas_handle, p_ctrl->p_kernel_driver_streams[0]);
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	magma_init();
	magma_setdevice(p_ctrl->device);
	cudaDeviceSynchronize();
	// TODO @sergioalo reuse cublas handle if both are active???
	magma_queue_create_from_cuda(p_ctrl->device, p_ctrl->p_kernel_driver_streams[0], NULL, NULL, &p_ctrl->magma_queue);
	#endif // _CTRL_MAGMA_
}

void Ctrl_Cuda_EvalTask(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_KERNEL:
			Ctrl_Cuda_EvalTaskKernelLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_GLOBALSYNC:
			Ctrl_Cuda_EvalTaskGlobalSync(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_ALLOCTILE:
			Ctrl_Cuda_EvalTaskAllocTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_SELECTTILE:
			Ctrl_Cuda_EvalTaskSelectTile(p_ctrl, p_task);
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
		case CTRL_TASK_TYPE_DESTROYCTRL:
			Ctrl_Cuda_Destroy(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_SETDEPENDANCEMODE:
			Ctrl_Cuda_EvalTaskSetDependanceMode(p_ctrl, p_task);
			break;
		default:
			fprintf(stderr, "[Ctrl_Cuda] Unsupported task type: %d.\n", p_task->task_type);
			fflush(stderr);
			exit(EXIT_FAILURE);
	}
}

void Ctrl_Cuda_ExecTask(Ctrl_Task *p_task, Ctrl_Cuda *p_ctrl) {
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	cudaStream_t stream = Ctrl_Cuda_GetStreamById(p_ctrl, p_task->stream);

	// increment op counter for kernel streams
	if (p_task->stream >= 2) {
		int kqid = p_task->stream - 2;
		p_ctrl->p_stream_op_count[kqid]++;
		// cudaStream_t *p_stream = &p_ctrl->p_kernel_driver_streams[(kqid)*EXTRA_STREAMS + p_ctrl->p_stream_indexes[kqid]];
		if (p_ctrl->p_stream_op_count[kqid] == MAX_STREAM_TASKS) {
			// resest counter
			p_ctrl->p_stream_op_count[kqid] = 0;
			// update index
			p_ctrl->p_stream_indexes[kqid] = (p_ctrl->p_stream_indexes[kqid] + 1) % EXTRA_STREAMS;

			// event pointing to end of previous stream
			cudaEvent_t ev;
			CUDA_OP(cudaEventCreateWithFlags(&ev, cudaEventDisableTiming));
			CUDA_OP(cudaEventRecord(ev, stream));

			// use new stream
			stream = Ctrl_Cuda_GetStreamById(p_ctrl, p_task->stream);
			CUDA_OP(cudaStreamWaitEvent(stream, ev, 0));
		}
	}

	p_task->request.cuda.p_stream = &stream;
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_KERNEL:
			p_task->pfn_kernel_wrapper(p_task->request, p_task->device_id, CTRL_TYPE_CUDA, p_task->threads, p_task->blocksize, p_task->p_arguments);
			CUDA_OP(cudaEventRecord(p_task->event.event.event_cuda, stream));
			Ctrl_GenericEvent_Release(p_task->event);
			break;
		case CTRL_TASK_TYPE_MOVETO:
			Ctrl_Cuda_ExecTaskMoveTo(p_task, p_ctrl);
			CUDA_OP(cudaEventRecord(p_task->event.event.event_cuda, stream));
			Ctrl_GenericEvent_Release(p_task->event);
			break;
		case CTRL_TASK_TYPE_MOVEFROM:
			Ctrl_Cuda_ExecTaskMoveFrom(p_task, p_ctrl);
			CUDA_OP(cudaEventRecord(p_task->event.event.event_cuda, stream));
			Ctrl_GenericEvent_Release(p_task->event);
			break;
		case CTRL_TASK_TYPE_WAITEVENT:
			CUDA_OP(cudaStreamWaitEvent(stream, p_task->event.event.event_cuda, 0));
			break;
		default:
			fprintf(stderr, "[Ctrl_Cuda] ExecTask: task type %d should not get here\n", p_task->task_type);
			exit(EXIT_FAILURE);
	}
}

int Ctrl_Cuda_GetNumQueues(Ctrl_Cuda *p_ctrl) {
	return p_ctrl->n_kernel_streams + 2;
}

Ctrl_TaskQueue **Ctrl_Cuda_GetHostQueues(Ctrl_Cuda *p_ctrl, Ctrl_TaskQueue **pp_queues) {
	pp_queues[0] = p_ctrl->p_htd_host_stream;
	pp_queues[1] = p_ctrl->p_dth_host_stream;

	for (int i = 0; i < p_ctrl->n_kernel_streams; i++) {
		pp_queues[i + 2] = p_ctrl->pp_kernel_host_streams[i];
	}

	return &pp_queues[Ctrl_Cuda_GetNumQueues(p_ctrl)];
}

cudaStream_t Ctrl_Cuda_GetStreamById(Ctrl_Cuda *p_ctrl, int qid) {
	if (qid < 0 || qid > Ctrl_Cuda_GetNumQueues(p_ctrl)) {
		fprintf(stderr, "[Ctrl_Cuda_GetStreamById] Invalid qid %d\n", qid);
		exit(EXIT_FAILURE);
	}

	switch (qid) {
		case 0:
			return p_ctrl->htd_driver_stream;
		case 1:
			return p_ctrl->dth_driver_stream;
		default:
			return p_ctrl->p_kernel_driver_streams[(qid - 2) * EXTRA_STREAMS + p_ctrl->p_stream_indexes[qid - 2]];
	}
}

void Ctrl_Cuda_GetInfo(Ctrl_Cuda *p_ctrl, Ctrl_Info *p_info) {
	struct cudaDeviceProp cu_dev_prop;
	cudaGetDeviceProperties(&cu_dev_prop, p_ctrl->device);
	p_info->type = "CUDA";
	strncpy(p_info->device_name, cu_dev_prop.name, CTRL_MAX_DEV_NAME - 1);
	p_info->device_name[255] = '\0';
	p_info->n_kernel_queues  = p_ctrl->n_kernel_streams;
}

void Ctrl_Cuda_CreateTex(Ctrl_Cuda *p_ctrl, HitTile *p_tile, Ctrl_TexDesc tex_desc) {
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;

	if (hit_tileDims(*p_tile) != 2) {
		fprintf(stderr, "[Ctrl_Cuda_CreateTex] Error: textures are currently only supported for 2D matrixes.");
		exit(EXIT_FAILURE);
	}

	if (p_tile_data_impl->type == CTRL_TYPE_NULL) {
		fprintf(stderr, "[Ctrl_Cuda_CreateTex] Error: tile not asociated with ctrl.");
		exit(EXIT_FAILURE);
	}

	// TODO @sergioalo check if pitch is valid for texture

	if (p_tile_data_impl->device_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Cuda_CreateTex] Error: Device memory not allocated for this tile. You must allocate device memory before creating textures.\n");
		exit(EXIT_FAILURE);
	}

	// TODO @sergioalo channel descriptor stuff should not be hardcoded
	// TODO @sergioalo overriden texture objects are not destroyed, but this is also not done on cuda samples

	// Create texture object
	struct cudaResourceDesc resDesc;
	memset(&resDesc, 0, sizeof(resDesc));
	resDesc.resType                  = cudaResourceTypePitch2D;
	resDesc.res.pitch2D.devPtr       = p_tile_data_cuda->p_device_data;
	resDesc.res.pitch2D.width        = tex_desc.width == 0 ? hit_tileDimCard(*p_tile, 1) : tex_desc.width;
	resDesc.res.pitch2D.height       = tex_desc.height == 0 ? hit_tileDimCard(*p_tile, 0) : tex_desc.height;
	resDesc.res.pitch2D.desc         = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
	resDesc.res.pitch2D.pitchInBytes = p_tile_data_cuda->pitch;
	struct cudaTextureDesc texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.normalizedCoords = tex_desc.normalized_coords;
	texDesc.filterMode       = tex_desc.filter_mode;
	texDesc.addressMode[0]   = tex_desc.addr_mode[0];
	texDesc.addressMode[1]   = tex_desc.addr_mode[1];
	texDesc.addressMode[2]   = tex_desc.addr_mode[2];
	texDesc.readMode         = tex_desc.read_mode;
	CUDA_OP(cudaCreateTextureObject(&p_tile_data_cuda->texture, &resDesc, &texDesc, NULL));
}

void Ctrl_Cuda_SetDevice() {
	CUDA_OP(cudaSetDevice(0));
}

/*********************************
 ******* Private functions *******
 *********************************/

void Ctrl_Cuda_InitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_Tile *p_tile_data = (Ctrl_Tile *)(p_task->p_tile->ext);
	p_tile_data->valid_impls++;

	Ctrl_Cuda_Tile *p_tile_data_impl_cuda               = (Ctrl_Cuda_Tile *)malloc(sizeof(Ctrl_Cuda_Tile));
	p_tile_data->p_impls[p_ctrl->global_id].type        = CTRL_TYPE_CUDA;
	p_tile_data->p_impls[p_ctrl->global_id].tile.p_cuda = p_tile_data_impl_cuda;

	p_tile_data_impl_cuda->p_ctrl  = p_ctrl;
	p_tile_data_impl_cuda->pitch   = 0;
	p_tile_data_impl_cuda->texture = 0;

	Ctrl_Tile_List *p_list_node = (Ctrl_Tile_List *)malloc(sizeof(Ctrl_Tile_List));

	p_list_node->p_prev                = NULL;
	p_list_node->p_next                = NULL;
	p_list_node->p_tile_ext            = p_tile_data;
	p_tile_data_impl_cuda->p_tile_elem = p_list_node;

	if (p_ctrl->p_tile_list_tail != NULL) {
		p_ctrl->p_tile_list_tail->p_next = p_list_node;
		p_list_node->p_prev              = p_ctrl->p_tile_list_tail;
		p_ctrl->p_tile_list_tail         = p_list_node;
	} else {
		p_ctrl->p_tile_list_head = p_ctrl->p_tile_list_tail = p_list_node;
	}

	// TODO @sergioalo these creates are redundant, could create one of each type and then call retain as in ocl
	p_tile_data_impl_cuda->host_last_kernel_read_event  = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, p_ctrl->global_id);
	p_tile_data_impl_cuda->host_last_kernel_write_event = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, p_ctrl->global_id);
	p_tile_data_impl_cuda->host_last_dth_event          = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, p_ctrl->global_id);
	p_tile_data_impl_cuda->host_last_htd_event          = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, p_ctrl->global_id);
	p_tile_data_impl_cuda->dev_last_kernel_read_event   = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);
	p_tile_data_impl_cuda->dev_last_kernel_write_event  = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);
	p_tile_data_impl_cuda->dev_last_dth_event           = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);
	p_tile_data_impl_cuda->dev_last_htd_event           = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);
}

void Ctrl_Cuda_EvalTaskMoveToInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile) {
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;

	// Wait for appropiate events
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	for (int i = 0; i < Ctrl_GetNCtrls(); i++) {
		Ctrl_MoveToWait(&p_tile_data->p_impls[i], p_ctrl->p_htd_host_stream);
	}

	Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_kernel_read_event, p_ctrl->p_htd_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_kernel_write_event, p_ctrl->p_htd_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data->last_host_write_event, p_ctrl->p_htd_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_kernel_read_event, p_ctrl->p_htd_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_kernel_write_event, p_ctrl->p_htd_host_stream);

	// Wait for previous task to finish if policy is sync
	Ctrl_SyncWait(p_ctrl->p_htd_host_stream);

	Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_htd_event);
	p_tile_data_cuda->dev_last_htd_event = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);

	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_MOVETO;
	task.event     = p_tile_data_cuda->dev_last_htd_event;
	task.tile      = *p_tile;
	Ctrl_GenericEvent_Retain(task.event);
	Ctrl_TaskQueue_Push(p_ctrl->p_htd_host_stream, task);
	Ctrl_CpuEvent_Record(&p_tile_data_cuda->host_last_htd_event.event.event_cpu, p_ctrl->p_htd_host_stream);

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->host_seq_event.event.event_cpu, p_ctrl->p_dth_host_stream);
		Ctrl_GenericEvent_Release(p_ctrl->dev_seq_event);
		p_ctrl->dev_seq_event = p_tile_data_cuda->dev_last_htd_event;
		Ctrl_GenericEvent_Retain(p_ctrl->dev_seq_event);
	}

	// Update state of the tile
	p_tile_data->host_status        = CTRL_TILE_VALID;
	p_tile_data_impl->device_status = CTRL_TILE_VALID;
}

void Ctrl_Cuda_MoveToWait(Ctrl_Cuda_Tile *p_tile_data, Ctrl_TaskQueue *p_queue) {
	Ctrl_GenericEvent_StreamWait(p_tile_data->host_last_dth_event, p_queue);
	Ctrl_GenericEvent_StreamWait(p_tile_data->dev_last_dth_event, p_queue);
}

void Ctrl_Cuda_ExecTaskMoveTo(Ctrl_Task *p_task, Ctrl_Cuda *p_ctrl) {
	HitTile        *p_tile           = &p_task->tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)p_tile->ext;
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data->p_impls[p_ctrl->global_id].tile.p_cuda;

	size_t pitch = p_tile_data_cuda->pitch == 0 ? (p_tile->baseExtent) * p_tile->origAcumCard[1] : p_tile_data_cuda->pitch;
	/* @arturo TODO: STRIDED TILES */

	// Send memcpy to cuda stream
	/* TILES WITH THEIR OWN MEMORY ALLOCATION, OR CONTIGUOUS 1D TILES NEED ONLY ONE CONTIGUOUS COPY */
	if (p_tile->shape.info.sig.numDims == 1) {
		CUDA_OP(
			cudaMemcpyAsync(p_tile_data_cuda->p_device_data,
							p_tile->data,
							((size_t)(p_tile->acumCard)) * (p_tile->baseExtent),
							cudaMemcpyHostToDevice,
							p_ctrl->htd_driver_stream));
	}
	/* 2D TILES */
	else if (p_tile->shape.info.sig.numDims == 2) {
		CUDA_OP(
			cudaMemcpy2DAsync(p_tile_data_cuda->p_device_data,
							  pitch,
							  p_tile->data,
							  (p_tile->baseExtent) * p_tile->origAcumCard[1],
							  (p_tile->baseExtent) * p_tile->card[1],
							  p_tile->card[0],
							  cudaMemcpyHostToDevice,
							  p_ctrl->htd_driver_stream));
	}
	/* 3D TILES */
	else if (p_tile->shape.info.sig.numDims == 3) {
		struct cudaMemcpy3DParms params = {0};

		params.srcPtr = make_cudaPitchedPtr(p_tile->data,
											(p_tile->baseExtent) * p_tile->origAcumCard[2],
											p_tile->origAcumCard[2],
											p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.dstPtr = make_cudaPitchedPtr(p_tile_data_cuda->p_device_data,
											(p_tile->baseExtent) * p_tile->origAcumCard[2],
											p_tile->origAcumCard[2],
											p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.extent = make_cudaExtent(p_tile->card[2] * p_tile->baseExtent, p_tile->card[1], p_tile->card[0]);
		params.kind   = cudaMemcpyHostToDevice;

		CUDA_OP(cudaMemcpy3DAsync(&params, p_ctrl->htd_driver_stream));
	} else {
		fprintf(stderr, "Internal Error: Number of dimensions not supported for non-owner tile in MoveTo: %d\n",
				p_tile->shape.info.sig.numDims);
	}
}

void Ctrl_Cuda_EvalTaskMoveFromInner(Ctrl_Cuda *p_ctrl, HitTile *p_tile) {
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;

	// Wait for appropiate events
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	for (int i = 0; i < Ctrl_GetNCtrls(); i++) {
		Ctrl_MoveFromWait(&p_tile_data->p_impls[i], p_ctrl->p_dth_host_stream);
	}

	Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_kernel_write_event, p_ctrl->p_dth_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data->last_host_read_event, p_ctrl->p_dth_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data->last_host_write_event, p_ctrl->p_dth_host_stream);
	Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_kernel_write_event, p_ctrl->p_dth_host_stream);

	// wait for previous task to finish if policy is sync
	Ctrl_SyncWait(p_ctrl->p_dth_host_stream);

	Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_dth_event);
	p_tile_data_cuda->dev_last_dth_event = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);

	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_MOVEFROM;
	task.event     = p_tile_data_cuda->dev_last_dth_event;
	task.tile      = *p_tile;
	Ctrl_GenericEvent_Retain(task.event);
	Ctrl_TaskQueue_Push(p_ctrl->p_dth_host_stream, task);
	Ctrl_CpuEvent_Record(&p_tile_data_cuda->host_last_dth_event.event.event_cpu, p_ctrl->p_dth_host_stream);

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->host_seq_event.event.event_cpu, p_ctrl->p_htd_host_stream);
		Ctrl_GenericEvent_Release(p_ctrl->dev_seq_event);
		p_ctrl->dev_seq_event = p_tile_data_cuda->dev_last_htd_event;
		Ctrl_GenericEvent_Retain(p_ctrl->dev_seq_event);
	}

	// Update state of the tile
	p_tile_data->host_status        = CTRL_TILE_VALID;
	p_tile_data_impl->device_status = CTRL_TILE_VALID;
}

void Ctrl_Cuda_MoveFromWait(Ctrl_Cuda_Tile *p_tile_data, Ctrl_TaskQueue *p_queue) {
	// TODO @sergioalo does this need to wait for dth transfers?
	Ctrl_GenericEvent_StreamWait(p_tile_data->host_last_htd_event, p_queue);
	Ctrl_GenericEvent_StreamWait(p_tile_data->dev_last_htd_event, p_queue);
}

void Ctrl_Cuda_ExecTaskMoveFrom(Ctrl_Task *p_task, Ctrl_Cuda *p_ctrl) {
	HitTile        *p_tile           = &p_task->tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)p_tile->ext;
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data->p_impls[p_ctrl->global_id].tile.p_cuda;

	size_t pitch = p_tile_data_cuda->pitch == 0 ? (p_tile->baseExtent) * p_tile->origAcumCard[1] : p_tile_data_cuda->pitch;

	/* @arturo TODO: STRIDED TILES */
	// Send memcpy to cuda stream
	/* TILES WITH THEIR OWN MEMORY ALLOCATION, OR CONTIGUOUS 1D TILES NEED ONLY ONE CONTIGUOUS COPY */
	if (p_tile->shape.info.sig.numDims == 1) {
		CUDA_OP(
			cudaMemcpyAsync(p_tile->data,
							p_tile_data_cuda->p_device_data,
							((size_t)(p_tile->acumCard)) * (p_tile->baseExtent),
							cudaMemcpyDeviceToHost,
							p_ctrl->dth_driver_stream));
	}
	/* CONTIGUOUS 2D TILES */
	else if (p_tile->shape.info.sig.numDims == 2) {
		CUDA_OP(
			cudaMemcpy2DAsync(p_tile->data,                                   // dst
							  (p_tile->baseExtent) * p_tile->origAcumCard[1], // dpitch
							  p_tile_data_cuda->p_device_data,                // src
							  pitch,                                          // spitch
							  (p_tile->baseExtent) * p_tile->card[1],         // width
							  p_tile->card[0],                                // height
							  cudaMemcpyDeviceToHost,
							  p_ctrl->dth_driver_stream));
	}
	/* CONTIGUOUS 3D TILES */
	else if (p_tile->shape.info.sig.numDims == 3) {
		struct cudaMemcpy3DParms params = {0};

		params.srcPtr = make_cudaPitchedPtr(p_tile_data_cuda->p_device_data,
											(p_tile->baseExtent) * p_tile->origAcumCard[2],
											p_tile->origAcumCard[2],
											p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.dstPtr = make_cudaPitchedPtr(p_tile->data,
											(p_tile->baseExtent) * p_tile->origAcumCard[2],
											p_tile->origAcumCard[2],
											p_tile->origAcumCard[1] / p_tile->origAcumCard[2]);
		params.extent = make_cudaExtent(p_tile->card[2] * p_tile->baseExtent, p_tile->card[1], p_tile->card[0]);
		params.kind   = cudaMemcpyDeviceToHost;

		CUDA_OP(cudaMemcpy3DAsync(&params, p_ctrl->dth_driver_stream));
	} else {
		fprintf(stderr, "Internal Error: Number of dimensions not supported for non-owner tile in MoveFrom: %d\n",
				p_tile->shape.info.sig.numDims);
	}
}

/**********************************
 ** TASKS'S EVALUATION FUNCTIONS **
 **********************************/

void Ctrl_Cuda_Destroy(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	if (p_ctrl->p_tile_list_head != NULL) {
		fprintf(stderr, "Warning: Tiles left attached to ctrl %d\n", p_ctrl->global_id);
		fflush(stderr);
	}

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	// send destroy signal to queue manager thread
	if (p_task->flags) {
		Ctrl_Task task = CTRL_TASK_NULL;
		task.task_type = CTRL_TASK_TYPE_DESTROYCTRL;
		Ctrl_TaskQueue_Push(p_ctrl->p_htd_host_stream, task);
	}

	for (int i = 0; i < p_ctrl->n_kernel_streams; i++) {
		CUDA_OP(cudaStreamDestroy(p_ctrl->p_kernel_driver_streams[i]));
	}
	CUDA_OP(cudaStreamDestroy(p_ctrl->htd_driver_stream));
	CUDA_OP(cudaStreamDestroy(p_ctrl->dth_driver_stream));

	Ctrl_GenericEvent_Release(p_ctrl->host_seq_event);
	Ctrl_GenericEvent_Release(p_ctrl->dev_seq_event);

	free(p_ctrl->p_stream_op_count);
	free(p_ctrl->p_stream_indexes);
	free(p_ctrl->p_kernel_driver_streams);
	free(p_ctrl->pp_kernel_host_streams);

	#ifdef _CTRL_CUBLAS_
	cublasDestroy(p_ctrl->cublas_handle);
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	magma_queue_destroy(p_ctrl->magma_queue);
	magma_finalize();
	#endif // _CTRL_MAGMA_
}

void Ctrl_Cuda_EvalTaskGlobalSync(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	for (Ctrl_Tile_List *p_aux = p_ctrl->p_tile_list_head; p_aux != NULL; p_aux = p_aux->p_next) {
		Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_aux->p_tile_ext);
		Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data->p_impls[p_ctrl->global_id].tile.p_cuda;

		Ctrl_GenericEvent_Wait(p_tile_data->last_host_read_event);
		Ctrl_GenericEvent_Wait(p_tile_data->last_host_write_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_kernel_read_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_kernel_write_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_dth_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_htd_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_kernel_read_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_kernel_write_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_dth_event);
		Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_htd_event);
	}
}

void Ctrl_Cuda_EvalTaskKernelLaunch(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	// Check if the specified stream exists:
	if (p_task->stream < 0 || p_task->stream >= p_ctrl->n_kernel_streams) {
		fprintf(stderr, "[Ctrl_Cuda] Internal Error: Tried to execute a task on a nonexistent stream: %d", p_task->stream);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	Ctrl_TaskQueue *p_host_kernel_queue = p_ctrl->pp_kernel_host_streams[p_task->stream];

	// wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile        *p_tile           = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
			Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];
			Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;
			KHitTile       *p_ktile          = (KHitTile *)(p_task->p_arguments + p_task->p_displacements[i]);

			if (hit_tileIsNull(*p_tile)) {
				fprintf(stderr, "Warning: Launching task %s, skipping null tile on parameter %d (starting at 0)\n", p_task->p_func_name, i);
				fflush(stderr);
				continue;
			}

			if (p_tile_data_impl->device_status == CTRL_TILE_UNALLOC) {
				fprintf(stderr, "[Ctrl_Cuda] Internal Error: Launching kernel %s with a tile with no device memory as parameter %d (starting at 0)\n", p_task->p_func_name, i);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}

			// TODO @sergioalo care for other dimensions
			if (p_tile_data_cuda->pitch != 0) {
				p_ktile->origAcumCard[1] = p_tile_data_cuda->pitch / p_tile->baseExtent;
			}

			// if tile's role is IN or IO, is not updated on device and host has memory allocated transfer it
			if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data_impl->device_status == CTRL_TILE_INVALID &&
				p_tile_data->host_status != CTRL_TILE_UNALLOC && p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
				if (p_tile_data->host_status != CTRL_TILE_VALID) {
					for (int j = 0; j < Ctrl_GetNCtrls(); j++) {
						Ctrl_Tile_Impl *p_tile_impl_j = &p_tile_data->p_impls[j];
						if (p_tile_impl_j->device_status == CTRL_TILE_VALID) {
							// TODO @sergioalo if tiles had ptr to associated abstract ctrl normal movefrom could be used and this switch removed
							switch (p_tile_impl_j->type) {
								#ifdef _CTRL_ARCH_CPU_
								case CTRL_TYPE_CPU:
									Ctrl_Cpu_EvalTaskMoveFromInner(p_tile_impl_j->tile.p_cpu->p_ctrl, p_tile);
									break;
								#endif // _CTRL_ARCH_CPU_

								#ifdef _CTRL_ARCH_CUDA_
								case CTRL_TYPE_CUDA:
									Ctrl_Cuda_EvalTaskMoveFromInner(p_tile_impl_j->tile.p_cuda->p_ctrl, p_tile);
									break;
								#endif // _CTRL_ARCH_CUDA_

								#ifdef _CTRL_ARCH_HIP_
								case CTRL_TYPE_HIP:
									Ctrl_Hip_EvalTaskMoveFromInner(p_tile_impl_j->tile.p_hip->p_ctrl, p_tile);
									break;
								#endif // _CTRL_ARCH_HIP_

								#ifdef _CTRL_ARCH_OPENCL_GPU_
								case CTRL_TYPE_OPENCL_GPU:
									Ctrl_OpenCLGpu_EvalTaskMoveFromInner(p_tile_impl_j->tile.p_opencl->p_ctrl, p_tile);
									break;
								#endif // _CTRL_ARCH_OPENCL_GPU_

								#ifdef _CTRL_ARCH_FPGA_
								case CTRL_TYPE_FPGA:
									Ctrl_FPGA_EvalTaskMoveFromInner(p_tile_impl_j->tile.p_fpga->p_ctrl, p_tile);
									break;
								#endif // _CTRL_ARCH_FPGA_
								default:
									break;
							}
							break;
						}
					}
					if (p_tile_data->host_status == CTRL_TILE_INVALID) {
						fprintf(stderr, "[Ctrl_Cuda] Warning: Tile with uninitialized data as input on kernel %s, parameter: %d (starting at 0)\n", p_task->p_func_name, i);
						fflush(stderr);
					}
				}
				Ctrl_Cuda_EvalTaskMoveToInner(p_ctrl, p_tile);
			}

			if (p_tile_data_impl->device_status == CTRL_TILE_INVALID && p_task->p_roles[i] != KERNEL_OUT) {
				fprintf(stderr, "[Ctrl_Cuda] Warning: Tile with uninitialized data as input on kernel %s, parameter: %d (starting at 0)\n", p_task->p_func_name, i);
				fflush(stderr);
			}

			if (p_ctrl->n_kernel_streams != 1) { // no need to wait for other kernels of this dev if we only have 1 driver kernel stream
				Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_kernel_write_event, p_host_kernel_queue);
				Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_kernel_write_event, p_host_kernel_queue);
			}

			if (p_tile_data->host_status != CTRL_TILE_UNALLOC && !(p_tile_data_impl->device_status == CTRL_TILE_VALID && p_tile_data->host_status == CTRL_TILE_INVALID)) {
				Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_htd_event, p_host_kernel_queue);
				Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_htd_event, p_host_kernel_queue);
			}

			if (p_task->p_roles[i] != KERNEL_IN) {
				if (p_ctrl->n_kernel_streams != 1) { // no need to wait for other kernels of this dev if we only have 1 driver kernel stream
					Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_kernel_read_event, p_host_kernel_queue);
					Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_kernel_read_event, p_host_kernel_queue);
				}

				if (p_tile_data->host_status != CTRL_TILE_UNALLOC && !(p_tile_data_impl->device_status == CTRL_TILE_VALID && p_tile_data->host_status == CTRL_TILE_INVALID)) {
					Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->host_last_dth_event, p_host_kernel_queue);
					Ctrl_GenericEvent_StreamWait(p_tile_data_cuda->dev_last_dth_event, p_host_kernel_queue);
				}
			}
		}
	}

	// wait for previous task to finish if policy is sync
	Ctrl_SyncWait(p_host_kernel_queue);

	// create request with info for kernel execution (stream is obtained in exec_task, no need to pass it here)
	Ctrl_Request request = {0};

	#ifdef _CTRL_CUBLAS_
	request.cuda.p_cublas_handle = &(p_ctrl->cublas_handle);
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	request.cuda.p_magma_queue = &(p_ctrl->magma_queue);
	#endif // _CTRL_MAGMA_

	Ctrl_GenericEvent kernel_event = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CUDA, p_ctrl->global_id);

	p_task->request = request;
	p_task->event   = kernel_event;
	Ctrl_GenericEvent_Retain(p_task->event);
	Ctrl_TaskQueue_Push(p_host_kernel_queue, *p_task);

	// update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile        *p_tile           = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
			Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];
			Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;

			if (p_task->p_roles[i] != KERNEL_IN) {
				if (p_tile_data->host_status == CTRL_TILE_VALID) p_tile_data->host_status = CTRL_TILE_INVALID;
				// invalidate tile on all other devs except this
				for (int j = 0; j < Ctrl_GetNCtrls(); j++) {
					Ctrl_Tile_Impl *p_tile_data_impl_j = &p_tile_data->p_impls[j];
					if (p_tile_data_impl_j->type == CTRL_TYPE_NULL) {
						continue;
					}
					if (p_tile_data_impl_j->device_status == CTRL_TILE_VALID) p_tile_data_impl_j->device_status = CTRL_TILE_INVALID;
				}

				p_tile_data_impl->device_status = CTRL_TILE_VALID;
				Ctrl_CpuEvent_Record(&p_tile_data_cuda->host_last_kernel_write_event.event.event_cpu, p_host_kernel_queue);
				Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_kernel_write_event);
				p_tile_data_cuda->dev_last_kernel_write_event = kernel_event;
				Ctrl_GenericEvent_Retain(p_tile_data_cuda->dev_last_kernel_write_event);
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				Ctrl_CpuEvent_Record(&p_tile_data_cuda->host_last_kernel_read_event.event.event_cpu, p_host_kernel_queue);
				Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_kernel_read_event);
				p_tile_data_cuda->dev_last_kernel_read_event = kernel_event;
				Ctrl_GenericEvent_Retain(p_tile_data_cuda->dev_last_kernel_read_event);
			}
		}
	}

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->host_seq_event.event.event_cpu, p_host_kernel_queue);
		Ctrl_GenericEvent_Release(p_ctrl->dev_seq_event);
		p_ctrl->dev_seq_event = kernel_event;
		Ctrl_GenericEvent_Retain(p_ctrl->dev_seq_event);
	}
	Ctrl_GenericEvent_Release(kernel_event);
}

void Ctrl_Cuda_HostTaskWait(Ctrl_Cuda_Tile *p_tile, char rol, Ctrl_TaskQueue *p_queue) {
	Ctrl_GenericEvent_StreamWait(p_tile->host_last_dth_event, p_queue);
	Ctrl_GenericEvent_StreamWait(p_tile->dev_last_dth_event, p_queue);

	if (rol != KERNEL_IN) {
		Ctrl_GenericEvent_StreamWait(p_tile->host_last_htd_event, p_queue);
		Ctrl_GenericEvent_StreamWait(p_tile->dev_last_htd_event, p_queue);
	}
}

void Ctrl_Cuda_SyncWait(Ctrl_Cuda *p_ctrl, Ctrl_TaskQueue *p_queue) {
	Ctrl_GenericEvent_StreamWait(p_ctrl->host_seq_event, p_queue);
	Ctrl_GenericEvent_StreamWait(p_ctrl->dev_seq_event, p_queue);
}

void Ctrl_Cuda_AllocHost(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile   *p_tile      = p_task->p_tile;
	Ctrl_Tile *p_tile_data = (Ctrl_Tile *)(p_tile->ext);

	// TODO @sergioalo pinned for multiple devices?
	if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
		if (p_task->flags & CTRL_MEM_ALLOC_HOST || !(p_task->flags & CTRL_MEM_ALLOC_DEV)) {
			if ((p_task->flags & CTRL_MEM_PINNED) || (!(p_task->flags & CTRL_MEM_NOPINNED) && p_ctrl->default_alloc_mode == CTRL_MEM_PINNED)) {
				// Allocate host "pinned" memory
				p_tile_data->pinned = CTRL_TYPE_CUDA;
				CUDA_OP(cudaHostAlloc(&(p_tile->data), ((size_t)(p_tile->origAcumCard[0])) * (p_tile->baseExtent), cudaHostAllocPortable));
			} else {
				// Allocate host memory the usual way (on CUDA this is probably never a good idea to do, but the option is there)
				p_tile_data->pinned = CTRL_TYPE_NULL;
				p_tile->data        = (void *)malloc((size_t)p_tile->acumCard * p_tile->baseExtent);
			}
			p_tile_data->host_status = CTRL_TILE_INVALID;
			p_tile->memPtr           = p_tile->data;
		}
	}
}

void Ctrl_Cuda_EvalTaskAllocTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	CUDA_OP(cudaSetDevice(p_ctrl->device));

	HitTile        *p_tile           = p_task->p_tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];

	if (p_tile_data_impl->type == CTRL_TYPE_NULL)
		Ctrl_Cuda_InitTile(p_ctrl, p_task);

	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;
	Ctrl_Cuda_AllocHost(p_ctrl, p_task);

	if (p_task->flags & CTRL_MEM_ALLOC_DEV || !(p_task->flags & CTRL_MEM_ALLOC_HOST)) {
		if (p_tile_data_impl->device_status != CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cuda] Warning: Device memory already allocated for this tile, ignoring this call.\n");
			fflush(stderr);
			return;
		}
		// Allocate device memory
		if (p_task->flags & CTRL_MEM_ALIGNED && hit_tileDims(*p_tile) == 2) {
			CUDA_OP(cudaMallocPitch(&(p_tile_data_cuda->p_device_data), &p_tile_data_cuda->pitch, (p_tile->baseExtent) * p_tile->card[1], p_tile->card[0]));
		} else {
			CUDA_OP(cudaMalloc(&(p_tile_data_cuda->p_device_data), ((size_t)(p_tile->origAcumCard[0])) * (p_tile->baseExtent)));
		}
		p_tile_data_impl->device_status = CTRL_TILE_INVALID;
	}

	// TODO Habria que poner qstride, origAcumCard, y memStatus en p_tile_data con los dev (al tener dos espacios de memoria)
}

void Ctrl_Cuda_EvalTaskSelectTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = p_task->p_tile;

	CUDA_OP(cudaSetDevice(p_ctrl->device));
	Ctrl_Cuda_InitTile(p_ctrl, p_task);

	HitTile        *p_parent           = p_tile->ref;
	Ctrl_Tile      *p_tile_data        = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl   = &p_tile_data->p_impls[p_ctrl->global_id];
	Ctrl_Cuda_Tile *p_tile_data_cuda   = p_tile_data_impl->tile.p_cuda;
	Ctrl_Tile      *p_parent_data      = ((Ctrl_Tile *)(p_tile->ref->ext));
	Ctrl_Tile_Impl *p_parent_data_impl = &p_parent_data->p_impls[p_ctrl->global_id];
	Ctrl_Cuda_Tile *p_parent_data_cuda = p_parent_data_impl->tile.p_cuda;

	if (p_tile->memStatus == HIT_MS_NOT_OWNER) {
		p_tile_data_impl->device_status = p_parent_data_impl->device_status;
		p_tile_data_cuda->p_device_data = p_parent_data_cuda->p_device_data + (p_tile->data - p_parent->data);

		if (p_parent_data_cuda->pitch != 0) {
			fprintf(stderr, "[Ctrl_Cuda_EvalTaskSelectTile] Error: subselections of tiles with padding on the device (allocated with CTRL_MEM_ALIGNED) not supported.\n");
			exit(EXIT_FAILURE);
		}
	}
}

void Ctrl_Cuda_EvalTaskFreeTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile        *p_tile           = p_task->p_tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data_impl->tile.p_cuda;

	// tile not initialized
	if (p_tile_data_impl->type == CTRL_TYPE_NULL) {
		fprintf(stderr, "[Ctrl_Cuda] Warning: Free from tile not attached to ctrl.\n");
		return;
	}

	p_tile_data->valid_impls--;

	// TODO @sergioalo if host stuff is not freed, should we wait for host task events?
	// Wait for all work related to this tile to finish
	Ctrl_GenericEvent_Wait(p_tile_data->last_host_read_event);
	Ctrl_GenericEvent_Wait(p_tile_data->last_host_write_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_kernel_read_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_kernel_write_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_dth_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_htd_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_kernel_read_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_kernel_write_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_dth_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_htd_event);

	// destroy events inside the tile
	Ctrl_GenericEvent_Release(p_tile_data_cuda->host_last_kernel_read_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->host_last_kernel_write_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->host_last_dth_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->host_last_htd_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_kernel_read_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_kernel_write_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_dth_event);
	Ctrl_GenericEvent_Release(p_tile_data_cuda->dev_last_htd_event);

	// Remove tle from tile linked list
	if (p_tile_data_cuda->p_tile_elem->p_prev != NULL) {
		p_tile_data_cuda->p_tile_elem->p_prev->p_next = p_tile_data_cuda->p_tile_elem->p_next;
	} else {
		p_ctrl->p_tile_list_head = p_tile_data_cuda->p_tile_elem->p_next;
	}

	if (p_tile_data_cuda->p_tile_elem->p_next != NULL) {
		p_tile_data_cuda->p_tile_elem->p_next->p_prev = p_tile_data_cuda->p_tile_elem->p_prev;
	} else {
		p_ctrl->p_tile_list_tail = p_tile_data_cuda->p_tile_elem->p_prev;
	}

	// Clear node fields
	p_tile_data_cuda->p_tile_elem->p_tile_ext = NULL;
	p_tile_data_cuda->p_tile_elem->p_next     = NULL;
	p_tile_data_cuda->p_tile_elem->p_prev     = NULL;

	// Free node
	free(p_tile_data_cuda->p_tile_elem);

	if (p_tile->memStatus == HIT_MS_OWNER) {
		if (p_tile_data_impl->device_status != CTRL_TILE_UNALLOC) {
			// Free device image of the tile
			CUDA_OP(cudaFree(p_tile_data_cuda->p_device_data));
			if (p_tile_data_cuda->texture != 0) {
				cudaDestroyTextureObject(p_tile_data_cuda->texture);
			}
		}
	}

	// Clear tile fields
	p_tile_data_cuda->p_ctrl = NULL;

	// Free tile
	free(p_tile_data_cuda);

	// if this was the last ctrl the tile was attached to free the host stuff as well
	if (p_tile_data->valid_impls == 0) {
		Ctrl_FreeHostInner(p_tile);
	}
}

void Ctrl_Cuda_EvalTaskMoveTo(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile        *p_tile           = p_task->p_tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];

	if (hit_tileIsNull(*p_tile)) return;

	if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Cuda] Internal Error: Trying to move tile from host to device but host memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data_impl->device_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Cuda] Internal Error: Trying to move tile from host to device but device memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data->host_status == CTRL_TILE_INVALID) {
		fprintf(stderr, "[Ctrl_Cuda] Warning: Moving a tile from host to device with invalid data on host memory\n");
		fflush(stderr);
	}

	// If tile is not updated perform the transfer
	if (!(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data_impl->device_status == CTRL_TILE_VALID) || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_Cuda_EvalTaskMoveToInner(p_ctrl, p_tile);
	}
}

void Ctrl_Cuda_EvalTaskMoveFrom(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile        *p_tile           = p_task->p_tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[p_ctrl->global_id];

	if (hit_tileIsNull(*p_tile)) return;

	if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Cuda] Internal Error: Trying to move tile from device to host but host memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data_impl->device_status == CTRL_TILE_UNALLOC) {
		fprintf(stderr, "[Ctrl_Cuda] Internal Error: Trying to move tile from device to host but device memory was not allocated\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (p_tile_data_impl->device_status == CTRL_TILE_INVALID) {
		fprintf(stderr, "[Ctrl_Cuda] Warning: Moving a tile from device to host with invalid data on device memory\n");
		fflush(stderr);
	}

	// If tile is not updated perform the transfer
	if (!(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data_impl->device_status == CTRL_TILE_VALID) || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_Cuda_EvalTaskMoveFromInner(p_ctrl, p_tile);
	}
}

void Ctrl_Cuda_EvalTaskWaitTile(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	HitTile        *p_tile           = p_task->p_tile;
	Ctrl_Tile      *p_tile_data      = (Ctrl_Tile *)(p_tile->ext);
	Ctrl_Cuda_Tile *p_tile_data_cuda = p_tile_data->p_impls[p_ctrl->global_id].tile.p_cuda;

	if (hit_tileIsNull(*p_tile)) return;

	// Wait for all work related to this tile to finish
	Ctrl_GenericEvent_Wait(p_tile_data->last_host_read_event);
	Ctrl_GenericEvent_Wait(p_tile_data->last_host_write_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_kernel_read_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_kernel_write_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_dth_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->host_last_htd_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_kernel_read_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_kernel_write_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_dth_event);
	Ctrl_GenericEvent_Wait(p_tile_data_cuda->dev_last_htd_event);
}

void Ctrl_Cuda_EvalTaskSetDependanceMode(Ctrl_Cuda *p_ctrl, Ctrl_Task *p_task) {
	p_ctrl->dependance_mode = p_task->flags;
}

///@endcond
