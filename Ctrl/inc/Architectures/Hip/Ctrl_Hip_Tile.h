#ifndef _CTRL_HIP_TILE_H_
#define _CTRL_HIP_TILE_H_
///@cond INTERNAL
/**
 * @file Ctrl_Hip_Tile.h
 * @brief Ctrl tile implentation for HIP devices.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <hip/hip_runtime_api.h>
#include <stdbool.h>

#include "Core/Ctrl_TaskQueue.h"

/**
 * \brief Metadata from a tile associated to a HIP ctrl
 */
typedef struct Ctrl_Hip_Tile {
	struct Ctrl_Hip       *p_ctrl;                       /**< Pointer to the ctrl to which this tile is associated */
	struct Ctrl_Tile_List *p_tile_elem;                  /**< Node of the linked list containing all tiles of the ctrl to which this tile is associated */
	void                  *p_device_data;                /**< Pointer to device image of this tile */
	size_t                 pitch;                        /**< Pitch of the device image in bytes. Useful when using texture memory. */
	hipTextureObject_t     texture;                      /**< For texture memory allocations. */
	Ctrl_GenericEvent      host_last_kernel_read_event;  /**< Host event to sincronyze tasks, represents last write operation on device*/
	Ctrl_GenericEvent      host_last_kernel_write_event; /**< Host event to sincronyze tasks, represents last read operation on device*/
	Ctrl_GenericEvent      host_last_dth_event;          /**< Host event to sincronyze tasks, represents last DTH comunication*/
	Ctrl_GenericEvent      host_last_htd_event;          /**< Host event to sincronyze tasks, represents last HTD comunication*/
	Ctrl_GenericEvent      dev_last_kernel_read_event;   /**< Driver event to sincronyze tasks, represents last write operation on device*/
	Ctrl_GenericEvent      dev_last_kernel_write_event;  /**< Driver event to sincronyze tasks, represents last read operation on device*/
	Ctrl_GenericEvent      dev_last_dth_event;           /**< Driver event to sincronyze tasks, represents last DTH comunication*/
	Ctrl_GenericEvent      dev_last_htd_event;           /**< Driver event to sincronyze tasks, represents last HTD comunication*/
} Ctrl_Hip_Tile;

///@endcond
#endif //_CTRL_HIP_TILE_H_
