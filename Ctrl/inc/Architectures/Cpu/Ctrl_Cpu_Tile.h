#ifndef _CTRL_CPU_TILE_H_
#define _CTRL_CPU_TILE_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_Tile.h
 * @brief Ctrl tile implentation for Cpu devices.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Core/Ctrl_TaskQueue.h"

/**
 * \brief Metadata from a tile associated to a CPU ctrl
 */
typedef struct Ctrl_Cpu_Tile {
	struct Ctrl_Cpu       *p_ctrl;                  /**< Pointer to the ctrl to which this tile is associated */
	struct Ctrl_Tile_List *p_tile_elem;             /**< Node of the linked list containing all tiles of the ctrl to which this tile is associated */
	void                  *p_device_data;           /**< Pointer to device image of this tile */
	Ctrl_GenericEvent      last_kernel_read_event;  /**< Event to sincronyze tasks, represents last write operation on device*/
	Ctrl_GenericEvent      last_kernel_write_event; /**< Event to sincronyze tasks, represents last read operation on device*/
	Ctrl_GenericEvent      last_dth_event;          /**< Event to sincronyze tasks, represents last DTH comunication*/
	Ctrl_GenericEvent      last_htd_event;          /**< Event to sincronyze tasks, represents last HTD comunication*/
} Ctrl_Cpu_Tile;
///@endcond
#endif //_CTRL_CPU_TILE_H_
