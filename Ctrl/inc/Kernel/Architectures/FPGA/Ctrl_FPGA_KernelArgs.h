#ifndef _CTRL_KERNEL_FPGA_ARGS_H_
#define _CTRL_KERNEL_FPGA_ARGS_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_KernelArgs.h
 * @brief Macros to cast Ctrl FPGA tiles to KHitTiles for their use in kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Architectures/FPGA/Ctrl_FPGA_Tile.h"
#include "Kernel/Ctrl_ImplType.h"

/**
 * Macro used to convert tiles into \e KHitTiles for kernel execution.
 * @hideinitializer
 *
 * @param name name of the tile parameter.
 *
 * @see KHitTile
 */
#define CTRL_KERNEL_FPGA_KTILE_DEVICE_DATA(name)                                                                     \
	case CTRL_TYPE_FPGA: {                                                                                           \
		Ctrl_FPGA_Tile *p_tile_data = (Ctrl_FPGA_Tile *)(((Ctrl_Tile *)name->ext)->p_impls[p_ctrl->id].tile.p_fpga); \
		k_##name##_void.data        = (void *)(&(p_tile_data->device_data));                                         \
		break;                                                                                                       \
	}

///@endcond
#endif // _CTRL_KERNEL_FPGA_ARGS_H_
