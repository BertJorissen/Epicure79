///@cond INTERNAL
/**
 * @file Ctrl_Core.c
 * @author Trasgo Group
 * @brief Source code for core functions of Controllers.
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

#include "Core/Ctrl_Core.h"

/**
 * Contains the ctrl.
 */
Ctrl Ctrl_global;

/**
 * Topology of the machine.
 */
hwloc_topology_t topo;

/**
 * Index of the numa node to use for host threads.
 */
int host_node = 0;

Ctrl_TaskQueue *p_ctrl_host_stream = NULL;

#ifdef _CTRL_QUEUE_
/**
 * Extract and evaluate tasks from the queue inside \p p_ctrl until a task of type CTRL_TASK_TYPE_DESTROYCNTRL.
 * Only active if queues are active via compilation flags.
 * 
 * @param p_ctrl pointer to the ctrl containing the queue to extract tasks from.
 * 
 * @see Ctrl_ExecTask
 */
void Ctrl_ConsumeTaskQueue(Ctrl *p_ctrl);
#endif //_CTRL_QUEUE_

/**
 * Evaluate \p p_task.
 * 
 * This calls to the appropiate eval_task depending on \p p_ctrl's type.
 * 
 * @param p_ctrl pointer to the ctrl the task was sent to.
 * @param p_task pointer to the task to be evaluated.
 * 
 * @see Ctrl_Cpu_EvalTask, Ctrl_Cuda_EvalTask, Ctrl_OpenCLGpu_EvalTask
 */
void Ctrl_ExecTask(Ctrl *p_ctrl, Ctrl_Task *p_task);

/**
 * If ctrl queues are active add a task of type \p type to \p p_ctrl inner queue, if ctrl queues are not active pass that
 * task to \e Ctrl_ExecTask instead.
 * 
 * @param p_ctrl ctrl to send the task to.
 * @param type type of the task to send.
 * @param p_tile tile to place inside the task. This can be null for certain task's type.
 * 
 * @see Ctrl_ExecTask, Ctrl_ConsumeTaskQueue, Ctrl_TaskQueue_Push
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
 * @see Ctrl_ExecTask, Ctrl_ConsumeTaskQueue, Ctrl_TaskQueue_Push, Ctrl_AllocInner, Ctrl_SelectInner
 */
void Ctrl_AddTaskFlagged(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile, int flags);

/**
 * Evaluation for host tasks and events by the host task thread.
 * 
 * @param p_task task to evaluate.
 * 
 * @see Ctrl_Thread_HostTask_Thread
 */
void Ctrl_EvalTask(Ctrl_Task *p_task);

/**
 * Sync with queue manager thread of \p p_ctrl if queues are enabled, if queues are not enabled this function does nothing.
 * 
 * @param p_ctrl pointer to the ctrl to perform the syncronization.
 */
void Ctrl_Sync(Ctrl *p_ctrl);

/**
 * Sync with queue manager threadof \p p_ctrl if queues are enabled and policy of \p p_ctrl is CTRL_POLICY_SYNC, if queues 
 * are not enabled this function does nothing.
 * 
 * @param p_ctrl Ctrl to sync with.
 */
void Ctrl_SyncIfSync(Ctrl *p_ctrl);

/**
 * Function for extra threads for ctrls. If extra threads are needed for a ctrl, this function takes the spawner thread created
 * and divides it and distributes it as needed depending on the configuration of \e Ctrl_global.
 * 
 * @see Ctrl_Thread_Init
 */
void Ctrl_Thread_EvalThread();

/**
 * Function for host task thread. Creates host task queue and starts executing tasks from it until a destroy task arrives to the queue.
 * 
 * @see Ctrl_Thread_Init, Ctrl_EvalTask
 */
void Ctrl_Thread_HostTask_Thread();

/**
 * Calculates the number of threads needed for \p p_ctrl.
 * 
 * @param p_ctrl Ctrl we want to get the number of extra threads of.
 * 
 * @return Number of threads needed for \p p_ctrl.
 */
int Ctrl_GetNumThreads(Ctrl *p_ctrl);

int Ctrl_Thread_Init() {
	#pragma omp single
	{
		hwloc_topology_init(&topo);
		hwloc_topology_load(topo);
	}
	hwloc_obj_t obj;
	if (omp_get_thread_num() == 0) {
		// bind thread to first PU
		obj = hwloc_get_obj_below_by_type(topo,HWLOC_OBJ_NUMANODE, host_node, HWLOC_OBJ_CORE, 0);
		if (obj){
		   hwloc_set_cpubind(topo, obj->cpuset, HWLOC_CPUBIND_THREAD);
		}
		return 0;
	} else if (omp_get_thread_num() == 1) { //host task executor
		// bind thread to first PU of core 1
		obj = hwloc_get_obj_below_by_type(topo,HWLOC_OBJ_NUMANODE, host_node, HWLOC_OBJ_CORE, 1);
		if (obj){
		   hwloc_set_cpubind(topo, obj->cpuset, HWLOC_CPUBIND_THREAD);
		}
		Ctrl_Thread_HostTask_Thread();
		#ifdef _CTRL_QUEUE_
		#pragma omp barrier
		#endif //_CTRL_QUEUE_
	} else if (omp_get_thread_num() == 2){     //spawner thread
		Ctrl_Thread_EvalThread();
	}

	#pragma omp barrier
	#pragma omp single nowait
	{
		hwloc_topology_destroy(topo);
	}
	return 1;
}

void Ctrl_Thread_HostTask_Thread(){
	// Create host task queue
	p_ctrl_host_stream=(Ctrl_TaskQueue *)malloc(sizeof(Ctrl_TaskQueue));
	Ctrl_TaskQueue_Init(p_ctrl_host_stream);

	// Let other threads continue
	#ifdef _CTRL_QUEUE_
	#pragma omp barrier
	#endif //_CTRL_QUEUE_
	#pragma omp barrier

	// Extract and evaluate tasks until destroy task
	bool finish = false;
	while (!finish) {
		Ctrl_Task *p_task = NULL;
		p_task = Ctrl_TaskQueue_Pop(p_ctrl_host_stream);
		if (p_task->task_type == CTRL_TASK_TYPE_DESTROYCNTRL) {
			finish = true;
		} else {
			Ctrl_EvalTask(p_task);
			// Update queue last_finished index
			#pragma omp atomic update
				p_ctrl_host_stream->last_finished++;
			// free stuff inside task
			if (p_task->task_type != CTRL_TASK_TYPE_HOST){
				Ctrl_TaskQueue_FreeTask(p_task);
			}	
		}
	}
	// Destroy queue (clean up)
	Ctrl_TaskQueue_Destroy(p_ctrl_host_stream);
	free(p_ctrl_host_stream);
}

void Ctrl_Thread_EvalThread() {
	// Wait for ctrl to be created
	#pragma omp barrier
	Ctrl ctrl = Ctrl_global;

	#ifdef _CTRL_QUEUE_
	// sync with main thread again
	omp_set_lock(ctrl.p_lock_ctrl);
	#pragma omp barrier
	#endif //_CTRL_QUEUE_
	
	// Divide thread if ctrl needs more threads
	#pragma omp parallel num_threads(Ctrl_GetNumThreads(&ctrl))
	{
		#ifdef _CTRL_QUEUE_
		if(omp_get_thread_num()==0){    //queue manager
			// bind thread to first PU of core 2
			hwloc_obj_t obj = hwloc_get_obj_below_by_type(topo,HWLOC_OBJ_NUMANODE, host_node, HWLOC_OBJ_CORE, 2);
			if (obj){
				hwloc_set_cpubind(topo, obj->cpuset, HWLOC_CPUBIND_THREAD);
			}
			Ctrl_ConsumeTaskQueue(&ctrl);

		}else{                          //extra threads
		#endif //_CTRL_QUEUE_
			// Distribute new threads
			switch(ctrl.type){
				#ifdef _CTRL_ARCH_CPU_
					case CTRL_TYPE_CPU:
						Ctrl_Cpu_ThreadInit(&(ctrl.p_impl->cpu), topo, host_node);
						break;
				#endif // _CTRL_ARCH_CPU_

				default:
					printf("Ctrls of type %d don't know how to use these extra threads!", ctrl.type);
					exit(EXIT_FAILURE);
			}
		#ifdef _CTRL_QUEUE_
		}
		#endif // _CTRL_QUEUE_
	}

	#ifdef _CTRL_QUEUE_
	#pragma omp barrier
	omp_unset_lock(ctrl.p_lock_ctrl);
	#endif //_CTRL_QUEUE_
}

void Ctrl_CreateWrapper(Ctrl_Type type, Ctrl *p_ctrl, Ctrl_Policy policy) {
	Ctrl_global.p_impl = (Ctrl_Impl *)malloc(sizeof(Ctrl_Impl));
	#ifdef _CTRL_QUEUE_
	Ctrl_global.p_task_queue = (Ctrl_TaskQueue *)malloc(sizeof(Ctrl_TaskQueue));
	Ctrl_global.p_lock_first_host = (omp_lock_t *)malloc(sizeof(omp_lock_t));
	Ctrl_global.p_lock_first_ctrl = (omp_lock_t *)malloc(sizeof(omp_lock_t));
	Ctrl_global.p_lock_host = (omp_lock_t *)malloc(sizeof(omp_lock_t));
	Ctrl_global.p_lock_ctrl = (omp_lock_t *)malloc(sizeof(omp_lock_t));
	#endif //_CTRL_QUEUE_

	p_ctrl->type = type;

	if(policy != CTRL_POLICY_DEFAULT) {
		p_ctrl->policy = policy;
	} else {
		if ( getenv("CTRL_ASYNC_MODE") ) {
			if (atoi( getenv("CTRL_ASYNC_MODE") ) > 0) {
				p_ctrl->policy = CTRL_POLICY_ASYNC;
			} else {
				p_ctrl->policy = CTRL_POLICY_SYNC;
			}
		} else {
			p_ctrl->policy = CTRL_POLICY_SYNC;
		}
	}
	
	#ifdef _CTRL_QUEUE_
	omp_init_lock(p_ctrl->p_lock_first_host);
	omp_init_lock(p_ctrl->p_lock_first_ctrl);
	omp_init_lock(p_ctrl->p_lock_host);
	omp_init_lock(p_ctrl->p_lock_ctrl);

	Ctrl_TaskQueue_Init(p_ctrl->p_task_queue);
	#endif //_CTRL_QUEUE_
}

#ifdef _CTRL_ARCH_FPGA_
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_FPGA(Ctrl_Policy policy, int device, int platform, int exec_mode, int streams) {
		Ctrl_CreateWrapper(CTRL_TYPE_FPGA, &Ctrl_global, policy);

		PCtrl p_ctrl = &Ctrl_global;
		Ctrl_FPGA_Create((&(p_ctrl->p_impl->fpga)), policy, device, platform, exec_mode, streams);
		#ifdef _CTRL_QUEUE_
		p_ctrl->p_impl->fpga.p_lock_first_host = p_ctrl->p_lock_first_host;
		p_ctrl->p_impl->fpga.p_lock_first_ctrl = p_ctrl->p_lock_first_ctrl;
		p_ctrl->p_impl->fpga.p_lock_host = p_ctrl->p_lock_host;
		p_ctrl->p_impl->fpga.p_lock_ctrl = p_ctrl->p_lock_ctrl;
		
		#pragma omp barrier
		omp_set_lock(p_ctrl->p_lock_host);
		#endif //_CTRL_QUEUE_

		#pragma omp barrier
		return &Ctrl_global;
	}
#endif //_CTRL_ARCH_FPGA_
#ifdef _CTRL_ARCH_OPENCL_GPU_
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_OPENCL_GPU(Ctrl_Policy policy, int device, int platform, int streams) {
		Ctrl_CreateWrapper(CTRL_TYPE_OPENCL_GPU, &Ctrl_global, policy);

		PCtrl p_ctrl = &Ctrl_global;
		Ctrl_OpenCLGpu_Create((&(p_ctrl->p_impl->opencl_gpu)), policy, device, platform, streams);
		#ifdef _CTRL_QUEUE_
		p_ctrl->p_impl->opencl_gpu.p_lock_first_host = p_ctrl->p_lock_first_host;
		p_ctrl->p_impl->opencl_gpu.p_lock_first_ctrl = p_ctrl->p_lock_first_ctrl;
		p_ctrl->p_impl->opencl_gpu.p_lock_host = p_ctrl->p_lock_host;
		p_ctrl->p_impl->opencl_gpu.p_lock_ctrl = p_ctrl->p_lock_ctrl;
		
		#pragma omp barrier
		omp_set_lock(p_ctrl->p_lock_host);
		#endif //_CTRL_QUEUE_

		#pragma omp barrier
		return &Ctrl_global;
	}
#endif //_CTRL_ARCH_OPENCL_GPU_
#ifdef _CTRL_ARCH_CUDA_
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_CUDA(Ctrl_Policy policy, int device, int streams) {
		Ctrl_CreateWrapper(CTRL_TYPE_CUDA, &Ctrl_global, policy);

		PCtrl p_ctrl = &Ctrl_global;
		Ctrl_Cuda_Create((&(p_ctrl->p_impl->cuda)), policy, device, streams);
		#ifdef _CTRL_QUEUE_
		p_ctrl->p_impl->cuda.p_lock_first_host = p_ctrl->p_lock_first_host;
		p_ctrl->p_impl->cuda.p_lock_first_ctrl = p_ctrl->p_lock_first_ctrl;
		p_ctrl->p_impl->cuda.p_lock_host = p_ctrl->p_lock_host;
		p_ctrl->p_impl->cuda.p_lock_ctrl = p_ctrl->p_lock_ctrl;
		
		#pragma omp barrier
		omp_set_lock(p_ctrl->p_lock_host);
		#endif //_CTRL_QUEUE_

		#pragma omp barrier
		return &Ctrl_global;
	}
#endif //_CTRL_ARCH_CUDA_
#ifdef _CTRL_ARCH_CPU_
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_CPU(Ctrl_Policy policy, int n_threads, int *p_numa_nodes, int n_numa_nodes, bool mem_moves) {
		Ctrl_CreateWrapper(CTRL_TYPE_CPU, &Ctrl_global, policy);

		PCtrl p_ctrl = &Ctrl_global;
		p_ctrl->p_impl->cpu.topo=topo;
		Ctrl_Cpu_Create(&(p_ctrl->p_impl->cpu), policy, n_threads, p_numa_nodes, n_numa_nodes, mem_moves);
		#ifdef _CTRL_QUEUE_
		p_ctrl->p_impl->cpu.p_lock_first_host = p_ctrl->p_lock_first_host;
		p_ctrl->p_impl->cpu.p_lock_first_ctrl = p_ctrl->p_lock_first_ctrl;
		p_ctrl->p_impl->cpu.p_lock_host = p_ctrl->p_lock_host;
		p_ctrl->p_impl->cpu.p_lock_ctrl = p_ctrl->p_lock_ctrl;
		
		#pragma omp barrier
		omp_set_lock(p_ctrl->p_lock_host);
		#endif //_CTRL_QUEUE_

		#pragma omp barrier
		return &Ctrl_global;
	}
#endif //_CTRL_ARCH_CPU_

#ifdef _CTRL_QUEUE_
void Ctrl_ConsumeTaskQueue(Ctrl *p_ctrl) {
	#ifdef _CTRL_ARCH_CUDA_
	if (p_ctrl->type==CTRL_TYPE_CUDA){
		Ctrl_Cuda_NewThreadSetup(&(p_ctrl->p_impl->cuda));
	}
	#endif // _CTRL_ARCH_CUDA_

	bool finish = false;

	while (finish == false) {
		Ctrl_Task *p_task = NULL;
		p_task = Ctrl_TaskQueue_Pop(p_ctrl->p_task_queue);
		if (p_task->task_type == CTRL_TASK_TYPE_DESTROYCNTRL) {
			finish = true;
		}
		Ctrl_ExecTask(p_ctrl, p_task);
	}
}
#endif //_CTRL_QUEUE_

void Ctrl_ExecTask(Ctrl *p_ctrl, Ctrl_Task *p_task){
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
				fprintf(stderr, "Ctrl_ExecTask: Unsupported architecture. Recompile Ctrl library with the proper support.\n");
				exit(EXIT_FAILURE);
	}
	#ifdef _CTRL_QUEUE_
	#pragma omp atomic update
		p_ctrl->p_task_queue->last_finished++;
	#endif //_CTRL_QUEUE_
	if(p_ctrl->type==CTRL_TYPE_CPU){
		if (p_task->task_type != CTRL_TASK_TYPE_HOST && p_task->task_type != CTRL_TASK_TYPE_KERNEL) {
			Ctrl_TaskQueue_FreeTask(p_task);
		}
	}else{
		if (p_task->task_type != CTRL_TASK_TYPE_HOST) {
			Ctrl_TaskQueue_FreeTask(p_task);
		} 
	}
}

void Ctrl_Destroy(Ctrl *p_ctrl) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_DESTROYCNTRL, NULL);

	#ifdef _CTRL_QUEUE_
	#pragma omp barrier
	omp_unset_lock(p_ctrl->p_lock_host);
	#endif //_CTRL_QUEUE_

	#pragma omp barrier

	#ifdef _CTRL_QUEUE_
	// Ctrl_TaskQueue_Destroy(p_ctrl->p_task_queue);

	omp_destroy_lock(p_ctrl->p_lock_first_host);
	omp_destroy_lock(p_ctrl->p_lock_first_ctrl);
	omp_destroy_lock(p_ctrl->p_lock_host);
	omp_destroy_lock(p_ctrl->p_lock_ctrl);
	free(p_ctrl->p_task_queue);
	free(p_ctrl->p_lock_first_host);
	free(p_ctrl->p_lock_first_ctrl);
	free(p_ctrl->p_lock_host);
	free(p_ctrl->p_lock_ctrl);
	#endif //_CTRL_QUEUE_

	free(p_ctrl->p_impl);
}

void Ctrl_AddTask(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile) {
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = type;

	task.p_tile = p_tile;

	#ifdef _CTRL_QUEUE_
	Ctrl_TaskQueue_Push(p_ctrl->p_task_queue, task);
	#else
	Ctrl_ExecTask(p_ctrl, &task);
	#endif //_CTRL_QUEUE_
}

void Ctrl_AddTaskFlagged(Ctrl *p_ctrl, Ctrl_TaskType type, HitTile *p_tile, int flags) {
	Ctrl_Task task = CTRL_TASK_NULL;
	task.task_type = type;

	task.p_tile = p_tile;
	task.flags = flags;

	#ifdef _CTRL_QUEUE_
	Ctrl_TaskQueue_Push(p_ctrl->p_task_queue, task);
	#else
	Ctrl_ExecTask(p_ctrl, &task);
	#endif //_CTRL_QUEUE_
}

void Ctrl_Sync(Ctrl *p_ctrl) {
	#ifdef _CTRL_QUEUE_
	omp_set_lock(p_ctrl->p_lock_first_host);
	omp_set_lock(p_ctrl->p_lock_ctrl);
	omp_unset_lock(p_ctrl->p_lock_first_host);

	omp_unset_lock(p_ctrl->p_lock_ctrl);
	omp_unset_lock(p_ctrl->p_lock_host);

	omp_set_lock(p_ctrl->p_lock_first_ctrl);
	omp_set_lock(p_ctrl->p_lock_host);
	omp_unset_lock(p_ctrl->p_lock_first_ctrl);
	#endif //_CTRL_QUEUE_
}

void Ctrl_SyncIfSync(Ctrl *p_ctrl) {
	#ifdef _CTRL_QUEUE_
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		Ctrl_Sync(p_ctrl);
	}
	#endif //_CTRL_QUEUE_
}

void Ctrl_LaunchKernel(Ctrl *p_ctrl, Ctrl_Task task) {
	#ifdef _CTRL_QUEUE_
	Ctrl_TaskQueue_Push(p_ctrl->p_task_queue, task);
	#else
	Ctrl_ExecTask(p_ctrl, &task);
	#endif //_CTRL_QUEUE_
}

void Ctrl_LaunchHostTask(Ctrl *p_ctrl, Ctrl_Task task) {
	#ifdef _CTRL_QUEUE_
	Ctrl_TaskQueue_Push(p_ctrl->p_task_queue, task);
	
	Ctrl_SyncIfSync(p_ctrl);
	#else
	Ctrl_ExecTask(p_ctrl, &task);
	#endif //_CTRL_QUEUE_
}

void Ctrl_GlobalSync(Ctrl *p_ctrl) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_GLOBALSYNC, NULL);

	Ctrl_Sync(p_ctrl);
}

void Ctrl_AllocInner(Ctrl *p_ctrl, HitTile *p_tile, int flags) {
	/* UPDATES FOR SHADOW COPIES, OR ALLOCATION OF SELECTIONS OF NO-MEMORY VARIABLES */
	// ADJUST ORIG_ACUM_CARD TO CARDINALITIES TO TRANSFORM ON MEMORY_OWNER, AS IN HITMAP
	if ( (p_tile->memStatus == HIT_MS_NOT_OWNER) || (p_tile->memStatus == HIT_MS_NOMEM) ) {
			/* 3.1. NEW STRIDES TO ACCESS ARE ALWAYS 1 */
			for (int i = 0; i < hit_shapeDims(p_tile->shape); i++) {
					p_tile->qstride[i] = 1;
			}

			/* 3.2. UPDATE ORIGINAL ACUMULATED CARDINALITIES, NOW IT HAS ITS OWN MEMORY */
			//Code below extracted from the body of hit_tileUpdateAcumCards(p_tile);
			p_tile->origAcumCard[hit_shapeDims(p_tile->shape)] = 1;
			int cardinality = 1;

			for (int i = (hit_shapeDims(p_tile->shape) - 1); i >= 0; i--) {
					cardinality = cardinality * p_tile->card[i];
					p_tile->origAcumCard[i] = cardinality;
			}
			p_tile->acumCard = p_tile->origAcumCard[0];
	}
	p_tile->memStatus = HIT_MS_OWNER;

	// RE-ADJUST TO PITCHED CARDINALITIES
#ifdef ALIGNED_SIZE
	// TODO: Check if this works. This code was written by Arturo.
	//  Manu 04/2021
	int dims = hit_tileDims( *p_tile);
	if ( dims > 1 ) {
		size_t toPitch = p_tile->card[dims - 1] * p_tile->baseExtent;

		toPitch = ( ( toPitch + ALIGNED_SIZE - 1) / ALIGNED_SIZE ) * ALIGNED_SIZE / p_tile->baseExtent;
		p_tile->origAcumCard[dims - 1] = toPitch;
		for (int i = dims - 2; i >= 0; i--) {
			toPtich *= p_tile->card[1];
			p_tile->origAcumCard[i] = toPitch;
		}
	}
#endif

	Ctrl_AddTaskFlagged(p_ctrl, CTRL_TASK_TYPE_ALLOCTILE, p_tile, flags);

	Ctrl_Sync(p_ctrl);
}

void Ctrl_SelectInner(Ctrl *p_ctrl, HitTile *p_tile, int flags) {
	Ctrl_AddTaskFlagged(p_ctrl, CTRL_TASK_TYPE_SELECTTILE, p_tile, flags);

	Ctrl_Sync(p_ctrl);
}

void Ctrl_DomainInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_DOMAINTILE, p_tile);

	Ctrl_Sync(p_ctrl);
}

void Ctrl_FreeInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_FREETILE, p_tile);

	Ctrl_Sync(p_ctrl);
}

void Ctrl_MoveToInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_MOVETO, p_tile);

	Ctrl_SyncIfSync(p_ctrl);
}

void Ctrl_MoveFromInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_MOVEFROM, p_tile);

	Ctrl_SyncIfSync(p_ctrl);
}

void Ctrl_WaitTileInner(Ctrl *p_ctrl, HitTile *p_tile) {
	Ctrl_AddTask(p_ctrl, CTRL_TASK_TYPE_WAITTILE, p_tile);

	Ctrl_Sync(p_ctrl);
}

void Ctrl_Hosttask_Sync() {
	Ctrl_TaskQueue_Synchronize(p_ctrl_host_stream);
}

void Ctrl_Synchronize() {
	Ctrl_GlobalSync(&Ctrl_global);
	Ctrl_Hosttask_Sync();
}

void Ctrl_SetHostAffinity(int node) {
	host_node = node;
}

void Ctrl_SetDependanceMode(Ctrl *p_ctrl, int mode){
	Ctrl_AddTaskFlagged(p_ctrl, CTRL_TASK_TYPE_SETDEPENDANCEMODE, NULL, mode);
	Ctrl_SyncIfSync(p_ctrl);
}

int Ctrl_Dev(Ctrl_Type type, int* avail_impls, int n_impl) {
	int result = -1;
	for (int i = 0; i < n_impl; i++) {
		if ( avail_impls[i] > result ) {
			switch (type) {
				case CTRL_TYPE_CPU:
					if ( CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], CPU) ) {
						result = avail_impls[i];
					}
					#ifdef _CTRL_MKL_
						else if (avail_impls[i] == CPULIB_MKL) {
							result = avail_impls[i];
						}
					#endif // _CTRL_MKL_
					break;
				case CTRL_TYPE_CUDA:
					if ( CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], CUDA) ) {
						result = avail_impls[i];
					}
					#ifdef _CTRL_CUBLAS_
						else if (avail_impls[i] == CUDALIB_CUBLAS) {
							result = avail_impls[i];
						}
					#endif // _CTRL_CUBLAS_ 
					#ifdef _CTRL_MAGMA_
						else if (avail_impls[i] == CUDALIB_MAGMA) {
							result = avail_impls[i];
						}
					#endif // _CTRL_MAGMA_ 
					break;
				case CTRL_TYPE_OPENCL_GPU:
					if ( CTRL_IMPL_IN_RANGE(avail_impls[i], GENERIC) || CTRL_IMPL_IN_RANGE(avail_impls[i], OPENCLGPU) ) {
						result = avail_impls[i];
					}
					break;
				case CTRL_TYPE_FPGA:
					if ( CTRL_IMPL_IN_RANGE(avail_impls[i], FPGA) ) {
						result = avail_impls[i];
					}
					break;
			}
		}
	}
	return result;
}

void Ctrl_EvalTask(Ctrl_Task *p_task) {
	// Evaluation of task
	switch ( p_task->task_type ) {
		case CTRL_TASK_TYPE_HOST: 
			p_task->pfn_hostTask_wrapper(p_task->p_arguments);
			break;
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
			printf("Core EvalTask: task type not implemented on this stream\n");
			exit(EXIT_FAILURE);
			break;
	}
}

/* returns the number of threads p_ctrl will need */
int Ctrl_GetNumThreads(Ctrl *p_ctrl) {
	#ifdef _CTRL_QUEUE_
		int default_num_threads= 1;
	#else
		int default_num_threads= 0;
	#endif //_CTR_QUEUE_
	switch (p_ctrl->type){
	#ifdef _CTRL_ARCH_CPU_
		case CTRL_TYPE_CPU:
			return default_num_threads + Ctrl_Cpu_GetNumThreads(&(p_ctrl->p_impl->cpu));
			break;
	#endif // _CTRL_ARCH_CPU_
	
	default:
		return default_num_threads;
	}
}

///@endcond
