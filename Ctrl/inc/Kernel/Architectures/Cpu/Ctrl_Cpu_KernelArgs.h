#ifndef _CTRL_KERNEL_CPU_ARGS_H_
#define _CTRL_KERNEL_CPU_ARGS_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_KernelArgs.h
 * @brief Macros to cast Ctrl Cpu tiles to KHitTiles for their use in kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Core/Ctrl_Tile.h"

/**
 * Macro used to convert tiles into \e KHitTiles for kernel execution.
 * @hideinitializer
 *
 * @param name name of the tile parameter.
 *
 * @see KHitTile
 */
#define CTRL_KERNEL_CPU_KTILE_DEVICE_DATA(name)                                                           \
	case CTRL_TYPE_CPU:                                                                                   \
		k_##name##_void.data = (((Ctrl_Tile *)name->ext)->p_impls[p_ctrl->id].tile.p_cpu->p_device_data); \
		break;

///@endcond
#endif // _CTRL_KERNEL_CPU_ARGS_H_
