/**
 * @file Ctrl_FPGA_Tile.h
 * @brief Ctrl tile implentation for FPGA devices.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */
#ifndef _CTRL_FPGA_TILE_H_
#define _CTRL_FPGA_TILE_H_

#include <stdbool.h>

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include "Core/Ctrl_TaskQueue.h"

///@cond INTERNAL
/**
 * \brief Metadata from a tile associated to a FPGA ctrl
 */
typedef struct Ctrl_FPGA_Tile {
	struct Ctrl_FPGA      *p_ctrl;                       /**< Pointer to the ctrl to which this tile is associated */
	struct Ctrl_Tile_List *p_tile_elem;                  /**< Node of the linked list containing all tiles of the ctrl to which this tile is associated */
	cl_mem                 device_data;                  /**< OpenCL buffer for device image of this tile */
	Ctrl_GenericEvent      host_last_kernel_read_event;  /**< Host event to sincronyze tasks, represents last write operation on device*/
	Ctrl_GenericEvent      host_last_kernel_write_event; /**< Host event to sincronyze tasks, represents last read operation on device*/
	Ctrl_GenericEvent      host_last_dth_event;          /**< Host event to sincronyze tasks, represents last DTH comunication*/
	Ctrl_GenericEvent      host_last_htd_event;          /**< Host event to sincronyze tasks, represents last HTD comunication*/
	Ctrl_GenericEvent      dev_last_kernel_read_event;   /**< Driver event to sincronyze tasks, represents last write operation on device*/
	Ctrl_GenericEvent      dev_last_kernel_write_event;  /**< Driver event to sincronyze tasks, represents last read operation on device*/
	Ctrl_GenericEvent      dev_last_dth_event;           /**< Driver event to sincronyze tasks, represents last DTH comunication*/
	Ctrl_GenericEvent      dev_last_htd_event;           /**< Driver event to sincronyze tasks, represents last HTD comunication*/
} Ctrl_FPGA_Tile;
///@endcond
#endif // _CTRL_FPGA_TILE_H_
