///@cond INTERNAL
/**
 * @file Ctrl_FPGA.c
 * @author Gabriel Rodriguez-Canal
 * @brief Source code for FPGA backend.
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

#include "Architectures/FPGA/Ctrl_FPGA.h"

Ctrl_FPGA_KernelParams FPGA_initial_kp = CTRL_FPGA_KERNELPARAMS_NULL; 

/*************************************************************
 ******** Prototypes of private functions ********************
 *************************************************************/

/**
 * Sync with main thread if queues are enabled, if queues are not enabled this function does nothing.
 * 
 * @param p_ctrl Pointer to the ctrl to perform the syncronization.
 */
void Ctrl_FPGA_Sync(Ctrl_FPGA *p_ctrl);

/**
 * Allocate memory for a new \e Ctrl_FPGA_Tile.
 * 
 * @param p_ctrl Pointer to the ctrl to be attached to the tile.
 * @param p_task Pointer to the task containing the hitTile that will contain the new \e Ctrl_FPGA_Tile.
 * 
 * @see Ctrl_FPGA_EvalTaskAllocTile, Ctrl_FPGA_Tile
 */
void Ctrl_FPGA_CreateTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Initializate a \e Ctrl_FPGA_Tile.
 *
 * @param p_ctrl Pointer to the ctrl attached to the tile and to be updated of the initialization.
 * @param p_task Pointer to the task containing the HitTile that contains the \e Ctrl_FPGA_Tile to be.
 * initialized.
 */
void Ctrl_FPGA_InitTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Perform memory transfer from host to device.
 * 
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 * 
 * @see Ctrl_FPGA_EvalTaskMoveTo
 */
void Ctrl_FPGA_EvalTaskMoveToInner(Ctrl_FPGA *p_ctrl, HitTile *p_tile);

/**
 * Perform memory transfer from device to host.
 * 
 * @param p_ctrl Pointer to the ctrl attached to the tile to be moved.
 * @param p_tile Pointer to the task containing the tile to be moved.
 * 
 * @see Ctrl_FPGA_EvalTaskMoveFrom
 */
void Ctrl_FPGA_EvalTaskMoveFromInner(Ctrl_FPGA *p_ctrl, HitTile *p_tile);

/*************************************************************
 ******** Prototypes of tasks' evaluation functions **********
 *************************************************************/

/**
 * Destroy a FPGA ctrl.
 * @param p_ctrl Ctrl to be destroyed.
 */
void Ctrl_FPGA_Destroy(Ctrl_FPGA *p_ctrl);

/**
 * Evaluation of kernel launch.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_Launch
 */
void Ctrl_FPGA_EvalTaskKernelLaunch(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of host task launch.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_HostTask
 */
void Ctrl_FPGA_EvalTaskHostTaskLaunch(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of global sync. Waits for all work related to any tile attached to this ctrl.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_GlobalSync
 */
void Ctrl_FPGA_EvalTaskGlobalSync(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of creation of tiles.
 *
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 *
 * @see Ctrl_FPGA_EvalTask, Ctrl_Domain
 */ 
void Ctrl_FPGA_EvalTaskDomainTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of allocation of tiles.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_Alloc
 */
void Ctrl_FPGA_EvalTaskAllocTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of freeing of tiles.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_Free
 */
void Ctrl_FPGA_EvalTaskFreeTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 * 
 * This function checks state of tile and, if comunication is necesary calls to \e Ctrl_FPGA_EvalTaskMoveToInner.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_FPGA_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_FPGA_EvalTaskMoveTo(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of comunication of tile from host to device.
 * 
 * This function checks state of tile and, if comunication is necesary calls to \e Ctrl_FPGA_EvalTaskMoveToInner.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_FPGA_EvalTaskMoveToInner, Ctrl_MoveTo
 */
void Ctrl_FPGA_EvalTaskMoveFrom(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of wait. Waits for all the work related to the tile specified in \p p_task .
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 * 
 * @see Ctrl_FPGA_EvalTask, Ctrl_WaitTile
 */
void Ctrl_FPGA_EvalTaskWaitTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/**
 * Evaluation of change of dependance mode.
 * 
 * @param p_ctrl Ctrl in charge of task.
 * @param p_task Task to be evaluated.
 */
void Ctrl_FPGA_EvalTaskSetDependanceMode(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task);

/********************************************
 ******** FPGA Controller functions *********
 ********************************************/

void Ctrl_FPGA_Create(Ctrl_FPGA *p_ctrl, Ctrl_Policy policy, int device, int platform, int exec_mode) {
	cl_int err;

	p_ctrl->policy = policy;
	p_ctrl->dependance_mode = CTRL_MODE_IMPLICIT;

	// get OpenCL platform id from platform index
	cl_platform_id *p_platform_ids = (cl_platform_id *)malloc((platform + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP( clGetPlatformIDs(platform + 1, p_platform_ids, NULL) );
	p_ctrl->platform_id = p_platform_ids[platform];
	free(p_platform_ids);

	// get OpenCL device id from device index
	cl_device_id *p_device_ids = (cl_device_id *)malloc((device + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP( clGetDeviceIDs(p_ctrl->platform_id, CL_DEVICE_TYPE_ACCELERATOR, device + 1, p_device_ids, NULL) );
	p_ctrl->device_id = p_device_ids[device];
	free(p_device_ids);

	// Create OpenCL context
	cl_context_properties context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)p_ctrl->platform_id, 0};
	p_ctrl->context = clCreateContext(context_properties, 1, &(p_ctrl->device_id), NULL, NULL, &err);
	OPENCL_ASSERT_ERROR( err );
	
	p_ctrl->queue_properties = 0;
	#ifdef _CTRL_OPENCL_GPU_PROFILING_
		p_ctrl->queue_properties |= CL_QUEUE_PROFILING_ENABLE;
	#endif

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	// Create OpenCL queue for kernel execution
	p_ctrl->queue = clCreateCommandQueue(p_ctrl->context, p_ctrl->device_id, p_ctrl->queue_properties, &err);
	OPENCL_ASSERT_ERROR( err );

	// Create default event and set it as completed as there is nothing to wait for in the begining
	p_ctrl->default_event = clCreateUserEvent(p_ctrl->context, &err);
	OPENCL_ASSERT_ERROR( err );
	OPENCL_ASSERT_OP( clSetUserEventStatus(p_ctrl->default_event, CL_COMPLETE) );

	// Create kernel and host task events from default event
	p_ctrl->last_kernel_event = p_ctrl->default_event;
	OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->last_kernel_event) );
	p_ctrl->last_host_task_event = p_ctrl->default_event;
	OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->last_host_task_event) );
	
	for(Ctrl_FPGA_KernelParams *curr_k_par = &FPGA_initial_kp; curr_k_par->p_next != NULL; curr_k_par = curr_k_par->p_next) {
		FILE *binary_file;
		static char* kernel_path;
		kernel_path = (char*)malloc(CTRL_KERNEL_PATH_LENGTH*sizeof(char));
		strcpy(kernel_path, curr_k_par->p_binary_name);
		switch(exec_mode) {
			case FPGA_EMULATION:
				strcat(kernel_path, "_emu");
				break;
			case FPGA_PROFILING:
				strcat(kernel_path, "_profiling");
				break;
			default:
				break;
		}
		strcat(kernel_path, "_Ctrl.aocx");

		if(!(binary_file = fopen(kernel_path, "rb"))) {
			printf("Kernel file not found.\n");
			exit(ERR_NOT_FOUND);
		}
		fseek(binary_file, 0, SEEK_END);
		size_t binary_length = ftell(binary_file);
		unsigned char *binary_str = (unsigned char*)malloc(binary_length * sizeof(unsigned char));
		rewind(binary_file);
		if(!(fread(binary_str, binary_length, 1, binary_file))) {
			printf("Error reading kernel file\n");
			exit(ERR_READ);
		}

		curr_k_par->p_binary_str = binary_str;
		curr_k_par->binary_length = binary_length;

		*curr_k_par->p_program = clCreateProgramWithBinary(p_ctrl->context, 1,
			&p_ctrl->device_id, (const size_t *)&curr_k_par->binary_length, (const unsigned char**)&curr_k_par->p_binary_str, NULL, &err);
		OPENCL_ASSERT_ERROR( err );
		
		err = clBuildProgram(*curr_k_par->p_program, 1, &p_ctrl->device_id, NULL, NULL, NULL);
		if (err == CL_BUILD_PROGRAM_FAILURE) {
			size_t log_size;
			clGetProgramBuildInfo(*curr_k_par->p_program, p_ctrl->device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
			char *log = (char *) malloc(log_size);
			clGetProgramBuildInfo(*curr_k_par->p_program, p_ctrl->device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
			printf("%s\n", log); fflush(stdout);
			free(log);
		}
		OPENCL_ASSERT_ERROR( err );
		
		*curr_k_par->p_kernel = clCreateKernel(*curr_k_par->p_program, (const char *)curr_k_par->p_kernel_name, &err);
		OPENCL_ASSERT_ERROR( err );
	}

	#ifdef _CTRL_OPENCL_GPU_PROFILING_
		p_ctrl->platform = platform;
		p_ctrl->device = device;
		
		p_ctrl->profiling_info_start = CL_PROFILING_COMMAND_START;
		p_ctrl->profiling_info_end = CL_PROFILING_COMMAND_END;

		p_ctrl->profiling_read_events = (cl_event *)malloc(_OPENCL_GPU_PROFILING_N_READ_TASKS_ * sizeof(cl_event));
		p_ctrl->profiling_write_events = (cl_event *)malloc(_OPENCL_GPU_PROFILING_N_WRITE_TASKS_ * sizeof(cl_event));
		p_ctrl->profiling_kernel_events = (cl_event *)malloc(_OPENCL_GPU_PROFILING_N_KERNEL_TASKS_ * sizeof(cl_event));

		p_ctrl->i_read_task = 0;
		p_ctrl->i_write_task = 0;
		p_ctrl->i_kernel_task = 0;

		p_ctrl->profiling_total = 0;
		p_ctrl->profiling_sum = 0;
		p_ctrl->profiling_offloading = 0;
		p_ctrl->profiling_read = 0;
		p_ctrl->profiling_write = 0;
		p_ctrl->profiling_kernel = 0;

		p_ctrl->profiling_start = 0;
		p_ctrl->profiling_end = 0;

		p_ctrl->has_first_profiling_event = false;

		p_ctrl->last_profiling_event = p_ctrl->default_event;
		OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->last_profiling_event) );
		
		#ifdef _CTRL_OPENCL_GPU_PROFILING_VERBOSE_
			p_ctrl->profiling_visual_events = (visual_event *)malloc(
					(_OPENCL_GPU_PROFILING_N_READ_TASKS_ + 
					_OPENCL_GPU_PROFILING_N_WRITE_TASKS_ + 
					_OPENCL_GPU_PROFILING_N_KERNEL_TASKS_) * 
					sizeof(visual_event)
			);
			p_ctrl->i_visual_task = 0;
		#endif
	#endif // _CTRL_OPENCL_GPU_PROFILING_
}

void Ctrl_FPGA_EvalTask(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	switch ( p_task->task_type ) {
		case CTRL_TASK_TYPE_KERNEL:
			Ctrl_FPGA_EvalTaskKernelLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_HOST:
			Ctrl_FPGA_EvalTaskHostTaskLaunch(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_GLOBALSYNC:
			Ctrl_FPGA_EvalTaskGlobalSync(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_ALLOCTILE:
			Ctrl_FPGA_EvalTaskAllocTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DOMAINTILE:
			Ctrl_FPGA_EvalTaskDomainTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_FREETILE:
			Ctrl_FPGA_EvalTaskFreeTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVETO:
			Ctrl_FPGA_EvalTaskMoveTo(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_MOVEFROM:
			Ctrl_FPGA_EvalTaskMoveFrom(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_WAITTILE:
			Ctrl_FPGA_EvalTaskWaitTile(p_ctrl, p_task);
			break;
		case CTRL_TASK_TYPE_DESTROYCNTRL:
			Ctrl_FPGA_Destroy(p_ctrl);
			break;
		case CTRL_TASK_TYPE_SETDEPENDANCEMODE:
			Ctrl_FPGA_EvalTaskSetDependanceMode(p_ctrl, p_task);
			break;
		default:
			fprintf(stderr, "[Ctrl_FPGA] Unsupported task type:%d.\n", p_task->task_type);
			exit(EXIT_FAILURE);
	}
}

/*********************************
 ******* Private functions *******
 *********************************/

void Ctrl_FPGA_Sync(Ctrl_FPGA *p_ctrl) {	
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

void Ctrl_FPGA_CreateTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)malloc(sizeof(Ctrl_FPGA_Tile));

	p_tile_data->p_ctrl = p_ctrl;

	p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST_DEV;

	p_tile->ext = (void *)p_tile_data;
	
	p_tile_data->is_initialized = false;
}

void Ctrl_FPGA_InitTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task){
	cl_int err;

	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);	

	//create node for the the new tile
	Ctrl_FPGA_Tile_List *p_list_node = (Ctrl_FPGA_Tile_List *)malloc(sizeof(Ctrl_FPGA_Tile_List));
	p_list_node->p_prev = NULL;
	p_list_node->p_next = NULL;
	p_list_node->p_tile_ext = p_tile_data;
	p_tile_data->p_tile_elem = p_list_node;

	//insert node into the linked list of tiles
	if (p_ctrl->p_tile_list_tail != NULL){
		p_ctrl->p_tile_list_tail->p_next = p_list_node;
		p_list_node->p_prev = p_ctrl->p_tile_list_tail;
		p_ctrl->p_tile_list_tail = p_list_node;
	} else {
		p_ctrl->p_tile_list_head = p_ctrl->p_tile_list_tail = p_list_node;
	}

	// Create events for this tile
	p_tile_data->kernel_last_read_event = p_ctrl->default_event;
	p_tile_data->kernel_last_write_event = p_ctrl->default_event;

	p_tile_data->offloading_last_read_event = p_ctrl->default_event;
	p_tile_data->offloading_last_write_event = p_ctrl->default_event;

	p_tile_data->host_last_read_event = p_ctrl->default_event;
	p_tile_data->host_last_write_event = p_ctrl->default_event;
	
	OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->kernel_last_read_event) );
	OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->kernel_last_write_event) );

	OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->offloading_last_read_event) );
	OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->offloading_last_write_event) );

	OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->host_last_read_event) );
	OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->host_last_write_event) );

	// Create queue for mem transfers for this tile
	p_tile_data->queue = clCreateCommandQueue(p_ctrl->context, p_ctrl->device_id, p_ctrl->queue_properties, &err);
	OPENCL_ASSERT_ERROR( err );

	p_tile_data->is_initialized = true;
}

void Ctrl_FPGA_EvalTaskMoveToInner(Ctrl_FPGA *p_ctrl, HitTile *p_tile) {
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);
	
	cl_event aux = p_tile_data->offloading_last_write_event;

	// populate wait list with appropiate events 
	cl_event wait_list[4];
	wait_list[0] = p_tile_data->kernel_last_read_event;
	wait_list[1] = p_tile_data->kernel_last_write_event;
	wait_list[2] = p_tile_data->offloading_last_read_event;
	wait_list[3] = p_tile_data->host_last_write_event;

	// Enqueue the transfer operation
	OPENCL_ASSERT_OP( 
		clEnqueueWriteBuffer(
			p_tile_data->queue, 
			p_tile_data->device_data, 
			CL_FALSE, 
			0, 
			((size_t)(p_tile->acumCard)) * (p_tile->baseExtent), 
			p_tile->data,
			4,
			wait_list, 
			&(p_tile_data->offloading_last_write_event) 
		)
	);

	OPENCL_ASSERT_OP( clFlush(p_tile_data->queue) );
	OPENCL_ASSERT_OP( clReleaseEvent(aux) );

	OPENCL_PROFILE_VISUAL( 
			CTRL_OPENCL_PROFILE_QUEUE_WRITE, 
			p_tile_data->offloading_last_write_event, 
			CTRL_OPENCL_PROFILE_OP_WRITE 
	);
	OPENCL_PROFILE_WRITE( p_tile_data->offloading_last_write_event );

	// Update the state of the tile
	p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST_DEV;
}

void Ctrl_FPGA_EvalTaskMoveFromInner(Ctrl_FPGA *p_ctrl, HitTile *p_tile) {
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

	cl_event aux = p_tile_data->offloading_last_read_event;

	// populate wait list with appropiate events 
	cl_event wait_list[4];
	wait_list[0] = p_tile_data->kernel_last_write_event;
	wait_list[1] = p_tile_data->offloading_last_write_event;
	wait_list[2] = p_tile_data->host_last_read_event;
	wait_list[3] = p_tile_data->host_last_write_event;

	// Enqueue the transfer operation
	OPENCL_ASSERT_OP( 
		clEnqueueReadBuffer(
			p_tile_data->queue, 
			p_tile_data->device_data, 
			CL_FALSE, 
			0, 
			((size_t)(p_tile->acumCard)) * (p_tile->baseExtent), 
			p_tile->data,
			4,
			wait_list,
			&(p_tile_data->offloading_last_read_event) 
		)
	);

	OPENCL_ASSERT_OP( clFlush(p_tile_data->queue) );
	OPENCL_ASSERT_OP( clReleaseEvent(aux) );

	OPENCL_PROFILE_VISUAL( 
			CTRL_OPENCL_PROFILE_QUEUE_READ, 
			p_tile_data->offloading_last_read_event, 
			CTRL_OPENCL_PROFILE_OP_READ 
	);
	OPENCL_PROFILE_READ( p_tile_data->offloading_last_read_event );

	// Update the state of the tile
	p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST_DEV;
}

/**********************************
 ** TASKS'S EVALUATION FUNCTIONS **
 **********************************/

void Ctrl_FPGA_Destroy(Ctrl_FPGA *p_ctrl) {
	#ifdef _CTRL_OPENCL_GPU_PROFILING_
		#ifdef _OPENCL_GPU_TEST_OUTPUT_
			printf("%d %d\n", p_ctrl->platform, p_ctrl->device);
			printf("%d %d %d %d %d\n", p_ctrl->i_read_task + p_ctrl->i_write_task + p_ctrl->i_kernel_task,
					p_ctrl->i_kernel_task, p_ctrl->i_read_task + p_ctrl->i_write_task, p_ctrl->i_read_task, p_ctrl->i_write_task);
		#else
			printf("\n ----------------------- ARGS ----------------------- \n");
			printf("\n PLATFORM: %d", p_ctrl->platform);
			printf("\n DEVICE: %d", p_ctrl->device);
			printf("\n TOTAL TASKS: %d", p_ctrl->i_read_task + p_ctrl->i_write_task + p_ctrl->i_kernel_task);
			printf("\n KERNEL TASKS: %d", p_ctrl->i_kernel_task);
			printf("\n OFFLOADING TASKS: %d", p_ctrl->i_read_task + p_ctrl->i_write_task);
			printf("\n READ TASKS: %d", p_ctrl->i_read_task);
			printf("\n WRITE TASKS: %d", p_ctrl->i_write_task);
			if (p_ctrl->policy == CTRL_POLICY_ASYNC) {
				printf("\n POLICY ASYNC");
			} else {
				printf("\n POLICY SYNC");
			}
			printf("\n\n ---------------------------------------------------- \n");
		#endif // _OPENCL_GPU_TEST_OUTPUT_

		#ifdef _CTRL_OPENCL_GPU_PROFILING_VERBOSE_
			FILE *f = fopen("visual_profiler_info.txt", "w");

			for (int i = 0; i < p_ctrl->i_visual_task; i++) {
				OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_visual_events[i].event, p_ctrl->profiling_info_start, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_start)), NULL) );
				OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_visual_events[i].event, p_ctrl->profiling_info_end, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_end)), NULL) );

				fprintf(f, "%d; %d; %lu; %lu\n", p_ctrl->profiling_visual_events[i].queue, p_ctrl->profiling_visual_events[i].op, p_ctrl->profiling_start, p_ctrl->profiling_end);
				OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->profiling_visual_events[i].event) );
			}

			fclose(f);
			free(p_ctrl->profiling_visual_events);
		#endif

		for (int i = 0; i < p_ctrl->i_read_task; i++) {
			OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_read_events[i], p_ctrl->profiling_info_start, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_start)), NULL) );
			OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_read_events[i], p_ctrl->profiling_info_end, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_end)), NULL) );

			p_ctrl->profiling_sum += p_ctrl->profiling_end - p_ctrl->profiling_start;
			p_ctrl->profiling_read += p_ctrl->profiling_end - p_ctrl->profiling_start;
			p_ctrl->profiling_offloading += p_ctrl->profiling_end - p_ctrl->profiling_start;
			
			OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->profiling_read_events[i]) );
		}
		free(p_ctrl->profiling_read_events);

		for (int i = 0; i < p_ctrl->i_write_task; i++) {
			OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_write_events[i], p_ctrl->profiling_info_start, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_start)), NULL) );
			OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_write_events[i], p_ctrl->profiling_info_end, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_end)), NULL) );

			p_ctrl->profiling_sum += p_ctrl->profiling_end - p_ctrl->profiling_start;
			p_ctrl->profiling_write += p_ctrl->profiling_end - p_ctrl->profiling_start;
			p_ctrl->profiling_offloading += p_ctrl->profiling_end - p_ctrl->profiling_start;
			
			OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->profiling_write_events[i]) );
		}
		free(p_ctrl->profiling_write_events);

		for (int i = 0; i < p_ctrl->i_kernel_task; i++) {
			OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_kernel_events[i], p_ctrl->profiling_info_start, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_start)), NULL) );
			OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->profiling_kernel_events[i], p_ctrl->profiling_info_end, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_end)), NULL) );
			
			p_ctrl->profiling_sum += p_ctrl->profiling_end - p_ctrl->profiling_start;
			p_ctrl->profiling_kernel += p_ctrl->profiling_end - p_ctrl->profiling_start;

			OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->profiling_kernel_events[i]) );
		}
		free(p_ctrl->profiling_kernel_events);


		OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->first_profiling_event, p_ctrl->profiling_info_start, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_start)), NULL) );
		OPENCL_ASSERT_OP( clGetEventProfilingInfo(p_ctrl->last_profiling_event, p_ctrl->profiling_info_end, sizeof(cl_ulong), (void *)(&(p_ctrl->profiling_end)), NULL) );

		p_ctrl->profiling_total = p_ctrl->profiling_end - p_ctrl->profiling_start;

		OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->first_profiling_event) );
		OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->last_profiling_event) );

		#ifdef _OPENCL_GPU_TEST_OUTPUT_
			printf("%lf %lf %lf %lf %lf\n", p_ctrl->profiling_total / 1000000.0, p_ctrl->profiling_sum / 1000000.0, p_ctrl->profiling_kernel / 1000000.0, p_ctrl->profiling_offloading / 1000000.0, p_ctrl->profiling_read / 1000000.0, p_ctrl->profiling_write / 1000000.0);
		#else
			printf("\n -------------------- PROFILING --------------------- \n");
			printf("\n Total Time: %.3lf", p_ctrl->profiling_total / 1000000.0);
			printf("\n Acumulate Sum Time: %.3lf", p_ctrl->profiling_sum / 1000000.0);
			printf("\n Kernel Time: %.3lf", p_ctrl->profiling_kernel / 1000000.0);
			printf("\n Offloading Time: %.3lf", p_ctrl->profiling_offloading / 1000000.0);
			printf("\n Read Time: %.3lf", p_ctrl->profiling_read / 1000000.0);
			printf("\n Write Time: %.3lf", p_ctrl->profiling_write / 1000000.0);
			printf("\n\n ---------------------------------------------------- \n");
		#endif // _OPENCL_GPU_TEST_OUTPUT_
	#endif // _CTRL_OPENCL_GPU_PROFILING_

	// Destroy events
	OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->last_kernel_event) );
	OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->last_host_task_event) );
	OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->default_event) );

	// Destroy and free kernel stuff
	Ctrl_FPGA_KernelParams *p_aux = NULL;
	Ctrl_FPGA_KernelParams *p_res = &FPGA_initial_kp;
	while(p_res->p_next != NULL) {
		p_aux = p_res;
		p_res = p_res->p_next;
		OPENCL_ASSERT_OP( clReleaseKernel(*p_aux->p_kernel) );
		OPENCL_ASSERT_OP( clReleaseProgram(*p_aux->p_program) );
		free(p_aux->p_kernel);
		free(p_aux->p_program);
		free(p_aux->p_binary_name);
		free(p_aux->p_binary_str);
	}

	p_ctrl->p_tile_list_head = NULL;
	p_ctrl->p_tile_list_tail = NULL;

	OPENCL_ASSERT_OP( clReleaseCommandQueue(p_ctrl->queue) );
	OPENCL_ASSERT_OP( clReleaseContext(p_ctrl->context) );

	// Send destroy task to host task stream
	Ctrl_Task task=CTRL_TASK_NULL;
	task.task_type=CTRL_TASK_TYPE_DESTROYCNTRL;
	Ctrl_TaskQueue_Push(p_ctrl_host_stream,task);
}

void Ctrl_FPGA_EvalTaskGlobalSync(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	// Wait for kernel queue to empty
	OPENCL_ASSERT_OP( clFinish(p_ctrl->queue) );

	// Wait for all tile's queues to empty
	for (Ctrl_FPGA_Tile_List *p_aux = p_ctrl->p_tile_list_head; p_aux != NULL; p_aux = p_aux->p_next) {
		OPENCL_ASSERT_OP( clFinish(p_aux->p_tile_ext->queue) );
	}

	// Wait for last host task to finish 
	OPENCL_ASSERT_OP( clWaitForEvents(1, &(p_ctrl->last_host_task_event)) );

	Ctrl_FPGA_Sync(p_ctrl);
}

void Ctrl_FPGA_EvalTaskKernelLaunch(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	int n_event_wait = 0;
	cl_event p_event_wait_list[60];

	//wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			KHitTile *p_ktile = (KHitTile *)((uint8_t *)p_task->p_arguments + p_task->p_displacements[i]);
			Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

			if (hit_tileIsNull(*p_tile)) {
				fprintf(stderr, "Warning: Launching task, skipping null tile on parameter %d\n", i); fflush(stderr);
				continue;
			}

			//if tile's role is IN or IO and is not updated on device transfer it
			if (p_task->p_roles[i] != KERNEL_OUT && p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
				if (p_tile_data->last_update == CTRL_TILE_LAST_UPDATE_HOST) {
					Ctrl_FPGA_EvalTaskMoveToInner(p_ctrl, p_tile);
				}
			}

			p_ktile->data = &(p_tile_data->device_data);

			p_event_wait_list[n_event_wait] = p_tile_data->kernel_last_write_event;
			n_event_wait++;

			p_event_wait_list[n_event_wait] = p_tile_data->offloading_last_write_event;
			n_event_wait++;

			if (p_task->p_roles[i] != KERNEL_IN) {
				p_event_wait_list[n_event_wait] = p_tile_data->kernel_last_read_event;
				n_event_wait++;
				p_event_wait_list[n_event_wait] = p_tile_data->offloading_last_read_event;
				n_event_wait++;
			}
		}
	}

	cl_event event_aux = p_ctrl->last_kernel_event;

	//create request with info for kernel execution
	Ctrl_Request request;
	request.fpga.context = &(p_ctrl->context);
	request.fpga.queue = &(p_ctrl->queue);
	request.fpga.device_id = &(p_ctrl->device_id);
	request.fpga.p_last_kernel_event = &(p_ctrl->last_kernel_event);
	request.fpga.p_event_wait_list = p_event_wait_list;
	request.fpga.n_event_wait = n_event_wait;
	request.fpga.n_arguments = p_task->n_arguments;
	request.fpga.p_roles = p_task->p_roles;
	request.fpga.p_displacements = p_task->p_displacements;
	
	//Launch kernel to FPGA kernel queue
	p_task->pfn_kernel_wrapper(request, p_task->device_id, p_task->threads, p_task->blocksize, p_task->p_arguments);

	OPENCL_ASSERT_OP( clReleaseEvent(event_aux) );

	OPENCL_PROFILE_VISUAL( CTRL_OPENCL_PROFILE_QUEUE_MAIN, p_ctrl->last_kernel_event, CTRL_OPENCL_PROFILE_OP_KERNEL );
	OPENCL_PROFILE_KERNEL( p_ctrl->last_kernel_event );

	// Release appropiate events on arguments accoeding to their roles and substitute them with this kernel's event
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

			if (p_task->p_roles[i] != KERNEL_IN) {
				p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_DEV;

				OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->kernel_last_write_event) );
				p_tile_data->kernel_last_write_event = p_ctrl->last_kernel_event;
				OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->kernel_last_write_event) );
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->kernel_last_read_event) );
				p_tile_data->kernel_last_read_event = p_ctrl->last_kernel_event;
				OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->kernel_last_read_event) );
			}
		}
	}
}

void Ctrl_FPGA_EvalTaskHostTaskLaunch(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	cl_int err;
	
	// Create new event for this host task
	cl_event cl_host_task_event = clCreateUserEvent(p_ctrl->context, &err);
	OPENCL_ASSERT_ERROR( err );

	// Exange old host task event with the new one
	OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->last_host_task_event) );
	p_ctrl->last_host_task_event = cl_host_task_event;
	OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->last_host_task_event) );
	
	// Create generic event to wait to tiles events on host task queue
	Ctrl_GenericEvent host_task_event = CTRL_GENERIC_EVENT_NULL;
	host_task_event.event_type=CTRL_EVENT_TYPE_OPENCL;
	
	// Wait for appropiate events from arguments according to their role
	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

			// If tile's role is IN or IO and is not updated on host transfer it
			if (p_task->p_roles[i] != KERNEL_OUT && p_ctrl->dependance_mode == CTRL_MODE_IMPLICIT) {
				if (p_tile_data->last_update == CTRL_TILE_LAST_UPDATE_DEV) {
					Ctrl_FPGA_EvalTaskMoveFromInner(p_ctrl, p_tile);
				}
			}

			host_task_event.event.event_cl = p_tile_data->offloading_last_read_event;
			OPENCL_ASSERT_OP( clRetainEvent(host_task_event.event.event_cl) );
			Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);

			host_task_event.event.event_cl = p_tile_data->host_last_write_event;
			OPENCL_ASSERT_OP( clRetainEvent(host_task_event.event.event_cl) );
			Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);

			if (p_task->p_roles[i] != KERNEL_IN) {
				p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST;
				host_task_event.event.event_cl = p_tile_data->offloading_last_write_event;
				OPENCL_ASSERT_OP( clRetainEvent(host_task_event.event.event_cl) );
				Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);

				host_task_event.event.event_cl = p_tile_data->host_last_read_event;
				OPENCL_ASSERT_OP( clRetainEvent(host_task_event.event.event_cl) );
				Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);
			}
		}
	}

	cl_event event_seq;
	if (p_ctrl->policy != CTRL_POLICY_ASYNC) {
		//OPENCL_ASSERT_OP( clEnqueueMarkerWithWaitList(p_ctrl->queue, 0, NULL, &event_seq) );
		OPENCL_ASSERT_OP( clEnqueueMarker(p_ctrl->queue, &event_seq) );
		host_task_event.event.event_cl=event_seq;
		OPENCL_ASSERT_OP( clRetainEvent(host_task_event.event.event_cl) );
		Ctrl_GenericEvent_StreamWait(host_task_event, p_ctrl_host_stream);
	}

	Ctrl_TaskQueue_Push(p_ctrl_host_stream, *p_task);
	host_task_event.event.event_cl=cl_host_task_event;
	Ctrl_GenericEvent_StreamSignal(host_task_event, p_ctrl_host_stream);

	for (int i = 0; i < p_task->n_arguments; i++) {
		if (p_task->p_roles[i] != KERNEL_INVAL) {
			HitTile *p_tile = (HitTile *)(p_task->pp_pointers[i]);
			Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

			host_task_event.event.event_cl = p_tile_data->offloading_last_read_event;
			Ctrl_GenericEvent_StreamRelease(host_task_event, p_ctrl_host_stream);

			host_task_event.event.event_cl = p_tile_data->host_last_write_event;
			Ctrl_GenericEvent_StreamRelease(host_task_event, p_ctrl_host_stream);

			if (p_task->p_roles[i] != KERNEL_IN) {
				p_tile_data->last_update = CTRL_TILE_LAST_UPDATE_HOST;
				host_task_event.event.event_cl = p_tile_data->offloading_last_write_event;
				Ctrl_GenericEvent_StreamRelease(host_task_event, p_ctrl_host_stream);

				host_task_event.event.event_cl = p_tile_data->host_last_read_event;
				Ctrl_GenericEvent_StreamRelease(host_task_event, p_ctrl_host_stream);

				host_task_event.event.event_cl = p_tile_data->host_last_write_event;
				Ctrl_GenericEvent_StreamRelease(host_task_event, p_ctrl_host_stream);
				p_tile_data->host_last_write_event = cl_host_task_event;
				OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->host_last_write_event) );
			}

			if (p_task->p_roles[i] != KERNEL_OUT) {
				host_task_event.event.event_cl = p_tile_data->host_last_read_event;
				Ctrl_GenericEvent_StreamRelease(host_task_event, p_ctrl_host_stream);
				p_tile_data->host_last_read_event = cl_host_task_event;
				OPENCL_ASSERT_OP( clRetainEvent(p_tile_data->host_last_read_event) );
			}
		}
	}

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		OPENCL_ASSERT_OP( clWaitForEvents(1, &cl_host_task_event) );
		OPENCL_ASSERT_OP( clReleaseEvent(event_seq) );
		Ctrl_FPGA_Sync(p_ctrl);
	}
}

void Ctrl_FPGA_EvalTaskDomainTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task){
	Ctrl_FPGA_CreateTile(p_ctrl, p_task);
	Ctrl_FPGA_Sync(p_ctrl);
}

void Ctrl_FPGA_EvalTaskAllocTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	cl_int err;

	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

	if ( ! (p_tile_data->is_initialized) )
		Ctrl_FPGA_InitTile(p_ctrl, p_task);

	p_tile_data->mem_flags = p_task->flags;
	
	p_tile_data->queue = clCreateCommandQueue(p_ctrl->context, p_ctrl->device_id, p_ctrl->queue_properties, &err);
	OPENCL_ASSERT_ERROR( err );

	if ( (p_tile_data->mem_flags & CTRL_MEM_ALLOC_BOTH) || !((p_tile_data->mem_flags & CTRL_MEM_ALLOC_HOST) || (p_tile_data->mem_flags & CTRL_MEM_ALLOC_DEV) )) {
		// Allocate memory in both host and device
		
		p_tile_data->device_data = clCreateBuffer(p_ctrl->context, CL_MEM_READ_WRITE, ((size_t)(p_tile->acumCard)) * (p_tile->baseExtent), NULL, &err);
		OPENCL_ASSERT_ERROR( err );

		posix_memalign(&p_tile->data, AOCL_ALIGNMENT, (size_t)p_tile->acumCard * p_tile->baseExtent);

		p_tile->memPtr = p_tile->data;
	} else if (p_tile_data->mem_flags & CTRL_MEM_ALLOC_HOST) { // Allocate memory only in host
		fprintf(stderr, "Internal Error: Not supported flag in Alloc: CTRL_MEM_ALLOC_HOST\n"); fflush(stderr); exit(EXIT_FAILURE);
	} else if (p_tile_data->mem_flags & CTRL_MEM_ALLOC_DEV) { // Allocate memory only in device
		fprintf(stderr, "Internal Error: Not supported flag in Alloc: CTRL_MEM_ALLOC_DEV\n"); fflush(stderr); exit(EXIT_FAILURE);
	}

	Ctrl_FPGA_Sync(p_ctrl);
}

void Ctrl_FPGA_EvalTaskFreeTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

	if(p_tile_data->is_initialized){
		cl_event wait_list[6]; 
		wait_list[0] = p_tile_data->kernel_last_read_event;
		wait_list[1] = p_tile_data->kernel_last_write_event;
		wait_list[2] = p_tile_data->offloading_last_read_event;
		wait_list[3] = p_tile_data->offloading_last_write_event;
		wait_list[4] = p_tile_data->host_last_read_event;
		wait_list[5] = p_tile_data->host_last_write_event;
		clWaitForEvents(6, wait_list);

		if (p_tile->memStatus == HIT_MS_OWNER) {
			free(p_tile->data);
			OPENCL_ASSERT_OP( clReleaseMemObject(p_tile_data->device_data) );
			p_tile->memPtr = NULL;
			p_tile->data = NULL;
			p_tile->memStatus = HIT_MS_NOMEM;
		}
	
		OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->kernel_last_read_event) );
		OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->kernel_last_write_event) );

		OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->offloading_last_read_event) );
		OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->offloading_last_write_event) );

		OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->host_last_read_event) );
		OPENCL_ASSERT_OP( clReleaseEvent(p_tile_data->host_last_write_event) );

		OPENCL_ASSERT_OP( clReleaseCommandQueue(p_tile_data->queue) );

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

		p_tile->ext = NULL;
		p_tile_data->p_ctrl = NULL;
		
		p_tile_data->p_tile_elem->p_tile_ext = NULL;
		p_tile_data->p_tile_elem->p_next = NULL;
		p_tile_data->p_tile_elem->p_prev = NULL;

		free(p_tile_data->p_tile_elem);
	}

	free(p_tile_data);

	Ctrl_FPGA_Sync(p_ctrl);
}

void Ctrl_FPGA_EvalTaskMoveTo(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	if (p_tile_data->last_update != CTRL_TILE_LAST_UPDATE_HOST_DEV || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_FPGA_EvalTaskMoveToInner(p_ctrl, p_tile);
	}
	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		OPENCL_ASSERT_OP( clWaitForEvents(1, &(p_tile_data->offloading_last_write_event)) );
		Ctrl_FPGA_Sync(p_ctrl);
	}
}

void Ctrl_FPGA_EvalTaskMoveFrom(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

	if (p_tile_data->last_update != CTRL_TILE_LAST_UPDATE_HOST_DEV || p_ctrl->dependance_mode == CTRL_MODE_EXPLICIT) {
		Ctrl_FPGA_EvalTaskMoveFromInner(p_ctrl, p_tile);
	}

	if (p_ctrl->policy == CTRL_POLICY_SYNC) {
		OPENCL_ASSERT_OP( clWaitForEvents(1, &(p_tile_data->offloading_last_read_event)) );
		Ctrl_FPGA_Sync(p_ctrl);
	}
}

void Ctrl_FPGA_EvalTaskWaitTile(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	HitTile *p_tile = (HitTile *)(p_task->p_tile);
	Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(p_tile->ext);

	if (hit_tileIsNull(*p_tile)) return;

	cl_event wait_list[6]; 
	wait_list[0] = p_tile_data->kernel_last_read_event;
	wait_list[1] = p_tile_data->kernel_last_write_event;
	wait_list[2] = p_tile_data->offloading_last_read_event;
	wait_list[3] = p_tile_data->offloading_last_write_event;
	wait_list[4] = p_tile_data->host_last_read_event;
	wait_list[5] = p_tile_data->host_last_write_event;

	OPENCL_ASSERT_OP( clWaitForEvents(6, wait_list) );
	Ctrl_FPGA_Sync(p_ctrl);
}

void Ctrl_FPGA_EvalTaskSetDependanceMode(Ctrl_FPGA *p_ctrl, Ctrl_Task *p_task) {
	p_ctrl->dependance_mode = p_task->flags;
	if (p_ctrl->policy == CTRL_POLICY_SYNC){
		Ctrl_FPGA_Sync(p_ctrl);
	}
}
///@endcond
