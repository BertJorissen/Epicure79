/**
 * @file kernel_protos.h
 * @author Trasgo Group
 * @brief HSOpticalFlow: Prototypes and characterizations for all kernels used in this program
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
#include "kernels_params.h"

/* Define kernel prototypes */

CTRL_KERNEL_PROTO(Solve, 2, CUDA, DEFAULT, OPENCLGPU, DEFAULT, solver_params);
CTRL_KERNEL_PROTO(Warp, 2, CUDA, DEFAULT, OPENCLGPU, DEFAULT, warp_params);
CTRL_KERNEL_PROTO(Upscale, 2, CUDA, DEFAULT, OPENCLGPU, DEFAULT, upscale_params);
CTRL_KERNEL_PROTO(Downscale, 2, CUDA, DEFAULT, OPENCLGPU, DEFAULT, downscale_params);
CTRL_KERNEL_PROTO(ComputeDerivatives, 2, CUDA, DEFAULT, OPENCLGPU, DEFAULT, derivatives_params);
CTRL_KERNEL_PROTO(Add, 1, GENERIC, DEFAULT, add_params);
CTRL_KERNEL_PROTO(Zero, 1, GENERIC, DEFAULT, zero_params);
CTRL_KERNEL_PROTO(Norm, 1, CPU, DEFAULT, norm_params);

/* Define characterizarion for each kernel */
CTRL_KERNEL_CHAR(Solve, MANUAL, SOLVECHAR_1, SOLVECHAR_0);
CTRL_KERNEL_CHAR(Warp, MANUAL, WARPCHAR_1, WARPCHAR_0);
CTRL_KERNEL_CHAR(ComputeDerivatives, MANUAL, DERIVCHAR_1, DERIVCHAR_0);
CTRL_KERNEL_CHAR(Upscale, MANUAL, SCALECHAR_1, SCALECHAR_0);
CTRL_KERNEL_CHAR(Downscale, MANUAL, SCALECHAR_1, SCALECHAR_0);
CTRL_KERNEL_CHAR(Add, MANUAL, 8, 32);
// TODO this kernels are created/modified by us, what char to use
CTRL_KERNEL_CHAR(Zero, MANUAL, 8, 32);
CTRL_KERNEL_CHAR(Norm, MANUAL, 0);