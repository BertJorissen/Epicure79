#ifndef _CTRL_CORE_TILE_H_
#define _CTRL_CORE_TILE_H_
///@cond INTERNAL
/**
 * @file Ctrl_Tile.h
 * @brief Tile struct and memory states.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifdef _CTRL_ARCH_CPU_
#include "Architectures/Cpu/Ctrl_Cpu_Tile.h"
#endif // _CTRL_ARCH_CPU_

#ifdef _CTRL_ARCH_CUDA_
#include "Architectures/Cuda/Ctrl_Cuda_Tile.h"
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_HIP_
#include "Architectures/Hip/Ctrl_Hip_Tile.h"
#endif // _CTRL_ARCH_HIP_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include "Architectures/OpenCL/Ctrl_OpenCL_Tile.h"
#endif // _CTRL_ARCH_OPENCL_GPU_

#ifdef _CTRL_ARCH_FPGA_
#include "Architectures/FPGA/Ctrl_FPGA_Tile.h"
#endif // _CTRL_ARCH_FPGA_

#include "Core/Ctrl_Type.h"

typedef struct Ctrl_Tile_Impl {
	Ctrl_Type type;
	int       device_status;
	union {
		#ifdef _CTRL_ARCH_CPU_
		Ctrl_Cpu_Tile *p_cpu;
		#endif // _CTRL_ARCH_CPU_

		#ifdef _CTRL_ARCH_CUDA_
		Ctrl_Cuda_Tile *p_cuda;
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_HIP_
		Ctrl_Hip_Tile *p_hip;
		#endif // _CTRL_ARCH_HIP_

		#ifdef _CTRL_ARCH_OPENCL_GPU_
		Ctrl_OpenCL_Tile *p_opencl;
		#endif // _CTRL_ARCH_OPENCL_GPU_

		#ifdef _CTRL_ARCH_FPGA_
		Ctrl_FPGA_Tile *p_fpga;
		#endif // _CTRL_ARCH_FPGA_
	} tile;
} Ctrl_Tile_Impl;

// TODO @sergioalo pinned should probably use a different enum type
typedef struct Ctrl_Tile {
	Ctrl_GenericEvent last_host_read_event;  /**< Event to sincronyze tasks, represents last write operation on host*/
	Ctrl_GenericEvent last_host_write_event; /**< Event to sincronyze tasks, represents last read operation on host*/
	int               host_status;           /**< Information about the status of the host memory of this tile (unallocated, invalid or valid) */
	int               valid_impls;           /**< Number of valid entris in p_impls list */
	Ctrl_Type         pinned;                /**< Type of pinning of the host image, to know how to free it. CTRL_TYPE_NULL-> not pinned */
	void             *p_pin_queue;           /**< OpenCL queue used to map/unmap host image of this tile to try to make it pinned (only used when host mem is pinned via OpenCL) */
	void             *p_pinned_data;         /**< OpenCL buffer to map host image of this tile to try to make it pinned (only used when host mem is pinned via OpenCL) */
	Ctrl_Tile_Impl   *p_impls;               /**< list of arch specific tiles, of size n_ctrls */
} Ctrl_Tile;

/**
 * \brief List af all tiles associated to a ctrl
 */
typedef struct Ctrl_Tile_List {
	Ctrl_Tile             *p_tile_ext;
	struct Ctrl_Tile_List *p_prev;
	struct Ctrl_Tile_List *p_next;
} Ctrl_Tile_List;

#define CTRL_TILE_UNALLOC 0
#define CTRL_TILE_INVALID 1
#define CTRL_TILE_VALID   2

///@endcond
#endif // _CTRL_CORE_TILE_H_