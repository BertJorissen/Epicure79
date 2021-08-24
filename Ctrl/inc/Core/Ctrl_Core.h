#ifndef _CTRL_CORE_H_
#define _CTRL_CORE_H_
/**
 * @file Ctrl_Core.h
 * @author Trasgo Group
 * @brief Prototypes for initializing ctrls and launching operations to ctrls.
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
#ifndef CTRL_FPGA_KERNEL_FILE
	#include <omp.h>
	#include <hwloc.h>
#endif //CTRL_FPGA_KERNEL_FILE

#ifdef _CTRL_DEBUG_
	#include <stdio.h>
#endif

#ifndef CTRL_FPGA_KERNEL_FILE
	#include "hitmap2.h"
#else
	#include "hit_kernel.h"
#endif

#ifndef CTRL_FPGA_KERNEL_FILE
	#include "Core/Ctrl_Policy.h"
	#include "Core/Ctrl_TaskQueue.h"
	#include "Core/Ctrl_Type.h"

	#include "Kernel/Ctrl_KernelArgs.h"
#endif //CTRL_FPGA_KERNEL_FILE

#ifdef _CTRL_ARCH_CPU_
	#include "Architectures/Cpu/Ctrl_Cpu.h"
#else
	#define CTRL_CPU_LAUNCH( ... )
	#define CTRL_CPU_LAUNCH_STREAM( ... )
#endif // _CTRL_ARCH_CPU_


#ifdef _CTRL_ARCH_CUDA_
	#include "Architectures/Cuda/Ctrl_Cuda.h"
#else
	#define CTRL_CUDA_LAUNCH( ... )
	#define CTRL_CUDA_LAUNCH_STREAM( ... )
#endif // _CTRL_ARCH_CUDA_


#ifdef _CTRL_ARCH_OPENCL_GPU_
	#include "Architectures/OpenCL/Ctrl_OpenCL_Gpu.h"
#else
	#define CTRL_OPENCL_GPU_LAUNCH( ... )
	#define CTRL_OPENCL_GPU_LAUNCH_STREAM( ... )
#endif // _CTRL_ARCH_OPENCL_GPU_


#ifdef _CTRL_ARCH_FPGA_
	#ifndef CTRL_FPGA_KERNEL_FILE
		#include "Architectures/FPGA/Ctrl_FPGA.h"
	#endif
#else
	#define CTRL_FPGA_LAUNCH( ... )
	#define CTRL_FPGA_LAUNCH_STREAM( ... )
#endif // _CTRL_ARCH_FPGA_

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef CTRL_FPGA_KERNEL_FILE
#define MAX_BLOCK_SIZE	256
/**
 * @brief Contains specific architecture ctrl structures.
 */
typedef union {

	#ifdef _CTRL_ARCH_CPU_
		Ctrl_Cpu cpu;
	#endif // _CTRL_ARCH_CPU_

	#ifdef _CTRL_ARCH_CUDA_
		struct Ctrl_Cuda cuda; 
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
		struct Ctrl_OpenCLGpu opencl_gpu;
	#endif // _CTRL_ARCH_OPENCL_GPU_

	#ifdef _CTRL_ARCH_FPGA_
		struct Ctrl_FPGA fpga;
	#endif // _CTRL_ARCH_OPENCL_GPU_

} Ctrl_Impl;

/**
 * @brief Abstract controller.
 */
typedef struct {
  int         id;         /**< Id of the ctrl */
  Ctrl_Type     type;       /**< Type of the ctrl */
  Ctrl_Impl     *p_impl;      /**< Specific implementation for this type of ctrl  */
#ifdef _CTRL_QUEUE_
  omp_lock_t      *p_lock_first_host; /**< Lock to sync with queue manager thread */
  omp_lock_t      *p_lock_first_ctrl; /**< Lock to sync with queue manager thread */
  omp_lock_t      *p_lock_host;   /**< Lock to sync with queue manager thread */
  omp_lock_t      *p_lock_ctrl;   /**< Lock to sync with queue manager thread */

  Ctrl_TaskQueue    *p_task_queue;    /**< Task queue for this ctrl */
#endif //_CTRL_QUEUE_
  Ctrl_Policy     policy;       /**< Policy to be used by this ctrl */
  int         device;       /**< Used to decide which kernel implementation is prefered */
  void        *p_ext;       /**< IDK what this is for */

} Ctrl;

typedef Ctrl * PCtrl;

/**
 * Launch a kernel to the ctrl queue.
 * This call is always asynchronous.
 * 
 * @hideinitializer
 * 
 * @param p_ctrl ctrl to launch kernel.
 * @param name kernel to launch.
 * @param threads block of threads to execute the kernel with.
 * @param group block sizes for this kernel execution. .
 * 		Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param ... arguments for the kernel.
 * 
 * @see CTRL_KERNEL, CTRL_KERNEL_PROTO, Ctrl_LaunchToStream
 */
#define Ctrl_Launch( p_ctrl, name, threads, group, ... ) \
	switch ((p_ctrl)->type) \
	{ \
		CTRL_CPU_LAUNCH( p_ctrl, name, threads, group, __VA_ARGS__ ) \
		CTRL_CUDA_LAUNCH( p_ctrl, name, threads, group, __VA_ARGS__ ) \
		CTRL_OPENCL_GPU_LAUNCH( p_ctrl, name, threads, group, __VA_ARGS__ ) \
		CTRL_FPGA_LAUNCH( p_ctrl, name, threads, group, __VA_ARGS__ ) \
		default: \
			fprintf(stderr, "[Ctrl_Launch] Unsupported Ctrl type: %d. Recompile the library with the proper support.\n", p_ctrl->type); \
			exit(EXIT_FAILURE); \
	}

/**
 * Launch a kernel to the ctrl queue, and to a given stream.
 * This call is always asynchronous.
 * 
 * @hideinitializer
 * 
 * @param p_ctrl: ctrl to launch kernel
 * @param name: kernel to launch
 * @param threads: block of threads to execute the kernel with
 * @param group: block sizes for this kernel execution. 
 * 		Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param stream: stream to launch the kernel to.
 * @param ...: arguments for the kernel
 * 
 * @see CTRL_KERNEL, CTRL_KERNEL_PROTO, Ctrl_Launch
 */
#define Ctrl_LaunchToStream( p_ctrl, name, threads, group, stream, ... ) \
	switch ((p_ctrl)->type) \
	{ \
		CTRL_CPU_LAUNCH_STREAM( p_ctrl, name, threads, group, stream,  __VA_ARGS__ ) \
		CTRL_CUDA_LAUNCH_STREAM( p_ctrl, name, threads, group, stream, __VA_ARGS__ ) \
		CTRL_OPENCL_GPU_LAUNCH_STREAM( p_ctrl, name, threads, group, stream, __VA_ARGS__ ) \
		CTRL_FPGA_LAUNCH_STREAM( p_ctrl, name, threads, group, stream, __VA_ARGS__ ) \
		default: \
			fprintf(stderr, "[Ctrl_Launch] Unsupported Ctrl type: %d. Recompile the library with the proper support.\n", p_ctrl->type); \
			exit(EXIT_FAILURE); \
	}


/**
 * Launch a host task to the ctrl queue.
 * This call is synchronous if \p p_ctrl policy is CTRL_POLICY_SYNC otherwise it is asynchronous.
 * 
 * @hideinitializer
 * 
 * @param p_ctrl ctrl to launch host task.
 * @param name host task to launch.
 * @param ... arguments for the host task.
 * 
 * @see CTRL_HOST_TASK, CTRL_HOST_TASK_PROTO
 */
#define Ctrl_HostTask( p_ctrl, name, ... ) \
	Ctrl_LaunchHostTask(p_ctrl, Ctrl_HostTaskCreate_##name( CTRL_KERNEL_ARGS_TO_POINTERS( __VA_ARGS__) )); \

/*
 * Queue thread errors and warnings. For everybody's safety :D
 */
#ifdef _CTRL_QUEUE_
#define _CTRL_QUEUE_WARN_1_
#define _CTRL_QUEUE_WARN_0_ fprintf(stderr, "[Ctrl Core] Internal Error: not enough threads for the Ctrl Queue.\n\n"); fflush(stderr); exit(EXIT_FAILURE);
#else
#define _CTRL_QUEUE_WARN_1_
#if defined(_CTRL_ARCH_CPU_) && defined(_CTRL_DEBUG_)
#define _CTRL_QUEUE_WARN_0_ fprintf(stderr, "[Ctrl Core] Internal Warning: CPU support is enabled, but no spawner thread was provided. Use __ctrl_block__(1, 1) if using a CPU Ctrl; otherwise, the program will probably enter a deadlock.\n\n"); fflush(stderr);
#else
#define _CTRL_QUEUE_WARN_0_
#endif
#endif

/**
 * Iniciate a block where ctrls can be created and used
 * 
 * @hideinitializer
 * 
 * @param num_ctrl number of ctrls to be used in the block.
 * @param num_ctrl_threads number of ctrls that will need extra threads to work and will be used in the block (currently, it's value must be the same as \p num_ctrl ).
 * @todo allow more than 1 ctrl
 */
#define __ctrl_block__(num_ctrl, num_ctrl_threads)		\
	_CTRL_QUEUE_WARN_##num_ctrl_threads##_ \
	omp_set_max_active_levels(2);					\
	omp_set_num_threads(2 + num_ctrl_threads);		\
	_Pragma("omp parallel") if (Ctrl_Thread_Init() == 0)

/**
 * Generate particular polymorphic types of HitTile and KHitTile.
 *
 * This macro allows to define particular polymorphic types of HitTile. The new defined type
 * will be named: \e HitTile_\<baseType\>.
 *
 * @hideinitializer
 * 
 * @param type Name of a valid native or derived C type.
 */
#define Ctrl_NewType( type ) \
		hitNewType( type ) ; \
		hit_ktileNewType( type ); \
		static inline HitTile_##type Ctrl_Domain_##type(Ctrl *p_ctrl, HitShape shape) { \
			HitTile_##type new_tile = HIT_TILE_NULL_STATIC; \
			hit_tileDomainShape( &new_tile, type, shape ); \
			Ctrl_DomainInner( p_ctrl, ((HitTile *)(&new_tile)) ); \
			return new_tile; \
		} \
		static inline HitTile_##type Ctrl_DomainAlloc_##type(Ctrl *p_ctrl, HitShape shape, int flags) { \
			HitTile_##type new_tile = Ctrl_Domain_##type(p_ctrl, shape); \
			Ctrl_Alloc(p_ctrl, new_tile, flags); \
			return new_tile; \
		} \
			static inline HitTile_##type Ctrl_Select_##type(Ctrl *p_ctrl, HitTile *p_parent, HitShape shape, int flags) { \
			HitTile_##type new_tile = HIT_TILE_NULL_STATIC; \
			if ( hit_shapeDims(shape) == -1 ) return *(HitTile_##type*) &HIT_TILE_NULL; \
			if ( ! (flags & CTRL_SELECT_ARR_COORD) && ! (flags & CTRL_SELECT_NO_BOUND) ) { \
				hit_tileSelect( &new_tile, p_parent, shape ); \
			} else if ( (flags & CTRL_SELECT_ARR_COORD) && ! (flags & CTRL_SELECT_NO_BOUND) ) { \
				hit_tileSelectArrayCoords( &new_tile, p_parent, shape ); \
			} else if ( ! (flags & CTRL_SELECT_ARR_COORD) && (flags & CTRL_SELECT_NO_BOUND) ) { \
				hit_tileSelectNoBoundary( &new_tile, p_parent, shape ); \
			} else if ( (flags & CTRL_SELECT_ARR_COORD) && (flags & CTRL_SELECT_NO_BOUND)  ) { \
				hit_tileSelectArrayCoordsNoBoundary( &new_tile, p_parent, shape ); \
			} else { \
				fprintf(stderr, "Internal Error: Unknow flags in select\n"); fflush(stderr); exit(EXIT_FAILURE); \
			} \
			Ctrl_SelectInner( p_ctrl, ((HitTile *)(&new_tile)), flags ); \
			return new_tile; \
		}
#else
#ifndef CTRL_HOST_COMPILE
	#ifndef CTRL_FPGA_BIN_NAME
		#define Ctrl_NewType( type ) \
				hit_ktileNewType( type );
	#else //CTRL_FPGA_BIN_NAME
		#define Ctrl_NewType( type )
	#endif //CTRL_FPGA_BIN_NAME
#else //CTRL_HOST_COMPILE
	#define Ctrl_NewType( type )
#endif //CTRL_HOST_COMPILE
#endif

#ifndef CTRL_FPGA_KERNEL_FILE

/**
 * Create a Ctrl
 * 
 * @hideinitializer
 * 
 * @param type the type of ctrl.
 * @param policy policy for the ctrl to use.
 * @param ... extra parameters needed for specific types of ctrls.
 * 
 * @returns a pointer to the ctrl
 * 
 * @see Ctrl_CreateWrapper
 */
#define Ctrl_Create( type, policy, ... ) Ctrl_CreateWrapperMacro_##type(policy, __VA_ARGS__ );

#ifdef _CTRL_ARCH_FPGA_
	#define Ctrl_CreateWrapperMacro_CTRL_TYPE_FPGA(...) Ctrl_CreateWrapperFPGA(__VA_ARGS__, streams, noStreams)(__VA_ARGS__)
	#define Ctrl_CreateWrapperFPGA(_1, _2, _3, _4, _5, type, ...) Ctrl_CreateWrapperFPGA_##type
	#define Ctrl_CreateWrapperFPGA_streams(policy, device, platform, exec_mode, streams) Ctrl_CreateWrapper_CTRL_TYPE_FPGA(policy, device, platform, exec_mode, streams)
	#define Ctrl_CreateWrapperFPGA_noStreams(policy, device, platform, exec_mode) Ctrl_CreateWrapper_CTRL_TYPE_FPGA(policy, device, platform, exec_mode, 1)
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_FPGA(Ctrl_Policy policy, int device, int platform, int exec_mode, int streams);
#endif

#ifdef _CTRL_ARCH_OPENCL_GPU_
	#define Ctrl_CreateWrapperMacro_CTRL_TYPE_OPENCL_GPU(...) Ctrl_CreateWrapperCL_GPU(__VA_ARGS__, streams, noStreams)(__VA_ARGS__)
	#define Ctrl_CreateWrapperCL_GPU(_1, _2, _3, _4, type, ...) Ctrl_CreateWrapperCL_GPU_##type
	#define Ctrl_CreateWrapperCL_GPU_streams(policy, device, platform, streams) Ctrl_CreateWrapper_CTRL_TYPE_OPENCL_GPU(policy, device, platform, streams)
	#define Ctrl_CreateWrapperCL_GPU_noStreams(policy, device, platform) Ctrl_CreateWrapper_CTRL_TYPE_OPENCL_GPU(policy, device, platform, 1)
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_OPENCL_GPU(Ctrl_Policy policy, int device, int platform, int streams);
#endif

#ifdef _CTRL_ARCH_CUDA_
	#define Ctrl_CreateWrapperMacro_CTRL_TYPE_CUDA(...) Ctrl_CreateWrapperCUDA(__VA_ARGS__, streams, noStreams)(__VA_ARGS__)
	#define Ctrl_CreateWrapperCUDA(_1, _2, _3, type, ...) Ctrl_CreateWrapperCUDA_##type
	#define Ctrl_CreateWrapperCUDA_streams(policy, device, streams) Ctrl_CreateWrapper_CTRL_TYPE_CUDA(policy, device, streams)
	#define Ctrl_CreateWrapperCUDA_noStreams(policy, device) Ctrl_CreateWrapper_CTRL_TYPE_CUDA(policy, device, 1)
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_CUDA(Ctrl_Policy policy, int device, int streams);
#endif

#ifdef _CTRL_ARCH_CPU_
	#define Ctrl_CreateWrapperMacro_CTRL_TYPE_CPU(...) Ctrl_CreateWrapper_CTRL_TYPE_CPU(__VA_ARGS__)
	PCtrl Ctrl_CreateWrapper_CTRL_TYPE_CPU(Ctrl_Policy policy, int n_threads, int *p_numa_nodes, int n_numanodes, bool mem_moves);
#endif

#ifdef DOXYGEN
	/**
	 * Allocate memory for a tile.
	 * This is always a synchronous call.
	 * @hideinitializer
	 * 
	 * @param ctrl pointer to ctrl to allocate memory with.
	 * @param tile the tile to allocate memory to.
	 * @param flags memory flags for the allocation. This parameter is optional and by default is CTRL_MEM_ALLOC_BOTH. .
	 * 		Valid values include:
	 * 			CTRL_MEM_ALLOC_BOTH: Allocate memory on both host and device.
	 * 			CTRL_MEM_ALLOC_HOST: Allocate memory only on host.
	 * 			CTRL_MEM_ALLOC_DEV: Allocate memory only on device.
	 * 			CTRL_MEM_PINNED: Allocate pinned memory if possible.
	 * 			CTRL_MEM_NOPINNED: Allocate non pinned memory.
	 * 		Flags CTRL_MEM_ALLOC_BOTH, CTRL_MEM_ALLOC_HOST and CTRL_MEM_ALLOC_DEV are mutually exclusive and if multiple of 
	 * 		them are specified at the same time, CTRL_MEM_ALLOC_BOTH will have the most priority, then CTRL_MEM_ALLOC_HOST, 
	 * 		and finally CTRL_MEM_ALLOC_DEV. If none of them are specified the default value will apply (CTRL_MEM_ALLOC_BOTH).
	 * 		Similarly CTRL_MEM_PINNED and CTRL_MEM_NOPINNED are mutually exclusive, if both are specified CTRL_MEM_PINNED will
	 * 		take priority. If neither of them are specified default behaviour based on \p ctrl type and possible more factors
	 * 		will apply.
	 */
	#define Ctrl_Alloc(ctrl, tile, flags)
#else //DOXYGEN
	#define Ctrl_Alloc(...) Ctrl_AllocMacro( __VA_ARGS__, WithFlags, NoFlags )( __VA_ARGS__ )
#endif //DOXYGEN

#define Ctrl_AllocMacro(_1, _2, _3, AllocType, ...) Ctrl_Alloc##AllocType
#define Ctrl_AllocWithFlags(p_ctrl, tile, flags) Ctrl_AllocInner( p_ctrl, (HitTile *)&tile, flags )
#define Ctrl_AllocNoFlags(p_ctrl, tile) Ctrl_AllocInner( p_ctrl, (HitTile *)&tile, CTRL_MEM_ALLOC_BOTH )
void Ctrl_AllocInner(Ctrl *p_ctrl, HitTile *p_tile, int flags);

/**
 * Create a tile.
 * 
 * Create and initialize the shape and base type of a new multidimensional array tile,
 * and attach it to ctrl.
 * This is always a synchronous call.
 * 
 * @hideinitializer
 * 
 * @param ctrl pointer to ctrl to attach the tile to.
 * @param type type of data inside the tile.
 * @param shape hit_shape used to create the tile.
 * 
 * @returns HitTile_type created
 * 
 * @pre \p type must have been declared as a type with @see Ctrl_NewType
 */
#define Ctrl_Domain(ctrl, type, shape) Ctrl_Domain_##type( ctrl, shape );
void Ctrl_DomainInner(Ctrl *p_ctrl, HitTile *p_tile);

#ifdef DOXYGEN
	/**
	 * Create a tile and allocate memory for it.
	 * 
	 * Create the shape and base type of a new multidimensional array tile, allocate
	 * memory for it in both host and device memory, and attach it to the ctrl.
	 * 
	 * This is always a synchronous call.
	 *
	 * @hideinitializer
	 *
	 * @param ctrl pointer to ctrl to attach the tile to and allocate memory with.
	 * @param type type of data inside the tile.
	 * @param shape hit_shape used to create the tile.
	 * @param flags memory flags for the allocation. This parameter is optional and by default is CTRL_MEM_ALLOC_BOTH. .
	 * 		Valid values include:
	 * 			CTRL_MEM_ALLOC_BOTH: Allocate memory on both host and device
	 * 			CTRL_MEM_ALLOC_HOST: Allocate memory only on host
	 * 			CTRL_MEM_ALLOC_DEV: Allocate memory only on device
	 *			CTRL_MEM_PINNED: Allocate pinned memory if possible.
	 * 			CTRL_MEM_NOPINNED: Allocate non pinned memory.
	 * 		Flags CTRL_MEM_ALLOC_BOTH, CTRL_MEM_ALLOC_HOST and CTRL_MEM_ALLOC_DEV are mutually exclusive and if multiple of 
	 * 		them are specified at the same time, CTRL_MEM_ALLOC_BOTH will have the most priority, then CTRL_MEM_ALLOC_HOST, 
	 * 		and finally CTRL_MEM_ALLOC_DEV. If none of them are specified the default value will apply (CTRL_MEM_ALLOC_BOTH).
	 * 		Similarly CTRL_MEM_PINNED and CTRL_MEM_NOPINNED are mutually exclusive, if both are specified CTRL_MEM_PINNED will
	 * 		take priority. If neither of them are specified default behaviour based on \p ctrl type and possible more factors
	 * 		will apply.
	 * 
	 * @returns HitTile_type created and allocated
	 * 
	 * @pre \p type must have been declared as a type with @see Ctrl_NewType
	 */
	#define Ctrl_DomainAlloc(ctrl, type, shape, flags)
#else //DOXYGEN
	#define Ctrl_DomainAlloc(...) Ctrl_DomainAllocMacro( __VA_ARGS__, WithFlags, NoFlags )( __VA_ARGS__ )
#endif //DOXYGEN

#define Ctrl_DomainAllocMacro(_1, _2, _3, _4, AllocType, ...) Ctrl_DomainAlloc##AllocType
#define Ctrl_DomainAllocWithFlags(ctrl, type, shape, flags) Ctrl_DomainAlloc_##type( ctrl, shape, flags )
#define Ctrl_DomainAllocNoFlags(ctrl, type, shape) Ctrl_DomainAlloc_##type( ctrl, shape, CTRL_MEM_ALLOC_BOTH )

/**
 * Create a tile subselection.
 *
 * Initialize a hierarchical subselection using tile coordinates.
 * This is always a synchronous call.
 *
 * @hideinitializer
 *
 * @param ctrl: pointer to ctrl that the parent tile is attached to.
 * @param type: type of data inside the tiles.
 * @param p_parent: parent tile to make a subselection from.
 * @param flags: subselection flags.
 *
 * @returns HitTile_type, the subselection from p_tile.
 *
 * @pre \p type must have been declared as a type with @see Ctrl_NewType
 */
#define Ctrl_Select(ctrl, type, p_parent, shape, flags) Ctrl_Select_##type( ctrl, ((HitTile *)(&p_parent)), shape, flags );
void Ctrl_SelectInner(Ctrl *p_ctrl, HitTile *p_tile, int flags);

/**
 * Free multiple tiles.
 * This is always a synchronous call.
 * 
 * @hideinitializer
 * 
 * @param ctrl pointer to ctrl that has the tiles attached.
 * @param ... tiles to free.
 * 
 * @pre tiles have to be allocated and attached to \p ctrl.
 * @see Ctrl_Alloc
 */
#define Ctrl_Free( ctrl, ... ) Ctrl_FreeN( ctrl, CTRL_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
#define Ctrl_FreeN( ctrl, n_tiles, ... ) Ctrl_FreeN2( ctrl, n_tiles, __VA_ARGS__ )
#define Ctrl_FreeN2( ctrl, n_tiles, ... ) Ctrl_OP_##n_tiles( Free, ctrl, __VA_ARGS__ )
void Ctrl_FreeInner(Ctrl *p_ctrl, HitTile *p_tile);

/**
 * Move multiple tiles from host memory to device.
 * This call is synchronous if \p ctrl policy is CTRL_POLICY_SYNC otherwise it is asynchronous.
 * 
 * @hideinitializer
 * 
 * @param ctrl pointer to ctrl that has the tiles attached.
 * @param ... tiles to be moved.
 * 
 * @pre tiles have to be allocated and attached to \p ctrl.
 * @see Ctrl_Alloc
 */
#define Ctrl_MoveTo( ctrl, ... ) Ctrl_MoveToN( ctrl, CTRL_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
#define Ctrl_MoveToN( ctrl, n_tiles, ... ) Ctrl_MoveToN2( ctrl, n_tiles, __VA_ARGS__ )
#define Ctrl_MoveToN2( ctrl, n_tiles, ... ) Ctrl_OP_##n_tiles( MoveTo, ctrl, __VA_ARGS__ )
void Ctrl_MoveToInner(Ctrl *p_ctrl, HitTile *p_tile);

/**
 * Move multiple tiles from device memory to host.
 * This call is synchronous if \p ctrl policy is CTRL_POLICY_SYNC otherwise it is asynchronous.
 * 
 * @hideinitializer
 * 
 * @param ctrl pointer to ctrl that has the tiles attached.
 * @param ... tiles to be moved.
 * 
 * @pre tiles have to be allocated and attached to \p ctrl.
 * @see Ctrl_Alloc
 */
#define Ctrl_MoveFrom( ctrl, ... ) Ctrl_MoveFromN( ctrl, CTRL_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
#define Ctrl_MoveFromN( ctrl, n_tiles, ... ) Ctrl_MoveFromN2( ctrl, n_tiles, __VA_ARGS__ )
#define Ctrl_MoveFromN2( ctrl, n_tiles, ... ) Ctrl_OP_##n_tiles( MoveFrom, ctrl, __VA_ARGS__ )
void Ctrl_MoveFromInner(Ctrl *p_ctrl, HitTile *p_tile);

/**
 * Wait for all operations involving the tiles to end.
 * This call is always synchronous.
 * 
 * @hideinitializer
 * 
 * @param ctrl pointer to ctrl that has the tiles attached.
 * @param ... tiles to wait to.
 * 
 * @pre tiles have to be allocated and attached to \p ctrl
 * @see Ctrl_Alloc
 */
#define Ctrl_WaitTile( ctrl, ... ) Ctrl_WaitTileN( ctrl, CTRL_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
#define Ctrl_WaitTileN( ctrl, n_tiles, ... ) Ctrl_WaitTileN2( ctrl, n_tiles, __VA_ARGS__ )
#define Ctrl_WaitTileN2( ctrl, n_tiles, ... ) Ctrl_OP_##n_tiles( WaitTile, ctrl, __VA_ARGS__ )
void Ctrl_WaitTileInner(Ctrl *p_ctrl, HitTile *p_tile);

/**
 * Wait for all operations involving all the tiles attached to \p ctrl to end.
 * This call is always synchronous.
 * 
 * @param p_ctrl Pointer to ctrl that has the tiles attached.
 */
void Ctrl_GlobalSync(Ctrl *p_ctrl);

/**
 * Wait for all host tasks to finish
 */
void Ctrl_Hosttask_Sync();

/**
 * Wait for all operations to finish
 */
void Ctrl_Synchronize();

/**
 * Set the host affinity to \p node numa node.
 * This is useful to set the affinity on a node closer to the device for performance purpouses.
 * 
 * @param node Index of the numa node where host threads should be binded to.
 * 
 * @pre This function must be called before calling __ctrl_block__
 */
void Ctrl_SetHostAffinity(int node);

/**
 * Destroy a ctrl.
 * 
 * @param p_ctrl pointer to ctrl to be destroyed.
 */
void Ctrl_Destroy(Ctrl *p_ctrl);

/**
 * @brief Change dependance mode for this ctrl.
 * 
 * All tasks sent from this point will use the mode provided. Tasks already sent will not be affected.
 * 
 * @param p_ctrl pointer to ctrl to be changed.
 * @param mode new mode for \p p_ctrl .
 * 		Valid values include:
 * 			CTRL_MODE_IMPLICIT: ctrl does memory transfers automatically when needed. This is default.
 * 			CTRL_MODE_EXPLICIT: ctrl does memory transfers only when explicitly told to.
 */
void Ctrl_SetDependanceMode(Ctrl *p_ctrl, int mode);

/// @cond INTERNAL
/**
 * Push \p task to \p p_ctrl queue to execute.
 * 
 * @param p_ctrl Ctrl to launch the kernel.
 * @param task Task of the kernel to be launched.
 * 
 * @see Ctrl_Launch
 */
void Ctrl_LaunchKernel(Ctrl *p_ctrl, Ctrl_Task task);

/**
 * Push \p task to \p p_ctrl queue to execute and wait for it if \p p_ctrl is CTRL_POLICY_SYNC.
 * 
 * @param p_ctrl Ctrl to launch the host task.
 * @param task Host task to be launched.
 * 
 * @see Ctrl_HostTask
 */
void Ctrl_LaunchHostTask(Ctrl *p_ctrl, Ctrl_Task task);

/**
 * Set affinity of threads and send them to therir tasks
 * @returns 0 if master 1 otherwise
 */
int Ctrl_Thread_Init();

/**
 * Function to choose the best implementation for a given kernel.
 * 
 * @param type Type of the ctrl to launch the kernel.
 * @param avail_impls list of avaiable implementations.
 * @param  n_impl Number of avaiable implementations.
 * @return int Implementation chosen.
 */
int Ctrl_Dev(Ctrl_Type type, int* avail_impls, int n_impl);

#define Ctrl_OP_1( op, ctrl, tile )   Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile)));
#define Ctrl_OP_2( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_1( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_3( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_2( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_4( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_3( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_5( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_4( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_6( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_5( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_7( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_6( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_8( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_7( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_9( op, ctrl, tile, ... )  Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_8( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_10( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_9( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_11( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_10( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_12( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_11( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_13( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_12( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_14( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_13( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_15( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_14( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_16( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_15( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_17( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_16( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_18( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_17( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_19( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_18( op, ctrl, __VA_ARGS__ )
#define Ctrl_OP_20( op, ctrl, tile, ... ) Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile))); Ctrl_OP_19( op, ctrl, __VA_ARGS__ )

#ifdef __cplusplus
	}
#endif

///@endcond
#endif // CTRL_FPGA_KERNEL_FILE

#endif /* _CTRL_CORE_H_ */
