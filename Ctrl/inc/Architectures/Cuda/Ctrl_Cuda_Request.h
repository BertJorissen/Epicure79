#ifndef _CTRL_CUDA_REQUEST_H_
#define _CTRL_CUDA_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cuda_Request.h
 * @author Trasgo Group
 * @brief Specific CUDA implementation for a request.
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

#include <cuda_runtime_api.h>

#ifdef _CTRL_CUBLAS_
#include <cublas_v2.h>
#endif // _CTRL_CUBLAS_

#ifdef _CTRL_MAGMA_
#include "magma_lapack.h"
#include "magma_v2.h"
#endif //_CTRL_MAGMA_

/**
 * \brief CUDA specific info needed to execute a kernel.
 */
typedef struct Ctrl_Cuda_Request {
	#ifdef _CTRL_CUBLAS_
	cublasHandle_t *p_cublas_handle; /**< CUBLAS handle for CUDALIB_CUBLAS type kernels */
	#endif // _CTRL_CUBLAS_

	#ifdef _CTRL_MAGMA_
	magma_queue_t *p_magma_queue; /**< MAGMA queue for CUDALIB_MAGMA type kernels */
	#endif // _CTRL_MAGMA_

	cudaStream_t *p_stream; /**< CUDA stream to launch the kernels to */
} Ctrl_Cuda_Request;
///@endcond

#endif // _CTRL_CUDA_REQUEST_H_
