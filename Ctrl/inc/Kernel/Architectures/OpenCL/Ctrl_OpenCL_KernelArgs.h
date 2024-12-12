#ifndef _CTRL_KERNEL_OPENCL_ARGS_H_
#define _CTRL_KERNEL_OPENCL_ARGS_H_
///@cond INTERNAL
/**
 * @file Ctrl_OpenCL_KernelArgs.h
 * @brief Macros to cast Ctrl OpenCL GPU tiles to KHitTiles for their use in kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Core/Ctrl_Tile.h"
#include "Kernel/Ctrl_ImplType.h"

/**
 * Macro used to convert tiles into \e KHitTiles for kernel execution.
 * @hideinitializer
 *
 * @param name name of the tile parameter.
 *
 * @see KHitTile
 */
#define CTRL_KERNEL_OPENCL_KTILE_DEVICE_DATA(name)                                                                         \
	case CTRL_TYPE_OPENCL_GPU: {                                                                                           \
		Ctrl_OpenCL_Tile *p_tile_data = (Ctrl_OpenCL_Tile *)(((Ctrl_Tile *)name->ext)->p_impls[p_ctrl->id].tile.p_opencl); \
		k_##name##_void.data          = (void *)(&(p_tile_data->device_data));                                             \
		k_##name##_void.ext.ocl.tex   = p_tile_data->texture;                                                              \
		k_##name##_void.ext.ocl.smp   = p_tile_data->sampler;                                                              \
		break;                                                                                                             \
	}

///@endcond
#endif // _CTRL_KERNEL_OPENCL_ARGS_H_
