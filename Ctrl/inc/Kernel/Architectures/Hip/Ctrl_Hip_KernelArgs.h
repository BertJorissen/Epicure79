#ifndef _CTRL_KERNEL_HIP_ARGS_H_
#define _CTRL_KERNEL_HIP_ARGS_H_
///@cond INTERNAL
/**
 * @file Ctrl_Hip_KernelArgs.h
 * @brief Macros to cast Ctrl HIP tiles to KHitTiles for their use in kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Core/Ctrl_Tile.h"
#include <hip/hip_runtime_api.h>

/**
 * Macro used to convert tiles into \e KHitTiles for kernel execution.
 * @hideinitializer
 *
 * @param name name of the tile parameter.
 *
 * @see KHitTile
 */
#define CTRL_KERNEL_HIP_KTILE_DEVICE_DATA(name)                                                                  \
	case CTRL_TYPE_HIP:                                                                                          \
		k_##name##_void.data        = (((Ctrl_Tile *)name->ext)->p_impls[p_ctrl->id].tile.p_hip->p_device_data); \
		k_##name##_void.ext.hip.tex = (((Ctrl_Tile *)name->ext)->p_impls[p_ctrl->id].tile.p_hip->texture);       \
		break;

///@endcond
#endif // _CTRL_KERNEL_HIP_ARGS_H_
