///@cond INTERNAL
/**
 * @file Ctrl_Cpu.c
 * @author Trasgo Group
 * @brief Source code for Cpu backend.
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

#include "Architectures/Cpu/Ctrl_Cpu.h"

/*************************************************************
 ******** Prototypes of private functions ********************
 *************************************************************/

/**
 * Sync with main thread if queues are enabled, if queues are not enabled this function does nothing.
 *
 * @param p_ctrl Pointer to the ctrl to perform the syncronization.
 */
void Ctrl_Cpu_Sync(Ctrl_Cpu *p_ctrl);

/**
 * Allocate memory for a new \e Ctrl_Cpu_Tile.
 *
 * @param p_ctrl Pointer to the ctrl to be attached to the tile.
 * @param p_task Pointer to the task containing the hitTile that will contain the new \e Ctrl_Cpu_Tile.
 *
 * @see Ctrl_Cpu_EvalTaskAllocTile, Ctrl_Cpu_Tile
 */
void Ctrl_Cpu_CreateTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Initializate a \e Ctrl_Cpu_Tile.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile and to be updated of the initialization.
 * @param p_task Pointer to the task containing the HitTile that contains the \e Ctrl_Cpu_Tile to be.
 * initialized.
 */
void Ctrl_Cpu_InitTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Perform memory transfer from host to device.
 *
 * Pushes appropiate wait events and MoveTo task to the "move to" task queue.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 *
 * @pre Memory transfers must be enabled on this ctrl.
 * @see Ctrl_Cpu_EvalTaskMoveTo
 */
void Ctrl_Cpu_EvalTaskMoveToInner(Ctrl_Cpu *p_ctrl, HitTile *p_tile);

/**
 * Perform memory transfer from device to host.
 *
 * Pushes appropiate wait events and MoveFrom task to the "move from" task queue.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 *
 * @pre Memory transfers must be enabled on this ctrl.
 * @see Ctrl_Cpu_EvalTaskMoveFrom
 */
void Ctrl_Cpu_EvalTaskMoveFromInner(Ctrl_Cpu *p_ctrl, HitTile *p_tile);

/**
 * Extract and evaluate tasks from \p p_stream until a task of type CTRL_TASK_TYPE_DESTROYCNTRL is found.
 *
 * This function calls \e Ctrl_Cpu_EvalTaskInner for the evaluation of tasks. This function is used for cpu kernel queue
 * and cpu memory transfer queues.
 *
 * @param p_stream Stream to extract the tasks from.
 *
 * @see Ctrl_Cpu_EvalTaskInner, Ctrl_Cpu_ThreadInit
 */
void Ctrl_Cpu_StreamConsume(Ctrl_TaskQueue *p_stream);

/**
 * Evaluation for inner cpu tasks.
 *
 * Not to be confused with \e Ctrl_Cpu_EvalTask, this function evaluates \e inner tasks, meaning that this is used from
 * \e Ctrl_Cpu_StreamConsume for tasks in cpu kernel and cpu trasnsfers queues.
 *
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Cpu_StreamConsume
 */
void Ctrl_Cpu_EvalTaskInner(Ctrl_Task *p_task);

/*************************************************************
 ******** Prototypes of tasks' evaluation functions **********
 *************************************************************/

/**
 * Destroy a CPU ctrl.
 * @param p_ctrl Ctrl to be destroyed.
 */
void Ctrl_Cpu_Destroy(Ctrl_Cpu *p_ctrl);

/**
 * Evaluation of kernel launch.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Launch
 */
void Ctrl_Cpu_EvalTaskKernelLaunch(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of host task launch.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_HostTask
 */
void Ctrl_Cpu_EvalTaskHostTaskLaunch(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of global sync. Waits for all work related to any tile attached to this ctrl.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_GlobalSync
 */
void Ctrl_Cpu_EvalTaskGlobalSync(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of creation of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Domain
 */
void Ctrl_Cpu_EvalTaskDomainTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of allocation of tiles.
 *
 * In case mem moves are off allocation can only be made once, affinity of desired
 * allocation is respected.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Alloc
 */
void Ctrl_Cpu_EvalTaskAllocTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of subselecting tiles.
 *
 * @param p_ctrl: Ctrl in charge of task.
 * @param p_task: task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Select
 */
void Ctrl_Cpu_EvalTaskSelectTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);
/**
 * Evaluation of freeing of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Free
 */
void Ctrl_Cpu_EvalTaskFreeTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 *
 * This function checks state of tile and if transfers are enabled, if comunication is necesary it calls to
 * \e Ctrl_Cpu_EvalTaskMoveToInner.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Cpu_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_Cpu_EvalTaskMoveTo(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from device to host.
 *
 * This function checks state of tile and if transfers are enabled, if comunication is necesary it calls to
 * \e Ctrl_Cpu_EvalTaskMoveToInner.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Cpu_EvalTaskMoveFromInner, Ctrl_MoveFrom
 */
void Ctrl_Cpu_EvalTaskMoveFrom(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of wait. Waits for all the work related to the tile specified in \p p_task .
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_WaitTile
 */
void Ctrl_Cpu_EvalTaskWaitTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of change of dependance mode.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated. .
 *
 */
void Ctrl_Cpu_EvalTaskSetDependanceMode(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task);

/********************************************
 ******** CPU Controller functions **********
 ********************************************/

void Ctrl_Cpu_Create(Ctrl_Cpu *p_ctrl, Ctrl_Policy policy, int n_cores, int *p_numa_nodes, int n_numa_nodes, bool mem_moves) {
	p_ctrl->policy           = policy;
	p_ctrl->n_cores          = n_cores;
	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;
	p_ctrl->dependance_mode  = CTRL_MODE_IMPLICIT;

	// Set the cpuset (for kernel bind) to the ORed cpusets of the NUMA nodes selected by the user
	p_ctrl->device_cpuset = hwloc_bitmap_alloc();
	if (n_numa_nodes > 0) {
		hwloc_obj_t obj;
		for (int i = 0; i < n_numa_nodes; i++) {
			obj = hwloc_get_obj_by_type(p_ctrl->topo, HWLOC_OBJ_NUMANODE, p_numa_nodes[i]);
			if (!obj) {
				fprintf(stderr, "[Ctrl_Cpu] warning: Numanode %d not found, ignoring it\n", p_numa_nodes[i]);
				break;
			}
			hwloc_bitmap_or(p_ctrl->device_cpuset, p_ctrl->device_cpuset, obj->cpuset);
		}
	}

	p_ctrl->mem_moves = mem_moves;
	p_ctrl->event_seq = Ctrl_CpuEvent_Create();

	// Alloc queues for kernel and memory transfers
	p_ctrl->p_kernel_stream = (Ctrl_TaskQueue *)malloc(sizeof(Ctrl_TaskQueue));
	Ctrl_TaskQueue_Init(p_ctrl->p_kernel_stream);

	if (p_ctrl->mem_moves) {
		p_ctrl->p_moveTo_stream   = (Ctrl_TaskQueue *)malloc(sizeof(Ctrl_TaskQueue));
		p_ctrl->p_moveFrom_stream = (Ctrl_TaskQueue *)malloc(sizeof(Ctrl_TaskQueue));
		Ctrl_TaskQueue_Init(p_ctrl->p_moveTo_stream);
		Ctrl_TaskQueue_Init(p_ctrl->p_moveFrom_stream);
	}
}

void Ctrl_Cpu_EvalTask(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_KERNEL:
			Ctrl_Cpu_EvalTaskKernelLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_HOST:
			Ctrl_Cpu_EvalTaskHostTaskLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_GLOBALSYNC:
			Ctrl_Cpu_EvalTaskGlobalSync(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_ALLOCTILE:
			Ctrl_Cpu_EvalTaskAllocTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DOMAINTILE:
			Ctrl_Cpu_EvalTaskDomainTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_SELECTTILE:
			Ctrl_Cpu_EvalTaskSelectTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_FREETILE:
			Ctrl_Cpu_EvalTaskFreeTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVETO:
			Ctrl_Cpu_EvalTaskMoveTo(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVEFROM:
			Ctrl_Cpu_EvalTaskMoveFrom(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_WAITTILE:
			Ctrl_Cpu_EvalTaskWaitTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DESTROYCNTRL:
			Ctrl_Cpu_Destroy(p_ctrl);
			break;
		case CTRL_TASK_TYPE_SETDEPENDANCEMODE:
			Ctrl_Cpu_EvalTaskSetDependanceMode(p_ctrl, p_task);
			break;
		default:
			fprintf(stderr, "[Ctrl_Cpu] Unsupported task type: %d.\n", p_task->task_type);
			exit(EXIT_FAILURE);
	}
}

int Ctrl_Cpu_GetNumThreads(Ctrl_Cpu *p_ctrl) {
	return p_ctrl->mem_moves ? 3 : 1;
}

void Ctrl_Cpu_ThreadInit(Ctrl_Cpu *p_ctrl, hwloc_topology_t topo, int node) {
	// index of the thread used to execute kernels
	#ifdef _CTRL_QUEUE_
	int kernel_thread = 1;
	#else // _CTRL_QUEUE_
	int kernel_thread = 0;
	#endif //_CTRL_QUEUE_

	if (omp_get_thread_num() == kernel_thread) { // kernel thread
		// bind thread
		if (!hwloc_bitmap_iszero(p_ctrl->device_cpuset)) {
			hwloc_set_cpubind(topo, p_ctrl->device_cpuset, HWLOC_CPUBIND_THREAD);
		}
		// start executing tasks from the queue
		Ctrl_Cpu_StreamConsume(p_ctrl->p_kernel_stream);

	} else if (omp_get_thread_num() == kernel_thread + 1) { // host to device
		// bind thread
		hwloc_obj_t obj = hwloc_get_obj_below_by_type(topo, HWLOC_OBJ_NUMANODE, node, HWLOC_OBJ_CORE, kernel_thread + 2);
		if (obj) {
			hwloc_set_cpubind(topo, obj->cpuset, HWLOC_CPUBIND_THREAD);
		}
		// start executing tasks from the queue
		Ctrl_Cpu_StreamConsume(p_ctrl->p_moveTo_stream);

	} else if (omp_get_thread_num() == kernel_thread + 2) { // device to host
		// bind thread
		hwloc_obj_t obj = hwloc_get_obj_below_by_type(topo, HWLOC_OBJ_NUMANODE, node, HWLOC_OBJ_CORE, kernel_thread + 3);
		if (obj) {
			hwloc_set_cpubind(topo, obj->cpuset, HWLOC_CPUBIND_THREAD);
		}
		// start executing tasks from the queue
		Ctrl_Cpu_StreamConsume(p_ctrl->p_moveFrom_stream);
	}
}

/*********************************
 ******* Private functions *******
 *********************************/

void Ctrl_Cpu_Sync(Ctrl_Cpu *p_ctrl) {
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

void Ctrl_Cpu_CreateTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)malloc(sizeof(Ctrl_Cpu_Tile));

	p_tile_data->p_ctrl = p_ctrl;

	p_tile_data->host_status   = CTRL_TILE_UNALLOC;
	p_tile_data->device_status = CTRL_TILE_UNALLOC;

	p_tile->ext = (void *)p_tile_data;

	p_tile_data->p_parent_ext = NULL;

	p_tile_data->is_initialized = false;
}

void Ctrl_Cpu_InitTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

	// create node for the the new tile
	Ctrl_Cpu_Tile_List *p_list_node = (Ctrl_Cpu_Tile_List *)malloc(sizeof(Ctrl_Cpu_Tile_List));

	p_list_node->p_prev      = NULL;
	p_list_node->p_next      = NULL;
	p_list_node->p_tile_ext  = p_tile_data;
	p_tile_data->p_tile_elem = p_list_node;

	// insert node into the linked list of tiles
	if (p_ctrl->p_tile_list_tail != NULL) {
		p_ctrl->p_tile_list_tail->p_next = p_list_node;
		p_list_node->p_prev              = p_ctrl->p_tile_list_tail;
		p_ctrl->p_tile_list_tail         = p_list_node;
	} else {
		p_ctrl->p_tile_list_head = p_ctrl->p_tile_list_tail = p_list_node;
	}

	p_tile_data->host_last_read_event  = Ctrl_CpuEvent_Create();
	p_tile_data->host_last_write_event = Ctrl_CpuEvent_Create();

	if (p_ctrl->mem_moves) {
		p_tile_data->kernel_last_read_event  = Ctrl_CpuEvent_Create();
		p_tile_data->kernel_last_write_event = Ctrl_CpuEvent_Create();

		p_tile_data->offloading_last_read_event  = Ctrl_CpuEvent_Create();
		p_tile_data->offloading_last_write_event = Ctrl_CpuEvent_Create();
	}

	p_tile_data->is_initialized = true;
}

void Ctrl_Cpu_EvalTaskMoveToInner(Ctrl_Cpu *p_ctrl, HitTile *p_tile) {
	Ctrl_Cpu_Tile    *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);
	Ctrl_GenericEvent move_event  = CTRL_GENERIC_EVENT_NULL;
	move_event.event_type         = CTRL_EVENT_TYPE_CPU;

	// Wait for appropiate events
	move_event.event.event_cpu = p_tile_data->kernel_last_read_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveTo_stream);

	move_event.event.event_cpu = p_tile_data->kernel_last_write_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveTo_stream);

	move_event.event.event_cpu = p_tile_data->offloading_last_read_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveTo_stream);

	move_event.event.event_cpu = p_tile_data->host_last_write_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveTo_stream);

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		move_event.event.event_cpu = p_ctrl->event_seq;
		Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveTo_stream);
	}

	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_MOVETO;
	task.p_tile    = p_tile;

	// Push moveFrom task to moveFrom execution queue
	Ctrl_TaskQueue_Push(p_ctrl->p_moveTo_stream, task);

	// Update events
	Ctrl_CpuEvent_Record(&p_tile_data->offloading_last_write_event, p_ctrl->p_moveTo_stream);

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->event_seq, p_ctrl->p_moveTo_stream);
	}

	// Update state of the tile
	p_tile_data->host_status   = CTRL_TILE_VALID;
	p_tile_data->device_status = CTRL_TILE_VALID;
}

void Ctrl_Cpu_EvalTaskMoveFromInner(Ctrl_Cpu *p_ctrl, HitTile *p_tile) {
	Ctrl_Cpu_Tile    *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);
	Ctrl_GenericEvent move_event  = CTRL_GENERIC_EVENT_NULL;
	move_event.event_type         = CTRL_EVENT_TYPE_CPU;

	// Wait for appropiate events
	move_event.event.event_cpu = p_tile_data->kernel_last_write_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveFrom_stream);

	move_event.event.event_cpu = p_tile_data->offloading_last_write_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveFrom_stream);

	move_event.event.event_cpu = p_tile_data->host_last_read_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveFrom_stream);

	move_event.event.event_cpu = p_tile_data->host_last_write_event;
	Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveFrom_stream);

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		move_event.event.event_cpu = p_ctrl->event_seq;
		Ctrl_GenericEvent_StreamWait(move_event, p_ctrl->p_moveFrom_stream);
	}

	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_MOVEFROM;
	task.p_tile    = p_tile;
	// Push moveFrom task to moveFrom execution queue
	Ctrl_TaskQueue_Push(p_ctrl->p_moveFrom_stream, task);

	// Update events
	Ctrl_CpuEvent_Record(&p_tile_data->offloading_last_read_event, p_ctrl->p_moveFrom_stream);

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->event_seq, p_ctrl->p_moveFrom_stream);
	}

	// Update state of the tile
	p_tile_data->host_status   = CTRL_TILE_VALID;
	p_tile_data->device_status = CTRL_TILE_VALID;
}

void Ctrl_Cpu_StreamConsume(Ctrl_TaskQueue *p_stream) {
	bool finish = false;
	while (!finish) {
		// Extract task from queue
		Ctrl_Task *p_task = Ctrl_TaskQueue_Pop(p_stream);

		if (p_task->task_type == CTRL_TASK_TYPE_DESTROYCNTRL) {
			finish = true;
		} else {
			// Evaluate task
			Ctrl_Cpu_EvalTaskInner(p_task);
			// Update task counter
			#pragma omp atomic update
			p_stream->last_finished++;
			// Free task (kernel tasks must be destroyed at the end)
			if (p_task->task_type != CTRL_TASK_TYPE_KERNEL) {
				Ctrl_TaskQueue_FreeTask(p_task);
			}
		}
	}
	Ctrl_TaskQueue_Destroy(p_stream);
	free(p_stream);
}

void Ctrl_Cpu_EvalTaskInner(Ctrl_Task *p_task) {
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_KERNEL:
			p_task->pfn_kernel_wrapper(p_task->request, p_task->device_id, p_task->ctrl_type, p_task->threads, p_task->blocksize, p_task->p_arguments);
			break;
		case CTRL_TASK_TYPE_MOVETO: {
			Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_task->p_tile->ext);
			HitTile       *p_tile      = p_task->p_tile;
			/* TODO: STRIDED TILES */

			/* TILES WITH THEIR OWN MEMORY ALLOCATION, OR CONTIGUOUS 1D TILES NEED ONLY ONE CONTIGUOUS COPY */
			if ((p_tile->memStatus == HIT_MS_OWNER) || (p_tile->shape.info.sig.numDims == 1)) {
				memcpy(p_tile_data->p_device_data, p_tile->data, ((size_t)(p_tile->acumCard)) * (p_tile->baseExtent));
			}
			/* CONTIGUOUS 2D TILES */
			else if (p_tile->shape.info.sig.numDims == 2) {
				for (int i = 0; i < p_tile->card[0]; i++) {
					memcpy((void *)(p_tile_data->p_device_data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1])),
						   (void *)(p_tile->data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1])),
						   ((size_t)(p_tile->card[1])) * (p_tile->baseExtent));
				}
			}
			/* CONTIGUOUS 3D TILES */
			// TODO: Check if 3D transfers are correct! - Manu 04/2021
			else if (p_tile->shape.info.sig.numDims == 3) {
				for (int i = 0; i < p_tile->card[0]; i++) {
					for (int j = 0; j < p_tile->card[1]; j++) {
						memcpy((void *)(p_tile_data->p_device_data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1] + j * p_tile->origAcumCard[2])),
							   (void *)(p_tile->data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1] + j * p_tile->origAcumCard[2])),
							   ((size_t)(p_tile->card[2])) * (p_tile->baseExtent));
					}
				}
			} else {
				fprintf(stderr, "[Ctrl_Cpu] error: Number of dimensions not supported for non-owner tile in MoveTo: %d\n",
						p_tile->shape.info.sig.numDims);
			}
			break;
		}
		case CTRL_TASK_TYPE_MOVEFROM: {
			Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_task->p_tile->ext);
			HitTile       *p_tile      = p_task->p_tile;
			/* TODO: STRIDED TILES */

			/* TILES WITH THEIR OWN MEMORY ALLOCATION, OR CONTIGUOUS 1D TILES NEED ONLY ONE CONTIGUOUS COPY */
			if ((p_tile->memStatus == HIT_MS_OWNER) || (p_tile->shape.info.sig.numDims == 1)) {
				memcpy(p_tile->data, p_tile_data->p_device_data, ((size_t)(p_tile->acumCard)) * (p_tile->baseExtent));
			}
			/* CONTIGUOUS 2D TILES */
			else if (p_tile->shape.info.sig.numDims == 2) {
				for (int i = 0; i < p_tile->card[0]; i++) {
					memcpy((void *)(p_tile->data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1])),
						   (void *)(p_tile_data->p_device_data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1])),
						   ((size_t)(p_tile->card[1])) * (p_tile->baseExtent));
				}
			}
			/* CONTIGUOUS 3D TILES */
			// TODO: Check if 3D transfers are correct! - Manu 04/2021
			else if (p_tile->shape.info.sig.numDims == 3) {
				for (int i = 0; i < p_tile->card[0]; i++) {
					for (int j = 0; j < p_tile->card[1]; j++) {
						memcpy((void *)(p_tile->data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1] + j * p_tile->origAcumCard[2])),
							   (void *)(p_tile_data->p_device_data + (p_tile->baseExtent) * (i * p_tile->origAcumCard[1] + j * p_tile->origAcumCard[2])),
							   ((size_t)(p_tile->card[2])) * (p_tile->baseExtent));
					}
				}
			} else {
				fprintf(stderr, "[Ctrl_Cpu] error: Number of dimensions not supported for non-owner tile in MoveFrom: %d\n",
						p_tile->shape.info.sig.numDims);
			}
			break;
		}
		case CTRL_TASK_TYPE_WAITEVENT:
			Ctrl_GenericEvent_Wait(p_task->event);
			break;
		case CTRL_TASK_TYPE_SIGNALEVENT:
			Ctrl_GenericEvent_Signal(p_task->event);
			break;
		case CTRL_TASK_TYPE_RELEASEEVENT:
			Ctrl_GenericEvent_Release(p_task->event);
			break;
		default:
			fprintf(stderr, "[Ctrl_Cpu] EvalTaskInner: task type %d not implemented on this stream\n", p_task->task_type);
			exit(EXIT_FAILURE);
	}
}

/**********************************
 ** TASKS'S EVALUATION FUNCTIONS **
 **********************************/

void Ctrl_Cpu_Destroy(Ctrl_Cpu *p_ctrl) {
	// send destroy task to kernel and host tasks streams
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_DESTROYCNTRL;
	Ctrl_TaskQueue_Push(p_ctrl->p_kernel_stream, task);
	Ctrl_TaskQueue_Push(p_ctrl_host_stream, task);

	// send destroy task to transfer streams
	if (p_ctrl->mem_moves) {
		Ctrl_TaskQueue_Push(p_ctrl->p_moveFrom_stream, task);
		Ctrl_TaskQueue_Push(p_ctrl->p_moveTo_stream, task);
	}

	// Clean Pointers to lined list of tasks (should be empty by now)
	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	// Free reamaining fields from Ctrl
	Ctrl_CpuEvent_Destroy(&p_ctrl->event_seq);
	hwloc_bitmap_free(p_ctrl->device_cpuset);
}

void Ctrl_Cpu_EvalTaskGlobalSync(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	// wait for all events from all tiles
	for (Ctrl_Cpu_Tile_List *p_aux = p_ctrl->p_tile_list_head; p_aux != NULL; p_aux = p_aux->p_next) {
		Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_aux->p_tile_ext);

		Ctrl_CpuEvent_Wait(p_tile_data->host_last_read_event);
		Ctrl_CpuEvent_Wait(p_tile_data->host_last_write_event);
		if (p_ctrl->mem_moves) {
			Ctrl_CpuEvent_Wait(p_tile_data->kernel_last_read_event);
			Ctrl_CpuEvent_Wait(p_tile_data->kernel_last_write_event);

			Ctrl_CpuEvent_Wait(p_tile_data->offloading_last_read_event);
			Ctrl_CpuEvent_Wait(p_tile_data->offloading_last_write_event);
		}
	}
	Ctrl_Cpu_Sync(p_ctrl);
}

void Ctrl_Cpu_EvalTaskKernelLaunch(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_GenericEvent kernel_event = CTRL_GENERIC_EVENT_NULL;
	kernel_event.event_type        = CTRL_EVENT_TYPE_CPU;

	// wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

			if (hit_tileIsNull(*p_tile)) {
				fprintf(stderr, "Warning: Launching task, skipping null tile on parameter %d\n", i);
				fflush(stderr);
				continue;
			}

			if (p_ctrl->mem_moves) {
				if (p_tile_data->device_status == CTRL_TILE_UNALLOC) {
					fprintf(stderr, "[Ctrl_Cpu] Internal Error: Launching kernel with tile with no device memory as argument and mem moves active %d\n", i);
					fflush(stderr);
				}

				if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data->device_status == CTRL_TILE_INVALID) {
					if (p_tile_data->host_status != CTRL_TILE_VALID) {
						fprintf(stderr, "[Ctrl_Cpu] Warning: Tile with uninitialized data as input on kernel %d\n", i);
						fflush(stderr);
					} else {
						// if tile's role is IN or IO, is not updated on device and host has memory allocated transfer it
						if (p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_Cpu_EvalTaskMoveToInner(p_ctrl, p_tile);
						}
					}
				}

				kernel_event.event.event_cpu = p_tile_data->kernel_last_write_event;
				Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);

				kernel_event.event.event_cpu = p_tile_data->offloading_last_write_event;
				Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);

				if (p_task->p_roles[i] != KERNEL_IN) {
					kernel_event.event.event_cpu = p_tile_data->kernel_last_read_event;
					Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);

					kernel_event.event.event_cpu = p_tile_data->offloading_last_read_event;
					Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);
				}
			} else {
				if (p_tile_data->device_status == CTRL_TILE_UNALLOC && p_tile_data->host_status == CTRL_TILE_UNALLOC) {
					fprintf(stderr, "[Ctrl_Cpu] Internal Error: Tile with no memory allocated as argument to kernel %d\n", i);
					fflush(stderr);
					exit(EXIT_FAILURE);
				}

				if (p_task->p_roles[i] != KERNEL_OUT && (p_tile_data->host_status == CTRL_TILE_INVALID || p_tile_data->device_status == CTRL_TILE_INVALID)) {
					fprintf(stderr, "[Ctrl_Cpu] Warning: Tile with uninitialized data as input on kernel %d\n", i);
					fflush(stderr);
				}

				kernel_event.event.event_cpu = p_tile_data->host_last_write_event;
				Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);

				if (p_task->p_roles[i] != KERNEL_IN) {
					kernel_event.event.event_cpu = p_tile_data->host_last_read_event;
					Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);
				}
			}
		}
	}
	// create request with info for kernel execution
	Ctrl_Request request;
	request.cpu.n_cores = p_ctrl->n_cores;

	p_task->request   = request;
	p_task->ctrl_type = CTRL_TYPE_CPU;

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		kernel_event.event.event_cpu = p_ctrl->event_seq;
		Ctrl_GenericEvent_StreamWait(kernel_event, p_ctrl->p_kernel_stream);
	}

	// push kernel task to kernel exec queue
	Ctrl_TaskQueue_Push(p_ctrl->p_kernel_stream, *p_task);

	// update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

			if (p_ctrl->mem_moves) {
				if (p_task->p_roles[i] != KERNEL_IN) {
					p_tile_data->device_status = CTRL_TILE_VALID;
					if (p_tile_data->host_status == CTRL_TILE_VALID) p_tile_data->host_status = CTRL_TILE_INVALID;
					Ctrl_CpuEvent_Record(&p_tile_data->kernel_last_write_event, p_ctrl->p_kernel_stream);
				}

				if (p_task->p_roles[i] != KERNEL_OUT) {
					Ctrl_CpuEvent_Record(&p_tile_data->kernel_last_read_event, p_ctrl->p_kernel_stream);
				}
			} else {
				if (p_task->p_roles[i] != KERNEL_IN) {
					if (p_tile_data->host_status != CTRL_TILE_UNALLOC)
						p_tile_data->host_status = CTRL_TILE_VALID;
					else
						p_tile_data->device_status = CTRL_TILE_VALID;

					Ctrl_CpuEvent_Record(&p_tile_data->host_last_write_event, p_ctrl->p_kernel_stream);
				}

				if (p_task->p_roles[i] != KERNEL_OUT) {
					Ctrl_CpuEvent_Record(&p_tile_data->host_last_read_event, p_ctrl->p_kernel_stream);
				}
			}
		}
	}

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->event_seq, p_ctrl->p_kernel_stream);
	}
}

void Ctrl_Cpu_EvalTaskHostTaskLaunch(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_GenericEvent host_task_event = CTRL_GENERIC_EVENT_NULL;
	host_task_event.event_type        = CTRL_EVENT_TYPE_CPU;

	// wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

			if (p_ctrl->mem_moves) {
				if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
					fprintf(stderr, "[Ctrl_Cpu] Internal Error: Launching host task with tile with no host memory as argument with mem moves active %d\n", i);
					fflush(stderr);
				}

				if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data->host_status == CTRL_TILE_INVALID) {
					if (p_tile_data->device_status != CTRL_TILE_VALID) {
						fprintf(stderr, "[Ctrl_Cpu] Warning: Tile with uninitialized data as input on host task %d\n", i);
						fflush(stderr);
					} else {
						// if tile's role is IN or IO, is not updated on host and device is valid transfer it
						if (p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_Cpu_EvalTaskMoveFromInner(p_ctrl, p_tile);
						}
					}
				}

				host_task_event.event.event_cpu = p_tile_data->offloading_last_read_event;
				Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);
			} else {
				if (p_tile_data->device_status == CTRL_TILE_UNALLOC && p_tile_data->host_status == CTRL_TILE_UNALLOC) {
					fprintf(stderr, "[Ctrl_Cpu] Internal Error: Tile with no memory allocated as argument to host task %d\n", i);
					fflush(stderr);
					exit(EXIT_FAILURE);
				}
				if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data->host_status == CTRL_TILE_INVALID) {
					fprintf(stderr, "[Ctrl_Cpu] Warning: Tile with uninitialized data as input on kernel %d\n", i);
					fflush(stderr);
				}
			}

			host_task_event.event.event_cpu = p_tile_data->host_last_write_event;
			Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);

			if (p_task->p_roles[i] != KERNEL_IN) {
				if (p_ctrl->mem_moves) {
					p_tile_data->host_status = CTRL_TILE_VALID;
					if (p_tile_data->device_status == CTRL_TILE_VALID) p_tile_data->device_status = CTRL_TILE_INVALID;
					host_task_event.event.event_cpu = p_tile_data->offloading_last_write_event;
					Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);
				} else {
					if (p_tile_data->host_status != CTRL_TILE_UNALLOC)
						p_tile_data->host_status = CTRL_TILE_VALID;
					else
						p_tile_data->device_status = CTRL_TILE_VALID;
				}

				host_task_event.event.event_cpu = p_tile_data->host_last_read_event;
				Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);
			}
		}
	}

	// wait for previous task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		host_task_event.event.event_cpu = p_ctrl->event_seq;
		Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);
	}

	// push host task to host exec queue
	Ctrl_TaskQueue_Push(p_ctrl_host_stream, *p_task);

	// update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile       *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);
			if (p_task->p_roles[i] != KERNEL_IN) {
				Ctrl_CpuEvent_Record(&p_tile_data->host_last_write_event, p_ctrl_host_stream);
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				Ctrl_CpuEvent_Record(&p_tile_data->host_last_read_event, p_ctrl_host_stream);
			}
		}
	}

	// wait for host task to finish if policy is sync
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Record(&p_ctrl->event_seq, p_ctrl_host_stream);
		Ctrl_CpuEvent_Wait(p_ctrl->event_seq);
		Ctrl_Cpu_Sync(p_ctrl);
	}
}

void Ctrl_Cpu_EvalTaskDomainTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_Cpu_CreateTile(p_ctrl, p_task);
	Ctrl_Cpu_Sync(p_ctrl);
}

void Ctrl_Cpu_EvalTaskAllocTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

	if (!(p_tile_data->is_initialized))
		Ctrl_Cpu_InitTile(p_ctrl, p_task);

	// memory was already allocated for this tile but mem moves are off
	if (!p_ctrl->mem_moves && (p_tile_data->host_status != CTRL_TILE_UNALLOC || p_tile_data->device_status != CTRL_TILE_UNALLOC)) {
		fprintf(stderr, "[Ctrl_Cpu] Warning: Memory movements are off for this ctrl and memory for this tile was already allocated. Ignoring allocation call.\n");
		Ctrl_Cpu_Sync(p_ctrl);
		return;
	}

	if (p_task->flags & (CTRL_MEM_ALLOC_BOTH | CTRL_MEM_ALLOC_HOST) || !(p_task->flags & CTRL_MEM_ALLOC_DEV)) {
		if (p_tile_data->host_status != CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cpu] Warning: Host memory already allocated for this tile, ignoring this call.\n");
			fflush(stderr);
			Ctrl_Cpu_Sync(p_ctrl);
			return;
		}
		// Allocate memory for the host image of the data inside the tile, equivalent to hit_tileAlloc(p_tile);
		p_tile->data             = (void *)malloc((size_t)p_tile->acumCard * p_tile->baseExtent);
		p_tile->memPtr           = p_tile->data;
		p_tile_data->host_status = CTRL_TILE_INVALID;
		if (!p_ctrl->mem_moves) {
			p_tile_data->p_device_data = p_tile->data;
		}
	}

	if ((p_ctrl->mem_moves && (p_task->flags & (CTRL_MEM_ALLOC_DEV | CTRL_MEM_ALLOC_BOTH) || !(p_task->flags & CTRL_MEM_ALLOC_HOST))) ||
		(!p_ctrl->mem_moves && (p_task->flags & CTRL_MEM_ALLOC_DEV) && !(p_task->flags & (CTRL_MEM_ALLOC_BOTH | CTRL_MEM_ALLOC_HOST)))) {

		if (p_tile_data->device_status != CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cpu] Warning: Device memory already allocated for this tile, ignoring this call.\n");
			fflush(stderr);
			Ctrl_Cpu_Sync(p_ctrl);
			return;
		}
		p_tile_data->p_device_data = (void *)hwloc_alloc_membind(p_ctrl->topo, (size_t)p_tile->acumCard * p_tile->baseExtent, p_ctrl->device_cpuset, HWLOC_MEMBIND_BIND, 0);
		p_tile_data->device_status = CTRL_TILE_INVALID;
		if (!p_ctrl->mem_moves) {
			p_tile->data = p_tile_data->p_device_data;
		}
	}

	Ctrl_Cpu_Sync(p_ctrl);
}

void Ctrl_Cpu_EvalTaskSelectTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	Ctrl_Cpu_CreateTile(p_ctrl, p_task);

	HitTile *p_tile = (HitTile *)(p_task->p_tile);

	if (hit_tileIsNull(*p_tile)) {
		Ctrl_Cpu_Sync(p_ctrl);
		return;
	}

	HitTile       *p_parent      = p_tile->ref;
	Ctrl_Cpu_Tile *p_tile_data   = (Ctrl_Cpu_Tile *)(p_tile->ext);
	p_tile_data->p_parent_ext    = ((Ctrl_Cpu_Tile *)(p_tile->ref->ext));
	Ctrl_Cpu_Tile *p_parent_data = p_tile_data->p_parent_ext;

	if ((p_task->flags & CTRL_SELECT_INIT) == CTRL_SELECT_INIT) {
		Ctrl_Cpu_InitTile(p_ctrl, p_task);
	}

	if (p_tile->memStatus == HIT_MS_NOT_OWNER) {
		p_tile_data->host_status   = p_tile_data->p_parent_ext->host_status;
		p_tile_data->device_status = p_tile_data->p_parent_ext->device_status;

		p_tile_data->p_device_data = p_parent_data->p_device_data + (p_tile->data - p_parent->data);
	}

	Ctrl_Cpu_Sync(p_ctrl);
}

void Ctrl_Cpu_EvalTaskFreeTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

	if (p_tile_data->is_initialized) {
		// wait for the work related to the tile to finish
		Ctrl_CpuEvent_Wait(p_tile_data->host_last_write_event);
		Ctrl_CpuEvent_Wait(p_tile_data->host_last_read_event);
		if (p_ctrl->mem_moves) {
			Ctrl_CpuEvent_Wait(p_tile_data->kernel_last_read_event);
			Ctrl_CpuEvent_Wait(p_tile_data->kernel_last_write_event);

			Ctrl_CpuEvent_Wait(p_tile_data->offloading_last_read_event);
			Ctrl_CpuEvent_Wait(p_tile_data->offloading_last_write_event);
		}

		// Destroy events of this tile
		Ctrl_CpuEvent_Destroy(&p_tile_data->host_last_write_event);
		Ctrl_CpuEvent_Destroy(&p_tile_data->host_last_read_event);
		if (p_ctrl->mem_moves) {
			Ctrl_CpuEvent_Destroy(&p_tile_data->kernel_last_read_event);
			Ctrl_CpuEvent_Destroy(&p_tile_data->kernel_last_write_event);

			Ctrl_CpuEvent_Destroy(&p_tile_data->offloading_last_read_event);
			Ctrl_CpuEvent_Destroy(&p_tile_data->offloading_last_write_event);
		}

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

	if (p_tile->memStatus == HIT_MS_OWNER) {
		if (p_tile_data->host_status != CTRL_TILE_UNALLOC) {
			// Free host image of the tile, equivalent to hit_tileFree(*p_tile);
			free(p_tile->data);
		}

		if (p_tile_data->device_status != CTRL_TILE_UNALLOC) {
			// Free device image of the tile
			hwloc_free(p_ctrl->topo, p_tile_data->p_device_data, (size_t)p_tile->acumCard * p_tile->baseExtent);
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

	Ctrl_Cpu_Sync(p_ctrl);
}

void Ctrl_Cpu_EvalTaskMoveTo(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	// If transfers are active and tile is not updated perform the transfer
	if (p_ctrl->mem_moves) {
		if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cpu] Internal Error: Tryinng to move tile from host to device but host memory was not allocated\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}

		if (p_tile_data->device_status == CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cpu] Internal Error: Tryinng to move tile from host to device but device memory was not allocated\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}

		if (p_tile_data->host_status == CTRL_TILE_INVALID) {
			fprintf(stderr, "[Ctrl_Cpu] Warning: Moving a tile from host to device with invalid data on host memory\n");
			fflush(stderr);
		}

		// If tile is not updated perform the transfer
		if (!(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data->device_status == CTRL_TILE_VALID) || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
			Ctrl_Cpu_EvalTaskMoveToInner(p_ctrl, p_tile);
		}
	}

	// If policy is sync wait for transfer to finish
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Wait(p_ctrl->event_seq);
		Ctrl_Cpu_Sync(p_ctrl);
	}
}

void Ctrl_Cpu_EvalTaskMoveFrom(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	// If transfers are active and tile is not updated perform the transfer
	if (p_ctrl->mem_moves) {
		if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cpu] Internal Error: Trying to move tile from device to host but host memory was not allocated\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}

		if (p_tile_data->device_status == CTRL_TILE_UNALLOC) {
			fprintf(stderr, "[Ctrl_Cpu] Internal Error: Trying to move tile from device to host but device memory was not allocated\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}

		if (p_tile_data->device_status == CTRL_TILE_INVALID) {
			fprintf(stderr, "[Ctrl_Cpu] Warning: Moving a tile from device to host with invalid data on device memory\n");
			fflush(stderr);
		}

		if (!(p_tile_data->host_status == CTRL_TILE_VALID && p_tile_data->device_status == CTRL_TILE_VALID) || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
			Ctrl_Cpu_EvalTaskMoveFromInner(p_ctrl, p_tile);
		}
	}

	// If policy is sync wait for transfer to finish
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_CpuEvent_Wait(p_ctrl->event_seq);
		Ctrl_Cpu_Sync(p_ctrl);
	}
}

void Ctrl_Cpu_EvalTaskWaitTile(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	HitTile       *p_tile      = (HitTile *)(p_task->p_tile);
	Ctrl_Cpu_Tile *p_tile_data = (Ctrl_Cpu_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	// Wait for all work related to this tile to finish
	Ctrl_CpuEvent_Wait(p_tile_data->host_last_write_event);
	Ctrl_CpuEvent_Wait(p_tile_data->host_last_read_event);
	if (p_ctrl->mem_moves) {
		Ctrl_CpuEvent_Wait(p_tile_data->kernel_last_read_event);
		Ctrl_CpuEvent_Wait(p_tile_data->kernel_last_write_event);

		Ctrl_CpuEvent_Wait(p_tile_data->offloading_last_read_event);
		Ctrl_CpuEvent_Wait(p_tile_data->offloading_last_write_event);
	}
	Ctrl_Cpu_Sync(p_ctrl);
}

void Ctrl_Cpu_EvalTaskSetDependanceMode(Ctrl_Cpu *p_ctrl, Ctrl_Task *p_task) {
	p_ctrl->dependance_mode = p_task->flags;
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_Cpu_Sync(p_ctrl);
	}
}
///@endcond
