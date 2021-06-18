/**
 * @file Ctrl_FPGA_Tile.h
 * @author Gabriel Rodriguez-Canal
 * @brief Ctrl tile implentation for FPGA devices.
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
#ifndef _CTRL_FPGA_TILE_H_
#define _CTRL_FPGA_TILE_H_

#include <stdbool.h>

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

#include "Architectures/FPGA/Ctrl_FPGA.h"

///@cond INTERNAL
/**
 * \brief Metadata from a tile associated to a FPGA ctrl
 */
typedef struct Ctrl_FPGA_Tile {
    struct Ctrl_FPGA                *p_ctrl;                    /**< Pointer to the ctrl to which this tile is associated */

    struct Ctrl_FPGA_Tile_List      *p_tile_elem;               /**< Node of the linked list containing all tiles of the ctrl to which this tile is associated */

    cl_command_queue			    queue;                      /**< OpenCL command queue used for memory transfers related to this tile */
    
    cl_mem                          device_data;                /**< OpenCL buffer for device image of this tile */

    cl_event                        kernel_last_read_event;     /**< Event to sincronyze tasks, represents last write operation on device*/
    cl_event                        kernel_last_write_event;    /**< Event to sincronyze tasks, represents last read operation on device*/

    cl_event                        offloading_last_read_event; /**< Event to sincronyze tasks, represents last DTH comunication*/
    cl_event                        offloading_last_write_event;/**< Event to sincronyze tasks, represents last HTD comunication*/

    cl_event                        host_last_read_event;       /**< Event to sincronyze tasks, represents last write operation on host*/
    cl_event                        host_last_write_event;      /**< Event to sincronyze tasks, represents last read operation on host*/

    int                             last_update;                /**< Information about where the updated data of this tile is located (host, device or both) */

	int								mem_flags;      	    	/**< Flags to choose allocation method */
	bool							is_initialized;     		/**< Flag to check if tile has been initialized */
} Ctrl_FPGA_Tile;

/**
 * \brief List af all FPGA tiles associated to a ctrl
 */
typedef struct Ctrl_FPGA_Tile_List {
	Ctrl_FPGA_Tile                *p_tile_ext;
    struct Ctrl_FPGA_Tile_List    *p_prev;
	struct Ctrl_FPGA_Tile_List    *p_next;
} Ctrl_FPGA_Tile_List;

///@endcond
#endif // _CTRL_FPGA_TILE_H_
