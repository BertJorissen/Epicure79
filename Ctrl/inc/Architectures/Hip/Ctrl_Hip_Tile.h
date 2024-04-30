#ifndef _CTRL_HIP_TILE_H_
#define _CTRL_HIP_TILE_H_
///@cond INTERNAL
/**
 * @file Ctrl_Hip_Tile.h
 * @author Trasgo Group
 * @brief Ctrl tile implentation for HIP devices.
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

#include "Architectures/Hip/Ctrl_Hip.h"
#include <hip/hip_runtime_api.h>
#include <stdbool.h>

/**
 * \brief Metadata from a tile associated to a CPU ctrl
 */
typedef struct Ctrl_Hip_Tile {
	struct Ctrl_Hip           *p_ctrl;                      /**< Pointer to the ctrl to which this tile is associated */
	struct Ctrl_Hip_Tile_List *p_tile_elem;                 /**< Node of the linked list containing all tiles of the ctrl to which this tile is associated */
	hipStream_t                stream;                      /**< Stream to handle memory transfers associated with this tile */
	void                      *p_device_data;               /**< Pointer to device image of this tile */
	hipEvent_t                 kernel_last_read_event;      /**< Event to sincronyze tasks, represents last write operation on device*/
	hipEvent_t                 kernel_last_write_event;     /**< Event to sincronyze tasks, represents last read operation on device*/
	hipEvent_t                 offloading_last_read_event;  /**< Event to sincronyze tasks, represents last DTH comunication*/
	hipEvent_t                 offloading_last_write_event; /**< Event to sincronyze tasks, represents last HTD comunication*/
	hipEvent_t                 host_last_read_event;        /**< Event to sincronyze tasks, represents last write operation on host*/
	hipEvent_t                 host_last_write_event;       /**< Event to sincronyze tasks, represents last read operation on host*/
	int                        host_status;                 /**< Information about the status of the host memory of this tile (unallocated, invalid or valid) */
	int                        device_status;               /**< Information about the status of the device memory of this tile (unallocated, invalid or valid) */
	bool                       is_pinned;                   /**< Flag to check if host image of this tile is pinned memory */
	bool                       is_initialized;              /**< Flag to check if tile has been initialized */
	struct Ctrl_Hip_Tile      *p_parent_ext;                /**< Pointer to parent's ext field in hierarchical subselections */
} Ctrl_Hip_Tile;

/**
 * \brief List af all CPU tiles associated to a ctrl
 */
typedef struct Ctrl_Hip_Tile_List {
	Ctrl_Hip_Tile             *p_tile_ext;
	struct Ctrl_Hip_Tile_List *p_prev;
	struct Ctrl_Hip_Tile_List *p_next;
} Ctrl_Hip_Tile_List;
///@endcond

#endif //_CTRL_HIP_TILE_H_
