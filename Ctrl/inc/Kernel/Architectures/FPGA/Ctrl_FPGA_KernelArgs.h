#ifndef _CTRL_KERNEL_FPGA_ARGS_H_
#define _CTRL_KERNEL_FPGA_ARGS_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_KernelArgs.h
 * @author Gabriel Rodriguez-Canal
 * @brief Macros to cast Ctrl FPGA tiles to KHitTiles for their use in kernels.
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
