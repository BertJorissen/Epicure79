#ifndef _CTRL_CORE_H_
#define _CTRL_CORE_H_
/**
 * @file Ctrl_Core.h
 * @brief Prototypes for initializing ctrls and launching operations to ctrls.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */
#ifndef CTRL_FPGA_KERNEL_FILE
#include <hwloc.h>
#include <omp.h>
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
#include "Core/Ctrl_Info.h"
#include "Core/Ctrl_KHParams.h"
#include "Core/Ctrl_Policy.h"
#include "Core/Ctrl_TaskQueue.h"
#include "Core/Ctrl_TexDesc.h"
#include "Core/Ctrl_Type.h"

#include "Kernel/Ctrl_KernelArgs.h"
#endif //CTRL_FPGA_KERNEL_FILE

#ifdef _CTRL_ARCH_CPU_
#include "Architectures/Cpu/Ctrl_Cpu.h"
#else
#define CTRL_CPU_LAUNCH(...)
#define CTRL_CPU_LAUNCH_STREAM(...)
#endif // _CTRL_ARCH_CPU_

#ifdef _CTRL_ARCH_CUDA_
#include "Architectures/Cuda/Ctrl_Cuda.h"
#else
#define CTRL_CUDA_LAUNCH(...)
#define CTRL_CUDA_LAUNCH_STREAM(...)
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_HIP_
#include "Architectures/Hip/Ctrl_Hip.h"
#else
#define CTRL_HIP_LAUNCH(...)
#define CTRL_HIP_LAUNCH_STREAM(...)
#endif // _CTRL_ARCH_HIP_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include "Architectures/OpenCL/Ctrl_OpenCL_Gpu.h"
#else
#define CTRL_OPENCL_GPU_LAUNCH(...)
#define CTRL_OPENCL_GPU_LAUNCH_STREAM(...)
#endif // _CTRL_ARCH_OPENCL_GPU_

#ifdef _CTRL_ARCH_FPGA_
#ifndef CTRL_FPGA_KERNEL_FILE
#include "Architectures/FPGA/Ctrl_FPGA.h"
#endif
#else
#define CTRL_FPGA_LAUNCH(...)
#define CTRL_FPGA_LAUNCH_STREAM(...)
#endif // _CTRL_ARCH_FPGA_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Ctrl initizalization to use Controllers in distributed environments
 * @param int *argc	Pointer to the number of program arguments
 * @param char **argv[]	Pointer to the list of program arguments
 */
#define Ctrl_Init(pargc, pargv) hit_comInit(pargc, pargv)

/**
 * Ctrl finalization to use Controllers in distributed environments
 */
#define Ctrl_Finalize() hit_comFinalize()

#ifndef CTRL_FPGA_KERNEL_FILE
#define MAX_BLOCK_SIZE 256
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

	#ifdef _CTRL_ARCH_HIP_
	struct Ctrl_Hip hip;
	#endif // _CTRL_ARCH_HIP_

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
	int        id;     /**< Id of the ctrl */
	Ctrl_Type  type;   /**< Type of the ctrl */
	Ctrl_Impl *p_impl; /**< Specific implementation for this type of ctrl  */
	int        device; /**< Used to decide which kernel implementation is prefered */
} Ctrl;

typedef Ctrl *PCtrl;

/**
 * Launch a kernel to the ctrl queue.
 * This call is always asynchronous.
 *
 * @hideinitializer
 *
 * @param p_ctrl ctrl to launch kernel.
 * @param name kernel to launch.
 * @param threads block of threads to execute the kernel with.
 * @param group block sizes for this kernel execution.
 * 		Optional, if a block with 0 dimensions is passed (such as CTRL_THREAD_NULL), default characterization is used instead.
 * @param ... arguments for the kernel.
 *
 * @see CTRL_KERNEL, CTRL_KERNEL_PROTO, Ctrl_LaunchToStream
 */
#define Ctrl_Launch(p_ctrl, name, threads, group, ...)                                                                                  \
	switch ((p_ctrl)->type) {                                                                                                           \
		CTRL_CPU_LAUNCH(p_ctrl, name, threads, group, __VA_ARGS__)                                                                      \
		CTRL_CUDA_LAUNCH(p_ctrl, name, threads, group, __VA_ARGS__)                                                                     \
		CTRL_HIP_LAUNCH(p_ctrl, name, threads, group, __VA_ARGS__)                                                                      \
		CTRL_OPENCL_GPU_LAUNCH(p_ctrl, name, threads, group, __VA_ARGS__)                                                               \
		CTRL_FPGA_LAUNCH(p_ctrl, name, threads, group, __VA_ARGS__)                                                                     \
		default:                                                                                                                        \
			fprintf(stderr, "[Ctrl_Launch] Unsupported Ctrl type: %d. Recompile the library with the proper support.\n", p_ctrl->type); \
			exit(EXIT_FAILURE);                                                                                                         \
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
#define Ctrl_LaunchToStream(p_ctrl, name, threads, group, stream, ...)                                                                  \
	switch ((p_ctrl)->type) {                                                                                                           \
		CTRL_CPU_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, __VA_ARGS__)                                                       \
		CTRL_CUDA_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, __VA_ARGS__)                                                      \
		CTRL_HIP_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, __VA_ARGS__)                                                       \
		CTRL_OPENCL_GPU_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, __VA_ARGS__)                                                \
		CTRL_FPGA_LAUNCH_STREAM(p_ctrl, name, threads, group, stream, __VA_ARGS__)                                                      \
		default:                                                                                                                        \
			fprintf(stderr, "[Ctrl_Launch] Unsupported Ctrl type: %d. Recompile the library with the proper support.\n", p_ctrl->type); \
			exit(EXIT_FAILURE);                                                                                                         \
	}

/**
 * Launch a host task to the host queue.
 * This call is synchronous if policy is CTRL_POLICY_SYNC otherwise it is asynchronous.
 *
 * @hideinitializer
 *
 * @param name host task to launch.
 * @param ... arguments for the host task.
 *
 * @see CTRL_HOST_TASK, CTRL_HOST_TASK_PROTO
 */
#define Ctrl_HostTask(name, ...) \
	Ctrl_LaunchHostTask(Ctrl_HostTaskCreate_##name(CTRL_KERNEL_ARGS_TO_POINTERS(__VA_ARGS__)));

/**
 * Iniciate a block where ctrls can be created and used
 *
 * @hideinitializer
 *
 * @param config_file path to config file.
 */
#define __ctrl_block__(config_file) \
	Ctrl_ParseConfig(config_file);  \
	omp_set_nested(1);              \
	_Pragma("omp parallel")         \
	if (Ctrl_Thread_Init() == 0)

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
#define Ctrl_NewType(type) Ctrl_NewType2(type)
#define Ctrl_NewType2(type)                                                                                                \
	hitNewType(type);                                                                                                      \
	hit_ktileNewType(type);                                                                                                \
	static inline HitTile_##type Ctrl_Select_##type(HitTile *p_parent, HitShape shape, int flags) __attribute__((unused)); \
	static inline HitTile_##type Ctrl_DomainAlloc_##type(Ctrl *p_ctrl, HitShape shape, int flags) __attribute__((unused)); \
	static inline HitTile_##type Ctrl_Domain_##type(HitShape shape) __attribute__((unused));                               \
	static inline HitTile_##type Ctrl_Domain_##type(HitShape shape) {                                                      \
		HitTile_##type new_tile = hitTileNoMem_##type(shape);                                                              \
		Ctrl_DomainInner(((HitTile *)(&new_tile)));                                                                        \
		return new_tile;                                                                                                   \
	}                                                                                                                      \
	static inline HitTile_##type Ctrl_DomainAlloc_##type(Ctrl *p_ctrl, HitShape shape, int flags) {                        \
		HitTile_##type new_tile = Ctrl_Domain_##type(shape);                                                               \
		Ctrl_Alloc(p_ctrl, new_tile, flags);                                                                               \
		return new_tile;                                                                                                   \
	}                                                                                                                      \
	static inline HitTile_##type Ctrl_Select_##type(HitTile *p_parent, HitShape shape, int flags) {                        \
		HitTile_##type new_tile = HIT_TILE_NULL_STATIC;                                                                    \
		if (hit_shapeDims(shape) == -1) return *(HitTile_##type *)&HIT_TILE_NULL;                                          \
		if (!(flags & CTRL_SELECT_ARR_COORD) && !(flags & CTRL_SELECT_NO_BOUND)) {                                         \
			hit_tileSelect(&new_tile, p_parent, shape);                                                                    \
		} else if ((flags & CTRL_SELECT_ARR_COORD) && !(flags & CTRL_SELECT_NO_BOUND)) {                                   \
			hit_tileSelectArrayCoords(&new_tile, p_parent, shape);                                                         \
		} else if (!(flags & CTRL_SELECT_ARR_COORD) && (flags & CTRL_SELECT_NO_BOUND)) {                                   \
			hit_tileSelectNoBoundary(&new_tile, p_parent, shape);                                                          \
		} else if ((flags & CTRL_SELECT_ARR_COORD) && (flags & CTRL_SELECT_NO_BOUND)) {                                    \
			hit_tileSelectArrayCoordsNoBoundary(&new_tile, p_parent, shape);                                               \
		} else {                                                                                                           \
			fprintf(stderr, "Internal Error: Unknow flags in select\n");                                                   \
			fflush(stderr);                                                                                                \
			exit(EXIT_FAILURE);                                                                                            \
		}                                                                                                                  \
		Ctrl_SelectInner(((HitTile *)(&new_tile)), flags);                                                                 \
		return new_tile;                                                                                                   \
	}
#else
#ifndef CTRL_HOST_COMPILE
#ifndef CTRL_FPGA_BIN_NAME
#define Ctrl_NewType(type) \
	hit_ktileNewType(type);
#else //CTRL_FPGA_BIN_NAME
#define Ctrl_NewType(type)
#endif //CTRL_FPGA_BIN_NAME
#else //CTRL_HOST_COMPILE
#define Ctrl_NewType(type)
#endif //CTRL_HOST_COMPILE
#endif

#ifndef CTRL_FPGA_KERNEL_FILE

#ifdef DOXYGEN
/**
 * Allocate memory for a tile.
 * This is always a synchronous call.
 * @hideinitializer
 *
 * @param ctrl pointer to ctrl to allocate memory with.
 * @param tile the tile to allocate memory to.
 * @param flags memory flags for the allocation. This parameter is optional and by default is 0.
 * 		Valid values include:
 * 			CTRL_MEM_ALLOC_HOST: Allocate memory only on host.
 * 			CTRL_MEM_ALLOC_DEV: Allocate memory only on device.
 * 			CTRL_MEM_PINNED: Allocate pinned memory if possible.
 * 			CTRL_MEM_NOPINNED: Allocate non pinned memory.
 * 			CTRL_MEM_ALIGNED: Try to allocate aligned memory on the device. Currently only works for CUDA, HIP and OpenCL 2D.
 * 		Flags CTRL_MEM_ALLOC_HOST and CTRL_MEM_ALLOC_DEV are mutually exclusive.
 * 		If none of them or both are specified the default behaviour (allocating memory on host and device) will apply.
 * 		Similarly CTRL_MEM_PINNED and CTRL_MEM_NOPINNED are mutually exclusive, if both are specified CTRL_MEM_PINNED will
 * 		take priority. If neither of them are specified default behaviour based on \p ctrl type and possible more factors
 * 		will apply.
 */
#define Ctrl_Alloc(ctrl, tile, flags)
#else //DOXYGEN
#define Ctrl_Alloc(...) Ctrl_AllocMacro(__VA_ARGS__, WithFlags, NoFlags)(__VA_ARGS__)
#endif //DOXYGEN

#define Ctrl_AllocMacro(_1, _2, _3, AllocType, ...) Ctrl_Alloc##AllocType
#define Ctrl_AllocWithFlags(p_ctrl, tile, flags)    Ctrl_AllocInner(p_ctrl, (HitTile *)&tile, flags)
#define Ctrl_AllocNoFlags(p_ctrl, tile)             Ctrl_AllocInner(p_ctrl, (HitTile *)&tile, 0)
void Ctrl_AllocInner(Ctrl *p_ctrl, HitTile *p_tile, int flags);

/**
 * @brief Create a texture object asociated to tile \p tile and ctrl \p ctrl.
 *
 * \p tile must have memory already allocated on the device using a suitable alignment for the texture options.
 * This is always a synchronous call.
 *
 * @note Currently only CUDA, HIP and OpenCL GPU backends supports this.
 * @note OpenCL GPU support requires support for creating a 2D image from a buffer.
 * 		This is part of the 2.x standard and an extension in other opencl versions.
 * @note Some descriptor options may have additional requirements depending on the backend used, refer to CUDA, HIP and OpenCL documentations.
 * @hideinitializer
 *
 * @param ctrl pointer to ctrl to allocate memory with.
 * @param tile the tile to allocate memory to.
 * @param tex_desc texture desctriptor.
 */
#define Ctrl_CreateTex(p_ctrl, tile, tex_desc) Ctrl_CreateTexInner(p_ctrl, (HitTile *)&tile, tex_desc)
void Ctrl_CreateTexInner(Ctrl *p_ctrl, HitTile *p_tile, Ctrl_TexDesc tex_desc);

/**
 * Create a tile.
 *
 * Create and initialize the shape and base type of a new multidimensional array tile.
 * This is always a synchronous call.
 *
 * @hideinitializer
 *
 * @param type type of data inside the tile.
 * @param shape hit_shape used to create the tile.
 *
 * @returns HitTile_type created
 *
 * @pre \p type must have been declared as a type with @see Ctrl_NewType
 */
#define Ctrl_Domain(type, shape)  Ctrl_Domain2(type, shape);
#define Ctrl_Domain2(type, shape) Ctrl_Domain_##type(shape);
void Ctrl_DomainInner(HitTile *p_tile);

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
 * @param flags memory flags for the allocation. This parameter is optional and by default is 0.
 * 		Valid values include:
 * 			CTRL_MEM_ALLOC_HOST: Allocate memory only on host.
 * 			CTRL_MEM_ALLOC_DEV: Allocate memory only on device.
 * 			CTRL_MEM_PINNED: Allocate pinned memory if possible.
 * 			CTRL_MEM_NOPINNED: Allocate non pinned memory.
 * 			CTRL_MEM_ALIGNED: Try to allocate aligned memory on the device. Currently only works for CUDA, HIP and OpenCL 2D.
 * 		Flags CTRL_MEM_ALLOC_HOST and CTRL_MEM_ALLOC_DEV are mutually exclusive.
 * 		If none of them or both are specified the default behaviour (allocating memory on host and device) will apply.
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
#define Ctrl_DomainAlloc(...) Ctrl_DomainAllocMacro(__VA_ARGS__, WithFlags, NoFlags)(__VA_ARGS__)
#endif //DOXYGEN

#define Ctrl_DomainAllocMacro(_1, _2, _3, _4, AllocType, ...) Ctrl_DomainAlloc##AllocType
#define Ctrl_DomainAllocWithFlags(ctrl, type, shape, flags)   Ctrl_DomainAlloc_##type(ctrl, shape, flags)
#define Ctrl_DomainAllocNoFlags(ctrl, type, shape)            Ctrl_DomainAlloc_##type(ctrl, shape, 0)

/**
 * Create a tile subselection.
 *
 * Initialize a hierarchical subselection using tile coordinates.
 * This is always a synchronous call.
 *
 * @note Subselections of tiles with padding on the device (such as those allocated with CTRL_MEM_ALIGNED) are not currently supported
 * @note Subselections of tiles attached to one or more ctrls may not be attached to a new ctrl
 *
 * @hideinitializer
 *
 * @param type Type of data inside the tiles.
 * @param p_parent Parent tile to make a subselection from.
 * @param flags Subselection flags.
 *
 * @returns HitTile_type, the subselection from p_tile.
 *
 * @pre \p type must have been declared as a type with @see Ctrl_NewType
 */
#define Ctrl_Select(type, p_parent, shape, flags)  Ctrl_Select2(type, p_parent, shape, flags);
#define Ctrl_Select2(type, p_parent, shape, flags) Ctrl_Select_##type(((HitTile *)(&p_parent)), shape, flags);
void Ctrl_SelectInner(HitTile *p_tile, int flags);

/**
 * Free multiple tiles.
 * This is always a synchronous call.
 *
 * @hideinitializer
 *
 * @param ctrl pointer to ctrl that has the tiles attached. If NULL is passed tiles are freed from any ctrls they are attached to.
 * @param ... tiles to free.
 *
 * @pre tiles have to be allocated and attached to \p ctrl.
 * @see Ctrl_Alloc
 */
#define Ctrl_Free(ctrl, ...)            Ctrl_FreeN(ctrl, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)
#define Ctrl_FreeN(ctrl, n_tiles, ...)  Ctrl_FreeN2(ctrl, n_tiles, __VA_ARGS__)
#define Ctrl_FreeN2(ctrl, n_tiles, ...) Ctrl_OP_##n_tiles(Free, ctrl, __VA_ARGS__)
void Ctrl_FreeInner(Ctrl *p_ctrl, HitTile *p_tile);

/**
 * Move multiple tiles from host memory to device.
 * This call is always asynchronous.
 *
 * @hideinitializer
 *
 * @param ctrl pointer to ctrl that has the tiles attached.
 * @param ... tiles to be moved.
 *
 * @pre tiles have to be allocated and attached to \p ctrl.
 * @see Ctrl_Alloc
 */
#define Ctrl_MoveTo(ctrl, ...)            Ctrl_MoveToN(ctrl, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)
#define Ctrl_MoveToN(ctrl, n_tiles, ...)  Ctrl_MoveToN2(ctrl, n_tiles, __VA_ARGS__)
#define Ctrl_MoveToN2(ctrl, n_tiles, ...) Ctrl_OP_##n_tiles(MoveTo, ctrl, __VA_ARGS__)
void Ctrl_MoveToInner(Ctrl *p_ctrl, HitTile *p_tile);

/**
 * Move multiple tiles from device memory to host.
 * This call is always asynchronous.
 *
 * @hideinitializer
 *
 * @param ctrl pointer to ctrl that has the tiles attached.
 * @param ... tiles to be moved.
 *
 * @pre tiles have to be allocated and attached to \p ctrl.
 * @see Ctrl_Alloc
 */
#define Ctrl_MoveFrom(ctrl, ...)            Ctrl_MoveFromN(ctrl, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)
#define Ctrl_MoveFromN(ctrl, n_tiles, ...)  Ctrl_MoveFromN2(ctrl, n_tiles, __VA_ARGS__)
#define Ctrl_MoveFromN2(ctrl, n_tiles, ...) Ctrl_OP_##n_tiles(MoveFrom, ctrl, __VA_ARGS__)
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
#define Ctrl_WaitTile(ctrl, ...)            Ctrl_WaitTileN(ctrl, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)
#define Ctrl_WaitTileN(ctrl, n_tiles, ...)  Ctrl_WaitTileN2(ctrl, n_tiles, __VA_ARGS__)
#define Ctrl_WaitTileN2(ctrl, n_tiles, ...) Ctrl_OP_##n_tiles(WaitTile, ctrl, __VA_ARGS__)
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
 * Set the policy for ctrls to use.
 * For this function to be effective it must be called before \e __Ctrl_block__.
 * If this function is not called, the policy is determined by environment variable \e CTRL_POLICY_MODE.
 * If the environment variable is not set and this function is not called, the policy used will be \e CTRL_POLICY_ASYNC.
 *
 * @param policy Policy to use.
 *
 * @pre This function must be called before calling __ctrl_block__
 */
void Ctrl_SetPolicy(Ctrl_Policy policy);

/**
 * @brief Cleanup function for \e __ctrl_block__.
 *
 * Destroys all ctrls and the host queue. Must be called at the end of a ctrl block
 */
void Ctrl_EndBlock();

/**
 * @brief Get \e PCtrl with id \p id
 *
 * @param id id of the ctrl returned
 * @return PCtrl pointer to the ctrl
 *
 * @pre \p id must be a valid ctrl id: 0 <= \p id < Ctrl_GetNCtrls()
 */
PCtrl Ctrl_Get(int id);

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

/**
 * @return int Returns the total number of ctrls in the \e __ctrl_block__
 */
int Ctrl_GetNCtrls();

/**
 * Get information of the device asociated with \p p_ctrl.
 * @param p_ctrl ctrl to get the info from.
 *
 * @return Ctrl_Info struct containing information of the device asociated with \p p_ctrl.
 */
Ctrl_Info Ctrl_GetInfo(Ctrl *p_ctrl);

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
 * Push \p task host queue to execute and wait for it if policy is CTRL_POLICY_SYNC.
 *
 * @param task Host task to be launched.
 *
 * @see Ctrl_HostTask
 */
void Ctrl_LaunchHostTask(Ctrl_Task task);

/**
 * Set affinity of threads and send them to therir tasks
 *
 * @returns 0 if master 1 otherwise
 */
int Ctrl_Thread_Init();

/**
 * Parse device selection configuration file and create all ctrls specified in it.
 *
 * @param file path to config file.
 * @note This function can only be called after Ctrl_Init and before Ctrl_Finalize
 */
void Ctrl_ParseConfig(const char *file);

/**
 * Return the weights for all active processes found in the device selection configuration file
 * @note This function can only be called after ParseConfig
 */
HitWeights Ctrl_ConfigWeights();

/**
 * Function to choose the best implementation for a given kernel.
 *
 * @param type Type of the ctrl to launch the kernel.
 * @param avail_impls list of avaiable implementations.
 * @param  n_impl Number of avaiable implementations.
 * @return int Implementation chosen.
 */
int Ctrl_Dev(Ctrl_Type type, int *avail_impls, int n_impl);

/**
 * Enqueue wait for seq events from all ctrls in \p p_queue if policy is sync.
 *
 * @param p_queue queue to wait for the events
 */
void Ctrl_SyncWait(Ctrl_TaskQueue *p_queue);

/**
 * If \p p_tile_impl is valid, enqueue wait for appropiate events for a MoveTo operation in \p p_queue.
 *
 * @param p_tile_impl tile implementation to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_MoveToWait(Ctrl_Tile_Impl *p_tile_impl, Ctrl_TaskQueue *p_queue);

/**
 * If \p p_tile_impl is valid, enqueue wait for appropiate events for a MoveFrom operation in \p p_queue.
 *
 * @param p_tile_impl tile implementation to wait for
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_MoveFromWait(Ctrl_Tile_Impl *p_tile_impl, Ctrl_TaskQueue *p_queue);

/**
 * If \p p_tile_impl is valid, enqueue wait for appropiate events for a HostTask operation in \p p_queue.
 *
 * @param p_tile_impl tile implementation to wait for
 * @param rol rol of the tile in the host task operation
 * @param p_queue queue to enqueue the events on
 */
void Ctrl_HostTaskWait(Ctrl_Tile_Impl *p_tile_impl, char rol, Ctrl_TaskQueue *p_queue);

/**
 * Free the host part of a hit tile.
 *
 * @param p_tile tile to be freed.
 */
void Ctrl_FreeHostInner(HitTile *p_tile);

/**
 * Pin calling thread to host numa node (specified via ctrl device selection file) using hwloc.
 */
void Ctrl_PinToHostNuma();

// clang-format off
#define Ctrl_OP_1( op, ctrl, tile )       Ctrl_##op##Inner(ctrl, ((HitTile *)&(tile)));
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
// clang-format on

#ifdef __cplusplus
}
#endif

///@endcond
#endif // CTRL_FPGA_KERNEL_FILE

#endif /* _CTRL_CORE_H_ */
