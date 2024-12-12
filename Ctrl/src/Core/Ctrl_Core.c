///@cond INTERNAL
/**
 * @file Ctrl_Core.c
 * @brief Source code for core functions of Controllers.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Core/Ctrl_Config.h"
#include "Core/Ctrl_Core.h"
#include <string.h>

/**
 * optional weights for distributed computations
 */
HitWeights ctrl_weights = {0, NULL};

/**
 * Contains the list of ctrls.
 */
Ctrl *p_ctrl_global = NULL;

/**
 * Size of global list of ctrls \e p_ctrl_global.
 */
int n_ctrls;

/**
 * Topology of the machine.
 */
hwloc_topology_t topo;

/**
 * Index of the numa node to use for host threads.
 */
int host_node = 0;

/**
 * Policy to be used by ctrls.
 */
Ctrl_Policy policy = CTRL_POLICY_DEFAULT;

/**
 * Pointer to stream for host task execution
 */
Ctrl_TaskQueue *p_ctrl_host_stream = NULL;

/**
 * Evaluate \p p_task.
 *
 * This calls to the appropiate eval_task depending on \p p_ctrl's type.
 *
 * @param p_ctrl pointer to the ctrl the task was sent to.
 * @param p_task pointer to the task to be evaluated.
 *
 * @see Ctrl_Cpu_EvalTask, Ctrl_Cuda_EvalTask, Ctrl_Hip_EvalTask, Ctrl_OpenCLGpu_EvalTask
 */
void Ctrl_ExecTask(Ctrl *p_ctrl, Ctrl_Task *p_task);

/**
 * Create and execute a task of type \p type associated to \p p_ctrl .
 *
 * @param p_ctrl ctrl to send the task to.
 * @param type type of the task to send.
 * @param p_tile tile to place inside the task. This can be null for certain task's type.
 *
 * @see Ctrl_ExecTask, Ctrl_TaskQueue_Push
 */
void Ctrl_AddTask(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile);

/**
 * If ctrl queues are active add a task of type \p type that needs flags to \p p_ctrl inner queue, if ctrl queues are not
 * active pass that task to \e Ctrl_ExecTask instead.
 *
 * @param p_ctrl ctrl to send the task to.
 * @param type type of the task to send.
 * @param p_tile tile to place inside the task. This can be null for certain task's type.
 * @param flags flags to pass to the task.
 *
 * @see Ctrl_ExecTask, Ctrl_TaskQueue_Push, Ctrl_AllocInner, Ctrl_SelectInner
 */
void Ctrl_AddTaskFlagged(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile, int flags);

/**
 * Evaluation for host tasks and events by the host task thread.
 *
 * @param p_task task to evaluate.
 *
 * @see Ctrl_Thread_HostTask
 */
void Ctrl_EvalTask(Ctrl_Task *p_task);

/**
 * Function for extra threads for ctrls. If extra threads are needed for a ctrl, this function takes the spawner thread created
 * and divides it and distributes it as needed depending on the configuration of its ctrl.
 *
 * @see Ctrl_Thread_Init
 */
void Ctrl_Thread_Spawner();

/**
 * Function for host task thread. Creates host task queue and starts executing tasks from it until a destroy task arrives to the queue.
 *
 * @see Ctrl_Thread_Init, Ctrl_EvalTask
 */
void Ctrl_Thread_HostTask();

/**
 * Function for queue manager thread. Gets the host side queues used by ctrls and pushes tasks from them to driver streams.
 *
 * @see Ctrl_Thread_Init
 */
void Ctrl_Thread_QueueManager();

/**
 * Returns the number of threads needed for \p p_ctrl.
 *
 * @param p_ctrl Ctrl we want to get the number of extra threads of.
 *
 * @return Number of threads needed for \p p_ctrl.
 */
int Ctrl_GetNumThreads(Ctrl *p_ctrl);

/**
 * Returns the number of queues needed by \p p_ctrl.
 *
 * @param p_ctrl Ctrl we want to get the number of queues.
 *
 * @return Number of queues used by \p p_ctrl.
 */
int Ctrl_GetNumQueues(Ctrl *p_ctrl);

/**
 * Get the pointers to the host queues used by \p p_ctrl on list \p pp_queues
 *
 * @param p_ctrl Ctrl to get the queues from.
 * @param pp_queues [out] Pointer to pointers to the host queues used by this ctrl
 * @return Pointer right after the queue pointers stored on \p pp_queues
 *
 * @pre \p pp_queues must have enough memory allocated to store all queues from this ctrl.
 * @see Ctrl_GetNumQueues
 */
Ctrl_TaskQueue **Ctrl_GetHostQueues(Ctrl *p_ctrl, Ctrl_TaskQueue **pp_queues);

/**
 * Creates a ctrl with the args especified.
 *
 * @param type type of the ctrl to create
 * @param id id for the ctrl. Must be positive.
 * @param args string with the argument to the ctrl. Corresponds to a line of the config file for the ctrl_block
 *
 * @see __ctrl_block__
 */
void Ctrl_Create(Ctrl_Type type, int id, char *args);

/**
 * @brief Push a wait operation of \p event to \p stream if they are compatible
 *
 * This is an optimization because waiting for CUDA, HIP and OpenCL on the host via busy-wait testing is very expensive
 *
 * @param event Event to wait to
 * @param qid Queue index inside the ctrl to enqueue the wait into
 * @param p_ctrl Ctrl recieving \p event
 * @return true \p event was successfully enqueued into \p stream
 * @return false \p event is not compatible with \p stream
 */
bool Ctrl_PushEventIfCompat(Ctrl_GenericEvent event, int qid, Ctrl *p_ctrl);

void Ctrl_PinToHostNuma() {
	hwloc_obj_t obj = hwloc_get_obj_by_type(topo, HWLOC_OBJ_NUMANODE, host_node);
	if (!obj) {
		fprintf(stderr, "[Ctrl_PinToHostNuma] Warning host NUMA node %d not found.\n", host_node);
		fflush(stderr);
		return;
	}
	if (hwloc_set_cpubind(topo, obj->cpuset, HWLOC_CPUBIND_THREAD) != 0) {
		fprintf(stderr, "[Ctrl_PinToHostNuma] Warning pinning thread to host numa node %d returned an error, make sure you have permission to use those resources.\n", host_node);
		fflush(stderr);
		return;
	}

	hwloc_cpuset_t allowed_numa_cpuset = hwloc_bitmap_alloc();
	hwloc_bitmap_and(allowed_numa_cpuset, obj->cpuset, hwloc_topology_get_allowed_cpuset(topo));
	int allowed_numa_cores = hwloc_get_nbobjs_inside_cpuset_by_type(topo, allowed_numa_cpuset, HWLOC_OBJ_CORE);
	hwloc_bitmap_free(allowed_numa_cpuset);

	static int bound_threads = 0;

	int n_thr;
	#pragma omp atomic capture
	n_thr = ++bound_threads;

	if (allowed_numa_cores < n_thr) {
		fprintf(stderr, "[Ctrl_PinToHostNuma] Warning only %d cores are allowed for use in host numa node %d but %d threads are currently bound to it.\n", allowed_numa_cores, host_node, bound_threads);
		fflush(stderr);
	}
}

int Ctrl_Thread_Init() {

	if (omp_get_thread_num() == 0) {
		// bind main thread to host numa node
		Ctrl_PinToHostNuma();
		return 0;
	} else if (omp_get_thread_num() == 1) { // host task executor
		// bind host task thread to host numa node
		Ctrl_PinToHostNuma();
		Ctrl_Thread_HostTask();
	} else if (omp_get_thread_num() == 2) { // queue manager
		// bind queue manager thread to host numa node
		Ctrl_PinToHostNuma();
		Ctrl_Thread_QueueManager();
	} else { // spawner threads
		Ctrl_Thread_Spawner();
	}

	// All threads ready for main thread to cleanup
	#pragma omp barrier

	return 1;
}

void Ctrl_Thread_HostTask() {
	// Explicitly initialize cuda runtime for this thread to avoid performace penalties
	#ifdef _CTRL_ARCH_CUDA_
	for (int i = 0; i < n_ctrls; i++) {
		if (p_ctrl_global[i].type == CTRL_TYPE_CUDA) {
			Ctrl_Cuda_SetDevice();
		}
	}
	#endif // _CTRL_ARCH_CUDA_

	// Extract and evaluate tasks until destroy task
	bool finish = false;
	while (!finish) {
		Ctrl_Task *p_task = Ctrl_TaskQueue_Pop(p_ctrl_host_stream);
		if (p_task->task_type == CTRL_TASK_TYPE_DESTROYCTRL) {
			finish = true;
		} else {
			Ctrl_EvalTask(p_task);
			// Update queue last_finished index
			#pragma omp atomic update
			p_ctrl_host_stream->last_finished++;
			// free stuff inside task
			if (p_task->task_type != CTRL_TASK_TYPE_HOST) {
				Ctrl_TaskQueue_FreeTask(p_task);
			}
		}
	}
	// Destroy queue (clean up)
	Ctrl_TaskQueue_Destroy(p_ctrl_host_stream);
}

void Ctrl_Thread_Spawner() {
	// TODO @sergioalo the -3 should really be a constant
	int   cpu_ctrl_counter = 0;
	PCtrl p_ctrl           = NULL;
	for (int i = 0; i < n_ctrls; i++) {
		p_ctrl = &p_ctrl_global[i];
		if (p_ctrl->type != CTRL_TYPE_CPU)
			continue;
		if (cpu_ctrl_counter == omp_get_thread_num() - 3)
			break;
		cpu_ctrl_counter++;
	}

	// Divide thread if ctrl needs more threads
	#pragma omp parallel num_threads(Ctrl_GetNumThreads(p_ctrl))
	{
		// Distribute new threads
		switch (p_ctrl->type) {
			#ifdef _CTRL_ARCH_CPU_
			case CTRL_TYPE_CPU:
				Ctrl_Cpu_ThreadInit(&(p_ctrl->p_impl->cpu), topo);
				break;
			#endif // _CTRL_ARCH_CPU_
			default:
				#pragma omp single
				{
					fprintf(stderr, "[Ctrl Warning] Ctrls of type %d don't know how to use the extra threads in __ctrl_block__!\n", p_ctrl->type);
					fflush(stderr);
				}
				break;
		}
	}
}

void Ctrl_Create(Ctrl_Type type, int id, char *args) {
	PCtrl p_ctrl   = &p_ctrl_global[id];
	p_ctrl->p_impl = (Ctrl_Impl *)malloc(sizeof(Ctrl_Impl));
	p_ctrl->id     = id;
	p_ctrl->type   = type;
	// TODO @sergioalo send id as param to inner create?
	switch (type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			p_ctrl->p_impl->cpu.topo      = topo;
			p_ctrl->p_impl->cpu.global_id = p_ctrl->id;
			Ctrl_Cpu_Create(&(p_ctrl->p_impl->cpu), policy, args);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			p_ctrl->p_impl->cuda.global_id = p_ctrl->id;
			Ctrl_Cuda_Create(&(p_ctrl->p_impl->cuda), policy, args);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			p_ctrl->p_impl->hip.global_id = p_ctrl->id;
			Ctrl_Hip_Create(&(p_ctrl->p_impl->hip), policy, args);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			p_ctrl->p_impl->opencl_gpu.global_id = p_ctrl->id;
			Ctrl_OpenCLGpu_Create(&(p_ctrl->p_impl->opencl_gpu), policy, args);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			p_ctrl->p_impl->fpga.global_id = p_ctrl->id;
			Ctrl_FPGA_Create(&(p_ctrl->p_impl->fpga), policy, args);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl_Create] Unknown or unsupported architecture: %d\n", type);
			exit(EXIT_FAILURE);
	}
}

PCtrl Ctrl_Get(int id) {
	if (id < 0 || id >= Ctrl_GetNCtrls()) {
		fprintf(stderr, "[Ctrl_Get] Invalid ctrl id %d\n", id);
		exit(EXIT_FAILURE);
	}
	return &p_ctrl_global[id];
}

int Ctrl_GetNCtrls() {
	return n_ctrls;
}

void Ctrl_ExecTask(Ctrl *p_ctrl, Ctrl_Task *p_task) {
	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			Ctrl_Cpu_EvalTask(&(p_ctrl->p_impl->cpu), p_task);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_EvalTask(&(p_ctrl->p_impl->cuda), p_task);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_EvalTask(&(p_ctrl->p_impl->hip), p_task);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_EvalTask(&(p_ctrl->p_impl->opencl_gpu), p_task);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_EvalTask(&(p_ctrl->p_impl->fpga), p_task);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl_ExecTask] Unsupported architecture %d. Recompile Ctrl library with the proper support.\n", p_ctrl->type);
			exit(EXIT_FAILURE);
	}

	if (p_task->task_type != CTRL_TASK_TYPE_HOST && p_task->task_type != CTRL_TASK_TYPE_KERNEL) {
		Ctrl_TaskQueue_FreeTask(p_task);
	}
}

void Ctrl_EndBlock() {
	// Wait for all operations to end before starting cleanup
	Ctrl_Synchronize();

	bool freed_queues = 0;
	for (int i = 0; i < Ctrl_GetNCtrls(); i++) {
		// Tell ctrls to cleanup their internal stuff and propagate the destroy signal
		if (!freed_queues && (p_ctrl_global[i].type != CTRL_TYPE_CPU)) {
			// sends destroy signal to queue manager, only from 1 ctrl that is not cpu
			Ctrl_AddTaskFlagged(&p_ctrl_global[i], CTRL_TASK_TYPE_DESTROYCTRL, NULL, 1);
			freed_queues = 1;
		} else {
			Ctrl_AddTaskFlagged(&p_ctrl_global[i], CTRL_TASK_TYPE_DESTROYCTRL, NULL, 0);
		}
		// Free internal part of the ctrl
		free(p_ctrl_global[i].p_impl);
		p_ctrl_global[i].p_impl = NULL;
	}

	// Tell send destroy signal to host task queue
	Ctrl_Task task;
	task.task_type = CTRL_TASK_TYPE_DESTROYCTRL;
	Ctrl_TaskQueue_Push(p_ctrl_host_stream, task);

	// Wait for other threads to be ready for ctrl list full cleanup
	#pragma omp barrier
	free(p_ctrl_global);
	free(ctrl_weights.ratios);
	hwloc_topology_destroy(topo);
}

void Ctrl_AddTask(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile) {
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = type;

	task.p_tile = p_tile;

	Ctrl_ExecTask(p_ctrl, &task);
}

void Ctrl_AddTaskFlagged(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile, int flags) {
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = type;

	task.p_tile = p_tile;
	task.flags  = flags;

	Ctrl_ExecTask(p_ctrl, &task);
}

void Ctrl_LaunchKernel(Ctrl *p_ctrl, Ctrl_Task task) {
	Ctrl_ExecTask(p_ctrl, &task);
}

void Ctrl_LaunchHostTask(Ctrl_Task task) {
	Ctrl_Task *p_task = &task;
	// wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile   *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Tile *p_tile_data = (Ctrl_Tile *)(p_tile->ext);

			if (p_tile_data->host_status == CTRL_TILE_UNALLOC) {
				fprintf(stderr, "[Ctrl_Core] Ctrl_LaunchHostTask: Launching host task %s with tile with no host memory as argument %d\n", p_task->p_func_name, i);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}

			for (int j = 0; j < Ctrl_GetNCtrls(); j++) {
				Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[j];
				if (p_tile_data_impl->type == CTRL_TYPE_NULL) {
					continue;
				}

				// TODO @sergioalo Ctrl_MoveFromInner does unnecesary checks

				// TODO @sergioalo dependance mode should be on abstract ctrl
				// if tile's role is IN or IO, is not updated on host, and is updated on this device transfer it
				bool move_tile = p_task->p_roles[i] != KERNEL_OUT && p_tile_data->host_status == CTRL_TILE_INVALID && p_tile_data_impl->device_status == CTRL_TILE_VALID;

				switch (p_tile_data_impl->type) {
					#ifdef _CTRL_ARCH_CPU_
					case CTRL_TYPE_CPU:
						if (move_tile && p_ctrl_global[j].p_impl->cpu.dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_MoveFromInner(&p_ctrl_global[j], p_tile);
						}
						break;
					#endif // _CTRL_ARCH_CPU_

					#ifdef _CTRL_ARCH_CUDA_
					case CTRL_TYPE_CUDA:
						if (move_tile && p_ctrl_global[j].p_impl->cuda.dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_MoveFromInner(&p_ctrl_global[j], p_tile);
						}
						break;
					#endif // _CTRL_ARCH_CUDA_

					#ifdef _CTRL_ARCH_HIP_
					case CTRL_TYPE_HIP:
						if (move_tile && p_ctrl_global[j].p_impl->hip.dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_MoveFromInner(&p_ctrl_global[j], p_tile);
						}
						break;
					#endif // _CTRL_ARCH_HIP_

					#ifdef _CTRL_ARCH_OPENCL_GPU_
					case CTRL_TYPE_OPENCL_GPU:
						if (move_tile && p_ctrl_global[j].p_impl->opencl_gpu.dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_MoveFromInner(&p_ctrl_global[j], p_tile);
						}
						break;
					#endif // _CTRL_ARCH_OPENCL_GPU_

					#ifdef _CTRL_ARCH_FPGA_
					case CTRL_TYPE_FPGA:
						if (move_tile && p_ctrl_global[j].p_impl->fpga.dependance_mode == CTRL_MODE_IMPLICIT) {
							Ctrl_MoveFromInner(&p_ctrl_global[j], p_tile);
						}
						break;
					#endif // _CTRL_ARCH_FPGA_
					default:
						fprintf(stderr, "[Ctrl_Core] Ctrl_LaunchHostTask: unknown arch tile %d on host task %s\n", p_tile_data_impl->type, p_task->p_func_name);
						exit(EXIT_FAILURE);
				}
				Ctrl_HostTaskWait(p_tile_data_impl, p_task->p_roles[i], p_ctrl_host_stream);
			}

			if (p_task->p_roles[i] != KERNEL_OUT && p_tile_data->host_status == CTRL_TILE_INVALID) {
				fprintf(stderr, "[Ctrl_Core] Warning: Tile %d with uninitialized data as input on host task %s\n", i, p_task->p_func_name);
				fflush(stderr);
			}

			Ctrl_GenericEvent_StreamWait(p_tile_data->last_host_write_event, p_ctrl_host_stream);
			if (p_task->p_roles[i] != KERNEL_IN) {
				for (int j = 0; j < Ctrl_GetNCtrls(); j++) {
					Ctrl_Tile_Impl *p_tile_data_impl = &p_tile_data->p_impls[j];
					if (p_tile_data_impl->type == CTRL_TYPE_NULL) {
						continue;
					}
					if (p_tile_data_impl->device_status == CTRL_TILE_VALID) p_tile_data_impl->device_status = CTRL_TILE_INVALID;
				}

				p_tile_data->host_status = CTRL_TILE_VALID;
				Ctrl_GenericEvent_StreamWait(p_tile_data->last_host_read_event, p_ctrl_host_stream);
			}
		}
	}

	if (policy == CTRL_POLICY_SYNC) {
		Ctrl_SyncWait(p_ctrl_host_stream);
	}

	Ctrl_TaskQueue_Push(p_ctrl_host_stream, *p_task);

	// update events on arguments according to their roles
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile   *p_tile      = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_Tile *p_tile_data = (Ctrl_Tile *)(p_tile->ext);

			if (p_task->p_roles[i] != KERNEL_IN) {
				Ctrl_CpuEvent_Record(&p_tile_data->last_host_write_event.event.event_cpu, p_ctrl_host_stream);
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				Ctrl_CpuEvent_Record(&p_tile_data->last_host_read_event.event.event_cpu, p_ctrl_host_stream);
			}
		}
	}

	// wait for host task to finish if policy is sync
	if (policy == CTRL_POLICY_SYNC) {
		Ctrl_TaskQueue_Synchronize(p_ctrl_host_stream);
	}
}

void Ctrl_GlobalSync(Ctrl *p_ctrl) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_GLOBALSYNC, NULL);
}

void Ctrl_AllocInner(Ctrl *p_ctrl, HitTile *p_tile, int flags) {
	/* UPDATES FOR SHADOW COPIES, OR ALLOCATION OF SELECTIONS OF NO-MEMORY VARIABLES */
	// ADJUST ORIG_ACUM_CARD TO CARDINALITIES TO TRANSFORM ON MEMORY_OWNER, AS IN HITMAP
	if ((p_tile->memStatus == HIT_MS_NOT_OWNER) || (p_tile->memStatus == HIT_MS_NOMEM)) {
		/* 3.1. NEW STRIDES TO ACCESS ARE ALWAYS 1 */
		for (int i = 0; i < hit_shapeDims(p_tile->shape); i++) {
			p_tile->qstride[i] = 1;
		}

		/* 3.2. UPDATE ORIGINAL ACUMULATED CARDINALITIES, NOW IT HAS ITS OWN MEMORY */
		// Code below extracted from the body of hit_tileUpdateAcumCards(p_tile); (static inline fn)
		p_tile->origAcumCard[hit_shapeDims(p_tile->shape)] = 1;
		int cardinality                                    = 1;
		for (int i = (hit_shapeDims(p_tile->shape) - 1); i >= 0; i--) {
			cardinality             = cardinality * p_tile->card[i];
			p_tile->origAcumCard[i] = cardinality;
		}
		p_tile->acumCard = p_tile->origAcumCard[0];
	}
	p_tile->memStatus = HIT_MS_OWNER;

	// RE-ADJUST TO PITCHED CARDINALITIES
	#ifdef ALIGNED_SIZE
	// TODO: Check if this works. This code was written by Arturo.
	//  Manu 04/2021
	int dims = hit_tileDims(*p_tile);
	if (dims > 1) {
		size_t toPitch = p_tile->card[dims - 1] * p_tile->baseExtent;

		toPitch = ((toPitch + ALIGNED_SIZE - 1) / ALIGNED_SIZE) * ALIGNED_SIZE / p_tile->baseExtent;

		p_tile->origAcumCard[dims - 1] = toPitch;
		for (int i = dims - 2; i >= 0; i--) {
			toPtich *= p_tile->card[1];
			p_tile->origAcumCard[i] = toPitch;
		}
	}
	#endif

	Ctrl_AddTaskFlagged(p_ctrl, CTRL_TASK_TYPE_ALLOCTILE, p_tile, flags);
}

void Ctrl_CreateTexInner(Ctrl *p_ctrl, HitTile *p_tile, Ctrl_TexDesc tex_desc) {
	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			Ctrl_Cpu_CreateTex(&p_ctrl->p_impl->cpu, p_tile, tex_desc);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_CreateTex(&p_ctrl->p_impl->cuda, p_tile, tex_desc);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_CreateTex(&p_ctrl->p_impl->hip, p_tile, tex_desc);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_CreateTex(&p_ctrl->p_impl->opencl_gpu, p_tile, tex_desc);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_CreateTex(&p_ctrl->p_impl->fpga, p_tile, tex_desc);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl_Core] Ctrl_CreateTex: Unknown ctrl type %d", p_ctrl->type);
			exit(EXIT_FAILURE);
	}
}

void Ctrl_SelectInner(HitTile *p_tile, int flags) {
	// @arturo: Bug, select tasks for NULL Tiles should not be introduced in the queue
	if (hit_tileIsNull(*p_tile)) {
		#ifdef _CTRL_DEBUG_
		fprintf(stderr, "Warning: NULL tile on Ctrl_SelectInner\n");
		fflush(stderr);
		#endif // _CTRL_DEBUG_
		return;
	}

	Ctrl_DomainInner(p_tile);

	Ctrl_Tile *p_parent_data = ((Ctrl_Tile *)(p_tile->ref->ext));

	if (p_tile->memStatus == HIT_MS_NOT_OWNER) {
		Ctrl_Tile *p_tile_data   = (Ctrl_Tile *)(p_tile->ext);
		p_tile_data->host_status = p_parent_data->host_status;
	}

	for (int i = 0; i < n_ctrls; i++) {
		Ctrl_Tile_Impl *p_parent_data_impl = &p_parent_data->p_impls[i];
		// check if parent is asociated with ctrl i
		if (p_parent_data_impl->type == CTRL_TYPE_NULL) continue;

		Ctrl_AddTaskFlagged(&p_ctrl_global[i], CTRL_TASK_TYPE_SELECTTILE, p_tile, flags);
	}
}

void Ctrl_DomainInner(HitTile *p_tile) {
	Ctrl_Tile *p_tile_data = (Ctrl_Tile *)malloc(sizeof(Ctrl_Tile));

	p_tile_data->p_impls = (Ctrl_Tile_Impl *)calloc(Ctrl_GetNCtrls(), sizeof(Ctrl_Tile_Impl));

	p_tile_data->host_status = CTRL_TILE_UNALLOC;
	p_tile_data->valid_impls = 0;

	p_tile_data->last_host_read_event  = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, -1);
	p_tile_data->last_host_write_event = Ctrl_GenericEvent_Create(CTRL_EVENT_TYPE_CPU, -1);

	p_tile->ext = (void *)p_tile_data;
}

void Ctrl_FreeInner(Ctrl *p_ctrl, HitTile *p_tile) {
	if (hit_tileIsNull(*p_tile)) {
		#ifdef _CTRL_DEBUG_
		fprintf(stderr, "Warning: NULL tile on Ctrl_FreeInner\n");
		fflush(stderr);
		#endif // _CTRL_DEBUG_
		return;
	}

	if (p_ctrl == NULL) {
		Ctrl_Tile *p_tile_data = (Ctrl_Tile *)p_tile->ext;
		for (int i = 0; i < n_ctrls; i++) {
			// check if parent is asociated with ctrl i
			if (p_tile_data->p_impls[i].type == CTRL_TYPE_NULL) continue;

			Ctrl_AddTask(&p_ctrl_global[i], CTRL_TASK_TYPE_FREETILE, p_tile);
		}
	} else {
		Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_FREETILE, p_tile);
	}
}

void Ctrl_MoveToInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_MOVETO, p_tile);
}

void Ctrl_MoveFromInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_MOVEFROM, p_tile);
}

void Ctrl_WaitTileInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_WAITTILE, p_tile);
}

void Ctrl_Hosttask_Sync() {
	Ctrl_TaskQueue_Synchronize(p_ctrl_host_stream);
}

void Ctrl_Synchronize() {
	for (int i = 0; i < Ctrl_GetNCtrls(); i++) {
		if (p_ctrl_global[i].p_impl != NULL) Ctrl_GlobalSync(&p_ctrl_global[i]);
	}

	Ctrl_Hosttask_Sync();
}

void Ctrl_SetDependanceMode(Ctrl *p_ctrl, int mode) {
	Ctrl_AddTaskFlagged(p_ctrl, CTRL_TASK_TYPE_SETDEPENDANCEMODE, NULL, mode);
}

Ctrl_Info Ctrl_GetInfo(Ctrl *p_ctrl) {
	Ctrl_Info info     = CTRL_INFO_NULL;
	info.host_affinity = host_node;
	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			Ctrl_Cpu_GetInfo(&(p_ctrl->p_impl->cpu), &info);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_GetInfo(&(p_ctrl->p_impl->cuda), &info);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_GetInfo(&(p_ctrl->p_impl->hip), &info);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_GetInfo(&(p_ctrl->p_impl->opencl_gpu), &info);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_GetInfo(&(p_ctrl->p_impl->fpga), &info);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl_GetInfo] Unsupported architecture. Recompile Ctrl library with the proper support.\n");
			exit(EXIT_FAILURE);
	}
	return info;
}

int Ctrl_Dev(Ctrl_Type type, int *avail_impls, int n_impl) {
	int result = -1;
	switch (type) {
		case CTRL_TYPE_CPU:
			for (int i = 0; i < n_impl; i++) {
				if (avail_impls[i] > result) {
					if (CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], CPU)) {
						result = avail_impls[i];
					}
					#ifdef _CTRL_MKL_
					else if (avail_impls[i] == CPULIB_MKL) {
						result = avail_impls[i];
					}
					#endif // _CTRL_MKL_
				}
			}
			break;
		case CTRL_TYPE_CUDA:
			for (int i = 0; i < n_impl; i++) {
				if (avail_impls[i] > result) {
					if (CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], CUDA)) {
						result = avail_impls[i];
					} else if (CTRL_IMPL_IN_RANGE(avail_impls[i], CUDALIB)) {
						switch (avail_impls[i]) {
							case CUDALIB_DEFAULT:
								result = avail_impls[i];
								break;

							#ifdef _CTRL_CUBLAS_
							case CUDALIB_CUBLAS:
								result = avail_impls[i];
								break;
							#endif // _CTRL_CUBLAS_

							#ifdef _CTRL_MAGMA_
							case CUDALIB_MAGMA:
								result = avail_impls[i];
								break;
							#endif // _CTRL_MAGMA_
							default:
								#ifdef _CTRL_DEBUG_
								fprintf(stderr, "Warning: ignoring kernel implementation of type %d. Consider recompiling with proper support\n", avail_impls[i]);
								fflush(stderr);
								#endif // _CTRL_DEBUG_
								break;
						}
					}
				}
			}
			break;
		case CTRL_TYPE_HIP:
			for (int i = 0; i < n_impl; i++) {
				if (avail_impls[i] > result) {
					if (CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], HIP)) {
						result = avail_impls[i];
					} else if (CTRL_IMPL_IN_RANGE(avail_impls[i], HIPLIB)) {
						switch (avail_impls[i]) {
							case HIPLIB_DEFAULT:
								result = avail_impls[i];
								break;

							#ifdef _CTRL_HIPBLAS_
							case HIPLIB_HIPBLAS:
								result = avail_impls[i];
								break;
							#endif // _CTRL_HIPBLAS_
							default:
								#ifdef _CTRL_DEBUG_
								fprintf(stderr, "Warning: ignoring kernel implementation of type %d. Consider recompiling with proper support\n", avail_impls[i]);
								#endif // _CTRL_DEBUG_
								break;
						}
					}
				}
			}
			break;
		case CTRL_TYPE_OPENCL_GPU:
			for (int i = 0; i < n_impl; i++) {
				if (avail_impls[i] > result) {
					if (CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], OPENCLGPU) || CTRL_IMPL_IN_RANGE(avail_impls[i], OPENCLGPULIB)) {
						result = avail_impls[i];
					}
				}
			}
			break;
		case CTRL_TYPE_FPGA:
			for (int i = 0; i < n_impl; i++) {
				if (avail_impls[i] > result) {
					if (CTRL_IMPL_IN_RANGE(avail_impls[i], FPGA) || CTRL_IMPL_IN_RANGE(avail_impls[i], FPGALIB)) {
						result = avail_impls[i];
					}
				}
			}
			break;
		case CTRL_TYPE_NULL:
			fprintf(stderr, "[Ctrl_Core] Ctrl_Dev: null ctrl type\n");
			exit(EXIT_FAILURE);
	}
	return result;
}

void Ctrl_EvalTask(Ctrl_Task *p_task) {
	// Evaluation of task
	switch (p_task->task_type) {
		case CTRL_TASK_TYPE_HOST:
			p_task->pfn_hostTask_wrapper(p_task->p_arguments);
			break;
		case CTRL_TASK_TYPE_WAITEVENT:
			Ctrl_GenericEvent_Wait(p_task->event);
			Ctrl_GenericEvent_Release(p_task->event);
			break;
		case CTRL_TASK_TYPE_SIGNALEVENT:
			Ctrl_GenericEvent_Signal(p_task->event);
			break;
		default:
			fprintf(stderr, "[Core EvalTask] Task type not implemented on this stream\n");
			exit(EXIT_FAILURE);
			break;
	}
}

int Ctrl_GetNumThreads(Ctrl *p_ctrl) {
	int default_num_threads = 0;
	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			return default_num_threads + Ctrl_Cpu_GetNumThreads(&(p_ctrl->p_impl->cpu));
		#endif // _CTRL_ARCH_CPU_
		default:
			return default_num_threads;
	}
}

int Ctrl_GetNumQueues(Ctrl *p_ctrl) {
	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			return Ctrl_Cpu_GetNumQueues(&(p_ctrl->p_impl->cpu));
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			return Ctrl_Cuda_GetNumQueues(&(p_ctrl->p_impl->cuda));
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			return Ctrl_Hip_GetNumQueues(&(p_ctrl->p_impl->hip));
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			return Ctrl_OpenCLGpu_GetNumQueues(&(p_ctrl->p_impl->opencl_gpu));
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			return Ctrl_FPGA_GetNumQueues(&(p_ctrl->p_impl->fpga));
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl_GetNumQueues] Unsupported architecture %d. Recompile Ctrl library with the proper support.\n", p_ctrl->type);
			exit(EXIT_FAILURE);
	}
}

Ctrl_TaskQueue **Ctrl_GetHostQueues(Ctrl *p_ctrl, Ctrl_TaskQueue **pp_queues) {
	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			return Ctrl_Cpu_GetHostQueues(&(p_ctrl->p_impl->cpu), pp_queues);
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			return Ctrl_Cuda_GetHostQueues(&(p_ctrl->p_impl->cuda), pp_queues);
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			return Ctrl_Hip_GetHostQueues(&(p_ctrl->p_impl->hip), pp_queues);
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			return Ctrl_OpenCLGpu_GetHostQueues(&(p_ctrl->p_impl->opencl_gpu), pp_queues);
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			return Ctrl_FPGA_GetHostQueues(&(p_ctrl->p_impl->fpga), pp_queues);
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl_GetHostQueues] Unsupported architecture. Recompile Ctrl library with the proper support.\n");
			exit(EXIT_FAILURE);
	}
}

void Ctrl_SetPolicy(Ctrl_Policy p) {
	policy = p;
}

/**
 * Helper function to read a file to a string.
 *
 * @param file path to file
 * @return char* string with the contents of \p file . Must be freed after use.
 */
char *read_file(const char *file) {
	FILE *f = fopen(file, "r");

	if (!f) {
		fprintf(stderr, "ERROR: File for devices selection could not be opened: %s\n", file);
		exit(EXIT_FAILURE);
	}

	fseek(f, 0, SEEK_END);
	size_t length = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buffer = (char *)malloc(length + 1);

	size_t read_length = fread(buffer, 1, length, f);
	if (length != read_length) {
		free(buffer);
		fprintf(stderr, "ERROR: Reading devices selection file: %s\n", file);
		exit(EXIT_FAILURE);
	}
	fclose(f);

	buffer[length] = '\0';
	return buffer;
}

void Ctrl_ParseConfig(const char *file) {
	// READ CONFIG FILE IN A STRING BUFFER
	char *buffer = read_file(file);

	// GET HOST NAME AND RANK IN NODE
	char *hostname     = hit_comNodeName();
	int   rank_in_node = hit_comNodeGroupRank();

	// WEIGHTS: GET NUMBER OF ACTIVE PROCESSES TO CREATE ARRAY
	HitTopology com_topo         = hit_topology(plug_topPlain);
	int         global_num_procs = hit_topDimCard(com_topo, 0);
	int         global_rank      = hit_topDimRank(com_topo, 0);
	float       com_weight       = 1.0f; // Default value
	ctrl_weights.ratios          = (float *)malloc(sizeof(float) * global_num_procs);
	for (int i = 0; i < global_num_procs; i++)
		ctrl_weights.ratios[global_rank] = 1.0f; // Default value
	ctrl_weights.num_procs = global_num_procs;
	hit_topFree(com_topo);

	// LOCATE START OF THIS MACHINE NAME SECTION
	char node_name[MPI_MAX_PROCESSOR_NAME + 6]; // extra space for the "NODE " string
	sprintf(node_name, "node %s", hostname);

	char *node_data = strstr(buffer, node_name);
	if (node_data == NULL) {
		// FALLBACK: TRY TO LOCATE WILDCARD NODE SECTION
		sprintf(node_name, "node *");
		node_data = strstr(buffer, node_name);

		if (node_data == NULL) {
			fprintf(stderr, "ERROR: Device selection file -- No NODE section in file %s for node %s\n", file, hostname);
			exit(EXIT_FAILURE);
		}
	}
	// LOCATE END OF THIS MACHINE NAME SECTION AND CLEAN THE REST OF THE BUFFER
	char *end_node_data = strstr(node_data + 4, "node ");
	if (end_node_data != NULL) *end_node_data = '\0';

	// LOCATE START OF THIS RANK SECTION
	char proc_str[10];
	sprintf(proc_str, "proc %d ", rank_in_node);
	char *rank_data = strstr(node_data, proc_str);

	char proc_str_noaff[10];
	sprintf(proc_str_noaff, "proc %d\n", rank_in_node);
	char *rank_data_noaff = strstr(node_data, proc_str_noaff);
	if (rank_data == NULL && rank_data_noaff == NULL) {
		fprintf(stderr, "ERROR: Device selection file -- No config found in file %s for rank %d of node %s\n", file, rank_in_node, hostname);
		exit(EXIT_FAILURE);
	} else if (rank_data != NULL && rank_data_noaff != NULL) {
		fprintf(stderr, "ERROR: Device selection file -- Multiple configs found in file %s for rank %d of node %s\n", file, rank_in_node, hostname);
		exit(EXIT_FAILURE);
	}
	rank_data = rank_data == NULL ? rank_data_noaff : rank_data;

	// LOCATE END OF THIS RANK SECTION AND CLEAN THE REST OF THE BUFFER
	char *end_rank_data = strstr(rank_data + 4, "proc ");
	if (end_rank_data != NULL) *end_rank_data = '\0';

	// AFFINITY: GET (OPTIONAL) NUMA NODE IDENTIFIER FOR HOST CODE
	// SKIP "PROC" STRING AND RANK ID
	int foo, aff;
	int count = sscanf(rank_data, "proc %d %d\n", &foo, &aff);
	if (count < 1) {
		fprintf(stderr, "INTERNAL ERROR: Device selection file -- Internal proc rank missed! %d\n", rank_in_node);
		exit(EXIT_FAILURE);
	}
	if (foo != rank_in_node) {
		fprintf(stderr, "INTERNAL ERROR: Proc rank has changed during parsing! %d != %d\n", rank_in_node, foo);
		exit(EXIT_FAILURE);
	}
	// AFFINITY SPECIFICATION FOUND FOR THIS RANK
	if (count == 2) host_node = aff;

	// CONSUME LINE
	rank_data = strstr(rank_data, "\n");
	if (rank_data == NULL) {
		fprintf(stderr, "ERROR: Device selection file -- No devices, premature end of section in file %s, node %s, rank %d\n", file, hostname, rank_in_node);
		exit(EXIT_FAILURE);
	}
	// SKIP NEW LINE CHAR
	rank_data++;

	// READ DEVICES
	Ctrl_Config *devs = (Ctrl_Config *)malloc(sizeof(Ctrl_Config));

	int ndevs                           = 0;
	int ncpu                            = 0;
	int noclgpu __attribute__((unused)) = 0;
	int nfpga __attribute__((unused))   = 0;

	for (char *tok = strtok(rank_data, " \t\n");
		 tok != NULL;
		 tok = strtok(NULL, " \t\n")) {

		ndevs++;
		devs = (Ctrl_Config *)realloc(devs, ndevs * sizeof(Ctrl_Config));

		if (!strcmp(tok, "cpu")) {
			ncpu++;
			devs[ndevs - 1] = (Ctrl_Config){.type = CTRL_TYPE_CPU, .args = strtok(NULL, "\n")};
		} else if (!strcmp(tok, "cuda")) {
			devs[ndevs - 1] = (Ctrl_Config){.type = CTRL_TYPE_CUDA, .args = strtok(NULL, "\n")};
		} else if (!strcmp(tok, "hip")) {
			devs[ndevs - 1] = (Ctrl_Config){.type = CTRL_TYPE_HIP, .args = strtok(NULL, "\n")};
		} else if (!strcmp(tok, "opencl")) {
			noclgpu++;
			devs[ndevs - 1] = (Ctrl_Config){.type = CTRL_TYPE_OPENCL_GPU, .args = strtok(NULL, "\n")};
		} else if (!strcmp(tok, "fpga")) {
			nfpga++;
			devs[ndevs - 1] = (Ctrl_Config){.type = CTRL_TYPE_FPGA, .args = strtok(NULL, "\n")};
		} else if (!strcmp(tok, "weight")) {
			ndevs--; // This entry is not a device
			char *weight_str = strtok(NULL, "\n");
			int   ok         = sscanf(weight_str, "%f\n", &com_weight);
			if (ok != 1) {
				fprintf(stderr, "ERROR: Device selection file -- Non readable weight value in file %s, node %s, proc %d, value string: %s\n", file, hostname, rank_in_node, tok);
				exit(EXIT_FAILURE);
			}
		} else {
			fprintf(stderr, "ERROR: Device selection file -- Unknown device type in file %s, node %s, proc %d, device name: %s\n", file, hostname, rank_in_node, tok);
			exit(EXIT_FAILURE);
		}
	}

	// WEIGHTS: COMMUNICATE WEIGHTS
	MPI_Allgather(&com_weight, 1, MPI_FLOAT, ctrl_weights.ratios, 1, MPI_FLOAT, MPI_COMM_WORLD);

	// LOAD HARDWARE TOPOLOGY INFO
	hwloc_topology_init(&topo);
	hwloc_topology_set_flags(topo, HWLOC_TOPOLOGY_FLAG_INCLUDE_DISALLOWED);
	hwloc_topology_load(topo);

	// INITIALIZE CONTROLLERS LIST
	n_ctrls       = ndevs;
	p_ctrl_global = (Ctrl *)malloc(ndevs * sizeof(Ctrl));
	omp_set_num_threads(3 + ncpu);

	if (policy == CTRL_POLICY_DEFAULT) {
		if (getenv("CTRL_POLICY")) {
			if (!strcmp(getenv("CTRL_POLICY"), "SYNC")) {
				policy = CTRL_POLICY_SYNC;
			} else {
				policy = CTRL_POLICY_ASYNC;
			}
		} else {
			policy = CTRL_POLICY_ASYNC;
		}
	}

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	Ctrl_OpenCLGpu_AllocKernel(noclgpu);
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#ifdef _CTRL_ARCH_FPGA_
	Ctrl_FPGA_AllocKernel(nfpga);
	#endif // _CTRL_ARCH_FPGA_

	// CREATE HOST TASK QUEUE
	p_ctrl_host_stream = Ctrl_TaskQueue_Create();

	// CREATE CONTROLLERS
	for (int i = 0; i < ndevs; i++) {
		Ctrl_Create(devs[i].type, i, devs[i].args);
	}

	// FREE TEMP DATA STRUCTURES
	free(devs);
	free(buffer);
}

HitWeights Ctrl_ConfigWeights() {
	return ctrl_weights;
}

bool Ctrl_PushEventIfCompat(Ctrl_GenericEvent event, int qid, Ctrl *p_ctrl) {
	if (!Ctrl_Event_CheckCompat(event, p_ctrl->type, p_ctrl->id))
		return false;

	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = CTRL_TASK_TYPE_WAITEVENT;
	task.stream    = qid;
	task.event     = event;

	switch (p_ctrl->type) {
		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_ExecTask(&task, &p_ctrl->p_impl->cuda);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_ExecTask(&task, &p_ctrl->p_impl->hip);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_ExecTask(&task, &p_ctrl->p_impl->opencl_gpu);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_ExecTask(&task, &p_ctrl->p_impl->fpga);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			fprintf(stderr, "[Ctrl Core] Queue manager: Invalid ctrl type %d, task type %d (wait event push)\n", p_ctrl->type, task.task_type);
			exit(EXIT_FAILURE);
			break;
	}
	return true;
}

void Ctrl_Thread_QueueManager() {
	int n_queues = 0;
	int p_nqueues[n_ctrls];
	int p_displacements[n_ctrls];
	for (int cid = 0; cid < n_ctrls; cid++) {
		p_nqueues[cid] = Ctrl_GetNumQueues(&p_ctrl_global[cid]);
		n_queues += p_nqueues[cid];
		p_displacements[cid] = cid == 0 ? 0 : p_displacements[cid - 1] + p_nqueues[cid - 1];
	}

	if (n_queues == 0) {
		return;
	}

	Ctrl_TaskQueue  *pp_queues[n_queues];
	Ctrl_TaskQueue **pp_aux = pp_queues;
	for (int i = 0; i < n_ctrls; i++) {
		pp_aux = Ctrl_GetHostQueues(&p_ctrl_global[i], pp_aux);
	}

	while (true) {
		for (int cid = 0; cid < n_ctrls; cid++) {
			for (int qid = 0; qid < p_nqueues[cid]; qid++) {
				// global qid
				int gqid = p_displacements[cid] + qid;

				Ctrl_Task *p_task = Ctrl_TaskQueue_GetNext(pp_queues[gqid]);
				if (p_task == NULL) {
					continue;
				}

				Ctrl *p_ctrl = &p_ctrl_global[cid];

				switch (p_task->task_type) {
					case CTRL_TASK_TYPE_KERNEL:
					case CTRL_TASK_TYPE_MOVEFROM:
					case CTRL_TASK_TYPE_MOVETO:
						p_task->stream = qid;
						Ctrl_TaskQueue_Pop(pp_queues[gqid]);
						switch (p_ctrl->type) {
							#ifdef _CTRL_ARCH_CUDA_
							case CTRL_TYPE_CUDA:
								Ctrl_Cuda_ExecTask(p_task, &p_ctrl->p_impl->cuda);
								break;
							#endif // _CTRL_ARCH_CUDA_

							#ifdef _CTRL_ARCH_HIP_
							case CTRL_TYPE_HIP:
								Ctrl_Hip_ExecTask(p_task, &p_ctrl->p_impl->hip);
								break;
							#endif // _CTRL_ARCH_HIP_

							#ifdef _CTRL_ARCH_OPENCL_GPU_
							case CTRL_TYPE_OPENCL_GPU:
								Ctrl_OpenCLGpu_ExecTask(p_task, &p_ctrl->p_impl->opencl_gpu);
								break;
							#endif // _CTRL_ARCH_OPENCL_GPU_

							#ifdef _CTRL_ARCH_FPGA_
							case CTRL_TYPE_FPGA:
								Ctrl_FPGA_ExecTask(p_task, &p_ctrl->p_impl->fpga);
								break;
							#endif // _CTRL_ARCH_FPGA_
							default:
								fprintf(stderr, "[Ctrl Core] Queue manager: Unknown ctrl type %d, task type %d\n", p_ctrl->type, p_task->task_type);
								exit(EXIT_FAILURE);
								break;
						}
						#pragma omp atomic update
						pp_queues[gqid]->last_finished++;
						break;
					case CTRL_TASK_TYPE_WAITEVENT:
						// use short circuit to push the event to driver stream if possible instead of waiting on the host
						if (Ctrl_PushEventIfCompat(p_task->event, qid, p_ctrl) || Ctrl_GenericEvent_Test(p_task->event)) {
							Ctrl_TaskQueue_Pop(pp_queues[gqid]);
							Ctrl_GenericEvent_Release(p_task->event);
							#pragma omp atomic update
							pp_queues[gqid]->last_finished++;
						}
						break;
					case CTRL_TASK_TYPE_DESTROYCTRL:
						for (int j = 0; j < n_queues; j++) {
							Ctrl_TaskQueue_Destroy(pp_queues[j]);
						}
						return;
					default:
						fprintf(stderr, "[Ctrl Core] Queue manager: Unknown task type %d\n", p_task->task_type);
						exit(EXIT_FAILURE);
						break;
				}
			}
		}
	}
}

void Ctrl_SyncWait(Ctrl_TaskQueue *p_queue) {
	if (policy == CTRL_POLICY_SYNC) {
		for (int i = 0; i < Ctrl_GetNCtrls(); i++) {
			switch (p_ctrl_global[i].type) {
				#ifdef _CTRL_ARCH_CPU_
				case CTRL_TYPE_CPU:
					Ctrl_Cpu_SyncWait(&p_ctrl_global[i].p_impl->cpu, p_queue);
					break;
				#endif // _CTRL_ARCH_CPU_

				#ifdef _CTRL_ARCH_CUDA_
				case CTRL_TYPE_CUDA:
					Ctrl_Cuda_SyncWait(&p_ctrl_global[i].p_impl->cuda, p_queue);
					break;
				#endif // _CTRL_ARCH_CUDA_

				#ifdef _CTRL_ARCH_HIP_
				case CTRL_TYPE_HIP:
					Ctrl_Hip_SyncWait(&p_ctrl_global[i].p_impl->hip, p_queue);
					break;
				#endif // _CTRL_ARCH_HIP_

				#ifdef _CTRL_ARCH_OPENCL_GPU_
				case CTRL_TYPE_OPENCL_GPU:
					Ctrl_OpenCLGpu_SyncWait(&p_ctrl_global[i].p_impl->opencl_gpu, p_queue);
					break;
				#endif // _CTRL_ARCH_OPENCL_GPU_

				#ifdef _CTRL_ARCH_FPGA_
				case CTRL_TYPE_FPGA:
					Ctrl_FPGA_SyncWait(&p_ctrl_global[i].p_impl->fpga, p_queue);
					break;
				#endif // _CTRL_ARCH_FPGA_
				default:
					fprintf(stderr, "[Ctrl_Core] Ctrl_SyncWait: unknown ctrl type %d", p_ctrl_global[i].type);
					exit(EXIT_FAILURE);
			}
		}
	}
}

void Ctrl_MoveToWait(Ctrl_Tile_Impl *p_tile_impl, Ctrl_TaskQueue *p_queue) {
	switch (p_tile_impl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			Ctrl_Cpu_MoveToWait(p_tile_impl->tile.p_cpu, p_queue);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_MoveToWait(p_tile_impl->tile.p_cuda, p_queue);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_MoveToWait(p_tile_impl->tile.p_hip, p_queue);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_MoveToWait(p_tile_impl->tile.p_opencl, p_queue);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_MoveToWait(p_tile_impl->tile.p_fpga, p_queue);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			break;
	}
}

void Ctrl_MoveFromWait(Ctrl_Tile_Impl *p_tile_impl, Ctrl_TaskQueue *p_queue) {
	switch (p_tile_impl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			Ctrl_Cpu_MoveFromWait(p_tile_impl->tile.p_cpu, p_queue);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_MoveFromWait(p_tile_impl->tile.p_cuda, p_queue);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_MoveFromWait(p_tile_impl->tile.p_hip, p_queue);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_MoveFromWait(p_tile_impl->tile.p_opencl, p_queue);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_MoveFromWait(p_tile_impl->tile.p_fpga, p_queue);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			break;
	}
}

void Ctrl_HostTaskWait(Ctrl_Tile_Impl *p_tile_impl, char rol, Ctrl_TaskQueue *p_queue) {
	switch (p_tile_impl->type) {
		#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			Ctrl_Cpu_HostTaskWait(p_tile_impl->tile.p_cpu, rol, p_queue);
			break;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		case CTRL_TYPE_CUDA:
			Ctrl_Cuda_HostTaskWait(p_tile_impl->tile.p_cuda, rol, p_queue);
			break;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		case CTRL_TYPE_HIP:
			Ctrl_Hip_HostTaskWait(p_tile_impl->tile.p_hip, rol, p_queue);
			break;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		case CTRL_TYPE_OPENCL_GPU:
			Ctrl_OpenCLGpu_HostTaskWait(p_tile_impl->tile.p_opencl, rol, p_queue);
			break;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		case CTRL_TYPE_FPGA:
			Ctrl_FPGA_HostTaskWait(p_tile_impl->tile.p_fpga, rol, p_queue);
			break;
		#endif // _CTRL_ARCH_FPGA_
		default:
			break;
	}
}

void Ctrl_FreeHostInner(HitTile *p_tile) {
	Ctrl_Tile *p_tile_data = (Ctrl_Tile *)(p_tile->ext);

	// Destroy events of this tile
	Ctrl_GenericEvent_Release(p_tile_data->last_host_read_event);
	Ctrl_GenericEvent_Release(p_tile_data->last_host_write_event);

	if (p_tile->memStatus == HIT_MS_OWNER) {
		// Free host image of the tile, equivalent to hit_tileFree(*p_tile);
		if (p_tile_data->host_status != CTRL_TILE_UNALLOC) {
			// TODO @sergioalo move stuff to apropiate backend
			switch (p_tile_data->pinned) {
				case CTRL_TYPE_CPU:
					// global hwloc topology is the same that cpu ctrls use
					hwloc_free(topo, p_tile->data, (size_t)p_tile->acumCard * p_tile->baseExtent);
					break;

				#ifdef _CTRL_ARCH_CUDA_
				case CTRL_TYPE_CUDA:
					CUDA_OP(cudaFreeHost(p_tile->data));
					break;
				#endif //_CTRL_ARCH_CUDA_

				#ifdef _CTRL_ARCH_HIP_
				case CTRL_TYPE_HIP:
					HIP_OP(hipHostFree(p_tile->data));
					break;
				#endif //_CTRL_ARCH_HIP_

				#if defined(_CTRL_ARCH_OPENCL_GPU_) || defined(_CTRL_ARCH_FPGA_)
				case CTRL_TYPE_OPENCL_GPU:
					OPENCL_ASSERT_OP(clEnqueueUnmapMemObject((cl_command_queue)p_tile_data->p_pin_queue, (cl_mem)p_tile_data->p_pinned_data, p_tile->data, 0, NULL, NULL));
					OPENCL_ASSERT_OP(clFlush((cl_command_queue)p_tile_data->p_pin_queue));
					OPENCL_ASSERT_OP(clFinish((cl_command_queue)p_tile_data->p_pin_queue));
					OPENCL_ASSERT_OP(clReleaseMemObject((cl_mem)p_tile_data->p_pinned_data));
					OPENCL_ASSERT_OP(clReleaseCommandQueue((cl_command_queue)p_tile_data->p_pin_queue));
					break;
				#endif //_CTRL_ARCH_OPENCL_GPU_ || _CTRL_ARCH_FPGA_
				default:
					free(p_tile->data);
					break;
			}
		}
		p_tile->memPtr    = NULL;
		p_tile->data      = NULL;
		p_tile->memStatus = HIT_MS_NOMEM;
	}

	// Clear tile fields
	p_tile->ext = NULL;

	// Free tile
	free(p_tile_data->p_impls);
	free(p_tile_data);
}

///@endcond
