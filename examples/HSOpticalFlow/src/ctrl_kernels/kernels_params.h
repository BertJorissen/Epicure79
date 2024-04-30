/**
 * @file kernel_params.h
 * @author Trasgo Group
 * @brief HSOpticalFlow: List of parameters for kernels
 * @version 5.0
 * @date 2023-07-04
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
 * @copyright Copyright (c) 2007-2023, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
 */

/* DECLARE THE COMMON PARAMETERS FOR ALL KERNEL VERSIONS */
#include "Ctrl.h"
Ctrl_NewType(float);

#define add_params         3, IN, HitTile_float, op1, IN, HitTile_float, op2, OUT, HitTile_float, sum
#define solver_params      8, IN, HitTile_float, du0, IN, HitTile_float, dv0, IN, HitTile_float, Ix, IN, HitTile_float, Iy, IN, HitTile_float, Iz, INVAL, float, alpha, OUT, HitTile_float, du1, OUT, HitTile_float, dv1
#define warp_params        4, IN, HitTile_float, src, IN, HitTile_float, u, IN, HitTile_float, v, OUT, HitTile_float, out
#define upscale_params     3, IN, HitTile_float, src, INVAL, float, scale, OUT, HitTile_float, out
#define downscale_params   2, IN, HitTile_float, src, OUT, HitTile_float, out
#define derivatives_params 5, IN, HitTile_float, I0, IN, HitTile_float, I1, OUT, HitTile_float, Ix, OUT, HitTile_float, Iy, OUT, HitTile_float, Iz
#define zero_params        1, OUT, HitTile_float, tile
#define norm_params        5, IN, HitTile_float, u, IN, HitTile_float, v, INVAL, double *, p_sum, INVAL, double *, p_res, INVAL, int, frame