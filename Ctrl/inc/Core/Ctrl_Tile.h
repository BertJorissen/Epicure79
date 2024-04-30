#ifndef _CTRL_CORE_TILE_H_
#define _CTRL_CORE_TILE_H_
///@cond INTERNAL
/**
 * @file Ctrl_Tile.h
 * @author Trasgo Group
 * @brief Tile struct and memory states.
 * @version 4.0
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

#ifdef _CTRL_ARCH_CPU_
#include "Architectures/Cpu/Ctrl_Cpu_Tile.h"
#endif // _CTRL_ARCH_CPU_

#ifdef _CTRL_ARCH_CUDA_
#include "Architectures/Cuda/Ctrl_Cuda_Tile.h"
#endif // _CTRL_ARCH_CUDA_

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
		#endif // _CTRL_ARCH_CUDA_

		#ifdef _CTRL_ARCH_CUDA_
		Ctrl_Cuda_Tile *p_cuda;
		#endif // _CTRL_ARCH_CUDA_

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